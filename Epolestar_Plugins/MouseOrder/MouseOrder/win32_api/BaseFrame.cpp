#include "PreInclude.h"
HCURSOR CURSOR_ARROW;
HCURSOR CURSOR_WE;		//横向
HCURSOR CURSOR_NS;		//纵向
HCURSOR CURSOR_WSE;		//左上和右下
HCURSOR CURSOR_ESW;		//左下和右上
HCURSOR CURSOR_HAND;	//手势

TBaseFrame::TBaseFrame() 
	: m_BeginHitTest(HTCLIENT)
{
	m_RestoreWindow.left = 0;
	m_RestoreWindow.right = MIN_WIDTH;
	m_RestoreWindow.top = 0;
	m_RestoreWindow.bottom = MIN_HEIGHT;
}

TBaseFrame::~TBaseFrame()
{
	DestroyWindow(m_Hwnd);
}

void TBaseFrame::Show()
{
	CURSOR_ARROW = LoadCursor(NULL, IDC_ARROW);
	CURSOR_WE = LoadCursor(NULL, IDC_SIZEWE);
	CURSOR_NS = LoadCursor(NULL, IDC_SIZENS);
	CURSOR_WSE = LoadCursor(NULL, IDC_SIZENWSE);
	CURSOR_ESW = LoadCursor(NULL, IDC_SIZENESW);
	CURSOR_HAND = LoadCursor(NULL, IDC_HAND);
	//创建窗口，设置标题和图标属性
	CreateFrm(0, WS_POPUP | WS_CLIPCHILDREN | WS_MINIMIZEBOX);// | WS_SYSMENU | WS_MAXIMIZEBOX);
	SetWindowText(m_Hwnd,L"");
	SetClassLongPtr(m_Hwnd, GCL_HICON, (LONG)LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(101)));
	SetCenterShow(GetHwnd());
}

void TBaseFrame::ShowMinimize()
{
	ShowWindow(m_Hwnd, SW_MINIMIZE);
}

const long DEFAULT_DPI = 96;

void SCALING_DPI_WND(DEVMODE& dm,RECT& rect)
{
	if (dm.dmLogPixels > DEFAULT_DPI)
	{
		rect.left *= DEFAULT_DPI;
		rect.left /= dm.dmLogPixels;
		rect.top *= DEFAULT_DPI;
		rect.top /= dm.dmLogPixels;
		rect.right *= DEFAULT_DPI;
		rect.right /= dm.dmLogPixels;
		rect.bottom *= DEFAULT_DPI;
		rect.bottom /= dm.dmLogPixels;
	}
}

//处理开始菜单栏
BOOL CALLBACK EnumShellTrayWndProc(_In_ HWND hwnd, _In_ LPARAM lParam)
{
	DEVMODE& dm(*(DEVMODE*)lParam);

	//判断类名称，符合规则 Shell_%sTrayWnd 格式，%s可能为空或Secondary
	wchar_t cname[51];
	GetClassName(hwnd, cname, sizeof(cname) / sizeof(wchar_t));

	if (0 != wcsncmp(cname, L"Shell_", 6))
		return TRUE;

	int len(wcsnlen(cname, sizeof(cname) / sizeof(wchar_t)-1));
	if (len < 7) //不够TrayWnd长度
		return TRUE;

	if (0 != wcsncmp(&cname[len - 7], L"TrayWnd", 7))
		return TRUE;

	//判断是否在当前屏幕
	RECT wr;
	GetWindowRect(hwnd, &wr);

	if (wr.right - wr.left >(int)dm.dmPelsWidth || wr.bottom - wr.top > (int)dm.dmPelsHeight)
		SCALING_DPI_WND(dm,wr);
	
	if (wr.left < dm.dmPosition.x || wr.top < dm.dmPosition.y
		|| wr.right > dm.dmPosition.x + (int)dm.dmPelsWidth || wr.bottom > dm.dmPosition.y + (int)dm.dmPelsHeight)
		return TRUE;

	//矫正dm，判断任务栏 贴合位置，因为三边贴边，判断不贴的那个边
	if (wr.top != dm.dmPosition.y) //底部
	{
		dm.dmPelsHeight -= (wr.bottom - wr.top);
	}
	else if (wr.bottom != dm.dmPosition.y + dm.dmPelsHeight) //顶部
	{
		dm.dmPelsHeight -= (wr.bottom - wr.top);
		dm.dmPosition.y += (wr.bottom - wr.top);
	}
	else if (wr.left != dm.dmPosition.x) //右边
	{
		dm.dmPelsWidth -= (wr.right - wr.left);
	}
	else if (wr.right != dm.dmPosition.x + dm.dmPelsWidth) //左边
	{
		dm.dmPelsWidth -= (wr.right - wr.left);
		dm.dmPosition.x += (wr.right - wr.left);
	}

	return FALSE;
}

