#pragma once

class CapitalRiskMgt : public ICommonInteractRiskCheck, public ISQuoteEventSpi, public ICspAccessSpi
{
public:
	CapitalRiskMgt();

public:
	//�ֲ��ؼ��
	virtual bool __cdecl PositionRangeCheck(const TSendOrder &TSOrder, ErrorReflectInfo &eError);
	//�����ֲ���
	virtual bool __cdecl CancelPositionRangeCheck(std::set<int> &vecOrderId, char *userNo = "");

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
	bool CheckMoneyCancelThd(const TSendOrder &TSOrder, ErrorReflectInfo &eError);
	bool CheckMoneyThd(const double fStandard, const TMoney *pMoney);
	bool CheckPercentageThd(const double firstMoney, const double secondMoney);
	bool CheckFixedCancelThd(const double firstMoney, const double secondMoney);

	wchar_t		m_sPathSound[MAX_PATH];

};