/*!
* ofxTriangle by kikko.fr
* -> C++ Triangle warper by Piyush Kumar
* -> Point inside polygon by Theo
*/

#ifndef OFXTRIANGLE_H_INCLUDED
#define OFXTRIANGLE_H_INCLUDED

//#include "ofMain.h"
//#include "ofxOpenCv.h"
#include "del_interface.hpp"
#include "../OpenCV.h"

using namespace tpp;
using namespace std;

typedef struct
{
    Point a;
    Point b;
    Point c;

    float area;

} ofxTriangleData;

class ofxTriangle
{
    public :

        ofxTriangle(){}
        ~ofxTriangle(){}

        // Triangulate a openCV blob
		void triangulate(vBlob &cvblob, int resolution = 50/*, int rdmPoints = 0*/);

        // Triangulate a vector of Point
        void addRdmPoint(vector<Delaunay::Point> * v);

		Point getTriangleCenter(cv::Point *tr);
        bool isPointInsidePolygon(Point *polygon,int N, Point p);

  //      void draw();
  //      void draw(float x, float y);
        void clear();


        Delaunay* delobject;

        int nTriangles;
        vector <ofxTriangleData> triangles;

    private :

        vBlob* blob;

};

#endif // OFXTRIANGLE_H_INCLUDED
