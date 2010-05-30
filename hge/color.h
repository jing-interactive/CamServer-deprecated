#pragma once
//color.h, the color struct definition

//sizeof(color_t) = 4
//so no need to use color_t& instead of color_t as parameter.
//while, using color_t bring us some benifit.

#include <windows.h>
#include <stdio.h>
#include <assert.h>

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
	inline color_t& operator +=(const color_t& another){

		_r += another._r;
		_g += another._g;
		_b += another._b;
		//_a += another._a;
		assert(_r>=0 || _g>=00 || _b>=00
			|| _r <= 255 || _g<=255 || _b<=255);
		return 	*this;
	}
	inline color_t& operator -=(const color_t& another){
		assert(0 && "should never call color_t::oprator -(const color_t& )");
/*
				_r -= another._r;
				_g -= another._g;
				_b -= another._b;
				//_a -= another._a;
				assert(_r>=0 || _g>=00 || _b>=00
					|| _r <= 255 || _g<=255 || _b<=255);*/
				return 	*this;
		
	}
	inline color_t& operator *=(float k){
		assert(0 && "should never call color_t::oprator *=(float )");

/*
					_r *= k;
					_g *= k;
					_b *= k;
					//_a *= k;
					assert(_r>=0 || _g>=00 || _b>=00
						|| _r <= 255 || _g<=255 || _b<=255);*/
			
		return *this;
	}

	inline color_t& operator *=(BYTE k){
		_r = _r*k >>8;
		_g = _g*k >>8;
		_b = _b*k >>8;
		return *this;
	}
		
	inline friend color_t operator + (const color_t& left, const color_t& right){
		color_t ret(left);
		ret += right;
		return ret;
	}

	inline friend color_t operator - (const color_t& left, const color_t& right){
		color_t ret(left);
		ret -= right;
		return ret;
	}
	

	inline friend color_t operator * (const color_t& left, float k){
		color_t ret(left);
		ret *= k;
		return ret;
	}
	void fprint(FILE* fp)
	{
		fprintf(fp, "argb <%d, %d, %d, %d>\n", _a, _r, _g, _b);
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