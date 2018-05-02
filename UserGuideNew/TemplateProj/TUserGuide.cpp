#include "BaseInclude.h"

#define ImageFilePath   L"\\images\\"
TUserGuide * g_pUserGuide = nullptr;

#define btnWidth  175
#define btnHeight 39
#define btnNextWidth 70
#define btnNextHeight 31

#define LayoutImgStartIndex  6
#define LayOutImgEndIndex    9

#define zoomThreshold      1.2
#define zoomOutPercent     0.8

char szCHS[][20] = { "上一步", "下一步", "取消向导", "学习教程", "跳过使用教程", "跳过", "取消", "完成" };
char szCHT[][20] = { "上壹步", "下壹步", "取消向導", "學習教程", "跳過使用教程", "跳過", "取消", "完成" };


enum {
	LastStep_Index,
	NextStep_Index,
	CancelGuide_Index,
	LearnTeaching_Index,
	SkipTeaching_Index,
	Skip_Index,
	Cancel_Index,
	Finish_Index,
};

char * _GetText(int i)
{
	if (g_language == CHT)
		return szCHT[i];
	return szCHS[i];
}

TUserGuide::TUserGuide()
{
	m_dZoomOutPercent = 1;
}

void TUserGuide::ShowFrm()
{
	for (int i = 0; i < ARRAYSIZE(m_szImgName); i++)
	{
		if (g_language == CHT)
			wsprintf(m_szImgName[i], L"%dT.jpg", i);
		else
			wsprintf(m_szImgName[i], L"%d.jpg", i);
	}
	Gdiplus::GdiplusStartupInput    m_Gdistart;
	Gdiplus::GdiplusStartup(&gdiplusStartupToken, &m_Gdistart, NULL);

	GetCurrentDirectory(ARRAYSIZE(m_szImgFilePath), m_szImgFilePath);
	lstrcat(m_szImgFilePath, ImageFilePath);
	lstrcat(m_szImgFilePath, m_szImgName[0]);
	m_Img = Image::FromFile(m_szImgFilePath);

	m_frmRect.X = 0;
	m_frmRect.Y = 0;
	m_frmRect.Width = m_Img->GetWidth();
	m_frmRect.Height = m_Img->GetHeight();

	RECT rect;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);
	
	int areaWidth = rect.right - rect.left;
	int areaHeight = rect.bottom - rect.top;

	if (areaWidth < m_frmRect.Width*zoomThreshold || areaHeight < m_frmRect.Height*zoomThreshold) //
	{
		m_frmRect.Width *= zoomOutPercent;
		m_frmRect.Height *= zoomOutPercent;
		m_dZoomOutPercent = zoomOutPercent;
	}

	m_rcClose.top = 0;
	m_rcClose.left = m_frmRect.Width - 25;
	m_rcClose.bottom = 25;
	m_rcClose.right = m_frmRect.Width;

	HWND hwnd = FindWindow(L"class TMainFrame", NULL);
	CreateFrm(L"StructFrameUG", hwnd, WS_VISIBLE | WS_POPUP | WS_CLIPCHILDREN | WS_SYSMENU, WS_EX_TOOLWINDOW | WS_EX_CONTROLPARENT);
	rect = { (GetSystemMetrics(SM_CXSCREEN) - m_frmRect.Width) / 2, 
		(GetSystemMetrics(SM_CYSCREEN) - m_frmRect.Height) / 2,
		(GetSystemMetrics(SM_CXSCREEN) + m_frmRect.Width) / 2, 
		(GetSystemMetrics(SM_CYSCREEN) + m_frmRect.Height) / 2 };
	SetWindowPos(m_Hwnd, HWND_TOP, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_SHOWWINDOW);
}

