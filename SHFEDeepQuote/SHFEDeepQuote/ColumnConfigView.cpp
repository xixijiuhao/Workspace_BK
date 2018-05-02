#include "PreInclude.h"
#define closeWidth 9
const int FRAME_H = 200;
const int FRAME_V = 260;
extern SHFEDeepQuoteFrm* g_DeepQuoteFrm;

ColumnConfigView::ColumnConfigView()
{
	m_iSelectedIndex = -1;
	clSequence = new ColumnSequence();
}

LRESULT ColumnConfigView::OnDealMsg(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_SIZE:
		SetWidgetPos();
		return PROCESSED;
	case WM_DRAWITEM:
		OnDrawItem(wParam, lParam);
		return PROCESSED;
	case WM_MEASUREITEM:
		OnMeasureItem(wParam, lParam);
		return PROCESSED;
	case SSWM_STATIC_BUTTON_CLICKDOWN:
		return PROCESSED;
	default:
		break;
	}
	return NOT_PROCESSED;
}

void ColumnConfigView::OnCreate()
{
	CreateWidget();
	InitWidget();
	
	int x = (GetSystemMetrics(SM_CXSCREEN) - 200) / 3;
	int y = (GetSystemMetrics(SM_CYSCREEN) - 260) / 2;
	SetWindowPos(m_Hwnd, HWND_TOP, x, y, FRAME_H, FRAME_V, SWP_SHOWWINDOW);

}

void ColumnConfigView::CreateWidget()
{
	m_listColumn.CreateList(m_Hwnd, LVS_REPORT| LVS_NOSORTHEADER | LVS_OWNERDRAWFIXED | WS_BORDER, 20, 25, LVS_EX_FULLROWSELECT, ID_LISTCOLUMN);

	m_btnUpMove.Create(m_Hwnd, ID_UPMOVE);
	m_btnDownMove.Create(m_Hwnd, ID_DOWNMOVE);
	m_btnSure.Create(m_Hwnd, ID_SUREBUTTON);
	m_btnClose.Create(m_Hwnd, ID_CLOSEBUTTON);
}

void ColumnConfigView::SetWidgetPos()
{
	RECT rect = { 0 };
	GetClientRect(m_Hwnd, &rect);
	int iLeft = 15;
	int iRight = 135;
	int iLeftTop = CAPTION_HEIGHT + 40;
	int iRightTop = CAPTION_HEIGHT + 35;
	int iyVap = 10;
	int ibtnWidth = 40;
	int ibtnHeight = 25;
	POINT pt = { iLeft,iLeftTop };
	rect.left += iLeft;
	rect.top += iLeftTop;
	m_listColumn.MoveWindow(pt.x, pt.y, 80, 130);

	pt = { iRight,iRightTop };

 	m_btnUpMove.MoveWindow(pt.x, pt.y, ibtnWidth, ibtnHeight);
	pt.y += ibtnHeight + iyVap;
	m_btnDownMove.MoveWindow(pt.x, pt.y, ibtnWidth, ibtnHeight);
	pt.y += ibtnHeight + iyVap * 2;
	m_btnSure.MoveWindow(pt.x, pt.y, ibtnWidth, ibtnHeight);
	pt.y += ibtnHeight + iyVap;
	m_btnClose.MoveWindow(pt.x, pt.y, ibtnWidth, ibtnHeight);
}

void ColumnConfigView::InitWidget()
{
	m_listColumn.AddColumn(L"列头", 80);
	*clSequence=g_DeepQuoteFrm->GetColumnSequence();
	char* psText ="买价";
	for (int i = 0; i < 4; i++)
	{
		if (i == 0)
			SetItem(clSequence->FirstColumn, psText);
		else if (i == 1)
			SetItem(clSequence->SecondColumn, psText);
		else if (i == 2)
			SetItem(clSequence->ThirdColumn, psText);
		else
			SetItem(clSequence->FourthColumn, psText);
		m_listColumn.AddItem(psText, i, 0);
	}
	psText = "上移";
	m_btnUpMove.SetButtonText(psText);
	psText = "下移";
	m_btnDownMove.SetButtonText(psText);
	psText = "确定";
	m_btnSure.SetButtonText(psText);
	psText = "关闭";
	m_btnClose.SetButtonText(psText);
}