void SCALING_DPI(DEVMODE& dm)
{
	if (dm.dmLogPixels > DEFAULT_DPI)
	{
		dm.dmPosition.x *= DEFAULT_DPI;
		dm.dmPosition.x /= dm.dmLogPixels;
		dm.dmPosition.y *= DEFAULT_DPI;
		dm.dmPosition.y /= dm.dmLogPixels;
		dm.dmPelsWidth *= DEFAULT_DPI;
		dm.dmPelsWidth /= dm.dmLogPixels;
		dm.dmPelsHeight *= DEFAULT_DPI;
		dm.dmPelsHeight /= dm.dmLogPixels;
	}
}
void GetCurrScreenRect(RECT& r)
{
	POINT cp;
	GetCursorPos(&cp);

	DISPLAY_DEVICE dd;
	memset(&dd, 0, sizeof(DISPLAY_DEVICE));
	dd.cb = sizeof(DISPLAY_DEVICE);

	DEVMODE dm;
	memset(&dm, 0, sizeof(DEVMODE));
	dm.dmSize = sizeof(DEVMODE);
	dm.dmDriverExtra = 0;

	for (int i = 0; EnumDisplayDevices(0, i, &dd, 0); i++)
	{
		if (!EnumDisplaySettingsEx(dd.DeviceName, ENUM_CURRENT_SETTINGS, &dm, 0))
			continue;

		SCALING_DPI(dm);

		if (0 == dm.dmPosition.x && 0 == dm.dmPosition.y)
		{
			r.left = dm.dmPosition.x;
			r.top = dm.dmPosition.y;
			r.right = r.left + dm.dmPelsWidth;
			r.bottom = r.top + dm.dmPelsHeight;
		}

		//在此设备屏幕内部，不能判断去掉任务栏后的位置和尺寸，任务栏可能在顶部
		if (cp.x >= dm.dmPosition.x && cp.y >= dm.dmPosition.y
			&& cp.x < dm.dmPosition.x + (int)dm.dmPelsWidth && cp.y < dm.dmPosition.y + (int)dm.dmPelsHeight)
		{
			r.left = dm.dmPosition.x;
			r.top = dm.dmPosition.y;
			r.right = r.left + dm.dmPelsWidth;
			r.bottom = r.top + dm.dmPelsHeight;
			return;
		}
	}
}
void TBaseFrame::CloseFrame()
{
	DestroyWindow(m_Hwnd);
}
void TBaseFrame::SetFramePos(int x, int y, int cx, int cy, UINT flag)
{
	if (0 == (flag & SWP_NOMOVE))
	{
		m_RestoreWindow.left = x;
		m_RestoreWindow.top = y;
	}

	if (0 == (flag & SWP_NOSIZE))
	{
		m_RestoreWindow.right = cx;
		m_RestoreWindow.bottom = cy;
	}

	SetWindowPos(m_Hwnd, 0, x, y, cx, cy, flag);
	//子窗口调整
	AdjustChildrenPos();
}
void TBaseFrame::CheckAndSetMinimize()
{
	RECT wr;
	GetWindowRect(m_Hwnd, &wr);

	DISPLAY_DEVICE dd;
	memset(&dd, 0, sizeof(DISPLAY_DEVICE));
	dd.cb = sizeof(DISPLAY_DEVICE);

	DEVMODE dm;
	memset(&dm, 0, sizeof(DEVMODE));
	dm.dmSize = sizeof(DEVMODE);

	int w = MIN_WIDTH;
	int h = MIN_HEIGHT;

	POINT ltp;	//左上角
	ltp.x = wr.left;
	ltp.y = wr.top;

	POINT rtp;	//右上角
	rtp.x = wr.right - 1;
	rtp.y = wr.top;

	POINT rbp;	//右下角
	rbp.x = wr.right - 1;
	rbp.y = wr.bottom - 1;

	for (int i = 0; EnumDisplayDevices(0, i, &dd, 0); i++)
	{
		if (!EnumDisplaySettingsEx(dd.DeviceName, ENUM_CURRENT_SETTINGS, &dm, 0))
			continue;

		SCALING_DPI(dm);

		if (0 == dm.dmPosition.x && 0 == dm.dmPosition.y)
		{
			w = dm.dmPelsWidth / 2;
			h = dm.dmPelsHeight / 2;
		}

		//左上角或右上角在屏幕内，不调整2016-12-19
		if ((ltp.x >= dm.dmPosition.x && ltp.x < dm.dmPosition.x + (int)dm.dmPelsWidth
			&& ltp.y >= dm.dmPosition.y && ltp.y < dm.dmPosition.y + (int)dm.dmPelsHeight)
			||
			(rtp.x >= dm.dmPosition.x && rtp.x < dm.dmPosition.x + (int)dm.dmPelsWidth
				&& rtp.y >= dm.dmPosition.y && rtp.y < dm.dmPosition.y + (int)dm.dmPelsHeight))
			return;

	}

	//修改窗口位置为0，0
	SetFramePos(0, 0, w, h, SWP_NOZORDER | SWP_NOSENDCHANGING);
}
void TBaseFrame::SetCenterShow(HWND hWnd)
{
	long scrWidth, scrHeight;
	RECT rect, reCurScreen;
	GetCurrScreenRect(reCurScreen);
	scrWidth = reCurScreen.right - reCurScreen.left;
	scrHeight = reCurScreen.bottom - reCurScreen.top;
	GetWindowRect(hWnd, &rect);
	SetWindowPos(hWnd, HWND_TOPMOST, reCurScreen.left + (scrWidth - (m_RestoreWindow.right- m_RestoreWindow.left)) / 2, reCurScreen.top + (scrHeight - (m_RestoreWindow.bottom - m_RestoreWindow.top)) / 2, (m_RestoreWindow.right - m_RestoreWindow.left), (m_RestoreWindow.bottom - m_RestoreWindow.top), SWP_NOACTIVATE | SWP_SHOWWINDOW);
}
void TBaseFrame::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
	//左键按下，移动窗口，边框调整大小
	if (m_Hwnd == GetCapture())	//(MK_LBUTTON == wParam) && (
	{
		POINT MoveEndPoint;
		GetCursorPos(&MoveEndPoint);

		long dx(MoveEndPoint.x - m_MoveBeginPoint.x);
		long dy(MoveEndPoint.y - m_MoveBeginPoint.y);

		if (0 == dx && 0 == dy)	//双击或无位移时不处理
			return;

		RECT wr;
		GetWindowRect(m_Hwnd, &wr);
		wr.right -= wr.left;
		wr.bottom -= wr.top;

		//注意控制最小尺寸
		switch (m_BeginHitTest)
		{
		case HTTOP:
			if (wr.bottom - dy >= MIN_HEIGHT)
			{
				SetFramePos(wr.left, wr.top + dy, wr.right, wr.bottom - dy, SWP_NOZORDER | SWP_NOSENDCHANGING);
				m_MoveBeginPoint = MoveEndPoint;
			}
			break;
		case HTBOTTOM:
			if (wr.bottom + dy >= MIN_HEIGHT)
			{
				SetFramePos(0, 0, wr.right, wr.bottom + dy, SWP_NOZORDER | SWP_NOMOVE | SWP_NOSENDCHANGING);
				m_MoveBeginPoint = MoveEndPoint;
			}
			break;
		case HTLEFT:
			if (wr.right - dx >= MIN_WIDTH)
			{
				SetFramePos(wr.left + dx, wr.top, wr.right - dx, wr.bottom, SWP_NOZORDER | SWP_NOSENDCHANGING);
				m_MoveBeginPoint = MoveEndPoint;
			}
			break;
		case HTRIGHT:
			if (wr.right + dx >= MIN_WIDTH)
			{
				SetFramePos(0, 0, wr.right + dx, wr.bottom, SWP_NOZORDER | SWP_NOMOVE | SWP_NOSENDCHANGING);
				m_MoveBeginPoint = MoveEndPoint;
			}
			break;
		case HTTOPLEFT:
			if (wr.bottom - dy >= MIN_HEIGHT && wr.right - dx >= MIN_WIDTH)
			{
				SetFramePos(wr.left + dx, wr.top + dy, wr.right - dx, wr.bottom - dy, SWP_NOZORDER | SWP_NOSENDCHANGING);
				m_MoveBeginPoint = MoveEndPoint;
			}
			else if (wr.bottom - dy >= MIN_HEIGHT)
			{
				SetFramePos(wr.left, wr.top + dy, wr.right, wr.bottom - dy, SWP_NOZORDER | SWP_NOSENDCHANGING);
				m_MoveBeginPoint.y = MoveEndPoint.y;
			}
			else if (wr.right - dx >= MIN_WIDTH)
			{
				SetFramePos(wr.left + dx, wr.top, wr.right - dx, wr.bottom, SWP_NOZORDER | SWP_NOSENDCHANGING);
				m_MoveBeginPoint.x = MoveEndPoint.x;
			}
			break;
		case HTTOPRIGHT:
			if (wr.bottom - dy >= MIN_HEIGHT && wr.right + dx >= MIN_WIDTH)
			{
				SetFramePos(wr.left, wr.top + dy, wr.right + dx, wr.bottom - dy, SWP_NOZORDER | SWP_NOSENDCHANGING);
				m_MoveBeginPoint = MoveEndPoint;
			}
			else if (wr.bottom - dy >= MIN_HEIGHT)
			{
				SetFramePos(wr.left, wr.top + dy, wr.right, wr.bottom - dy, SWP_NOZORDER | SWP_NOSENDCHANGING);
				m_MoveBeginPoint.y = MoveEndPoint.y;
			}
			else if (wr.right + dx >= MIN_WIDTH)
			{
				SetFramePos(0, 0, wr.right + dx, wr.bottom, SWP_NOZORDER | SWP_NOMOVE | SWP_NOSENDCHANGING);
				m_MoveBeginPoint.x = MoveEndPoint.x;
			}
			break;
		case HTBOTTOMLEFT:
			if (wr.bottom + dy >= MIN_HEIGHT && wr.right - dx >= MIN_WIDTH)
			{
				SetFramePos(wr.left + dx, wr.top, wr.right - dx, wr.bottom + dy, SWP_NOZORDER | SWP_NOSENDCHANGING);
				m_MoveBeginPoint = MoveEndPoint;
			}
			else if (wr.bottom + dy >= MIN_HEIGHT)
			{
				SetFramePos(0, 0, wr.right, wr.bottom + dy, SWP_NOZORDER | SWP_NOMOVE | SWP_NOSENDCHANGING);
				m_MoveBeginPoint.y = MoveEndPoint.y;
			}
			else if (wr.right - dx >= MIN_WIDTH)
			{
				SetFramePos(wr.left + dx, wr.top, wr.right - dx, wr.bottom, SWP_NOZORDER | SWP_NOSENDCHANGING);
				m_MoveBeginPoint.x = MoveEndPoint.x;
			}
			break;
		case HTBOTTOMRIGHT:
			if (wr.bottom + dy >= MIN_HEIGHT && wr.right + dx >= MIN_WIDTH)
			{
				SetFramePos(0, 0, wr.right + dx, wr.bottom + dy, SWP_NOZORDER | SWP_NOMOVE | SWP_NOSENDCHANGING);
				m_MoveBeginPoint = MoveEndPoint;
			}
			else if (wr.bottom + dy >= MIN_HEIGHT)
			{
				SetFramePos(0, 0, wr.right, wr.bottom + dy, SWP_NOZORDER | SWP_NOMOVE | SWP_NOSENDCHANGING);
				m_MoveBeginPoint.y = MoveEndPoint.y;
			}
			else if (wr.right + dx >= MIN_WIDTH)
			{
				SetFramePos(0, 0, wr.right + dx, wr.bottom, SWP_NOZORDER | SWP_NOMOVE | SWP_NOSENDCHANGING);
				m_MoveBeginPoint.x = MoveEndPoint.x;
			}
			break;
		default:
			SetFramePos(wr.left + dx, wr.top + dy, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOSENDCHANGING);
			m_MoveBeginPoint = MoveEndPoint;
			break;
		}
			
	}
}

