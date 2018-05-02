#include "..\PreInclude.h"
//ɫ��
extern G_COLOREF g_ColorRefData;
extern G_UIFONT g_FontData;
extern IStopLossAndProfitAPI *g_pStopLossProfit;
CPopWin::CPopWin() 
:m_opr(NULL)
,m_nPos(0)
, m_nWidth(0)
, m_nHeight(0)
, m_nUintx(N_UNIT_WIDTH)
, m_nUinty(N_UNIT_HEIGHT)
, m_bBase(true)
{

}

CPopWin::~CPopWin()
{
	if (IsWindow(m_Hwnd))
		DestroyWindow(m_Hwnd);
}

void CPopWin::CreatePopWin(IOperator* opr, LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle, int nleft, int ntop, UINT ncol, UINT nrow)
{
	m_nCol = ncol;
	m_nRow = nrow;
	m_nLeft = nleft;
	m_nTop = ntop;
	m_opr = opr;
	UpdateData();
	//m_vstring.push_back(LoadRC::w_load_str(IDS_ARBAPP));
	//m_vstring.push_back(LoadRC::w_load_str(IDS_ARBSPLIT));
	//m_vstring.push_back(LoadRC::w_load_str(IDS_STRING1178));
	//m_vstring.push_back(LoadRC::w_load_str(IDS_STRING1179));
	
	CreateFrm(classname, parent, style | WS_POPUP | WS_BORDER, exstyle);
}
void CPopWin::UpdateData(bool bBase)
{
	if (m_bBase == bBase)
		return;
	m_bBase = bBase;
	m_vstring.clear();
	m_vstring.push_back(LoadRC::w_load_str(IDS_LIMIT_PRICE));
	m_vstring.push_back(LoadRC::w_load_str(IDS_MARKET_PRICE));
	if (!bBase&&m_opr&&m_opr->GetMouseWinCfg(m_MouseCfg))
	{
		if (m_MouseCfg.cFlag[Type_LimitStop])
			m_vstring.push_back(LoadRC::w_load_str(IDS_LIMIT_STOP_PRICE));
		if (m_MouseCfg.cFlag[Type_Stop])
			m_vstring.push_back(LoadRC::w_load_str(IDS_STOP_PRICE));
		if (m_MouseCfg.cFlag[Type_IceBerg])
			m_vstring.push_back(LoadRC::w_load_str(IDS_ICEBERG));
		if (m_MouseCfg.cFlag[Type_Shadow])
			m_vstring.push_back(LoadRC::w_load_str(IDS_SHADOW));
		if (m_MouseCfg.cFlag[Type_Swap])
			m_vstring.push_back(LoadRC::w_load_str(IDS_SWAP));
	}
}
LRESULT CPopWin::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC			hdc;
	RECT		reFresh;
	PAINTSTRUCT ps;

	switch (message)
	{
	case WM_CREATE:
		{
			hdc = GetDC(m_Hwnd);
		 
			//////////////////////////////////////////////////////////////////////////
			m_HdcMem = CreateCompatibleDC(hdc);
			m_HdcUnit = CreateCompatibleDC(hdc);
			//////////////////////////////////////////////////////////////////////////
			ReleaseDC(m_Hwnd, hdc);

			m_nUintx = N_UNIT_WIDTH;
			m_nUinty = N_UNIT_HEIGHT;
			m_reUnit.left = 0;
			m_reUnit.right = m_nUintx;
			m_reUnit.top = 0;
			m_reUnit.bottom = m_nUinty;

			m_nWidth = m_nCol*N_UNIT_WIDTH + 2;
			m_nHeight = m_nRow*N_UNIT_HEIGHT+2;

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
		if (m_hFGBitmap)
			DeleteObject(m_hFGBitmap);

		m_hBKBitmap = CreateCompatibleBitmap(hdc, m_nWidth, m_nHeight);
		SelectObject(m_HdcMem, m_hBKBitmap);
		m_hFGBitmap = CreateCompatibleBitmap(hdc, m_nUintx, m_nUinty);
		SelectObject(m_HdcUnit, m_hFGBitmap);

		ReleaseDC(m_Hwnd, hdc);
		//////////////////////////////////////////////////////////////////////////
		break;
	case WM_PAINT:
	{
		HBRUSH	hBKBrush = CreateSolidBrush(g_ColorRefData.GetColorWhite());	//������ˢ
		HBRUSH  hFGBrush = CreateSolidBrush(g_ColorRefData.GetColorBlue());	//ǰ����ˢ ��Ԫ
		
		hdc = BeginPaint(m_Hwnd, &ps);

		SelectObject(m_HdcUnit, g_FontData.GetFontWord15());

		FillRect(m_HdcMem, &m_reClient, hBKBrush);
		for (int j = 0; j < m_nHeight / m_nUinty; j++)//����ˢ�µ�Ԫ��
			for (int i = 0; i < m_nWidth / m_nUintx; i++)
			{
				reFresh.left = i*m_nUintx;
				reFresh.right = i*m_nUintx + m_nUintx;
				reFresh.top = j*m_nUinty;
				reFresh.bottom = j*m_nUinty + m_nUinty;
				if (m_nPos == i + j*(m_nWidth / m_nUintx))
				{
					FillRect(m_HdcUnit, &m_reUnit, hFGBrush);//������
					SetTextColor(m_HdcUnit, g_ColorRefData.GetColorWhite());// �����
				}
				else
				{
					FillRect(m_HdcUnit, &m_reUnit, hBKBrush);//������
					SetTextColor(m_HdcUnit, g_ColorRefData.GetColorBlue());// ������
				}
				SetBkMode(m_HdcUnit, TRANSPARENT);//�������屳�� 

				if (int(m_vstring.size()) > i + j*(m_nWidth / m_nUintx))
				{
					DrawText(m_HdcUnit, m_vstring[i + j*(m_nWidth / m_nUintx)].c_str(), m_vstring[i + j*(m_nWidth / m_nUintx)].size(), &m_reUnit, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
					BitBlt(m_HdcMem, reFresh.left, reFresh.top, m_nUintx, m_nUinty, m_HdcUnit, 0, 0, SRCCOPY);
				}
			}
		BitBlt(hdc, 0, 0, m_nWidth, m_nHeight, m_HdcMem, 0, 0, SRCCOPY);

		EndPaint(m_Hwnd, &ps);

		DeleteObject(hFGBrush);
		DeleteObject(hBKBrush);
		
	}	
		break;
	case WM_MOUSEMOVE:
		POINT pointM;
		pointM.x = LOWORD(lParam);
		pointM.y = HIWORD(lParam);
		if (pointM.x / m_nUintx + pointM.y / m_nUinty*(m_nWidth / m_nUintx)<int(m_vstring.size())&&pointM.x / m_nUintx + pointM.y / m_nUinty*(m_nWidth / m_nUintx) != m_nPos)
		{
			m_nPos = pointM.x / m_nUintx + pointM.y / m_nUinty*(m_nWidth / m_nUintx);
			InvalidateRect(m_Hwnd, &m_reClient, TRUE);
		}
		break;
	case WM_LBUTTONDOWN:
		POINT pointB;
		pointB.x = LOWORD(lParam);
		pointB.y = HIWORD(lParam);
		m_nPos = pointB.x / m_nUintx + pointB.y / m_nUinty*(m_nWidth / m_nUintx);
		if (m_nPos < m_vstring.size())
		{
			ShowWindow(m_Hwnd, SW_HIDE);
			if (m_opr)
				m_opr->SetPriceType(m_vstring[m_nPos].c_str());		
		}
		break;
	case WM_DESTROY:
		DeleteObject(m_hBKBitmap);
		DeleteObject(m_hFGBitmap);
		DeleteDC(m_HdcMem);
		DeleteDC(m_HdcUnit);
		break;
	case WM_ACTIVATE:
		if (WA_INACTIVE == wParam)
			ShowWindow(m_Hwnd, SW_HIDE);
		break;
	case WM_ERASEBKGND:
		break;
	default:                      /* ������Ȥ����Ϣ����DefWindowProc()���� */
		return NOT_PROCESSED;
	}

	return PROCESSED;
}