#include "FloDebug.h"



CFloDebug*	CFloDebug:: m_pItslef;
FILE*		CFloDebug:: m_fpDebug;


CFloDebug::CFloDebug(char *filename){
	//
	m_fpDebug = fopen(filename,"w");
	assert(m_fpDebug && "can not open the debug file");

	// get the current time
/*	timeb timebuffer;
	char *timeline;
	char timestring[280];

	_ftime(&timebuffer);
	timeline = ctime(&(timebuffer.time));

	sprintf(timestring, "%.19s.%hu, %s", timeline, timebuffer.millitm, &timeline[20]);*/

	// write out error header with time
	Write("\nOpening Debug Output File (%s)\n",filename);

	// now the file is created, re-open with append mode

//	fclose(m_fpDebug);
//	m_fpDebug = fopen(filename,"w");
//	assert(m_fpDebug && "can not close the debug file");
}

CFloDebug::~CFloDebug(void){
	//
	Close();
}


int CFloDebug::Write(char *string, ...){
	//
	static char buffer[120]; // working buffer

	va_list arglist; // variable argument list

	// make sure both the error file and string are valid
	if (!string || !m_fpDebug)
	   return(0);

	// print out the string using the variable number of arguments on stack
	va_start(arglist,string);
	vsprintf(buffer,string,arglist);
	va_end(arglist);

	// write string to file
	fprintf(m_fpDebug,buffer);

	// flush buffer incase the system bails
	fflush(m_fpDebug);

	// return success
	return(1);
}


int CFloDebug::Close(void){
	// this function closes the error file

	if (m_fpDebug){
		Write("\nClosing Debug Output File.");
		fclose(m_fpDebug);
		m_fpDebug = NULL;
		return(1);
	}
	else
	   return(0);

}

static struct DebugHelper
{
	~DebugHelper()
	{
		delete CFloDebug::GetItself();
	}
}_singleton_helper;

CFloDebug* CFloDebug:: GetItself(void){
	//
	if (!m_pItslef){
		//
		m_pItslef = new CFloDebug(debug_file_name);
	}
	return m_pItslef;
}

FILE* CFloDebug::get_file(void){
	//
	return m_fpDebug;
}
