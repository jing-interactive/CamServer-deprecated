#include "FloDebug.h"
#include <string.h>

//this trick is learned from CSK :)
static struct DebugHelper
{
	~DebugHelper()
	{
		delete CFloDebug::GetItself();
	}
}_singleton_helper;

CFloDebug*	CFloDebug:: m_pItslef;
FILE*		CFloDebug:: fp;
char		CFloDebug::fileName[256];

CFloDebug::CFloDebug(char *filename){
	//
	strcpy(fileName, filename);
	fp = NULL;
}

CFloDebug::~CFloDebug(void){
	//
	Close();
}

int CFloDebug::Write(char *string, ...){
	//
	if (!fp)
	{
		fp = fopen(fileName,"w");
		assert(fp && "can not open the debug file");
	}
	static char buffer[256]; // working buffer

	va_list arglist; // variable argument list

	// make sure both the error file and string are valid
	if (!string || !fp)
	   return(0);

	// print out the string using the variable number of arguments on stack
	va_start(arglist,string);
	vsprintf(buffer,string,arglist);
	va_end(arglist);

	// write string to file
	fprintf(fp,buffer);

	// flush buffer incase the system bails
	fflush(fp);

	// return success
	return(1);
}


int CFloDebug::Close(void){
	// this function closes the error file

	if (fp){
		fclose(fp);
		fp = NULL;
		return(1);
	}
	else
	   return(0);

}

CFloDebug* CFloDebug:: GetItself(void){
	//
	if (!m_pItslef){
		//
		time_t rawtime;
		char fileName[256];
		time ( &rawtime );
		tm* timeinfo = localtime ( &rawtime );
		system("mkdir logs");
		strftime(fileName,80,"logs/%Y_%b_%d__%H_%M_%S.log",timeinfo);
		printf("%s created.\n", fileName);
		m_pItslef = new CFloDebug(fileName);
	}
	return m_pItslef;
}

FILE* CFloDebug::get_file(void){
	//
	return fp;
}
