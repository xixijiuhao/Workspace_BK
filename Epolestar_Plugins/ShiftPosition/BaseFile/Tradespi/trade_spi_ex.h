#pragma once
	
#include "trade_spi.h"
#include "_trade_spi_switch.h"


//外部直接继承该类即可自动响应回调函数, 不用再处理WM_TradeData消息和IsWnd(uint &)函数了
//ITradeSpiSwitch用于重定义数据路由类，不传入则会使用默认的数据路由类TraderSpiSwitch
class TradeSpiEx:public TraderSpi
{
public:
	TradeSpiEx(ITradeSpiSwitch * spi_switch = nullptr);
	virtual ~TradeSpiEx();
private:
	void * p_trade_wnd;
	virtual bool __cdecl IsWnd(uint & WinHwnd){ return false; }
};

