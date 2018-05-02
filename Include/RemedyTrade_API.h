#ifndef _REMEDYTRADE_API_
#define _REMEDYTRADE_API_

#include "TradeApi_Struct.h"

const wchar_t PLUG_REMEDYTRADEAPI_NAME[] = L"RemedyTrade";
const wchar_t PLUG_REMEDYTRADEAPI_VERSION[] = L"9.3.0.1";

struct HWND__; typedef HWND__* HWND;

//定单操作类型
enum TDlgOperType
{
	dotInput,	//录单
	dotCarry,	//carry单
	dotModify,	//改单
	dotRemove,	//删单
	dotMove,	//移单
	dotMultiMove,//定单拆分
	dotExecute	//行权
};

typedef int ListDataType;
const ListDataType ldtOrder = 0;		//委托信息
const ListDataType ldtMatch = 2;		//成交信息


class IRemedyTrade
{
public:
	/**
	* 创建窗口函数，窗口销毁时自动销毁对象
	* @param OperType 定单操作类型
	* @param pData 数据类型指针 需要根据DataType的值强制转化为TMatch或者TOrder
	* @param DataType 数据类型 指定pData的类型
	* @param hParentWnd 父窗口句柄
	* @return
	*/
	virtual void __cdecl GetMatchObject(TDlgOperType OperType = dotInput, void * pData = nullptr, ListDataType DataType = ldtMatch, HWND hParentWnd = nullptr) = 0;

	/**
	* 创建窗口函数，窗口销毁时自动销毁对象
	* @param OperType 定单操作类型
	* @param pOrder 数据类型指针
	* @param hParentWnd 父窗口句柄
	* @return
	*/
	virtual void __cdecl GetOrderObject(TDlgOperType OperType = dotModify, TOrder * pOrder = nullptr, HWND hParentWnd = nullptr) = 0;
};

#endif