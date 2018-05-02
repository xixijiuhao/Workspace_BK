#pragma once

#include <windows.h>

//使用哪种实现
#define xRWLock RWLock_B

//读写锁
//写优先的实现，读者较快；//去掉read_lock则为非写优先的实现，没有读操作的时候写操作才可以执行，读者最快
//基于CRITICAL_SECTION实现，简单高效，只支持在进程内使用，不支持进程间使用，两个读线程同时到来时会短暂互斥
class RWLock_A
{
private://
	volatile long reader_count; //读者计数
	CRITICAL_SECTION read_lock;	//读锁
	CRITICAL_SECTION write_lock;//写锁
public:
	RWLock_A()
	{
		reader_count = 0;
		InitializeCriticalSectionAndSpinCount(&read_lock, 4000);
		InitializeCriticalSectionAndSpinCount(&write_lock, 4000); 
	}
	virtual ~RWLock_A()
	{
		DeleteCriticalSection(&read_lock);
		DeleteCriticalSection(&write_lock);
	}

	//加读锁
	void readLock()
	{
		EnterCriticalSection(&read_lock);
		if (InterlockedIncrement(&reader_count) == 1)
			EnterCriticalSection(&write_lock);
		LeaveCriticalSection(&read_lock);
	}
	//尝试读，尝试成功后需解锁，尝试失败则说明当前不能读
	bool tryRead()
	{
		if (TryEnterCriticalSection(&read_lock))
		{
			LeaveCriticalSection(&read_lock);
			if(InterlockedIncrement(&reader_count) == 1)	
				EnterCriticalSection(&write_lock);
			return true;
		}
		else
			return false;
	}
	//解读锁
	void readUnLock()
	{
		if (InterlockedDecrement(&reader_count) == 0)
			LeaveCriticalSection(&write_lock);
	}

	//加写锁，在此加读锁，以保证写优先
	void writeLock()
	{
		EnterCriticalSection(&read_lock);		
		EnterCriticalSection(&write_lock);
	}
	//尝试写，非写优先，尝试成功后需解锁，尝试失败则说明当前不能写
	bool tryWrite()
	{
		if (TryEnterCriticalSection(&read_lock))
		{
			EnterCriticalSection(&write_lock);
			return true;
		}
		else
			return false;
	}
	//解写锁
	void writeUnLock()
	{
		LeaveCriticalSection(&write_lock);
		LeaveCriticalSection(&read_lock);
	}
};

/**************************************************************************************/
//读写锁，写优先的实现，写者速度较快 volatile
class RWLock_B
{
private:
	HANDLE writeThread;
	volatile long reader_count; //读者计数
	HANDLE read_lock;			//读锁
	CRITICAL_SECTION write_lock;//写锁
public:
	RWLock_B()
	{
		reader_count = 0;
	  	read_lock = CreateEvent(nullptr, true, true, nullptr);
		InitializeCriticalSectionAndSpinCount(&write_lock, 4000); 
	}
	virtual ~RWLock_B()
	{
		writeUnLock();
		CloseHandle(read_lock); 
		DeleteCriticalSection(&write_lock);
	}

	//加读锁
	void readLock()
	{
		if (writeThread == GetCurrentThread() || WaitForSingleObject(read_lock, INFINITE) == WAIT_OBJECT_0)
		{
			if (InterlockedIncrement(&reader_count) == 1)
				EnterCriticalSection(&write_lock);
		}
	}
	//尝试读，尝试成功后需解锁，尝试失败则说明当前不能读
	bool tryRead()
	{
		if (writeThread == GetCurrentThread() || WaitForSingleObject(read_lock, 0) == WAIT_OBJECT_0)
		{
			if(InterlockedIncrement(&reader_count) == 1)	
				EnterCriticalSection(&write_lock);
			return true;
		}
		else
			return false;
	}
	//解读锁
	void readUnLock()
	{
		if (InterlockedDecrement(&reader_count) == 0)
			LeaveCriticalSection(&write_lock);
	}

	//加写锁，写优先的
	void writeLock()
	{
		EnterCriticalSection(&write_lock);
		writeThread = GetCurrentThread();
		ResetEvent(read_lock);				//挂起读线程
	}
	//尝试写，非写优先，尝试成功后需解锁，尝试失败则说明当前不能写
	bool tryWrite()
	{
		if (TryEnterCriticalSection(&write_lock))
		{
			writeThread = GetCurrentThread();
			ResetEvent(read_lock);				//挂起读线程
			return true;
		}
		else
			return false;
	}
	//解写锁
	void writeUnLock()
	{
		SetEvent(read_lock);				//激活读线程
		LeaveCriticalSection(&write_lock);
	}
};

