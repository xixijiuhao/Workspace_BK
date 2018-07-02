
#define NO_WARN_MBCS_MFC_DEPRECATION

#include "trade_spi_ex.h"
#include "trade_spi_switch.h"
#include "IxFrm.h"

extern HINSTANCE g_hinst;
/******************************* TraderSpiWnd ********************************/
//TraderSpiWnd��CPP���������Է�ֹ���������
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
	//�Ƿ񴰿ڣ���ȷ���Ƿ���Ϣ�������߳�ִ�лص�
	virtual bool __cdecl IsWnd(uint & WinHwnd)
	{
		WinHwnd = (uint)m_Hwnd;
		return true;
	}
	
	//������Ϣ������Ϣ����·�ɵ�p_spi��Ӧ�����ݴ�����
	LRESULT OnTradeData(WPARAM wParam, LPARAM lParam)
	{
		return p_spi_switch->DoSwitch(p_spi, wParam, lParam) ? 1 : 0;
	}
};



/******************************* TradeSpiEx ********************************/
//�̳���TraderSpi������ע�ᵽTradeData�У�����ͨ��_TraderSpiWnd��ʵ�ֻص�
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