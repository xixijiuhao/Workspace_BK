#include "any.h"


void swap(any & lhs, any & rhs)
{
	lhs.swap(rhs);
}

//��ȡm_pcontent->helder���ݵķ�����������ȡ����������  
template<typename _ty> _ty any_cast(const any & _any)  
{  
	return _any.value<_ty>();  
}