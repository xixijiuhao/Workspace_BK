#pragma once

#include "private_struct.h"
#include "trade_spi_switch.h"

using namespace std;

/********************************************* TTaskItem ********************************************/
//实现任务数据处理方法的数据体
struct TTaskItem
{
	ITradeSpi *	p_notify;
	TTradeData	m_data;

private:
	enum TMsgRtnType
	{
		mrtPosted	,	//消息投递成功
		mrtInvalid	,	//非法订阅者
		mrtPostFaild	//消息投递失败
	};
	TraderSpiSwitch trade_switch;

	//以消息形式将交易数据通知到界面，返回数据是否已被处理
	TMsgRtnType do_message()
	{
		if (p_notify == nullptr)
			return mrtInvalid;

		uint win_hwnd = 0;
		if (!p_notify->IsWnd(win_hwnd) || win_hwnd == 0 || HWND_BROADCAST == (HWND)win_hwnd)//无句柄或是广播消息
			return mrtPostFaild;

		//PostMessage阻塞本线程的执行，SendMessage则反之(用在此处可能导致死锁)
		//如果用SendMessage，则需要在WM_m_data消息的处理函数开始处加上 if (InSendMessage())	ReplyMessage(lResult);
		//即判断：如果该消息是发自另外一个线程，则立即 RelpyMessage，回复消息，参数lResult即是返回值。
		//windows消息队列最大容量10000笔，队列满时post会失败，所以post失败后需要等待并重试
		
		//高位第一个字节为数据类型，第二个字节为附加位，低位2个字节为错误码
		while (!::PostMessage((HWND)win_hwnd, WM_TradeData, reinterpret_cast<WPARAM>(m_data.pdata),
			TradeMsgFun::make_lparam(m_data.data_type, m_data.is_last, m_data.error_id)))		
		{
			Sleep(10);
			StrFun::trace("trade msg queue filled, sleep 10, hwnd:%08X datatype:%d", win_hwnd, m_data.data_type);
		}

		return mrtPosted;
	}
public:
	TTaskItem() : trade_switch(false){}
	inline void do_notify()
	{ 
		if (do_message() == mrtPostFaild)
			trade_switch.DoSwitch(p_notify, reinterpret_cast<WPARAM>(m_data.pdata), TradeMsgFun::make_lparam(m_data.data_type, m_data.is_last, m_data.error_id));
	}
};