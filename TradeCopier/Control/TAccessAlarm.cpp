#include "PreInclude.h"

#define widgetHeight  20
#define textLeft      10
#define textTop       (30+15)

#define frmWidth   330
#define frmHeight   150
#define btnWidth    70
TAccessAlarm::TAccessAlarm(HWND hwnd, char* szTitle, int type) :TModalWnd(hwnd, szTitle)
{
	m_iFrmType = type;
	m_iDaysLeft = 0;
}
TAccessAlarm::~TAccessAlarm()
{

}
void TAccessAlarm::CreateSubCtrl()
{

}
void TAccessAlarm::SetModalRect()
{
	MoveWindow(m_Hwnd, (GetSystemMetrics(SM_CXSCREEN) - 300) / 2, (GetSystemMetrics(SM_CYSCREEN) - 150) / 2, frmWidth, frmHeight, true);
}
void TAccessAlarm::DrawMain(TMemDC* pmemdc)
{
	RECT rc = { 0 };
	GetClientRect(m_Hwnd, &rc);
	rc.top = CAPTION_HEIGHT;
	rc.top += 15;
	rc.left = 10;
	rc.right -= 10;
	rc.bottom = rc.top + widgetHeight;
	SelectObject(pmemdc->GetHdc(), g_FontData.g_FontWord12);

	SetTextColor(pmemdc->GetHdc(), g_ColorRefData.g_ColorBlack);

	if (m_iFrmType == noAuthent)
	{
		DrawText(pmemdc->GetHdc(), L"��֤δͨ��������ϵ��ʢ�г�����021-50196965��Ȩ��", -1, &rc, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	}
	else if (m_iFrmType == userTimeOut)
	{
		DrawText(pmemdc->GetHdc(), L"��Ȩ���ڣ�����ϵ��ʢ�г�����021-50196965��Ȩ��", -1, &rc, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	}
	else if (m_iFrmType == daysLeftWarn)
	{
		TCHAR szText[100] = { 0 };
		wsprintf(szText, L"�ù���ʹ����Ȩ������Ϊ%d��%d��%d�գ�ʣ��ʱ��Ϊ%d��", m_iExpireDate / 10000, m_iExpireDate % 10000 / 100,
			m_iExpireDate % 100, m_iDaysLeft);
		DrawText(pmemdc->GetHdc(), szText, -1, &rc, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		rc.top += widgetHeight;
		rc.bottom = rc.top + widgetHeight;
		DrawText(pmemdc->GetHdc(), L"Ϊ��Ӱ������ʹ�ã�����ϵ��ʢ�г���021-50196965��Ȩ��", -1, &rc, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	}
}
void TAccessAlarm::OnCreate()
{
	int left = textLeft + 60;
	int top = widgetHeight+ textTop;
	
	m_btnOk.CreateButton(m_Hwnd, "�رմ���", 0);
	top += widgetHeight*2 +10;
	left = (frmWidth - btnWidth) / 2;
	m_btnOk.MoveWindow(left, top, btnWidth, widgetHeight);

}

LRESULT	TAccessAlarm::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		OnCreate();
		return PROCESSED;
	case SSWM_STATIC_BUTTON_CLICKDOWN:
		DestroyWindow(m_Hwnd);
		return PROCESSED;
	default:
		return __super::WndProc(message, wParam, lParam);
	}
}

unsigned int getDays(unsigned int date)
{
	char monthDay[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	unsigned int year = date/10000 ;
	if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))
		monthDay[1] = 29;

	unsigned int days = 0;
	for (int i = 2016; i < year; i++)
	{
		if ((i % 4 == 0 && i % 100 != 0) || (i % 400 == 0))
			days += 366;
		else
			days += 365;
	}
	days += date % 100;
	int month = date % 10000 / 100;
	for (int i = 0; i < month - 1; i++)
	{
		days += monthDay[i];
	}
	return days;
}
//����Ȩ�����ڣ�7���ڿ�ʼ�ж�ʣ��ʱ��
bool TAccessAlarm::SetDate(unsigned int serverDate, unsigned int expireDate)
{
	unsigned int serverDays = getDays(serverDate);
	unsigned int exprireDays = getDays(expireDate);
	m_iExpireDate = expireDate;
	if (serverDays + 7 > exprireDays)
	{
		m_iDaysLeft = exprireDays - serverDays;
		return true;
	}
	else
		m_iDaysLeft = 0;
	return false;
}