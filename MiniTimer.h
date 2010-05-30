#pragma once

#include <windows.h>
#include <assert.h>


//uncomment this if u have "FloLib/FloDebug.h"
//#define USING_FLO_DEBUG

#ifdef USING_FLO_DEBUG
#include "FloLib/FloDebug.h"
#else
#include <stdio.h>
#endif

#pragma comment(lib,"winmm.lib")

class MiniTimer
{
public:
	MiniTimer()
	{
		s_level++;
		resetStartTime();
	}
	~MiniTimer()
	{
		s_level--;
		assert(s_level>=0);
	}
	DWORD getTimeElapsedMS()//mil-seconds
	{
		return ::timeGetTime() - _start_time;
	}
	void resetStartTime()
	{
		_start_time = ::timeGetTime();
	}
	float getTimeElapsed()//seconds
	{
		return 0.001f*(::timeGetTime() - _start_time);
	}

	void profileFunction(char* funcName)
	{
#ifdef USING_FLO_DEBUG
		FloWrite("%s[%s] cost : %d ms\n", getProperBlank(), funcName, getTimeElapsedMS());
#else 
		printf("%s[%s] cost : %d ms\n", getProperBlank(), funcName, getTimeElapsedMS());	
#endif
		resetStartTime();
	}

	static char* getProperBlank()
	{
		DWORD i;
		for (i=0;i<s_level*2;i++)
			_blank_str[i]='-';
		_blank_str[i]=0;
		return _blank_str;
	}

private:
	DWORD _start_time;
	static DWORD s_level;
	static char _blank_str[MAX_PATH];
};

