// Todo: get capture working @ 640 x 480
// f


#include "CBlobTracker.h"

#include <map>

#define MIN_AREA 2.0
#define DETECT_THRESHOLD 1		// this is really handled by the Rectify filter
#define MIN_WEIGHT	255.0f
#define MAX_LABELS	1000

using namespace std;



// FIXME: findFinger should search back a few frames just in case 

using namespace touchlib;

const float CBlobTracker::DEFAULT_MINIMUM_DISPLACEMENT_THRESHOLD = 2.0f;

///////////////////////////////////////////////////
// blob detecting

CBlobTracker::CBlobTracker()
{
	currentID = 1;
	extraIDs = 0;

	// note: CTouchscreen will set it's own defaults (and also read from the config file)
	// so changing these here won't do much
	reject_distance_threshold = 250;
	reject_min_dimension = 2;
	reject_max_dimension = 250;

	minimumDisplacementThreshold = DEFAULT_MINIMUM_DISPLACEMENT_THRESHOLD;

	ghost_frames = 0;
}

void CBlobTracker::setup(int r_dist, int r_min_dim, int r_max_dim, int g_frames, float minimumDisplacementThreshold)
{
	reject_distance_threshold = r_dist;
	reject_min_dimension = r_min_dim;
	reject_max_dimension = r_max_dim;

	ghost_frames = g_frames;

	this->minimumDisplacementThreshold = minimumDisplacementThreshold;
}

