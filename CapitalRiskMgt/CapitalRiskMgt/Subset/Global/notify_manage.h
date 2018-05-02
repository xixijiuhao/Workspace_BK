//功  能：临时使用的数据通知线程
//创建人：刘明祥

#pragma once

#include "WinTools.h"
#include <set>
#include <queue>
#include "private_struct.h"
#include "task_data.h"

using namespace std;

//线程池改造方案：
//AddNotify时将notify注册到一个具体的通知线程上，DelNotify时再从这个线程上移除
//AddTask时将数据传到所有通知线程，每个线程通知自己下面的notify对象

//下面是临时的解决方案，只用一个线程，通知所有notify对象, 处理数据是一个无锁操作，内带消息聚合
/***************************************** NotifyManage ***************************************/
//回调接口管理
class NotifyManage: public TThread
{
private:	
	typedef queue<TTradeData>					queData;
	queData										m_datas_a;
	queData										m_datas_b;
	queData *									p_reads;
	queData *									p_writes;

	ScopeLock;
	ScopeLock1;

	typedef set<ITradeSpi *>					setNotify;
	setNotify									m_notifys;

	typedef set<uint64>							setKeys;
	setKeys										m_keys;
public:
	NotifyManage() : p_reads(&m_datas_a), p_writes(&m_datas_b)
	{		
		Start();	
	}

	virtual ~NotifyManage()
	{	
		Terminate();
	}

	void AddNotify(ITradeSpi * pnotify)
	{	
		LockScope1();
		//StrFun::trace("reg  : %08x", pnotify);
		m_notifys.insert(pnotify);
	}

	void DelNotify(ITradeSpi * pnotify)
	{	
		LockScope1();
		//StrFun::trace("unreg: %08x", pnotify);
		m_notifys.erase(pnotify);
	}

	inline void AddTask(const TTradeData & data)
	{	
		LockScope();
		p_writes->push(data);
	}	
private:
	void Run()
	{
		uint64 key;
		TTaskItem task;
		pair<setKeys::iterator, bool> inst;
		while (!isTerminated())		
		{
			if (p_writes->empty())
			{
				Sleep(1);
				continue;
			}
			else
			{
				LockScope();
				swap(p_writes, p_reads);
			}
						
			while (!p_reads->empty())
			{
				task.m_data = p_reads->front();
				p_reads->pop();

				if (task.m_data.can_aggregation())
				{
					key = task.m_data.data_type;
					inst = m_keys.insert(key << 32 | (uint)task.m_data.pdata);
					if (!inst.second)
						continue;
				}
					
				LockScope1();
				for (setNotify::iterator iter = m_notifys.begin(); iter != m_notifys.end(); iter++)
				{
					_ASSERT(*iter);
					task.p_notify = *iter;
					task.do_notify();
				}
			}//p_writes->empty
			m_keys.clear();
		}//isTerminated
	}
};