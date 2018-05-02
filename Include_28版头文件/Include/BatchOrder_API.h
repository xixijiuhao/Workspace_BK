#pragma once

#include "TradeApi_API.h"
#include "BatchOrder_Struct.h"
#define WM_BATCHORDER_CHANGED			(WM_USER+1011)

//该插件导出IBatchOrder接口
const wchar_t PLUG_BATCHORDER_NAME[] = L"BatchOrder";		//插件名
const wchar_t PLUG_BATCHORDER_VERSION[] = L"1.0.0.1";	//插件版本信息

class IVecInt;

class ISendOrder
{
public:
	//外部重写该函数实现下单
	virtual bool __cdecl SendBatchOrder(TSendOrder* order, IVecInt& nVectId) = 0;		//批量下单接口
	//
	virtual void __cdecl OnNotify() = 0;
};


//批量下单功能接口
//调用该接口之前必须先设置ISendOrder
class IBatchOrder
{
public:
	//设置下单接口
	virtual void __cdecl SetSendOrder(ISendOrder* pOrder) = 0;

	//批量下单调用接口
	//TSendOrder 要批量下的订单 UserNo 填GroupNo
	virtual	int __cdecl Order(TSendOrder * pOrder,bool bRecove = false) = 0;

	//批量撤单，如果没有执行批量操作则直接处理该笔撤单
	virtual	int __cdecl Cancel(TOrder * pOrder) = 0;

	//只改价格，以新价格重下，如果没有执行批量操作则直接处理该笔修改
	virtual int __cdecl Modify(TOrder * pOrder) = 0;

	//设置批量下单功能是否可用
	virtual void  __cdecl SetActive(bool bActive) = 0;

	//获得批量下单功能是否可用
	virtual bool  __cdecl GetActive() = 0;

	//获取比例分组信息
	virtual vctGroup & GetGroup() = 0;

	//创建批量下单设置对话框
	virtual void CreateBatchOrderDlg(HWND hParentWnd) = 0;

	//创建批量下单设置对话框
	virtual HWND CreateBatchSetWnd(HWND hParentWnd) = 0;

	//
	virtual HWND GetHwnd() = 0;
};
