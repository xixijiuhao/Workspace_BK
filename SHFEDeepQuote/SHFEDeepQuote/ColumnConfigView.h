#pragma once

class ColumnConfigView :public VirtualWnd
{
public:
	ColumnConfigView();
private:
	LRESULT OnDealMsg(UINT message, WPARAM wParam, LPARAM lParam);
private:
	void OnCreate();
	void CreateWidget();
	void SetWidgetPos();
	void InitWidget();
	void OnDrawItem(WPARAM wParam, LPARAM lParam);
	void OnMeasureItem(WPARAM wParam, LPARAM lParam);
	LRESULT OnNcDestory(WPARAM wParam, LPARAM lParam);
	void OnBtnClick(WPARAM wParam, LPARAM lParam);
	LRESULT OnNotify(WPARAM wParam, LPARAM lParam);
	void OnChangeItem(TListView list, int upordown);
	void RefreshColumn();
	void SetItem(int column, char* &psText);
private:
	TListView		m_listColumn;
	TStaticButton	m_btnUpMove;
	TStaticButton	m_btnDownMove;
	TStaticButton	m_btnSure;
	TStaticButton	m_btnClose;
	int				m_iSelectedIndex;
	ColumnSequence	*clSequence;
	int				m_iColumnQuence[4] = { 0 };
private:
	enum {
		ID_LISTCOLUMN = 1, ID_UPMOVE, ID_DOWNMOVE, ID_SUREBUTTON, ID_CLOSEBUTTON,
	};
};