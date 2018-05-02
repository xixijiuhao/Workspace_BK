/***************************************************************
PluginName:TradeReport //交易信息汇报


@Func		创建交易信息汇总操作资源
@param		v		版本号
@return 	void*	交易信息汇总操作指针 (ITradeReport *)
void* __cdecl CreatePluginApi(TPluginVersionType v);


@Func		释放交易信息汇总资源
@param		p		交易信息汇总操作指针（ITradeReport *）
@return		void
void __cdecl FreePluginApi(void* p);


UpdateTime:2015-1-12
version:
***************************************************************/
#pragma once
#include "TradeApi_Struct.h"
#include <string>

using namespace std;

const wchar_t PLUG_TRADEREPORT_NAME[] = L"TradeReport";	//插件名
const wchar_t PLUG_TRADEREPORT_VERSION[] = L"1.0.0.1";	//插件版本信息

typedef const TMatch * PMatchs[];
typedef const TOrder * POrders[];	
typedef const TPosition * PPositions[];

class ITradeReport
{
public:
	virtual bool __cdecl ExportLiquidate(const wchar_t * FileName, const int MaxRowOfPage=66) = 0;				//人工平仓检核表
	virtual bool __cdecl ExportMatch(const wchar_t * FileName, const int MaxRowOfPage = 66) = 0;					//上手成交单一览表
	virtual bool __cdecl ExportUpperLiquidate(const wchar_t * FileName, const bool bCVS = false, const bool bBase = false, const int MaxRowOfPage = 66) = 0;		//上手平仓核对表
	virtual  const char* __cdecl MatchTotal(PMatchs * pMatchs, int Count) = 0;								//上手成交汇总
	virtual const char* __cdecl PositionTotal(PPositions * pPositions, int Count) = 0;
	virtual const char* __cdecl PositionSubTotal(PPositions * pPositionSubs, int Count) = 0;
};

