#pragma once

class KLineIntroductView : public TIxFrm
{
public:
	KLineIntroductView();
	void SetText(std::vector<string>& text);
	void CreateView(HWND hParent, RECT r);
	void MoveWindow(RECT rect);
	void OnSize(WPARAM wParam, LPARAM lParam);
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	void OnPaint();
	void OnMouseWheel(WPARAM wParam, LPARAM lParam);
	void OnLButtonDown(WPARAM wParam, LPARAM lParam);
	void OnLButtonUp(WPARAM wParam, LPARAM lParam);
	void OnMouseMove(WPARAM wParam, LPARAM lParam);
	void OnMouseLeave();
	void TrackMouse();
	void OnDealSetFocus();
private:
	std::vector<string>	m_vtText;//每行有Index
	std::vector<string>	m_vtFirstColText;//每行有Index
	std::vector<string>	m_vtSecondColText;//每行有Index
	std::vector<RECT>		m_vtRect;
private:
	bool					m_bMousetrack;//控制是否跟踪鼠标
	int						m_iYVap;//行间距
	int						m_iHeight;
	int						m_iMaxBottom;
	HBRUSH					m_bkBrush;
	RECT					m_rect;
	RECT					m_CurRect;
	RECT					m_ScrollRect;
	string					m_strCurFirstText;
};
