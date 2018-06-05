#include "BaseInclude.h"

#define ImageFilePath   L"\\images\\"
#define UP_BIG	L"up_big.png"
#define UP_SMALL L"up_small.png"
#define DOWN_BIG L"down_big.png"
#define DOWN_SMALL L"down_small.png"

KLineView::KLineView():m_bMouseTract(false), m_bMouseDown(false), m_bShowCross(false)
{
}

KLineView::~KLineView()
{
}
void KLineView::CreateView(HWND parent, CRECT& rect)
{
	InitChartRect(rect);
	m_iParentHwnd = parent;
	CreateFrm(L"KLineView", parent, WS_VISIBLE | WS_CHILD);
	SetWindowPos(m_Hwnd, 0, rect.x, rect.y, rect.cx,  rect.cy, 0);

	Gdiplus::GdiplusStartupInput    m_Gdistart;
	Gdiplus::GdiplusStartup(&gdiplusStartupToken, &m_Gdistart, NULL);
}

void KLineView::MoveWindow(CRECT& rect)
{
	InitChartRect(rect);
	SetWindowPos(m_Hwnd, 0, rect.x, rect.y, rect.cx, rect.cy, 0);
}
void KLineView::UpdateKLineView()
{
	InvalidateRect(m_Hwnd, NULL, false);
}
LRESULT KLineView::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		OnCreate();
		return PROCESSED;
	case WM_PAINT:
		OnPaint();
		return PROCESSED;
	case WM_SIZE:
		OnSize();
		return PROCESSED;
	case WM_LBUTTONDOWN:
		OnLButtonDown(wParam, lParam);
		break;
	case WM_LBUTTONUP:
		OnLButtonUp(wParam, lParam);
		break;
	case WM_MOUSEMOVE:
		OnMouseMove(wParam, lParam);
		break;
	case WM_MOUSELEAVE:
		OnMouseLeave();
	case WM_NCDESTROY:
		Gdiplus::GdiplusShutdown(gdiplusStartupToken);
		return PROCESSED;
	default:
		break;
	}
	return NOT_PROCESSED;
}
void KLineView::OnCreate()
{

}

void KLineView::OnDealSetFocus()
{
	InvalidateRect(m_Hwnd, NULL, TRUE);
}

