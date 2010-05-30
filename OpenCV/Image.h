#ifndef __TOUCHLIB_IMAGE__
#define __TOUCHLIB_IMAGE__

#include <cv.h>
#include <touchlib_platform.h>

namespace touchlib
{

	// This class serves as a wrapper to OpenCV's image class
	// to provide a simple interface to pixels and height/width info.

	template<class T> 
	class TOUCHLIB_CORE_EXPORT Image  
	{  
		public:  
			Image(IplImage* img=0) {imgp=img;}  
			~Image(){imgp=0;}  

			int getHeight() 
			{ 
				if(imgp) 
					return imgp->height; 
				else
					return 0;
			};
			int getWidth() 
			{ 
				if(imgp)
					return imgp->width; 
				else
					return 0;
			};

			void operator=(IplImage* img) {imgp=img;}  
			inline T* operator[](const int rowIndx) 
			{    
				return ((T *)(imgp->imageData + rowIndx*imgp->widthStep)); 
			};

			IplImage* imgp;  
	};

	typedef struct
	{  
		unsigned char b,g,r;
	} RgbPixel;

	typedef struct
	{  
		float b,g,r;
	} RgbPixelFloat;

	typedef Image<RgbPixel>       RgbImage;
	typedef Image<RgbPixelFloat>  RgbImageFloat;
	typedef Image<unsigned char>  BwImage;
	typedef Image<float>          BwImageFloat;




}

#endif  // __TOUCHLIB_IMAGE__
