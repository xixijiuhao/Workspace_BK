#pragma once

#include "private_struct.h"
#include "trade_spi_switch.h"

using namespace std;

/********************************************* TTaskItem ********************************************/
//ʵ���������ݴ�������������
struct TTaskItem
{
	ITradeSpi *	p_notify;
	TTradeData	m_data;

private:
	enum TMsgRtnType
	{
		mrtPosted	,	//��ϢͶ�ݳɹ�
		mrtInvalid	,	//�Ƿ�������
		mrtPostFaild	//��ϢͶ��ʧ��
	};
	TraderSpiSwitch trade_switch;

	//����Ϣ��ʽ����������֪ͨ�����棬���������Ƿ��ѱ�����
	TMsgRtnType do_message()
	{
		if (p_notify == nullptr)
			return mrtInvalid;

		uint win_hwnd = 0;
		if (!p_notify->IsWnd(win_hwnd) || win_hwnd == 0 || HWND_BROADCAST == (HWND)win_hwnd)//�޾�����ǹ㲥��Ϣ
			return mrtPostFaild;

		//PostMessage�������̵߳�ִ�У�SendMessage��֮(���ڴ˴����ܵ�������)
		//�����SendMessage������Ҫ��WM_m_data��Ϣ�Ĵ�������ʼ������ if (InSendMessage())	ReplyMessage(lResult);
		//���жϣ��������Ϣ�Ƿ�������һ���̣߳������� RelpyMessage���ظ���Ϣ������lResult���Ƿ���ֵ��
		//windows��Ϣ�����������10000�ʣ�������ʱpost��ʧ�ܣ�����postʧ�ܺ���Ҫ�ȴ�������
		
		//��λ��һ���ֽ�Ϊ�������ͣ��ڶ����ֽ�Ϊ����λ����λ2���ֽ�Ϊ������
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