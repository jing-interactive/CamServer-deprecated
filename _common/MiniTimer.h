#pragma once

#ifdef WIN32
    #include <windows.h>
    #pragma comment(lib,"winmm.lib")
#else
    #include <time.h>
    static unsigned int timeGetTime()
    {
        unsigned int uptime = 0;
        struct timespec on;
        if(clock_gettime(CLOCK_MONOTONIC, &on) == 0)
             uptime = on.tv_sec*1000 + on.tv_nsec/1000000;
        return uptime;
    }
#endif


//uncomment this if u have "FloDebug.h"
//#define USING_FLO_DEBUG

#ifdef USING_FLO_DEBUG
#include "FloDebug.h"
#else
#include <stdio.h>
#endif

class MiniTimer
{
public:
	MiniTimer()
	{
		resetStartTime();
	}

	unsigned int getTimeElapsedMS()//mil-seconds
	{
		return timeGetTime() - _start_time;
	}

	void resetStartTime()
	{
		_start_time = timeGetTime();
	}

	float getTimeElapsed()//seconds
	{
		return 0.001f*(timeGetTime() - _start_time);
	}

	void profileFunction(char* funcName)
	{
#ifdef USING_FLO_DEBUG
		FloWrite("%s[%s] : %d ms\n", getProperBlank(), funcName, getTimeElapsedMS());
#else
		printf("%s[%s] : %d ms\n", getProperBlank(), funcName, getTimeElapsedMS());
#endif
		resetStartTime();
	}

	static char* getProperBlank()
	{
		return "--";
	}

private:
	unsigned int _start_time;
};

