#include "VideoApp.h"
#include "AppConfig.h"
#include "UI.h"
#include "MiniTimer.h"

using namespace cv;

VideoApp theApp;//global

#ifdef WIN32
int CamServer_WindowCallback(HWND hwnd, UINT uMsg, WPARAM wparam, LPARAM lparam, int* was_processed)
{
    if (uMsg == WM_DESTROY)
    {
        if ((hwnd == (HWND)monitor_gui::handle && theApp.monitorVisible) || (hwnd == (HWND)param_gui::handle && !param_gui::is_changing_layout))
            theApp.app_running = false;
    }
    return 0;
}
#endif

VideoGrabThread::VideoGrabThread(VideoInput& input):MiniThread("grab"),input(input)
{
    fps = 0;
}

bool VideoGrabThread::isDirty()
{
    bool ret = mDirty;
    mDirty = false;
    return ret;
}

void VideoGrabThread::threadedFunction()
{
    MiniTimer timer(theConfig.log_file);
    int frame_counter = 0;
    int ms_counter = 0;	
    while(true)
    {
        if (!theApp.app_running)
            return;

        timer.resetStartTime();
        input.getFrame();
        mDirty = true; 
        if (input.mInputType == VideoInput::From_Video || input.mInputType == VideoInput::From_Image)
        {
            unsigned int elapse = timer.getTimeElapsedMS();
            if (elapse < 40)
                SLEEP(40 - elapse);
        }

        frame_counter++;
        if ((ms_counter += timer.getTimeElapsedMS()) > 1000)
        {
            fps = frame_counter;
            frame_counter = 0;
            ms_counter = 0;
        }

        timer.profileFunction("<<thread>>input.get_frame()");
    }
} 

VideoApp::VideoApp()
{
    monitorVisible = true; 
    selectedCorner = NULL;

    toResetBackground = false;

    app_running = true;
    input_inited = false;

    g_Fx = g_Fy = g_prevFx = g_prevFy = 0;
    channels = 3;
}

bool VideoApp::init(int argc, char** argv)
{
    if (theConfig.minim_window)
    {
#ifdef WIN32
        keybd_event(VK_LWIN, 0, 0, 0);
        keybd_event('D', 0, 0, 0);
        keybd_event(VK_LWIN, 0, KEYEVENTF_KEYUP, 0);
#endif
    }

    if (!mInput.init(argc,argv))
    {
        input_inited = true;
#ifdef WIN32
        system("pause");
#endif
        return false;
    }
    input_inited = true;
    grab_thread = new VideoGrabThread(mInput);
#ifdef _DEBUG
    grab_thread->startThread(true, true);//debug mode is noisy
#else
    grab_thread->startThread(true, false);
#endif

    //grab unrelated
    sender.setup( theConfig.CLIENT, theConfig.PORT );
    printf("[OSC] setup client as %s : %d\n", theConfig.CLIENT.c_str(), theConfig.PORT);

    //theConfig.auto_explosure = _input.getAutoExplosure();
    /*if (face_track)*/
#ifdef FACE_DETECTION_ENABLED
    if (!haar.init("../../data/haarcascade_frontalface_alt.xml"))
    {//try another folder
        haar.init("../data/haarcascade_frontalface_alt.xml");
    }
    haar.scale = 2;
#endif
    //grab related
    size = mInput.mSize;
    channels = mInput.mChannel;

    if (size.width < 400)
    {
        size = size*2;//hack: enlarge the size of camera with small resolution
    }
    total.create(cv::Size(size.width*1.5, size.height*1.5), CV_8UC3);
    total = CV_GRAY;

    float k = 0;
    subWindowWidth = size.width*0.75;
    subWindowHeight = size.height*0.75;
    windowWidth = subWindowWidth * 2;
    windowHeight = subWindowHeight * 2;
    half = cv::Size(subWindowWidth-k, subWindowHeight-k);

    cv::Size_<double> halfRoiSize(0.5, 0.5);
    mRoiMainWindows[0] = cv::Rect_<double>(Point2d(0  ,0),      halfRoiSize);
    mRoiMainWindows[1] = cv::Rect_<double>(Point2d(0.5,0),      halfRoiSize);
    mRoiMainWindows[2] = cv::Rect_<double>(Point2d(0  ,0.5),    halfRoiSize);
    mRoiMainWindows[3] = cv::Rect_<double>(Point2d(0.5, 0.5),   halfRoiSize);	

    dstQuad[0] = Point(0,0);
    dstQuad[1] = Point(subWindowWidth,0);
    dstQuad[2] = Point(subWindowWidth,subWindowHeight);
    dstQuad[3] = Point(0,subWindowHeight);

    if (!theConfig.load_from(CONFIG_FILE))
    {
        resetCorners();

    }
    warpMatrix = cv::getPerspectiveTransform(theConfig.cornersA, dstQuad);

    onParamFlip(theConfig.paramFlipX, theConfig.paramFlipY);

    half_raw.create(half, CV_8UC(channels));
    frame.create(half, CV_8UC(channels));
    black_frame.create(half, CV_8UC(channels));
    white_frame.create(half, CV_8UC(channels));
    prevBg.create(half, CV_8UC(channels));

    grayBuffer.create(half, CV_8UC(1));

    black_frame = CV_BLACK;
    white_frame = CV_WHITE;

    if (theConfig.delay_for_run > 0)					// if necessary
        SLEEP(theConfig.delay_for_run * 1000);	// sleep before the app shows up

    monitor_gui::show(true);
    param_gui::show(true);
    param_gui::init();

    onRefreshBack();

#ifdef WIN32
    cvSetPreprocessFuncWin32(CamServer_WindowCallback);
#endif
    return true;
}

