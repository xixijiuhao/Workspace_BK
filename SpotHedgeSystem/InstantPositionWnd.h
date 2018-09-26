#pragma once

typedef struct InstantPosInfo
{
	SmsPriceType				AvePostionPrice;			//�ֲ־���
	SmsUserNoType				UserNo;						//����Ա���		
	SmsContractNoType           ContractNo;                 //��Լ���
	SmsDirectType               Direct;                     //��������
	SmsQtyType                  PositionQty;                //�ֲ�����
	SmsPriceType				PreferPrice;				//�Ƽ���
	SmsPriceType				LastPrice;					//���¼�

	InstantPosInfo()
	{
		memset(this, 0, sizeof(InstantPosInfo));
	}
}InstantPosInfo;

class InstantPositionWnd :public TIxFrm
{
public:
	InstantPositionWnd();
	~InstantPositionWnd();
	void Create(LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle = 0);
	void Register(SystemFrame* pdata);
	void AddPosToList(string opneAgreeNo,vector<InstantPosInfo>& v_pos);			//��ӵ��б�
	int CalculatePosition(string targetagreeNo, vector<InstantPosInfo> & v_posinfo, bool b_reqry = true);
	int  CalculatSpotRemain(string agreeNo);
	TListView			m_listInstantPosition;
	CxCombox			m_cbAgreementNo;

	map<string, map<string, SmsMatchDataRsp>>											m_map_mAgreeMatch;			//<��ͬ�ţ�map<�ɽ��ţ��ɽ�����>>
	map<string, SmsAgreementDataRsp>													m_map_Agree;				//<��ͬ�ţ���ͬ����>
	map<SmsAgreementDataRsp, set<SmsAgreementDataRsp>>									m_map_sAgreeRel;			//<�������ݣ�set<ƽ������>>
	SpecPriceMapType																	m_map_specPrice;			//�ֻ��۸�

	HANDLE										m_hQryMatch;
	HANDLE										m_hQrySpecPrice;
protected:
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	enum
	{
		ID_Begin,

		ID_LabAgreementNo,
		ID_CbAgreementNo,

		ID_BtnQry,
		ID_BtnCover,

		ID_ListView_InstantPosition,

		ID_End
	};

private:
	void OnCreate();
	void OnPaint();
	void OnCommand(WPARAM wParam, LPARAM lParam);
	void OnDrawItem(WPARAM wParam, LPARAM lParam);
	void OnNotify(WPARAM wParam, LPARAM lParam);
	LRESULT OnCtlColor(WPARAM wParam, LPARAM lParam);
	void OnNcDestroy();
	void OnDestroy();
	void OnSize(WPARAM wParam, LPARAM lParam);
	void OnMeasureItem(WPARAM wParam, LPARAM lParam);
	void OnBtnClickDown(WPARAM wParam, LPARAM lParam);
	void OnTimer();
	void OnBtnQry(bool b_reqry = true);
	void OnBtnCover();
private:
	SystemFrame * m_System;
	HINSTANCE			m_hInstance;

	TStatic				m_labAgreementNo;

	TStaticButton		m_btnQry;
	TStaticButton		m_btnCover;

private:
	double	m_tradeDot;				//ÿ�ֳ���
	int     m_cursel;
	bool    m_bReqry;	
};