void KLineView::OnPaint()
{
	TMemDC dc(m_Hwnd);
	KLineSaveDC Save(dc);
	HBRUSH CenterBr = CreateSolidBrush(RGB(10, 10, 10));
	HBRUSH FrameBbr = CreateSolidBrush(RGB(39, 38, 46));

	FillRect(dc.GetHdc(), &m_clRect.CenterRect, CenterBr);

	DrawFrame(dc);
	if (m_clModel->m_vtKLineDrawData.size() && m_clModel->m_vtKLineDrawPoint.size()) 
	{
		DrawBottomTime(dc);//画底部时间轴
		DrawYAxisData(dc);//画Y轴
		DrawContractName(dc);//画合约名称
		DrawKLine(dc);//画K线
		DrawLastPriceLine(dc);//画最新价横线
		if (m_bShowCross) 
		{
			DrawCross(dc);
		}
		//阻力位、压力位、更新时间点，策略趋势
		if (m_clModel->m_httpData.articleID && m_clModel->m_httpData.valid) 
		{
			DrawTrend(dc);
		}
	}
	//屏蔽修改划线
	//FrameRect(dc.GetHdc(), &m_clRect.LeftRect, FrameBbr);
	RECT r = { 0 };
	GetClientRect(m_Hwnd, &r);
	//FrameRect(dc.GetHdc(), &r, FrameBbr);
	DeleteObject(FrameBbr);
	DeleteObject(CenterBr);
}
void KLineView::DrawFrame(TMemDC& dc)
{
	KLineSaveDC Save(dc);
	HPEN KLineFramePen = CreatePen(PS_SOLID, 1, KLineFrameColor);
	HBRUSH KLineFrameBr = CreateSolidBrush(KLineFrameColor);
	SelectObject(dc.GetHdc(), KLineFramePen);
	FrameRect(dc.GetHdc(), &m_clRect.CenterRect, KLineFrameBr);

	//MoveToEx(dc.GetHdc(), 0, m_clRect.TopRect.bottom, NULL);
	//LineTo(dc.GetHdc(), m_clRect.TopRect.right, m_clRect.TopRect.bottom);
	DeleteObject(KLineFramePen);
	DeleteObject(KLineFrameBr);
}
void KLineView::DrawYAxisData(TMemDC& dc)
{
	KLineSaveDC Save(dc);
	SetBkMode(dc.GetHdc(), TRANSPARENT);
	HPEN linePen = CreatePen(PS_SOLID, 0.2, KLineFrameColor);
	SelectObject(dc.GetHdc(), linePen);
	SelectObject(dc.GetHdc(), g_FontData.GetFontWord12());
	SetTextColor(dc.GetHdc(), KLineFontNumberColor);
	SPriceStrType wtext;
	SIZE size; 
	RECT r = m_clRect.LeftRect;
	SPriceType high = m_clModel->m_dMaxPrice;
	
	//画X坐标顶轴
	g_pQuoteApi->FormatPrice(high, m_clModel->m_KContract.pContract->Commodity, wtext, false, false);
	GetTextExtentPointA(dc.GetHdc(), wtext, strlen(wtext), &size);
	r.left = r.right - size.cx - 8;
	r.bottom = r.top + size.cy;
	DrawTextA(dc.GetHdc(), wtext, strlen(wtext), &r, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
	MoveToEx(dc.GetHdc(), r.right, r.top , NULL);
	LineTo(dc.GetHdc(), m_clRect.CenterKLineRect.right, r.top);

	//画X坐标底轴
	SPriceType low = m_clModel->m_dMinPrice;
	r = m_clRect.LeftRect;
	g_pQuoteApi->FormatPrice(low, m_clModel->m_KContract.pContract->Commodity, wtext,false,  false);
	GetTextExtentPointA(dc.GetHdc(), wtext, strlen(wtext), &size);
	r.left = r.right - size.cx - 8;
	r.top = r.bottom - size.cy;
	DrawTextA(dc.GetHdc(), wtext, strlen(wtext), &r, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
	MoveToEx(dc.GetHdc(), r.right, r.bottom, NULL);
	LineTo(dc.GetHdc(), m_clRect.CenterKLineRect.right, r.bottom);

	HPEN dotLinePen = CreatePen(PS_DOT, 0.2, KLineFrameColor);
	SelectObject(dc.GetHdc(), dotLinePen);
	r = m_clRect.LeftRect;
	SPriceType mid = (high + low) / 2;
	DrawAxisPrice(dc, r, mid);
	SPriceType midHigh = (mid + high) / 2;
	DrawAxisPrice(dc, r, midHigh);
	SPriceType midLow = (mid + low) / 2;
	DrawAxisPrice(dc, r, midLow);

	DeleteObject(dotLinePen);
	DeleteObject(linePen);
}
void KLineView::DrawAxisPrice(TMemDC& dc, RECT r, SPriceType price)
{
	SPriceStrType wtext;
	SIZE size;
	g_pQuoteApi->FormatPrice(price, m_clModel->m_KContract.pContract->Commodity, wtext, false, false);
	GetTextExtentPointA(dc.GetHdc(), wtext, strlen(wtext), &size);
	r.left = r.right - size.cx - 8;
	int y = m_clModel->GetYPosByPrice(price);
	r.top = y - size.cy / 2;
	r.bottom = y + size.cy / 2;
	MoveToEx(dc.GetHdc(), r.right, y, NULL);
	LineTo(dc.GetHdc(), m_clRect.CenterKLineRect.right, y);
	DrawTextA(dc.GetHdc(), wtext, strlen(wtext), &r, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
}
void KLineView::DrawContractName(TMemDC& dc)
{
	KLineSaveDC SaveDC(dc);
	SetBkMode(dc.GetHdc(), TRANSPARENT);
	
	SelectObject(dc.GetHdc(), g_FontData.GetFontWord15());
	SetTextColor(dc.GetHdc(), KLineFontNameColor);
	RECT r = m_clRect.TopRect;
	r.left += KLineView_TextVap;

	SContractNameType tempName = {0};
	wchar_t name[100] = { 0 };
	g_pQuoteApi->GetContractName(m_clModel->m_KContract.pContract->ContractNo, tempName);
	CharToWChar(tempName, name);
	DrawText(dc.GetHdc(), name, wcslen(name), &r, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
}
void KLineView::DrawBottomTime(TMemDC& dc)
{
	KLineSaveDC Save(dc);
	SetBkMode(dc.GetHdc(), TRANSPARENT);
	RECT r = m_clRect.BottomRect;
	SelectObject(dc.GetHdc(), g_FontData.GetFontWord12());
	SetTextColor(dc.GetHdc(), KLineFontNumberColor);
	if (m_clModel->m_vtKLineDrawData.size() >= 0) 
	{
		int index = 0;
		DrawTime(dc, r, index);
		index = m_clModel->m_vtKLineDrawPoint.size() / 2;
		DrawTime(dc, r, index);
		index = m_clModel->m_vtKLineDrawPoint.size() - 1;
		DrawTime(dc, r, index);
	}
}
void KLineView::DrawTime(TMemDC& dc, RECT r, int index)
{
	SIZE size;
	wchar_t wtext[128];
	SHisQuoteData d = m_clModel->m_vtKLineDrawData.at(index);
	KLinePoint pos = m_clModel->m_vtKLineDrawPoint.at(index);
	KLineUtil::GetDrawTime(d, wtext, sizeof(wtext)/ sizeof(wchar_t));
	GetTextExtentPoint(dc.GetHdc(), wtext, wcslen(wtext), &size);
	if (index == 0) 
	{
		r.left = r.right - size.cx;
	}
	else if (index == m_clModel->m_vtKLineDrawData.size() - 1) 
	{
		r.right = r.left + size.cx;
	}
	else 
	{
		r.left = pos.x - size.cx / 2;
		r.right = r.left + size.cx;
	}
	DrawText(dc.GetHdc(), wtext, wcslen(wtext), &r, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
}
void KLineView::DrawKLine(TMemDC& dc)
{
	KLineSaveDC Save(dc);
	HBRUSH KLineUpBr = CreateSolidBrush(KLineUpColor);
	HBRUSH KLineDownBr = CreateSolidBrush(KLineDownColor);
	HPEN KLineUpPen = CreatePen(PS_SOLID, 1, KLineUpColor);
	HPEN KLineDownPen = CreatePen(PS_SOLID, 1, KLineDownColor);
	int iLen = m_clModel->m_vtKLineDrawPoint.size();
	int iKLineWidth = KLineUtil::GetKLineWidth();
	for (int index = 0 ; index < iLen; index++)
	{
		SHisQuoteData data = m_clModel->m_vtKLineDrawData[index];
		KLinePoint point = m_clModel->m_vtKLineDrawPoint[index];
		
		if (data.QLastPrice > data.QOpeningPrice) {
			RECT rc = { point.x, point.CloseY,  point.x + iKLineWidth, point.OpenY };
			SelectObject(dc.GetHdc(), KLineUpPen);
			FrameRect(dc.GetHdc(), &rc, KLineUpBr);

			MoveToEx(dc.GetHdc(), point.x + iKLineWidth / 2, point.HighY, NULL);
			LineTo(dc.GetHdc(), point.x + iKLineWidth / 2, point.CloseY);
			MoveToEx(dc.GetHdc(), point.x + iKLineWidth / 2, point.OpenY, NULL);
			LineTo(dc.GetHdc(), point.x + iKLineWidth / 2, point.LowY);
		}
		else if(data.QLastPrice < data.QOpeningPrice)
		{
			RECT rc = { point.x, point.OpenY,  point.x + iKLineWidth, point.CloseY };
			SelectObject(dc.GetHdc(), KLineDownPen);
			FillRect(dc.GetHdc(), &rc, KLineDownBr);
			MoveToEx(dc.GetHdc(), point.x + iKLineWidth / 2, point.HighY, NULL);
			LineTo(dc.GetHdc(), point.x + iKLineWidth / 2, point.LowY);
		}
		else if (data.QLastPrice == data.QOpeningPrice) 
		{
			SelectObject(dc.GetHdc(), KLineDownPen);
			MoveToEx(dc.GetHdc(), point.x + iKLineWidth / 2, point.HighY, NULL);
			LineTo(dc.GetHdc(), point.x + iKLineWidth / 2, point.LowY);

			MoveToEx(dc.GetHdc(), point.x , point.OpenY, NULL);
			LineTo(dc.GetHdc(), point.x + iKLineWidth , point.OpenY);
		}
		
	}
	DeleteObject(KLineUpBr);
	DeleteObject(KLineDownBr);
	DeleteObject(KLineUpPen);
	DeleteObject(KLineDownPen);
}
void KLineView::DrawLastPriceLine(TMemDC& dc)
{
	KLineSaveDC Save(dc);
	HPEN lastPricePen = CreatePen(PS_SOLID, 1, KLineFontNameColor);
	SelectObject(dc.GetHdc(), lastPricePen);
	MoveToEx(dc.GetHdc(), m_clRect.RightRect.left + KLineView_TextVap, m_clModel->m_LastPricePoint, NULL);
	LineTo(dc.GetHdc(), m_clRect.RightRect.right, m_clModel->m_LastPricePoint);

	DeleteObject(lastPricePen);
}

void KLineView::DrawTCRSLine(TMemDC& dc)
{
	KLineSaveDC Save(dc);
	HPEN redPen = CreatePen(PS_DOT, 1, KLineRedColor);
	HPEN greenPen = CreatePen(PS_DOT, 1, KLineGreenColor);
	HPEN bluePen = CreatePen(PS_DOT, 1, KlineBlueColor);
	
	
	SelectObject(dc.GetHdc(), g_FontData.g_FontWord12);
	SetBkMode(dc.GetHdc(), TRANSPARENT);
	SelectObject(dc.GetHdc(), greenPen);
	SetTextColor(dc.GetHdc(), KLineGreenColor);
	DrawResSupLine(dc, m_clModel->m_httpData.res2, "RES2: ");
	DrawResSupLine(dc, m_clModel->m_httpData.res3, "RES3: ");


	SelectObject(dc.GetHdc(), redPen);
	SetTextColor(dc.GetHdc(), KLineRedColor);
	DrawResSupLine(dc, m_clModel->m_httpData.sup2, "SUP2: ");
	DrawResSupLine(dc, m_clModel->m_httpData.sup3, "SUP3: ");


	SelectObject(dc.GetHdc(), bluePen);
	SetTextColor(dc.GetHdc(), KlineBlueColor);
	DrawResSupLine(dc, m_clModel->m_httpData.pivot, "PIVOT: ");



	if (m_clModel->m_httpData.pivot == m_clModel->m_httpData.sup1) {
		SelectObject(dc.GetHdc(), greenPen);
		SetTextColor(dc.GetHdc(), KLineGreenColor);
		DrawResSupLine(dc, m_clModel->m_httpData.res1, "RES1: ");
	}
	else {
		SelectObject(dc.GetHdc(), redPen);
		SetTextColor(dc.GetHdc(), KLineRedColor);
		DrawResSupLine(dc, m_clModel->m_httpData.sup1, "SUP1: ");
	}
	

	DeleteObject(redPen);
	DeleteObject(greenPen);
	DeleteObject(bluePen);
}
void KLineView::DrawTrend(TMemDC& dc)
{
	//画压力线和阻力线
	DrawTCRSLine(dc);
	int index = m_clModel->GetDateTimeIndex(m_clModel->m_httpData.dateTime);
	if (index >= 0 && index < m_clModel->m_vtKLineDrawPoint.size()) {
		//进行判断是否冲破压力线和阻力线
		SPriceType lowPrice = m_clModel->m_vtKLineDrawData[0].QLowPrice;
		SPriceType highPrice = m_clModel->m_vtKLineDrawData[0].QHighPrice;
		if (highPrice >= m_clModel->m_httpData.res1 || lowPrice <= m_clModel->m_httpData.sup1) {
			return;
		}
		//KLinePoint point = m_clModel->m_vtKLineDrawPoint[index];
		bool  up = false;
		up = m_clModel->m_httpData.pivot == m_clModel->m_httpData.res1 ? false : true;
		KLinePoint point = m_clModel->m_vtKLineDrawPoint[0];
		POINT to = { 0 };
		int yTo = up ? m_clModel->GetYPosByPrice(m_clModel->m_httpData.res2) : m_clModel->GetYPosByPrice(m_clModel->m_httpData.sup2);
		to.x = m_clRect.CenterKLineRect.right;
		//to.x = point.x;
		to.y = yTo;
		//KLineUtil::NewDrawTrend(dc, point, to, up);
		//POINT imagePoint = { };
		//KLineUtil::NewDrawTrendWithImage(point, to, up, imagePoint);

		wstring imageName = L"";
		m_frmRect = { point.x + KLineUtil::GetKLineWidth() * 2 , yTo,36,36 };
		if (up)
		{
			imageName = UP_SMALL;
			if (m_clRect.CenterKLineRect.bottom - m_clRect.CenterKLineRect.top > 276)
			{
				imageName = UP_BIG;
				m_frmRect.Height = 72;
			}
		}
		else
		{
			imageName = DOWN_SMALL;
			m_frmRect.Y -= 36;
			if (m_clRect.CenterKLineRect.bottom - m_clRect.CenterKLineRect.top > 276)
			{
				imageName = DOWN_BIG;
				m_frmRect.Y -= 36;
				m_frmRect.Height = 72;
			}
		}
		GetImgeWithPath(imageName.c_str());
		Gdiplus::Graphics graph(dc.GetHdc());
		graph.DrawImage(m_Img, m_frmRect);
	}
}
void KLineView::DrawResSupLine(TMemDC& dc, SPriceType price, std::string name)
{
	int y = m_clModel->GetYPosByPrice(price);
	MoveToEx(dc.GetHdc(), m_clRect.LeftRect.right, y, NULL);
	LineTo(dc.GetHdc(), m_clRect.RightRect.left, y);

	SPriceStrType tmp = { 0 };
	SIZE size;
	g_pQuoteApi->FormatPrice(price, m_clModel->m_KContract.pContract->Commodity, tmp, false,false);

	char showTemp[100] = { 0 };
	sprintf_s(showTemp, "%s%s", name.c_str(), tmp);
	GetTextExtentPointA(dc.GetHdc(), showTemp, strlen(showTemp), &size);
	RECT r = { m_clRect.LeftRect.right, y - size.cy, m_clRect.LeftRect.right + size.cx, y };
	DrawTextA(dc.GetHdc(), showTemp, strlen(showTemp), &r, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
}

void KLineView::GetImgeWithPath(const wchar_t* imageName)
{
	GetCurrentDirectory(ARRAYSIZE(m_szImgFilePath), m_szImgFilePath);
	lstrcat(m_szImgFilePath, ImageFilePath);
	lstrcat(m_szImgFilePath, imageName);
	m_Img = Gdiplus::Image::FromFile(m_szImgFilePath);

	//TCHAR * cp_pos = wcsrchr(m_szImgFilePath, L'\\');
	//if (0 != cp_pos)
	//	cp_pos[1] = 0;
	//wcscat_s(m_szImgFilePath, pszPath);
}

void KLineView::OnSize()
{

}

void KLineView::InitChartRect(CRECT rect)
{
	//初始化内部Rect信息
	m_clRect.TopRect = { 0, 0, rect.cx, KLineView_TopHeight };
	m_clRect.LeftRect = { 0, KLineView_TopHeight, KLineView_LeftWidth, rect.cy - KLineView_BottomHeight };
	m_clRect.CenterRect = { KLineView_LeftWidth, KLineView_TopHeight, rect.cx  - KLineView_RightWidth, rect.cy  - KLineView_BottomHeight };
	m_clRect.RightRect = { rect.cx - KLineView_RightWidth, KLineView_TopHeight, rect.cx , rect.cy - KLineView_BottomHeight };
	m_clRect.BottomRect = { KLineView_LeftWidth, rect.cy - KLineView_BottomHeight, m_clRect.CenterRect.right, rect.cy};
	m_clRect.CenterTopRect = { KLineView_LeftWidth, m_clRect.TopRect.bottom, m_clRect.CenterRect.right, m_clRect.TopRect.bottom + KLineView_TopHeight };
	m_clRect.CenterKLineRect = { KLineView_LeftWidth, m_clRect.CenterTopRect.bottom, m_clRect.CenterRect.right, m_clRect.CenterRect.bottom - KLineView_BottomHeight };
	m_clRect.CenterBottomRect = { KLineView_LeftWidth, m_clRect.CenterBottomRect.top + KLineView_TopHeight , m_clRect.CenterRect.right,m_clRect.CenterBottomRect.top + KLineView_TopHeight };
}
void KLineView::TraceMouse()
{
	if (!m_bMouseTract)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = m_Hwnd;
		tme.dwHoverTime = 0;
		::TrackMouseEvent(&tme);
		m_bMouseTract = true;
	}
}
void KLineView::ReleaseMouse()
{
	m_bMouseTract = false;
	m_bMouseDown = false;
}
void KLineView::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
	POINT point = { 0 };
	GetCursorPos(&point);
	ScreenToClient(m_Hwnd, &point);
	//滑动K线数据
	if (m_bMouseDown) {
		SetCursor(LoadCursor(NULL, IDC_SIZEWE));
		int Count = (point.x - m_BeginPoint.x) / KLineUtil::GetKLineTotalWidth() * 2;
		if (abs(Count) >= 1) {
			int hopetail = m_clModel->m_iEndIndex + Count;
			m_clModel->SetEndIndex(hopetail);
			m_clModel->UpdateSubData(true);
			InvalidateRect(m_Hwnd, NULL, false);
			m_BeginPoint = point;
		}	
	}
	//绘制十字光标
	if (m_bShowCross) {
		if (point.x > m_clRect.CenterKLineRect.right || point.x < m_clRect.CenterKLineRect.left || point.y > m_clRect.CenterKLineRect.bottom
			|| point.y < m_clRect.CenterKLineRect.top){
			m_bShowCross = false;
		}
		InvalidateRect(m_Hwnd, NULL, false);
	}
}
void KLineView::OnMouseLeave()
{
	ReleaseMouse();
}
void KLineView::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
	TraceMouse();
	GetCursorPos(&m_BeginPoint);
	ScreenToClient(m_Hwnd, &m_BeginPoint);
	if (PtInRect(&m_clRect.CenterRect, m_BeginPoint)) {
		SetCursor(LoadCursor(NULL, IDC_SIZEWE));
		m_bMouseDown = true;
	}	
}
void KLineView::OnLButtonUp(WPARAM wParam, LPARAM lParam)
{
	m_bMouseDown = false;
	SetCursor(LoadCursor(NULL, IDC_ARROW));
	m_bShowCross = !m_bShowCross;
	InvalidateRect(m_Hwnd, nullptr, false);
}
void KLineView::DrawCross(TMemDC& dc)
{
	KLineSaveDC Save(dc);
	POINT point;
	HPEN linePen = CreatePen(PS_SOLID, 1, g_ColorRefData.g_ColorWhite);
	SetBkMode(dc.GetHdc(), TRANSPARENT);
	GetCursorPos(&point);
	ScreenToClient(m_Hwnd, &point);
	if (point.x > m_clRect.CenterKLineRect.right || point.x < m_clRect.CenterKLineRect.left || point.y > m_clRect.CenterKLineRect.bottom
		|| point.y < m_clRect.CenterKLineRect.top)
	{
		m_bShowCross = FALSE;
		return;
	}

	//横线
	SelectObject(dc.GetHdc(), linePen);
	SelectObject(dc.GetHdc(), g_FontData.GetFontNum13());
	SetTextColor(dc.GetHdc(), g_ColorRefData.GetColorWhite());
	MoveToEx(dc.GetHdc(), KLineView_LeftWidth, point.y, NULL);
	LineTo(dc.GetHdc(), m_clRect.RightRect.left, point.y);	

	//竖线
	int index = m_clModel->GetXIndex(point.x);
	KLinePoint p = m_clModel->m_vtKLineDrawPoint[index];
	p.x += KLineUtil::GetKLineWidth() / 2;
	MoveToEx(dc.GetHdc(), p.x, m_clRect.TopRect.bottom, NULL);
	LineTo(dc.GetHdc(), p.x, m_clRect.CenterRect.bottom);

	//画下方的时间
	SHisQuoteData d = m_clModel->m_vtKLineDrawData[index];
	wchar_t wtext[128] = { 0 };
	SIZE size = { 0 };
	KLineUtil::GetDrawTime(d, wtext, sizeof(wtext) / sizeof(wchar_t));
	RECT r = { p.x, m_clRect.BottomRect.top, 0, m_clRect.BottomRect.bottom };
	GetTextExtentPoint(dc.GetHdc(), wtext, wcslen(wtext), &size);
	r.right = r.left + size.cx;
	if (r.right > m_clRect.RightRect.right) {
		r.right = m_clRect.RightRect.right;
		r.left = r.right - size.cx;
	}
	FillRect(dc.GetHdc(), &r, g_ColorRefData.g_brush_black);
	FrameRect(dc.GetHdc(), &r, g_ColorRefData.g_brush_white);
	DrawText(dc.GetHdc(), wtext, wcslen(wtext), &r, DT_LEFT | DT_SINGLELINE | DT_VCENTER);

	SPriceType price = m_clModel->GetPriceByYPos(point.y);

	char wtext1[256] = { 0 };
	g_pQuoteApi->FormatPrice(price, m_clModel->m_KContract.pContract->Commodity, wtext1, false, false);
	GetTextExtentPointA(dc.GetHdc(), wtext1, strlen(wtext1), &size);
	r.right = m_clRect.LeftRect.right;
	r.left = r.right - size.cx - KLineView_TextVap;
	r.top = point.y - size.cy / 2;
	r.bottom = r.top + size.cy;
	FillRect(dc.GetHdc(), &r, g_ColorRefData.g_brush_black);
	FrameRect(dc.GetHdc(), &r, g_ColorRefData.g_brush_white);
	DrawTextA(dc.GetHdc(), wtext1, strlen(wtext1), &r, DT_LEFT | DT_SINGLELINE | DT_VCENTER |  DT_CENTER);
}