void VideoApp::onParamFlip(int fx, int fy)
{ 
    g_Fx = fx;
    g_Fy = fy;

    if (theConfig.bg_mode == REAL_BG)
    {
        if (prevBg.empty())//if not initialized
            onRefreshBack();
        else
        {
            Mat temp = prevBg.clone();
            vFlip(temp, abs(fx-g_prevFx), abs(fy-g_prevFy));
            backModel->init(temp);
        }
    }
}

#define addFloatX(num) m.addFloatArg(num/_W)
#define addFloatY(num) m.addFloatArg(num/_H)

void VideoApp::send_custom_msg()
{
    const float _W = subWindowWidth;
    const float _H = subWindowHeight;

    //start
    {
        ofxOscMessage m;
        m.setAddress( "/start" );
        sender.sendMessage( m );
    }

    //face
    if (theConfig.face_track)
    {
        int nFaces = haar.blobs.size();
        for (int i=0;i<nFaces;i++)
        {
            vBlob& obj = haar.blobs[i];
            ofxOscMessage m;
            m.setAddress( "/face");
            m.addIntArg(-1);
            m.addStringArg("");
            addFloatX(obj.center.x);
            addFloatY(obj.center.y);
            addFloatX(obj.box.x);
            addFloatY(obj.box.y);
            addFloatX(obj.box.width);
            addFloatY(obj.box.height);
            sender.sendMessage( m );

            vDrawRect(frame, obj.box, vDefaultColor(i));
        }
    }

    //object
    vector<vTrackedBlob>* pTrackBlobs[] = {&blobTracker.trackedBlobs, &blobTracker.deadBlobs};
    for (int v=0;v<2;v++)
    {
        vector<vTrackedBlob>* pTb = pTrackBlobs[v];
        int nB = pTb->size();
        for (unsigned int i=0;i<nB;i++)
        {
            vTrackedBlob& obj = (*pTb)[i];

            //if (obj.status == statusStill)
            //	//do send still blobs out
            //	continue;
            // TODO: move the drawing parts to opengl
            Mat m = frame;
            if (obj.isHole)
                vPolyLine(m, obj.pts, cv::Scalar(0,0,0), 1);
            else
                vPolyLine(m, obj.pts, vDefaultColor(obj.id), 1);
            //	vDrawRect(frame, obj.box, CV_RGB(0,122,255));
            sprintf(g_buffer, "#%d", obj.id);
            vDrawText(m, obj.center.x, obj.center.y, g_buffer, vDefaultColor(obj.id));

            int id = obj.id;
            int cx = obj.rotBox.center.x;
            int cy = obj.rotBox.center.y;
            int w = obj.rotBox.size.width;
            int h = obj.rotBox.size.height;
            float angle = obj.angle;
            {
                ofxOscMessage m;
                if (obj.isHole)
                    m.setAddress( "/hole");
                else
                    m.setAddress( "/contour");

                m.addIntArg(id);                        //0
                m.addStringArg(obj.getStatusString());  //1
                addFloatX(cx);                          //2
                addFloatY(cy);                          //3
                addFloatX(w);                           //4
                addFloatY(h);                           //5
                m.addFloatArg(angle);                   //6
                int nPts = obj.pts.size();
                m.addIntArg(nPts);                      //7
                for (int k=0;k<nPts;k++)
                {
                    addFloatX(obj.pts[k].x);
                    addFloatY(obj.pts[k].y);
                }
                sender.sendMessage( m );
            }
        }
    }
    {
        ofxOscMessage m;
        m.setAddress( "/end" );
        sender.sendMessage( m );
    }	
}

