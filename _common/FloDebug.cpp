#include "FloDebug.h"
#include <string.h>

//this trick is learned from CSK :)
static struct DebugHelper
{
	~DebugHelper()
	{
		CFloDebug::Close();
	}
}_singleton_helper;

CFloDebug*	CFloDebug:: m_pItslef;
FILE*		CFloDebug:: fp;
char		CFloDebug::fileName[256];

CFloDebug::CFloDebug(char *filename)
{
	strcpy(fileName, filename);
	fp = NULL;
}

CFloDebug::~CFloDebug(void)
{
	Close();
}

bool CFloDebug::Write(char *string, ...){
	//
	if (!fp)
	{
		fp = fopen(fileName,"w");
		assert(fp && "can not open the debug file");
	}
	static char buffer[256];

	va_list arglist;

	if (!string || !fp)
	   return false;

	va_start(arglist,string);
	vsprintf(buffer,string,arglist);
	va_end(arglist);

	fprintf(fp,buffer);

	fflush(fp);
	return true;
}


bool CFloDebug::Close(void)
{
	if (fp){
		fclose(fp);
		fp = NULL;
		delete m_pItslef;
		return true;
	}
	else
	   return false;
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

FILE* CFloDebug::GetFILE(void){
	//
	return fp;
}
