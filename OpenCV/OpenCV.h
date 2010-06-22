//vinjn's wrapper for OpenCV

#pragma once

#pragma warning( disable: 4244 )
#pragma warning( disable: 4996 )
#pragma warning( disable: 4819 )

#include <cv.h>
#include <highgui.h>
#include <cvaux.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <vector>

using namespace cv;
using std::vector;

//CV_FOURCC('P','I','M','1')    = MPEG-1 codec
//CV_FOURCC('M','J','P','G')    = motion-jpeg codec (does not work well)
//CV_FOURCC('M', 'P', '4', '2') = MPEG-4.2 codec
//CV_FOURCC('D', 'I', 'V', '3') = MPEG-4.3 codec
//CV_FOURCC('D', 'I', 'V', 'X') = MPEG-4 codec
//CV_FOURCC('U', '2', '6', '3') = H263 codec
//CV_FOURCC('I', '2', '6', '3') = H263I codec
//CV_FOURCC('F', 'L', 'V', '1') = FLV1 codec


template<class T> class Image
{
private:
	IplImage* imgp;
public:
	Image(IplImage* img=0) {imgp=img;}
	~Image(){imgp=0;}
	void operator=(IplImage* img) {imgp=img;}
	inline T* operator[](const int rowIndx) {
		return ((T *)(imgp->imageData + rowIndx*imgp->widthStep));}
};

typedef struct{
	unsigned char b,g,r;
} RgbPixel;

typedef struct{
	float b,g,r;
} RgbPixelFloat;

typedef Image<RgbPixel>       RgbImage;
typedef Image<RgbPixelFloat>  RgbImageFloat;
typedef Image<unsigned char>  BwImage;
typedef Image<float>          BwImageFloat;

/*
For a single-channel byte image:
IplImage* img=cvCreateImage(cvSize(640,480),IPL_DEPTH_8U,1);
BwImage imgA(img);
imgA[i][j] = 111;
For a multi-channel byte image:
IplImage* img=cvCreateImage(cvSize(640,480),IPL_DEPTH_8U,3);
RgbImage  imgA(img);
imgA[i][j].b = 111;
imgA[i][j].g = 111;
imgA[i][j].r = 111;
For a multi-channel float image:
IplImage* img=cvCreateImage(cvSize(640,480),IPL_DEPTH_32F,3);
RgbImageFloat imgA(img);
imgA[i][j].b = 111;
imgA[i][j].g = 111;
imgA[i][j].r = 111;
*/
void vCopyImageTo(CvArr* small_image, IplImage* big_image, const CvRect& region);

void vDrawText(IplImage* img, int x,int y,char* str, CvScalar clr=CV_RGB(255,255,255));

//在title窗口中，显示w * h个小图片(input)
void vShowManyImages(char* title, CvImage& input, int w, int h);

