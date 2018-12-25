#pragma once
class AddManualCloseWnd :public TIxFrm
{
public:
	AddManualCloseWnd();
	//~AddManualCloseWnd();
	void ShowFrm(HWND hParent);
	void Register(SystemFrame* pdata);
	void InitWnd(SmsMatchDataRsp closeMatch, string closeAgreeNo, vector<SmsMatchDataRsp>& unrel, vector<SmsManualCloseDataRsp>& rel);
	void AddToSelList(SmsMatchDataRsp& match);			//添加到列表
	void AddToAddList(SmsManualCloseDataRsp& data);
	TListView	m_listAddMatch;
	TListView	m_listSelMatch;
protected:
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
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
	//void OnBtnOk();
	//void OnBtnCancel();
	void OnBtnClickDown(WPARAM wParam, LPARAM lParam);
	
	void DealAddMatchList(LPARAM lParam);
	void DealSelMatchList(LPARAM lParam);

	void OnBtnAdd();
	void OnBtnDel();
	void ShowMenu(HMENU hmenu);

private:
	void CreateWidget();
	void InitWidget();
	void SetWidgetPos();

	bool m_bMouseTrack;
	//bool m_bisOpen;

	RECT		m_rcClose;
	bool		m_bInCloseRect;
	void		DrawClose(TMemDC *pMemDC);

	enum
	{
		ID_labCloseAgreeNo,
		ID_editCloseAgreeNo,
		ID_labCloseMatchNo,
		ID_editCloseMatchNo,

		ID_ListView_AddMatch,

		ID_labCloseMatchTip,

		ID_labOpenMatchNo,
		ID_editOpenMatchNo,
		ID_labCloseQty,
		ID_editCloseQty,

		ID_ListView_SelMatch,

		//ID_btnDelete,
		ID_btnAdd,

		IDM_delAddMatch,
	};
private:
	HINSTANCE	m_hInstance;
	SystemFrame * m_subSystem;
	HMENU		m_menuDelete;

	TStatic     m_labCloseAgreeNo;
	CxEdit      m_editCloseAgreeNo;

	TStatic     m_labCloseMatchNo;
	CxEdit      m_editCloseMatchNo;

	TStatic     m_labCloseMatchTip;

	TStatic     m_labOpenMatchNo;
	CxEdit      m_editOpenMatchNo;

	TStatic     m_labCloseQty;
	CxEdit      m_editCloseQty;



	//TStaticButton   m_btnDelete;
	TStaticButton   m_btnAdd;
};