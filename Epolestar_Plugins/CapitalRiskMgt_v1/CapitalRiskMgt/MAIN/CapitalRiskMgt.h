#pragma once

struct CancelPositionData
{
	int nPositionRange; //仓差
	std::set<int> vecOrderID;
	TContractSingleKey *contract;


	CancelPositionData()
	{
		nPositionRange = 0;
		contract = NULL;
	}
};

struct CancelChangeData
{
	std::set<int> setRemoveID;      //不能撤单的ID
	std::set<int> setAddID;         //增加委托的id
	std::set<int> setSrcID;         //如果确认不撤单需要去掉原来的委托
};

class CapitalRiskMgt : public ICommonInteractRiskCheck, public ISQuoteEventSpi, public ICspAccessSpi
{
public:
	CapitalRiskMgt();

public:
	//仓差风控检测
	virtual bool __cdecl PositionRangeCheck(const TSendOrder &TSOrder, ErrorReflectInfo &eError);
	//撤单仓差检测
	virtual bool __cdecl CancelPositionRangeCheck(std::set<int> &vecOrderId, char *userNo = "");

public:
	//连接成功
	virtual void __stdcall OnConnect() {}
	//连接断开
	virtual void __stdcall OnDisconnect() {}
	//登录应答
	virtual void __stdcall OnLogin(const SUserNoType user, const SErrCodeType err) {}
	//初始化完成（第二次登录不再初始化）
	virtual void __stdcall OnInitComplete();
	//接入服务初始化完成 仅一次
	virtual void __stdcall OnInitCompleteCsp() {};

private:
	bool CheckMaxOpen(const TSendOrder &TSOrder, ErrorReflectInfo &eError);
	bool CheckCancel(const TSendOrder &TSOrder, ErrorReflectInfo &eError);
	bool CheckPositionRange(const TSendOrder &TSOrder, ErrorReflectInfo &eError);
	bool CheckPositionSingleKey(const TContractSingleKey &key, ErrorReflectInfo &eError, TDirect direct, const TSendOrder &TSOrder);

	void CalOrderPosition(std::set<int> &orderIdSet, std::map<std::string, CancelPositionData> &codeMapPosition,
		std::map<int, std::vector<std::string> > &mapOrderIDCmbContract);
	void CalCodePositionRange(const char *userNo, std::map<std::string, CancelPositionData> &codeMapPosition);
	void CheckCancelPositionRange(const char *userNo, std::map<std::string, CancelPositionData> &codeMapPosition, std::set<int> &orderIdSet,
		std::map<int, std::vector<std::string> > &mapOrderIDCmbContract, std::map<std::string, CancelChangeData> &mapChangeData);
	std::vector<int> FilterOrder(TDataContainer &tempDataCon, const char *userNo, std::string sCode, TDirect direct);
	void ShowPopInfo(std::map<std::string, CancelChangeData> &mapChangeData, std::set<int> &orderIdSet);

private:
	//组合变单腿
	void CombTContractKeyToTContractSingleKey(const TContractKey &ContractKey, TContractSingleKey &SigCont1, TContractSingleKey &SigCont2);
	//组合合约是否以第一腿为准，1是，0以第二腿为准
	bool CombContractIsFirst(const TCommodityKey & CommodityKey);

private:
	bool m_bAuthored;
};