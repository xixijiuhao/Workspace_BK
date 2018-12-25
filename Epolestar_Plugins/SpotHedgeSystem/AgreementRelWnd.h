#pragma once
class AgreementRelWnd :public TIxFrm
{
public:
	AgreementRelWnd();
	~AgreementRelWnd();

	void Create(LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle = 0);

	TListView			m_listAgreementRel;
protected:
	enum ControlID
	{
		ID_Begin,
		ID_ListView_AgreeementRel,

		IDM_AddAgreementRel,
		IDM_Line,
		IDM_ModAgreementRel,
		IDM_DelAgreementRel,

		ID_End
	};
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
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

	void DealAgreeList(LPARAM lParam);
	void ShowMenu(HMENU hmenu);
	void EnableMenu(uint id, bool bEnable);
private:
	//AddAgreeementWnd	m_addAgreementWnd;
	HMENU				m_menuAgreementRel;
};

