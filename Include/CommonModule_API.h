#ifndef _COMMON_MODULE_
#define _COMMON_MODULE_


const wchar_t PLUG_COMMONMOULE_NAME[] = L"CommonModule";
const wchar_t PLUG_COMMONMOULE_VERSION[] = L"1.0.0.1";

#include "TradeApi_Struct.h"
#include "CommonModule_Struct.h"

#include <set>

enum CommonInteractType
{
    InteractType_Common = 0,    //ͨ��
    InteractType_FastOrder,     //�����µ�
    InteractType_RiskCheck,     //���ռ��
};

class ICommonInteract
{
public:
	//ͨ������֪ͨ
	virtual void __cdecl OnCommonCfgChanged(CM_SUBID PartID, char* cdata) = 0;
};

class ICommonInteractFast
{
public:
    //�����µ�������Ϣ����
    virtual void __cdecl OnFastError(const ErrorReflectInfo* eInfo) = 0;

    //�����µ��������Ϣ �������ʿ�ݼ��ص�
    virtual void __cdecl OnFastOrderInfo(const TSendOrder* TSOrder, double dRatio) = 0;

    //���ٹ�ѡ״̬
    virtual void __cdecl OnFastStateInfo(int nstate) = 0;

    //�����µ���Ϣ��ȡ
    virtual bool __cdecl GetFastInfo(sFastInfo& sInfo) = 0;
};

class ICommonInteractRiskCheck
{
public:
    //�ֲ��ؼ��
    virtual bool __cdecl PositionRangeCheck(const TSendOrder &TSOrder, ErrorReflectInfo &eError) = 0;

    //�����ֲ���
    virtual bool __cdecl CancelPositionRangeCheck(std::set<int> &vecOrderId, char *userNo = "") = 0;
};

//
class ICommonModule
{
public:

	/*
		������Ϣ 	TSOrder
		������Ϣ 	eError
		����������	IVecInt
		�������	param
		����ֵ		���ͳɹ���ʧ��
	*/
	//�µ�
	virtual bool InsertOrder(const TSendOrder &TSOrder, ErrorReflectInfo &eError, IVecInt &vOrderIds,
		const CM_PARAM* param = NULL) = 0;

	//�ĵ�
	virtual bool ModifyOrder(TModifyOrder &TMOrder) = 0;

	//��������
	virtual bool BatchCancel(TOrder* porder) = 0;

	//�����ĵ�
	virtual bool BatchModify(TOrder* porder) = 0;

    //����
    virtual bool CancelOrder(std::set<int> &vecOrderId, char *userNo = "") = 0;

	//�µ�����ע��
	virtual bool Regist(void *p, CommonInteractType type = InteractType_Common) = 0;

	//ע��
	virtual bool UnRegist(void *p, CommonInteractType type = InteractType_Common) = 0;

	//���ÿ����µ�
	virtual bool StartFastOrder(ICommonInteractFast* p) = 0;

	//ֹͣ�����µ� 
	virtual bool StopFastOrder(ICommonInteractFast* p) = 0;

	//˫�� ���� �����
	virtual bool SetCodeFromQuote(const char* scode, const char* sfield, int nlevel, const char* cType, const char *sSign, C_FillOrderInfo& rInfo, C_ExcInfo *pExcInfo = NULL) = 0;

	//����ֲֺϼ�
	virtual bool SetCodeFromPosTotal(const char* sKey, const char* cType, C_FillOrderInfo& rInfo, C_ExcInfo* pExcInfo = NULL) = 0;

	//����ֲ���ϸ
	virtual bool SetCodeFromPosSub(const char* sPosNo, const char* cType, C_FillOrderInfo& rInfo, C_ExcInfo* pExcInfo = NULL) = 0;

	//˫�� ���� ί���	
	virtual bool SetCodeFromOrder(const char* sOrderID, const char* cType, C_FillOrderInfo& rInfo, C_ExcInfo* pExcInfo = NULL) = 0;

	//�Լ�ƽ��
	virtual int CoverPosition(const TPosition* pPos, wchar_t(&ErrorText)[51], TOrderWay OrderWay) = 0;

	//ȫƽ
	virtual void CoverAllPosition(TOrderWay OrderWay,char* UserNo = "",bool bSubmit = true) = 0;

	//��ȡĬ���µ���
	virtual uint GetDefaultQtyByOffset(const char* szSign, const TContractKey &ContractKey, char offset) = 0;

	//��ȡ�����µ���ѡ״̬
	virtual bool GetFastOrderWorkState() = 0;

	//ί�� ��/׷��
	virtual bool ChaseOrder(uint OrderID, wchar_t(&ErrorText)[51], CM_ChaseType ctype = 0) = 0;

	//���鷢��
	virtual bool QuoteClickOrder(const char* scont,TDirect tdirect,double dprice,bool bMakesure) = 0;
};

#endif