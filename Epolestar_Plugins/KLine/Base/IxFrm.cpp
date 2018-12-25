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
		frm = (TIxFrm*)((LPCREATESTRUCT)lParam)->lpCreateParams;
		frm->m_Hwnd = hwnd;
		SetWindowLongPtr(hwnd, 0, (LONG)frm);
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

void TIxFrm::CreateFrm(LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle)
{
	WNDCLASSEX wndclass;
	wndclass.cbClsExtra = 0;						//类扩展数据大小，一般不用
	wndclass.cbWndExtra = sizeof(void*);			//窗口扩展数据大小，一般不用
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.hInstance = GetModuleHandle(NULL);		//应用程序实例，这里没用
	wndclass.hIcon = NULL;							//应用程序的图标句柄
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW); //应用程序的光标句柄
	wndclass.hbrBackground = NULL;					//防止界面闪烁，置空背景画刷 (HBRUSH)GetStockObject(WHITE_BRUSH);//CreateSolidBrush(RGB(45, 45, 48));//
	wndclass.lpszMenuName = NULL;					//菜单名，该名以资源的方式存在，这里没用
	wndclass.lpszClassName = classname;				//窗口类的名字，这个必须指定且不能重复，否则会失败，除非你需要重复的，否则尽量避免
	wndclass.hIconSm = NULL;
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;     //指示类窗口类的风格，比如指定水平、垂直重画；是否显示关闭窗口等
	wndclass.lpfnWndProc = IxFrmProc;				//窗口过程函数指针，这个是最重要的参数，该参数指定消息执行的处理函数入口
	RegisterClassEx(&wndclass);

	CreateWindowEx(
		exstyle, 
		classname, 
		0, 
		style,
		0, 
		0, 
		0, 
		0, 
		parent, 
		NULL, 
		GetModuleHandle(NULL), 
		this);

	HWND hwnd = GetParent();
	hwnd = NULL;
}
