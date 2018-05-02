#pragma once

class CMyLock
{
	friend class Guard;
public:
	CMyLock()
	{
		InitializeCriticalSection(&m_lockInst);
	}

	~CMyLock()
	{
		DeleteCriticalSection(&m_lockInst);
	}

	void Lock()
	{
		EnterCriticalSection(&m_lockInst);
	}

	void UnLock()
	{
		LeaveCriticalSection(&m_lockInst);
	}

	class Guard
	{
	public:
		Guard(CMyLock& critSec) : m_lockRef(critSec.m_lockInst)
		{
			EnterCriticalSection(&m_lockRef);
		}

		~Guard()
		{
			LeaveCriticalSection(&m_lockRef);
		}
	private:
		CRITICAL_SECTION& m_lockRef;
	};
private:
	CRITICAL_SECTION m_lockInst;
};