void TUserGuide::GetImgPath(const wchar_t* pszPath)
{
	TCHAR * cp_pos = wcsrchr(m_szImgFilePath, L'\\');
	if (0 != cp_pos)
		cp_pos[1] = 0;
	wcscat_s(m_szImgFilePath, pszPath);
}
void TUserGuide::DrawCloseBtn(TMemDC* pmemdc)
{
	RECT rect;
	memcpy_s(&rect, sizeof(RECT), &m_rcClose, sizeof(RECT));
	if (m_bhoverclose)
	{

		HBRUSH hbrClose = CreateSolidBrush(RGB(150, 30, 0));
		FillRect(pmemdc->GetHdc(), &rect, hbrClose);
		DeleteObject(hbrClose);
	}
	InflateRect(&rect, -8, -8);
	HPEN hpen = CreatePen(PS_SOLID, 2, m_colorBK);
	SelectObject(pmemdc->GetHdc(), hpen);
	MoveToEx(pmemdc->GetHdc(), rect.left, rect.top, nullptr);
	LineTo(pmemdc->GetHdc(), rect.right, rect.bottom);
	MoveToEx(pmemdc->GetHdc(), rect.left, rect.bottom, nullptr);
	LineTo(pmemdc->GetHdc(), rect.right, rect.top);

	DeleteObject(hpen);
}
void TUserGuide::SetImgPath(int index)
{
	WCHAR* cp_pos = wcsrchr(m_szImgFilePath, L'\\');
	if (cp_pos != 0)
		cp_pos[0] = 0;
	lstrcat(m_szImgFilePath, m_szImgName[index]);
}
LRESULT TUserGuide::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		OnCreate();
		return PROCESSED;
	case WM_PAINT:
		OnPaint();
		return PROCESSED;
	case WM_NCDESTROY:
		Gdiplus::GdiplusShutdown(gdiplusStartupToken);    
		return PROCESSED;
	case WM_LBUTTONDOWN:
		OnLButtonDown(wParam, lParam);
		return PROCESSED;
	case WM_MOUSEMOVE:
		OnMouseMove(wParam, lParam);
		return PROCESSED;
	case WM_MOUSELEAVE:
		OnMouseLeave(wParam, lParam);
		return PROCESSED;
	case SSWM_STATIC_BUTTON_CLICKDOWN:
		OnClickBtn(wParam, lParam);
		return PROCESSED;
	default:
		return NOT_PROCESSED;
	}
	return NOT_PROCESSED;
}
void TUserGuide::OnClickBtn(WPARAM wParam, LPARAM lParam)
{
	static int setLayOutIndex = 0;

	if (m_iImgIndex >= LayoutImgStartIndex &&m_iImgIndex < totalImgNum - 1 && lParam == ID_Btn3)
		setLayOutIndex = m_iImgIndex;
	//	else
	//		setLayOutIndex = 0;

	if (m_iImgIndex == 0)
	{
		switch (lParam)
		{
		case ID_Btn1:
			ShowWindow(m_Hwnd, SW_HIDE);
			break;
		case ID_Btn2:
			m_iImgIndex = 1;
			break;
		case ID_Btn3:
			m_iImgIndex = LayoutImgStartIndex;
			break;
		}
	}
	else if (m_iImgIndex > 0 && m_iImgIndex < LayoutImgStartIndex)
	{
		switch (lParam)
		{
		case ID_Btn4:
			m_iImgIndex--;
			break;
		case ID_Btn3:
			m_iImgIndex = LayoutImgStartIndex;
			break;
		case ID_Btn5:
			m_iImgIndex++;
			break;
		}
	}
	else if (m_iImgIndex >= LayoutImgStartIndex && m_iImgIndex <= LayOutImgEndIndex)
	{

		switch (lParam)
		{
		case ID_Btn1:
			ShowWindow(m_Hwnd, SW_HIDE);
			m_iImgIndex = 0;
			break;
		case ID_Btn2:
			m_iImgIndex = 0;
			break;
		case ID_Btn3:
			m_iImgIndex = totalImgNum - 1;
			break;
		}
	}
	else if (m_iImgIndex == totalImgNum - 1)
	{
		switch (lParam)
		{
		case ID_Btn1:
			ShowWindow(m_Hwnd, SW_HIDE);
			break;
		case ID_Btn2:
			m_iImgIndex = setLayOutIndex;
			break;
		case ID_Btn3:
			//
		{
			ShowWindow(m_Hwnd, SW_HIDE);

			if (g_pTKeyOrder)
				g_pTKeyOrder->Show(TKeyViewHide);

			g_pMainFrame->ResetLayout(setLayOutIndex - LayoutImgStartIndex + 1);
			int width = GetSystemMetrics(SM_CXSCREEN);

			if (setLayOutIndex == LayOutImgEndIndex - 1)
			{
				if (g_pTKeyOrder)
					g_pTKeyOrder->Show(TKeyViewShow);
			}
			m_iImgIndex = -1;
			break;
		}
		}
	}
	if (m_iImgIndex != -1)
	{
		GetImgPath(m_szImgName[m_iImgIndex]);
		m_Img = Image::FromFile(m_szImgFilePath);
		SetBtnPos();
		InvalidateRect(m_Hwnd, 0, false);
	}
}
void TUserGuide::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
	POINT pt = { LOWORD(lParam), HIWORD(lParam) };
	if (m_iImgIndex >= LayoutImgStartIndex&&PtInRect(&m_rcClose, pt))
	{
		ShowWindow(m_Hwnd, SW_HIDE);
	}
	else
	{
		PostMessage(m_Hwnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
	}
	OnSelLayout(pt);
}
void TUserGuide::OnSelLayout(POINT pt)
{
	if (m_iImgIndex >= LayoutImgStartIndex && m_iImgIndex <= LayOutImgEndIndex)
	{
		for (int i = 0; i < m_vecLayoutSelRc.size(); i++)
		{
			if (PtInRect(&m_vecLayoutSelRc[i], pt))
			{
				m_iImgIndex = LayoutImgStartIndex + i;
				GetImgPath(m_szImgName[m_iImgIndex]);
				m_Img = Image::FromFile(m_szImgFilePath);
				InvalidateRect(m_Hwnd, 0, false);
				break;
			}
		}
	}
	else if (m_iImgIndex == ARRAYSIZE(m_szImgName) - 1)
	{
		if (PtInRect(&m_rcLink, pt))
		{
			ShellExecute(NULL, L"open", L"http://www.epolestar.cn", NULL, NULL, SW_SHOW);
		}
	}
}
void TUserGuide::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
	if (!m_bMouseTrack)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = m_Hwnd;
		tme.dwHoverTime = 0;
		::TrackMouseEvent(&tme);
		m_bMouseTrack = true;
	}
	POINT pt;
	pt.x = LOWORD(lParam);
	pt.y = HIWORD(lParam);
	if (PtInRect(&m_rcClose, pt) && !m_bhoverclose)
	{
		m_bhoverclose = true;
		InvalidateRect(m_Hwnd, &m_rcClose, false);
	}
	else if (!PtInRect(&m_rcClose, pt) && m_bhoverclose)
	{
		m_bhoverclose = false;
		InvalidateRect(m_Hwnd, &m_rcClose, false);
	}

	SetCursorStyle(pt);
}
void TUserGuide::SetCursorStyle(POINT pt)
{
	if (m_iImgIndex >= LayoutImgStartIndex && m_iImgIndex <= LayOutImgEndIndex)
	{
		for (int i = 0; i < m_vecLayoutSelRc.size(); i++)
		{
			if (PtInRect(&m_vecLayoutSelRc[i], pt))
			{
				SetCursor(LoadCursor(NULL, IDC_HAND));
				return;
			}

		}
	}
	else if (m_iImgIndex == ARRAYSIZE(m_szImgName) - 1)
	{
		if (PtInRect(&m_rcLink, pt))
		{
			SetCursor(LoadCursor(NULL, IDC_HAND));
			return;
		}
	}
	SetCursor(LoadCursor(NULL, IDC_ARROW));

}
void TUserGuide::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	m_bhoverclose = false;
	m_bMouseTrack = false;
	InvalidateRect(m_Hwnd, NULL, false);
}
void TUserGuide::OnCreate()
{
	InitImgBtnRect(); //初始化button位置

	for (int i = 0; i < ARRAYSIZE(m_btnAction); i++)
	{
		m_btnAction[i].Create(m_Hwnd, "", ID_Btn1 + i);
		m_btnAction[i].SetFont(g_FontData.g_FontWord15);
	}
	m_btnAction[0].MoveWindow(m_rcBottomBtn[0].left, m_rcBottomBtn[0].top, m_rcBottomBtn[0].right - m_rcBottomBtn[0].left, m_rcBottomBtn[0].bottom - m_rcBottomBtn[0].top);
	m_btnAction[1].MoveWindow(m_rcBottomBtn[1].left, m_rcBottomBtn[1].top, m_rcBottomBtn[1].right - m_rcBottomBtn[1].left, m_rcBottomBtn[1].bottom - m_rcBottomBtn[1].top);
	m_btnAction[2].MoveWindow(m_rcBottomBtn[2].left, m_rcBottomBtn[2].top, m_rcBottomBtn[2].right - m_rcBottomBtn[2].left, m_rcBottomBtn[2].bottom - m_rcBottomBtn[2].top);
	m_btnAction[3].SetButtonText(_GetText(LastStep_Index));
	m_btnAction[4].SetButtonText(_GetText(NextStep_Index));

	Init();
}

