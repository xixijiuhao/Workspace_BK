#ifndef _REMEDYTRADE_API_
#define _REMEDYTRADE_API_

#include "TradeApi_Struct.h"

const wchar_t PLUG_REMEDYTRADEAPI_NAME[] = L"RemedyTrade";
const wchar_t PLUG_REMEDYTRADEAPI_VERSION[] = L"9.3.0.1";

struct HWND__; typedef HWND__* HWND;

//������������
enum TDlgOperType
{
	dotInput,	//¼��
	dotCarry,	//carry��
	dotModify,	//�ĵ�
	dotRemove,	//ɾ��
	dotMove,	//�Ƶ�
	dotMultiMove,//�������
	dotExecute	//��Ȩ
};

typedef int ListDataType;
const ListDataType ldtOrder = 0;		//ί����Ϣ
const ListDataType ldtMatch = 2;		//�ɽ���Ϣ


class IRemedyTrade
{
public:
	/**
	* �������ں�������������ʱ�Զ����ٶ���
	* @param OperType ������������
	* @param pData ��������ָ�� ��Ҫ����DataType��ֵǿ��ת��ΪTMatch����TOrder
	* @param DataType �������� ָ��pData������
	* @param hParentWnd �����ھ��
	* @return
	*/
	virtual void __cdecl GetMatchObject(TDlgOperType OperType = dotInput, void * pData = nullptr, ListDataType DataType = ldtMatch, HWND hParentWnd = nullptr) = 0;

	/**
	* �������ں�������������ʱ�Զ����ٶ���
	* @param OperType ������������
	* @param pOrder ��������ָ��
	* @param hParentWnd �����ھ��
	* @return
	*/
	virtual void __cdecl GetOrderObject(TDlgOperType OperType = dotModify, TOrder * pOrder = nullptr, HWND hParentWnd = nullptr) = 0;
};

#endif