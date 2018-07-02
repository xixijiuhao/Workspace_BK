#include "BaseInclude.h"

void TDateTime::Create(HWND hParent, UINT style)
{
	style |= WS_VISIBLE | WS_CHILD;
	m_hDateTime = CreateWindowEx(0, DATETIMEPICK_CLASS, 0, style, 0, 0, 0, 0, hParent, 0, GetModuleHandle(NULL), 0);
	SetWindowSubclass(m_hDateTime, DTProc, 0, 0);
}
void TDateTime::SetTimeFormat(TCHAR *pszFormat)
{
	DateTime_SetFormat(m_hDateTime, pszFormat);
}
void TDateTime::GetDate(char* pStrTime)
{
	SYSTEMTIME st;
	DateTime_GetSystemtime(m_hDateTime, &st);
	pStrTime[0] = st.wYear / 1000 + 0x30;
	pStrTime[1] = st.wYear % 1000 / 100 + 0x30;
	pStrTime[2] = st.wYear % 100 / 10 + 0x30;
	pStrTime[3] = st.wYear % 10 + 0x30;
	pStrTime[4] = '-';
	pStrTime[5] = st.wMonth / 10 + 0x30;
	pStrTime[6] = st.wMonth % 10 + 0x30;
	pStrTime[7] = '-';
	pStrTime[8] = st.wDay / 10 + 0x30;
	pStrTime[9] = st.wDay % 10 + 0x30;
}
//这个其实获取的是日期
//设置日期
void TDateTime::SetDate(char* pTime)
{
	if (!pTime)
		return;
	SYSTEMTIME st = { 0 };
	GetLocalTime(&st);
	if (pTime != NULL && pTime[0] != '\0')
	{
		WORD wYear = (pTime[0] - 0x30) * 1000;
		wYear += (pTime[1] - 0x30) * 100;
		wYear += (pTime[2] - 0x30) * 10;
		wYear += pTime[3] - 0x30;
		st.wYear = wYear;
		WORD wMonth = (pTime[5] - 0x30) * 10;
		wMonth += pTime[6] - 0x30;
		st.wMonth = wMonth;
		WORD wDay = (pTime[8] - 0x30) * 10;
		wDay += pTime[9] - 0x30;
		st.wDay = wDay;
		DateTime_SetSystemtime(m_hDateTime, GDT_VALID, &st);
	}
}
void TDateTime::SetTime(char* pTime)
{
	if (!pTime || strlen(pTime) < 8)
		return;
	SYSTEMTIME st = { 0 };
	GetLocalTime(&st);
	st.wHour = (pTime[0] - 0x30) * 10 + pTime[1] - 0x30;
	st.wMinute = (pTime[3] - 0x30) * 10 + pTime[4] - 0x30;
	st.wSecond = (pTime[6] - 0x30) * 10 + pTime[7] - 0x30;
	DateTime_SetSystemtime(m_hDateTime, GDT_VALID, &st);
}
void TDateTime::UpdateTime()
{
	SYSTEMTIME st = { 0 };
	GetLocalTime(&st);
	DateTime_SetSystemtime(m_hDateTime, GDT_VALID, &st);
}
void TDateTime::GetDateTime(char*pTime)
{
	if (!pTime)
		return;
	char strDate[11] = { 0 };
	char strTime[9] = { 0 };
	GetDate(strDate);
	GetTime(strTime);
	//pTime的长度为20
	sprintf_s(pTime, 20, "%s %s", strDate, strTime);
}
void TDateTime::GetTime(char *pTime)
{
	if (!pTime) return;
	SYSTEMTIME st;
	DateTime_GetSystemtime(m_hDateTime, &st);
	pTime[0] = st.wHour / 10 + 0x30;
	pTime[1] = st.wHour % 10 + 0x30;
	pTime[2] = ':';
	pTime[3] = st.wMinute / 10 + 0x30;
	pTime[4] = st.wMinute % 10 + 0x30;
	pTime[5] = ':';
	pTime[6] = st.wSecond / 10 + 0x30;
	pTime[7] = st.wSecond % 10 + 0x30;
	pTime[8] = '\0';
}
void TDateTime::SetTimeCurrent()
{
	SYSTEMTIME st;
	GetLocalTime(&st);
	DateTime_SetSystemtime(m_hDateTime, GDT_VALID, &st);
}
void TDateTime::Enable(bool bEnable)
{
	EnableWindow(m_hDateTime, bEnable);
}
LRESULT CALLBACK TDateTime::DTProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	if (message == WM_KEYDOWN && VK_TAB == wParam)
		SendMessage(::GetParent(hwnd), SSWM_TAB_SWITCH_FOCUS, 0, LPARAM(hwnd));
	return DefSubclassProc(hwnd, message, wParam, lParam);
}

void TDateTime::MoveWindow(int x, int y, int cx, int cy)
{
	SetWindowPos(m_hDateTime, 0, x, y, cx, cy, SWP_NOZORDER);
}

time_t TDateTime::GetTime()
{
	SYSTEMTIME stTime = { 0 };
	DateTime_GetSystemtime(m_hDateTime, &stTime);
	tm t = { 0 };
	t.tm_year = stTime.wYear - 1900;
	t.tm_mon = stTime.wMonth - 1;
	t.tm_mday = stTime.wDay;

	t.tm_hour = stTime.wHour;
	t.tm_min = stTime.wMinute;
	t.tm_sec = stTime.wSecond;

	return mktime(&t);
}