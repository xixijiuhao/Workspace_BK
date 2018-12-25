//功  能：分拣处理池
//创建人：刘明祥

#pragma once

#include "WinTools.h"
#include <queue>
#include <unordered_map>
#include <list>

using namespace std;


/********************************************* SortingMachinePool *********************************************/
//分拣处理池，分类处理，相同关键字的数据用相同的处理机处理，每个处理机都有独立的处理队列，适合处理有严格时序性的数据
//数据到来时根据数据的任务关键字，查看是否已经对其分配了处理机，如果已经分配了，则直接将数据加入该处理机中，
//否则，如果处理机总数没有达到允许创建的最大处理机数量，则创建创建一个新处理机，并将其添加到任务-处理机映射表和处理机列表中，
//如果处理机数量已经达到允许创建的最大处理机数量，则不再创建新的处理机，而是直接取处理机队列的第一个处理机，
//并将其添加到任务-处理机映射表，同时将该处理机从处理机队列的队头移到队尾（据此均衡负载）。
template<class _Machine>class SortingMachinePool
{
private:
	uint							m_MaxMachineCount;	//允许创建的最大处理机数量
	unordered_map<uint, _Machine *> m_MachinesMap;		//任务-处理机映射表
	list<_Machine *>				m_Machines;			//处理机列表
	
	ScopeLock;

	//申请空闲处理机，如没有空闲且处理机总数没有超过最大处理机数量则创建新处理机，否则返回空
	_Machine * GetMachine(void * pData)							
	{
		LockScope();
		uint key = _Machine::TaskKey(pData);
		_Machine * pDef = nullptr;
		_Machine * pMachine = (_Machine *)StlFun::MapFindOnly(m_MachinesMap, key, pDef) ;
		if (pMachine == nullptr)
		{
			if (m_Machines.size() < m_MaxMachineCount)
				pMachine = new _Machine();				//创建新处理机
			else
			{
				pMachine = m_Machines.front();				
				m_Machines.pop_front();
			}
			m_Machines.push_back(pMachine);
			StlFun::CiteToMap(m_MachinesMap, pMachine, key);
		}
		return pMachine;
	}
public:
	SortingMachinePool(int MaxMachineCount):m_MaxMachineCount(MaxMachineCount > 0 ? MaxMachineCount : 1)
	{ 
	}
	virtual ~SortingMachinePool()
	{
		LockScope();
		//清除处理机
		m_MachinesMap.clear();
		_Machine * pMachine;
		while (!m_Machines.empty())
		{
			pMachine = m_Machines.back();
			m_Machines.pop_back();
			delete pMachine;
		}

	}
	//添加任务：能申请到处理机则直接执行，否则将数据加入任务队列 
	void AddTask(void * pData, int DataSize)
	{
		_Machine * pMachine = GetMachine(pData);
		Assert(pMachine);
		pMachine->AddData(pData);
	}
};

/********************************************* TaskQueueThread ******************************************/
//处理机线程, 自带任务队列
template<class _ThreadTask>class TaskQueueThread: public TThread
{
private:
	ScopeLock;
	TEvent *					m_pEvent;
	queue<_ThreadTask>			m_Tasks;			//待处理的数据队列
	typedef unordered_map<uint64, _ThreadTask*> taskMap;
	taskMap						m_DataKeys;
public:	
	//创建处理机，传入指向其所属池的指针
	TaskQueueThread()
	{
		m_pEvent=new TEvent(true, false);
		Start();
	}
	virtual ~TaskQueueThread()
	{
		Terminate();
		m_DataKeys.clear();
		delete m_pEvent;
	}
	static uint TaskKey(void * pTask)
	{
		return ((_ThreadTask *)pTask)->TaskKey();
	}
	//设置线程任务，并激活线程
	void AddData(void * pTask)	
	{
		LockScope();

		if (isTerminated() || pTask==nullptr)
			return;

		{

			//数据聚合
			_ThreadTask * p = (_ThreadTask *)pTask;
			uint64 key = p->DataKey();
			taskMap::iterator it = m_DataKeys.find(key);
			if (it != m_DataKeys.end())
			{
#ifdef _DEBUG
				StrFun::trace("The data has been in existence: %08X ", key);
#endif
				//*it->second = *p;
				return;
			}

			//数据入队
			m_Tasks.push(*p);
			m_DataKeys.insert(taskMap::value_type(key, &m_Tasks.back()));
		}
		m_pEvent->Set();
	};
private:
	void Run()
	{
		while (!isTerminated())
			if (m_pEvent->Wait(INFINITE))	
			{
				{
					LockScope();
					while (!m_Tasks.empty())
					{
						_ThreadTask & task = m_Tasks.front();
						task.DoNotify();
						m_DataKeys.erase(task.DataKey());
						m_Tasks.pop();
					}
				m_pEvent->Reset();
				}
				//Sleep(10);
			}
	}
};