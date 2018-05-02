//单例模版

#pragma once

//#include "wintools.h"

template <class CLASS>
class Singleton 
{
private:	
	static CLASS* m_pInstance;	
protected:
	Singleton(){};
	Singleton(const Singleton&);
	Singleton& operator = (const Singleton&);

	//static TCriticalSection m_CritSec;
public:
  //  template<typename... Args>
	static inline CLASS& GetInstance()//GetInstance(Args... args)
	{	
		if (m_pInstance==nullptr)
		{
			//LockScope();	
			//if (m_pInstance==nullptr)
				m_pInstance = new CLASS();//new CLASS(args...)	//有传入参数时怎么做 C++0x模版可以有变长参数，可惜MS不支持	
		}
		return *m_pInstance;
	}
};

template <class CLASS>
CLASS* Singleton<CLASS>::m_pInstance=nullptr;

//template <class CLASS>
//TCriticalSection Singleton<CLASS>::m_CritSec;