void ColumnConfigView::SetItem(int sequence, char* &psText)
{
	if (sequence == 0)
		psText = "买价";
	else if (sequence == 1)
		psText = "买量";
	else if (sequence == 2)
		psText = "卖价";
	else if (sequence == 3)
		psText = "卖量";

}

void ColumnConfigView::OnDrawItem(WPARAM wParam, LPARAM lParam)
{
	LPDRAWITEMSTRUCT p = (LPDRAWITEMSTRUCT)lParam;
	m_listColumn.OnDrawItem(lParam);
}

void ColumnConfigView::OnMeasureItem(WPARAM wParam, LPARAM lParam)
{
	LPMEASUREITEMSTRUCT p = (LPMEASUREITEMSTRUCT)lParam;
	m_listColumn.OnMeasureItem(p->itemHeight);
}

LRESULT ColumnConfigView::OnNcDestory(WPARAM wParam, LPARAM lParam)
{
	delete this;
	m_ColumuConfig = NULL;
	return PROCESSED;
}

void ColumnConfigView::OnBtnClick(WPARAM wParam, LPARAM lParam)
{
	LPTSTR m_cSelectedText = new TCHAR[8];
	LPTSTR m_cTempText = new TCHAR[8];
	int up = -1, down = 1;
	switch (lParam)
	{
	case ID_UPMOVE:
		//OnChangeItem(m_listColumn, down);
		if (m_iSelectedIndex < 1)
			break;
		ListView_GetItemText(m_listColumn.GetListHwnd(), m_iSelectedIndex + up, 0, m_cTempText, sizeof(m_cTempText));
		ListView_GetItemText(m_listColumn.GetListHwnd(), m_iSelectedIndex, 0, m_cSelectedText, sizeof(m_cSelectedText));
		ListView_SetItemText(m_listColumn.GetListHwnd(), m_iSelectedIndex, 0, m_cTempText);
		ListView_SetItemText(m_listColumn.GetListHwnd(), m_iSelectedIndex + up, 0, m_cSelectedText);

		ListView_SetItemState(m_listColumn.GetListHwnd(), m_iSelectedIndex + up, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
		ListView_SetItemState(m_listColumn.GetListHwnd(), m_iSelectedIndex, 0, LVIS_FOCUSED | LVIS_SELECTED);
		m_iSelectedIndex += up;
		break;
	case ID_DOWNMOVE:
		//OnChangeItem(m_listColumn, down);
		if (m_iSelectedIndex > 2)
			break;
		ListView_GetItemText(m_listColumn.GetListHwnd(), m_iSelectedIndex + down, 0, m_cTempText, sizeof(m_cTempText));
		ListView_GetItemText(m_listColumn.GetListHwnd(), m_iSelectedIndex, 0, m_cSelectedText, sizeof(m_cSelectedText));
		ListView_SetItemText(m_listColumn.GetListHwnd(), m_iSelectedIndex, 0, m_cTempText);
		ListView_SetItemText(m_listColumn.GetListHwnd(), m_iSelectedIndex + down, 0, m_cSelectedText);

		ListView_SetItemState(m_listColumn.GetListHwnd(), m_iSelectedIndex + down, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
		ListView_SetItemState(m_listColumn.GetListHwnd(), m_iSelectedIndex, 0, LVIS_FOCUSED | LVIS_SELECTED);
		m_iSelectedIndex += down;
		break;
	case ID_SUREBUTTON:
		RefreshColumn();
		SendMessage(m_hParent, WM_COLUMNQUENCE, (WPARAM)clSequence, 0);
		CloseWnd();
		break;
	case ID_CLOSEBUTTON:
		CloseWnd();
		break;
	default:
		break;
	} 
}

LRESULT ColumnConfigView::OnNotify(WPARAM wParam, LPARAM lParam)
{
	LPNMITEMACTIVATE p = (LPNMITEMACTIVATE)lParam;
	if (p->hdr.code == NM_CLICK)
	{
		m_iSelectedIndex = p->iItem;
		return PROCESSED;
	}
	return NOT_PROCESSED;
}

void ColumnConfigView::OnChangeItem(TListView list, int upordown)
{
	LPTSTR m_cSelectedText = new TCHAR[8];
	LPTSTR m_cTempText = new TCHAR[8];
	if ((upordown < 0 && m_iSelectedIndex < 1) || (upordown > 0 && m_iSelectedIndex > 2))
		return;
	ListView_GetItemText(m_listColumn.GetListHwnd(), m_iSelectedIndex + upordown, 0, m_cTempText, sizeof(m_cTempText));
	ListView_GetItemText(m_listColumn.GetListHwnd(), m_iSelectedIndex, 0, m_cSelectedText, sizeof(m_cSelectedText));
	ListView_SetItemText(m_listColumn.GetListHwnd(), m_iSelectedIndex, 0, m_cTempText);
	ListView_SetItemText(m_listColumn.GetListHwnd(), m_iSelectedIndex + upordown, 0, m_cSelectedText);

	ListView_SetItemState(m_listColumn.GetListHwnd(), m_iSelectedIndex + upordown, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
	ListView_SetItemState(m_listColumn.GetListHwnd(), m_iSelectedIndex, 0, LVIS_FOCUSED | LVIS_SELECTED);
	m_iSelectedIndex += upordown;
}

void ColumnConfigView::RefreshColumn()
{
	LPTSTR psText = new TCHAR[8];
	for (int i = 0; i < 4; i++)
	{
			ListView_GetItemText(m_listColumn.GetListHwnd(), i, 0, psText, sizeof(psText));
		if (i == 0)
		{
			if (wcscmp(psText, L"买价") == 0)
				clSequence->FirstColumn = BuyPrice;
			else if(wcscmp(psText, L"买量") == 0)
				clSequence->FirstColumn = BuyNum;
			else if(wcscmp(psText, L"卖价") == 0)
				clSequence->FirstColumn = SellPrice;
			else if(wcscmp(psText, L"卖量") == 0)
				clSequence->FirstColumn = SellNum;
		}
		else if (i == 1)
		{
			if (wcscmp(psText, L"买价") == 0)
				clSequence->SecondColumn = BuyPrice;
			else if (wcscmp(psText, L"买量") == 0)
				clSequence->SecondColumn = BuyNum;
			else if (wcscmp(psText, L"卖价") == 0)
				clSequence->SecondColumn = SellPrice;
			else if (wcscmp(psText, L"卖量") == 0)
				clSequence->SecondColumn = SellNum;
		}
		else if (i == 2)
		{
			if (wcscmp(psText, L"买价") == 0)
				clSequence->ThirdColumn = BuyPrice;
			else if (wcscmp(psText, L"买量") == 0)
				clSequence->ThirdColumn = BuyNum;
			else if (wcscmp(psText, L"卖价") == 0)
				clSequence->ThirdColumn = SellPrice;
			else if (wcscmp(psText, L"卖量") == 0)
				clSequence->ThirdColumn = SellNum;
		}
		else
		{
			if (wcscmp(psText, L"买价") == 0)
				clSequence->FourthColumn = BuyPrice;
			else if (wcscmp(psText, L"买量") == 0)
				clSequence->FourthColumn = BuyNum;
			else if (wcscmp(psText, L"卖价") == 0)
				clSequence->FourthColumn = SellPrice;
			else if (wcscmp(psText, L"卖量") == 0)
				clSequence->FourthColumn = SellNum;
		}		
	}
}