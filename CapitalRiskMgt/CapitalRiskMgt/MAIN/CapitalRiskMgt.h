#pragma once

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
	bool CheckMoneyCancelThd(const TSendOrder &TSOrder, ErrorReflectInfo &eError);
	bool CheckMoneyThd(const double fStandard, const TMoney *pMoney);
	bool CheckPercentageThd(const double firstMoney, const double secondMoney);
	bool CheckFixedCancelThd(const double firstMoney, const double secondMoney);

	wchar_t		m_sPathSound[MAX_PATH];

};