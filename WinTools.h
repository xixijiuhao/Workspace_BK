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
			//����һ���߳�
			/*unsigned long _beginthreadex(
			void *security,                               //��ȫ���ԣ�NULLΪĬ�ϰ�ȫ����
			unsigned stack_size,                          //ָ���̶߳�ջ�Ĵ�С�����Ϊ0�����̶߳�ջ��С�ʹ��������̵߳���ͬ��һ����0
			unsigned(__stdcall *start_address)(void *),   //ָ���̺߳����ĵ�ַ��Ҳ�����̵߳���ִ�еĺ�����ַ(�ú������Ƽ��ɣ��������ƾͱ�ʾ��ַ)
			void *arglist,                                //���ݸ��̵߳Ĳ�����ָ�룬����ͨ����������ָ�룬���̺߳�������ת��Ϊ��Ӧ���ָ��
			unsigned initflag,                            //�̳߳�ʼ״̬��0:�������У�CREATE_SUSPEND��suspended�����ң�
			unsigned *thrdaddr                            //���ڼ�¼�߳�ID�ĵ�ַ
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
		//�ȴ�Object����Ϊ���ź�(signaled)ʱ�ŷ��ص�
		DWORD WaitForMultipleObjects(
		  DWORD nCount,             // ָ���б��еľ������  ���ֵΪMAXIMUM_WAIT_OBJECTS��64��
		  CONST HANDLE *lpHandles,  // ��������ָ�롣lpHandlesΪָ������������еĵ�һ��Ԫ�� HANDLE���Ϳ���Ϊ��Event��Mutex��Process��Thread��Semaphore������
		  BOOL fWaitAll,            // �ȴ������ͣ����ΪTRUE����ʾ���Ƕ��󶼷����źţ������һֱ�ȴ���ȥ�����FALSE����ʾ�κζ��󷢳��źż���
		  DWORD dwMilliseconds      // ָ��Ҫ�Ⱥ�ĺ�����������Ϊ�㣬��ʾ�������ء���ָ������INFINITE����ɸ���ʵ��������޵ȴ���ȥ
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
		int capacity;  // ����
		int size;      // ��ǰ����
		char Data[1];  // ��Ϣ����
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