void TBaseFrame::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
	//获取窗口移动起点
	GetCursorPos(&m_MoveBeginPoint);
	m_BeginHitTest = TestHit();

	SetCapture(m_Hwnd);
}

void TBaseFrame::OnLButtonUp(WPARAM wParam, LPARAM lParam)
{
	ReleaseCapture();
}

void TBaseFrame::OnLButtonDbClk(WPARAM wParam, LPARAM lParam)
{
	
}

void TBaseFrame::OnSetCursor(WPARAM wParam, LPARAM lParam)
{
	switch (TestHit())
	{
	case HTTOPLEFT:
	case HTBOTTOMRIGHT:
		if (CURSOR_WSE != GetCursor())
			SetCursor(CURSOR_WSE);
		break;
	case HTTOPRIGHT:
	case HTBOTTOMLEFT:
		if (CURSOR_ESW != GetCursor())
			SetCursor(CURSOR_ESW);
		break;
	case HTTOP:
	case HTBOTTOM:
		if (CURSOR_NS != GetCursor())
			SetCursor(CURSOR_NS);
		break;
	case HTLEFT:
	case HTRIGHT:
		if (CURSOR_WE != GetCursor())
			SetCursor(CURSOR_WE);
		break;
	default:
		if (CURSOR_ARROW != GetCursor())
			SetCursor(CURSOR_ARROW);
		break;
	}
}

