#pragma once

class KLineSelectTab:public TIxFrm
{
public:
	KLineSelectTab();
	~KLineSelectTab();
public:
	typedef std::function<void(int)> onTabChange;
	inline void RegistFunc(onTabChange func) 
	{
		if (func != nullptr) 
		{
			m_func = func;
		}
	}
	void CreateView(HWND hParent, RECT r);
	void MoveWindow(RECT rect)
	{
		SetWindowPos(m_Hwnd, 0, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, 0);
	}
protected:
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	void OnPaint();
	void OnMouseMove(WPARAM wParam, LPARAM lParam);
	void OnMouseLeave();
	void OnLButtonDown(WPARAM wParam, LPARAM lParam);
	void OnLButtonUp(WPARAM wParam, LPARAM lParam);

private:
	void TrackMouse();
	int  GetTabIndex(POINT pt);
	void InitRect(TMemDC dc);
private:
	int						m_iWidth;
	int						m_iHeight;
	HBRUSH					m_BackgroundBrush;
	HWND					m_hParent;
	onTabChange				m_func;
	std::vector<RECT>		m_vtRect;
	std::vector<wstring>    m_MainPattern;
	std::vector<wstring>    m_SonPattern;
	std::vector<wstring>    m_wtext;
private:
	BOOL					m_bMousetrack;
	BOOL					m_bInitRect;
	int						m_iHoverIndex;
	int						m_iCurrIndex;
};
