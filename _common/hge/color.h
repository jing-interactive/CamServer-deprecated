#pragma once
//color.h, the color struct definition

//sizeof(color_t) = 4
//so no need to use color_t& instead of color_t as parameter.
//while, using color_t bring us some benifit.

#include <windows.h>

struct color_t
{
	inline color_t(const DWORD argb = 0xFF000000){
		_argb = argb;
	}
	color_t( BYTE r, BYTE g, BYTE b, BYTE a = 0xFF ){
		_r = r;
		_g = g;
		_b = b;
		_a = a;
	} 
	operator DWORD() const{
		return _argb;
	}
	union 
    {
		DWORD _argb;                  // compressed format
		BYTE _argb_M[4];             // array format
		struct {  BYTE _b,_g,_r,_a;  }; // explict name format
    }; 
};