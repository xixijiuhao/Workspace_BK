#ifndef _TPOPUP_WND_H
#define _TPOPUP_WND_H

//////////////////////////////////////////////////////////////////////////
//作为WS_POPUP窗口的基类窗口
//////////////////////////////////////////////////////////////////////////
class TPopupWnd : public TIxFrm
{
public:
	TPopupWnd();
	virtual ~TPopupWnd();
public:
	virtual	bool		Create(HWND hparent);
	void				SetAutoHide(bool bauto = false){ m_bauto = bauto; }
public:
	virtual  void		MoveWindow(const int& x, const int& y, const int& cx, const int& cy);
	void				ShowWindow(int nCmdShow);
public:
	HWND				GetParent(){ return m_parent; }
protected:
	virtual		LRESULT		WndProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual		void			DrawMain(TMemDC* pmemdc);

protected:
	void				OnPaint();
	void				OnLbuttonDown(WPARAM wParam, LPARAM lParam);
	void				OnActivate(WPARAM wParam,LPARAM lParam);
protected:
	HWND				m_parent;
	bool				m_bauto;
};
#endif