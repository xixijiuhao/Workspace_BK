#include "..\PreInclude.h"
//色彩
extern G_COLOREF g_ColorRefData;
extern G_UIFONT g_FontData;
using namespace std;

extern ITradeApi *g_pTradeApi;
extern ITradeData *g_pTradeData;
//extern IQuoteApi *g_QuoteApi;
extern IStarApi *g_pStarApi;

#define MAX_SIZE	250

string GetSpreadKeyTrade(const SSpreadContract& tSpread)
{
	char cKey[103] = { 0 };
	string ccode1 = "";
	string ccode2 = "";
	string scode1 = tSpread.Contract1->ContractNo;
	string scode2 = tSpread.Contract2->ContractNo;
	CConvert::QContractNoTypeToStr(scode1, ccode1);
	CConvert::QContractNoTypeToStr(scode2, ccode2);
	sprintf_s(cKey, "%s%c%s", ccode1.c_str(), tSpread.RateMode, ccode2.c_str());
	return cKey;
}
LRESULT CALLBACK ListViewSpreadProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int id = GetWindowLong(hwnd, GWL_ID);
	CPopWinCodeQuote *pWinCode = (CPopWinCodeQuote*)GetWindowLong(hwnd, GWL_USERDATA);
	if (!pWinCode)
		return false;
	switch (message)
	{
		case WM_MOUSEMOVE:
		{
			POINT pointB;
			pointB.x = LOWORD(lParam);
			pointB.y = HIWORD(lParam);
			LVHITTESTINFO temp;
			temp.pt = pointB;
			temp.flags = LVHT_ABOVE;
			ListView_SubItemHitTest(hwnd, &temp);

			if (pWinCode)
			{
				//位置变动刷新
				if (pWinCode->m_sel_row != temp.iItem)
				{
					RECT r;
					//原位置刷新
					
					ListView_GetSubItemRect(hwnd, pWinCode->m_sel_row, 1, LVIR_BOUNDS, &r);
					InvalidateRect(hwnd, &r, TRUE);
					ListView_GetSubItemRect(hwnd, pWinCode->m_sel_row, 2, LVIR_BOUNDS, &r);
					InvalidateRect(hwnd, &r, TRUE);
					
					pWinCode->m_sel_row = temp.iItem;
				
					ListView_GetSubItemRect(hwnd, pWinCode->m_sel_row, 1, LVIR_BOUNDS, &r);
					InvalidateRect(hwnd, &r, TRUE);
					ListView_GetSubItemRect(hwnd, pWinCode->m_sel_row, 2, LVIR_BOUNDS, &r);
					InvalidateRect(hwnd, &r, TRUE);
					
						
				}
			}
		}
		break;
		case WM_NOTIFY:
		{
			NMHDR *pNMHDR = (NMHDR *)lParam;
			LPNMLISTVIEW pNMListView = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
			switch (pNMListView->hdr.code)
			{
			case HDN_BEGINTRACKW:
			case HDN_BEGINTRACKA:
				return true;
			}
		}
		break;
	}
	
	return CallWindowProc(pWinCode->m_ListViewProc, hwnd, message, wParam, lParam);
	
}

CPopWinCodeQuote::CPopWinCodeQuote() 
: m_opr(NULL)
, m_nWidth(0)
, m_nHeight(0)
, m_sel_row(-1)
{
	
}

CPopWinCodeQuote::~CPopWinCodeQuote()
{
	if (IsWindow(m_Hwnd))
		DestroyWindow(m_Hwnd);
}

void CPopWinCodeQuote::CreatePopWin(IOperator* opr,LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle, int nleft, int ntop, UINT nWidth, UINT nrow)
{
	m_nWidth = nWidth;
	m_nRow = nrow;
	m_nLeft = nleft;
	m_nTop = ntop;
	m_opr  = opr;
	CreateFrm(classname, parent, style | WS_POPUP | WS_BORDER, exstyle);
}
bool CPopWinCodeQuote::UpdateData()
{
	ListView_DeleteAllItems(m_hListView);//清行
	while (ListView_DeleteColumn(m_hListView, 0));//清列
	LV_COLUMN LvCol;
	memset(&LvCol, 0, sizeof(LvCol));							// Zero Members
	LvCol.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;				// Type of mask
	LvCol.fmt = LVCFMT_CENTER;

	LvCol.pszText = L"";
	LvCol.cx = 0;
	ListView_InsertColumn(m_hListView, 0, &LvCol);
	LvCol.pszText = L"";
	LvCol.cx = 250;
	ListView_InsertColumn(m_hListView, 1, &LvCol);
	LvCol.pszText = L"";
	LvCol.cx = 80;
	ListView_InsertColumn(m_hListView, 2, &LvCol);

	int nCount = g_pStarApi->GetSpreadInfoCount();
	SSpreadContract tSpread[MAX_SIZE] = { 0 };
	SSpreadContract s;
	memset(&s, 0, sizeof(SSpreadContract));

	int i = 0;
	wchar_t cRatio[51] = { 0 };
	LV_ITEM ltm;
	ltm.mask = LVIF_TEXT;
	ltm.pszText = L"";
	ltm.iItem = i;
	ltm.iSubItem = 0;
	ListView_InsertItem(m_hListView, &ltm);
	swprintf_s(cRatio, LoadResWchar_t(IDS_SprContract));
	ListView_SetItemText(m_hListView, i, 1, cRatio);
	swprintf_s(cRatio, L" + ");
	ListView_SetItemText(m_hListView, i, 2, cRatio);
	for (int n = 0; n < nCount / MAX_SIZE + 1; n++)
	{
		int nret = g_pStarApi->GetSpreadInfo(s.Contract1 ? s.Contract1->ContractNo : "", s.Contract2?s.Contract2->ContractNo:"", s.RateMode, tSpread, MAX_SIZE, true);
		if (nret > 0)
		{
			for (i = 0; i < nret; i++)
			{
				SSpreadContract *p = (SSpreadContract *)&tSpread[i];
				{
					string scode = GetSpreadKeyTrade(*p);
					wchar_t cRatio[51] = { 0 };

					LV_ITEM ltm;
					ltm.mask = LVIF_TEXT;
					ltm.pszText = L"";
					ltm.iItem = i+1;
					ltm.iSubItem = 0;
					ListView_InsertItem(m_hListView, &ltm);
					swprintf_s(cRatio, L"%s", LoadRC::ansi_to_unicode(scode).c_str());
					ListView_SetItemText(m_hListView, i+1, 1, cRatio);
					swprintf_s(cRatio, L"%llu : %llu", p->SpreadQty1, p->SpreadQty2);
					ListView_SetItemText(m_hListView, i+1, 2, cRatio);
				}
			}
			memcpy(&s, &tSpread[nret - 1], sizeof(SSpreadContract));
		}
		else
			break;
	}
	return true;
}
void CPopWinCodeQuote::SelectCode()
{
	ShowWindow(m_Hwnd, SW_HIDE);

	if (m_opr)
		m_opr->SetContractNo(m_sCode, cstChange | cstSelect | cstQuote);
}

