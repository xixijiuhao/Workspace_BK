#include "_wnd.h"
#include "trade_spi_ex.h"
#include "trade_spi_switch.h"

/******************************* TraderSpiWnd ********************************/
//TraderSpiWnd在CPP里声明，以防止其他类访问
class _TraderSpiWnd : public Wnd, public TraderSpi
{
private:
	ITradeSpi *		p_spi;
	TraderSpiSwitch	def_spi_switch;
	ITradeSpiSwitch * p_spi_switch;
public:		
	_TraderSpiWnd(ITradeSpi * spi, ITradeSpiSwitch * spi_switch = nullptr)
		: p_spi(spi)
		, p_spi_switch(spi_switch == nullptr ? &def_spi_switch : spi_switch)
		, TraderSpi(true)
	{		
		Wnd::Create(0, 0, "_TraderSpiWnd", ::GetDesktopWindow(), TRect(0,0,0,0));
	}		
	virtual ~_TraderSpiWnd()
	{
		p_spi_switch = nullptr;
		p_spi = nullptr;
	}	
private:
	virtual bool __cdecl IsWnd(uint & WinHwnd)
	{
		WinHwnd = (uint)m_hwnd;
		return true;
	}
protected:
	virtual LRESULT wnd_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
	{
		//交易消息处理，消息过来路由到p_spi对应的数据处理函数
		if (message == WM_TradeData && p_spi_switch != nullptr)
			return p_spi_switch->DoSwitch(p_spi, wparam, lparam) ? 1 : 0;
		return Wnd::wnd_proc(hwnd, message, wparam, lparam);
	}
};


/******************************* TradeSpiEx ********************************/
//继承自TraderSpi，但不注册到TradeData中，而是通过_TraderSpiWnd来实现回调
TradeSpiEx::TradeSpiEx(ITradeSpiSwitch * spi_switch) : TraderSpi(false), p_trade_wnd(nullptr)
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