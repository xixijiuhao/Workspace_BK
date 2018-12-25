#include "BaseInclude.h"

TTabs::TTabs()
{
	m_hfont = CreateFont(18, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
		CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("微软雅黑"));
	m_iTabWidth = 60;
	m_iButtonWidth = 15;
	m_iEdge = 5;
	m_iTabHeight = 20;
	m_bButton = false;
	m_bButtonHover = false;
	m_iSelIndex = 0;
	memset(&m_rcButton, 0, sizeof(RECT));

	m_clrNormal = RGB(230, 230, 230);
	m_clrSelect = RGB(240, 240, 240);
	m_clrFrmHover = RGB(100, 100, 90);
	m_clrFrame = RGB(150, 150, 150);
}

TTabs::~TTabs()
{
	DeleteObject(m_hfont);
}



bool TTabs::Create(HWND hParent)
{
	CreateFrm(_T("TTabTags"), hParent, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN);
	return true;
}

void TTabs::MoveWindow(const int& x, const int& y, const int& cx, const int& cy)
{
	if (cx < 0 || cy < 0)return;
	m_iHeight = cy;
	m_iWidth = cx;
	SetWindowPos(m_Hwnd, NULL, x, y, cx, cy, SWP_NOZORDER);
	CalculateTabRect();
	RECT rc;
	GetClientRect(m_Hwnd, &rc);
	InvalidateRect(m_Hwnd, &rc, true);
}

void TTabs::CalculateTabRect()
{
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	if (m_iTabWidth <= 0)m_iTabWidth = 1;
	m_iMaxDisplay = max(m_iWidth - m_iEdge,0) / m_iTabWidth;
	//先确定选中的tab index,若index+1>maxdisplay,则从index-maxdisplay+1开始画，其余送入下拉菜单
	if (m_iSelIndex + 1 > m_iMaxDisplay)
	{
		m_iBeginIndex = m_iSelIndex - m_iMaxDisplay + 1;
		m_iEndIndex = m_iSelIndex;
	}
	else
	{
		m_iBeginIndex = 0;
		m_iEndIndex = m_iMaxDisplay - 1;//这个endIndex，若是总共的tab个数不足以撑满，则表示最多显示到的index
	}
	std::vector<TabContrl>::iterator iter;
	int cnt = 0;
	for (iter = m_TabVec.begin(); iter != m_TabVec.end(); iter++, cnt++)
	{
		//赋值tabcontrl位置
		TabContrl tab = *iter;
		tab.bShow = true;
		if (tab.iIndex<m_iBeginIndex || tab.iIndex>m_iEndIndex)//隐藏，放入下拉菜单
		{
			tab.bShow = false;
			m_TabVec[cnt] = tab;
			//m_DrawMenuVec.push_back(tab);//这里会自动创建副本存入么？MARK
			continue;
		}
		tab.tabRect.top = rect.top;
		tab.tabRect.bottom = rect.bottom;
		tab.tabRect.left = rect.left + m_iEdge + (tab.iIndex - m_iBeginIndex)*m_iTabWidth;
		tab.tabRect.right = tab.tabRect.left + m_iTabWidth;
		m_TabVec[cnt] = tab;
	}
	if (cnt > m_iMaxDisplay)
	{
		m_bButton = true;
		m_rcButton.top = rect.top;
		m_rcButton.bottom = rect.bottom;
		m_rcButton.right = rect.right;
		m_rcButton.left = rect.right - m_iButtonWidth;
	}
	else
	{
		m_bButton = false;
	}
}

const TabContrl* TTabs::GetSelItem()
{
	if (m_iSelIndex > m_TabVec.size() || m_iSelIndex < 0)//防止越界
	{
		return NULL;
	}
	else
	{
		return &m_TabVec[m_iSelIndex];
	}
}

void TTabs::SetData(const std::vector<std::string>& tabstrVec)
{
	m_TabVec.clear();
	int index = 0;
	std::vector<std::string>::const_iterator iter;
	for (iter = tabstrVec.begin(); iter != tabstrVec.end(); iter++, index++)
	{
		TabNode stnode;
		stnode.sztext = *iter;
		stnode.iIndex = index;
		m_TabVec.push_back(stnode);
		if (index == m_iSelIndex)
		{
			stnode.iState = TABS_SELECTED;
		}
	}

}

