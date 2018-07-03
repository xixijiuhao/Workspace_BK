#ifndef _WIN_TOOLS_
#define _WIN_TOOLS_

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <process.h>

class TCriticalSection
{
public:
	class Lock
	{
	public:
		explicit Lock(TCriticalSection &crit) : m_crit(crit) { m_crit.Enter(); }
		~Lock() { m_crit.Leave(); }

	private:
		TCriticalSection &m_crit;

		Lock(const Lock &rhs);
		Lock &operator=(const Lock &rhs);
	};

	TCriticalSection() { ::InitializeCriticalSection(&m_crit); }
	~TCriticalSection() { ::DeleteCriticalSection(&m_crit); }

	bool TryEnter()	{ return (FALSE != ::TryEnterCriticalSection(&m_crit)); }
	void Enter()
	{
		::EnterCriticalSection(&m_crit);
	}
	void Leave()	{ ::LeaveCriticalSection(&m_crit); }

private:
	CRITICAL_SECTION m_crit;

	TCriticalSection(const TCriticalSection &rhs);
	TCriticalSection &operator=(const TCriticalSection &rhs);
};

class TEvent
{
public:
	TEvent(LPSECURITY_ATTRIBUTES lpSecurityAttributes,
		bool bManualReset, bool bInitialState, LPCTSTR lpName)
		: m_hEvent(::CreateEvent(lpSecurityAttributes, bManualReset, bInitialState, lpName))
	{
		if (NULL == m_hEvent) throw 0;
	}
	TEvent(bool bManualReset, bool bInitialState) : m_hEvent(::CreateEvent(0, bManualReset, bInitialState, 0)) {}
	~TEvent()	{ ::CloseHandle(m_hEvent); }

	HANDLE GetHandle() const { return m_hEvent; }

	void Wait() const { Wait(INFINITE); }
	bool Wait(DWORD timeoutMillis) const { return (::WaitForSingleObject(m_hEvent, timeoutMillis) == WAIT_OBJECT_0); }

	void Reset() { ::ResetEvent(m_hEvent); }
	void Set() { ::SetEvent(m_hEvent); }
	void Pulse() { ::PulseEvent(m_hEvent); }

private:
	HANDLE m_hEvent;

	TEvent(const TEvent &rhs);
	TEvent &operator=(const TEvent &rhs);
};

class CAutoResetEvent : public TEvent
{
public:
	CAutoResetEvent(bool initialState = false)
		: TEvent(false, initialState) { }
};

class CManualResetEvent : public TEvent
{
public:
	CManualResetEvent(bool initialState = false)
		: TEvent(true, initialState)  { }
};

class TThread;
class TRunnable
{
public:
	friend class TThread;
protected:
	virtual void Run() = 0;
};

class TThread : public TRunnable
{
public:
	TThread() : m_hThread(INVALID_HANDLE_VALUE), m_Runnable(NULL), m_Terminated(false) {}
	explicit TThread(TRunnable* runnable) : m_hThread(INVALID_HANDLE_VALUE), m_Runnable(runnable), m_Terminated(false) {}
	virtual ~TThread() { if (m_hThread != INVALID_HANDLE_VALUE) ::CloseHandle(m_hThread); }

	HANDLE GetHandle() const { return m_hThread; }

	bool Wait(DWORD timeoutMillis) const { return (::WaitForSingleObject(m_hThread, timeoutMillis) == WAIT_OBJECT_0); }
	void Wait() const { Wait(INFINITE); }

	void Start()
	{
		m_Terminated = false;

		if (INVALID_HANDLE_VALUE == m_hThread)
		{
			//创建一个线程
			/*unsigned long _beginthreadex(
			void *security,                               //安全属性，NULL为默认安全属性
			unsigned stack_size,                          //指定线程堆栈的大小。如果为0，则线程堆栈大小和创建它的线程的相同。一般用0
			unsigned(__stdcall *start_address)(void *),   //指定线程函数的地址，也就是线程调用执行的函数地址(用函数名称即可，函数名称就表示地址)
			void *arglist,                                //传递给线程的参数的指针，可以通过传入对象的指针，在线程函数中再转化为对应类的指针
			unsigned initflag,                            //线程初始状态，0:立即运行；CREATE_SUSPEND：suspended（悬挂）
			unsigned *thrdaddr                            //用于记录线程ID的地址
			);
			*/
			m_hThread = (HANDLE)::_beginthreadex(0, 0, ThreadFunction, (void*)this, 0, &m_ThreadID);

			if (INVALID_HANDLE_VALUE == m_hThread)
				throw 0;
		}
	}
	void Terminate() { m_Terminated = true; }

	unsigned int ThreadID() { return m_ThreadID; }

protected:
	bool isTerminated() { return m_Terminated; }

private:
	HANDLE m_hThread;
	TRunnable* m_Runnable;
	bool m_Terminated;
	unsigned int m_ThreadID;

	void ThreadRun()
	{
		if (m_Runnable)
			m_Runnable->Run();
		else
			this->Run();
	}

	TThread(const TThread &rhs);
	TThread &operator=(const TThread &rhs);

