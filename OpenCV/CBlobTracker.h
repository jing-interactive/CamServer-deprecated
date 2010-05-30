#ifndef __TOUCHLIB_CBLOBTRACKER__
#define __TOUCHLIB_CBLOBTRACKER__

#include <vector>
#include <math.h>
#include <vector2d.h>
#include <rect2d.h>

#include <Image.h>
#include <touchlib_platform.h>
#include "ITouchListener.h"
#include "TouchData.h"


#define HISTORY_FRAMES	10

// a finger is a blob with an ID and prediction info
// CFinger should be able to accept a CBlob as a constructor

namespace touchlib
{

	class TOUCHLIB_CORE_EXPORT CBlob
	{
    public:
		CBlob()
		{
			area = 0.0f;
			weight = 0.0f;
		}
		vector2df center;
		float area;
		rect2df box;
		float angle;
		float weight;

		int tagID;		// for fiducal markers. 0 = regular touchpoint.
	};


	struct BlobEquiv
	{
		unsigned char from;
		unsigned char to;
		int x;
		int y;
	};

    //////////////

	class TOUCHLIB_CORE_EXPORT CFinger : public CBlob
	{
	public:
		CFinger() 
		{
			ID = -1;
			markedForDeletion = false;
			framesLeft = 0;
		}

		CFinger(const CBlob &b)
		{
			ID = -1;
			center = b.center;
			area = b.area;
			box = b.box;
			angle = b.angle;
		}

		int getLowestError()
		{
			int best=-1;
			float best_v=99999.0f;

			for(unsigned int i=0; i<error.size(); i++)
			{
				if(error[i] < best_v)
				{
					best = i;
					best_v = error[i];
				}
			}

			return best;
		};

		TouchData getTouchData()
		{
			TouchData data;
			data.ID = ID;
			data.X = center.X;
			data.Y = center.Y;

			data.angle = angle;
			data.width = box.getWidth();
			data.height = box.getHeight();

			data.dX = delta.X;
			data.dY = delta.Y;

			data.tagID = tagID;

			return data;
		};

		int ID;

		vector2df delta;
		vector2df predictedPos;

		vector2df displacement;

		float deltaArea;

		bool markedForDeletion;
		int framesLeft;

		std::vector<float> error;
		std::vector<int> closest;		// ID's of the closest points, sorted..
	};



	class TOUCHLIB_EXPORT CBlobTracker 
	{
	public:
                static const float DEFAULT_MINIMUM_DISPLACEMENT_THRESHOLD;

		CBlobTracker();
		void findBlobs(BwImage &img, BwImage &label_img);
		void findBlobs_contour(BwImage &img, BwImage &label_img);
		void ProcessResults();
		void gatherEvents();

		virtual bool getFingerInfo(int ID, TouchData *data);
		virtual void registerListener(ITouchListener *listener);
		void setup(int r_dist, int r_min_dim, int r_max_dim, int g_frames, float minimumDisplacementThreshold);

	private:
		void doTouchEvent(TouchData data);
		void doUpdateEvent(TouchData data);
		void doUntouchEvent(TouchData data);
		inline void recordEquiv(unsigned char from, unsigned char to, int x, int y);
		inline void permute2(int k);
		inline void permute(int k);
		inline bool checkValid(int start);
		inline bool checkValidNew(int start);
		void labelFill(BwImage &label_img, int x, int y, unsigned char from, unsigned char to);

		int level;
		CFinger *findFinger(int hist, int id);
		float getError(CFinger &old, CFinger &cur);


#ifdef WIN32
#pragma warning( disable : 4251 )  // http://www.unknownroad.com/rtfm/VisualStudio/warningC4251.html
#endif

		int currentID;
		int extraIDs;
		int numcheck;

		int reject_distance_threshold;
		int reject_min_dimension;
		int reject_max_dimension;
		int ghost_frames;

		float minimumDisplacementThreshold;

		std::vector<std::vector<int> > matrix;
		std::vector<int> ids;
		std::vector<std::vector<CFinger> > history;
		std::vector<CBlob> blobList;
		std::vector<BlobEquiv> equivalences;
		std::vector<CFinger> current;
		std::vector<ITouchListener *> listenerList;

#ifdef WIN32
#pragma warning( default : 4251 )
#endif

	};

}

#endif // __TOUCHLIB_CBLOBTRACKER__
