#include "OpenCV/OpenCV.h"
#include "ofxBox2d/ofxBox2d.h"
#include "ofxDelaunay/ofxDelaunay.h"

#define WIN_TITLE "Paper Act"

#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEY_UP(vk_code)   ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)

bool isPrevKeyDown[256];

bool isKeyToggle(int vk, bool* keyStatus = NULL);

bool isKeyToggle(int vk, bool* keyStatus){
	//
	if (KEY_DOWN((vk)) && !(isPrevKeyDown[vk])){
		(isPrevKeyDown[vk]) = true;
		if (keyStatus != NULL)
			*keyStatus =! *keyStatus;//key status needs update
		return true;
	}
	if (!KEY_DOWN((vk))){
		isPrevKeyDown[vk] = false;
	}
	return false;
}

bool showPoly = true;

vector	<ofxBox2dPolygon>	polygons;//	defalut box2d polgons
ofxBox2dCircle player;
ofxBox2dCircle obstacle;
ofxBox2d	physics;//	the box2d world
ofxDelaunay triangulator;
Delaunay delaunay;
bool isOnGround = false;
bool isFirstJump = false;

struct _player_ground_cb : public ofxBox2dContactListener
{
	void contactAdd(b2Vec2 p) 
	{
		isOnGround = true;
		isFirstJump = false;

		//b2Vec2 v = player.body->GetLinearVelocity();
		//if (v.x < 10) v.x = 0;
		//player.setVelocity(v.x, v.y);
	}

	void contactRemove(b2Vec2 p)
	{
		isOnGround = false;
		isFirstJump = true;
	}

}player_ground_cb;


Point getTriangleCenter(Point tr[3])
{
	float c_x = (tr[0].x + tr[1].x + tr[2].x) / 3;
	float c_y = (tr[0].y + tr[1].y + tr[2].y) / 3;

	return Point(c_x, c_y);
}

void vFillPoly(IplImage* img, const vector<Point>& pt_list, const Scalar& clr = Scalar(255,255,255))
{
	const Point* pts = &pt_list[0];
	const int npts = pt_list.size();
	cv::fillPoly(Mat(img), &pts, &npts, 1, clr);

}

bool isPointInsidePolygon(const vector<Point>& polygon, const Point& p)
{
	int polySides = polygon.size();
	bool  oddNodes=false;

	for (int i=0, j = polySides-1; i<polySides; j=i++) 
	{
		int ix = polygon[i].x;
		int iy = polygon[i].y;
		int jx = polygon[j].x;
		int jy = polygon[j].y;
		if (iy<p.y && jy>=p.y || jy<p.y && iy>=p.y)
		{
			if (ix+(p.y-iy)/(jy-iy)*(jx-ix)<p.x)
				oddNodes=!oddNodes;
		}
	}
	return oddNodes; 
}

bool isPointInsidePolygon2(const vector<Point>& polygon, const Point& p)
{
	int counter = 0;
	int i;
	double xinters;
	Point p1,p2;
	int N = polygon.size();

	p1 = polygon[0];

	for (i=1;i<=N;i++)
	{
		p2 = polygon[i % N];
		if (p.y > MIN(p1.y,p2.y)) {
			if (p.y <= MAX(p1.y,p2.y)) {
				if (p.x <= MAX(p1.x,p2.x)) {
					if (p1.y != p2.y) {
						xinters = (p.y-p1.y)*(p2.x-p1.x)/(p2.y-p1.y)+p1.x;
						if (p1.x == p2.x || p.x <= xinters)
							counter++;
					}
				}
			}
		}
		p1 = p2;
	}

	if (counter % 2 == 0)
		return false;
	else
		return true;
}


b2Vec2 tob2Vec2(const CvPoint& pt)
{
	return b2Vec2(pt.x, pt.y);
}

CvPoint toCvPoint(const b2Vec2& pt)
{
	return cvPoint(pt.x, pt.y);
}

char buffer[256];
int thresh = 93;

void init_physics()
{
	physics.init();
	physics.setGravity(0, 0.5);
	//	physics.createFloor();
	physics.checkBounds(true);
	physics.setFPS(30.0);
	physics.setContactListener(&player_ground_cb);

	player.setPhysics(0.3,0.1, 1);	
	player.setup(physics.getWorld(), 10, 10, 4);
	player.setPosition(320,0);

	//obstacle.setPhysics(0,0.1, 0.1);
	//obstacle.setup(physics.getWorld(), 10, 10, 10);
	//obstacle.setPosition(400,50); 

}

VideoInput video_input;

