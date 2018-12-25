#pragma once

#include "_base.h"
#include <list>


template<typename _DATA>
class Datas: public IDatas<_DATA>
{
private:
	vector<_DATA> m_vector;
public:
	virtual ~Datas(){ m_vector.clear(); };
	void sort(int (* pfun)(_DATA & p1, _DATA & p2))
	{
		std::sort(m_vector.begin(), m_vector.end(), pfun);
	}
public:
	virtual uint __cdecl size()
	{
		return m_vector.size();
	}
	virtual _DATA __cdecl at(uint i)
	{
		return m_vector[i];
	}
	virtual void __cdecl swap(uint i, uint j)
	{
		std::swap(m_vector[i], m_vector[j]);
	}
	virtual void __cdecl clear()
	{
		m_vector.clear();
	}
	virtual void __cdecl push_back(_DATA data)
	{
		m_vector.push_back(data);
	};
	virtual bool __cdecl erase(uint i)
	{
		if (i >= m_vector.size())
			return false;

		vector<_DATA>::iterator iter = m_vector.begin();
		m_vector.erase(iter + i);
		return true;
	};
	_DATA operator[](const uint & ind)
	{
		return m_vector[ind];
	}
	
	void to_list(list<_DATA> & list)
	{
		uint i = 0;
		while (i < m_vector.size())
			list.push_back(m_vector[i++]);
	};
};

