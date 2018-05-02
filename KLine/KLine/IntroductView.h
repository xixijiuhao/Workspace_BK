#pragma once

class KLineIntroductView : public TIxFrm
{
public:
	KLineIntroductView();
	void SetText(std::vector<string>& text);
	void CreateView(HWND hParent, RECT r);
	void MoveWindow(RECT rect);
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	void OnPaint();
private:
	std::vector<string>	m_vtText;//每行有Index
	std::vector<RECT>		m_vtRect;
private:
	int						m_iYVap;//行间距
	HBRUSH					m_bkBrush;
	RECT					m_rect;
};
