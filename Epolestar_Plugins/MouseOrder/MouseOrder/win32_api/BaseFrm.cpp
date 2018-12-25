#include "PreInclude.h"

extern HCURSOR CURSOR_ARROW;

void TBaseFrm::Redraw()
{
	::RedrawWindow(m_Hwnd, 0, 0, RDW_INVALIDATE | RDW_UPDATENOW | RDW_NOCHILDREN);
}

LRESULT CALLBACK TBaseFrm::BaseFrmProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//���ɺͻ�ȡfrm
	TBaseFrm* frm(NULL);

	if (WM_NCCREATE == message)
	{
		frm = (TBaseFrm*)((LPCREATESTRUCT)lParam)->lpCreateParams;
		frm->m_Hwnd = hwnd;
		SetWindowLongPtr(hwnd, 0, (LONG)frm);
	}
	else
	{
		frm = (TBaseFrm*)GetWindowLongPtr(hwnd, 0);
	}

	if (NULL == frm)
		return DefWindowProc(hwnd, message, wParam, lParam);

	//��Ϣ����
	LRESULT ret(PROCESSED);

	switch (message)
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);

			RECT cr;
			GetClientRect(hwnd, &cr);

			HDC mdc = CreateCompatibleDC(hdc);
			SetBkMode(mdc, TRANSPARENT);

			HBITMAP bmp = CreateCompatibleBitmap(hdc, cr.right, cr.bottom);
			SelectObject(mdc, bmp);

			RECT& ur(ps.rcPaint);

			frm->OnPaint(mdc, cr, ur);

			BitBlt(hdc, ur.left, ur.top, ur.right - ur.left, ur.bottom - ur.top, mdc, ur.left, ur.top, SRCCOPY);

			DeleteObject(bmp);
			DeleteDC(mdc);

			EndPaint(hwnd, &ps);
		}
		break;
	case WM_MOUSEMOVE:
		//��������뿪
		if (!frm->m_MouseLeaveTrack)
		{
			TRACKMOUSEEVENT t;
			t.cbSize = sizeof(TRACKMOUSEEVENT);
			t.dwFlags = TME_LEAVE;
			t.dwHoverTime = 0;
			t.hwndTrack = hwnd;
			frm->m_MouseLeaveTrack = (TRUE == TrackMouseEvent(&t));
		}
		frm->OnMouseMove(wParam, lParam);
		break;
	case WM_SETCURSOR:
		frm->OnSetCursor(wParam, lParam);
		return TRUE;
		break;
	case WM_LBUTTONDOWN:
		frm->OnLButtonDown(wParam, lParam);
		break;
	case WM_LBUTTONUP:
		frm->OnLButtonUp(wParam, lParam);
		break;
	case WM_LBUTTONDBLCLK:
		frm->OnLButtonDbClk(wParam, lParam);
		break;
	case WM_MOUSELEAVE:
		frm->m_MouseLeaveTrack = false;
		frm->OnMouseLeave(wParam, lParam);
		break;
	case WM_KEYDOWN:
		frm->OnKeyDown(wParam, lParam);
		break;
	case WM_MOUSEWHEEL:
		frm->OnMouseWheel(wParam, lParam);
		break;
	case WM_KILLFOCUS:
		frm->OnKillFocus(wParam, lParam);
		break;
	case WM_CAPTURECHANGED:
		frm->OnCaptureChanged(wParam, lParam);
		break;
	case WM_ACTIVATE:
		frm->OnActive(wParam, lParam);
		break;
	case WM_SHOWWINDOW:
		frm->OnShowWindow(wParam, lParam);
		break;
	case WM_SIZE:
		frm->OnSize(wParam, lParam);
		break;
	case WM_DESTROY:
		frm->OnDestroy();
		break;
	case WM_NCDESTROY:
		frm->OnNcDestroy();
		break;
	default:
		ret = frm->OnMessage(message, wParam, lParam);
		break;
	}

	//��鴦����ֵ
	if (PROCESSED == ret)
		return ret;

	return DefWindowProc(hwnd, message, wParam, lParam);
}

void TBaseFrm::CreateFrm(HWND parent, DWORD style, DWORD exstyle)
{
	//�������뿪�¼�
	m_MouseLeaveTrack = false;

	//ȡ����
	char cname[51];
	strncpy_s(cname, typeid(*this).name(), sizeof(cname) - 1);
	wchar_t wname[51];
	MultiByteToWideChar(936, 0, cname, -1, wname, sizeof(wname)/sizeof(wchar_t));

	//���ֽڷ�ʽ�����Ĵ��ڣ������������ʹ��unicode��ʽ���᲻��������SetWindowText

	WNDCLASSEX wndclass;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = sizeof(void*);
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.hInstance = GetModuleHandle(NULL);
	wndclass.hIcon = NULL;
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = NULL;	//��ֹ������˸���ÿձ�����ˢ
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = wname;
	wndclass.hIconSm = NULL;
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wndclass.lpfnWndProc = BaseFrmProc;
	RegisterClassEx(&wndclass);

	CreateWindowEx(exstyle, wname, 0, style,
		0, 0, 0, 0, parent, NULL, GetModuleHandle(NULL), this);
}

void TBaseFrm::OnSetCursor(WPARAM wParam, LPARAM lParam)
{
	if (CURSOR_ARROW != GetCursor())
		SetCursor(CURSOR_ARROW);
}