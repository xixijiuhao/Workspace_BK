#include "BaseInclude.h"


using namespace std;
wchar_t g_sUnicode[1024] = { 0 };
CxSubClassWnd::CxSubClassWnd() :m_hParenthWnd(NULL)
, m_hWnd(NULL)
, m_OldProc(NULL)
{

}
CxSubClassWnd::~CxSubClassWnd()
{

}
BOOL CxSubClassWnd::Init(HWND hWnd)
{
	if (IsWindow(hWnd))
	{
		m_hWnd = hWnd;

		m_hParenthWnd = GetParent(m_hWnd);
		if (SetProp(m_hWnd, _T("CxSubClassWnd"), (HANDLE)this) == 0)
		{
			return FALSE;
		}
		m_OldProc = SetWindowLong(m_hWnd, GWL_WNDPROC, (LONG)stdProc);//�ı䴰�ڻص�����

	}
	return FALSE;
}
BOOL CxSubClassWnd::CxGetWindowText(wstring &w)
{
	::GetWindowTextW(m_hWnd, g_sUnicode, 1024);
	w = g_sUnicode;
	return TRUE;
}
LRESULT WINAPI CxSubClassWnd::stdProc(HWND hWnd, UINT uMsg, UINT wParam, LONG lParam)
{
	CxSubClassWnd* cw = (CxSubClassWnd*)GetProp(hWnd, _T("CxSubClassWnd"));
	return cw->WndProc(uMsg, wParam, lParam);
}
//�ı�ԭBUTTON�Ļص�����
LRESULT CALLBACK CxSubClassWnd::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	return CallWindowProc((WNDPROC)m_OldProc, m_hWnd, message, wParam, lParam); //Ĭ�ϻص�����
}

void CxSubClassWnd::MoveWindow(int x, int y, int cx, int cy)
{
	SetWindowPos(m_hWnd, 0, x, y, cx, cy, SWP_NOZORDER);
}

void CxSubClassWnd::SetFont(HFONT font)
{
	SendMessage(m_hWnd, WM_SETFONT, (WPARAM)font, 0);
}