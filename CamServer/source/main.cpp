#define VERSION "0.4.1"

#include <process.h>
#include "VideoApp.h"

void enableMemleakCheck(int breakpt = 0)
{
	#if defined _DEBUG && defined _MSC_VER
	#include <crtdbg.h>
	if (breakpt > 0)
		_CrtSetBreakAlloc(breakpt);
	_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
	#endif
}

void startup_beep(void *)
{
	printf("CamServer %s  vinjn @ RoboPeak\n", VERSION);
	for (int b=0;b<8;b++)
	{
		for (int i=0;i<10;i++)
			printf("%c ", rand()%3+1);
		printf("\n");
		::Beep(sin(b/20.0f*3.14)*300,100);
	}
}

void ready_beep(void *)
{
	for (int b=0;b<1;b++)
		::Beep(cos(b/20.0f*3.14)*100,100);
}

void exit_beep(void *)
{
	printf("CamServer %.1f  vinjn @ RoboPeak\n", VERSION);
	for (int b=8;b>1;b--)
	{
		for (int j=0;j<8-b;j++)
			printf("\t");
		printf("bye~\n");
		::Beep(sin(b/20.0f*3.14)*300,100);
	}
}
bool using_debug_file = true;

int main(int argc, char** argv )
{
	enableMemleakCheck(); 
	_beginthread(startup_beep, 0, 0);

	if (theApp.init(argc, argv))
	{
		_beginthread(ready_beep, 0, 0);
		theApp.run();
	}
	exit_beep(NULL);

	return 0;
}
