#pragma once

struct CancelPositionData
{
	int nPositionRange; //�ֲ�
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
	std::set<int> setRemoveID;      //���ܳ�����ID
	std::set<int> setAddID;         //����ί�е�id
	std::set<int> setSrcID;         //���ȷ�ϲ�������Ҫȥ��ԭ����ί��
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
	//�ֲ��ؼ��
	virtual bool __cdecl PositionRangeCheck(const TSendOrder &TSOrder, ErrorReflectInfo &eError);
	//�����ֲ���
	virtual bool __cdecl CancelPositionRangeCheck(std::set<int> &vecOrderId, char *userNo = "");

public:
	virtual int __cdecl CheckOrderOperation(const void *data, int nType);

public:
	//���ӳɹ�
	virtual void __stdcall OnConnect() {}
	//���ӶϿ�
	virtual void __stdcall OnDisconnect() {}
	//��¼Ӧ��
	virtual void __stdcall OnLogin(const SUserNoType user, const SErrCodeType err) {}
	//��ʼ����ɣ��ڶ��ε�¼���ٳ�ʼ����
	virtual void __stdcall OnInitComplete();
	//��������ʼ����� ��һ��
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