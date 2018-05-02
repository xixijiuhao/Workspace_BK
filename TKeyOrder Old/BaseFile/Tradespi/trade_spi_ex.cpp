
#define NO_WARN_MBCS_MFC_DEPRECATION

#include "trade_spi_ex.h"
#include "trade_spi_switch.h"
#include "IxFrm.h"

extern HINSTANCE g_hinst;
/******************************* TraderSpiWnd ********************************/
//TraderSpiWnd在CPP里声明，以防止其他类访问
class _TraderSpiWnd : public TIxFrm, public TraderSpi
{
private:
	ITradeSpi *		p_spi;
	TraderSpiSwitch	def_spi_switch;
	ITradeSpiSwitch * p_spi_switch;

	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
			case WM_TradeData:
				return OnTradeData(wParam, lParam);
			default:
				break;
		}
		return -1;
	}
public:		
	_TraderSpiWnd(ITradeSpi * spi, ITradeSpiSwitch * spi_switch = nullptr)
		: p_spi(spi)
		, p_spi_switch(spi_switch == nullptr ? &def_spi_switch : spi_switch)
		, TraderSpi(true)
	{
		CreateFrm(L"TradeSpi_Ixfrm", NULL, 0, 0);
	}	
	virtual ~_TraderSpiWnd()
	{
		DestroyWindow(m_Hwnd);
	}
private:
	//是否窗口，以确定是发消息还是用线程执行回调
	virtual bool __cdecl IsWnd(uint & WinHwnd)
	{
		WinHwnd = (uint)m_Hwnd;
		return true;
	}
	
	//交易消息处理，消息过来路由到p_spi对应的数据处理函数
	LRESULT OnTradeData(WPARAM wParam, LPARAM lParam)
	{
		return p_spi_switch->DoSwitch(p_spi, wParam, lParam) ? 1 : 0;
	}
};



/******************************* TradeSpiEx ********************************/
//继承自TraderSpi，但不注册到TradeData中，而是通过_TraderSpiWnd来实现回调
TradeSpiEx::TradeSpiEx(ITradeSpiSwitch * spi_switch): TraderSpi(false), p_trade_wnd(nullptr)
{
 	p_trade_wnd = new _TraderSpiWnd(this, spi_switch);
}
TradeSpiEx::~TradeSpiEx()
{
	if (p_trade_wnd)
	{
		delete (_TraderSpiWnd*)p_trade_wnd;
		p_trade_wnd=nullptr;
	}
}