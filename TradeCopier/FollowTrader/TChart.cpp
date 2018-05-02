#include "PreInclude.h"

#define    topGap    15
#define    bottomGap 15
#define    rightGap  50

#define    minYLineGap 25

#define    chartArrowLength 12
TChartFrm::TChartFrm()
{
	m_iVTextWidth = 30;
	m_iDrawIndxGap = 1;
	m_iYVerticalGap = minYLineGap;
}
TChartFrm::~TChartFrm()
{

}
void TChartFrm::Create(HWND hParent)
{
	CreateFrm(L"ChartFrm", hParent, WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS);
}
void TChartFrm::MoveWindow(int x, int y, int cx, int cy)
{
	SetWindowPos(m_Hwnd, NULL, x, y, cx, cy, SWP_SHOWWINDOW);
	//m_rcChart.left = m_iVTextWidth;
	//m_rcChart.top = topGap;
	//m_rcChart.right =  cx - rightGap;
	//m_rcChart.bottom = cy - bottomGap;
	//
	//m_iXLength = m_rcChart.right - m_rcChart.left - chartArrowLength;
	//m_iYHeight = m_rcChart.bottom - m_rcChart.top - chartArrowLength;
	//m_startPt.x = m_rcChart.left /*+ 5*/;
	//m_startPt.y = m_rcChart.bottom - m_iYHeight / 2; /*- 5*/;
	SetChartSize();
	DealData();
}
void TChartFrm::SetChartSize()
{
	RECT rect = { 0 };
	GetClientRect(m_Hwnd, &rect);
	m_rcChart.left = m_iVTextWidth;
	int cx = rect.right - rect.left;
	int cy = rect.bottom - rect.top;
	m_rcChart.top = topGap;
	m_rcChart.right = cx - rightGap;
	m_rcChart.bottom = cy - bottomGap;

	m_iXLength = m_rcChart.right - m_rcChart.left - chartArrowLength;
	m_iYHeight = m_rcChart.bottom - m_rcChart.top - chartArrowLength;
	m_startPt.x = m_rcChart.left /*+ 5*/;
	m_startPt.y = m_rcChart.bottom - m_iYHeight / 2; /*- 5*/;
}
void TChartFrm::SetData(vector<long long>*pMoneyData)
{
	m_pTradeData = pMoneyData;
	DealData();
}
void TChartFrm::InitData()
{
	m_vPoints.clear();
	m_vecYLine.clear();
	m_vecYLineString.clear();
	m_dYPerMoney = 0;
	m_iYVerticalGap = minYLineGap;
}
long long TChartFrm::GetMaxDiff()
{
	long long maxData = 0;
	for (UINT i = 1; i < m_pTradeData->size(); i++)
	{
		long long tempDiff = abs(m_pTradeData->at(i) - m_pTradeData->at(0));
		if (tempDiff > maxData)
			maxData = tempDiff;
	}
	return maxData;
}
void TChartFrm::CalcData()
{
	m_iIndexGap = 1;
	if (m_pTradeData->size() > 1)
	{
		//点与点之间最小间隔为2个像素，计算隔多少个数据取一个数据，数据太多无法显示的情况下
		while (m_pTradeData->size() * 2 / m_iIndexGap >= m_iXLength) //2表示2个像素
		{
			m_iIndexGap++;
		}
		m_iHorziontalGap = m_iXLength*m_iIndexGap / (m_pTradeData->size() - 1);

		//计算间隔多少点写一个坐标
		m_iDrawIndxGap = 1;
		m_iDrawThresh = 18;
		if (m_pTradeData->size() > 100)
			m_iDrawThresh = 26;
		else if (m_pTradeData->size() > 1000)
			m_iDrawThresh = 32;
		while (m_iHorziontalGap*m_iDrawIndxGap < m_iDrawThresh)
			m_iDrawIndxGap++;
	}
	else
		m_iHorziontalGap = m_iXLength;
}
void TChartFrm::CalcYLineGap(long long maxDiff)
{
	if (maxDiff == 0)return;
	m_dYPerMoney = maxDiff * 2 *1.0/ m_iYHeight*minYLineGap;
	m_iYVerticalGap = minYLineGap;
	if (m_dYPerMoney > 20 && m_dYPerMoney < 400)
	{
		m_dYPerMoney = m_dYPerMoney - m_dYPerMoney % 10 + 10; //向整十取整
		m_iYVerticalGap = m_iYHeight*m_dYPerMoney/(maxDiff * 2) ;
	}
	else if (m_dYPerMoney > 400)
	{
		m_dYPerMoney = m_dYPerMoney - m_dYPerMoney % 100 + 100;
		m_iYVerticalGap = m_iYHeight*m_dYPerMoney / (maxDiff * 2);
	}

	if (m_iYVerticalGap)
	{
		int iCount = m_iYHeight / m_iYVerticalGap + 1;
		int iTemp = 0;
		TCHAR  wszText[30] = { 0 };
		for (int i = 0; i < iCount; i++)
		{
			iTemp = (m_iYVerticalGap)*(i + 1);
			if (iTemp < m_iYHeight / 2)
			{
				m_vecYLine.push_back(m_startPt.y + iTemp);
				wsprintf(wszText, L"%d", (int)(-m_dYPerMoney)*(i + 1));
				m_vecYLineString.push_back(wszText);
				m_vecYLine.push_back(m_startPt.y - iTemp);
				wsprintf(wszText, L"%d", (int)m_dYPerMoney*(i + 1));
				m_vecYLineString.push_back(wszText);
			}
			else
				break;
		}
		int iSize = m_vecYLineString.size();
		if (iSize)
		{
			m_iVTextWidth = m_vecYLineString[iSize - 1].size() * 6 + 3;
			SetChartSize();
		}
	}

}
void TChartFrm::SetPoint(long long maxDiff)
{
	if (maxDiff)     //如果差值不为0
	{
		m_dVerticalGap = maxDiff*1.0 / m_iYHeight * 2; //计算平均多少钱代表一个像素
		m_bIsLine = false;
		POINT pt = { 0, 0 };
		pt.x = m_startPt.x;
		pt.y = m_startPt.y;
		m_vPoints.push_back(pt);
		for (UINT i = 1; i < m_pTradeData->size(); i++)
		{
			if (i%m_iIndexGap == 0)
			{
				pt.x += m_iHorziontalGap;
				pt.y = m_startPt.y - (m_pTradeData->at(i) - m_pTradeData->at(0)) / m_dVerticalGap;
				m_vPoints.push_back(pt);
			}
		}
	}
	else  //一条直线
	{
		POINT pt = { 0, 0 };
		m_vPoints.push_back(pt);
		m_bIsLine = true;
	}
}
void TChartFrm::DealData()
{
	InitData();
	if (m_pTradeData->size() == 0)
	{
		InvalidateRect(m_Hwnd, NULL, false);
		return;
	}
	long long maxData = GetMaxDiff();
	CalcYLineGap(maxData);
	CalcData(); // maxData = 0说明是一条直线
	SetPoint(maxData);
	
	InvalidateRect(m_Hwnd, NULL, false);
}
LRESULT TChartFrm::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:
		OnPaint();
		return PROCESSED;
	case WM_CREATE:
		OnCreate();
		return PROCESSED;
	default:
		return NOT_PROCESSED;
	}
}
void TChartFrm::OnCreate()
{
}
void TChartFrm::OnPaint()
{
	TMemDC memdc(m_Hwnd);
	SetBkMode(memdc.GetHdc(), TRANSPARENT);
	SelectObject(memdc.GetHdc(), g_FontData.g_FontWord12);
	HBRUSH hbr = CreateSolidBrush(RGB(255, 255, 255));
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	FillRect(memdc.GetHdc(), &rect, hbr);
	DrawLine(&memdc);
	DrawArrow(&memdc, m_rcChart);
	DeleteObject(hbr);
}
void TChartFrm::DrawLine(TMemDC *pMemDC)
{
	HPEN hPen = CreatePen(PS_DOT, 1, RGB(50, 50, 50));
	SelectObject(pMemDC->GetHdc(), hPen);
	int iY = m_startPt.y ;
	int iEndX = m_startPt.x + m_iXLength;
	MoveToEx(pMemDC->GetHdc(), m_startPt.x, iY, 0);
	LineTo(pMemDC->GetHdc(), iEndX, iY);
	RECT rect = { iEndX, iY - 8, iEndX + 40, iY + 8 };
	::DrawText(pMemDC->GetHdc(), L"盈亏线", -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	rect = { 0, m_startPt.y - 6, m_iVTextWidth, m_startPt.y + 6 }; 
	::DrawText(pMemDC->GetHdc(), L"0", -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	rect = { m_rcChart.right, m_rcChart.bottom - 8, m_rcChart.right + 50, m_rcChart.bottom + 8 };
	::DrawText(pMemDC->GetHdc(), L"平仓次数", -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	rect = { m_rcChart.left + 6, m_rcChart.top - 4, m_rcChart.left + 43, m_rcChart.top + 12 };
	::DrawText(pMemDC->GetHdc(), L"盈亏额", -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	for (int i = 0; i < m_vPoints.size(); i++)
	{
		if (i > 0&& i%m_iDrawIndxGap==0)
		{
			MoveToEx(pMemDC->GetHdc(), m_vPoints[i].x, m_rcChart.bottom, 0);
			LineTo(pMemDC->GetHdc(), m_vPoints[i].x, m_rcChart.bottom - m_iYHeight);
		}
	}

	for (int i = 0; i < m_vecYLine.size(); i++)
	{
		int xStart = m_startPt.x;
		int xEnd = m_startPt.x + m_iXLength;
		MoveToEx(pMemDC->GetHdc(), xStart, m_vecYLine[i], 0);
		LineTo(pMemDC->GetHdc(), xEnd, m_vecYLine[i]);
		RECT rcText = { 0, m_vecYLine[i] - 6, m_iVTextWidth, m_vecYLine[i] + 6 };
		DrawText(pMemDC->GetHdc(), m_vecYLineString[i].c_str(), -1, &rcText, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}
	DeleteObject(hPen);
}
void TChartFrm::DrawScale(TMemDC *pMemDC)
{
	RECT rc = m_rcChart;
	HPEN hPen = CreatePen(PS_SOLID, 1, RGB(30, 30, 30));

	SelectObject(pMemDC->GetHdc(), hPen);
	MoveToEx(pMemDC->GetHdc(), rc.left, rc.top, 0);
	LineTo(pMemDC->GetHdc(), rc.left, rc.bottom);
	LineTo(pMemDC->GetHdc(), rc.right, rc.bottom);

	POINT arrowPt[3];
	int height = 8;
	int width = 4;
	arrowPt[0].y = rc.top + 8;
	arrowPt[0].x = rc.left - 4;
	arrowPt[1].x = rc.left;
	arrowPt[1].y = rc.top;
	arrowPt[2].x = rc.left + 4;
	arrowPt[2].y = rc.top + 8;
	HBRUSH hbrBk = CreateSolidBrush(RGB(30, 30, 30));
	SelectObject(pMemDC->GetHdc(), hbrBk);
	Polygon(pMemDC->GetHdc(), arrowPt, 3);
	arrowPt[0].x = rc.right - height;
	arrowPt[0].y = rc.bottom - width;
	arrowPt[1].x = rc.right;
	arrowPt[1].y = rc.bottom;
	arrowPt[2].x = rc.right - height;
	arrowPt[2].y = rc.bottom + width;
	Polygon(pMemDC->GetHdc(), arrowPt, 3);
	if (m_vPoints.size() > 1)
	{
		for (UINT i = 1; i < m_vPoints.size(); i++)
		{
			MoveToEx(pMemDC->GetHdc(), m_vPoints[i].x, rc.bottom, 0);
			LineTo(pMemDC->GetHdc(), m_vPoints[i].x, rc.bottom - 5);
		}
	}
	DeleteObject(hPen);
	DeleteObject(hbrBk);
}
void TChartFrm::DrawArrow(TMemDC *pMemDC, RECT rect)
{
	DrawScale(pMemDC);
	HPEN hLinePen = CreatePen(PS_SOLID, 2, RGB(91, 155, 213));
	HBRUSH hbrBk = CreateSolidBrush(RGB(91, 155, 213));
	SelectObject(pMemDC->GetHdc(), hbrBk);
	SelectObject(pMemDC->GetHdc(), hLinePen);
	if (m_vPoints.size() != 0)
	{
		if (!m_bIsLine)
		{
			UINT iCount = m_vPoints.size();
			MoveToEx(pMemDC->GetHdc(), m_vPoints[0].x, m_vPoints[0].y, 0);

			for (UINT i = 0; i < m_vPoints.size(); i++)
			{
				LineTo(pMemDC->GetHdc(), m_vPoints[i].x, m_vPoints[i].y);
				Ellipse(pMemDC->GetHdc(), m_vPoints[i].x - 3, m_vPoints[i].y - 3, m_vPoints[i].x + 3, m_vPoints[i].y + 3);
				if (i%m_iDrawIndxGap == 0)
				{
					RECT rect = { m_vPoints[i].x - m_iDrawThresh / 2, m_rcChart.bottom + 3, m_vPoints[i].x + m_iDrawThresh / 2, m_rcChart.bottom + 16 };
					TCHAR szText[20] = { 0 };
					wsprintf(szText, L"%d", i*m_iIndexGap);
					::DrawText(pMemDC->GetHdc(), szText, -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				}
			}
		}
		else
		{
			MoveToEx(pMemDC->GetHdc(), m_startPt.x, m_startPt.y , 0);
			LineTo(pMemDC->GetHdc(), m_startPt.x + m_iXLength, m_startPt.y);
		}
	}
	DeleteObject(hLinePen);
}