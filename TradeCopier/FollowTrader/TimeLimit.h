#ifndef  _T_TIME_LIMIT_H_
#define  _T_TIME_LIMIT_H_


class TimeLimitFrm : public TModalWnd
{
public:
	TimeLimitFrm(HWND hparent, const char* sztext);
	~TimeLimitFrm();
public:
	void SetContent(const wchar_t* szcontent);
protected:
	virtual	void	DrawMain(TMemDC* pmemdc);
	virtual	void	SetModalRect();
	virtual	void	CreateSubCtrl();
	virtual	LRESULT	WndProc(UINT message, WPARAM wParam, LPARAM lParam);
};

#endif