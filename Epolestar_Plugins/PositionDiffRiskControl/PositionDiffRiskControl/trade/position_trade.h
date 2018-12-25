#pragma once

class PositonTrade : public ICommonInteractRiskCheck, public ISQuoteEventSpi,public ICspAccessSpi
{
private:
	const TUserInfo*	m_UserInfo;							//用户信息
	const TLogin* 		m_LoginUser;						//登录帐号

	double				equity;								//客户昨权益
	double				moneyThreshold;						//客户权益阈值
	
	uint				diffMax;							//仓差最大值
	uint				singleDiff;							//单边挂单量
	uint				diff;								//仓差

	long				buyPosition;						//买持仓
	long				sellPosition;						//卖持仓

	long				buyOpenParQty;						//买开挂单量
	long				sellOpenParQty;						//卖开挂单量
	long				buyCoverParQty;						//买平挂单量
	long				sellCoverParQty;					//卖平挂单量


public:
	PositonTrade();
	virtual ~PositonTrade();
	//仓差风控检测
	virtual bool __cdecl PositionRangeCheck(const TSendOrder &TSOrder, ErrorReflectInfo &eError);
	//撤单风控
	virtual bool __cdecl CancelPositionRangeCheck(std::set<int> &vecOrderId, char *userNo = "");
public:
	//连接成功
	virtual void __stdcall OnConnect();
	//连接断开
	virtual void __stdcall OnDisconnect();
	//登录应答
	virtual void __stdcall OnLogin(const SUserNoType user, const SErrCodeType err);
	//初始化完成（第二次登录不再初始化）
	virtual void __stdcall OnInitComplete();
public:
	//初始化完成 仅一次
	virtual void __stdcall OnInitCompleteCsp() {};
private:
	//判断是否低于仓差
	bool isUnderPosDif(std::set<int>&);
	//获取仓差最大值
	bool getPosDiffMax(const TSendOrder* ,uint&);
	//是否有对应单
	bool hasRightOrder(int OrderID, std::set<int>&, std::set<int>&, Datas<const TOrder*>&);
	//弹出不可撤订单信息
	void showCannotCancelOrder(std::set<int>&);
};