void TUserGuide::OnPaint()
{
	TMemDC memdc(m_Hwnd);
	Gdiplus::Graphics graph(memdc.GetHdc());
	graph.DrawImage(m_Img, m_frmRect);
	if (m_iImgIndex >= LayoutImgStartIndex)
		DrawCloseBtn(&memdc);
}

void TUserGuide::OnMenuClk(const unsigned int MenuIndex, const HWND PastLife)
{
	if (!g_pUserGuide)
	{
		g_pUserGuide = new TUserGuide;
		g_pUserGuide->ShowFrm();
	}
	else
	{
		HWND hwnd = g_pUserGuide->GetHwnd();
		if (!IsWindowVisible(hwnd))
		{
			g_pUserGuide->Init();
			ShowWindow(hwnd, SW_SHOW);
		}
		else
			ShowWindow(hwnd, SW_HIDE);
	}
}
void TUserGuide::OnHotKeyDown(const bool Alt, const bool Ctrl, const bool Shift, const unsigned char VKey, const bool Exclusive, HWND source)
{
	OnMenuClk(0, 0);
}
void TUserGuide::InitImgBtnRect()
{
	//m_rcBottomBtn[0].left = 33;
	//m_rcBottomBtn[0].bottom = m_frmRect.Height - 14;
	//m_rcBottomBtn[0].right = m_rcBottomBtn[0].left + btnWidth;
	//m_rcBottomBtn[0].top = m_rcBottomBtn[0].bottom - btnHeight;

	//m_rcBottomBtn[2].left = 993;
	//m_rcBottomBtn[2].right = m_rcBottomBtn[2].left + btnWidth;
	//m_rcBottomBtn[2].top = m_rcBottomBtn[0].top;
	//m_rcBottomBtn[2].bottom = m_rcBottomBtn[0].bottom;

	//m_rcBottomBtn[1].left = 509;
	//m_rcBottomBtn[1].right = m_rcBottomBtn[1].left + btnWidth + 10;
	//m_rcBottomBtn[1].top = m_rcBottomBtn[0].top;
	//m_rcBottomBtn[1].bottom = m_rcBottomBtn[0].bottom;


	//RECT rcNext = { 0 };

	//POINT ptO = { 1560, 153 };
	//POINT potLast[] = { { 702, 711 },{ 1324, 675 },{ 732, 379 },{ 1102, 409 },{ 1056, 605 } };
	//POINT ptNext[] = { { 514, 711 },{ 1140, 675 },{ 544, 379 },{ 916, 409 },{ 872, 605 } };

	//for (int i = 0; i < ARRAYSIZE(ptNext); i++)
	//{
	//	rcNext.left = ptO.x - ptNext[i].x;
	//	rcNext.top = ptNext[i].y - ptO.y;
	//	rcNext.right = rcNext.left + btnNextWidth;
	//	rcNext.bottom = rcNext.top + btnNextHeight;

	//	m_vecNectStepRc.push_back(rcNext);

	//	rcNext.left = ptO.x - potLast[i].x;
	//	rcNext.right = rcNext.left + btnNextWidth;
	//	rcNext.top = potLast[i].y - ptO.y;
	//	rcNext.bottom = rcNext.top + btnNextHeight;
	//	m_vecLastStepRc.push_back(rcNext);
	//}

	//RECT rcLayout = { 0 };
	//rcLayout = { 1,73,221,203 };
	//m_vecLayoutSelRc.push_back(rcLayout);
	//rcLayout = { 1, 205, 221, 335 };
	//m_vecLayoutSelRc.push_back(rcLayout);
	//rcLayout = { 1, 337, 221, 468 };
	//m_vecLayoutSelRc.push_back(rcLayout);
	//rcLayout = { 1, 470, 221, 579 };
	//m_vecLayoutSelRc.push_back(rcLayout);
	//rcLayout = { 1, 581, 221, 731 };
	//m_vecLayoutSelRc.push_back(rcLayout);


	//m_rcLink = { 578,252,987,275 };





	m_rcBottomBtn[0].left = 33;
	m_rcBottomBtn[0].top = 900 - 153;
	m_rcBottomBtn[0].bottom = m_rcBottomBtn[0].top + btnHeight;
	m_rcBottomBtn[0].right = m_rcBottomBtn[0].left + btnWidth;
//	m_rcBottomBtn[0].top = m_rcBottomBtn[0].bottom - btnHeight;

	m_rcBottomBtn[2].left = 993;
	m_rcBottomBtn[2].right = m_rcBottomBtn[2].left + btnWidth;
	m_rcBottomBtn[2].top = m_rcBottomBtn[0].top;
	m_rcBottomBtn[2].bottom = m_rcBottomBtn[0].bottom;

	m_rcBottomBtn[1].left = 509;
	m_rcBottomBtn[1].right = m_rcBottomBtn[1].left + btnWidth + 10;
	m_rcBottomBtn[1].top = m_rcBottomBtn[0].top;
	m_rcBottomBtn[1].bottom = m_rcBottomBtn[0].bottom;


	for (int i = 0; i < ARRAYSIZE(m_rcBottomBtn); i++)
	{
		m_rcBottomBtn[i].left *= m_dZoomOutPercent;
		m_rcBottomBtn[i].right *= m_dZoomOutPercent;
		m_rcBottomBtn[i].top *= m_dZoomOutPercent;
		m_rcBottomBtn[i].bottom *= m_dZoomOutPercent;
	}

	RECT rcNext = { 0 };

	POINT ptO = { 1560, 153 };
	//POINT ptO = { 2560, 653 };
	POINT potLast[] = { { 702, 711 },{ 1324, 675 },{ 732, 379 },{ 1102, 409 },{ 1056, 605 } };
	POINT ptNext[] = { { 514, 711 },{ 1140, 675 },{ 544, 379 },{ 916, 409 },{ 872, 605 } };

	for (int i = 0; i < ARRAYSIZE(ptNext); i++)
	{
		rcNext.left = ptO.x - ptNext[i].x;
		rcNext.top = ptNext[i].y - ptO.y;
		rcNext.right = rcNext.left + btnNextWidth;
		rcNext.bottom = rcNext.top + btnNextHeight;


		rcNext.left *= m_dZoomOutPercent;
		rcNext.right *= m_dZoomOutPercent;
		rcNext.top *= m_dZoomOutPercent;
		rcNext.bottom *= m_dZoomOutPercent;

		m_vecNectStepRc.push_back(rcNext);

		rcNext.left = ptO.x - potLast[i].x;
		rcNext.right = rcNext.left + btnNextWidth;
		rcNext.top = potLast[i].y - ptO.y;
		rcNext.bottom = rcNext.top + btnNextHeight;

		rcNext.left *= m_dZoomOutPercent;
		rcNext.right *= m_dZoomOutPercent;
		rcNext.top *= m_dZoomOutPercent;
		rcNext.bottom *= m_dZoomOutPercent;

		m_vecLastStepRc.push_back(rcNext);
	}

	RECT rcLayout = { 0 };
	rcLayout = { 1,73,221,236 };
	m_vecLayoutSelRc.push_back(rcLayout);
	rcLayout = { 1, 238, 221, 399 };
	m_vecLayoutSelRc.push_back(rcLayout);
	rcLayout = { 1, 401, 221, 565 };
	m_vecLayoutSelRc.push_back(rcLayout);
	rcLayout = { 1, 567, 221, 730 };
	m_vecLayoutSelRc.push_back(rcLayout);


	for (int i = 0; i < m_vecLayoutSelRc.size(); i++)
	{
		m_vecLayoutSelRc[i].left *= m_dZoomOutPercent;
		m_vecLayoutSelRc[i].right *= m_dZoomOutPercent;
		m_vecLayoutSelRc[i].top *= m_dZoomOutPercent;
		m_vecLayoutSelRc[i].bottom *= m_dZoomOutPercent;
	}

	m_rcLink = { 578,252,987,275 };


	m_rcLink.left *= m_dZoomOutPercent;
	m_rcLink.right *= m_dZoomOutPercent;
	m_rcLink.top *= m_dZoomOutPercent;
	m_rcLink.bottom *= m_dZoomOutPercent;

}

