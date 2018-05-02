#ifndef _TCOMBOXLIST_H
#define _TCOMBOXLIST_H

typedef struct tagComboxStruct
{
	std::string		sztext;
	std::string		szvalue;
	int				nindex;
	bool			bshow;

	tagComboxStruct()
	{
		bshow = true;
	}
}ComboxStruct;

class TComboxList : public TPopupWnd
{
public:
	TComboxList();
	virtual ~TComboxList();
	enum{ Combox_Normal = 0,Combox_Check};
public:
	virtual	bool			Create(HWND hparent,int nstyle = Combox_Normal);
	void					SetData(const std::vector<ComboxStruct>& stldata);
	ComboxStruct			GetData(int nindex){ return m_stldata.at(nindex); }
public:
	int						GetListHeight();
	virtual  void			MoveWindow(const int& x, const int& y, const int& cx, const int& cy);
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
protected:
	int						m_nmouseover;
	int						m_nleftalign;
	int						m_nstyle;
protected:
	int						GetLineHeight();
	int						GetLineWidth();
	void					ResetScrollBars(bool brestpos = false);
	void					RefreshData();
	bool					IsOnlyShow();
private:
	bool					m_bVscrollVisable;
	HFONT					m_font;
	std::vector<ComboxStruct>		m_stldata;
private:
	const	int				m_nlinegap = 10;
	const	int				m_nmaxheight = 230;
};
#endif