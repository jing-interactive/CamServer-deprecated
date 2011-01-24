#include "ofxThread.h"
#include "VideoApp.h"

#define VERSION "0.4.1"

void enableMemleakCheck(int breakpt = 0)
{
	#if defined _DEBUG && defined _MSC_VER
	#include <crtdbg.h>
	if (breakpt > 0)
		_CrtSetBreakAlloc(breakpt);
	_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
	#endif
}

bool using_debug_file = true;

struct StartThread: public ofxThread
{
	void threadedFunction()
	{
		printf("CamServer %s  vinjn @ RoboPeak\n", VERSION);
		for (int b=0;b<8;b++)
		{
			for (int i=0;i<10;i++)
				printf("%c ", rand()%3+1);
			printf("\n");
			BEEP(sin(b/20.0f*3.14)*300,100);
		}
	}
}start_thread;

struct ReadyThread: public ofxThread
{
	void threadedFunction()
	{
		for (int b=0;b<1;b++)
			BEEP(cos(b/20.0f*3.14)*100,100);
	}
}ready_thread;

struct ExitThread: public ofxThread
{
	void threadedFunction()
	{
		printf("CamServer %.1f  vinjn @ RoboPeak\n", VERSION);
		for (int b=8;b>1;b--)
		{
			for (int j=0;j<8-b;j++)
				printf("\t");
			printf("bye~\n");
			BEEP(sin(b/20.0f*3.14)*300,100);
		}
	}
}exit_thread;

int main(int argc, char** argv )
{
	enableMemleakCheck(); 
	start_thread.startThread();

	if (theApp.init(argc, argv))
	{
		ready_thread.startThread();
		theApp.run();
	}
	exit_thread.startThread();

	return 0;
}
