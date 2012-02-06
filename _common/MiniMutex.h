#pragma once

#include <windows.h>

class MiniMutex: public CRITICAL_SECTION
{
public:
    MiniMutex()
            : m_nLockCount(0)
    {
        ::InitializeCriticalSection(this);
    }

    ~MiniMutex()
    {
        ::DeleteCriticalSection(this);
    }

    void lock()
    {
        ::EnterCriticalSection(this);
        m_nLockCount++;
    }
    void unlock()
    {
        ::LeaveCriticalSection(this);
        m_nLockCount--;
    }

public :
    int m_nLockCount;
};

struct ScopedLocker
{
	MiniMutex& _mutex;
	ScopedLocker(MiniMutex& mutex):_mutex(mutex)
	{
		_mutex.lock();
	}
	~ScopedLocker()
	{
		_mutex.unlock();
	}
};

/*usage
MiniMutex a_mutex;

{
ScopedLocker(a_mutex);
//do something
}
*/
#define LOCK_START(mutex) {ScopedLocker _locker(mutex)
#define LOCK_END() }

class MiniEvent
{
public:
	MiniEvent(BOOL bManualReset = TRUE)
	{
		// start in non-signaled state (red light)
		// event is set manually
		// auto reset after every Wait
		_handle = ::CreateEvent (NULL, bManualReset, FALSE, NULL);
	}

	~MiniEvent()
	{
		::CloseHandle (_handle);
	}

	// put into signaled state
	void set() 
	{ 
		::SetEvent (_handle); 
	}

	void wait(DWORD dwMilliseconds = INFINITE)
	{
		// Wait until event is in signaled (green) state
		::WaitForSingleObject (_handle, dwMilliseconds);
	}

	operator HANDLE() 
	{ 
		return _handle;
	}

	HANDLE getHandle() const
	{
		return _handle;
	}
private:
	HANDLE _handle;
};
