#include "BaseInclude.h"

extern ILanguageApi* g_pLanguageApi;

KLineIntroductView::KLineIntroductView():m_iYVap(10)
{
	m_bkBrush = g_ColorRefData.g_brush_black;
	m_iHeight = 0;
}
void KLineIntroductView::SetText(ACSpiData &spiData)
{
	m_vtFirstColText.clear();
	m_vtSecondColText.clear();
	m_vtText.clear();
	if (strcmp(spiData.m_sACPattern, "ChartPattern") == 0)
	{
		wstring ChartPatternText[ChartPatternTextCnt] = 
		{
			g_pLanguageApi->LangText(Exchange),
			g_pLanguageApi->LangText(Symbol),
			g_pLanguageApi->LangText(Interval),
			g_pLanguageApi->LangText(Pattern),

			g_pLanguageApi->LangText(Length),
			g_pLanguageApi->LangText(Direction),
			g_pLanguageApi->LangText(IdentifiedTime),
			g_pLanguageApi->LangText(Quality),

			g_pLanguageApi->LangText(TrendChange),
			g_pLanguageApi->LangText(InitialTrend),
			g_pLanguageApi->LangText(Volume),
			g_pLanguageApi->LangText(Uniformity),

			g_pLanguageApi->LangText(Clarity1),
			g_pLanguageApi->LangText(Breakout),
			g_pLanguageApi->LangText(ForecastPrice1)
		};

		m_vtText.insert(make_pair(LoadRC::unicode_to_ansi(ChartPatternText[0]),spiData.m_PatternParam.exchange));
		m_vtText.insert(make_pair(LoadRC::unicode_to_ansi(ChartPatternText[1]), spiData.m_PatternParam.symbol));
		m_vtText.insert(make_pair(LoadRC::unicode_to_ansi(ChartPatternText[2]), to_string(spiData.m_PatternParam.interval)));
		m_vtText.insert(make_pair(LoadRC::unicode_to_ansi(ChartPatternText[3]), spiData.m_PatternParam.pattern));

		m_vtText.insert(make_pair(LoadRC::unicode_to_ansi(ChartPatternText[4]), to_string(spiData.m_PatternParam.length)));
		m_vtText.insert(make_pair(LoadRC::unicode_to_ansi(ChartPatternText[5]), to_string(spiData.m_PatternParam.direction)));
		m_vtText.insert(make_pair(LoadRC::unicode_to_ansi(ChartPatternText[6]), to_string(spiData.m_PatternParam.identified_time)));
		m_vtText.insert(make_pair(LoadRC::unicode_to_ansi(ChartPatternText[7]), to_string(spiData.m_PatternParam.quality)));

		m_vtText.insert(make_pair(LoadRC::unicode_to_ansi(ChartPatternText[8]), spiData.m_PatternParam.trend));
		m_vtText.insert(make_pair(LoadRC::unicode_to_ansi(ChartPatternText[9]), to_string(spiData.m_PatternParam.initialtrend)));
		m_vtText.insert(make_pair(LoadRC::unicode_to_ansi(ChartPatternText[10]), to_string(spiData.m_PatternParam.volumeincrease)));
		m_vtText.insert(make_pair(LoadRC::unicode_to_ansi(ChartPatternText[11]), to_string(spiData.m_PatternParam.uniformity)));

		m_vtText.insert(make_pair(LoadRC::unicode_to_ansi(ChartPatternText[12]), to_string(spiData.m_PatternParam.clarity_c)));
		m_vtText.insert(make_pair(LoadRC::unicode_to_ansi(ChartPatternText[13]), to_string(spiData.m_PatternParam.breakout)));
		m_vtText.insert(make_pair(LoadRC::unicode_to_ansi(ChartPatternText[14]), to_string(spiData.m_PatternParam.predictionpriceto)));

	}
	else if (strcmp(spiData.m_sACPattern, "FibonacciPattern") == 0)
	{
		wstring FibonacciPatternText[FibonacciPatternTextCnt] = 
		{
			g_pLanguageApi->LangText(Exchange),
			g_pLanguageApi->LangText(Symbol),
			g_pLanguageApi->LangText(Interval),
			g_pLanguageApi->LangText(Pattern),

			g_pLanguageApi->LangText(Length),
			g_pLanguageApi->LangText(Direction),
			g_pLanguageApi->LangText(IdentifiedTime),
			g_pLanguageApi->LangText(Quality),

			g_pLanguageApi->LangText(TimeSymmetry),
			g_pLanguageApi->LangText(PriceSymmetry),
			g_pLanguageApi->LangText(Clarity2),
			g_pLanguageApi->LangText(Target0),

			g_pLanguageApi->LangText(Target3),
			g_pLanguageApi->LangText(Target5),
			g_pLanguageApi->LangText(Target6),
			g_pLanguageApi->LangText(Target7),

			g_pLanguageApi->LangText(Target10),
			g_pLanguageApi->LangText(Target12),
			g_pLanguageApi->LangText(Target16)
		};

		m_vtText.insert(make_pair(LoadRC::unicode_to_ansi(FibonacciPatternText[0]), spiData.m_PatternParam.exchange));
		m_vtText.insert(make_pair(LoadRC::unicode_to_ansi(FibonacciPatternText[1]), spiData.m_PatternParam.symbol));
		m_vtText.insert(make_pair(LoadRC::unicode_to_ansi(FibonacciPatternText[2]), to_string(spiData.m_PatternParam.interval)));
		m_vtText.insert(make_pair(LoadRC::unicode_to_ansi(FibonacciPatternText[3]), spiData.m_PatternParam.pattern));

		m_vtText.insert(make_pair(LoadRC::unicode_to_ansi(FibonacciPatternText[4]), to_string(spiData.m_PatternParam.length)));
		m_vtText.insert(make_pair(LoadRC::unicode_to_ansi(FibonacciPatternText[5]), to_string(spiData.m_PatternParam.direction)));
		m_vtText.insert(make_pair(LoadRC::unicode_to_ansi(FibonacciPatternText[6]), to_string(spiData.m_PatternParam.identified_time)));
		m_vtText.insert(make_pair(LoadRC::unicode_to_ansi(FibonacciPatternText[7]), to_string(spiData.m_PatternParam.quality)));

		m_vtText.insert(make_pair(LoadRC::unicode_to_ansi(FibonacciPatternText[8]), to_string(spiData.m_PatternParam.timesymmetry)));
		m_vtText.insert(make_pair(LoadRC::unicode_to_ansi(FibonacciPatternText[9]), to_string(spiData.m_PatternParam.pricesymmetry)));
		m_vtText.insert(make_pair(LoadRC::unicode_to_ansi(FibonacciPatternText[10]), to_string(spiData.m_PatternParam.clarity_f)));
		m_vtText.insert(make_pair(LoadRC::unicode_to_ansi(FibonacciPatternText[11]), to_string(spiData.m_PatternParam.target0)));

		m_vtText.insert(make_pair(LoadRC::unicode_to_ansi(FibonacciPatternText[12]), to_string(spiData.m_PatternParam.target3)));
		m_vtText.insert(make_pair(LoadRC::unicode_to_ansi(FibonacciPatternText[13]), to_string(spiData.m_PatternParam.target5)));
		m_vtText.insert(make_pair(LoadRC::unicode_to_ansi(FibonacciPatternText[14]), to_string(spiData.m_PatternParam.target6)));
		m_vtText.insert(make_pair(LoadRC::unicode_to_ansi(FibonacciPatternText[15]), to_string(spiData.m_PatternParam.target7)));

		m_vtText.insert(make_pair(LoadRC::unicode_to_ansi(FibonacciPatternText[16]), to_string(spiData.m_PatternParam.target10)));
		m_vtText.insert(make_pair(LoadRC::unicode_to_ansi(FibonacciPatternText[17]), to_string(spiData.m_PatternParam.target12)));
		m_vtText.insert(make_pair(LoadRC::unicode_to_ansi(FibonacciPatternText[18]), to_string(spiData.m_PatternParam.target16)));
	}
	else if (strcmp(spiData.m_sACPattern, "KeyLevelsPattern") == 0)
	{
		wstring KeyLevelsText[KeyLevelsTextCnt] = 
		{
			g_pLanguageApi->LangText(Exchange),
			g_pLanguageApi->LangText(Symbol),
			g_pLanguageApi->LangText(Interval),
			g_pLanguageApi->LangText(Pattern),

			g_pLanguageApi->LangText(Length),
			g_pLanguageApi->LangText(Direction),
			g_pLanguageApi->LangText(IdentifiedTime),
			g_pLanguageApi->LangText(Quality),

			g_pLanguageApi->LangText(ResistanceLevel),
			g_pLanguageApi->LangText(ForecastPrice2)
		};

		m_vtText.insert(make_pair(LoadRC::unicode_to_ansi(KeyLevelsText[0]), spiData.m_PatternParam.exchange));
		m_vtText.insert(make_pair(LoadRC::unicode_to_ansi(KeyLevelsText[1]), spiData.m_PatternParam.symbol));
		m_vtText.insert(make_pair(LoadRC::unicode_to_ansi(KeyLevelsText[2]), to_string(spiData.m_PatternParam.interval)));
		m_vtText.insert(make_pair(LoadRC::unicode_to_ansi(KeyLevelsText[3]), spiData.m_PatternParam.pattern));

		m_vtText.insert(make_pair(LoadRC::unicode_to_ansi(KeyLevelsText[4]), to_string(spiData.m_PatternParam.length)));
		m_vtText.insert(make_pair(LoadRC::unicode_to_ansi(KeyLevelsText[5]), to_string(spiData.m_PatternParam.direction)));
		m_vtText.insert(make_pair(LoadRC::unicode_to_ansi(KeyLevelsText[6]), to_string(spiData.m_PatternParam.identified_time)));
		m_vtText.insert(make_pair(LoadRC::unicode_to_ansi(KeyLevelsText[7]), to_string(spiData.m_PatternParam.quality)));

		m_vtText.insert(make_pair(LoadRC::unicode_to_ansi(KeyLevelsText[8]), to_string(spiData.m_PatternParam.price)));
		m_vtText.insert(make_pair(LoadRC::unicode_to_ansi(KeyLevelsText[9]), to_string(spiData.m_PatternParam.predictionpricefrom)));
	} 
	m_vtFirstColText = m_vtText;
}
void KLineIntroductView::CreateView(HWND hParent, RECT r)
{
	CreateFrm(L"cxp_IntroductView", hParent, WS_VISIBLE | WS_CHILD);
	SetWindowPos(m_Hwnd, 0, r.left, r.top, r.right - r.left, r.bottom - r.top, 0);
	RECT client = {};
	GetClientRect(m_Hwnd, &client);
	m_CurRect = client;
	m_rect = client;
	m_iMaxBottom = m_rect.bottom - (INTRODUCTTEXTHIGHT - m_rect.bottom);

	m_ScrollRect.left = m_rect.right - 36;
	m_ScrollRect.right = m_rect.right - 30;
	m_ScrollRect.top = client.top;
	m_ScrollRect.bottom = client.top + 60;
}
void KLineIntroductView::MoveWindow(RECT rect)
{
	SetWindowPos(m_Hwnd, 0, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, 0);
	InvalidateRect(m_Hwnd, NULL, false);
}
LRESULT KLineIntroductView::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_SIZE:
		OnSize(wParam, lParam);
		break;
	case WM_PAINT:
		OnPaint();
		break;
	case WM_MOUSEWHEEL:
		OnMouseWheel(wParam, lParam);
		break;
	case WM_SETFOCUS:
		OnDealSetFocus();
		break;
	default:
		break;
	}
	return NOT_PROCESSED;
}

