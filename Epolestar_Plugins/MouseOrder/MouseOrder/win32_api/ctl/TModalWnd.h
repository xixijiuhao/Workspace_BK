#ifndef _TMODAL_WND_H
#define _TMODAL_WND_H

class TModalWnd : public TIxFrm
{
public:
	TModalWnd(HWND hparent);
	~TModalWnd();
public:
	bool						Domodal();
	virtual		void			CreateSubCtrl();
	void						GetRemarkInfo(char(&sRemark)[51]);
protected:
	bool						IsWndMessage(HWND Hwnd, MSG msg);
	void						OnPaint();
	void						OnSize();
	void						OnLbuttonDown(WPARAM wParam, LPARAM lParam);
	void						OnMouseMove(WPARAM wParam, LPARAM lParam);
	void						OnMouseLeave();
protected:
	virtual		void			SetModalRect();
	virtual		LRESULT			WndProc(UINT message, WPARAM wParam, LPARAM lParam);
protected:
	BOOL						DestroyWindow(HWND hWnd);
protected:
	std::wstring				m_sztitle;
	std::wstring				m_szok;
	std::wstring				m_szcancel;
	char						m_sRemark[50];
	HWND						m_parent;
protected:
	RECT						m_rctitle;
	RECT						m_rctext;
	RECT						m_rcBtnOk;
	RECT						m_rcBtnCancel;
	RECT						m_rcclose;

	bool						m_bhoverclose;
	bool						m_bhoverbtnok;
	bool						m_bhoverbtncancel;
	bool						m_btrack;
	int							m_nret;
protected:
	bool						m_bdomodal;
	void						BecomeModal();
	void						EndModal();
protected:
	HBITMAP						m_hbmptemp;				//目的大小位图
	HDC							m_hdctemp;					//缓存DC 图片加文字
	int							m_nWidth;
	int							m_nHeight;

	HWND						m_hEdit;
};
#endif