LRESULT TTabs::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:
		OnPaint();
		return PROCESSED;
	case WM_LBUTTONDOWN:
		OnLButtonDown(wParam, lParam);
		return PROCESSED;
	default:
		return NOT_PROCESSED;
	}

}

void TTabs::OnPaint()
{
	TMemDC memDC(m_Hwnd);
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	HBRUSH bk = CreateSolidBrush(g_ColorRefData.GetColorBackground());
	FillRect(memDC.GetHdc(), &rect, bk);
	DrawTab(&memDC);
	DrawButton(&memDC);
	DeleteObject(bk);
}

void TTabs::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
	POINT pt;
	pt.x = LOWORD(lParam);
	pt.y = HIWORD(lParam);
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	if (m_bButton&&PtInRect(&m_rcButton,pt))
	{
		OnDrawButtonDown();//如果有下拉菜单，处理下拉菜单，然后直接return；
		return;
	}
	int cnt = 0;
	std::vector<TabContrl>::iterator iter;
	for (iter = m_TabVec.begin(); iter != m_TabVec.end(); iter++, cnt++)
	{
		TabContrl stnode = (*iter);
		if (PtInRect(&stnode.tabRect, pt)&&stnode.bShow)
		{
			if (m_iSelIndex == cnt)return;//如果选中原来那个就不发了
			m_iSelIndex = cnt;
			PostMessage(::GetParent(m_Hwnd), SSWM_TABTAG_SELECTED, 0, (LPARAM)cnt);
			m_TabVec[cnt].iState = TABS_SELECTED;
			if (m_iSelIndex + 1 > m_iMaxDisplay)
			{
				m_iBeginIndex = m_iSelIndex - m_iMaxDisplay + 1;
				m_iEndIndex = m_iSelIndex;
			}
			else
			{
				m_iBeginIndex = 0;
				m_iEndIndex = m_iMaxDisplay - 1;
			}
		}
		stnode.iState = TABS_NORMAL;
		stnode.bShow = true;
		m_TabVec[cnt] = stnode;
	}
	for (iter = m_TabVec.begin(), cnt = 0; iter != m_TabVec.end(); iter++, cnt++)
	{
		//再次赋值tabcontrl位置，这样逻辑不容易乱，交换位置交换的很乱，而且不可避免地，都需要遍历两次vector
		//想出好的方案再说吧
		TabContrl stnode = *iter;
		if (stnode.iIndex<m_iBeginIndex || stnode.iIndex>m_iEndIndex)
		{
			stnode.bShow = false;
			m_TabVec[cnt] = stnode;
			continue;
		}
		stnode.tabRect.top = rect.top;
		stnode.tabRect.bottom = rect.bottom;
		stnode.tabRect.left = m_iEdge + rect.left + (stnode.iIndex - m_iBeginIndex)*m_iTabWidth;
		stnode.tabRect.right = stnode.tabRect.left + m_iTabWidth;
		m_TabVec[cnt] = stnode;
	}
	InvalidateRect(m_Hwnd, NULL, false);
}
void TTabs::OnDrawButtonDown()
{

}


