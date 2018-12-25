//��  �ܣ�����غʹ����
//�����ˣ�������

#pragma once

#include "WinTools.h"
#include <queue>
#include <unordered_set>

using namespace std;

/***************************************** MachinePool ******************************************/
//���ȴ���������������ݵĳ�:
//1.�ؿ��Կ��ƴ�����ļ���(Resume)�͹���(suspend)���������ݶԳ���˵��͸���ģ������������ʶ����������
//2.һ����ֻ�ܰ���ͬһ�����͵��������ݺ��ܹ�ʶ���������ݵĴ����
//3.�ر�����һ��������������ص������߳��Լ��ĵ�ǰ״̬������ض�����е��ȡ��������(TaskFinnished)
//4.����������ʱ�ж����޿��д����������ȡ��һ�������伤���������������ݣ�
//�����жϴ���������Ƿ�ﵽ���ޣ���û�дﵽ�����򴴽��µĴ������ִ�д�����
//����������ӵ�����������Ŷӣ��ȴ�������������к�˳��ִ��
//5.��һ�������������һ����������ж�����������Ƿ��д�������������ȡ��һ�����ݲ����������ô��������
//���򽫸ô�������𣬲�������д�������У��ȴ�����������ʱʹ��
//_Machine��ʾ�������
template<class _Machine>class MachinePool
{
private:
	size_t				m_MaxMachineCount;	//����������
	size_t				m_MachineCount;		//��ǰ���������
	ScopeLock;
	queue<void *>		m_Tasks;			//����������ݶ���
	queue<_Machine *>	m_IdelMachines;		//���д��������
	size_t				m_SleepTick;

	bool				m_DataJoin;			//���ݺϲ�
	typedef unordered_set<uint64> taskSet;
	taskSet				m_DataKeys;


	//������ݵ��������
	void AddItem(void * pData, int DataSize)		
	{
		LockScope();

		void * pItem	= malloc(DataSize);
		memcpy_s(pItem, DataSize, pData, DataSize);

		//���ݾۺ�
		if (m_DataJoin)
		{
			uint64 key = _Machine::DataKey(pData);
			if (m_DataKeys.find(key) != m_DataKeys.end())
				return;
			m_DataKeys.insert(key);
		}

		//�������
		m_Tasks.push(pItem);		
	}
	//������д��������û�п����Ҵ��������û�г��������������򴴽��´���������򷵻ؿ�
	_Machine * GetMachine()							
	{
		LockScope();

		_Machine * pMachine=nullptr;

		if (! m_IdelMachines.empty())
		{
			pMachine = m_IdelMachines.front();			//��ÿ��д����
			m_IdelMachines.pop();
		}
		else if (m_MachineCount<m_MaxMachineCount)		
		{
			pMachine = new _Machine(this, m_SleepTick);	//�����´����
			m_MachineCount++;
		}

		return pMachine;
	}
public:
	MachinePool(int MaxMachineCount, size_t SleepTick = 0, bool DataJoin = false):m_MaxMachineCount(MaxMachineCount),m_MachineCount(0),m_SleepTick(SleepTick), m_DataJoin(DataJoin){}
	virtual ~MachinePool()
	{
		LockScope();

		//������д����
		_Machine * pMachine;
		while (!m_IdelMachines.empty())
		{
			pMachine = m_IdelMachines.front();
			m_IdelMachines.pop();
			delete pMachine;
		}

		//��������������
		void * pItem;
		while (!m_Tasks.empty())
		{
			pItem = m_Tasks.front();
			m_Tasks.pop();
			free(pItem);
		}
	}
	//������������뵽�������ֱ��ִ�У��������ݼ���������� 
	void AddTask(void * pData, int DataSize)
	{
		_Machine * pMachine = GetMachine();

		if (pMachine==nullptr)					
			AddItem(pData, DataSize);					//�������
		else
			pMachine->Resume(pData);
	}
	//������������ʱ���������������޴���������ݣ�����ֱ�Ӵ�����һ�����ݣ������������ж���
	void TaskFinnished(_Machine * pMachine)
	{
		LockScope();

		if (m_Tasks.empty())							//û�д��������ݣ��������������ж���
		{
			pMachine->suspend();
			m_IdelMachines.push(pMachine);
		}
		else											//�д��������ݣ���ֱ�Ӵ����������
		{
			void * pItem	= m_Tasks.front();

			if (m_DataKeys.size() > 0)
				m_DataKeys.erase(_Machine::DataKey(pItem));//ɾ���������ݾۺϵ����ݹؼ���

			m_Tasks.pop();
			pMachine->Resume(pItem);
			free(pItem);							   //���ﻹ���ͷź������Ժ��Ϊ����ػ��հ�
		}
	}
};
/********************************************* TaskThread ******************************************/
//1.�����ʵ��(�����߳�)��
//���������ʵ�ֵ�һ����Աm_pOwner����������Resume��suspend
//�ؿ��Խ��������ݴ��봦�����Resume����ʵ�ִ���������ݵĴ���
//��������������ݴ������֮���Լ�����ص�TaskFinnished�������Ա�ظ��ݴ������ִ��״̬���������Ӧ�ĵ���
//2._ThreadTask(�����߳�TaskThread���Խ��ܵ���������)��
//_ThreadTask���������ʾ�߳����ȼ������γ�Ա����Priority�ʹ������ݵĳ�Ա����void DoNotify()
template<class _ThreadTask>class TaskThread:public TThread
{
private:
	TEvent *					m_pEvent;
	_ThreadTask					m_Task;
	MachinePool<TaskThread> *	m_pOwner;
	size_t						m_sleep_tick;
public:	
	//���������������ָ���������ص�ָ��
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
	//�����߳����񣬲������߳�
	void Resume(void * pTask)	
	{
		if (pTask==nullptr)
			return;				//�մ��������߳��˳�
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
					m_Task.DoNotify();									//����ִ��	
				}
				__finally
				{
					if (m_sleep_tick > 0)
						Sleep(m_sleep_tick);

					if (m_pOwner)
						m_pOwner->TaskFinnished(this);					//����ִ����ϣ�����������ȥ����
					else
						suspend();
				}
			}
	}
};