void TUserGuide::Init()
{
	m_iImgIndex = 0;
	GetImgPath(m_szImgName[0]);
	m_Img = Image::FromFile(m_szImgFilePath);
	SetBtnPos();
}

void TUserGuide::SetBtnPos()
{
	int showIndex = 0;
	if (m_iImgIndex == 0)
	{

		m_btnAction[0].SetButtonText(_GetText(CancelGuide_Index));
		m_btnAction[1].SetButtonText(_GetText(LearnTeaching_Index));
		m_btnAction[2].SetButtonText(_GetText(SkipTeaching_Index));
		showIndex = BottomLeft_Btn | BottomRight_Btn | BottomCenter_Btn;
	}
	else if (m_iImgIndex > 0 && m_iImgIndex < LayoutImgStartIndex)
	{
		m_btnAction[2].SetButtonText(_GetText(Skip_Index));
		m_btnAction[3].MoveWindow(m_vecLastStepRc[m_iImgIndex - 1].left,
			m_vecLastStepRc[m_iImgIndex - 1].top,
			m_vecLastStepRc[m_iImgIndex - 1].right - m_vecLastStepRc[m_iImgIndex - 1].left,
			m_vecLastStepRc[m_iImgIndex - 1].bottom - m_vecLastStepRc[m_iImgIndex - 1].top);

		m_btnAction[4].MoveWindow(m_vecNectStepRc[m_iImgIndex - 1].left,
			m_vecNectStepRc[m_iImgIndex - 1].top,
			m_vecNectStepRc[m_iImgIndex - 1].right - m_vecNectStepRc[m_iImgIndex - 1].left,
			m_vecNectStepRc[m_iImgIndex - 1].bottom - m_vecNectStepRc[m_iImgIndex - 1].top);
		showIndex = BottomRight_Btn | Inside_NextBtn | Inside_LastBtn;
	}
	else if (m_iImgIndex >= LayoutImgStartIndex && m_iImgIndex <= LayOutImgEndIndex)
	{
		m_btnAction[0].SetButtonText(_GetText(CancelGuide_Index));
		m_btnAction[1].SetButtonText(_GetText(LastStep_Index));
		m_btnAction[2].SetButtonText(_GetText(NextStep_Index));
		showIndex = BottomLeft_Btn | BottomRight_Btn | BottomCenter_Btn;
	}
	else if (m_iImgIndex == totalImgNum - 1)
	{
		m_btnAction[0].SetButtonText(_GetText(Cancel_Index));
		m_btnAction[2].SetButtonText(_GetText(Finish_Index));
		showIndex = BottomLeft_Btn | BottomRight_Btn | BottomCenter_Btn;
	}

	for (int i = 0; i < ARRAYSIZE(m_btnAction); i++)
	{
		int index = 1 << i;
		if (index & showIndex)
		{
			ShowWindow(m_btnAction[i].GetHwnd(), SW_SHOW);
		}
		else
		{
			ShowWindow(m_btnAction[i].GetHwnd(), SW_HIDE);
		}
	}
}