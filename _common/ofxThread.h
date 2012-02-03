#ifndef _OFX_THREAD_H_
#define _OFX_THREAD_H_

#ifdef WIN32	
#include <stdio.h>
#include <windows.h>
#include <process.h>
#else
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#endif

class ofxThread{

public:
	ofxThread()
	{
		threadRunning = false; 
#ifdef WIN32 
		InitializeCriticalSection(&critSec); 
#else 
		pthread_mutex_init(&myMutex, NULL); 
#endif 
	}
	virtual ~ofxThread()
	{
#ifndef WIN32 
		pthread_mutex_destroy(&myMutex); 
#endif 
		stopThread(); 
	}
	bool isThreadRunning()
	{
		//should be thread safe - no writing of vars here 
		return threadRunning; 
	}
	void startThread(bool _blocking = true, bool _verbose = true)
	{
		if( threadRunning ){ 
			if(verbose)printf("ofxThread: thread already running\n"); 
			return; 
		} 

		//have to put this here because the thread can be running 
		//before the call to create it returns 
		threadRunning   = true; 

#ifdef WIN32 
		//InitializeCriticalSection(&critSec); 
		myThread = (HANDLE)_beginthreadex(NULL, 0, this->thread,  (void *)this, 0, NULL); 
#else 
		//pthread_mutex_init(&myMutex, NULL); 
		pthread_create(&myThread, NULL, thread, (void *)this); 
#endif 

		blocking      =   _blocking; 
		verbose         = _verbose; 
	}
	bool lock()
	{
#ifdef WIN32 
		if(blocking)EnterCriticalSection(&critSec); 
		else { 
			if(!TryEnterCriticalSection(&critSec)){ 
				if(verbose)printf("ofxThread: mutext is busy \n"); 
				return false; 
			} 
		} 
		if(verbose)printf("ofxThread: we are in -- mutext is now locked \n"); 
#else 

		if(blocking){ 
			if(verbose)printf("ofxThread: waiting till mutext is unlocked\n"); 
			pthread_mutex_lock(&myMutex); 
			if(verbose)printf("ofxThread: we are in -- mutext is now locked \n"); 
		}else{ 
			int value = pthread_mutex_trylock(&myMutex); 
			if(value == 0){ 
				if(verbose)printf("ofxThread: we are in -- mutext is now locked \n"); 
			} 
			else{ 
				if(verbose)printf("ofxThread: mutext is busy - already locked\n"); 
				return false; 
			} 
		} 
#endif 

		return true; 
	}
	bool unlock()
	{
#ifdef WIN32 
		LeaveCriticalSection(&critSec); 
#else 
		pthread_mutex_unlock(&myMutex); 
#endif 

		if(verbose)printf("ofxThread: we are out -- mutext is now unlocked \n"); 

		return true; 
	}
	void stopThread()
	{
		if(threadRunning){ 
#ifdef WIN32 
			CloseHandle(myThread); 
#else 
			//pthread_mutex_destroy(&myMutex); 
			pthread_detach(myThread); 
#endif 
			if(verbose)printf("ofxThread: thread stopped\n"); 
			threadRunning = false; 
		}else{ 
			if(verbose)printf("ofxThread: thread already stopped\n"); 
		} 
	}

protected:

	//-------------------------------------------------
	//you need to overide this with the function you want to thread
	virtual void threadedFunction(){
		if(verbose)printf("ofxThread: overide threadedFunction with your own\n");
	}

	//-------------------------------------------------

#ifdef WIN32
	static unsigned int __stdcall thread(void * objPtr){
		ofxThread* me	= (ofxThread*)objPtr;
		me->threadedFunction();
		return 0;
	}

#else
	static void * thread(void * objPtr){
		ofxThread* me	= (ofxThread*)objPtr;
		me->threadedFunction();
		return 0;
	}
#endif


#ifdef WIN32
	HANDLE            myThread;
	CRITICAL_SECTION  critSec;  	//same as a mutex
#else
	pthread_t        myThread;
	pthread_mutex_t  myMutex;
#endif

	bool threadRunning;
	bool blocking;
	bool verbose;
};

#endif
