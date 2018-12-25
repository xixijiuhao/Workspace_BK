#include "BaseInclude.h"

#define ImageFilePath   L"\\images\\"

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
	InvalidateRect(m_Hwnd, NULL, false);
}

void KLineView::MoveWindow(CRECT& rect)
{
	InitChartRect(rect);
	SetWindowPos(m_Hwnd, 0, rect.x, rect.y, rect.cx, rect.cy, 0);
	InvalidateRect(m_Hwnd, NULL, false);
}
void KLineView::UpdateKLineView(ACContract &contract)
{
	m_KContract = contract;
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

	//DrawACImage(dc);
	DrawKlineAndAC(dc);

	DeleteObject(FrameBbr);
	DeleteObject(CenterBr);
}

void KLineView::DrawKlineAndAC(TMemDC &dc)
{
	DrawFrame(dc);
	if (m_clModel->m_vtKLineDrawData.size() && m_clModel->m_vtKLineDrawPoint.size())
	{
		DrawBottomTime(dc);			//画底部时间轴
		DrawYAxisData(dc);			//画Y轴
		DrawContractName(dc);		//画合约名称
		DrawKLine(dc);				//画K线
		DrawLastPriceLine(dc);		//画最新价横线
		if (m_bShowCross)
		{
			DrawCross(dc);
		}
		//画AC策略图形
		if (m_clModel->m_httpData.m_bValid)
		{
			switch (m_clModel->m_iTCTerm)
			{
			case 0 :
				DrawTrend(dc);
				break;
			case 1:
				DrawBigMovement(dc);
				break;
			case 2:
				DrawConsecutiveCandles(dc);
				break;
			default:
				break;
			}
		}
	}
}

void KLineView::DrawACImage(TMemDC &dc)
{
	wstring imageName = L"Analysis.png";
	GetCurrentDirectory(ARRAYSIZE(m_szImgFilePath), m_szImgFilePath);
	lstrcat(m_szImgFilePath, ImageFilePath);
	lstrcat(m_szImgFilePath, imageName.c_str());

	char imagewidth[11] = {};
	char imagehigth[11] = {};
	string imageurl = "";
	char key[256] = { 0 };
	sprintf_s(key, "%s|%s", m_KContract.commoditySymbol, m_KContract.pattern);
	auto Iter = m_clModel->m_TCDataMap.find(key);
	if (Iter != m_clModel->m_TCDataMap.end())
	{
		imageurl = Iter->second.m_sPatternImageUrl;
		if (imageurl.empty())
			return;
		sprintf_s(imagewidth, sizeof(imagewidth), "w=%d", m_clRect.CenterRect.right - m_clRect.CenterRect.left);
		sprintf_s(imagehigth, sizeof(imagehigth), "h=%d", m_clRect.CenterRect.bottom - m_clRect.CenterRect.top);
		imageurl.replace(imageurl.find("w=480"), 5, imagewidth);
		imageurl.replace(imageurl.find("h=275"), 5, imagehigth);
	}
	wstring url = LoadRC::ansi_to_unicode(imageurl.c_str());
	
	HRESULT hr = URLDownloadToFile(NULL, url.c_str(), m_szImgFilePath, 0, NULL);
	if (hr == S_OK)
	{
		m_frmRect = { 50,50,50,50 };
		m_Img = Gdiplus::Image::FromFile(m_szImgFilePath);
		Gdiplus::Graphics graph(dc.GetHdc());
		graph.DrawImage(m_Img, m_frmRect);
	}
}

