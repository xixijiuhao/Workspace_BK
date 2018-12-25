#ifndef _TCHECKBOX_H
#define _TCHECKBOX_H
////////////////////////**//////////////////////////////////////////////////
typedef struct tagCheckStruct
{
	std::string		sztext;
	std::string		sztextex;
	std::string		sztext2;
	std::string		szparam;//±∏”√
}CheckStruct;
class TCheckBox : public TIxFrm
{
public:
	TCheckBox();
	virtual ~TCheckBox();
	enum{ TCS_CHECK = 0,TCS_RADIO,TCS_CHECK_HOOK};
public:
	bool			Create(HWND hwnd, const char* ptext, int ntype = TCS_CHECK_HOOK, int nindex = 0);
	void			MoveWindow(const int& x, const int& y, const int& cx, const int& cy);
public:
	void			SetFont(HFONT hfont);
	void			SetValue(bool bvalue);
	void			SetText(const char* ptext);
	void			SetTextEX(const CheckStruct& stcheck);
	void			SetBkColor(COLORREF bkclr);
	void			SetFgColor(COLORREF fgclr);
public:
	bool			GetValue(){ return m_bvalue; }
	const char*	GetText(){ return m_stcheck.sztext.c_str(); }
	void			GetTextEX(CheckStruct& stcheck){ stcheck = m_stcheck; }
protected:
	virtual		LRESULT		WndProc(UINT message, WPARAM wParam, LPARAM lParam);
protected:
	void				OnPaint();
	void				OnLbuttonDown(WPARAM wParam,LPARAM lParam);
	void				OnKeyDown(WPARAM wParam,LPARAM lParam);
private:
	void				DrawCheckbox(TMemDC* pmemdc,RECT& rect);
	void				DrawRadio(TMemDC* pmemdc, RECT& rect);
	void				DrawCheckHook(TMemDC* pmemdc, RECT& rect);
private:
	bool					m_bfocus;
	bool					m_bvalue;
	int					m_nindex;
	int					m_checktype;
	RECT					m_rccheckbox;
protected:
	HFONT				m_font;
	COLORREF				m_bkclr;
	COLORREF				m_fgclr;
	CheckStruct			m_stcheck;
private:
	const		int		m_checksize = 15;
	const		int		m_selchecksize = 13;
};
#endif