void KLineIntroductView::OnDealSetFocus()
{
	InvalidateRect(m_Hwnd, NULL, TRUE);
}

void KLineIntroductView::OnSize(WPARAM wParam, LPARAM lParam)
{
	RECT client = {};
	GetClientRect(m_Hwnd, &client);
	m_rect = client;
	m_CurRect = client;
	m_iMaxBottom = m_rect.bottom - (INTRODUCTTEXTHIGHT - m_rect.bottom);
	m_ScrollRect.left = m_rect.right - 36;
	m_ScrollRect.right = m_rect.right - 30;
	m_ScrollRect.top = client.top;
	m_ScrollRect.bottom = client.top + 60;
}

void KLineIntroductView::OnMouseWheel(WPARAM wParam, LPARAM lParam)
{
	int delta = GET_WHEEL_DELTA_WPARAM(wParam);
	int offset = delta / 12;

	if (m_CurRect.bottom - m_CurRect.top >= INTRODUCTTEXTHIGHT)
		return;
	//控制滚轮，当文字已经到达最顶部则不能往上滚动；
	if (m_CurRect.top > 0 && offset > 0)
	{
		return;
	}
	//控制滚轮，当文字已经到达最底部则不能往下滚动；
	if (m_CurRect.bottom < m_iMaxBottom && offset < 0)
	{
		return;
	}

	m_CurRect.top += offset;
	m_CurRect.bottom += offset;

	double tmpMidDiv = (double)(INTRODUCTTEXTHIGHT - m_rect.bottom) / (m_rect.bottom - 60);
	int scrollOffset = delta / (tmpMidDiv * 12);

	m_ScrollRect.top -= scrollOffset;
	m_ScrollRect.bottom -= scrollOffset;
	InvalidateRect(m_Hwnd, NULL, false);
	//++m_iHeight;
}

