#pragma once

#define INTRODUCTTEXTHIGHT 600
#define ChartPatternTextCnt 15
#define FibonacciPatternTextCnt 19
#define KeyLevelsTextCnt 10

class KLineIntroductView : public TIxFrm
{
public:
	KLineIntroductView();
	void SetText(ACSpiData &spiData);
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
	void DrawIntroductText(TMemDC& dc, string curText, string curValueText, RECT r, SIZE size);
	void DrawFillRectangle(TMemDC& dc, string curValueText, RECT r, HBRUSH &bBrush);
private:
	std::unordered_map<string, string>	m_vtText;//每行有Index
	std::unordered_map<string, string>	m_vtFirstColText;//每行有Index
	std::unordered_map<string, string>	m_vtSecondColText;//每行有Index
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

	//wstring ChartPatternText[ChartPatternTextCnt];
	//wstring FibonacciPatternText[FibonacciPatternTextCnt];
	//wstring KeyLevelsText[KeyLevelsTextCnt];

	
};
