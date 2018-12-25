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

typedef vector<int> vecInt;
typedef std::map<std::string, std::set<int>> strIntMapSet;
typedef std::map<pair<std::string, std::string>, std::set<int>> UserNOContract_OrderIDMAP;

HWND __stdcall GetCfgWnd(const wchar_t *subtitle, const HWND parent);

class PositionDiffRiskMgt : public ICommonInteractRiskCheck, public ISQuoteEventSpi, public ICspAccessSpi, public IRiskModuleApi
{
public:
	PositionDiffRiskMgt();

public:
	//仓差风控检测
	virtual bool __cdecl PositionRangeCheck(const TSendOrder &TSOrder, ErrorReflectInfo &eError);
	//撤单仓差检测
	virtual bool __cdecl CancelPositionRangeCheck(std::set<int> &vecOrderId, char *userNo = "");

public:
	virtual int __cdecl CheckOrderOperation(const void *data, int nType);

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
	bool DealPositionRangeCheck(const TSendOrder &TSOrder, ErrorReflectInfo &eError);
	bool CheckPositionSingleKey(ErrorReflectInfo &eError, const TSendOrder &TSOrder);
	void CalBuyPositionAndSellPosition(int &nBuyCount, int &nSellCount, const TSendOrder &TSOrder);
	void CalCurPosiRangeAndSglDirectPosi(int &nBuyCount, int &nSellCount, int &curRange, int &singelDirectCount, const TSendOrder &TSOrder);

	void InitUserContract_OrderIDMap(std::set<int>& vecOrderId, std::set<int>& tempSetOrderID,
		UserNOContract_OrderIDMAP &map_UserContract_OrderID);
	void showCannotCancelOrder(std::set<int>& cannotCanelOrder);
	bool CheckConsideOrder(int OrderID, std::set<int>& canCanelOrder, std::set<int>& cannotCanelOrder, 
		std::set<int>& consideCanelOrder, Datas<const TOrder*>& OrderData);
	void FindCurCancelOrderAndConOrder(Datas<const TOrder*>& OrderData, TOrder * tempParOrder, const TOrder* tempOrder,
		int &curCancelOrderNum, int &conOrderNum, std::set<int>& canCanelOrder, std::set<int>& consideCanelOrder);
	bool CheckPositionLessRange(std::set<int> &OrderID, wchar_t *errorText);
	bool DealCancelOrder(std::set<int>& vecOrderId, UserNOContract_OrderIDMAP &map_UserContract_OrderID,
		std::set<int> &canCanelOrder, std::set<int> &cannotCanelOrder);
	bool DealCanCancelOrder(std::set<int>& vecOrderId, bool &canCancelAllOrder, std::set<int> &canCanelOrder, 
		std::set<int> &cannotCanelOrder, std::set<int> &consideCanelOrder);

	std::string GetContractKeyWithOrder(const TOrder* tOrder);
	wstring m_CurContractKey;
};