#define show_image(img_name) \
	cvNamedWindow(#img_name);\
	cvShowImage(#img_name, img_name); 

#define show_image2(img_name) \
	cvNamedWindow(#img_name, 0);\
	cvShowImage(#img_name, img_name); 

#define show_mat(img_name) \
	cv::namedWindow(#img_name, 0);\
	cv::imshow(#img_name, img_name); 

//mask is 8bit，掩板图片，mask中像素的值 > thresh，则img对应位置为原色，否则为0
//img is 24bit
void feature_out(IplImage* img, IplImage* mask, int thresh);

char* get_time(bool full_length = true);

typedef vector<IplImage*> image_array_t;

struct vBlob
{
	vBlob(Rect rc, Point ct, float ar = 0, float ang = 0, bool hole = false):box(rc),center(ct),area(ar), angle(ang), isHole(hole){}
	Rect box;
	Point center;
	float angle;
	float area;
	vector<Point> pts;
	bool isHole;
};

inline CvScalar random_color()
{
	static CvRNG   rng = cvRNG((unsigned)-1);
	int icolor = cvRandInt(&rng);
	return CV_RGB(icolor&255, (icolor>>8)&255, (icolor>>16)&255);
}

///////////////////////////////////////////////////////////////////////////////////////////

//For connected components:
#define CVCONTOUR_APPROX_LEVEL  2   // Approx.threshold - the bigger it is, the simpler is the boundary 

// This cleans up the forground segmentation mask derived from calls to cvBackCodeBookDiff
//
// mask			Is a grayscale (8 bit depth) "raw" mask image which will be cleaned up
//
// OPTIONAL PARAMETERS:
// poly1_hull0	If set, approximate connected component by (DEFAULT) polygon, or else convex hull (false)
// areaScale 	Area = image (width*height)*areaScale.  If contour area < this, delete that contour (DEFAULT: 0.1)
//

void vFindBlobs(IplImage *mask, vector<vBlob>& blobs,
				int morph_itr = 1,	 bool poly1_hull0=true, float areaScale = 0.1);
void vFindBlobs(IplImage *mask, 
				int morph_itr = 1,	 bool poly1_hull0=true, float areaScale = 0.1);//draw blobs only

#define vDrawRect(image, rc, clr) cvDrawRect(image, cvPoint(rc.x,rc.y), cvPoint(rc.x+rc.width,rc.y+rc.height), clr)


// parameters:
//  silh - input video frame
//  dst - resultant motion picture
//  args - optional parameters
vector<vBlob>  update_mhi( IplImage* silh, IplImage* dst);
/*

#include "OpenCV.h"

VideoInput video_input;

int main(int argc, char** argv )
{	
if (video_input.init(argc,argv))
{
while (true)
{
IplImage* raw = video_input.get_frame(); 
if (!raw)
break;
cvFlip(raw, 0, 1);

show_image(raw);
int key = cvWaitKey(1);
if (key == VK_ESCAPE)
break;
}
}

return 0;
}
*/

#define vGrayScale(clr, gray) cvCvtColor(clr, gray, CV_BGR2GRAY) 
#define vColorFul(gray, clr) cvCvtColor(gray, clr , CV_GRAY2BGR) 
#define vThresh(gray, thresh) cvThreshold( gray, gray, thresh, 255, CV_THRESH_BINARY )
#define vThreshInv(gray, thresh) cvThreshold( gray, gray, thresh, 255, CV_THRESH_BINARY_INV )
#define vAutoThresh(gray, max_value) cvAdaptiveThreshold(gray, gray, max_value)
#define vOpen(img, times) cvMorphologyEx( img, img, NULL, NULL, CV_MOP_OPEN, times );//去除白色小区域
#define vClose(img, times) cvMorphologyEx( img, img, NULL, NULL, CV_MOP_CLOSE, times );//去除黑色小区域
#define vDilate(img, times) cvMorphologyEx( img, img, NULL, NULL, CV_MOP_DILATE, times );
#define vErode(img, times) cvMorphologyEx( img, img, NULL, NULL, CV_MOP_ERODE, times );

#define vFullScreen(win_name) \
	cvSetWindowProperty(win_name, CV_WND_PROP_FULLSCREEN, 1);

struct VideoInput
{
	int _fps;

	enum e_InputType
	{
		From_Image = 0,
		From_Video,
		From_Camera,
		From_Count,
	}_InputType;

	CvCapture* _capture;

	IplImage* _frame;
	int _cam_idx;
	Size _size;
	Size _half_size;
	int _channel;
	int _codec;

	VideoInput();

	bool init(int cam_idx);
	bool init(char* video_file);
	bool init(int argc, char** argv);

	void wait(int t);

	IplImage* get_frame();

	void _post_init();

	~VideoInput();
};


struct HaarDetection
{
	CvHaarClassifierCascade* cascade;
	CvMemStorage* storage; 
	CvRect object_rect;

	HaarDetection();
	~HaarDetection();
	bool init(char* cascade_name);

	void detect_object(IN IplImage* img, OUT vector<CvRect>& regions);
};


struct IBackGround
{
	CvBGStatModel* bg_model;
	int thresh;
	IBackGround(){
		bg_model = NULL;
		thresh = 100;
	}

	virtual void init(IplImage* initial, void* param = NULL) = 0;

	virtual void update(IplImage* image, int mode = 0){
		cvUpdateBGStatModel( image, bg_model );
	}
	void setThresh(int th){
		thresh = th;
	}
	virtual IplImage* getForeground(){
		return bg_model->foreground;
	}

	virtual IplImage* getBackground(){
		return bg_model->background;
	}
	~IBackGround(){
		if (bg_model)
			cvReleaseBGStatModel(&bg_model);
	}
};


struct vBackFGDStat: public IBackGround
{
	void init(IplImage* initial, void* param = NULL);
};

struct vBackGaussian: public IBackGround
{
	void init(IplImage* initial, void* param = NULL);
};

#define DETECT_BOTH 0
#define DETECT_DARK 1
#define DETECT_BRIGHT 2

struct vBackGrayDiff: public IBackGround
{
	Ptr<IplImage> grayFrame;
	Ptr<IplImage> grayBg;
	Ptr<IplImage> grayDiff ;

	void init(IplImage* initial, void* param = NULL);

	///mode: 0-> 检测明与暗 1->检测黑暗 2->检测明亮
	void update(IplImage* image, int mode = DETECT_BOTH);

	IplImage* getForeground(){
		return grayDiff;
	}
	IplImage* getBackground(){
		return grayBg;
	}
};

struct vBackColorDiff: public IBackGround
{
	Ptr<IplImage> colorBg;
	Ptr<IplImage> grayDiff;

	int w;
	int h;
	int step; 
	int step2;

	void init(IplImage* initial, void* param = NULL){
		cv::Size size = cvGetSize(initial);
		colorBg.release();
		colorBg = cvCloneImage(initial);

		grayDiff.release();
		grayDiff = cvCreateImage(size, 8, 1);

		w = colorBg->width;
		h = colorBg->height;
		step = colorBg->widthStep; 
		step2 = grayDiff->widthStep;
	}

	void update(IplImage* image, int mode = 0){

		cvZero(grayDiff);
		for(int y=0;y<h;y++) 
			for(int x=0;x<w;x++) 
			{
				uchar* pixel = &((uchar*)(image->imageData + step*y))[x*3];
				uchar* pixel2 = &((uchar*)(colorBg->imageData + step*y))[x*3];
				uchar* p = &((uchar*)(grayDiff->imageData + step2*y))[x];

				uchar r = pixel[0] - pixel2[0];
				uchar g = pixel[1] - pixel2[1];
				uchar b = pixel[2] - pixel2[2];

				if ((r*r+g*g+b*b) > thresh*thresh)						
					*p = 255;
				else
					*p = 0;
			}
	}
	IplImage* getForeground(){
		return grayDiff;
	}
	IplImage* getBackground(){
		return colorBg;
	}
};

struct vBackCodeBook
{
	CvBGCodeBookModel* model;
	Ptr<IplImage> yuvImage;
	Ptr<IplImage> ImaskCodeBook;
	Ptr<IplImage> ImaskCodeBookCC;
	bool isLearning;

	vBackCodeBook();

	~vBackCodeBook();

	void init(CvSize size);

	bool learn(IplImage* image);
	void finish_learn();

	//!!!!never release the returned image
	IplImage* getForeground(IplImage* image);

	void release();
};

void vHighPass(IplImage* src, IplImage* dst, int blurLevel = 10, int noiseLevel = 3);

void vPerspectiveTransform(const CvArr* src, CvArr* dst, cv::Point srcQuad[4], cv::Point dstQuad[4]);

CvFGDStatModelParams cvFGDStatModelParams();

void vPolyLine(IplImage* dst, vector<Point>& pts, CvScalar clr=CV_RGB(255,255,255), int thick = 1);

CvScalar vDefaultColor(int idx);
void vGetPerspectiveMatrix(CvMat*& warp_matrix, cv::Point2f xsrcQuad[4], cv::Point2f xdstQuad[4]);



void vDrawDelaunay( CvSubdiv2D* subdiv,IplImage* src,IplImage * dst, bool drawLine = true );
void vDrawVoroni( CvSubdiv2D * subdiv, IplImage * src, IplImage * dst, bool drawLine = true );



struct Subdiv
{
	Subdiv(int w, int h);

	void insert(float x, float y);
	void clear();
	void build();

	void drawDelaunay( IplImage* src,IplImage * dst, bool drawLine = true );
	void drawVoroni( IplImage* src,IplImage * dst, bool drawLine = true );

	CvRect rect;
	CvMemStorage* storage;
	CvSubdiv2D* subdiv;
};

void on_default(int );

//亮度变换，nPercent为正时变亮，负则变暗 
int ContrastAdjust(const IplImage* srcImg,
				   IplImage* dstImg,
				   float nPercent);

//对比度变换，brightness小于1降低对比度，大于1增强对比度
int BrightnessAdjust(const IplImage* srcImg,
					 IplImage* dstImg,
					 float brightness);

void convertRGBtoHSV(const IplImage *imageRGB, IplImage *imageHSV);
void convertHSVtoRGB(const IplImage *imageHSV, IplImage *imageRGB);

#define cv_try_begin() try{

#define cv_try_end() 	}\
	catch (cv::Exception& ex)\
{\
	printf("[cv::Exception] %s\n", ex.what());\
}


#define vAddWeighted(src, alpha, dst) cvAddWeighted(src, alpha, dst, 1-alpha, 0, dst);