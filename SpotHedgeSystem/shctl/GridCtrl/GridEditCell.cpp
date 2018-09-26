#include "PreInclude.h"
//
TCell		g_clickcell;
TGridEditCell::TGridEditCell(CellEdit* pedit)
{
	m_edit = pedit;
}

TGridEditCell::~TGridEditCell()
{

}

bool TGridEditCell::ClickCell(const RECT& rect, const POINT& pt, HWND hwnd/* = NULL*/)
{
	if (GetMask()&GVIF_INT)
		m_edit->SetEditType(CellEdit::Type_Int);
	else if (GetMask()&GVIF_REAL)
		m_edit->SetEditType(CellEdit::Type_Real);
	else
		m_edit->SetEditType(CellEdit::Type_Common);
	m_edit->SetRect(rect.left, rect.top, _Width(rect), _Height(rect));
	if(!m_edit->GetFocus())
		m_edit->OnSetFocus(0,0);
	g_clickcell = m_pitem->cellid;
	return true;
}

void TGridEditCell::DrawMain(TMemDC* pmemdc, const RECT& rcmain)
{
	bool bfocus;
	bfocus = m_edit->GetFocus();

	__super::DrawMain(pmemdc, rcmain);
	if (bfocus)
	{
		LOGFONT lf = *GetFont();
		lf.lfHeight = 14;
		lf.lfWeight = 400;
		wcsncpy_s(lf.lfFaceName, L"Verdana");
		HFONT hfont = CreateFontIndirect(&lf);
		SelectObject(pmemdc->GetHdc(), hfont);
// 		m_edit->SetFont(TEXT("Verdana"), 14);
		m_edit->OnDraw(pmemdc);
		DeleteObject(hfont);
	}
}

LRESULT TGridEditCell::EditProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	CellEdit* pedit = (CellEdit*)dwRefData;
	switch (uMsg)
	{
	case WM_LBUTTONDOWN:
	{
		POINT pt;
		RECT rect = pedit->GetRect();
		
		pt.x = (int)(short)LOWORD(lParam);
		pt.y = (int)(short)HIWORD(lParam);

		pedit->OnLButtonDown(wParam, lParam);
		if (!PtInRect(&rect, pt)&& pedit->GetFocus())
//			pedit->OnKillFocus(wParam, lParam);
		{
			WPARAM wpm;
			pedit->OnKillFocus(wParam, lParam);
			wpm = MAKEWPARAM(g_clickcell.nrow, g_clickcell.ncol);
			SendMessage(hwnd, SSWM_GRID_MINIEDIT_MODITY, wpm, (LPARAM)pedit);
			g_clickcell.ncol = g_clickcell.nrow = -1;
			return 0;
		}
		
		break;
	}
	case WM_LBUTTONUP:
		pedit->OnLButtonUp(wParam, lParam);
		break;
	case WM_MOUSEMOVE:
		pedit->OnMouseMove(wParam, lParam);
		break;
	case WM_MOUSELEAVE:
		//pedit->OnMouseLeave();
		break;
	case WM_MOUSEWHEEL:
		if(pedit->GetFocus())
		{
			GridEditParam stparam;
			stparam.ncol = g_clickcell.ncol;
			stparam.nrow = g_clickcell.nrow;
			stparam.nsize = (short)HIWORD(wParam);
			stparam.pedit = pedit;
			SendMessage(hwnd, SSWM_GRID_CELLEDIT_WHEEL,/*0*/0, (LPARAM)&stparam);
			return 0;
		}
		break;
	case WM_KEYDOWN:
		if (pedit->GetFocus())
		{
			pedit->OnKeyDown(wParam, lParam);
			if (wParam == VK_RETURN)
			{
				WPARAM wpm;
				pedit->OnKillFocus(wParam, lParam);
				wpm = MAKEWPARAM(g_clickcell.nrow, g_clickcell.ncol);
				SendMessage(hwnd, SSWM_GRID_MINIEDIT_MODITY,/*0*/wpm, (LPARAM)pedit);
				g_clickcell.ncol = g_clickcell.nrow = -1;
			}
			else if (wParam == VK_UP || wParam == VK_DOWN)
				pedit->OnKillFocus(wParam, lParam);
			return 0;
		}
		break;
	case WM_CHAR:
		if (wParam >= '0' && wParam <= '9' || wParam == VK_RETURN
			|| wParam == VK_DELETE || wParam == '.'
			|| wParam == VK_BACK)
		{
			if (wParam == '.')
			{
				wchar_t wsztext[51];
				pedit->GetText(0, wsztext);//pedit->GetText(wsztext, 51);
				if (wcsstr(wsztext, L".") != NULL)
					break;
			}
			pedit->OnChar(wParam, lParam);
		}
		else if (wParam == '-'&&pedit->GetEditType() == CellEdit::Type_Real)
		{
			wchar_t szpre[MAX_PATH] = { 0 };
			wchar_t szcur[MAX_PATH] = { 0 };
			pedit->GetText(0, szpre);
			pedit->OnChar(wParam, lParam);
			pedit->GetText(0, szcur);
			if (!CConvert::IsValidNumber(szcur, false, true))
			{
				pedit->SetText(0, szpre);
				break;
			}
		}
		else if (wParam == VK_ESCAPE)
		{
			pedit->OnKillFocus(wParam, lParam);
			g_clickcell.ncol = g_clickcell.nrow = -1;
		}
		break;
		//////////////////////////////////////////////////////////////////////////
	case WM_SETFOCUS:
		//pedit->OnSetFocus(wParam, lParam);
		break;
	case WM_KILLFOCUS:
		pedit->OnKillFocus(wParam, lParam);
		g_clickcell.ncol = g_clickcell.nrow = -1;
		InvalidateRect(hwnd, 0, FALSE);
		break;
	case WM_LBUTTONDBLCLK:
		{
			POINT pt;
			RECT rect = pedit->GetRect();

			pt.x = (int)(short)LOWORD(lParam);
			pt.y = (int)(short)HIWORD(lParam);
			if (PtInRect(&rect, pt) && pedit->GetFocus())
			{
				pedit->OnDblClick(wParam, lParam);
				return 0;
			}
			break;
		}
	}
	LRESULT  lret = CallWindowProc(pedit->m_oldProc, hwnd, uMsg, wParam, lParam);
	return lret;
}