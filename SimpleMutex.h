#pragma once

#include <windows.h>


class CSimpleMutex: public CRITICAL_SECTION
{
public:
    CSimpleMutex ()
            : m_nLockCount(0)
    {
        ::InitializeCriticalSection(this);
    }

    ~CSimpleMutex ()
    {
        ::DeleteCriticalSection(this);
    }

    void Lock ()
    {
        ::EnterCriticalSection(this);
        m_nLockCount++;
    }
    void UnLock ()
    {
        ::LeaveCriticalSection(this);
        m_nLockCount--;
    }


public :
    int m_nLockCount;
};
