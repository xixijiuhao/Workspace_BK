//��  �ܣ���ʱʹ�õ�����֪ͨ�߳�
//�����ˣ�������

#pragma once

#include "WinTools.h"
#include <set>
#include <queue>
#include "private_struct.h"
#include "task_data.h"

using namespace std;

//�̳߳ظ��췽����
//AddNotifyʱ��notifyע�ᵽһ�������֪ͨ�߳��ϣ�DelNotifyʱ�ٴ�����߳����Ƴ�
//AddTaskʱ�����ݴ�������֪ͨ�̣߳�ÿ���߳�֪ͨ�Լ������notify����

//��������ʱ�Ľ��������ֻ��һ���̣߳�֪ͨ����notify����, ����������һ�������������ڴ���Ϣ�ۺ�
/***************************************** NotifyManage ***************************************/
//�ص��ӿڹ���
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