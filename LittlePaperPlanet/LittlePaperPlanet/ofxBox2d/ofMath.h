
// notes:
// -----------------------------------------------------------
// for fast things look here: http://musicdsp.org/archive.php?classid=5#115
// -----------------------------------------------------------
// the random () calls are based on misconceptions described here:
// http://www.azillionmonkeys.com/qed/random.html
// (Bad advice from C.L.C. FAQ)
// we should correct this -- 
// -----------------------------------------------------------


#ifndef _OF_MATH_H
#define _OF_MATH_H


#ifndef PI
	#define PI       3.14159265358979323846
#endif

#ifndef TWO_PI
	#define TWO_PI   6.28318530717958647693
#endif

#ifndef M_TWO_PI
	#define M_TWO_PI   6.28318530717958647693
#endif

#ifndef FOUR_PI
	#define FOUR_PI 12.56637061435917295385
#endif

#ifndef HALF_PI
	#define HALF_PI  1.57079632679489661923
#endif

#ifndef DEG_TO_RAD
	#define DEG_TO_RAD (PI/180.0)
#endif

#ifndef RAD_TO_DEG
	#define RAD_TO_DEG (180.0/PI)
#endif

#ifndef MIN
	#define MIN(x,y) (((x) < (y)) ? (x) : (y))
#endif

#ifndef MAX
	#define MAX(x,y) (((x) > (y)) ? (x) : (y))
#endif

#ifndef CLAMP
	#define CLAMP(val,min,max) (MAX(MIN(val,max),min))
#endif

#ifndef ABS
	#define ABS(x) (((x) < 0) ? -(x) : (x))
#endif

int 		ofNextPow2 ( int a );
void 		ofSeedRandom();
void 		ofSeedRandom(int val);
float 		ofRandom(float val0, float val1);		// random (x - y)
float 		ofRandomf();							// random (-1 - 1)
float 		ofRandomuf();							// random (0 - 1) 

float		ofNormalize(float value, float min, float max);
float		ofMap(float value, float inputMin, float inputMax, float outputMin, float outputMax);
float		ofClamp(float value, float min, float max);
float		ofLerp(float start, float stop, float amt);
float		ofDist(float x1, float y1, float x2, float y2);
float		ofDistSquared(float x1, float y1, float x2, float y2);
int			ofSign(float n);
bool		ofInRange(float t, float min, float max);
float		ofRadToDeg(float radians);
float		ofDegToRad(float degrees); 

#endif
