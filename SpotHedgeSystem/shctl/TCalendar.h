#ifndef _TCALENDAR_H
#define _TCALENDAR_H

#define SSWM_CALENDAR_CHANGE		WM_USER + 101
class TCalendar:public TPopupWnd
{
public:
	TCalendar(bool bhideparent = true) { m_bhideparent = bhideparent; }
	~TCalendar(){}
public:
	void		Move(int x, int y);
	char*		GetText(){ return m_sztext; }
	SYSTEMTIME	GetDate(){ return m_time; }
	void		SetRanage(SYSTEMTIME stbegin, SYSTEMTIME stend);
protected:
	bool		OnCreate(WPARAM wParam, LPARAM lParam);
	void		OnSize();
	void		OnNotify(WPARAM wParam, LPARAM lParam);
protected:
	virtual     void			OnActivate(WPARAM wParam, LPARAM lParam);
	virtual		LRESULT			WndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	HWND						m_calendar;
	char						m_sztext[50];
	SYSTEMTIME					m_time;
	int							m_nwidth;
	int							m_nheight;
	bool						m_bhideparent;
};
#endif