int TBaseFrame::TestHit()
{
	POINT cp;
	GetCursorPos(&cp);

	RECT wr;
	GetWindowRect(m_Hwnd, &wr);


	if (cp.x >= wr.left && cp.x < wr.left + BORDER_WIDTH)  //x在左边
	{
		if (cp.y >= wr.top && cp.y < wr.top + BORDER_WIDTH) //y在顶部
		{
			return HTTOPLEFT;
		}
		else if (cp.y < wr.bottom && cp.y >= wr.bottom - BORDER_WIDTH) //y在底部
		{
			return HTBOTTOMLEFT;
		}
		else if (cp.y >= wr.top + BORDER_WIDTH && cp.y < wr.bottom - BORDER_WIDTH) //y在中间
		{
			return HTLEFT;
		}
	}
	else if (cp.x < wr.right && cp.x >= wr.right - BORDER_WIDTH) //x在右边
	{
		if (cp.y >= wr.top && cp.y < wr.top + BORDER_WIDTH) //y在顶部
		{
			return HTTOPRIGHT;
		}
		else if (cp.y < wr.bottom && cp.y >= wr.bottom - BORDER_WIDTH) //y在底部
		{
			return HTBOTTOMRIGHT;
		}
		else if (cp.y >= wr.top + BORDER_WIDTH && cp.y < wr.bottom - BORDER_WIDTH) //y在中间
		{
			return HTRIGHT;
		}
	}
	else if (cp.x >= wr.left + BORDER_WIDTH && cp.x < wr.right - BORDER_WIDTH) //x在中间
	{
		if (cp.y >= wr.top && cp.y < wr.top + BORDER_WIDTH) //y在顶部
		{
			return HTTOP;
		}
		else if (cp.y < wr.bottom && cp.y >= wr.bottom - BORDER_WIDTH) //y在底部
		{
			return HTBOTTOM;
		}
	}

	return HTCLIENT;
}
