#ifndef _TMESSAGE_BOX_H
#define _TMESSAGE_BOX_H

#define TMB_OK                       0x00000000L
#define TMB_OKCANCEL                 0x00000001L
class TMessageBox : public TModalWnd
{
public:
	TMessageBox(HWND hparent, const char* sztext, UINT ntype);
	~TMessageBox();
public:
	void				SetContent(const wchar_t* szcontent);
public:
	wchar_t			m_szcontent[1024];
protected:
	void							OnSize();
	void							OnButtonDown(WPARAM wParam,LPARAM lParam);
protected:
	virtual			void				DrawMain(TMemDC* pmemdc);
	virtual			void				SetModalRect();
	virtual			void				CreateSubCtrl();
	virtual			LRESULT			WndProc(UINT message, WPARAM wParam, LPARAM lParam);
protected:
	TStaticButton*	m_pbtnok;
	TStaticButton*	m_pbtnno;
	UINT				m_ntype;
};
bool _TMessageBox_Domodal(HWND hwnd, char* sztitle, wchar_t* sztext,UINT ntype = TMB_OK);
#endif