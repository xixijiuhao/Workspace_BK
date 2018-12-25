#include "any.h"


void swap(any & lhs, any & rhs)
{
	lhs.swap(rhs);
}

//获取m_pcontent->helder数据的方法。用来获取真正的数据  
template<typename _ty> _ty any_cast(const any & _any)  
{  
	return _any.value<_ty>();  
}