void TTabs::DrawTab(TMemDC *pmemdc)
{
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	SelectObject(pmemdc->GetHdc(), m_hfont);
	SetBkMode(pmemdc->GetHdc(), TRANSPARENT); 
	SetTextColor(pmemdc->GetHdc(), RGB(50, 50, 50));
	HBRUSH hbrS = CreateSolidBrush(m_clrSelect);
	HBRUSH hbrN = CreateSolidBrush(m_clrNormal);
	HPEN hpen = CreatePen(PS_SOLID, 1, m_clrFrame);
	HPEN hpenH = CreatePen(PS_SOLID, 1, m_clrFrmHover);

	std::vector<TabContrl>::iterator iter;
	for (iter = m_TabVec.begin(); iter != m_TabVec.end();iter++)
	{
		if (!(iter->bShow))continue;
		TabNode stnode = *iter;
		if (stnode.iState == TABS_SELECTED)continue;
		SelectObject(pmemdc->GetHdc(), hbrN);
		SelectObject(pmemdc->GetHdc(), hpen);
		POINT pt[4];
		pt[0].x = stnode.tabRect.left + m_iEdge;
		pt[1].x = stnode.tabRect.right - m_iEdge;
		pt[2].x = stnode.tabRect.right + m_iEdge;
		pt[3].x = stnode.tabRect.left - m_iEdge;
		pt[0].y = pt[1].y = stnode.tabRect.top;
		pt[2].y = pt[3].y = stnode.tabRect.bottom;
		Polygon(pmemdc->GetHdc(), pt, 4);
		DrawTextA(pmemdc->GetHdc(), stnode.sztext.c_str(),-1,&stnode.tabRect,DT_CENTER|DT_VCENTER);
		/*if (stnode.iState == TABS_HOVER)鼠标悬停先不做*/

	}
	TabNode stnode = m_TabVec[m_iSelIndex];
	POINT pt[4];
	pt[0].x = stnode.tabRect.left + m_iEdge;
	pt[1].x = stnode.tabRect.right - m_iEdge;
	pt[2].x = stnode.tabRect.right + m_iEdge;
	pt[3].x = stnode.tabRect.left - m_iEdge;
	pt[0].y = pt[1].y = stnode.tabRect.top;
	pt[2].y = pt[3].y = stnode.tabRect.bottom;
	SelectObject(pmemdc->GetHdc(), hbrS);
	SelectObject(pmemdc->GetHdc(), hpen);
	Polygon(pmemdc->GetHdc(), pt, 4);
	SetTextColor(pmemdc->GetHdc(), RGB(40,40,40));
	DrawTextA(pmemdc->GetHdc(), stnode.sztext.c_str(), -1, &stnode.tabRect, DT_CENTER | DT_VCENTER);
	HPEN hpenN = CreatePen(PS_SOLID, 1, m_clrNormal);
	SelectObject(pmemdc->GetHdc(), hpenN);
	MoveToEx(pmemdc->GetHdc(), pt[2].x, pt[2].y, NULL);
	LineTo(pmemdc->GetHdc(), pt[3].x, pt[3].y);

	DeleteObject(hpen);
	DeleteObject(hpenH);
	DeleteObject(hpenN);
	DeleteObject(hbrN);
	DeleteObject(hbrS);
}

void TTabs::DrawButton(TMemDC *pmemdc)
{
	if (!m_bButton)return;
	HPEN hPen = CreatePen(PS_SOLID, 1, m_clrNormal);
	HPEN hPenF = CreatePen(PS_SOLID, 1, m_clrSelect);
	POINT pt[3];
	pt[0].x = m_rcButton.left - 1;
	pt[0].y = m_rcButton.top + 5;
	pt[1].x = m_rcButton.left + m_iButtonWidth / 2 - 1;
	pt[1].y = m_rcButton.top + m_iHeight / 3 + 6;
	pt[2].x = m_rcButton.right - 2;
	pt[2].y = m_rcButton.top + 5;

	SelectObject(pmemdc->GetHdc(), hPenF);
	MoveToEx(pmemdc->GetHdc(), pt[0].x, pt[0].y - 1, NULL);
	LineTo(pmemdc->GetHdc(), pt[1].x, pt[1].y - 1);
	LineTo(pmemdc->GetHdc(), pt[2].x, pt[2].y - 1);
	//MoveToEx(pmemdc->GetHdc(), pt[0].x - 1, pt[0].y + 1, NULL);
	//LineTo(pmemdc->GetHdc(), pt[1].x, pt[1].y + 2);
	//LineTo(pmemdc->GetHdc(), pt[2].x + 2, pt[2].y );
	SelectObject(pmemdc->GetHdc(), hPen);
	MoveToEx(pmemdc->GetHdc(), pt[0].x, pt[0].y, NULL);
	LineTo(pmemdc->GetHdc(), pt[1].x, pt[1].y);
	LineTo(pmemdc->GetHdc(), pt[2].x + 1, pt[2].y - 1);
	MoveToEx(pmemdc->GetHdc(), pt[0].x, pt[0].y + 1, NULL);
	LineTo(pmemdc->GetHdc(), pt[1].x, pt[1].y + 1);
	LineTo(pmemdc->GetHdc(), pt[2].x + 1, pt[2].y);
	DeleteObject(hPen);
	DeleteObject(hPenF);
}