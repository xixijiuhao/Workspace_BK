#pragma once
class StatementWnd :public TIxFrm
{
public:
	StatementWnd();
	//~StatementWnd();
	void ShowFrm(HWND hParent);
	void Register(SystemFrame* pdata);
	void CalculateData(string begin,string end);
	void SetTitle(wstring temp);
protected:
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	SystemFrame * m_subSystem;

	HINSTANCE			m_hInstance;
	TListView			m_listAll;
	HMENU				m_menuExport;

private:
	void OnCreate();
	void OnPaint();
	void OnSize(WPARAM wParam, LPARAM lParam);
	void OnMouseMove(WPARAM wParam, LPARAM lParam);
	void OnMouseLeave();
	void OnLButtonDown(WPARAM wParam, LPARAM lParam);
	void OnNcLButtonDown(WPARAM wParam, LPARAM lParam);
	void OnLButtonUp(WPARAM wParam, LPARAM lParam);

	void OnDrawItem(WPARAM wParam, LPARAM lParam);
	void OnMeasureItem(WPARAM wParam, LPARAM lParam);
	void OnMouseWheel(WPARAM wParam, LPARAM lParam);
	void KeyDown(WPARAM wParam, LPARAM lParam);
	void OnNcDestroy();
private:
	bool		m_bMouseTrack;
	RECT		m_rcClose;
	bool		m_bInCloseRect;
	wstring		wtitle;
	enum
	{
		ID_ListAll,
		IDM_Export,
	};
private:
	LRESULT OnCtlColor(WPARAM wParam, LPARAM lParam);
	void OnCommand(WPARAM wParam, LPARAM lParam);
	void OnNotify(WPARAM wParam, LPARAM lParam);

	void DealAllList(LPARAM lParam);

	void ShowMenu(HMENU hmenu);
	void DrawClose(TMemDC *pMemDC);

	std::vector<std::string> SplitString(const std::string & str, char cSplit);
	int	DateStrToInt(const std::string & str);
	//填开仓合同到列表
	void	addOpenAgree(SmsAgreementDataRsp& openagree, int row);
	void	addCloseMatch(SmsMatchDataRsp&closematch, int row);
private:
	vector<SmsAgreementDataRsp>		m_vecOpenAgree;				//开仓合同
	vector<SmsAgreementDataRsp>		m_vecCloseAgree;			//平仓合同
	vector<SmsAgreementRelDataRsp>	m_vecAgreeRel;				//合同关系
	vector<SmsCashInoutDataRsp>		m_vecCashInout;				//出入金
	vector<SmsFundDataRsp>			m_vecFund;					//资金
	vector<SmsMatchDataRsp>			m_vecOpenMatch;					//成交
	vector<SmsMatchDataRsp>			m_vecCloseMatch;					//成交
	vector<SmsOrderDataRsp>			m_vecOpenOrder;					//委托
	vector<SmsOrderDataRsp>			m_vecCloseOrder;					//委托
	vector<SmsSpecDataRsp>			m_vecSpec;					//规格
	vector<SmsTaxDataRsp>			m_vecTax;					//税率

};