void KLineView::DrawFrame(TMemDC& dc)
{
	KLineSaveDC Save(dc);
	HPEN KLineFramePen = CreatePen(PS_SOLID, 1, KLineFrameColor);
	HBRUSH KLineFrameBr = CreateSolidBrush(KLineFrameColor);
	SelectObject(dc.GetHdc(), KLineFramePen);
	FrameRect(dc.GetHdc(), &m_clRect.CenterRect, KLineFrameBr);

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
	//DrawTextA(dc.GetHdc(), wtext, strlen(wtext), &r, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
	MoveToEx(dc.GetHdc(), r.right, r.top , NULL);
	LineTo(dc.GetHdc(), m_clRect.CenterKLineRect.right, r.top);

	//画X坐标底轴
	SPriceType low = m_clModel->m_dMinPrice;
	r = m_clRect.LeftRect;
	g_pQuoteApi->FormatPrice(low, m_clModel->m_KContract.pContract->Commodity, wtext,false,  false);
	GetTextExtentPointA(dc.GetHdc(), wtext, strlen(wtext), &size);
	r.left = r.right - size.cx - 8;
	r.top = r.bottom - size.cy;
	//DrawTextA(dc.GetHdc(), wtext, strlen(wtext), &r, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
	MoveToEx(dc.GetHdc(), r.right, r.bottom, NULL);
	LineTo(dc.GetHdc(), m_clRect.CenterKLineRect.right, r.bottom);

	//画价格等分线
	HPEN dotLinePen = CreatePen(PS_DOT, 0.2, KLineFrameColor);
	SelectObject(dc.GetHdc(), dotLinePen);
	//r = m_clRect.LeftRect;
	r = { m_clRect.LeftRect.left, m_clRect.TopRect.top, m_clRect.LeftRect.right, m_clRect.BottomRect.bottom };
	int nPartNum = 10;
	for (int i = -10; i <= nPartNum + 10; ++i)
	{
		SPriceType mid = low + ((high - low) / nPartNum * i);
		DrawAxisPrice(dc, r, mid);
	}

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

	if (y < m_clRect.CenterRect.top || y > m_clRect.CenterRect.bottom)
		return;

	r.top = y - size.cy / 2;
	r.bottom = y + size.cy / 2;
	MoveToEx(dc.GetHdc(), r.right, y, NULL);
	LineTo(dc.GetHdc(), m_clRect.CenterRect.right, y);

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
	g_pQuoteApi->GetContractName(m_clModel->m_KContract.pContract->ContractNo, tempName);
	wstring wContractName = CConvert::UTF8ToUnicode(tempName);
	DrawText(dc.GetHdc(), wContractName.c_str(), wcslen(wContractName.c_str()), &r, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
}
void KLineView::DrawBottomTime(TMemDC& dc)
{
	KLineSaveDC Save(dc);
	HPEN linePen = CreatePen(PS_DOT, 0.2, KLineFrameColor);
	SelectObject(dc.GetHdc(), linePen);

	SetBkMode(dc.GetHdc(), TRANSPARENT);
	RECT r = m_clRect.BottomRect;
	SelectObject(dc.GetHdc(), g_FontData.GetFontWord12());
	SetTextColor(dc.GetHdc(), KLineFontNumberColor);
	if (m_clModel->m_vtKLineDrawData.size() >= 0) 
	{
		int nPartNum = 5;
		for (int index = 1; index < m_clModel->m_vtKLineDrawPoint.size(); ++index)
		{
			DrawTime(dc, r, index);
		}
	}
	DeleteObject(linePen);
}
void KLineView::DrawTime(TMemDC& dc, RECT r, int index)
{
	SIZE size;
	wchar_t wtext[32];
	SHisQuoteData d = m_clModel->m_vtKLineDrawData.at(index);
	KLinePoint pos = m_clModel->m_vtKLineDrawPoint.at(index - 1);
	if (KLineUtil::IsSameMonthTime(d))
	{
		return;
	}
	else
	{
		d = m_clModel->m_vtKLineDrawData.at(index - 1);
		KLineUtil::GetDrawTime(d, wtext, sizeof(wtext) / sizeof(wchar_t));
	}
	GetTextExtentPoint(dc.GetHdc(), wtext, wcslen(wtext), &size);
	MoveToEx(dc.GetHdc(), pos.x + KLineUtil::GetKLineWidth() / 2, m_clRect.CenterRect.top, NULL);
	LineTo(dc.GetHdc(), pos.x + KLineUtil::GetKLineWidth() / 2, m_clRect.CenterRect.bottom);

	r.left = pos.x - size.cx / 2;
	r.right = r.left + size.cx;
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
		
		if (data.QLastPrice > data.QOpeningPrice) 
		{
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

	DeleteObject(redPen);
	DeleteObject(greenPen);
	DeleteObject(bluePen);
}
POINT KLineView::GetPointByPriceAndDate(int nDate, SPriceType price)
{
	POINT p = {0, 0};

	SDateTimeType dateTime = KLineUtil::UnixTime2QuoteTime(nDate);
	int index = m_clModel->GetDateTimeIndex(dateTime);
	if (index < 0)
	{
		return p;
	}
	KLinePoint pos = m_clModel->m_vtKLineDrawPoint.at(index + 1);
	
	p.x = pos.x;
	p.y = m_clModel->GetYPosByPrice(price);

	return p;
}
void KLineView::GetPointVector(int nDate, SPriceType price, std::vector<POINT> &pointVec)
{
	POINT P = GetPointByPriceAndDate(nDate, price);
	if (P.x != 0 && P.y != 0)
	{
		pointVec.push_back(P);
	}
	return;
}

void KLineView::GetPosYVector(SPriceType price, std::map<int, std::string> &posMap)
{
	int Y = m_clModel->GetYPosByPrice(price);
	if (Y < m_clRect.CenterRect.bottom && Y > m_clRect.CenterRect.top)
	{
		posMap.emplace(make_pair(Y,to_string(price)));
	}
	return;
}

void KLineView::DrawFibonacciTrend(TMemDC& dc)
{
	KLineSaveDC Save(dc);
	SetBkMode(dc.GetHdc(), TRANSPARENT);
	HPEN lineBluePen = CreatePen(PS_SOLID, 2, KlineBlueColor);
	HPEN lineWhitePen = CreatePen(PS_SOLID, 1, g_ColorRefData.GetColorWhite());
	
	std::vector<POINT> pointVec;
	std::map<int, std::string> posMap;
	//存储X A B C D 点
	GetPointVector(m_clModel->m_httpData.m_PatternParam.timeX, m_clModel->m_httpData.m_PatternParam.priceX, pointVec);
	GetPointVector(m_clModel->m_httpData.m_PatternParam.timeA, m_clModel->m_httpData.m_PatternParam.priceA, pointVec);
	GetPointVector(m_clModel->m_httpData.m_PatternParam.timeB, m_clModel->m_httpData.m_PatternParam.priceB, pointVec);
	GetPointVector(m_clModel->m_httpData.m_PatternParam.timeC, m_clModel->m_httpData.m_PatternParam.priceC, pointVec);
	GetPointVector(m_clModel->m_httpData.m_PatternParam.timeD, m_clModel->m_httpData.m_PatternParam.priceD, pointVec);
	
	//存储target 
	GetPosYVector(m_clModel->m_httpData.m_PatternParam.target0, posMap);
	GetPosYVector(m_clModel->m_httpData.m_PatternParam.target3, posMap);
	GetPosYVector(m_clModel->m_httpData.m_PatternParam.target5, posMap);
	GetPosYVector(m_clModel->m_httpData.m_PatternParam.target6, posMap);
	GetPosYVector(m_clModel->m_httpData.m_PatternParam.target7, posMap);
	GetPosYVector(m_clModel->m_httpData.m_PatternParam.target10, posMap);
	GetPosYVector(m_clModel->m_httpData.m_PatternParam.target12, posMap);
	GetPosYVector(m_clModel->m_httpData.m_PatternParam.target16, posMap);

	SelectObject(dc.GetHdc(), g_FontData.GetFontWord12());
	SetTextColor(dc.GetHdc(), g_ColorRefData.GetColorWhite());
	SelectObject(dc.GetHdc(), lineBluePen);
	if (pointVec.size())
	{
		MoveToEx(dc.GetHdc(), pointVec[0].x, pointVec[0].y, NULL);
		for (int i = 1; i < pointVec.size(); ++i)
		{
			LineTo(dc.GetHdc(), pointVec[i].x, pointVec[i].y);
		}
	}

	//当completed未完成 timeD == -1 但是 priceD ！= -1时，要要出现C -> D -> target3的连续箭头
	bool completed = m_clModel->m_httpData.m_PatternParam.completed == 1;
	if (!completed && 
	   (m_clModel->m_httpData.m_PatternParam.timeD == -1) &&
	   (abs(m_clModel->m_httpData.m_PatternParam.priceD + 1) > 1e-6))
	{
		POINT PC = GetPointByPriceAndDate(m_clModel->m_httpData.m_PatternParam.timeC, m_clModel->m_httpData.m_PatternParam.priceC);
		int YD = m_clModel->GetYPosByPrice(m_clModel->m_httpData.m_PatternParam.priceD);
		int XD = m_clRect.CenterKLineRect.right;
		POINT PD = {XD, YD};
		KLineUtil::DrawTrend(dc, PC, PD);

		int half = KLineUtil::GetKLineWidth() / 2;
		Ellipse(dc.GetHdc(), PD.x - half, PD.y - half, PD.x + half, PD.y + half);

		POINT endP = {0};
		endP.x = m_clRect.CenterRect.right - 40;
		endP.y = m_clModel->GetYPosByPrice(m_clModel->m_httpData.m_PatternParam.target3);
		KLineUtil::DrawTrend(dc, PD, endP);
	}

	SelectObject(dc.GetHdc(), lineWhitePen);

	for (auto iterY : posMap)
	{
		MoveToEx(dc.GetHdc(), m_clRect.CenterRect.right, iterY.first, NULL);
		LineTo(dc.GetHdc(), m_clRect.CenterRect.right - 50, iterY.first);

		RECT r2 = {m_clRect.CenterRect.right - 100, iterY.first - 10, m_clRect.CenterRect.right, iterY.first + 10};
		DrawTextA(dc.GetHdc(), iterY.second.c_str(), iterY.second.length(), &r2, DT_LEFT | DT_EDITCONTROL | DT_VCENTER);
	}

	DeleteObject(lineWhitePen);
	DeleteObject(lineBluePen);
}

void KLineView::DrawTrend(TMemDC& dc)
{
	KLineSaveDC Save(dc);
	SetBkMode(dc.GetHdc(), TRANSPARENT);
	HPEN lineWhiteDotPen = CreatePen(PS_DOT, 1, g_ColorRefData.GetColorWhite());
	int iKLineWidth = KLineUtil::GetKLineWidth();
	bool isCompleted = m_clModel->m_httpData.m_PatternParam.completed == 1;
	bool  up = (m_clModel->m_httpData.m_PatternParam.direction == 1);

	//画发现时间线
	SelectObject(dc.GetHdc(), lineWhiteDotPen);
	SDateTimeType dateTime = KLineUtil::UnixTime2QuoteTime(m_clModel->m_httpData.m_PatternParam.identified_time);
	int index = m_clModel->GetDateTimeIndex(dateTime);
	KLinePoint comTimePos;
	if (index >= 0)
	{
		comTimePos = m_clModel->m_vtKLineDrawPoint.at(index + 1);
		comTimePos.x;
		MoveToEx(dc.GetHdc(), comTimePos.x + iKLineWidth / 2, m_clRect.CenterRect.top, NULL);
		LineTo(dc.GetHdc(), comTimePos.x + iKLineWidth / 2, m_clRect.CenterRect.bottom);
	}
	else
	{
		DeleteObject(lineWhiteDotPen);
		return;
	}

	//单独画斐波那契模型
	if (strcmp(m_clModel->m_httpData.m_sACPattern, "FibonacciPattern") == 0)
	{
		DrawFibonacciTrend(dc);
		DeleteObject(lineWhiteDotPen);
		return;
	}

	HPEN lineGreenPen = CreatePen(PS_SOLID, 2, KLineGreenColor);
	HPEN lineBluePen = CreatePen(PS_SOLID, 2, KlineBlueColor);
	HPEN lineGreenDashPen = CreatePen(PS_DASH, 1, KLineGreenColor);
	HPEN lineBlueDashPen = CreatePen(PS_DASH, 1, KlineBlueColor);
	HPEN lineWhitePen = CreatePen(PS_DASH, 1, g_ColorRefData.GetColorWhite());
	HBRUSH KPriceBr = CreateSolidBrush(g_ColorRefData.GetColorGray());

	//画箭头
	int longLineX = isCompleted ? (comTimePos.x) : (comTimePos.x + 200);
	KLinePoint point = m_clModel->m_vtKLineDrawPoint[index + 1];
	POINT to = { 0 };

	F64 priceY = up ? m_clModel->m_httpData.m_PatternParam.predictionpricefrom : m_clModel->m_httpData.m_PatternParam.predictionpriceto;
	int yTo = isCompleted ? m_clModel->GetYPosByPrice(priceY) : point.HighY + 100;
	to.x = point.x + 50;
	to.y = yTo;
	KLineUtil::NewDrawTrend(dc, point, to, up);

	//画价格预测区域
	if (isCompleted)
	{
		POINT priceform = GetPointByPriceAndDate(m_clModel->m_httpData.m_PatternParam.predictiontimefrom, m_clModel->m_httpData.m_PatternParam.predictionpricefrom);
		POINT priceto = {0, 0};
		priceto.x = priceform.x + 100;
		priceto.y = m_clModel->GetYPosByPrice(m_clModel->m_httpData.m_PatternParam.predictionpriceto);

		RECT r = { priceform.x, priceto.y, priceto.x, priceform.y };
		
		Rectangle(dc.GetHdc(), priceform.x, priceto.y, priceto.x, priceform.y);
		FillRect(dc.GetHdc(), &r, KPriceBr);

		string formText = "pricefrom: " + to_string(m_clModel->m_httpData.m_PatternParam.predictionpricefrom) + " , " + KLineUtil::UnixTime2Str(m_clModel->m_httpData.m_PatternParam.predictiontimefrom);
		string toText = "priceto: " + to_string(m_clModel->m_httpData.m_PatternParam.predictionpriceto) + " , " + KLineUtil::UnixTime2Str(m_clModel->m_httpData.m_PatternParam.predictiontimeto);
		SelectObject(dc.GetHdc(), g_FontData.GetFontWord12());
		SetTextColor(dc.GetHdc(), g_ColorRefData.GetColorWhite());

		RECT r2 = { 0 };
		if (priceform.y > m_clRect.CenterRect.bottom - 35)
		{
			r2 = { priceto.x + 10, priceto.y + 10 , priceto.x + 260, priceto.y + 26 };
		}
		else
		{
			r2 = { priceform.x + 10, priceform.y + 10 , priceform.x + 260, priceform.y + 26 };
		}
		DrawTextA(dc.GetHdc(), formText.c_str(), formText.length(), &r2, DT_LEFT | DT_EDITCONTROL | DT_VCENTER);
		r2.top = r2.bottom + 5;
		r2.bottom = r2.top + 16;
		DrawTextA(dc.GetHdc(), toText.c_str(), toText.length(), &r2, DT_LEFT | DT_EDITCONTROL | DT_VCENTER);
	}

	//画策略
	if (strcmp(m_clModel->m_httpData.m_sACPattern, "ChartPattern") == 0)
	{
		//画支撑线和阻力线
		POINT resistance0 = GetPointByPriceAndDate(m_clModel->m_httpData.m_PatternParam.resistancex0, m_clModel->m_httpData.m_PatternParam.resistancey0);
		POINT resistance1 = GetPointByPriceAndDate(m_clModel->m_httpData.m_PatternParam.resistancex1, m_clModel->m_httpData.m_PatternParam.resistancey1);

		POINT support0 = GetPointByPriceAndDate(m_clModel->m_httpData.m_PatternParam.supportx0, m_clModel->m_httpData.m_PatternParam.supporty0);
		POINT support1 = GetPointByPriceAndDate(m_clModel->m_httpData.m_PatternParam.supportx1, m_clModel->m_httpData.m_PatternParam.supporty1);

		if (resistance0.x == 0 || resistance1.x == 0 || support0.x == 0 || support1.x == 0)
			return;

		//通过直线方程计算直线延长线
		double rk = (resistance0.x == resistance1.x) ? 99999.0f : ((double)(resistance0.y - resistance1.y)) / (resistance0.x - resistance1.x);
		double rb = resistance1.y - rk * resistance1.x;
		int ry3 = ceil(rk * longLineX + rb);

		SelectObject(dc.GetHdc(), lineGreenPen);
		MoveToEx(dc.GetHdc(), resistance0.x, resistance0.y, NULL);
		//LineTo(dc.GetHdc(), resistance1.x, resistance1.y);
		//MoveToEx(dc.GetHdc(), resistance1.x, resistance1.y, NULL);
		LineTo(dc.GetHdc(), longLineX, ry3);

		double sk = (support0.x == support1.x) ? 99999.0f : ((double)(support0.y - support1.y)) / (support0.x - support1.x);
		double sb = support1.y - sk * support1.x;
		int sy3 = ceil(sk * longLineX + sb);

		SelectObject(dc.GetHdc(), lineBluePen);
		MoveToEx(dc.GetHdc(), support0.x, support0.y, NULL);
		//LineTo(dc.GetHdc(), support1.x, support1.y);
		//MoveToEx(dc.GetHdc(), support1.x, support1.y, NULL);
		LineTo(dc.GetHdc(), longLineX, sy3);
	}
	else if (strcmp(m_clModel->m_httpData.m_sACPattern, "KeyLevelsPattern") == 0)
	{
		if (strcmp(m_clModel->m_httpData.m_PatternParam.pattern,"Resistance") == 0)
		{
			SelectObject(dc.GetHdc(), lineGreenDashPen);
		}
		else
		{
			SelectObject(dc.GetHdc(), lineBlueDashPen);
		}
		
		POINT endPos = GetPointByPriceAndDate(m_clModel->m_httpData.m_PatternParam.identified_time, m_clModel->m_httpData.m_PatternParam.price);
		POINT firstPos = GetPointByPriceAndDate(m_clModel->m_httpData.m_PatternParam.point1, m_clModel->m_httpData.m_PatternParam.price);

		MoveToEx(dc.GetHdc(), m_clRect.CenterRect.left, endPos.y, NULL);
		LineTo(dc.GetHdc(), endPos.x, endPos.y);
	}

	DeleteObject(KPriceBr);
	DeleteObject(lineGreenPen);
	DeleteObject(lineBluePen);
	DeleteObject(lineWhitePen);
	DeleteObject(lineGreenDashPen);
	DeleteObject(lineBlueDashPen);
	DeleteObject(lineWhiteDotPen);
}

void KLineView::DrawBigMovement(TMemDC& dc)
{
	KLineSaveDC Save(dc);
	SetBkMode(dc.GetHdc(), TRANSPARENT);

	HPEN KLineFramePen = CreatePen(PS_SOLID, 2, g_ColorRefData.GetColorWhite());
	HBRUSH KLineFrameBr = CreateSolidBrush(g_ColorRefData.GetColorWhite());
	SelectObject(dc.GetHdc(), KLineFramePen);
	SelectObject(dc.GetHdc(), KLineFrameBr);

	if (m_clModel->m_httpData.m_BigMovementParamVec.size() == 0)
	{
		SelectObject(dc.GetHdc(), g_FontData.GetFontWord15());
		SetTextColor(dc.GetHdc(), g_ColorRefData.GetColorWhite());

		wstring wContractName = g_pLanguageApi->LangText(NoDataTipStrID);
		RECT r = { m_clRect.CenterRect.left + 50, m_clRect.CenterRect.top + 20,m_clRect.CenterRect.left + 350 ,m_clRect.CenterRect.top + 50 };
		DrawText(dc.GetHdc(), wContractName.c_str(), wcslen(wContractName.c_str()), &r, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
	}
	else
	{
		for (auto iter : m_clModel->m_httpData.m_BigMovementParamVec)
		{
			SDateTimeType dateTime = KLineUtil::UnixTime2QuoteTime(iter.patternstarttime);
			int index = m_clModel->GetDateTimeIndex(dateTime);
			if (index < 0)
			{
				return;
			}
			KLinePoint startpos = m_clModel->m_vtKLineDrawPoint.at(index + 1);

			dateTime = KLineUtil::UnixTime2QuoteTime(iter.patternendtime);
			index = m_clModel->GetDateTimeIndex(dateTime);
			if (index < 0)
			{
				return;
			}
			KLinePoint endpos = m_clModel->m_vtKLineDrawPoint.at(index + 1);

			RECT r = { 0 };
			if (iter.direction == 1)
			{
				r = { startpos.x, endpos.HighY - 30, endpos.x, startpos.LowY + 30 };
			}
			else
			{
				r = { startpos.x, startpos.HighY - 50, endpos.x, endpos.LowY + 50 };
			}
			FrameRect(dc.GetHdc(), &r, KLineFrameBr);
		}
	}

	DeleteObject(KLineFramePen);
	DeleteObject(KLineFrameBr);
}

void KLineView::DrawConsecutiveCandles(TMemDC& dc)
{
	KLineSaveDC Save(dc);
	SetBkMode(dc.GetHdc(), TRANSPARENT);
	HPEN KLineFramePen = CreatePen(PS_SOLID, 2, g_ColorRefData.GetColorWhite());
	HBRUSH KLineFrameBr = CreateSolidBrush(g_ColorRefData.GetColorWhite());
	SelectObject(dc.GetHdc(), KLineFramePen);
	SelectObject(dc.GetHdc(), KLineFrameBr);

	if (m_clModel->m_httpData.m_ConsecutiveParamVec.size() == 0)
	{
		SelectObject(dc.GetHdc(), g_FontData.GetFontWord15());
		SetTextColor(dc.GetHdc(), g_ColorRefData.GetColorWhite());

		wstring wContractName = g_pLanguageApi->LangText(NoDataTipStrID);
		RECT r = { m_clRect.CenterRect.left + 50, m_clRect.CenterRect.top + 20,m_clRect.CenterRect.left + 350 ,m_clRect.CenterRect.top + 50 };
		DrawText(dc.GetHdc(), wContractName.c_str(), wcslen(wContractName.c_str()), &r, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
	}
	else
	{
		for (auto iter : m_clModel->m_httpData.m_ConsecutiveParamVec)
		{
			SDateTimeType dateTime = KLineUtil::UnixTime2QuoteTime(iter.patternstarttime);
			int index = m_clModel->GetDateTimeIndex(dateTime);
			if (index < 0)
			{
				return;
			}
			KLinePoint startpos = m_clModel->m_vtKLineDrawPoint.at(index + 1);

			dateTime = KLineUtil::UnixTime2QuoteTime(iter.patternendtime);
			index = m_clModel->GetDateTimeIndex(dateTime);
			if (index < 0)
			{
				return;
			}
			KLinePoint endpos = m_clModel->m_vtKLineDrawPoint.at(index + 1);

			RECT r = { 0 };
			if (iter.direction == 1)
			{
				r = { startpos.x, endpos.HighY - 50, endpos.x, startpos.LowY + 50 };
			}
			else
			{
				r = { startpos.x, startpos.HighY - 50, endpos.x, endpos.LowY + 50 };
			}
			FrameRect(dc.GetHdc(), &r, KLineFrameBr);
		}
	}

	DeleteObject(KLineFramePen);
	DeleteObject(KLineFrameBr);
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
	
	int KLineRectHight = (m_clRect.CenterRect.bottom - m_clRect.CenterRect.top) / 2;
	m_clRect.CenterKLineRect = { KLineView_LeftWidth, m_clRect.CenterTopRect.bottom + KLineRectHight / 2, m_clRect.CenterRect.right - 200, m_clRect.CenterRect.bottom - KLineView_BottomHeight - KLineRectHight / 2 };
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
	if (m_bMouseDown) 
	{
		SetCursor(LoadCursor(NULL, IDC_SIZEWE));
		int Count = (point.x - m_BeginPoint.x) / KLineUtil::GetKLineTotalWidth() * 2;
		if (abs(Count) >= 1) 
		{
			int hopetail = m_clModel->m_iEndIndex + Count;
			m_clModel->SetEndIndex(hopetail);
			m_clModel->UpdateSubData(false);
			InvalidateRect(m_Hwnd, NULL, false);
			m_BeginPoint = point;
		}	
	}
	//绘制十字光标
	if (m_bShowCross) 
	{
		if (point.x > m_clRect.CenterKLineRect.right || 
			point.x < m_clRect.CenterKLineRect.left || 
			point.y > m_clRect.CenterKLineRect.bottom || 
			point.y < m_clRect.CenterKLineRect.top)
		{
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
	if (PtInRect(&m_clRect.CenterRect, m_BeginPoint)) 
	{
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
	if (point.x > m_clRect.CenterKLineRect.right || 
		point.x < m_clRect.CenterKLineRect.left || 
		point.y > m_clRect.CenterKLineRect.bottom || 
		point.y < m_clRect.CenterKLineRect.top)
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
	if (r.right > m_clRect.RightRect.right) 
	{
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

	DeleteObject(linePen);
}
