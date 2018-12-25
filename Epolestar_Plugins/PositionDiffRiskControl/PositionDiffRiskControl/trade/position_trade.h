#pragma once

class PositonTrade : public ICommonInteractRiskCheck, public ISQuoteEventSpi,public ICspAccessSpi
{
private:
	const TUserInfo*	m_UserInfo;							//�û���Ϣ
	const TLogin* 		m_LoginUser;						//��¼�ʺ�

	double				equity;								//�ͻ���Ȩ��
	double				moneyThreshold;						//�ͻ�Ȩ����ֵ
	
	uint				diffMax;							//�ֲ����ֵ
	uint				singleDiff;							//���߹ҵ���
	uint				diff;								//�ֲ�

	long				buyPosition;						//��ֲ�
	long				sellPosition;						//���ֲ�

	long				buyOpenParQty;						//�򿪹ҵ���
	long				sellOpenParQty;						//�����ҵ���
	long				buyCoverParQty;						//��ƽ�ҵ���
	long				sellCoverParQty;					//��ƽ�ҵ���


public:
	PositonTrade();
	virtual ~PositonTrade();
	//�ֲ��ؼ��
	virtual bool __cdecl PositionRangeCheck(const TSendOrder &TSOrder, ErrorReflectInfo &eError);
	//�������
	virtual bool __cdecl CancelPositionRangeCheck(std::set<int> &vecOrderId, char *userNo = "");
public:
	//���ӳɹ�
	virtual void __stdcall OnConnect();
	//���ӶϿ�
	virtual void __stdcall OnDisconnect();
	//��¼Ӧ��
	virtual void __stdcall OnLogin(const SUserNoType user, const SErrCodeType err);
	//��ʼ����ɣ��ڶ��ε�¼���ٳ�ʼ����
	virtual void __stdcall OnInitComplete();
public:
	//��ʼ����� ��һ��
	virtual void __stdcall OnInitCompleteCsp() {};
private:
	//�ж��Ƿ���ڲֲ�
	bool isUnderPosDif(std::set<int>&);
	//��ȡ�ֲ����ֵ
	bool getPosDiffMax(const TSendOrder* ,uint&);
	//�Ƿ��ж�Ӧ��
	bool hasRightOrder(int OrderID, std::set<int>&, std::set<int>&, Datas<const TOrder*>&);
	//�������ɳ�������Ϣ
	void showCannotCancelOrder(std::set<int>&);
};