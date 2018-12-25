#pragma once
class AddAgreementRelWnd :public TIxFrm
{
public:
	AddAgreementRelWnd();
	//~AddAgreementRelWnd();
	void ShowFrm(HWND hParent);
	void Register(SystemFrame* pdata);
	void InitWnd(SmsAgreementDataRsp& agree, bool isBuy, vector<SmsAgreementDataRsp>& unrel, vector<string>& rel);
	void AddAgreementToListSel(SmsAgreementDataRsp& agree);															//添加到列表sel
	void AddAgreementToListAdd(string agreeno);																		//添加到列表add
	bool m_bisBuy;
	TListView	m_listAgreeSel;
	TListView	m_listAgreeAdd;
	int			m_remain;
	void		RefreshTips2();
protected:
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	RECT		m_rcClose;
	bool		m_bInCloseRect;
	void		DrawClose(TMemDC *pMemDC);

	void		OpenLayOut();
	void		CloseLayOut();
private:
	SystemFrame * m_subSystem;
	HINSTANCE	m_hInstance;
	HMENU		m_menuAgreementRel;

	TStatic     m_labBuyAgreementNo;
	CxEdit      m_editBuyAgreementNo;

	TStatic     m_labSellAgreementNo;
	CxEdit      m_editSellAgreementNo;

	TStatic     m_labTips;
	TStatic     m_labTips2;

	//TStaticButton   m_btnDelete;
	//TStaticButton   m_btnAdd;

private:
	void OnCreate();
	void OnPaint();
	void OnSize(WPARAM wParam, LPARAM lParam);
	void OnMouseMove(WPARAM wParam, LPARAM lParam);
	void OnMouseLeave();
	void OnLButtonDown(WPARAM wParam, LPARAM lParam);
	void OnLButtonUp(WPARAM wParam, LPARAM lParam);
	void OnNcLButtonDown(WPARAM wParam, LPARAM lParam);
	//void OnSearchCode(LPARAM lParam);
	void OnMouseWheel(WPARAM wParam, LPARAM lParam);
	void KeyDown(WPARAM wParam, LPARAM lParam);
	void OnNcDestroy();
	void OnDrawItem(WPARAM wParam, LPARAM lParam);
	void OnMeasureItem(WPARAM wParam, LPARAM lParam);

private:
	LRESULT OnCtlColor(WPARAM wParam, LPARAM lParam);
	void OnCommand(WPARAM wParam, LPARAM lParam);
	void OnNotify(WPARAM wParam, LPARAM lParam);
	void OnBtnAdd();
	void OnBtnDelete();
	void OnBtnClickDown(WPARAM wParam, LPARAM lParam);
	void DealAgreeSelList(LPARAM lParam);
	void DealAgreeAddList(LPARAM lParam);
	void ShowMenu(HMENU hmenu);
private:
	void CreateWidget();
	void InitWidget();
	void SetWidgetPos();

	bool m_bMouseTrack;

	map<string, SmsAgreementDataRsp>	m_mapUnRelAgree;

	enum
	{
		ID_labBuyAgreementNo,
		ID_editBuyAgreementNo,
		ID_labSellAgreementNo,
		ID_editSellAgreementNo,
		ID_labTips,
		ID_labTips2,

		ID_ListView_AgreeAdd,
		ID_ListView_AgreeSel,

		//ID_btnAdd,
		//ID_btnDelete,
		IDM_deleteAgreeRel,
	};

};
