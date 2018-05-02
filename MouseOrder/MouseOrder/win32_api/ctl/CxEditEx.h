#pragma once

class CxEditEx :public TIxFrm
{
public:
	CxEditEx();
	~CxEditEx();
	void Create(int nID, int nSubID, HWND parent, int nx, int ny, int nwidth, int nheight, const wchar_t *defshow, bool bFrame = true);
	HWND GetEditWnd(){ return m_hEdit; };
	void UpdateFont();
public:
	WNDPROC	g_ExEditProc;
	bool m_bTracking;

	void OnDealHover();
	void OnDealLeave();
	void OnDealSetFocus();
	void OnDealKillFocud();
protected:
	virtual LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	bool m_bHover;
	bool m_bFocus;
    bool m_bFrame;
	
	HBRUSH m_ClientBrush;
	HBRUSH m_hOrderBrush;
	HBRUSH m_HotOrderBrush1;
	HBRUSH m_HotOrderBrush2;
	int		m_nWidth;
	int		m_nHeight;
	int		m_nLeft;
	int		m_nTop;
	HWND	m_hEdit;
	int		m_nWinId;
	int		m_nEditId;
	wchar_t	m_wDefShow[50];
	void OnPaint(bool bHover);
	
};

