#ifndef _TMODAL_WND_H
#define _TMODAL_WND_H

class TModalWnd : public TIxFrm
{
public:
	TModalWnd(HWND hparent, const char* sztitle);
	~TModalWnd();
public:
	bool						Domodal();
	virtual		void			CreateSubCtrl() = NULL;
protected:
	void						OnPaint();
	void						OnSize();
	void						OnLbuttonDown(WPARAM wParam, LPARAM lParam);
	void						OnMouseMove();
	void						OnMouseLeave();
protected:
	virtual		void			SetModalRect() = NULL;
	virtual		void			DrawMain(TMemDC* pmemdc){}
	virtual		LRESULT			WndProc(UINT message, WPARAM wParam, LPARAM lParam);
protected:
	BOOL						DestroyWindow(HWND hWnd);
protected:
	std::string				    m_sztitle;
	HWND						m_parent;
protected:
	RECT						m_rctitle;
	RECT						m_rcclose;

	bool						m_bhoverclose;
	bool						m_btrack;
	int						m_nret;
protected:
	bool				m_bdomodal;
	void				BecomeModal();
	void				EndModal();
protected:
	const int CAPTION_HEIGHT = 30;
};
#endif