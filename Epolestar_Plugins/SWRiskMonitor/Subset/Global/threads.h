//��  �ܣ�һЩ���õ��߳�����
//�����ˣ�������

#pragma once

#include "WinTools.h"
#include "global.h"
#include "mem_pool.h"
#include <queue>
#include <map>

using namespace std;


/************************************************ ThreadEx ****************************************************/
//�ɷ�������ⲿ���𼤻���߳�
class ThreadEx : public TThread
{
private:
	TEvent				m_total_event;	//�ܿ��¼��������ṩ�ⲿ���õĿ����¼�
	bool				m_sleep_tag;
protected:
	size_t				m_sleep;
public:
	ThreadEx(size_t sleep) : m_total_event(true, false), m_sleep_tag(false), m_sleep(sleep){}
	virtual ~ThreadEx()
	{	
		suspend();
		Terminate();
	}
	inline void resum()
	{
		m_total_event.Set();
	}
	inline void suspend()
	{
		m_total_event.Reset();
		if (m_sleep > 0)
			m_sleep_tag = true;
	}
private:	
	inline void do_sleep()
	{		
		if (m_sleep > 0)
			Sleep(m_sleep);
	}
	void Run()
	{	
		while (!isTerminated())
		{	
			do_sleep();
			//����Ƕ�ʱ�̣߳����������¼����ĵ�һ���������
			if (m_total_event.Wait(INFINITE) && !m_sleep_tag)	
				do_run();
			m_sleep_tag = false;
		}
	}
protected:
	virtual void do_run() = 0;
};

/********************************************* BaseSleepThread *************************************************/
//BaseSleepThread�ص��Ľӿ�
class IThreadTimer
{
public:
	virtual void on_timer(const void * psender) = 0;
};

//�������ⲿ���𼤻���̶߳�ʱ��
class BaseSleepThread : public ThreadEx
{
private:
	IThreadTimer *		p_on_timer;
public:
	BaseSleepThread(IThreadTimer * on_timer, size_t sleep = 5000) 
		: ThreadEx(sleep)
		, p_on_timer(on_timer)	
	{
		if (p_on_timer) Start();
	}
	void set_sleep_idle(size_t sleep = 5000)
	{
		if (sleep == m_sleep)
			return;
		if (sleep <= 0)
			sleep = 1;

		::InterlockedExchange(&m_sleep, sleep);
	}
private:	
	virtual void do_run()
	{		
		if (p_on_timer)
			p_on_timer->on_timer(this);
	}
};


/********************************************** QueueDealThread ************************************************/
//�����ⲿ���𡢼�������ݴ����̣߳�sleep��idle��Ϊ��ʱ��Ϊʵʱ�����̣߳�������Ƕ�ʱ�����߳�
//�洢�ṹΪqueue
template<typename _DATA>
class IDealDataEx
{
public:
	virtual void on_idle() = 0;
	virtual void on_deal_data(_DATA & data, const void * psender) = 0;
};
template<typename _DATA>
class QueueDealThread : public ThreadEx
{
protected:
	typedef queue<_DATA> queData;
	queData				m_datas_a;
	queData				m_datas_b;
	queData *			p_reads;
	queData *			p_writes;

	ScopeLock;
	
	size_t				m_idle;			//�������ݴ���������ֹCPUռ�ù���

	IDealDataEx<_DATA> * p_deal_class;
public:
	QueueDealThread(IDealDataEx<_DATA> * pdeal, size_t sleep = 1000, size_t idle = 0) 
		: ThreadEx(sleep)
		, p_deal_class(pdeal)
		, m_idle(idle)
		, p_reads(&m_datas_a)
		, p_writes(&m_datas_b)
	{
		if (p_deal_class) Start();
	}
	void set_sleep_idle(size_t sleep = 1000, size_t idle = 0)
	{
		if (sleep != m_sleep)
			::InterlockedExchange(&m_sleep, sleep);
		if (idle != m_idle)
			::InterlockedExchange(&m_idle, idle);
	}
	void add_data(_DATA & data)					//����Ҫ���������
	{
		if (p_deal_class == nullptr)
			return;

		LockScope();
		p_writes->push(data);
	}
private:
	virtual void do_run()
	{
		if (p_writes->empty())
		{
			Sleep(10);
			return p_deal_class->on_idle();;
		}
		else
		{
			LockScope();
			swap(p_writes, p_reads);
		}

		while (!p_reads->empty())
		{
			p_deal_class->on_deal_data(p_reads->front(), this);
			p_reads->pop();

			if (m_idle > 0)
				Sleep(m_idle);
		}
	}
};

