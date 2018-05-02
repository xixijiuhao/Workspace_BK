#pragma once

#include "TradeApi_API.h"
#include "BatchOrder_Struct.h"
#define WM_BATCHORDER_CHANGED			(WM_USER+1011)

//�ò������IBatchOrder�ӿ�
const wchar_t PLUG_BATCHORDER_NAME[] = L"BatchOrder";		//�����
const wchar_t PLUG_BATCHORDER_VERSION[] = L"1.0.0.1";	//����汾��Ϣ

class IVecInt;

class ISendOrder
{
public:
	//�ⲿ��д�ú���ʵ���µ�
	virtual bool __cdecl SendBatchOrder(TSendOrder* order, IVecInt& nVectId) = 0;		//�����µ��ӿ�
	//
	virtual void __cdecl OnNotify() = 0;
};


//�����µ����ܽӿ�
//���øýӿ�֮ǰ����������ISendOrder
class IBatchOrder
{
public:
	//�����µ��ӿ�
	virtual void __cdecl SetSendOrder(ISendOrder* pOrder) = 0;

	//�����µ����ýӿ�
	//TSendOrder Ҫ�����µĶ��� UserNo ��GroupNo
	virtual	int __cdecl Order(TSendOrder * pOrder,bool bRecove = false) = 0;

	//�������������û��ִ������������ֱ�Ӵ���ñʳ���
	virtual	int __cdecl Cancel(TOrder * pOrder) = 0;

	//ֻ�ļ۸����¼۸����£����û��ִ������������ֱ�Ӵ���ñ��޸�
	virtual int __cdecl Modify(TOrder * pOrder) = 0;

	//���������µ������Ƿ����
	virtual void  __cdecl SetActive(bool bActive) = 0;

	//��������µ������Ƿ����
	virtual bool  __cdecl GetActive() = 0;

	//��ȡ����������Ϣ
	virtual vctGroup & GetGroup() = 0;

	//���������µ����öԻ���
	virtual void CreateBatchOrderDlg(HWND hParentWnd) = 0;

	//���������µ����öԻ���
	virtual HWND CreateBatchSetWnd(HWND hParentWnd) = 0;

	//
	virtual HWND GetHwnd() = 0;
};
