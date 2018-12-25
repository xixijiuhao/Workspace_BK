#pragma once
class TAgreementPopWnd : public TPopupWnd
{
public:
	TAgreementPopWnd();
	virtual ~TAgreementPopWnd();
public:
	virtual	bool			Create(HWND hparent);
	void					SetData(const std::vector<string>& stldata);
	string					GetData(int nindex) { return m_stldata.at(nindex); }
	void					GetData(char* sztext, string& stdata);
public:
	int						GetListHeight();
	virtual  void			MoveWindow(const int& x, const int& y, const int& cx, const int& cy);
public:
	void					SetBkColor(COLORREF clrbk) { m_clrbk = clrbk; }
	void					SetFgColor(COLORREF clrFg) { m_clrfg = clrFg; }
protected:
	virtual		LRESULT		WndProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual		void		DrawMain(TMemDC* pmemdc, RECT& rect);
protected:
	void					OnPaint();
	void					OnSize();
	void					OnLbuttonDown(WPARAM wParam, LPARAM lParam);
	void					OnMouseMove(WPARAM wParam, LPARAM lParam);
	void					OnVScroll(WPARAM wParam, LPARAM lParam);
	void					OnMouseWheel(WPARAM wParam, LPARAM lParam);
	void					OnMouseLeave();
protected:
	int						GetLineHeight();
	int						GetLineWidth();
	void					ResetScrollBars(bool brestpos = false);
	void					RefreshData();
	bool					IsOnlyShow();
protected:
	int						m_nmouseover;
	int						m_nleftalign;
	int						m_nstyle;
private:
	int						m_nmaxheight = 230;
	COLORREF				m_clrbk;
	COLORREF				m_clrfg;
	const	int				m_nlinegap = 10;
	bool					m_btrack;
	bool					m_bVscrollVisable;
	HFONT					m_font;
	std::vector<string>		m_stldata;
};