LRESULT CPopWinCodeQuote::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC			hdc;
	switch (message)
	{
	case WM_CREATE:
		{
			hdc = GetDC(m_Hwnd);
			
			ReleaseDC(m_Hwnd, hdc);
			m_nHeight = 10*N_CODE_HEIGHT + 1;

			//////////////////////////////////////////////////////////////////////////ListView
			m_hListView = CreateWindowEx(0, WC_LISTVIEW,
				TEXT("List view"), WS_VISIBLE | WS_CHILD | LVS_REPORT,
				0, 0, m_nWidth, m_nHeight,
				m_Hwnd, (HMENU)ID_LIST_VIEW, GetModuleHandle(NULL), NULL);
			m_ListViewProc = (WNDPROC)SetWindowLong(m_hListView, GWL_WNDPROC, (LONG)ListViewSpreadProc);
			SetWindowLong(m_hListView, GWL_USERDATA, LONG(this));
			
			ListView_SetExtendedListViewStyleEx(m_hListView, NULL, LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

			SetWindowLong(m_hListView, GWL_STYLE, GetWindowLong(m_hListView, GWL_STYLE) | LVS_NOCOLUMNHEADER);
			//控制行高
			HIMAGELIST kk = ImageList_Create(1, 24, ILC_COLOR32, 1, 1);
			ListView_SetImageList(m_hListView, kk, LVSIL_SMALL);
			
			UpdateData();
			SendMessage(m_hListView, WM_SETFONT, (WPARAM)g_FontData.GetFontNum15(), (LPARAM)TRUE);
			SetWindowPos(m_Hwnd, NULL, m_nLeft, m_nTop, m_nWidth, m_nHeight, SWP_SHOWWINDOW);
		}
		break;
	case WM_NOTIFY:
		if ((int)wParam == ID_LIST_VIEW)
		{
			NMHDR *pNMHDR = (NMHDR *)lParam;
			LPNMLISTVIEW pNMListView = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
			switch (pNMHDR->code)
			{
				case NM_CLICK:
				{
					LPNMITEMACTIVATE lpnmitem = (LPNMITEMACTIVATE)lParam;
					wchar_t wtext[51] = { 0 };
					LVITEM lvitem;
					lvitem.mask = LVIF_TEXT;
					lvitem.iItem = lpnmitem->iItem;
					lvitem.iSubItem = 1/*lpnmitem->iSubItem*/;
					lvitem.cchTextMax = sizeof(wtext) / sizeof(wtext[0]);
					lvitem.pszText = wtext;
					ListView_GetItem(m_hListView, &lvitem);			
					
					memcpy(m_sCode, LoadRC::unicode_to_ansi(wtext).c_str(), sizeof(m_sCode));
					SelectCode();
							
				}
				break;
				case NM_CUSTOMDRAW:
				{
					LPNMLVCUSTOMDRAW lpDraw = (LPNMLVCUSTOMDRAW)lParam;
					{
						RECT itemRect;
						HDC dc;
						switch (lpDraw->nmcd.dwDrawStage)
						{
						case CDDS_PREPAINT:
							return CDRF_NOTIFYITEMDRAW;
						case CDDS_ITEMPREPAINT:
							return CDRF_NOTIFYSUBITEMDRAW;
						case CDDS_SUBITEM | CDDS_ITEMPREPAINT:
							dc = lpDraw->nmcd.hdc;
							itemRect = lpDraw->nmcd.rc;
							lpDraw->clrText = g_ColorRefData.GetColorBlack();
							lpDraw->clrTextBk = g_ColorRefData.GetColorWhite();
							if (lpDraw->nmcd.dwItemSpec == m_sel_row)
							{
								lpDraw->clrTextBk = g_ColorRefData.GetColorBlue();
								lpDraw->clrText = g_ColorRefData.GetColorWhite();
							}
							return CDRF_DODEFAULT;
						}
					}
				}
				break;
			}
		}
		break;
	case WM_ACTIVATE:
		if (WA_INACTIVE == LOWORD(wParam))
			ShowWindow(m_Hwnd, SW_HIDE);
		break;
	
	case WM_ERASEBKGND:
		break;
	case WM_DESTROY:
		break;
	default:                      /* 不感兴趣的消息交给DefWindowProc()处理 */
		return NOT_PROCESSED;
	}

	return PROCESSED;
}