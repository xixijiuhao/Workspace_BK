#include "PreInclude.h"

extern G_COLOREF g_ColorRefData;

CMousePriceEdit::CMousePriceEdit(unsigned int &pos) :m_nCursorPosRef(pos)
{
	m_bTracking = m_bHover = m_bFocus = FALSE;
}
CMousePriceEdit::~CMousePriceEdit()
{
}
void CMousePriceEdit::Init(HWND hwnd)
{
	CxSubClassWnd::Init(hwnd);
}
void CMousePriceEdit::DrawItemFrame(bool bHover, bool bFocus)
{
	HDC hDC = ::GetWindowDC(m_hWnd);
	RECT rect, rectClient;
	::GetWindowRect(m_hWnd, &rect);
	rectClient.left = 0;
	rectClient.top = 0;
	rectClient.right = rect.right - rect.left;
	rectClient.bottom = rect.bottom - rect.top;

	if (bHover || bFocus)
		::FrameRect(hDC, &rectClient, g_ColorRefData.GetHbrLightBlue());
	else
		::FrameRect(hDC, &rectClient, g_ColorRefData.GetHbrRGB150());
	
	::ReleaseDC(m_hWnd, hDC);
}
LRESULT CALLBACK CMousePriceEdit::WndProc(UINT message, WPARAM w, LPARAM l)
{
	//��ϣ������������1

	static bool	bClick = false;		//����
	static bool	bDbClick = false;	//˫��
	static bool	bTimer = false;		//��ʱ������

	switch (message)
	{
		case WM_SETFOCUS:
		{
			m_bFocus = TRUE;
			DrawItemFrame(m_bHover, m_bFocus);

			bClick = false;
			bDbClick = false;
			bTimer = false;
		}
		break;
		case WM_LBUTTONDOWN:
		{
			SetFocus(GetHwnd());//������Ҫ
			SetTimer(GetHwnd(), 1, ::GetDoubleClickTime() / 4, NULL);
			bTimer = true;
		}
		break;
		case WM_LBUTTONUP:
		{
			if (bTimer)
				bClick = true;
			else
			{
				bTimer = false;
				KillTimer(GetHwnd(), 1);	//�ر�ʱ��
				bClick = false;
			}
		}
		break;
		case WM_LBUTTONDBLCLK:
		{
			bClick = false;
			if (bTimer)
			{
				KillTimer(GetHwnd(), 1);
				bTimer = false;
			}
			Edit_SetSel(GetHwnd(), 0, -1);
		}
		break;
		case WM_TIMER:
		{
			if (bClick)
			{
				bClick = false;
				char strText[20] = {0};
				GetWindowTextA(GetHwnd(), strText,20);
				KillTimer(GetHwnd(), 1);
				int nStart = 0, nEnd = 0;
				::SendMessage(GetHwnd(), EM_GETSEL, (WPARAM)&nStart, (LPARAM)&nEnd);//��ȡѡ���ı��Ŀ�ʼ�����λ�ã�������nStart,nEnd
				if (((nEnd - nStart) > 0) || ((nEnd - nStart) > (int)m_nCursorPosRef))
					return false;
				if (m_nCursorPosRef <= 0)
					return false;

				if ((UINT)strlen(strText) < m_nCursorPosRef)
					Edit_SetSel(GetHwnd(), 0, -1);
				else
					Edit_SetSel(GetHwnd(), (int)(strlen(strText) - m_nCursorPosRef), (int)strlen(strText));
			}
		}
		break;
		case WM_NCPAINT:
		case WM_PAINT:
		{
			CallWindowProc((WNDPROC)m_OldProc, m_hWnd, message, w, l); //Ĭ�ϻص�����
			DrawItemFrame(m_bHover, m_bFocus);
			return 1;
		}
		break;
		case WM_MOUSEMOVE :
		{
			if (!m_bTracking)
			{
				TRACKMOUSEEVENT tme;
				tme.cbSize = sizeof(tme);
				tme.hwndTrack = m_hWnd;
				tme.dwFlags = TME_LEAVE | TME_HOVER;
				tme.dwHoverTime = 50;
				m_bTracking =_TrackMouseEvent(&tme)==TRUE;
			}
		}
		break;
		case WM_MOUSELEAVE:
		{
			m_bTracking = FALSE;
			m_bHover = FALSE;
			DrawItemFrame(m_bHover, m_bFocus);
		}
		break;
		case WM_MOUSEHOVER:
		{
			m_bHover = TRUE;
			DrawItemFrame(m_bHover, m_bFocus);
		}
		break;
		case WM_KILLFOCUS:
		{
			m_bFocus = FALSE;
			DrawItemFrame(m_bHover, m_bFocus);
		}
		break;
		default:
			break;
	}
	return CallWindowProc((WNDPROC)m_OldProc, m_hWnd, message, w, l); //Ĭ�ϻص�����
}