//RWLock_C为网上流传的原始代码，RWLock_A、RWLock_B是参考这个优化而来的
class RWLock_C
{
private:
	unsigned int reader_count; 	//读者计数
	unsigned int max_reader; //最大并发读者数
	CRITICAL_SECTION write_lock; //写锁
	CRITICAL_SECTION read_lock; //读锁
	CRITICAL_SECTION v_reader_count; //读者计数访问互斥锁
	CRITICAL_SECTION max_reader_lock; //最大并发读者锁
public:
	RWLock_C( unsigned int max_reader_num = 1000 )
	{
		reader_count = 0;
		max_reader = max_reader_num;
		InitializeCriticalSection(&write_lock); //临界区的初始化
		InitializeCriticalSection(&v_reader_count);
		InitializeCriticalSection(&read_lock);
		InitializeCriticalSection(&max_reader_lock);
	}
	virtual ~RWLock_C()
	{
		DeleteCriticalSection(&write_lock);
		DeleteCriticalSection(&v_reader_count);
		DeleteCriticalSection(&read_lock);
		DeleteCriticalSection(&max_reader_lock);
	}
	void readLock()
	{
		EnterCriticalSection(&write_lock);
		EnterCriticalSection(&v_reader_count);
		reader_count ++;
		if( reader_count == 1 )
			EnterCriticalSection(&read_lock);
		if( reader_count >= max_reader ) 
		{ 
			LeaveCriticalSection(&v_reader_count); 
			LeaveCriticalSection(&write_lock); 
			EnterCriticalSection(&max_reader_lock); //进入max读锁临界区
			return;
		}
		LeaveCriticalSection(&v_reader_count);
		LeaveCriticalSection(&write_lock);
	}
	bool tryRead()
	{
		if(TryEnterCriticalSection(&write_lock))
		{
			EnterCriticalSection(&v_reader_count);
			reader_count ++;
			if( reader_count == 1 )
				EnterCriticalSection(&read_lock);
			if( reader_count >= max_reader ) 
			{ 
				if( !TryEnterCriticalSection(&max_reader_lock) ) //尝试进入max读锁临界区
				{
					reader_count --; 
					LeaveCriticalSection(&v_reader_count);
					LeaveCriticalSection(&write_lock);
					return false; //不可读
				}
			}
			LeaveCriticalSection(&v_reader_count);
			LeaveCriticalSection(&write_lock);
			return true;
		}
		else return false;
	}
	void readUnLock()
	{
		EnterCriticalSection(&v_reader_count);
		if( reader_count >= max_reader )
			LeaveCriticalSection(&max_reader_lock);
		reader_count --;
		if( reader_count == 0 )
			LeaveCriticalSection(&read_lock);
		LeaveCriticalSection(&v_reader_count);
	}
	void writeLock()
	{
		EnterCriticalSection(&write_lock);
		EnterCriticalSection(&read_lock);
	}
	//尝试写（不能写优先，写优先需要用writeLock()进行等待性加锁）
	bool tryWrite()
	{
		if(TryEnterCriticalSection(&write_lock))
		{
			EnterCriticalSection(&read_lock);
			return true;
		}
		else return false;
	}
	void writeUnLock()
	{
		LeaveCriticalSection(&read_lock);
		LeaveCriticalSection(&write_lock);
	}
};


//自动加锁
template<typename T>
class AutoLock
{
private:
	T &m_lock;

public:
	explicit AutoLock(T &lock)
		: m_lock(lock)
	{
		m_lock.Lock();
	}
	virtual ~AutoLock()
	{
		m_lock.UnLock();
	}

private:
	AutoLock(const AutoLock &);
	AutoLock &operator=(const AutoLock &);
};

//自动加读锁
template<typename T>
class AutoRLock
{
private:
	T &m_lock;

public:
	explicit AutoRLock(T &lock)
		: m_lock(lock)
	{
		m_lock.readLock();
	}
	virtual ~AutoRLock()
	{
		m_lock.readUnLock();
	}

private:
	AutoRLock(const AutoRLock &);
	AutoRLock &operator=(const AutoRLock &);
};

//自动加写锁
template<typename T>
class AutoWLock
{
private:
	T &m_lock;

public:
	explicit AutoWLock(T & lock)
		: m_lock(lock)
	{
		m_lock.writeLock();
	}
	virtual ~AutoWLock()
	{
		m_lock.writeUnLock();
	}

private:
	AutoWLock(const AutoWLock &);
	AutoWLock &operator=(const AutoWLock &);
};
