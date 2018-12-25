#ifndef _TSTATICBUTTON_H
#define _TSTATICBUTTON_H

typedef UINT(CALLBACK* LPGNDLLFUNC)(HDC, CONST PTRIVERTEX, DWORD, CONST PVOID, DWORD, DWORD);
class TStaticButton : public TIxFrm
{
public:
	TStaticButton();
	~TStaticButton();
public:
	void Create(HWND hParent, const char* pText, int id);
public:
	bool			Create(HWND hparent, int nindex = 0);
	void			MoveWindow(const int& x, const int& y, const int& cx, const int& cy);
	bool			IsButtonEnable(){ return m_benable; }
	void			EnableButton(bool benable = true);
public:
	void			SetButtonText(const char* ptext);
	void			SetBkColor(COLORREF clrbk);
	void            SetFrameColor(COLORREF clrfrm);
	void            SetDTstyle(UINT DTstyle);
	void			SetFont(HFONT hfont);
	void            SetbtnTextColor(COLORREF color);
	void            SetbtnCursorStyle(LPCTSTR CursorStyle);
protected:
	virtual		LRESULT		WndProc(UINT message, WPARAM wParam, LPARAM lParam);
protected:
	void				OnPaint();
	void				OnLButtonDown();
	void				OnLButtonUp();
	void				OnMouseMove(WPARAM wParam, LPARAM lParam);
	void				OnMouseLeave(WPARAM wParam, LPARAM lParam);
	void                OnEnableWindow(WPARAM wParam, LPARAM lParam);
	void				OnSetFocus();
	void				OnKillFocus();
protected:
	void				DrawBtnBk(TMemDC* pmemdc, const RECT& rect);
private:
	int				    m_nindex;
	bool				m_btnclick;
	bool				m_bmouseover;
	bool				m_bmousetrack;
	bool				m_benable;
	bool				m_bfocus;
	bool                m_bClkChange;
private:
	HFONT			m_font;
	HCURSOR         m_hCursor;
	COLORREF		m_clrbk;
	COLORREF        m_clrfrm;
	COLORREF        m_textColor;
	std::string		m_sztext;
	HINSTANCE		m_gradient;
	LPGNDLLFUNC		_GradientFill;
	UINT            m_DTStyle;
};
#endif