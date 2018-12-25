#pragma once

#ifndef _TRADEAPI_TYPE_
typedef signed long long		int64;
typedef unsigned char			uchar;
typedef unsigned short			ushort;
typedef unsigned long int		uint;
typedef unsigned long long		uint64;
#else
#include "TradeApi_API.h"
#endif


/******************************************************************************/
//结构体初始化
template<class RECORD>  
static void FillZero(RECORD * pRecord)
{
	memset(pRecord, 0, sizeof(RECORD));
}
//结构体初始化
#define _InitStruct(Record) Record(){ FillZero(this); }

/******************************************************************************/
//数据数组可以以模板类接口方式返回
template<typename _DATA>
class IDatas
{
public:
	virtual uint __cdecl size() = 0;
	virtual _DATA __cdecl at(uint i) = 0;
	virtual void __cdecl clear() = 0;
	virtual void __cdecl push_back(_DATA data) = 0;
};

//数组长度
#define ARR_LEN(arr)	(sizeof(arr) == 0 ? 0 : sizeof(arr) / sizeof(arr[0]))