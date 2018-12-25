#pragma once

class CMultiColComboBox;
class COFInputDlg;
class CMultiColEdit:public CxSubClassWnd
{
public:
	CMultiColEdit(CMultiColComboBox& Combo, COFInputDlg&);
	~CMultiColEdit();
	void InitEdit(HWND hEdit);
private:
	CMultiColComboBox &m_Combo;
	COFInputDlg& m_ref;
	bool m_bHover;
	bool m_bFocus;
	bool m_bTracking;
	HBRUSH m_ClientBrush;
	HBRUSH m_hOrderBrush;
	HBRUSH m_HotOrderBrush1;
	HBRUSH m_HotOrderBrush2;
protected:
	LRESULT CALLBACK WndProc(UINT message, WPARAM wParam, LPARAM lParam);
};

