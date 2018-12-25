#include "..\PreInclude.h"
//ɫ��
extern G_COLOREF g_ColorRefData;
extern G_UIFONT g_FontData;
extern IStrategyTradeAPI *g_pStrategyApi;

#define NArrowWidth	7
#define NArrowOffset 5

CMouseHeadWin::CMouseHeadWin() 
: m_opr(NULL)
, m_nPos(0)
, m_nSelect(0)
, m_nPosDown(-1)
, m_nWidth(N_MOUSEHEAD_WIDTH/4)
, m_nHeight(N_MOUSEHEAD_HEIGHT)
, m_nUintx(N_MOUSEHEAD_WIDTH/4)
, m_bBatch(false)
, m_bTrader(true)
, m_bStopTrade(true)
{

}

CMouseHeadWin::~CMouseHeadWin()
{

}

int CMouseHeadWin::CreateHeadWin(CMousewin* opr, LPCTSTR classname, int nLeft, HWND parent, DWORD style)
{
	m_opr = opr;
	m_nLeft = nLeft;
	m_nUintx = N_MOUSEHEAD_WIDTH / 4;
	CreateFrm(classname, parent, style | WS_CHILD, 0);
	m_vstring.push_back(LoadResWchar_t(IDS_VertOrder));
	return 0;
}
int CMouseHeadWin::UpdateData(int nSize, TMouPriceTypeStrW str[10], bool bClearSpecial)
{
	//������������Ϣ
	if (bClearSpecial)
		m_sSpecial.clear();

	//����ǰ����Ϣ ����
	wstring wlast = m_vstring.size()>m_nSelect?m_vstring[m_nSelect]:m_sSpecial;	
	m_nSelect = 0;
	m_nPos = 0;
	m_vstring.clear();

	//��������Ԫ��
	for (int i=0;i<nSize;i++)
		m_vstring.push_back(str[i]);
	//ǰ��ѡ�� ӳ��
	bool bFind = false;
	for (uint i = 0; i < m_vstring.size(); i++)
	{
		if (wlast== m_vstring[i])
		{
			m_nSelect = i;
			m_nPos = m_nSelect;
			bFind = true;
			break;
		}
	}

	//���ÿ��
	m_nWidth = m_nUintx*m_vstring.size();

	//�������
	if (!m_sSpecial.empty())
	{
		vector<wstring>::iterator it = find(m_vstring.begin(), m_vstring.end(), m_sSpecial);
		if (it == m_vstring.end())
			m_nWidth = m_nUintx*(m_vstring.size() + 1);
		else
			m_sSpecial.clear();
	}
	
	//ͬ��ѡ��
	if (m_vstring.size() > m_nSelect)
		m_opr->SetHeadData(m_vstring[m_nSelect].c_str());
	else if (!m_sSpecial.empty())
		m_opr->SetHeadData(m_sSpecial.c_str());
	else//�޹̶��� �������� ��ʾ����Ĭ�Ͻ���
	{
		m_sSpecial = LoadResWchar_t(IDS_VertOrder);
		m_nWidth = m_nUintx*(m_vstring.size() + 1);
		m_opr->SetHeadData(m_sSpecial.c_str());
	}
		
	MoveWindow(m_Hwnd, m_nLeft, m_nTop, m_nWidth, m_nHeight, TRUE);

	if (bFind&&m_vstring.size() > 1)
		return 1;//�ҵ�ƥ�� ��ʾ
	else 
		return 0;//δ�ҵ�ƥ��
}
bool CMouseHeadWin::SetShowData(const wchar_t* wText)
{
	bool bFind = false;
	for (uint i = 0; i < m_vstring.size();i++)
	{
		if (!wcscmp(wText, m_vstring[i].c_str()))
		{
			m_nSelect = i;
			m_nPos = m_nSelect;
			InvalidateRect(m_Hwnd, NULL, TRUE);
			bFind = true;
			break;
		}
	}	
	if (!bFind)
	{
		if (m_sSpecial.empty())
		{
			m_sSpecial = wText;
			m_nSelect = m_vstring.size();
			m_nPos = m_nSelect;
			m_nWidth = m_nUintx*(m_vstring.size() + 1);
			MoveWindow(m_Hwnd, m_nLeft, m_nTop, m_nWidth, m_nHeight, TRUE);
		}
		else
		{
			m_sSpecial = wText;
			m_nSelect = m_vstring.size();
			m_nPos = m_nSelect;
			InvalidateRect(m_Hwnd, NULL, TRUE);
			UpdateWindow(m_Hwnd);
		}
	}
	return bFind;
}

