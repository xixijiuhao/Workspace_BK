#pragma once
// typedef std::map<uint, std::string> MapOrderIDToMatchNo;
// typedef std::map<std::string, MapOrderIDToMatchNo> MapMarkToOrderID;

struct PingPangInfo
{
	char cOrderNo[21];
	int  iOrderId;
	int  iTime;
	int  iSendQty;//记录大单成交之后已经发送的数量
	PingPangInfo()
	{
		iOrderId = -1;
		cOrderNo[0] = '\0';
		iTime = 0;	
		iSendQty = 0;
	}
};
class PingPangOrder
{
public:
	void RegistConfig(PointOrderConfig* pstConfig);
	void RegistHwnd(HWND hParent);
	void InsertPingPangOrder(TSendOrder stOrder);
	void OnDealOrder(const TOrder* pstOrder);
	void OnDealMatch(const TMatch* pstMatch);
	void OnTimer();
private:
	void InsertRemarkToInfo(char* Remark, PingPangInfo stInfo);
	void GetOrderRemark(char* OrderRemark, int iSize = 51);
private:
	void InitTSendOrderFromTMatch(TSendOrder& stOrder, const TMatch* pMatch);
	bool GetMinPrice(TContractKey* pContract, double& dminPrice);
private:
	HWND  m_hParent;
	PointOrderConfig* m_pstConfig;
private:
	std::map<std::string, PingPangInfo> m_mapRemarkToInfo;
	static int m_iNum;
};