int main(int argc, char** argv )
{	
	if (!video_input.init(argc,argv))
		return -1;

	cvNamedWindow(WIN_TITLE);
	IplImage* raw = video_input.get_frame(); 
//	IplImage* raw = cvLoadImage("../media/map4.jpg");
	IplImage* frame = cvCloneImage(raw);
	IplImage* gray = cvCreateImage(cvGetSize(raw), 8, 1);	
	//IplImage* world_raw = cvCreateImage(cvGetSize(raw), 8, 3);	
	IplImage* world = cvCreateImage(cvGetSize(raw), 8, 3);	

	cvCreateTrackbar("thresh", WIN_TITLE, &thresh, 255, 0);
	init_physics();
	std::vector<vBlob> blobs; 

	int w = raw->width;
	int h = raw->height;
	delaunay.init(w,h);

	while (true)
	{
		raw = video_input.get_frame();
		if (!raw)
			break;
		cvFlip(raw, 0, 1);
		cvCopy(raw, frame);
		vGrayScale(frame, gray);
		vThresh(gray, thresh);
		cvErode(gray, gray);
		//	cvCopyImage(gray, world_raw);

		vFindBlobs(gray, blobs, 0, true, 0.001);

		for (int j=0;j<polygons.size();j++)
		{
			ofxBox2dPolygon& p = polygons[j];
			p.destroyShape();
		} 
		polygons.clear();

		for (int i=0;i<blobs.size();i++)
		{
			vBlob& b = blobs[i];
			if (!b.isHole)
				continue;

			//vFillPoly(world_raw, b.pts);
			//		 cvDrawContours(world_raw,b.pts, CV_CVX_WHITE,CV_CVX_WHITE,-1,CV_FILLED,8); //draw to src

			//		 vPolyPoint(frame, b.pts, CV_RGB(255,0,0), 2);
			//			 vPolyLine(raw, b.pts, CV_RGB(0,255,0));
			if(showPoly)
			{
				sprintf(buffer, "%d: #%d", i, b.pts.size()); 
				vDrawText(frame, b.center.x, b.center.y, buffer, CV_RGB(255,0,22));
			}

			triangulator.reset();
			delaunay.clear();
			for(int j=0; j < b.pts.size(); j++)
			{
				triangulator.addPoint(b.pts[j].x, b.pts[j].y);
				delaunay.insert(b.pts[j].x, b.pts[j].y);
			}
			triangulator.triangulate();
			delaunay.build();

			int count = triangulator.getNumTriangles();
			ITRIANGLE* tri = triangulator.getTriangles();
			XYZ* p = triangulator.getPoints();

			for (int j=0;j< count;j++)
			{
				ITRIANGLE& v = tri[j];
				Point pt[3];
				pt[0] = Point(p[v.p1].x, p[v.p1].y);
				pt[1] = Point(p[v.p2].x, p[v.p2].y);
				pt[2] = Point(p[v.p3].x, p[v.p3].y);

				if( isPointInsidePolygon2(b.pts, getTriangleCenter(pt) ) )
				{
					ofxBox2dPolygon poly;

					poly.addVertex(pt[2].x, pt[2].y);
					poly.addVertex(pt[1].x, pt[1].y);
					poly.addVertex(pt[0].x, pt[0].y);

					if(poly.validateShape()) 
					{
						if (showPoly)
						{
							cvLine(frame, pt[0], pt[1], CV_RGB(0,255,0), 1);
							cvLine(frame, pt[1], pt[2], CV_RGB(0,255,0), 1);
							cvLine(frame, pt[2], pt[0], CV_RGB(0,255,0), 1); 
						}
						poly.setPhysics(0, 0.1, 1);
						poly.setup(physics.world, 0.0f, 0.0f);
						polygons.push_back(poly);
					}
				}//isPointInsidePolygon2
			}//
		}//end blob

		//	player.

#define f 0.02

		b2Vec2 center = player.body->GetWorldCenter();
		if (KEY_DOWN(VK_LEFT)) player.addImpulseForce(b2Vec2(-0.5*f,-0), center);
		if (KEY_DOWN(VK_RIGHT)) player.addImpulseForce(b2Vec2(0.5*f,0), center);
		if (isKeyToggle(VK_UP))
		{
			if (isOnGround || isFirstJump)
			{
				if (isFirstJump) isFirstJump = false;
				player.addImpulseForce(b2Vec2(0,-25*f), center);
			}
		}
		if (isKeyToggle(VK_SPACE))
			showPoly = !showPoly;
		//if (KEY_DOWN(VK_DOWN)) player.addImpulseForce(b2Vec2(0,f), center);

		//int vx = 0;
		//int vy = 0;
		//if (KEY_DOWN(VK_LEFT)) vx = -1;
		//if (KEY_DOWN(VK_RIGHT)) vx = 1;
		//if (KEY_DOWN(VK_UP)) vy = -1;
		//if (KEY_DOWN(VK_DOWN)) vy = 1;

		//player_pos += Point2f(vx*f, vy*f); 

		physics.update();
		//for (int i=0;i<polygons.size();i++)
		//	polygons[i].update();

		cvCircle(frame, toCvPoint(player.getPosition()), player.getRadius(), CV_RGB(100,0,200),3);
		cvCircle(frame, toCvPoint(obstacle.getPosition()), obstacle.getRadius(), CV_RGB(100,0,200),3);

		//cvCopyImage(world_raw, world); 

		//cvCircle(frame, player_pos, player.getRadius(), CV_RGB(100,0,200),3);
		//cvCircle(world, player_pos, player.getRadius(), CV_RGB(122,122,122),3);

		cvShowImage(WIN_TITLE, frame); 
		//	show_image(world);
		int key = cvWaitKey(1);

		if (key == VK_ESCAPE)
			break;
	}
	//	::Sleep(20);

	return 0;
}