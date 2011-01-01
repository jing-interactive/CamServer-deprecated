#pragma once

#include <stdlib.h>

#ifndef MIN
#define MIN(x,y) (((x) < (y)) ? (x) : (y))
#endif

#ifndef MAX
#define MAX(x,y) (((x) > (y)) ? (x) : (y))
#endif

//--------------------------------------------------
float ofRandom(float x, float y) {// random (x - y)

	float high = 0;
	float low = 0;
	float randNum = 0;
	// if there is no range, return the value
	if (x == y) return x; 			// float == ?, wise? epsilon?
	high = MAX(x,y);
	low = MIN(x,y);
	randNum = low + ((high-low) * rand()/(RAND_MAX + 1.0));
	return randNum;
}

//--------------------------------------------------
float ofRandomf() {// random (-1 - 1)
	float randNum = 0;
	randNum = (rand()/(RAND_MAX + 1.0)) * 2.0 - 1.0;
	return randNum;
}

//--------------------------------------------------
float ofRandomuf() {// random (0 - 1) 
	float randNum = 0;
	randNum = rand()/(RAND_MAX + 1.0);
	return randNum;
}
