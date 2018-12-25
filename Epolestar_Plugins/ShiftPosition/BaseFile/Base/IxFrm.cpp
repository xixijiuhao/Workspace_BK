#include "IxFrm.h"

bool TIxFrm::GetInParentPoint(HWND hwnd, POINT& point)
{
	RECT r;
	POINT pp = { 0, 0 };

	GetWindowRect(hwnd, &r);

	HWND parent(::GetParent(hwnd));
	if (NULL != parent)
		ClientToScreen(parent, &pp);

	point.x = r.left - pp.x;
	point.y = r.top - pp.y;

	return true;
}

LRESULT CALLBACK TIxFrm::IxFrmProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT ret(NOT_PROCESSED);
	TIxFrm* frm(NULL);
	switch (message)
	{
	case WM_NCCREATE:
		frm = (TIxFrm*)((LPCREATESTRUCT)lParam)->lpCreateParams;				//lpCreateParams即为传入的this指针
		frm->m_Hwnd = hwnd;														//将类的句柄存入指针中
		SetWindowLongPtr(hwnd, 0, (LONG)frm);									//将指针放入窗口的句柄中
		break;
	default:
		frm = (TIxFrm*)GetWindowLongPtr(hwnd, 0);
		break;
	}

	if (NULL != frm)
	{
		ret = frm->WndProc(message, wParam, lParam);
	}

	if (NOT_PROCESSED == ret)
		ret = DefWindowProc(hwnd, message, wParam, lParam);
	return ret;
}

HWND TIxFrm::GetParent()
{
	return ::GetParent(m_Hwnd);
}

bool TIxFrm::GetInParentPoint(POINT& point)
{
	return TIxFrm::GetInParentPoint(m_Hwnd, point);
}

void TIxFrm::CreateFrm(LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle, LPCTSTR windowname)
{
	WNDCLASSEX wndclass;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = sizeof(void*);									//申请出的额外用于存放frm指针
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.hInstance = GetModuleHandle(NULL);
	wndclass.hIcon = NULL;
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = NULL;//防止界面闪烁，置空背景画刷 (HBRUSH)GetStockObject(WHITE_BRUSH);//CreateSolidBrush(RGB(45, 45, 48));//
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = classname;
	wndclass.hIconSm = NULL;
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wndclass.lpfnWndProc = IxFrmProc;
	RegisterClassEx(&wndclass);

	CreateWindowEx(exstyle, classname, windowname, style,
		0, 0, 0, 0, parent, NULL, GetModuleHandle(NULL), this);				//this 传入的创建的窗口
}
