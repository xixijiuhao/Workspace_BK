#ifndef _COMMON_MODULE_
#define _COMMON_MODULE_


const wchar_t PLUG_COMMONMOULE_NAME[] = L"CommonModule";
const wchar_t PLUG_COMMONMOULE_VERSION[] = L"1.0.0.1";

#include "TradeApi_Struct.h"
#include "CommonModule_Struct.h"

#include <set>

enum CommonInteractType
{
    InteractType_Common = 0,    //通用
    InteractType_FastOrder,     //快速下单
    InteractType_RiskCheck,     //风险检查
};

class ICommonInteract
{
public:
	//通用配置通知
	virtual void __cdecl OnCommonCfgChanged(CM_SUBID PartID, char* cdata) = 0;
};

class ICommonInteractFast
{
public:
    //快速下单错误信息反馈
    virtual void __cdecl OnFastError(const ErrorReflectInfo* eInfo) = 0;

    //快速下单回填订单信息 数量倍率快捷键回调
    virtual void __cdecl OnFastOrderInfo(const TSendOrder* TSOrder, double dRatio) = 0;

    //快速勾选状态
    virtual void __cdecl OnFastStateInfo(int nstate) = 0;

    //快速下单信息获取
    virtual bool __cdecl GetFastInfo(sFastInfo& sInfo) = 0;
};

class ICommonInteractRiskCheck
{
public:
    //仓差风控检测
    virtual bool __cdecl PositionRangeCheck(const TSendOrder &TSOrder, ErrorReflectInfo &eError) = 0;

    //撤单仓差检测
    virtual bool __cdecl CancelPositionRangeCheck(std::set<int> &vecOrderId, char *userNo = "") = 0;
};

//
class ICommonModule
{
public:

	/*
		定单信息 	TSOrder
		错误信息 	eError
		定单号容器	IVecInt
		界面参数	param
		返回值		发送成功、失败
	*/
	//下单
	virtual bool InsertOrder(const TSendOrder &TSOrder, ErrorReflectInfo &eError, IVecInt &vOrderIds,
		const CM_PARAM* param = NULL) = 0;

	//改单
	virtual bool ModifyOrder(TModifyOrder &TMOrder) = 0;

	//批量撤单
	virtual bool BatchCancel(TOrder* porder) = 0;

	//批量改单
	virtual bool BatchModify(TOrder* porder) = 0;

    //撤单
    virtual bool CancelOrder(std::set<int> &vecOrderId, char *userNo = "") = 0;

	//下单界面注册
	virtual bool Regist(void *p, CommonInteractType type = InteractType_Common) = 0;

	//注销
	virtual bool UnRegist(void *p, CommonInteractType type = InteractType_Common) = 0;

	//启用快速下单
	virtual bool StartFastOrder(ICommonInteractFast* p) = 0;

	//停止快速下单 
	virtual bool StopFastOrder(ICommonInteractFast* p) = 0;

	//双击 单击 行情填单
	virtual bool SetCodeFromQuote(const char* scode, const char* sfield, int nlevel, const char* cType, const char *sSign, C_FillOrderInfo& rInfo, C_ExcInfo *pExcInfo = NULL) = 0;

	//点击持仓合计
	virtual bool SetCodeFromPosTotal(const char* sKey, const char* cType, C_FillOrderInfo& rInfo, C_ExcInfo* pExcInfo = NULL) = 0;

	//点击持仓明细
	virtual bool SetCodeFromPosSub(const char* sPosNo, const char* cType, C_FillOrderInfo& rInfo, C_ExcInfo* pExcInfo = NULL) = 0;

	//双击 单击 委托填单	
	virtual bool SetCodeFromOrder(const char* sOrderID, const char* cType, C_FillOrderInfo& rInfo, C_ExcInfo* pExcInfo = NULL) = 0;

	//对价平仓
	virtual int CoverPosition(const TPosition* pPos, wchar_t(&ErrorText)[51], TOrderWay OrderWay) = 0;

	//全平
	virtual void CoverAllPosition(TOrderWay OrderWay,char* UserNo = "",bool bSubmit = true) = 0;

	//获取默认下单量
	virtual uint GetDefaultQtyByOffset(const char* szSign, const TContractKey &ContractKey, char offset) = 0;

	//获取快速下单勾选状态
	virtual bool GetFastOrderWorkState() = 0;

	//委托 跟/追单
	virtual bool ChaseOrder(uint OrderID, wchar_t(&ErrorText)[51], CM_ChaseType ctype = 0) = 0;

	//行情发单
	virtual bool QuoteClickOrder(const char* scont,TDirect tdirect,double dprice,bool bMakesure) = 0;
};

#endif