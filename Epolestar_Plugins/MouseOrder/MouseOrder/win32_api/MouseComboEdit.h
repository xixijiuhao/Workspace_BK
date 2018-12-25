#pragma once
class CMouseComboBox;
class CMouseComboEdit:public CxSubClassWnd
{
public:
	CMouseComboEdit(CMouseComboBox& Combo);
	~CMouseComboEdit();
	void InitEdit(HWND hEdit);
private:
	CMouseComboBox &m_Combo;
	bool m_bHover;
	bool m_bFocus;
	bool m_bTracking;
	void DrawItemFrame(HWND hwnd, bool bHover, bool bFocus);
protected:
	LRESULT CALLBACK WndProc(UINT message, WPARAM wParam, LPARAM lParam);
};