void KLineIntroductView::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
	POINT point;
	point.x = LOWORD(lParam);
	point.y = HIWORD(lParam);

	if (PtInRect(&m_ScrollRect, point)) 
	{
		m_bMousetrack = false;
		InvalidateRect(m_Hwnd, NULL, false);
	}
}

void KLineIntroductView::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
	TrackMouse();
	POINT pt;
	pt.x = LOWORD(lParam);
	pt.y = HIWORD(lParam);

	if (m_bMousetrack) 
	{
		
	}
}

void KLineIntroductView::OnLButtonUp(WPARAM wParam, LPARAM lParam)
{
	m_bMousetrack = false;

	InvalidateRect(m_Hwnd, 0, false);
}

void KLineIntroductView::TrackMouse()
{
	if (!m_bMousetrack)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = m_Hwnd;
		tme.dwHoverTime = 0;
		::TrackMouseEvent(&tme);
		m_bMousetrack = true;
	}
}

void KLineIntroductView::OnMouseLeave()
{
	m_bMousetrack = false;
	InvalidateRect(m_Hwnd, 0, false);
}

void KLineIntroductView::OnPaint()
{
	int iHeight = m_rect.bottom - m_rect.top;
	TMemDC dc(m_Hwnd);
	KLineSaveDC Save(dc);
	SetBkMode(dc.GetHdc(), TRANSPARENT);

	if (!m_vtFirstColText.size() && !m_vtSecondColText.size()) 
	{
		return;
	}
	HBRUSH hbrFrame = CreateSolidBrush(KLineFrameColor);
	HPEN   penLine = CreatePen(PS_SOLID, 1, KLineFrameColor);
	SelectObject(dc.GetHdc(), g_FontData.g_FontWord13);
	SelectObject(dc.GetHdc(), penLine);
	HBRUSH scrollBrush = CreateSolidBrush(RGB(20,20,20));

	RECT client ={ };
	GetClientRect(m_Hwnd, &client);
	FillRect(dc.GetHdc(), &client, m_bkBrush);

	//对换合约之后进行控制
	//client = (m_strCurFirstText != m_vtFirstColText[0]) ? client : m_CurRect;
	string curSymbol = "";
	auto iter = m_vtFirstColText.find(LoadRC::unicode_to_ansi(g_pLanguageApi->LangText(Symbol)));
	if (iter != m_vtFirstColText.end())
	{
		curSymbol = iter->second;
	}

	if (m_strCurFirstText != curSymbol)
	{
		m_ScrollRect.left = m_rect.right - 36;
		m_ScrollRect.right = m_rect.right - 30;
		m_ScrollRect.top = client.top + 10;
		m_ScrollRect.bottom = client.top + 70;
	}
	else
	{
		client = m_CurRect;
	}

	RECT r = client;
	r.top += 40;
	r.right = (client.right - client.left) / 2 - 50;
	SIZE size;
	m_strCurFirstText = curSymbol;
	GetTextExtentPointA(dc.GetHdc(), m_strCurFirstText.c_str(), m_strCurFirstText.length(), &size);
	r.bottom = r.top + size.cy + m_iYVap;
	for (auto iter = m_vtFirstColText.begin(); iter != m_vtFirstColText.end(); ++iter)
	{
		string curText = iter->first;
		string curValueText = iter->second;
		if (iter->first.length())
		{
			GetTextExtentPointA(dc.GetHdc(), curText.c_str(), curText.length(), &size);
			if (m_vtFirstColText.size() == 1)
			{
				SelectObject(dc.GetHdc(), g_FontData.g_FontWord15);
				SetTextColor(dc.GetHdc(), KLineFontNameColor);
			}
			else
			{
				SetTextColor(dc.GetHdc(), KLineFontNameColor);
			}
	
			DrawIntroductText(dc, curText, curValueText, r, size);
			r.top = r.bottom;
			r.bottom = r.top + size.cy + m_iYVap;
		}
	}
	
	if (m_CurRect.bottom - m_CurRect.top < INTRODUCTTEXTHIGHT)
	{
		FillRect(dc.GetHdc(), &m_ScrollRect, scrollBrush);
	}
	
	DeleteObject(scrollBrush);
	DeleteObject(hbrFrame);
	DeleteObject(penLine);
}

