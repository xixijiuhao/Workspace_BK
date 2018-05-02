#pragma once
#include <windows.h> 
#include "TradeApi_API.h"

//实现交易数据回调的路由
class ITradeSpiSwitch
{
public:
	virtual bool __cdecl DoSwitch(ITradeSpi * spi, WPARAM wParam, LPARAM lParam) = 0;
};


//交易消息的LParam参数打包与解包
class TradeMsgFun
{
public:
	static LPARAM make_lparam(const int data_type, const int flag, const int error_code)
	{
		return (data_type << 24) |((flag << 24) >> 8) | (error_code & 0x0000FFFF);
	}
	static void parse_lparam(const LPARAM lparam, int & data_type, int & flag, int & error_code)
	{
		data_type	= lparam >> 24;
		flag		= (lparam << 8) >> 24;
		error_code	= lparam & 0x0000FFFF;
	}
};