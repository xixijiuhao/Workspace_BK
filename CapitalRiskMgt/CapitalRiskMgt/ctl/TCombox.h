#ifndef _TCOMBOX_H
#define _TCOMBOX_H

class TCombox : public TIxFrm
{
public:
	TCombox();
	virtual ~TCombox();
public:
	bool			Create(HWND hwnd, int nindex = 0);
	void			MoveWindow(const int& x, const int& y, const int& cx, const int& cy);
	void			Clear();
	void			ResizeData();
	int             GetSameTextIndex(string sText);
public:
	void			AddString(const char* ptext,const char* pvalue = NULL);
	void			CreateDropList();
	void			ReleaseComboxList();
public:
	void			SetUserComboxList(TPopupWnd* pwnd);
	void			SetFont(HFONT hfont);
	void			SetText(const char* ptext);
	char*		GetText(){ return const_cast<char*>(m_sztext.c_str()); }
	void			SetSelect(int nindex);
	int			GetSize(){return m_stldata.size(); }
	void			SetComboxHeight(int hight) { m_nComboxHeight = hight; }
protected:
	virtual		LRESULT		WndProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual		void			DrawDrop(TMemDC* pmemdc, const RECT& rect, HBRUSH framebrush);
protected:
	void				OnPaint();
	void				OnLbuttonDown(WPARAM wParam, LPARAM lParam);
	void				OnMouseMove(WPARAM wParam,LPARAM lParam);
	void				OnMouseLeave(WPARAM wParam, LPARAM lParam);
	void				OnComboxSelect(WPARAM wParam, LPARAM lParam);
private:
	int					m_nindex;
	bool					m_bmousetrack;
	bool					m_bmouseover;
	bool					m_blbtndown;
	bool					m_buserdef;
protected:
	HFONT				m_font;
	COLORREF				m_bkclr;
	std::string			m_sztext;
protected:
	std::vector<ComboxStruct>		m_stldata;
	TPopupWnd*			m_pcomboxlist;
private:
	int		m_nComboxHeight = 20;
	const	int		m_nDropWidth = 14;
};

#endif