#pragma once

#include "TradeApi_API.h"
#include <vector>

using namespace std;

typedef vector<const void *> VecVoidData;

class CDataContainer:public IDataContainer
{
public:
	//------------------------------樟創begin--------------------------------------//
	virtual uint __cdecl size()
	{
		return m_VecData.size();
	}
	virtual const void * __cdecl at(uint i)
	{
		return m_VecData.at(i);
	}
	virtual void __cdecl clear() 
	{
		m_VecData.clear();
	}
	virtual void __cdecl push_back(const void * data)
	{
		m_VecData.push_back(data);
	}
	//-------------------------------樟創end---------------------------------------//

	//------------------------------begin--------------------------------------//
	VecVoidData & GetData()
	{
		return m_VecData;
	}
	//-------------------------------end---------------------------------------//
public:
	VecVoidData m_VecData;
};

typedef std::vector<int> VecIntData;

class TVecInt :public IVecInt
{
public:
	//------------------------------樟創begin--------------------------------------//
	virtual unsigned int __cdecl size()
	{
		return m_VecData.size();
	}
	virtual int __cdecl at(unsigned int i)
	{
		return m_VecData.at(i);
	}
	virtual void __cdecl clear()
	{
		m_VecData.clear();
	}
	virtual void __cdecl push_back(int data)
	{
		m_VecData.push_back(data);
	}
	//-------------------------------樟創end---------------------------------------//

	//------------------------------begin--------------------------------------//
	VecIntData & GetData()
	{
		return m_VecData;
	}
	//-------------------------------end---------------------------------------//
public:
	VecIntData m_VecData;
};