LRESULT CMouseHeadWin::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC			hdc;
	PAINTSTRUCT ps;

	switch (message)
	{
	case WM_CREATE:
		{
			hdc = GetDC(m_Hwnd);		 	
			m_HdcMem = CreateCompatibleDC(hdc);
			ReleaseDC(m_Hwnd, hdc);

			MoveWindow(m_Hwnd, m_nLeft, m_nTop, m_nWidth, m_nHeight, TRUE);
		}
		break;
	case WM_SIZE:
		GetClientRect(m_Hwnd, &m_reClient);
		m_nWidth = m_reClient.right - m_reClient.left;
		m_nHeight = m_reClient.bottom - m_reClient.top;
		//////////////////////////////////////////////////////////////////////////
		hdc = GetDC(m_Hwnd);

		if (m_hBKBitmap)
			DeleteObject(m_hBKBitmap);
	
		m_hBKBitmap = CreateCompatibleBitmap(hdc, m_nWidth, m_nHeight);
		SelectObject(m_HdcMem, m_hBKBitmap);	

		ReleaseDC(m_Hwnd, hdc);

		m_opr->OnSizeHeadWidth();
		//////////////////////////////////////////////////////////////////////////
		break;
	case WM_PAINT:
	{
		HBRUSH  hFGBrush = g_ColorRefData.GetHbrBackground();	
		HPEN hpen = CreatePen(PS_SOLID, 1, g_ColorRefData.GetColorRGB150());
		
		hdc = BeginPaint(m_Hwnd, &ps);

		FillRect(m_HdcMem, &m_reClient, hFGBrush);

		//����
		SelectObject(m_HdcMem, hpen);
		MoveToEx(m_HdcMem, m_reClient.left-1, m_reClient.bottom-1, NULL);
		LineTo(m_HdcMem, m_reClient.right, m_reClient.bottom-1);

		SelectObject(m_HdcMem, g_FontData.GetFontWord13());	//����������
		SetBkMode(m_HdcMem, TRANSPARENT);					//�������屳�� 

		for (int i = 0; i < m_nWidth / m_nUintx/*&& i < int(m_vstring.size())*/; i++)	
			DrawItem(i);
		
		BitBlt(hdc, 0, 0, m_nWidth, m_nHeight, m_HdcMem, 0, 0, SRCCOPY);

		EndPaint(m_Hwnd, &ps);
		DeleteObject(hpen);
		
	}	
		break;
	case WM_LBUTTONDOWN:
		{
			POINT pointB;
			pointB.x = LOWORD(lParam);
			pointB.y = HIWORD(lParam);
			m_nPosDown = pointB.x / m_nUintx + pointB.y / m_nHeight*(m_nWidth / m_nUintx);
		}
		break;
	case WM_LBUTTONUP:
		{
			POINT pointB;
			pointB.x = LOWORD(lParam);
			pointB.y = HIWORD(lParam);
			m_nPos = pointB.x / m_nUintx + pointB.y / m_nHeight*(m_nWidth / m_nUintx);
			if (m_nPos < m_vstring.size() && m_nPosDown == m_nPos)
			{
				m_nSelect = m_nPos;
				if (m_opr)
					m_opr->SetHeadData(m_vstring[m_nSelect].c_str());
			}
			else if (m_nPos == m_vstring.size() && m_nPosDown == m_nPos)
			{
				m_nSelect = m_nPos;
				if (m_opr)
					m_opr->SetHeadData(m_sSpecial.c_str());
			}
			m_nPosDown = -1;
		}
		break;
	case WM_DESTROY:
		DeleteObject(m_hBKBitmap);
		DeleteDC(m_HdcMem);
		break;
	case WM_ERASEBKGND:
		break;
	default:                      /* ������Ȥ����Ϣ����DefWindowProc()���� */
		return NOT_PROCESSED;
	}

	return PROCESSED;
}
void CMouseHeadWin::DrawTriangle(HDC &hdc, int nStartx, int nStarty, int nWidth)//�������
{
	if (nWidth % 2 == 0)
		return;
	HPEN   hPen = CreatePen(PS_SOLID, 1, g_ColorRefData.GetColorRGB150());
	SelectObject(hdc, hPen);

	for (int i = 0; i < nWidth / 2 + 1; i++)
	{
		MoveToEx(hdc, nStartx + i, nStarty + i, NULL);
		LineTo(hdc, nStartx + nWidth - i, nStarty + i);
	}

	DeleteObject(hPen);
}
void CMouseHeadWin::DrawItem(int i)
{
	RECT		reFresh;
	reFresh.left = i*m_nUintx;
	reFresh.right = i*m_nUintx + m_nUintx - 1;
	reFresh.top = 0;
	reFresh.bottom = m_nHeight;

	//�����
	int nCut = 0;//(m_nSelect == i || m_nSelect - 1 == i) ? 0 : 4;
	MoveToEx(m_HdcMem, reFresh.right, reFresh.top + nCut, NULL);
	LineTo(m_HdcMem, reFresh.right, reFresh.bottom - nCut);

	if (m_nSelect == i)
	{
		SelectObject(m_HdcMem, g_FontData.GetFontWord13());	//����������
		HPEN hpen2 = CreatePen(PS_SOLID, 1, g_ColorRefData.GetColorBackground());
		//ѡ������߷ſ�
		HGDIOBJ OldPen = SelectObject(m_HdcMem, hpen2);
		MoveToEx(m_HdcMem, reFresh.left, reFresh.bottom - 1, NULL);
		LineTo(m_HdcMem, reFresh.right, reFresh.bottom - 1);
		SetTextColor(m_HdcMem, g_ColorRefData.GetColorGray());
		SelectObject(m_HdcMem, OldPen);
		DeleteObject(hpen2);
	}
	else
	{
		SelectObject(m_HdcMem, g_FontData.GetFontWord13());	//����������
		SetTextColor(m_HdcMem, g_ColorRefData.GetColorRGB150());
	}

	if (i<int(m_vstring.size()))
		DrawText(m_HdcMem, m_vstring[i].c_str(), m_vstring[i].size(), &reFresh, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	else
	{
		SetTextColor(m_HdcMem, m_nSelect == i?g_ColorRefData.GetColorLightRed(): RGB(200,100,100));
		DrawText(m_HdcMem, m_sSpecial.c_str(), m_sSpecial.size(), &reFresh, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}
}