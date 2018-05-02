#include "..\PreInclude.h"
using namespace std;

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
		if (SetProp(m_hWnd, L"CxSubClassWnd", (HANDLE)this) == 0)
		{
			return FALSE;
		}
		m_OldProc = SetWindowLong(m_hWnd, GWL_WNDPROC, (LONG)stdProc);//�ı䴰�ڻص�����

	}
	return FALSE;
}
int CxSubClassWnd::CxGetWindowText(wchar_t *w, unsigned int nLen)
{
	return GetWindowTextW(m_hWnd, w, nLen);
}
LRESULT WINAPI CxSubClassWnd::stdProc(HWND hWnd, UINT uMsg, UINT wParam, LONG lParam)
{
	CxSubClassWnd* cw = (CxSubClassWnd*)GetProp(hWnd, L"CxSubClassWnd");
	return cw->WndProc(uMsg, wParam, lParam);
}
//�ı�ԭBUTTON�Ļص�����
LRESULT CALLBACK CxSubClassWnd::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	return CallWindowProc((WNDPROC)m_OldProc, m_hWnd, message, wParam, lParam); //Ĭ�ϻص�����
}
