/***************************************************************
PluginName:TradeReport //������Ϣ�㱨


@Func		����������Ϣ���ܲ�����Դ
@param		v		�汾��
@return 	void*	������Ϣ���ܲ���ָ�� (ITradeReport *)
void* __cdecl CreatePluginApi(TPluginVersionType v);


@Func		�ͷŽ�����Ϣ������Դ
@param		p		������Ϣ���ܲ���ָ�루ITradeReport *��
@return		void
void __cdecl FreePluginApi(void* p);


UpdateTime:2015-1-12
version:
***************************************************************/
#pragma once
#include "TradeApi_Struct.h"
#include <string>

using namespace std;

const wchar_t PLUG_TRADEREPORT_NAME[] = L"TradeReport";	//�����
const wchar_t PLUG_TRADEREPORT_VERSION[] = L"1.0.0.1";	//����汾��Ϣ

typedef const TMatch * PMatchs[];
typedef const TOrder * POrders[];	
typedef const TPosition * PPositions[];

class ITradeReport
{
public:
	virtual bool __cdecl ExportLiquidate(const wchar_t * FileName, const int MaxRowOfPage=66) = 0;				//�˹�ƽ�ּ�˱�
	virtual bool __cdecl ExportMatch(const wchar_t * FileName, const int MaxRowOfPage = 66) = 0;					//���ֳɽ���һ����
	virtual bool __cdecl ExportUpperLiquidate(const wchar_t * FileName, const bool bCVS = false, const bool bBase = false, const int MaxRowOfPage = 66) = 0;		//����ƽ�ֺ˶Ա�
	virtual  const char* __cdecl MatchTotal(PMatchs * pMatchs, int Count) = 0;								//���ֳɽ�����
	virtual const char* __cdecl PositionTotal(PPositions * pPositions, int Count) = 0;
	virtual const char* __cdecl PositionSubTotal(PPositions * pPositionSubs, int Count) = 0;
};

