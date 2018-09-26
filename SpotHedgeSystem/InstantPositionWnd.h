#pragma once

typedef struct InstantPosInfo
{
	SmsPriceType				AvePostionPrice;			//持仓均价
	SmsUserNoType				UserNo;						//交易员编号		
	SmsContractNoType           ContractNo;                 //合约编号
	SmsDirectType               Direct;                     //买卖方向
	SmsQtyType                  PositionQty;                //持仓数量
	SmsPriceType				PreferPrice;				//推荐价
	SmsPriceType				LastPrice;					//最新价

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
	void AddPosToList(string opneAgreeNo,vector<InstantPosInfo>& v_pos);			//添加到列表
	int CalculatePosition(string targetagreeNo, vector<InstantPosInfo> & v_posinfo, bool b_reqry = true);
	int  CalculatSpotRemain(string agreeNo);
	TListView			m_listInstantPosition;
	CxCombox			m_cbAgreementNo;

	map<string, map<string, SmsMatchDataRsp>>											m_map_mAgreeMatch;			//<合同号，map<成交号，成交数据>>
	map<string, SmsAgreementDataRsp>													m_map_Agree;				//<合同号，合同数据>
	map<SmsAgreementDataRsp, set<SmsAgreementDataRsp>>									m_map_sAgreeRel;			//<开仓数据，set<平仓数据>>
	SpecPriceMapType																	m_map_specPrice;			//现货价格

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
	double	m_tradeDot;				//每手乘数
	int     m_cursel;
	bool    m_bReqry;	
};