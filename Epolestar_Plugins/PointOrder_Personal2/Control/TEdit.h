#ifndef _TEDIT_H
#define _TEDIT_H

class TEdit : public TIxFrm
{
public:
	TEdit();
	~TEdit();
public:
	static TEdit* CreateEdit(HWND hParent, RECT& rect, int id = TEDIT_STRING);
	static void CreateEdit(HWND hParent, TEdit &edit, int style = TEDIT_STRING, int index = 0, DWORD specialstyle = 0);
	enum{ TEDIT_STRING = 0, TEDIT_FLOAT, TEDIT_INT, TEDIT_SINT, TEDIT_NUMCHAR , TEDIT_IP};
public:
	bool				Create(HWND hparent, DWORD specialstyle = 0,int nindex = 0);
	void				MoveWindow(const int& x, const int& y, const int& cx, const int& cy);
	void				EnableEdit(bool benable = true);
	void				SetPopWin(bool Value){ m_bHavaPopWin = Value; }
public:
	void				SetFont(HFONT hfont);
	void				SetDefText(const char* ptext){ m_sztext = ptext; }
	void				SetWindowText(const char* ptext);
	void                SetFgColor(COLORREF fgcolor);
	bool                IsEmpty();

	void                SetWindowText(int num);
	void			    SetWindowText(double num, int style = 2);
public:
	HWND				GetEditHwnd(){ return m_edit; }
	void				GetWindowText(char* ptext, int nmaxcount);
	void                GetWindowNumber(int &number);
protected:
	virtual		LRESULT			WndProc(UINT message, WPARAM wParam, LPARAM lParam);
	static		LRESULT	CALLBACK	EditProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
protected:
	void				OnPaint();
	virtual void		OnSize();
	virtual void		OnLbuttonDown(WPARAM wParam, LPARAM lParam);
	virtual void		OnMouseMove(WPARAM wParam, LPARAM lParam);
	virtual void		OnMouseLeave(WPARAM wParam, LPARAM lParam);
	virtual void        OnKeyDown(WPARAM wParam, LPARAM lParam, HWND hparent);
	virtual void        OnKeyUp(){};
	void				OnKillFocus(WPARAM wParam, LPARAM lParam);
	void				OnCommand(WPARAM wParam, LPARAM lParam);
	void                OnEnableWindow(WPARAM wParam, LPARAM lParam);
	LRESULT             OnCtlColor(WPARAM wParam, LPARAM lParam);
	
protected:
	virtual	  void	DrawMain(TMemDC* pmemdc, const RECT& rect, HBRUSH framebrush){}
private:
	bool				m_bmouseover;
	bool				m_bmousetrack;
	bool				m_blbtndown;
	bool				m_benable;
	bool                m_bHavaPopWin;
	int                 m_index;
protected:
	HWND				m_edit;
	std::string		m_sztext;
private:
	RECT				m_rcedit;
	COLORREF            m_fgcolor;
protected:
	const	int		m_gapsize = 2;
	int				m_nid;

};
#endif