void VideoApp::send_tuio_msg()
{
    const float _W = subWindowWidth;
    const float _H = subWindowHeight;

    ofxOscBundle bundle;

    ofxOscMessage alive;
    {
        alive.setAddress("/tuio/2Dcur");
        alive.addStringArg("alive");
    }

    ofxOscMessage fseq;
    {
        fseq.setAddress( "/tuio/2Dcur" );
        fseq.addStringArg( "fseq" );
        fseq.addIntArg(mInput.mFrameNum);
    }

    int nBlobs = blobTracker.trackedBlobs.size();
    for (int i=0;i < nBlobs; i++)
    {
        vTrackedBlob& blob = blobTracker.trackedBlobs[i];

        Mat ma = frame;
        if (blob.isHole)
            vPolyLine(ma, blob.pts, cv::Scalar(0,0,0), 1);
        else
            vPolyLine(ma, blob.pts, vDefaultColor(blob.id), 1);
        //	vDrawRect(frame, obj.box, CV_RGB(0,122,255));
        sprintf(g_buffer, "#%d", blob.id);
        vDrawText(ma, blob.center.x, blob.center.y, g_buffer, vDefaultColor(blob.id));

        //Set Message
        ofxOscMessage m;
        m.setAddress( "/tuio/2Dcur" );
        m.addStringArg("set");
        m.addIntArg(blob.id);				// id
        addFloatX(blob.center.x);	// x
        addFloatX(blob.center.y);	// y
        addFloatX(blob.velocity.x);			// dX
        addFloatX(blob.velocity.y);			// dY
        m.addFloatArg(0);		// maccel
        bundle.addMessage(m);
        alive.addIntArg(blob.id);				// add blob to list of ALL active IDs
    }

    bundle.addMessage(alive);
    bundle.addMessage(fseq);
    sender.sendBundle(bundle);
}

void VideoApp::onRefreshBack()
{
    toResetBackground = true;
}

void VideoApp::resetCorners()
{
    theConfig.cornersA[0] = cv::Point2f(0,              0);
    theConfig.cornersA[1] = cv::Point2f(subWindowWidth, 0);
    theConfig.cornersA[3] = cv::Point2f(0,              subWindowHeight);
    theConfig.cornersA[2] = cv::Point2f(subWindowWidth, subWindowHeight);
}