// stolen from opencv - squares.c sample
// helper function:
// finds a cosine of angle between vectors
// from pt0->pt1 and from pt0->pt2 
double cosAngle( CvPoint* pt1, CvPoint* pt2, CvPoint* pt0 )
{
    double dx1 = pt1->x - pt0->x;
    double dy1 = pt1->y - pt0->y;
    double dx2 = pt2->x - pt0->x;
    double dy2 = pt2->y - pt0->y;
    return (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

// check out the number of children at each level some how to calculate a blob tag. 
// there can be a max of 9 squares at each level.. 
unsigned int getTag(CvSeq *curCont, int level=0)
{

	unsigned int num = 0;
	unsigned int sum = 0;

	if(level > 5)
		return 0;

	for( ; curCont != 0; curCont = curCont->h_next )	
	{
		num ++;

		if(curCont->v_next)
			sum += getTag(curCont->v_next, level+1);

		if(num > 9)
			return 9;

	}

	sum += num * (int)powf(10.0, level);

	return sum;
}


void CBlobTracker::findBlobs_contour(BwImage &img, BwImage &label_img)
{

	blobList.clear();
	CvMemStorage* storage = cvCreateMemStorage(0);
	CvSeq* cont = 0; 
	CvBox2D32f box;
	float halfx,halfy;
	CBlob blob;
	float temp;
	CvSeq *result;
    //CvSeq *squares = cvCreateSeq( 0, sizeof(CvSeq), sizeof(CvPoint), storage );

	double s, t;
	unsigned int i;

	bool isSquare = false;

	cvFindContours( img.imgp, storage, &cont, sizeof(CvContour), CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE );

	for( ; cont != 0; cont = cont->h_next )	{
		int count = cont->total; // This is number point in contour

		// First we check to see if this contour looks like a square.. 
		isSquare = false;
        result = cvApproxPoly( cont, sizeof(CvContour), storage,
            CV_POLY_APPROX_DP, cvContourPerimeter(cont)*0.02, 0 );

        if( result->total == 4 &&
            fabs(cvContourArea(result,CV_WHOLE_SEQ)) > 1000 &&
            cvCheckContourConvexity(result) )
        {
			s = 0;
            
            for( i = 0; i < 5; i++ )
            {
                // find minimum angle between joint
                // edges (maximum of cosine)
                if( i >= 2 )
                {
                    t = fabs(cosAngle(
                    (CvPoint*)cvGetSeqElem( result, i ),
                    (CvPoint*)cvGetSeqElem( result, i-2 ),
                    (CvPoint*)cvGetSeqElem( result, i-1 )));
                    s = s > t ? s : t;
                }
            }
            
            // if cosines of all angles are small
            // (all angles are ~90 degree) then this is a square.. 
            if( s < 0.5 )
			{
	

				box = cvMinAreaRect2(cont, storage);		
				blob.center.X = box.center.x;
				blob.center.Y = box.center.y;
				blob.angle = box.angle;

				halfx = box.size.width*0.5f;
				halfy = box.size.height*0.5f;
				blob.box.upperLeftCorner.set(box.center.x-halfx,box.center.y-halfy);
				blob.box.lowerRightCorner.set(box.center.x+halfx,box.center.y+halfy);

				blob.area = blob.box.getArea();

				// FIXME: it might be nice if we could get the actual weight.. 
				// It also might be nice to find the weighted center..

				blob.weight = 0;


				// use v_next.. 

				if(cont->v_next)
					blob.tagID = getTag(cont->v_next);

				printf("Square Detected %d\n", blob.tagID);

				blobList.push_back(blob);

				isSquare = true;
			}
		}

		// fallback, if it's a regular blob.
		if(!isSquare)
		{
			// Number point must be more than or equal to 6 (for cvFitEllipse_32f).    
			if( count >= 6)
			{
				// Fits ellipse to current contour.
				box = cvFitEllipse2(cont);	
			} else {
				box = cvMinAreaRect2(cont, storage);		
			}
			blob.center.X = box.center.x;
			blob.center.Y = box.center.y;
			blob.angle = box.angle;

			halfx = box.size.width*0.5f;
			halfy = box.size.height*0.5f;
			blob.box.upperLeftCorner.set(box.center.x-halfx,box.center.y-halfy);
			blob.box.lowerRightCorner.set(box.center.x+halfx,box.center.y+halfy);

			blob.area = blob.box.getArea();
			
			// FIXME: it might be nice if we could get the actual weight.. 
			// It also might be nice to
			blob.weight = 0;
			blob.tagID = 0;

			int h=(int)blob.box.getHeight(), w=(int)blob.box.getWidth();
			if(w >= reject_min_dimension && h >= reject_min_dimension && w < reject_max_dimension && h < reject_max_dimension)
				blobList.push_back(blob);
		}

	}		// end cont for loop

	cvReleaseMemStorage(&storage);

}

void CBlobTracker::findBlobs(BwImage &img, BwImage &label_img)
{
	// NOTE: assumes label img is black (0) at the start;
	
	int height = img.getHeight();
	int width = img.getWidth();

	if(height == 0 || width == 0)
		return;

	int x, y;

	int current_label = 0;

	//rect<int> labelBox[MAX_LABELS];

	// see http://homepages.inf.ed.ac.uk/rbf/CVonline/LOCAL_COPIES/OWENS/LECT3/node2.html
	blobList.clear();
	equivalences.clear();


	int pixels = 0;

	for(y=1; y<height; y++)
	{
		for(x=1; x<width; x++)
		{
			if(img[y][x] > DETECT_THRESHOLD)
			{
				pixels++;

				// too many brightspots, you need to calibrate your stuff..
				if(pixels > height * width / 4)
				{
					return;
				}

				unsigned char c;

				if(label_img[y-1][x-1] != 0)
				{
					c = label_img[y-1][x-1];
					label_img[y][x] = c;
					//labelBox[c].addPoint(x, y);
				}
				else if(label_img[y][x-1] == 0 && label_img[y-1][x] == 0)
				{
					current_label++;
					//if(current_label > MAX_LABELS)
					//{
						//printf("Error! too many labels\n");
						//return;
					//}
					label_img[y][x] = current_label;
					//labelBox[current_label].addPoint(x, y);

				}
				else if(((label_img[y][x-1] > 0) || (label_img[y-1][x] > 0)) 
					&& !(label_img[y][x-1] > 0 && label_img[y-1][x] > 0))
				{
					if(label_img[y][x-1] > 0) 
					{
						c = label_img[y][x-1];
						label_img[y][x] = c;
						//labelBox[c].addPoint(x, y);
					} else {
						c = label_img[y-1][x];
						label_img[y][x] = c;

						//labelBox[c].addPoint(x, y);
					}
				}

				else if(label_img[y][x-1] > 0 && label_img[y-1][x] > 0)
				{
					if(label_img[y][x-1] == label_img[y-1][x])
					{
						c = label_img[y][x-1];
						label_img[y][x] = c;
						//labelBox[c].addPoint(x,y);
					} 
					else 
					{
						recordEquiv(label_img[y][x-1], label_img[y-1][x], x-1, y);
						c = label_img[y-1][x];
						label_img[y][x] = c;
						//labelBox[c].addPoint(x,y);
					}
				}
			}
		}
	}

	unsigned int i;

	// merge down equivalences..
	std::vector<BlobEquiv>::iterator equiviter1;
	std::vector<BlobEquiv>::iterator equiviter2;

	for(equiviter1=equivalences.begin(); equiviter1 != equivalences.end(); equiviter1++)
	{
		for(equiviter2=equiviter1; equiviter2 != equivalences.end(); equiviter2++)
		{
			if((*equiviter1).from == (*equiviter2).to)
			{
				(*equiviter2).to = (*equiviter1).to;
			}
		}
	}

	//http://www.codeproject.com/gdi/QuickFill.asp

	//for(i=1; i<equivalences.size(); i++)
	//{
		//labelFill(label_img, equivalences[i].x, equivalences[i].y, equivalences[i].from, equivalences[i].to);
	//}

	//printf("Equivalences: %d", equivalences.size());

		for(x=1; x<width; x++)
		{
			for(y=1; y<height; y++)
			{
				if(label_img[y][x] != 0)
				{
					for(i=0; i<equivalences.size(); i++)
					{	

						if(label_img[y][x] == equivalences[i].from)
							label_img[y][x] = equivalences[i].to;
					}
				}
			}
		}

	/*
	for(i=1; i<equivalences.size(); i++)
	{	

		rect<int> box = labelBox[i];
		for(y=box.upperLeftCorner.Y; y<box.lowerRightCorner.Y; y++)
		{
			for(x=box.upperLeftCorner.X; x<box.lowerRightCorner.X; x++)
			{
				if(label_img[y][x] == equivalences[i].from)
					label_img[y][x] = equivalences[i].to;
			}
		}
	}
	*/

	std::map<int, CBlob> tempblobs;

	vector2df pos;
	// slow?

	for(y=1; y<height; y++)
	{
		for(x=1; x<width; x++)
		{

			unsigned char label = label_img[y][x];
			if( label != 0) 
			{
				pos.X = (float) x;
				tempblobs[label].box.addPoint(pos);
				tempblobs[label].center += pos * (float)img[y][x];
				tempblobs[label].weight += img[y][x];
				tempblobs[label].tagID = 0;
			}
		}
		pos.Y = (float) y;
	}

	std::map<int, touchlib::CBlob>::iterator iter;
	std::map<int, touchlib::CBlob>::iterator iter2;

	// calculate center, area, etc
	// add to real blobslist.

	for(iter = tempblobs.begin(); iter != tempblobs.end(); )
	{


		if(iter->second.weight < MIN_WEIGHT) 
		{
			iter2 = iter;
			iter++;
			tempblobs.erase(iter2);

		} else {
			iter->second.center = iter->second.center / iter->second.weight;
			iter->second.area = iter->second.box.getArea();

			blobList.push_back(iter->second);

			iter++;
		}
	}

	/*
	for(iter = tempblobs.begin(); iter != tempblobs.end(); iter++)
	{
		for(iter2 = tempblobs.begin(); iter2 != tempblobs.end(); iter2++)
		{
			if(iter != iter2 && iter->second.area > 0 && iter2->second.area > 0 && iter->second.box.isRectCollided(iter2->second.box))
			{
				rect2df overlaprect = iter2->second.box;
				overlaprect.clipAgainst(iter->second.box);
				float overlaparea = overlaprect.getArea();

				if(overlaparea / iter2->second.area > 0.5)
					iter2->second.area = 0.0;
			}
		}
	}
	*/
}


inline void CBlobTracker::recordEquiv(unsigned char from, unsigned char to, int x, int y)
{
	for(unsigned int i=0; i<equivalences.size(); i++)
	{
		if( (equivalences[i].from == from && equivalences[i].to == to) )
		{
			return;
		}
	}

	BlobEquiv equiv;
	equiv.from = from;
	equiv.to = to;
	equiv.x = x;
	equiv.y = y;

	equivalences.push_back(equiv);
}

void CBlobTracker::labelFill(BwImage &label_img, int x, int y, unsigned char from, unsigned char to)
{

	static int stack = 0;
	
	stack++;

	if(stack < 1000) 
	{
		if(label_img[y][x] == from)
		{
			label_img[y][x] = to;


			if(x > 0)
				labelFill(label_img, x-1, y, from, to);

			if(y > 0)
				labelFill(label_img, x, y-1, from, to);

			if(x < label_img.getWidth())
				labelFill(label_img, x+1, y, from, to);

			if(y < label_img.getHeight())
				labelFill(label_img, x, y+1, from, to);

		}
	} else {

	}

	stack--;
}


//////////////////////////////////////////
// blob tracking stuff below

void CBlobTracker::ProcessResults()
{
	// Fire off events, set id's..
	unsigned int i, j, k;

	history.push_back(current);

	if(history.size() > HISTORY_FRAMES) 
	{
		history.erase(history.begin());
	}

	current.clear();

	unsigned int numblobs = (unsigned int) blobList.size();

	for(i=0; i<numblobs; i++)
	{
		current.push_back(CFinger(blobList[i]));
	}

	vector<CFinger> *prev = &history[history.size()-1];

	const unsigned int cursize = (unsigned int) current.size();
	const unsigned int prevsize = (unsigned int) (*prev).size();

	// now figure out the 'error' for all the blobs in the current frame
	// error is defined as the distance between the current frame blobs and the blobs
	// in the last frame
	// potentially error could encompass things like deviation from the predicted
	// position, change in size, etc, but it's difficult to come up with a fair metric which 
	// includes more than one axis. Simply optimizing for the least change in distance
	// seems to work the best.

	for(i=0; i<cursize; i++)
	{
		current[i].error.clear();
		current[i].closest.clear();

		for(j=0; j<prevsize; j++)
		{
			float error = 0.0f;
			error = getError((*prev)[j], current[i]);
			current[i].error.push_back(error);
			current[i].closest.push_back(j);
		}
	}

	// sort so we can make a list of the closest blobs in the previous frame..
	for(i=0; i<cursize; i++)
	{
		// Bubble sort closest.
		for(j=0; j<prevsize; j++)
		{
			for(k=0; k<prevsize-1-j; k++)
			{
				// ugly as hell, I know.
				
				if(current[i].error[current[i].closest[k+1]] < current[i].error[current[i].closest[k]]) 
				{
				  int tmp = current[i].closest[k];			// swap
				  current[i].closest[k] = current[i].closest[k+1];
				  current[i].closest[k+1] = tmp;
				}
			}
		}
	}

	// Generate a matrix of all the possible choices
	// If we know there were four points last time and 6 points this time then 2 ID's will be -1.
	// then we will calculate the error and pick the matrix that has the lowest error
	// the more points the slower this will be.. fortunately we will be dealing with low numbers of points..
	// unfortunately the cpu time grows explosively since this is an NP Complete problem.
	// to remedy, we will assume that the chosen ID is going to be from one of the top 4 closest points.
	// this will eliminate possiblities that shouldn't lead to an optimal solution.

	ids.clear();

	//map<int, int> idmap;

	// collect id's.. 
	for(i=0; i<cursize; i++)
	{
		ids.push_back(-1);
	}

	extraIDs = cursize - prevsize;
	if(extraIDs < 0)
		extraIDs = 0;
	matrix.clear();

	// FIXME: we could scale numcheck depending on how many blobs there are
	// if we are tracking a lot of blobs, we could check less.. 
	

	if(cursize <= 4)
		numcheck = 4;
	else if(cursize <= 6)
		numcheck = 3;
	else if(cursize <= 10)
		numcheck = 2;
	else
		numcheck = 1;

	if(prevsize < numcheck)
		numcheck = prevsize;
	
	if(current.size() > 0)
		permute2(0);


	unsigned int num_results = matrix.size();

	//if(cursize > 0)
		//printf("matrix size: %d\n", num_results);

	// loop through all the potential ID configurations and find one with lowest error
	float best_error = 99999, error;
	int best_error_ndx = -1;

	for(j=0; j<num_results; j++)
	{
		error = 0;
		// get the error for each blob and sum
		for(i=0; i<cursize; i++)			
		{
			CFinger *f = 0;

			if(matrix[j][i] != -1) 
			{
				error += current[i].error[matrix[j][i]];
			}
		}

		if(error < best_error)
		{
			best_error = error;
			best_error_ndx = j;		
		}
	}

	// now that we know the optimal configuration, set the IDs and calculate some things..
	if(best_error_ndx != -1)
	{
		for(i=0; i<cursize; i++)
		{
			if(matrix[best_error_ndx][i] != -1)
				current[i].ID = (*prev)[matrix[best_error_ndx][i]].ID;
			else
				current[i].ID = -1;

			if(current[i].ID != -1)
			{
				CFinger *oldfinger = &(*prev)[matrix[best_error_ndx][i]];
				current[i].delta = (current[i].center - oldfinger->center);
				current[i].deltaArea = current[i].area - oldfinger->area;
				current[i].predictedPos = current[i].center + current[i].delta;
				current[i].displacement = oldfinger->displacement + current[i].delta;
			} else {
				current[i].delta = vector2df(0, 0);
				current[i].deltaArea = 0;
				current[i].predictedPos = current[i].center;
				current[i].displacement = vector2df(0.0f, 0.0f);
			}
		}

		//printf("Best index = %d\n", best_error_ndx);
	}


}

void CBlobTracker::gatherEvents()
{
	vector<CFinger> *prev = &history[history.size()-1];

	const unsigned int cursize = (unsigned int) current.size();
	const unsigned int prevsize = (unsigned int) (*prev).size();
	unsigned int i, j;

	// assign ID's for any blobs that are new this frame (ones that didn't get 
	// matched up with a blob from the previous frame).
	for(i=0; i<cursize; i++)
	{
		if(current[i].ID == -1)	
		{
			current[i].ID = currentID;

			currentID ++;
			if(currentID >= 65535)
				currentID = 0;

			doTouchEvent(current[i].getTouchData());
		} else {
			if (current[i].displacement.getLength() >= minimumDisplacementThreshold) {
				doUpdateEvent(current[i].getTouchData());
				current[i].displacement = vector2df(0.0f, 0.0f);
			}
		}
	}

	// if a blob disappeared this frame, send a finger up event
	for(i=0; i<prevsize; i++)		// for each one in the last frame, see if it still exists in the new frame.
	{
		bool found = false;
		for(j=0; j<cursize; j++)
		{
			if(current[j].ID == (*prev)[i].ID)
			{
				found = true;
				break;
			}
		}

		if(!found)
		{
			

			if(ghost_frames == 0)
			{
				doUntouchEvent((*prev)[i].getTouchData());
			} else if((*prev)[i].markedForDeletion)
			{
				(*prev)[i].framesLeft -= 1;
				if((*prev)[i].framesLeft <= 0)
					doUntouchEvent((*prev)[i].getTouchData());
				else
					current.push_back((*prev)[i]);	// keep it around until framesleft = 0
			} else
			{
				(*prev)[i].markedForDeletion = true;
				(*prev)[i].framesLeft = ghost_frames;
				current.push_back((*prev)[i]);	// keep it around until framesleft = 0
			}

		}
	}
}

inline void CBlobTracker::permute2(int start)
{  
  if (start == ids.size()) 
  {

		  //for( int i=0; i<start; i++)
		  //{
			//printf("%d, ", ids[i]);
		  //}
		  //printf("--------\n");
		  matrix.push_back(ids);

  }
  else 
  {
	  int numchecked=0;

	  for(int i=0; i<current[start].closest.size(); i++)
	  {
		if(current[start].error[current[start].closest[i]] > reject_distance_threshold)
			break;

		ids[start] = current[start].closest[i];
		if(checkValid(start))
		{
			permute2(start+1);
			numchecked++;

		}

		if(numchecked >= numcheck)
			break;
	  }

	  if(extraIDs > 0)
	  {
			ids[start] = -1;		// new ID
			if(checkValidNew(start))
			{
				permute2(start+1);
			}
	  }



  }
}

inline bool CBlobTracker::checkValidNew(int start)
{
	int newidcount = 0;

	newidcount ++;
	for(int i=0; i<start; i++)
	{
	  if(ids[i] == -1)
		  newidcount ++;
	}

	if(newidcount > extraIDs)		//extraIDs > 0 
	  return false;

	return true;
}

inline bool CBlobTracker::checkValid(int start)
{

	for(int i=0; i<start; i++)
	{
	  // check to see whether this ID exists already
	  if(ids[i] == ids[start])
		  return false;
	}

  return true;
}

// deprecated.. remove
inline void CBlobTracker::permute(int start)
{  
  if (start == ids.size()-1) 
  {
		//print(v, n);
		matrix.push_back(ids);
  }
  else 
  {
    for (unsigned int i = start; i < ids.size(); i++) 
	{
      int tmp = ids[i];
      
      ids[i] = ids[start];
      ids[start] = tmp;
      permute(start+1);
      ids[start] = ids[i];
      ids[i] = tmp;
    }
  }
}


// Meant to be queried by the finger listener
bool CBlobTracker::getFingerInfo(int ID, TouchData *data)
{
	for(unsigned int i=0; i<current.size(); i++)
	{
		if(current[i].ID == ID)	
		{
			*data = current[i].getTouchData();

			return true;
		}
	}
	return false;
}

CFinger *CBlobTracker::findFinger(int hist, int id)
{
	for(unsigned int i=0; i<history[hist].size(); i++)
	{
		if(history[hist][i].ID == id)
			return &(history[hist][i]);
	}

	return 0;
}


float CBlobTracker::getError(CFinger &old, CFinger &cur)
{
	//vector2df dev = cur.center - old.predictedPos;

	vector2df dev = cur.center - old.center;

	// FIXME: improve

	return (float) dev.getLength();

}

void CBlobTracker::registerListener(ITouchListener *listener)
{
	listenerList.push_back(listener);
}


void CBlobTracker::doTouchEvent(TouchData data)
{
	unsigned int i;
	for(i=0; i<listenerList.size(); i++)
	{
		listenerList[i]->fingerDown(data);
	}
}


void CBlobTracker::doUpdateEvent(TouchData data)
{
	unsigned int i;
	for(i=0; i<listenerList.size(); i++)
	{
		listenerList[i]->fingerUpdate(data);
	}
}

void CBlobTracker::doUntouchEvent(TouchData data)
{
	unsigned int i;
	for(i=0; i<listenerList.size(); i++)
	{
		listenerList[i]->fingerUp(data);
	}
}