	static unsigned int __stdcall ThreadFunction(void *pv)
	{
		TThread* pThis = (TThread*)pv;

		if (pThis)
		{
#ifndef _DEBUG
			try {
#endif
				pThis->ThreadRun();
#ifndef _DEBUG
			}
			catch (...) {}
#endif
		}

		return 0;
	}
};

class TIOCompletionPort
{
public:
	explicit TIOCompletionPort(size_t maxConcurrency = 0)
		: m_iocp(::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, maxConcurrency))
	{
		if (NULL == m_iocp) throw 0;
	}
	~TIOCompletionPort() { ::CloseHandle(m_iocp); }

	bool Associate(HANDLE hDevice, DWORD completionKey)
	{
		return m_iocp == ::CreateIoCompletionPort(hDevice, m_iocp, completionKey, 0);
	}

	bool PostStatus(DWORD dwNumBytes, DWORD completionKey, OVERLAPPED *pOverlapped = 0)
	{
		return FALSE != ::PostQueuedCompletionStatus(m_iocp, dwNumBytes, completionKey, pOverlapped);
	}

	bool GetStatus(PDWORD pdwNumBytes, DWORD *pCompletionKey, OVERLAPPED **ppOverlapped, DWORD dwMilliseconds = INFINITE)
	{
		return false;
		//return FALSE != ::GetQueuedCompletionStatus(m_iocp, pdwNumBytes, pCompletionKey, ppOverlapped, dwMilliseconds);
	}

private:
	HANDLE m_iocp;

	TIOCompletionPort(const TIOCompletionPort &rhs);
	TIOCompletionPort &operator=(const TIOCompletionPort &rhs);
};

class TMutex
{
public:
	friend class TCondition;
	class Lock
	{
	public:
		explicit Lock(TMutex &mutex) : m_Mutex(mutex) { m_Mutex.Enter(); }
		~Lock() { m_Mutex.Leave(); }

	private:
		TMutex &m_Mutex;

		Lock(const Lock &rhs);
		Lock &operator=(const Lock &rhs);
	};

	TMutex() { m_Mutex = ::CreateMutex(0, 0, 0); }
	~TMutex() { ::CloseHandle(m_Mutex); }

	void Enter()
	{
		::WaitForSingleObject(m_Mutex, INFINITE);
	}
	void Leave()	
	{ 
		::ReleaseMutex(m_Mutex);
	}

private:
	HANDLE m_Mutex;

	TMutex(const TMutex &rhs);
	TMutex &operator=(const TMutex &rhs);
};

class TCondition
{
public:
	explicit TCondition(TMutex& mutex) : m_Mutex(mutex)
	{
		/*
		HANDLE     CreateEvent(
			LPSECURITY_ATTRIBUTES     lpEventAttributes, //SD
			BOOL     bManualReset,                       //reset     type
			BOOL     bInitialState,                      //initial     state
			LPCTSTR     lpName                           //object     name
		);   */
		m_Event[0] = ::CreateEvent(0, 0, 0, 0);
		m_Event[1] = m_Mutex.m_Mutex;
	}
	~TCondition()
	{
		::CloseHandle(m_Event[0]);
	}

	void Wait()
	{
		m_Mutex.Leave();
		/*
		//等待Object被标为有信号(signaled)时才返回的
		DWORD WaitForMultipleObjects(
		  DWORD nCount,             // 指定列表中的句柄数量  最大值为MAXIMUM_WAIT_OBJECTS（64）
		  CONST HANDLE *lpHandles,  // 句柄数组的指针。lpHandles为指定对象句柄组合中的第一个元素 HANDLE类型可以为（Event，Mutex，Process，Thread，Semaphore）数组
		  BOOL fWaitAll,            // 等待的类型，如果为TRUE，表示除非对象都发出信号，否则就一直等待下去；如果FALSE，表示任何对象发出信号即可
		  DWORD dwMilliseconds      // 指定要等候的毫秒数。如设为零，表示立即返回。如指定常数INFINITE，则可根据实际情况无限等待下去
		);  
		*/
		::WaitForMultipleObjects(2, m_Event, TRUE, INFINITE);
	}
	void Notify()
	{
		::SetEvent(m_Event[0]);
	}

private:
	HANDLE m_Event[2];
	TMutex& m_Mutex;
};

class TBoundedQueue
{
public:
	class Buffer
	{
	public:
		void Init(int objsize) { capacity = objsize - sizeof(Buffer) + 1; }
		bool IsEmpty() { return (0 == size); }
		bool CanPut(const int len) { return (capacity - size > len); }
		void Put(const char* buf, const int len);
		void Take(char*& buf, int& len);
	private:
		int capacity;  // 容量
		int size;      // 当前长度
		char Data[1];  // 消息数组
	};

public:
	TBoundedQueue(int MB = 1);
	~TBoundedQueue();
	void Put(const char* buf, const int len);
	void Take(char*& buf, int& len);

private:
	TMutex mutex;
	TCondition notEmpty;
	TCondition notFull;
	Buffer* currBuffer;
	Buffer* nextBuffer;
	Buffer* swapBuffer;
	Buffer* workBuffer;
};

#endif