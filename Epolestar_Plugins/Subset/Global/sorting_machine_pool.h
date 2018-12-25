//��  �ܣ��ּ����
//�����ˣ�������

#pragma once

#include "WinTools.h"
#include <queue>
#include <unordered_map>
#include <list>

using namespace std;


/********************************************* SortingMachinePool *********************************************/
//�ּ���أ����ദ����ͬ�ؼ��ֵ���������ͬ�Ĵ��������ÿ����������ж����Ĵ�����У��ʺϴ������ϸ�ʱ���Ե�����
//���ݵ���ʱ�������ݵ�����ؼ��֣��鿴�Ƿ��Ѿ���������˴����������Ѿ������ˣ���ֱ�ӽ����ݼ���ô�����У�
//����������������û�дﵽ���������������������򴴽�����һ���´��������������ӵ�����-�����ӳ���ʹ�����б��У�
//�������������Ѿ��ﵽ�����������������������ٴ����µĴ����������ֱ��ȡ��������еĵ�һ���������
//��������ӵ�����-�����ӳ���ͬʱ���ô�����Ӵ�������еĶ�ͷ�Ƶ���β���ݴ˾��⸺�أ���
template<class _Machine>class SortingMachinePool
{
private:
	uint							m_MaxMachineCount;	//������������������
	unordered_map<uint, _Machine *> m_MachinesMap;		//����-�����ӳ���
	list<_Machine *>				m_Machines;			//������б�
	
	ScopeLock;

	//������д��������û�п����Ҵ��������û�г��������������򴴽��´���������򷵻ؿ�
	_Machine * GetMachine(void * pData)							
	{
		LockScope();
		uint key = _Machine::TaskKey(pData);
		_Machine * pDef = nullptr;
		_Machine * pMachine = (_Machine *)StlFun::MapFindOnly(m_MachinesMap, key, pDef) ;
		if (pMachine == nullptr)
		{
			if (m_Machines.size() < m_MaxMachineCount)
				pMachine = new _Machine();				//�����´����
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
		//��������
		m_MachinesMap.clear();
		_Machine * pMachine;
		while (!m_Machines.empty())
		{
			pMachine = m_Machines.back();
			m_Machines.pop_back();
			delete pMachine;
		}

	}
	//������������뵽�������ֱ��ִ�У��������ݼ���������� 
	void AddTask(void * pData, int DataSize)
	{
		_Machine * pMachine = GetMachine(pData);
		Assert(pMachine);
		pMachine->AddData(pData);
	}
};

/********************************************* TaskQueueThread ******************************************/
//������߳�, �Դ��������
template<class _ThreadTask>class TaskQueueThread: public TThread
{
private:
	ScopeLock;
	TEvent *					m_pEvent;
	queue<_ThreadTask>			m_Tasks;			//����������ݶ���
	typedef unordered_map<uint64, _ThreadTask*> taskMap;
	taskMap						m_DataKeys;
public:	
	//���������������ָ���������ص�ָ��
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
	//�����߳����񣬲������߳�
	void AddData(void * pTask)	
	{
		LockScope();

		if (isTerminated() || pTask==nullptr)
			return;

		{

			//���ݾۺ�
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

			//�������
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