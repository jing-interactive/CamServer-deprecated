#pragma once

//++++++++++++++++++++
//
//debug_file library can save your information in
//the file defaultly named "µ÷ÊÔÎÄ¼þ.txt"
//
//usage:
//after including it , u only need to call it this way
//
//FloWrite("%s", "Hello world!");
//
//
//vinjn 2006
//
//copyleft ¡ò flonier soft
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

class CFloDebug{
	//it's used to deal with error information
public:
	CFloDebug(char* filename);
	~CFloDebug();
	static CFloDebug* GetItself(void);

	static bool Close(void);
	static bool Write(char *string, ...);
	static FILE* GetFILE(void);
private:
	static char fileName[256];
	static FILE *fp;
	static CFloDebug*		m_pItslef;

};


#define FloWrite  if (using_debug_file) CFloDebug::GetItself()->Write
#define FloWriteLn()  FloWrite("\n=====================================\n");
#define shared_file_ptr  CFloDebug::GetItself()->GetFILE()

