#pragma once

//++++++++++++++++++++
//
//debug_file library can save your information in
//the file defaultly named "调试文件.txt"
//
//usage:
//after including it , u only need to call it this way
//
//FloWrite("%s", "Hello world!");
//
//
//vinjn 2006
//
//copyleft ◎ flonier soft
//
//
//++++++++++++++++++++


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>
#include <time.h>
#include <math.h>

extern bool using_debug_file;//u decide whether to use it.

#define debug_file_name  "调试文件.txt"//uh..  u can changed its name

class CFloDebug{
	//it's used to deal with error information
public:
	CFloDebug(char* filename);
	~CFloDebug();
	static CFloDebug* GetItself(void);

	static int Close(void);
	static int Write(char *string, ...);
	static FILE* get_file(void);
private:
	static FILE *m_fpDebug;
	static CFloDebug*		m_pItslef;

};


#define FloWrite  if (using_debug_file) CFloDebug::GetItself()->Write
#define shared_file_ptr  CFloDebug::GetItself()->get_file()

