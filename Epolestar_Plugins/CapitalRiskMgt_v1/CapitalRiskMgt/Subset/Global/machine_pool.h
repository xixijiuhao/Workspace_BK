//功  能：处理池和处理机
//创建人：刘明祥

#pragma once

#include "WinTools.h"
#include <queue>
#include <unordered_set>

using namespace std;

/***************************************** MachinePool ******************************************/
//调度处理机及待处理数据的池:
//1.池可以控制处理机的激活(Resume)和挂起(suspend)，任务数据对池来说是透明的，但处理机可以识别任务数据
//2.一个池只能包含同一种类型的任务数据和能够识别这类数据的处理机
//3.池必须有一个函数供处理机回调，告诉池自己的当前状态，方便池对其进行调度。这里就是(TaskFinnished)
//4.增加新任务时判断有无空闲处理机，有则取出一个并将其激活用来处理新数据，
//否则判断处理机总数是否达到上限，如没有达到上限则创建新的处理机来执行此任务，
//否则将新任务加到任务队列中排队，等待其他处理机空闲后顺序执行
//5.当一个处理机处理完一单任务后，则判断任务队列中是否还有待处理任务，有则取出一笔数据并将其分配给该处理机处理，
//否则将该处理机挂起，并加入空闲处理机队列，等待增加新任务时使用
//_Machine表示处理机类
template<class _Machine>class MachinePool
{
private:
	size_t				m_MaxMachineCount;	//最大处理机数量
	size_t				m_MachineCount;		//当前处理机数量
	ScopeLock;
	queue<void *>		m_Tasks;			//待处理的数据队列
	queue<_Machine *>	m_IdelMachines;		//空闲处理机队列
	size_t				m_SleepTick;

	bool				m_DataJoin;			//数据合并
	typedef unordered_set<uint64> taskSet;
	taskSet				m_DataKeys;


	//添加数据到任务队列
	void AddItem(void * pData, int DataSize)		
	{
		LockScope();

		void * pItem	= malloc(DataSize);
		memcpy_s(pItem, DataSize, pData, DataSize);

		//数据聚合
		if (m_DataJoin)
		{
			uint64 key = _Machine::DataKey(pData);
			if (m_DataKeys.find(key) != m_DataKeys.end())
				return;
			m_DataKeys.insert(key);
		}

		//数据入队
		m_Tasks.push(pItem);		
	}
	//申请空闲处理机，如没有空闲且处理机总数没有超过最大处理机数量则创建新处理机，否则返回空
	_Machine * GetMachine()							
	{
		LockScope();

		_Machine * pMachine=nullptr;

		if (! m_IdelMachines.empty())
		{
			pMachine = m_IdelMachines.front();			//获得空闲处理机
			m_IdelMachines.pop();
		}
		else if (m_MachineCount<m_MaxMachineCount)		
		{
			pMachine = new _Machine(this, m_SleepTick);	//创建新处理机
			m_MachineCount++;
		}

		return pMachine;
	}
public:
	MachinePool(int MaxMachineCount, size_t SleepTick = 0, bool DataJoin = false):m_MaxMachineCount(MaxMachineCount),m_MachineCount(0),m_SleepTick(SleepTick), m_DataJoin(DataJoin){}
	virtual ~MachinePool()
	{
		LockScope();

		//清除空闲处理机
		_Machine * pMachine;
		while (!m_IdelMachines.empty())
		{
			pMachine = m_IdelMachines.front();
			m_IdelMachines.pop();
			delete pMachine;
		}

		//清除待处理的数据
		void * pItem;
		while (!m_Tasks.empty())
		{
			pItem = m_Tasks.front();
			m_Tasks.pop();
			free(pItem);
		}
	}
	//添加任务：能申请到处理机则直接执行，否则将数据加入任务队列 
	void AddTask(void * pData, int DataSize)
	{
		_Machine * pMachine = GetMachine();

		if (pMachine==nullptr)					
			AddItem(pData, DataSize);					//数据入队
		else
			pMachine->Resume(pData);
	}
	//处理机完成任务时检查任务队列中有无待处理的数据，有则直接处理下一笔数据，无则将其加入空闲队列
	void TaskFinnished(_Machine * pMachine)
	{
		LockScope();

		if (m_Tasks.empty())							//没有待处理数据，则将其挂起后加入空闲队列
		{
			pMachine->suspend();
			m_IdelMachines.push(pMachine);
		}
		else											//有待处理数据，则直接处理该条数据
		{
			void * pItem	= m_Tasks.front();

			if (m_DataKeys.size() > 0)
				m_DataKeys.erase(_Machine::DataKey(pItem));//删除用于数据聚合的数据关键字

			m_Tasks.pop();
			pMachine->Resume(pItem);
			free(pItem);							   //这里还有释放函数，以后改为对象池回收吧
		}
	}
};
/********************************************* TaskThread ******************************************/
//1.处理机实例(任务线程)：
//处理机必须实现的一个成员m_pOwner和两个函数Resume、suspend
//池可以将任务数据传入处理机的Resume函数实现处理机对数据的处理
//处理机将任务数据处理完毕之后将自己传入池的TaskFinnished函数，以便池根据处理机的执行状态对其进行相应的调度
//2._ThreadTask(任务线程TaskThread可以接受的数据类型)：
//_ThreadTask必须包含表示线程优先级的整形成员变量Priority和处理数据的成员方法void DoNotify()
template<class _ThreadTask>class TaskThread:public TThread
{
private:
	TEvent *					m_pEvent;
	_ThreadTask					m_Task;
	MachinePool<TaskThread> *	m_pOwner;
	size_t						m_sleep_tick;
public:	
	//创建处理机，传入指向其所属池的指针
	TaskThread(MachinePool<TaskThread> * pOwner = nullptr, size_t sleep_tick = 0) :m_pOwner(pOwner), m_sleep_tick(sleep_tick)
	{
		m_pEvent=new TEvent(true, false);
		Start();
	}
	virtual ~TaskThread()
	{
		Terminate();
		delete m_pEvent;
	}
	//设置线程任务，并激活线程
	void Resume(void * pTask)	
	{
		if (pTask==nullptr)
			return;				//空处理，方便线程退出
		m_Task = *(_ThreadTask *)pTask;

		m_pEvent->Set();
	};
	void suspend()	{	m_pEvent->Reset();	}

	static uint64 DataKey(void * pTask)
	{
		return ((_ThreadTask *)pTask)->DataKey();
	}
private:
	void Run()
	{
		while (! isTerminated())
			if (m_pEvent->Wait(INFINITE))								
			{
				__try
				{
					m_Task.DoNotify();									//任务执行	
				}
				__finally
				{
					if (m_sleep_tick > 0)
						Sleep(m_sleep_tick);

					if (m_pOwner)
						m_pOwner->TaskFinnished(this);					//任务执行完毕，交给所属池去调度
					else
						suspend();
				}
			}
	}
};