void KLineIntroductView::DrawIntroductText(TMemDC& dc, string curText, string curValueText, RECT r, SIZE size)
{
	r.bottom = r.bottom + size.cy + m_iYVap;
	DrawTextA(dc.GetHdc(), curText.c_str(), curText.length(), &r, DT_LEFT | DT_EDITCONTROL | DT_WORDBREAK);

	if (LoadRC::unicode_to_ansi(g_pLanguageApi->LangText(Quality)) == curText)
	{
		HBRUSH bBuleBrush = CreateSolidBrush(KlineBlueColor);
		DrawFillRectangle(dc, curValueText, r, bBuleBrush);
		DeleteObject(bBuleBrush);
	}
	else if (LoadRC::unicode_to_ansi(g_pLanguageApi->LangText(InitialTrend)) == curText)
	{
		HBRUSH bGrayBrush = CreateSolidBrush(g_ColorRefData.GetColorGray());
		DrawFillRectangle(dc, curValueText, r, bGrayBrush);
		DeleteObject(bGrayBrush);
	}
	else if (LoadRC::unicode_to_ansi(g_pLanguageApi->LangText(Uniformity)) == curText)
	{
		HBRUSH bLightBuleBrush = CreateSolidBrush(g_ColorRefData.GetColorLightBlue());
		DrawFillRectangle(dc, curValueText, r, bLightBuleBrush);
		DeleteObject(bLightBuleBrush);
	}
	else if (LoadRC::unicode_to_ansi(g_pLanguageApi->LangText(Clarity1)) == curText)
	{
		HBRUSH bRGB150Brush = CreateSolidBrush(g_ColorRefData.GetColorRGB150());
		DrawFillRectangle(dc, curValueText, r, bRGB150Brush);
		DeleteObject(bRGB150Brush);
	}
	else if (LoadRC::unicode_to_ansi(g_pLanguageApi->LangText(Breakout)) == curText)
	{
		HBRUSH bRedBrush = CreateSolidBrush(KLineRedColor);
		DrawFillRectangle(dc, curValueText, r, bRedBrush);
		DeleteObject(bRedBrush);
	}
	else if (LoadRC::unicode_to_ansi(g_pLanguageApi->LangText(PriceSymmetry)) == curText)
	{
		HBRUSH bLightBuleBrush = CreateSolidBrush(g_ColorRefData.GetColorLightBlue());
		DrawFillRectangle(dc, curValueText, r, bLightBuleBrush);
		DeleteObject(bLightBuleBrush);
	}
	else if (LoadRC::unicode_to_ansi(g_pLanguageApi->LangText(Volume)) == curText)
	{
		HBRUSH bRGB150Brush = CreateSolidBrush(g_ColorRefData.GetColorRGB150());
		DrawFillRectangle(dc, curValueText, r, bRGB150Brush);
		DeleteObject(bRGB150Brush);
	}
	else if (LoadRC::unicode_to_ansi(g_pLanguageApi->LangText(Clarity2)) == curText)
	{
		HBRUSH bLightBuleBrush = CreateSolidBrush(g_ColorRefData.GetColorLightBlue());
		DrawFillRectangle(dc, curValueText, r, bLightBuleBrush);
		DeleteObject(bLightBuleBrush);
	}
	else if (LoadRC::unicode_to_ansi(g_pLanguageApi->LangText(Direction)) == curText)
	{
		int direction = atoi(curText.c_str());
		string itemText = "";
		RECT dr = { r.left + 100, r.top + 10 , r.left + 105 , r.top + 18 };
		RECT r2 = { r.left + 100, r.top, r.right + 200, r.bottom };
		if (direction == 1)
		{
			/*HBRUSH bRedBrush = CreateSolidBrush(KLineRedColor);
			SelectBrush(dc.GetHdc(), bRedBrush);
			POINT gPoint[4] = { { dr.left + 5, dr.top + 5 },{ dr.left, dr.top + 15 },{ dr.left + 10, dr.top + 15 },{ dr.left + 5, dr.top + 5 } };
			Polygon(dc.GetHdc(), gPoint, 4);
			DeleteObject(bRedBrush);*/
			itemText = "up";
		}
		else
		{
			/*HBRUSH bGreenBrush = CreateSolidBrush(KLineGreenColor);
			SelectBrush(dc.GetHdc(), bGreenBrush);
			POINT gPoint[4] = { { dr.left , dr.top + 5 },{ dr.left + 10, dr.top + 5 },{ dr.left + 5, dr.top + 15 },{ dr.left, dr.top + 5 } };
			Polygon(dc.GetHdc(), gPoint, 4);
			DeleteObject(bGreenBrush);*/
			itemText = "down";
		}
		DrawTextA(dc.GetHdc(), itemText.c_str(), itemText.length(), &r2, DT_LEFT | DT_EDITCONTROL | DT_VCENTER);
	}
	else if (LoadRC::unicode_to_ansi(g_pLanguageApi->LangText(IdentifiedTime)) == curText)
	{
		int ndatetime = atoi(curValueText.c_str());
		string sdatetime = KLineUtil::UnixTime2Str(ndatetime);
		RECT r2 = { r.left + 100, r.top, r.right + 200, r.bottom };
		DrawTextA(dc.GetHdc(), sdatetime.c_str(), sdatetime.length(), &r2, DT_LEFT | DT_EDITCONTROL | DT_VCENTER);
	}
	else
	{
		RECT r2 = { r.left + 100, r.top, r.right + 200, r.bottom };
		DrawTextA(dc.GetHdc(), curValueText.c_str(), curValueText.length(), &r2, DT_LEFT | DT_EDITCONTROL | DT_VCENTER);
	}
}

void KLineIntroductView::DrawFillRectangle(TMemDC& dc, string curValueText, RECT r, HBRUSH &bBrush)
{
	RECT qr = { r.left + 100, r.top + 10 , r.left + 105 , r.top + 18 };
	int Cnt = atoi(curValueText.c_str());
	if (Cnt == 0)
	{
		RECT tr = { r.left + 100, r.top, r.right + 200, r.bottom };
		curValueText = "N/A";
		DrawTextA(dc.GetHdc(), curValueText.c_str(), curValueText.length(), &tr, DT_LEFT | DT_EDITCONTROL | DT_VCENTER);
		return;
	}
	for (int i = 0; i < Cnt; ++i)
	{
		FillRect(dc.GetHdc(), &qr, bBrush);
		qr.left += 6;
		qr.right = qr.left + 5;
	}
}
