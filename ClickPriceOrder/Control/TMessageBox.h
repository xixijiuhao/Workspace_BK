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
	void SetContent(const wchar_t* szcontent);
public:
	wchar_t	m_szcontent[1024];
protected:
	void OnSize();
	void OnButtonDown(WPARAM wParam,LPARAM lParam);
	void OnKeyDown(WPARAM wParam, LPARAM lParam);
	void OnMouseMove();
	void OnLbuttonDown(WPARAM wParam, LPARAM lParam);

protected:
	virtual	void	DrawMain(TMemDC* pmemdc);
	virtual	void	SetModalRect();
	virtual	void	CreateSubCtrl();
	virtual	LRESULT	WndProc(UINT message, WPARAM wParam, LPARAM lParam);	

private:
	void SetBtnFocus();
	void SetBtnFocus(int id);
	void DrawFocusBtn(TMemDC* pmemdc, int id);
	void Result(int id);

protected:
	RECT m_rcOk;
	RECT m_rcNo;
	UINT m_ntype;

private:
	bool  m_bFocusOk;
	bool  m_bFocusNo;
	enum{ID_OK = 0, ID_NO, ID_NULL};
};
bool _TMessageBox_Domodal(HWND hwnd, const char* sztitle, const wchar_t* sztext,UINT ntype = TMB_OK);
#endif