#include "PreInclude.h"

#define  DaySecond			60*60*24
void TCalendar::Move(int x, int y)
{
	int nwidth = 200,nheight = 145;
	if (0/*g_bsysxp*/)
		nwidth = 300;
	SetWindowPos(m_Hwnd, HWND_TOPMOST/*NULL*/, x, y, nwidth, nheight, /*SWP_NOMOVE |*/ SWP_NOZORDER| SWP_NOACTIVATE);
	//MoveWindow(x, y, nwidth, nheight);
}

//Msg
bool TCalendar::OnCreate(WPARAM wParam, LPARAM lParam)
{
	m_calendar = CreateWindow(MONTHCAL_CLASSW, NULL, WS_CHILD | WS_VISIBLE | MCS_NOTODAY
		| MCS_NOTODAYCIRCLE/*| MCS_SHORTDAYSOFWEEK*/, 0, 0, 0, 0, m_Hwnd, NULL, GetModuleHandle(NULL), NULL);
//	SendMessage(m_calendar, WM_SETFONT, (WPARAM)g_font_15, 0);
	return true;
}
void TCalendar::OnSize()
{
	RECT rect; GetClientRect(m_Hwnd, &rect);
	::MoveWindow(m_calendar, 1, 1, rect.right - rect.left -2, 143, true);
}

void TCalendar::OnNotify(WPARAM wParam, LPARAM lParam)
{
	NMSELCHANGE* pchange = (NMSELCHANGE*)lParam;
	if (pchange->nmhdr.code == MCN_SELECT)
	{
		sprintf_s(m_sztext, "%04d-%02d-%02d", pchange->stSelStart.wYear, pchange->stSelStart.wMonth, pchange->stSelStart.wDay);
		m_time = pchange->stSelStart;
		PostMessage(m_parent, SSWM_CALENDAR_CHANGE, 0, (LPARAM)m_Hwnd);
		ShowWindow(SW_HIDE);
		if (m_parent&&m_bhideparent) ::ShowWindow(m_parent, SW_HIDE);
	}
}

void TCalendar::OnActivate(WPARAM wParam, LPARAM lParam)
{
	int nactive = LOWORD(wParam);   // activation flag 
	if (WA_INACTIVE == nactive&&m_bauto)
	{
		ShowWindow(SW_HIDE);
		if (m_parent&&m_bhideparent) ::ShowWindow(m_parent, SW_HIDE);
	}
}

LRESULT TCalendar::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		OnCreate(wParam, lParam);
		break;
	case WM_SIZE:
		OnSize();
		break;
	case WM_NOTIFY:
		OnNotify(wParam, lParam);
		break;
	case WM_ACTIVATE:
		OnActivate(wParam, lParam);
		break;
	default:
		return __super::WndProc(message, wParam, lParam);
	}

	return PROCESSED;
}

void TCalendar::SetRanage(SYSTEMTIME stbegin, SYSTEMTIME stend)
{
	tm	currtm;
	SYSTEMTIME stcur[2];
	time_t currt(time(NULL));

	currt -= DaySecond;
	localtime_s(&currtm, &currt);
	memset(stcur, 0, sizeof(SYSTEMTIME) * 2);
	stcur[0] = stbegin;
	stcur[1] = stend;
	MonthCal_SetRange(m_calendar, GDTR_MAX|GDTR_MIN, &stcur);
}