#pragma once
	
#include "trade_spi.h"
#include "_trade_spi_switch.h"


//�ⲿֱ�Ӽ̳и��༴���Զ���Ӧ�ص�����, �����ٴ���WM_TradeData��Ϣ��IsWnd(uint &)������
//ITradeSpiSwitch�����ض�������·���࣬���������ʹ��Ĭ�ϵ�����·����TraderSpiSwitch
class TradeSpiEx:public TraderSpi
{
public:
	TradeSpiEx(ITradeSpiSwitch * spi_switch = nullptr);
	virtual ~TradeSpiEx();
private:
	void * p_trade_wnd;
	virtual bool __cdecl IsWnd(uint & WinHwnd){ return false; }
};