/**************************************** RecycleThread **************************************/
//��Ч���ݻ����̣߳���ʱ�ͷ�
class RecycleThread: public ThreadEx
{
private:
	typedef pair<int, void *> TItemType;
	list<TItemType>		m_list;

	ScopeLock;	
	int					m_init_life;		//��ʼ����ֵ
public:
	RecycleThread(int init_life, size_t sleep = 5000)
		: ThreadEx(sleep)
		, m_init_life(init_life)
	{}
	void push_back(void * pdata)
	{
		LockScope();
		m_list.push_back(TItemType(m_init_life, pdata));

		if (m_list.size() == 1)
			ThreadEx::resum();
	}
private:	
	virtual void do_run()
	{		
		LockScope();
		list<TItemType>::iterator iter = m_list.begin();
		while (iter != m_list.end())
		{
			iter->first--;
			if (iter->first <= 0)
			{
				if (iter->second != nullptr)
					delete iter->second;
				m_list.erase(iter);
			}
		}

		if (m_list.empty())
			ThreadEx::suspend();
	}
};

/**************************************** DealThead **************************************/
//DealThead�߳�Ҫִ�еĽӿ�
class IDealBuf
{
public:
	virtual void on_deal_buf(const void * buf, int buf_size, const void * sender) = 0;
};
//�������ݴ����̣߳���˫����ʵ�������Ķ��ߴ�����д��֮��ʵ���첽����
class BufDealThread : public TThread
{
private:
	uint				m_write_thread_id;
	TCriticalSection	m_lock;

	IDealBuf *			p_deal_class;

	MemQueue			m_queue_a;
	MemQueue *			p_reads;

	MemQueue			m_queue_b;
	MemQueue *			p_writes;
public:
	BufDealThread(IDealBuf * pdeal) : p_reads(&m_queue_a), p_writes(&m_queue_b), p_deal_class(pdeal), m_write_thread_id(0)
	{
		if (p_deal_class != nullptr)
			Start();
	}
	virtual ~BufDealThread()
	{
		Terminate();
	}
	void add_data(const void * buf, int size)
	{
		TCriticalSection::Lock lock(m_lock);
		p_writes->push(buf, size);
	}

	//��һ���ⲿ�߳���ĵ���˳���ȵ���һ��begin_write���ٵ��ö��write��������һ��end_write�������������һ�����ݵ�д��
	void begin_write(int size)
	{		
		m_lock.Enter();
		m_write_thread_id = GetCurrentThreadId();
		p_writes->write_head(size);
	}
	bool write(const void * buf, int size)
	{
		if (GetCurrentThreadId() != m_write_thread_id)
			return false;

		p_writes->write_data(buf, size);
		return true;
	}
	void end_write()
	{	
		if (GetCurrentThreadId() != m_write_thread_id)
			return;
		m_write_thread_id = 0;
		m_lock.Leave();
	}
private:
	void Run()
	{
		int buf_size;
		char * pdata;
		while (!isTerminated())
		{
			if (p_writes->empty())
			{
				Sleep(1);
				continue;
			}
			else
			{
				TCriticalSection::Lock lock(m_lock);
				swap(p_writes, p_reads);
			}

			//�������ݣ���������һֱ����
			pdata = p_reads->front(buf_size);
			while (pdata && buf_size && p_deal_class)
			{
				p_deal_class->on_deal_buf(pdata, buf_size, this);	
				p_reads->pop();

				pdata = p_reads->front(buf_size);
			}
		}
	}
};



//#include <concrt.h>
//(concurrency::Context::Yield)();
