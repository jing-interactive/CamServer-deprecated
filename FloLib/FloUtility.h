#pragma once

#include <stdio.h>
#include <windowsx.h>

//little utilities

#define SAFE_DELETE(p) {if (p) delete p; p = NULL;}
#define SAFE_DELETE_ARRAY(p) {if (p) delete[] p; p = NULL;}
#define SAFE_RELEASE(p) {if (p) p->release(); p = NULL;}

#define random_integer(a, b) ((a) + rand()%((b)-(a)+1))
#define random_float() (float) (rand()/(RAND_MAX * 1.0))
#define SIGN(x) (((x) < 0) ? -1 : 1)

void get_line(FILE* f, char* string);


#define prefix_m(variable_name) _##variable_name
#define prefix_set(variable_name) set##variable_name
#define prefix_get(variable_name) get##variable_name

#define set_get_define(variable_type, variable_name)\
protected:\
	variable_type prefix_m(variable_name);\
public:\
	void prefix_set(variable_name)(variable_type variable_name)\
	{\
		prefix_m(variable_name) = variable_name;\
	}\
	variable_type prefix_get(variable_name)()\
	{\
		return prefix_m(variable_name);\
	}



#define get_define(variable_type, variable_name)\
protected:\
	variable_type prefix_m(variable_name);\
public:\
	variable_type prefix_get(variable_name)()\
	{\
		return prefix_m(variable_name);\
	}

#define set_define(variable_type, variable_name)\
protected:\
	variable_type prefix_m(variable_name);\
public:\
	void prefix_set(variable_name)(variable_type variable_name){ prefix_m(variable_name) = variable_name;}

#define return_false_if_failed(hr, func_name) if (FAILED(hr)) \
{\
	::MessageBox(0, #func_name##"() - failed",0,0);\
	return false;\
}

#define return_false_if(cause, func_name) if ((cause)) \
{\
	::MessageBox(0, #func_name##"() - "## #cause,0,0);\
	return false;\
}

#define return_if_failed(hr, func_name) if (FAILED(hr)) \
{\
	::MessageBox(0, #func_name##"() - failed",0,0);\
	return;\
}

#define return_if(cause, func_name) if ((cause)) \
{\
	::MessageBox(0, #func_name##"() - "## #cause,0,0);\
	return;\
}
