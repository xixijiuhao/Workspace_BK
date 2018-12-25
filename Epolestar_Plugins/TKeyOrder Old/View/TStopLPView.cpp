#include "BaseInclude.h"
extern LanguageID g_language;

//@资源
static char szCHS[][75] = {
	"设置止损单", "设置止盈单", "设置保本单", "新建止损单", "新建止盈单",
	"新建保本单", "多头 ", "空头 ", "已暂停 ", "正在运行 ",
	"开仓均价：", "止损", "止盈", "保本", "合约",
	"浮动止损，跟踪回撤", "启动", "暂停", "触发价", "手数", 
	"浮动点数", "止损止盈等策略单为本地触发，关闭客户端即失效；\n策略单均为最新价触发。", "止损价", "止盈价",  "保本价差",
	"止损价格不合时宜", "价差", "元 预计亏损", "元 百分比", "止盈价格不合时宜", 
	"元 预计盈利", "预计盈利", 
};

static char szCHT[][75] = {
	"設置止損單", "設置止盈單", "設置保本單", "新建止損單", "新建止盈單",
	"新建保本單", "多頭 ", "空頭 ", "已暫停 ", "正在運行 ",
	"開倉均價：", "止損", "止盈", "保本", "合約",
	"浮動止損，跟蹤回撤", "啟動", "暫停", "觸發價", "手數", 
	"浮動點數", "止損止盈等策略單為本地觸發，關閉客戶端即失效；\n策略單均為最新價觸發。", "止損價", "止盈價", "保本價差", 
	"止損價格不合時宜", "價差", "元 預計虧損", "元 百分比", "止盈價格不合時宜", 
	"元 預計盈利", "預計盈利",
};

static char szEng[][100] = {
	"StopLoss", "StopProfit", "BreakEven", "new StopLoss order", "new StopProfit order",
	"new BreakEven order", "Long ", "Short ", "Stop ", "Running ",
	"OpenPriceAvg:", "StopLoss", "StopProfit", "BreakEven", "ContractNo",
	"Floating StopLoss", "Start", "Pause", "Trigger Price",
	"Volume", "FloatPoint", "Strategy Orders are Invalidated when shutting down EpoleStar 9.3;\nOrders are Trggered at LastPrice.", "StopLoss price ", "StopProfit price ",
	"BreakEven spread ", "StopLoss price is not appropriate", "spread ", " Yuan Estimated deficit ", " Yuan Percentage ",
	"StopProfit price is not appropriate", " Yuan Estimated profit ", " Estimated profit ",
};

static char* _GetString_StopLPView(int i)
{
	if (g_language == ENU) return szEng[i];
	else if (g_language == CHT) return szCHT[i];
	return szCHS[i];
}


TStopLPView::TStopLPView(IMediator* md) :IColleague(md)
{
	m_rectClose = { 0 };
	m_rectMini = { 0 };
	m_colorCaption = RGB(150, 140, 140);
	m_colorBk = g_ColorRefData.GetColorBackground();
	Wnd_Width = 0;
	Wnd_Height = 0;
	m_bhoverclose = false;
	m_bhovermini = false;
	m_bmousetrack = false;
}

void TStopLPView::Create(HWND hParent,int PageID)
{
	CreateFrm(L"Stop_LP_View", hParent, WS_POPUP | WS_VISIBLE | WS_CLIPCHILDREN | WS_SYSMENU, WS_EX_TOOLWINDOW | WS_EX_CONTROLPARENT);
	POINT pt;
	GetCursorPos(&pt);
	MoveWindow(pt.x, pt.y, FRAME_WIDTH, FRAME_HEIGHT);
	SwitchPage(PageID);
}

void TStopLPView::SwitchPage(int PageID)
{
	CxCombox* pCombox = &m_cbStopLoss;
	switch (PageID)
	{
	case Page_StopLoss:
	{
		PageOneSH(true);
		PageTwoSH(false);
		PageThreeSH(false);
		SetTitle(_GetString_StopLPView(0));
		m_iSelPage = Page_StopLoss;
		break;
	}
	case Page_StopProfit:
	{
		PageOneSH(false);
		PageTwoSH(true);
		PageThreeSH(false);
		SetTitle(_GetString_StopLPView(1));
		m_iSelPage = Page_StopProfit;
		pCombox = &m_cbStopProfit;
		break;
	}
	case Page_BreakEven:
	{
		PageOneSH(false);
		PageTwoSH(false);
		PageThreeSH(true);
		SetTitle(_GetString_StopLPView(2));
		m_iSelPage = Page_BreakEven;
		pCombox = &m_cbBreakEven;
		break;
	}
	default:
		break;
	}
	UpdateViewWidget_CBChange(pCombox->GetSelect(), Page2OrderLPType(m_iSelPage));
	if (m_pData)
	{
		m_pData->iSelPage = m_iSelPage;
	}
	RefreshReminder();
}

LRESULT TStopLPView::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		OnCreate();
		return PROCESSED;
	case WM_PAINT:
		OnPaint();
		return PROCESSED;
	case WM_MOUSEMOVE:
		OnMouseMove(wParam, lParam);
		return PROCESSED;
	case WM_MOUSELEAVE:
		OnMouseLeave();
		return PROCESSED;
	case WM_LBUTTONDOWN:
		OnLButtonDown(wParam, lParam);
		return PROCESSED;
	case WM_LBUTTONUP:
		OnLButtonUp(wParam, lParam);
		return PROCESSED;
	case WM_CTLCOLORSTATIC:
		return OnCtrlColor(wParam, lParam);
	case SSWM_STATIC_BUTTON_CLICKDOWN:
		OnStaticBtnClick(wParam, lParam);
		return PROCESSED;
	case WM_NOTIFY:
		OnNotify(wParam, lParam);
		return PROCESSED;
	case WM_COMMAND:
		OnCommand(wParam, lParam);
		return PROCESSED;
	case SSWM_CLICK_CHECKBOX:
		OnCheckBox(lParam);
	default:
		return NOT_PROCESSED;
	}
}

void TStopLPView::MoveWindow(int x, int y, int cx, int cy)
{
	Wnd_Width = cx;
	Wnd_Height = cy;
	SetWindowPos(m_Hwnd, HWND_TOP, x, y, cx, cy, SWP_NOZORDER);
	InvalidateRect(m_Hwnd, NULL, true);
}

void TStopLPView::GetData()
{
	m_Mediator->Get(CMD_StopLPView_GetData, (void *&)m_pData, this);
}

void TStopLPView::GetComboxData()
{
	m_Mediator->Get(CMD_StopLPView_GetComboxData, (void *&)m_pComboxData, this);
}

void TStopLPView::SetData(int Sel,bool notMOD)
{

	//将该合约的止损单显示在ComboxList里
	GetData();
	GetComboxData();
	SetComboxList();

	
	CConvert::QContractNoTypeToStr(m_pData->RawContractID, m_StrCode);
	m_scContract.SetText(m_StrCode.c_str());


	int index[3] = { 0 };
	if (notMOD)
	{
		SwitchPage(Sel);
	}
	else
	{
		int cnt = m_pComboxData->size();
		int veccnt[3] = { 0 };
		if (cnt == 0)
		{
			return;
		}
		for (int i = 0; i < cnt; i++)
		{
			const TStopLPitem* pitem = (const TStopLPitem*)m_pComboxData->at(i);
			switch (pitem->orderType)
			{
			case otLoss:
			case otFloat:
				veccnt[0]++;
				if (pitem->keyID == Sel)
				{
					index[0] = veccnt[0];
					i = cnt - 1;//跳出循环
				}		
				break;
			case otProfit:
				veccnt[1]++;
				if (pitem->keyID == Sel)
				{
					index[1] = veccnt[1];
					i = cnt - 1;
				}
				break;
			case otBreakEven:
				veccnt[2]++;
				if (pitem->keyID == Sel)
				{
					index[2] = veccnt[2];
					i = cnt - 1;
				}
				break;
			default:
				break;
			}
			SwitchPage(OrderLPType2Page(pitem->orderType));

		}
	}
	SelComboxItem(index[0], index[1], index[2]);

	POINT pt;
	GetCursorPos(&pt);
	int top = pt.y - FRAME_HEIGHT / 2;
	top = top < 0 ? 0 : top;
	MoveWindow(pt.x - FRAME_WIDTH / 2, top, FRAME_WIDTH, FRAME_HEIGHT);
	ShowWindow(m_Hwnd, SW_SHOW);


}


void TStopLPView::UpdatePositionPrice()
{
	m_Mediator->Send(CMD_StopLPView_UpdatePositionPrice, nullptr, this);
}

void TStopLPView::ComboxUpdate(const TStopLPitem* pItem)
{
	if (nullptr == pItem)
	{
		SetComboxList();
		for (int cnt = 0; cnt <m_vecStopLossItem.size() ; cnt++)
		{
			if (m_vecStopLossItem[cnt]->keyID == m_DealingKeyID_StopL)
			{
				m_cbStopLoss.SetSelect(cnt + 1);
				UpdateViewWidget_CBChange(cnt + 1, otLoss);
				break;
			}
		}
		return;
	}
	int iDealingKeyID;
	CxCombox* cbPointer;
	vector<const TStopLPitem*>*	vecPointer;
	OrderLPType type;

	switch (pItem->orderType)
	{
	case otLoss:
	case otFloat:
	{
		iDealingKeyID = m_DealingKeyID_StopL;
		cbPointer = &m_cbStopLoss;
		vecPointer = &m_vecStopLossItem;
		type = otLoss;
		break;
	}
	case otProfit:
	{
		iDealingKeyID = m_DealingKeyID_StopP;
		cbPointer = &m_cbStopProfit;
		vecPointer = &m_vecStopProfitItem;
		type = otProfit;
		break;
	}
	case otBreakEven:
	{
		iDealingKeyID = m_DealingKeyID_BreakE;
		cbPointer = &m_cbBreakEven;
		vecPointer = &m_vecBreakEvenItem;
		type = otBreakEven;
		break;
	}
	default:
	{
		iDealingKeyID = m_DealingKeyID_StopL;
		cbPointer = &m_cbStopLoss;
		vecPointer = &m_vecStopLossItem;
		type = otLoss;
		break;
	}
	}

	if (osDeleted == pItem->state || osTrig == pItem->state)//Delete Item
	{
		if (pItem->keyID == iDealingKeyID)
		{
			cbPointer->SetSelect(0);
			UpdateViewWidget_CBChange(cbPointer->GetSelect(), type);
		}
		int cnt = 1;
		for (auto iter = vecPointer->begin(); iter != vecPointer->end(); iter++, cnt++)
		{
			if ((*iter)->keyID == pItem->keyID)
			{
				vecPointer->erase(iter);
				int i;
				m_pComboxData->FindItem(pItem, i);
				m_pComboxData->Delete(i);
				break;
			}
		}
		cbPointer->DeleteString(cnt);//删掉一行后，Selindex会不会变？

	}
	else if (m_vecStopLossItem.size() + m_vecStopProfitItem.size() + m_vecBreakEvenItem.size() < m_pComboxData->size())//Add Item
	{
		vecPointer->push_back(pItem);
		wstring str;
		FormComboxItemString(str, pItem);
		cbPointer->AddString(str.c_str());
		if (iDealingKeyID == -1)
		{
			cbPointer->SetSelect(str.c_str());
			UpdateViewWidget_CBChange(cbPointer->GetSelect(), type);
		}
	}
	else//Mod Item
	{
		wstring str;
		for (int cnt = 0; cnt < vecPointer->size(); cnt++)
		{
			if ((*vecPointer)[cnt]->keyID == pItem->keyID)
			{
				if (cbPointer->DeleteString(cnt + 1) != CB_ERR)
				{
					FormComboxItemString(str, pItem);
					cbPointer->AddString(str.c_str(), cnt + 1);
					(*vecPointer)[cnt] = pItem;//替换
					break;
				}
			}
		}
		if (pItem->keyID == iDealingKeyID)
		{
			cbPointer->SetSelect(str.c_str());
			UpdateViewWidget_CBChange(cbPointer->GetSelect(), type);
		}
	}
}


void TStopLPView::SetComboxList()
{
	//清空，初始化
	m_vecStopLossItem.clear();
	m_vecStopProfitItem.clear();
	m_vecBreakEvenItem.clear();
	m_cbStopLoss.Clear();
	m_cbStopProfit.Clear();
	m_cbBreakEven.Clear();
	m_cbStopLoss.AddString(ANSIToUnicode(_GetString_StopLPView(3)).c_str());
	m_cbStopLoss.SetSelect(0);
	m_cbStopProfit.AddString(ANSIToUnicode(_GetString_StopLPView(4)).c_str());
	m_cbStopProfit.SetSelect(0);
	m_cbBreakEven.AddString(ANSIToUnicode(_GetString_StopLPView(5)).c_str());
	m_cbBreakEven.SetSelect(0);


	int cnt = m_pComboxData->size();
	if (cnt == 0)
	{
		return;
	}
	for (int i = 0; i < cnt; i++)
	{
		const TStopLPitem* pitem = (const TStopLPitem*)m_pComboxData->at(i);
		if (pitem == nullptr)
		{
			continue;
		}
		if (pitem->state == osTrig || pitem->state == osDeleted)
		{
			continue;
		}
		AddCombox(pitem);
	}
}

void TStopLPView::AddCombox(const TStopLPitem* pitem)
{
	wstring string;
	FormComboxItemString(string,pitem);


	
	switch (pitem->orderType)
	{
	case otLoss:
	case otFloat:
		m_cbStopLoss.AddString(string.c_str());
		m_vecStopLossItem.push_back(pitem);
		break;
	case otProfit:
		m_cbStopProfit.AddString(string.c_str());
		m_vecStopProfitItem.push_back(pitem);
		break;
	case otBreakEven:
		m_cbBreakEven.AddString(string.c_str());
		m_vecBreakEvenItem.push_back(pitem);
		break;
	default:
		break;
	}
}

void TStopLPView::FormComboxItemString(wstring& str, const TStopLPitem* pitem)
{
	if (pitem->direct == dSell)
	{
		str += ANSIToUnicode(_GetString_StopLPView(6));
	}
	else
	{
		str += ANSIToUnicode(_GetString_StopLPView(7));
	}

	if (pitem->state == osSuspend)
	{
		str += ANSIToUnicode(_GetString_StopLPView(8));
	}
	else
	{
		str += ANSIToUnicode(_GetString_StopLPView(9));
	}
	str = str + ANSIToUnicode(_GetString_StopLPView(10));

	char temp[26];
	//_gcvt_s(temp, sizeof(temp), pitem->openPrice, 7);//这个末尾好像不会补零
	sprintf_s(temp, "%.2lf ", pitem->openPrice);
	wchar_t tempw[26];
	mbstowcs_s(nullptr, tempw, sizeof(tempw) / 2, temp, sizeof(temp));
	str += tempw;

	str += L"ID:";
	sprintf_s(temp, "%u", pitem->keyID);
	mbstowcs_s(nullptr, tempw, sizeof(tempw) / 2, temp, sizeof(temp));
	str += tempw;
}

void TStopLPView::SelComboxItem(int index1,int index2,int index3)
{
	m_cbStopLoss.SetSelect(index1);
	m_cbStopProfit.SetSelect(index2);
	m_cbBreakEven.SetSelect(index3);
	int index;
	switch (m_iSelPage)
	{
	case Page_StopLoss:
		index = index1;
		break;
	case Page_StopProfit:
		index = index2;
		break;
	case Page_BreakEven:
		index = index3;
		break;
	}
	UpdateViewWidget_CBChange(index, Page2OrderLPType(m_iSelPage));
}


void TStopLPView::OnPaint()
{
	TMemDC memdc(m_Hwnd);
	int iSavaDc = SaveDC(memdc.GetHdc());
	HBRUSH hbrCaption = CreateSolidBrush(m_colorCaption);
	HBRUSH hbrFrameBk = CreateSolidBrush(m_colorBk);
	HBRUSH hbrRed = CreateSolidBrush(RGB(201, 51, 39));
	HPEN   penRed = CreatePen(PS_SOLID, 1, RGB(201, 51, 39));
	HPEN   penBlack = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
	HPEN   penWhite = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
	SetBkMode(memdc.GetHdc(), TRANSPARENT);
	RECT rect;
	GetClientRect(m_Hwnd, &rect);



	RECT rc = rect;
	rc.top = CAPTION_HEIGHT; rc.bottom = rect.bottom;//背景
	FillRect(memdc.GetHdc(), &rc, hbrFrameBk);
	rc = rect;
	FrameRect(memdc.GetHdc(), &rc, hbrCaption);
	InflateRect(&rc, -1, -1);
	FrameRect(memdc.GetHdc(), &rc, hbrCaption);

	rc = rect;
	rc.bottom = CAPTION_HEIGHT;
	FillRect(memdc.GetHdc(), &rc, hbrCaption);//标题


	//写标题
	SetTextColor(memdc.GetHdc(), RGB(240, 240, 240));
	SelectObject(memdc.GetHdc(), g_FontData.GetFontWord13());
	rc.left = 8; rc.right = Wnd_Width; rc.top = rect.top;  rc.bottom = rect.top + CAPTION_HEIGHT;
	DrawText(memdc.GetHdc(), ANSIToUnicode(m_TitleName).c_str(), -1, &rc, DT_VCENTER | DT_CENTER | DT_SINGLELINE);

	m_rectClose = rect;
	m_rectClose.bottom = CAPTION_HEIGHT; m_rectClose.left = m_rectClose.right - CAPTION_HEIGHT;
	m_rectMini.left = m_rectClose.left + 10; m_rectMini.right = m_rectClose.right - 10;
	m_rectMini.top = m_rectClose.top + 10; m_rectMini.bottom = m_rectClose.bottom - 10;

	//写切换按钮
	SetTextColor(memdc.GetHdc(), RGB(10, 10, 10));
	SelectObject(memdc.GetHdc(), g_FontData.GetFontWord12());
	HBRUSH btnFramehbr = CreateSolidBrush(RGB(200, 200, 200));
	for (int i = 0; i < 3; i++)
	{
		FrameRect(memdc.GetHdc(), &m_rectChoice[i], btnFramehbr);
		if ((i + 1) != m_iSelPage)
		{
			FillRect(memdc.GetHdc(), &m_rectChoice[i], btnFramehbr);
		}
	}
	DeleteObject(btnFramehbr);
	DrawText(memdc.GetHdc(),LoadRC::ansi_to_unicode(_GetString_StopLPView(11)).c_str(), -1, &m_rectChoice[0], DT_VCENTER | DT_CENTER | DT_SINGLELINE);
	DrawText(memdc.GetHdc(), LoadRC::ansi_to_unicode(_GetString_StopLPView(12)).c_str(), -1, &m_rectChoice[1], DT_VCENTER | DT_CENTER | DT_SINGLELINE);
	DrawText(memdc.GetHdc(), LoadRC::ansi_to_unicode(_GetString_StopLPView(13)).c_str(), -1, &m_rectChoice[2], DT_VCENTER | DT_CENTER | DT_SINGLELINE);


	//画关闭按钮
	if (m_bhoverclose)
	{
		rc = m_rectClose; rc.bottom = m_rectClose.bottom;
		SelectObject(memdc.GetHdc(), penRed);
		FillRect(memdc.GetHdc(), &rc, hbrRed);
		SelectObject(memdc.GetHdc(), penWhite);
	}
	else
	{
		SelectObject(memdc.GetHdc(), penWhite);
	}

	rc = m_rectMini;

	MoveToEx(memdc.GetHdc(), rc.left, rc.top, 0);
	LineTo(memdc.GetHdc(), rc.right + 1, rc.bottom + 1);
	MoveToEx(memdc.GetHdc(), rc.right, rc.top, 0);
	LineTo(memdc.GetHdc(), rc.left - 1, rc.bottom + 1);


	SelectObject(memdc.GetHdc(), penBlack);

	RestoreDC(memdc.GetHdc(), iSavaDc);

	DeleteObject(hbrCaption);
	DeleteObject(hbrFrameBk);
	DeleteObject(hbrRed);
	DeleteObject(penRed);
	DeleteObject(penBlack);
	DeleteObject(penWhite);
}

void TStopLPView::OnCreate()
{
	CreateWidget();
	InitWidget();
	SetWidetPos();


}

void TStopLPView::CreateWidget()
{
	//m_scContract.Create(m_Hwnd, ANSIToUnicode(_GetString_StopLPView(14)).c_str());
	m_scContract.Create(m_Hwnd, L"", SS_RIGHT|SS_CENTERIMAGE);

	m_ckFloat.Create(m_Hwnd, 1, ANSIToUnicode(_GetString_StopLPView(15)).c_str());

	m_spinSLPrice.Create(m_Hwnd, 0, ID_editSLPrice, ID_spinSLPrice, CxEdit::floatStyle, false);

	m_spinSLQty.Create(m_Hwnd, 0, ID_editSLQty, ID_spinSLQty, CxEdit::intStyle, false);

	m_spinSPPrice.Create(m_Hwnd, 0, ID_editSPPrice, ID_spinSPPrice, CxEdit::floatStyle, false);

	m_spinSPQty.Create(m_Hwnd, 0, ID_editSPQty, ID_spinSPQty, CxEdit::intStyle, false);

	m_spinBEPrice.Create(m_Hwnd, 0, ID_editBEPrice, ID_spinBEPrice, CxEdit::floatStyle, false);

	m_spinBEQty.Create(m_Hwnd, 0, ID_editBEQty, ID_spinBEQty, CxEdit::intStyle, false);

	m_spinFloatPrice.Create(m_Hwnd, 0, ID_editFloatPrice, ID_spinFloatPrice, CxEdit::floatStyle, false);

	m_cbStopLoss.Create(m_Hwnd);
	m_cbStopProfit.Create(m_Hwnd, 1);
	m_cbBreakEven.Create(m_Hwnd, 2);

	m_btnStart.Create(m_Hwnd, _GetString_StopLPView(16), 0);
	m_btnPause.Create(m_Hwnd, _GetString_StopLPView(17), 1);

	m_scPrice.Create(m_Hwnd, ANSIToUnicode(_GetString_StopLPView(18)).c_str());
	m_scQty.Create(m_Hwnd, ANSIToUnicode(_GetString_StopLPView(19)).c_str());
	m_scFloatPrice.Create(m_Hwnd, ANSIToUnicode(_GetString_StopLPView(20)).c_str());

	m_scReMinder.Create(m_Hwnd, L"");
	m_scInfo.Create(m_Hwnd, ANSIToUnicode(_GetString_StopLPView(21)).c_str(), SS_LEFT);
}

void TStopLPView::InitWidget()
{
	m_scContract.SetFont(g_FontData.g_FontWord13);
	m_scPrice.SetFont(g_FontData.g_FontWord12);
	m_scQty.SetFont(g_FontData.g_FontWord12);
	m_scFloatPrice.SetFont(g_FontData.g_FontWord12);
	m_scReMinder.SetFont(g_FontData.g_FontWord12);
	m_scInfo.SetFont(g_FontData.g_FontWord12);

	m_ckFloat.SetFont(g_FontData.g_FontWord12);
	m_ckFloat.SetBkColor(g_ColorRefData.GetColorBackground());

	m_spinSLQty.SetFont(g_FontData.g_FontNum15);
	m_spinSLQty.SetWindowNumber(1);

	m_spinSPQty.SetFont(g_FontData.g_FontNum15);
	m_spinSPQty.SetWindowNumber(1);

	m_spinBEQty.SetFont(g_FontData.g_FontNum15);
	m_spinBEQty.SetWindowNumber(1);
	
	m_spinSLPrice.SetFont(g_FontData.g_FontNum15);
	m_spinSLPrice.SetWindowNumber(0);

	m_spinSPPrice.SetFont(g_FontData.g_FontNum15);
	m_spinSPPrice.SetWindowNumber(0);

	m_spinFloatPrice.SetFont(g_FontData.g_FontNum15);
	m_spinFloatPrice.SetWindowNumber(0);
	EnableWindow(m_spinFloatPrice.GetEditHwnd(), false);
	EnableWindow(m_spinFloatPrice.GetSpinHwnd(), false);

	m_spinBEPrice.SetFont(g_FontData.g_FontNum15);
	m_spinBEPrice.SetWindowNumber(0);

	m_cbStopLoss.SetFont(g_FontData.g_FontWord12);
	m_cbStopLoss.AddString(ANSIToUnicode(_GetString_StopLPView(3)).c_str());
	m_cbStopLoss.SetSelect(0);

	m_cbStopProfit.SetFont(g_FontData.g_FontWord12);
	m_cbStopProfit.AddString(ANSIToUnicode(_GetString_StopLPView(4)).c_str());
	m_cbStopProfit.SetSelect(0);

	m_cbBreakEven.SetFont(g_FontData.g_FontWord12);
	m_cbBreakEven.AddString(ANSIToUnicode(_GetString_StopLPView(5)).c_str());
	m_cbBreakEven.SetSelect(0);

	m_btnStart.SetbtnTextColor(RGB(0, 255, 0));
	m_btnStart.SetFont(g_FontData.g_FontWord13);
	m_btnPause.SetbtnTextColor(RGB(255, 0, 0));
	m_btnPause.SetFont(g_FontData.g_FontWord13);
	ShowWindow(m_btnPause.GetHwnd(), SW_HIDE);


}

void TStopLPView::SetWidetPos()
{
	RECT rect;
	GetClientRect(m_Hwnd, &rect);

	m_rectChoice[0] = { rect.left + 2, rect.top + CAPTION_HEIGHT, rect.left + 131, rect.top + CAPTION_HEIGHT + CHOOSEFRAME_HEIGHT };
	m_rectChoice[1] = { rect.left + 130, rect.top + CAPTION_HEIGHT, rect.left + 262, rect.top + CAPTION_HEIGHT + CHOOSEFRAME_HEIGHT };
	m_rectChoice[2] = { rect.left + 261, rect.top + CAPTION_HEIGHT, rect.left + 393, rect.top + CAPTION_HEIGHT + CHOOSEFRAME_HEIGHT };//三个切换按钮的位置

	int VGAP = 40;
	int HGAP = 50;
	int LeftGap = 60;
	int WidgetHeight = 20;

	rect.top = CAPTION_HEIGHT + VGAP;
//	rect.left = LeftGap / 2;
	rect.left = 10;
//	rect.right = rect.left + 90;
	rect.right = rect.left + 120;
	rect.bottom = rect.top + WidgetHeight;
	m_scContract.MoveWindow(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);

	rect.left = rect.right + 5;
	rect.right = rect.left + 215;
	m_cbStopLoss.MoveWindow(rect.left, rect.top, rect.right - rect.left, (rect.bottom - rect.top) * 5);
	m_cbStopProfit.MoveWindow(rect.left, rect.top, rect.right - rect.left, (rect.bottom - rect.top) * 5);
	m_cbBreakEven.MoveWindow(rect.left, rect.top, rect.right - rect.left, (rect.bottom - rect.top) * 5);



	rect.left = LeftGap / 2;
	rect.top += VGAP;
	rect.bottom += VGAP;
	rect.right = LeftGap / 2 + 50;
	m_scPrice.MoveWindow(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);

	rect.left = rect.right;
	rect.right = rect.left + 120;
	m_spinSLPrice.MoveWindow(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
	m_spinSPPrice.MoveWindow(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
	m_spinBEPrice.MoveWindow(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);

	rect.left = rect.right + HGAP;
	rect.right = rect.left + 100;
	m_spinSLQty.MoveWindow(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
	m_spinSPQty.MoveWindow(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
	m_spinBEQty.MoveWindow(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);

	rect.right = rect.left;
	rect.left = rect.right - 30;
	m_scQty.MoveWindow(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);

	rect.left = LeftGap;
	rect.right = rect.left + 300;
	rect.top += VGAP/2;
	rect.bottom += VGAP/2;
	m_scReMinder.MoveWindow(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);

	rect.left = LeftGap/2;
	rect.top += VGAP/2;
	rect.bottom += VGAP/2;
	rect.right = LeftGap + 100;
	m_ckFloat.MoveWindow(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);

	rect.left = rect.right;
	rect.right = rect.left + 110;
	m_spinFloatPrice.MoveWindow(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);

	rect.top += VGAP;
	rect.bottom = rect.top + 100;
	rect.left = LeftGap / 2;
	rect.right = rect.left + 100;
	m_btnPause.MoveWindow(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
	m_btnStart.MoveWindow(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);

	rect.left = rect.right + 5;
	rect.right = FRAME_WIDTH - HGAP / 2;
	m_scInfo.MoveWindow(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
}

void TStopLPView::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
	if (!m_bmousetrack)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = m_Hwnd;
		tme.dwHoverTime = 0;
		::TrackMouseEvent(&tme);

		m_bmousetrack = true;
	}
	POINT pt;
	pt.x = LOWORD(lParam);
	pt.y = HIWORD(lParam);

	m_bhoverclose = false;
	if (PtInRect(&m_rectClose, pt))
		m_bhoverclose = true;

	m_bhovermini = false;
	if (PtInRect(&m_rectMini, pt))
		m_bhovermini = true;

	InvalidateRect(m_Hwnd, 0, false);
}
void TStopLPView::OnMouseLeave()
{
	m_bmousetrack = false;
	m_bhoverclose = false;
	m_bhovermini = false;
	InvalidateRect(m_Hwnd, 0, false);
}
void TStopLPView::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
	RECT rect;
	GetClientRect(m_Hwnd, &rect);

	POINT pt;
	pt.x = LOWORD(lParam);
	pt.y = HIWORD(lParam);
	rect.bottom = rect.top + CAPTION_HEIGHT;
	if (PtInRect(&rect, pt) && !PtInRect(&m_rectClose, pt))
	{
		PostMessage(m_Hwnd, WM_NCLBUTTONDOWN, HTCAPTION, lParam);
	}
	else if (PtInRect(&m_rectChoice[0], pt)&&m_iSelPage!=Page_StopLoss)
	{
		SwitchPage(Page_StopLoss);
	}
	else if (PtInRect(&m_rectChoice[1], pt) && m_iSelPage != Page_StopProfit)
	{
		SwitchPage(Page_StopProfit);
	}
	else if (PtInRect(&m_rectChoice[2], pt) && m_iSelPage != Page_BreakEven)
	{
		SwitchPage(Page_BreakEven);
	}
}
void TStopLPView::OnLButtonUp(WPARAM wParam, LPARAM lParam)
{
	POINT pt;
	pt.x = LOWORD(lParam);
	pt.y = HIWORD(lParam);
	if (PtInRect(&m_rectClose, pt))
	{
		CloseWnd();
	}
}

LRESULT TStopLPView::OnCtrlColor(WPARAM wParam, LPARAM lParam)
{
	if ((HWND)lParam == m_scReMinder.GetHwnd())
	{
		SetTextColor((HDC)wParam, RGB(30, 117, 187));
	}
	else if ((HWND)lParam == m_scInfo.GetHwnd())
	{
		SetTextColor((HDC)wParam, RGB(30, 117, 187));
	}
	SetBkMode((HDC)wParam, TRANSPARENT);
	return (LRESULT)g_ColorRefData.g_brush_bk;
}

void TStopLPView::OnStaticBtnClick(WPARAM wParam, LPARAM lParam)
{
	switch ((int)lParam)
	{
	case 0:
		OnStart();
		/*ShowStartBtn(false)*/;
		break;
	case 1:
		OnStop();
		/*ShowStartBtn(true);*/
		break;
	default:
		break;
	}
}



void TStopLPView::OnNotify(WPARAM wParam, LPARAM lParam)
{
	if (LOWORD(wParam) == ID_spinSLPrice)
	{
		LPNMUPDOWN pSLPriceUpDown = (LPNMUPDOWN)lParam;
		DealSpinUD(m_spinSLPrice, pSLPriceUpDown->iDelta > 0);
	}
	else if (LOWORD(wParam) == ID_spinSPPrice)
	{
		LPNMUPDOWN pSPPriceUpDown = (LPNMUPDOWN)lParam;
		DealSpinUD(m_spinSPPrice, pSPPriceUpDown->iDelta > 0);
	}
	else if (LOWORD(wParam) == ID_spinBEPrice)
	{
		LPNMUPDOWN pBEPriceUpDown = (LPNMUPDOWN)lParam;
		DealSpinUD(m_spinBEPrice, pBEPriceUpDown->iDelta > 0);
	}
	else if (LOWORD(wParam) == ID_spinFloatPrice)
	{
		LPNMUPDOWN pFloatPriceUpDown = (LPNMUPDOWN)lParam;
		DealSpinUD(m_spinFloatPrice, pFloatPriceUpDown->iDelta > 0);
		double sign = (m_pData->direct == dBuy) ? 1 : -1;
		double dSLPrice = m_pData->LastPrice - sign * m_spinFloatPrice.GetDoubleData();
		char Price[20] = { 0 };
		DoubleToChar(m_pData->iPrecision, dSLPrice, Price);
		m_spinSLPrice.SetText(Price);	
	}
	else if (LOWORD(wParam) == ID_spinSLQty)
	{
		LPNMUPDOWN pSLQtyUpDown = (LPNMUPDOWN)lParam;
		DealSpinQty(m_spinSLQty, pSLQtyUpDown->iDelta > 0);
	}
	else if (LOWORD(wParam) == ID_spinSPQty)
	{
		LPNMUPDOWN pSPQtyUpDown = (LPNMUPDOWN)lParam;
		DealSpinQty(m_spinSPQty, pSPQtyUpDown->iDelta > 0);
	}
	else if (LOWORD(wParam) == ID_spinBEQty)
	{
		LPNMUPDOWN pBEQtyUpDown = (LPNMUPDOWN)lParam;
		DealSpinQty(m_spinBEQty, pBEQtyUpDown->iDelta > 0);
	}
	else
	{
		return;
	}
	RefreshReminder();
}

void TStopLPView::DealSpinUD(CxSpinEdit& spinEdit, bool Up)
{
	char Price[20] = { 0 };
	double dNow = spinEdit.GetDoubleData();
	if (Up)
		dNow += m_pData->minChangePrice;
	else
		dNow -= m_pData->minChangePrice;
	if (dNow < 0)
	{
		dNow = 0;
	}
	if (dNow > 1000000)
	{
		dNow = 1000000;
	}
	DoubleToChar(m_pData->iPrecision, dNow, Price);
	spinEdit.SetText(Price);
}

void TStopLPView::DealSpinQty(CxSpinEdit& spinEdit, bool Up)
{
	char Price[20] = { 0 };
	int dNow = spinEdit.GetWindowNumber();
	if (Up)
		dNow += 1;
	else
		dNow -= 1;
	if (dNow < 0)
	{
		dNow = 0;
	}
	if (dNow > 100000)
	{
		dNow = 100000;
	}
	_itoa_s(dNow, Price, 10);
	spinEdit.SetText(Price);
}

void TStopLPView::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if ((HWND)lParam == m_cbStopLoss.GetHwnd() && HIWORD(wParam) == CBN_SELCHANGE)
	{
		int index = m_cbStopLoss.GetSelect();
		if (index == CB_ERR)
			return;
		UpdateViewWidget_CBChange(index,otLoss);
	}
	else if ((HWND)lParam == m_cbStopProfit.GetHwnd() && HIWORD(wParam) == CBN_SELCHANGE)
	{
		int index = m_cbStopProfit.GetSelect();
		if (index == CB_ERR)
			return;
		UpdateViewWidget_CBChange(index, otProfit);
	}
	else if ((HWND)lParam == m_cbBreakEven.GetHwnd() && HIWORD(wParam) == CBN_SELCHANGE)
	{
		int index = m_cbBreakEven.GetSelect();
		if (index == CB_ERR)
			return;
		UpdateViewWidget_CBChange(index, otBreakEven);
	}
}

void TStopLPView::UpdateViewWidget_CBChange(int index, OrderLPType type)
{
	if (nullptr == m_pData)
		return;
	switch (type)
	{
	case otLoss:
	{
		if (index == 0)
		{
			//新止损单
			m_spinSLQty.SetWindowNumber(m_pData->Qty);
			SetWindowDouble(m_spinSLPrice.GetEditHwnd(), m_pData->iPrecision, m_pData->LastPrice);
			SetWindowDouble(m_spinFloatPrice.GetEditHwnd(), m_pData->iPrecision, 0);
			//**Enable控件，显示已暂停
			EnableWindow(m_spinSLQty.GetEditHwnd(), true);
			EnableWindow(m_spinSLQty.GetSpinHwnd(), true);
			EnableWindow(m_spinSLPrice.GetEditHwnd(), true);
			EnableWindow(m_spinSLPrice.GetSpinHwnd(), true);
			EnableWindow(m_spinFloatPrice.GetEditHwnd(), true);
			EnableWindow(m_spinFloatPrice.GetSpinHwnd(), true);
			EnableWindow(m_ckFloat.GetHwnd(), true);
			m_ckFloat.SetCheck(BST_UNCHECKED);
			OnCheckBox((LPARAM)m_ckFloat.GetHwnd());
			m_DealingKeyID_StopL = -1;

			//按钮状态变更
			ShowStartBtn(true);
		}
		else if (index > m_vecStopLossItem.size())
		{
			return;
		}
		else
		{
			const TStopLPitem* pitem = m_vecStopLossItem[index - 1];
			SetWindowDouble(m_spinSLPrice.GetEditHwnd(), m_pData->iPrecision, pitem->triggerPrice);
			m_spinSLQty.SetWindowNumber(pitem->Qty);
			SetWindowDouble(m_spinFloatPrice.GetEditHwnd(), m_pData->iPrecision, pitem->floatPoint);

			m_DealingKeyID_StopL = pitem->keyID;

			//按钮状态变更
			if (osSuspend == pitem->state)
			{
				ShowStartBtn(true);
				//**Enable控件，显示已暂停
				EnableWindow(m_spinSLQty.GetEditHwnd(), true);
				EnableWindow(m_spinSLQty.GetSpinHwnd(), true);
				EnableWindow(m_spinSLPrice.GetEditHwnd(), true);
				EnableWindow(m_spinSLPrice.GetSpinHwnd(), true);
				EnableWindow(m_spinFloatPrice.GetEditHwnd(), true);
				EnableWindow(m_spinFloatPrice.GetSpinHwnd(), true);
				EnableWindow(m_ckFloat.GetHwnd(), true);
				if (pitem->orderType == otFloat)
				{
					m_ckFloat.SetCheck(BST_CHECKED);
				}
				else
				{
					m_ckFloat.SetCheck(BST_UNCHECKED);
				}
				OnCheckBox((LPARAM)m_ckFloat.GetHwnd());
			}
			else
			{
				ShowStartBtn(false);
				//**Disable控件，显示已启动
				EnableWindow(m_spinSLQty.GetEditHwnd(), false);
				EnableWindow(m_spinSLQty.GetSpinHwnd(), false);
				EnableWindow(m_ckFloat.GetHwnd(), false);
				EnableWindow(m_spinSLPrice.GetEditHwnd(), false);
				EnableWindow(m_spinSLPrice.GetSpinHwnd(), false);
				EnableWindow(m_spinFloatPrice.GetEditHwnd(), false);
				EnableWindow(m_spinFloatPrice.GetSpinHwnd(), false);
			}
		}
		break;
	}
	case otProfit:
	{
		if (index == 0)
		{
			//新止盈单
			m_spinSPQty.SetWindowNumber(m_pData->Qty);
			SetWindowDouble(m_spinSPPrice.GetEditHwnd(), m_pData->iPrecision, m_pData->LastPrice);
			m_DealingKeyID_StopP = -1;

			//按钮状态变更
			ShowStartBtn(true);
			EnableWindow(m_spinSPQty.GetEditHwnd(), true);
			EnableWindow(m_spinSPQty.GetSpinHwnd(), true);
			EnableWindow(m_spinSPPrice.GetEditHwnd(), true);
			EnableWindow(m_spinSPPrice.GetSpinHwnd(), true);
		}
		else if (index > m_vecStopProfitItem.size())
		{
			return;
		}
		else
		{
			const TStopLPitem* pitem = m_vecStopProfitItem[index - 1];
			SetWindowDouble(m_spinSPPrice.GetEditHwnd(), m_pData->iPrecision, pitem->triggerPrice);
			m_spinSPQty.SetWindowNumber(pitem->Qty);
			m_DealingKeyID_StopP = pitem->keyID;

			//按钮状态变更
			if (osSuspend == pitem->state)
			{
				ShowStartBtn(true);
				//**Enable
				EnableWindow(m_spinSPQty.GetEditHwnd(), true);
				EnableWindow(m_spinSPQty.GetSpinHwnd(), true);
				EnableWindow(m_spinSPPrice.GetEditHwnd(), true);
				EnableWindow(m_spinSPPrice.GetSpinHwnd(), true);
			}
			else
			{
				ShowStartBtn(false);
				//**Disable
				EnableWindow(m_spinSPQty.GetEditHwnd(), false);
				EnableWindow(m_spinSPQty.GetSpinHwnd(), false);
				EnableWindow(m_spinSPPrice.GetEditHwnd(), false);
				EnableWindow(m_spinSPPrice.GetSpinHwnd(), false);
			}
		}
		break;
	}
	case otBreakEven:
	{
		if (index == 0)
		{
			//新保本单
			m_spinBEQty.SetWindowNumber(m_pData->Qty);
			SetWindowDouble(m_spinBEPrice.GetEditHwnd(), m_pData->iPrecision, 0);
			m_DealingKeyID_BreakE = -1;

			//按钮状态变更
			ShowStartBtn(true);
			EnableWindow(m_spinBEQty.GetEditHwnd(), true);
			EnableWindow(m_spinBEQty.GetSpinHwnd(), true);
			EnableWindow(m_spinBEPrice.GetEditHwnd(), true);
			EnableWindow(m_spinBEPrice.GetSpinHwnd(), true);
		}
		else if (index > m_vecBreakEvenItem.size())
		{
			return;
		}
		else
		{
			const TStopLPitem* pitem = m_vecBreakEvenItem[index - 1];
			SetWindowDouble(m_spinBEPrice.GetEditHwnd(), m_pData->iPrecision, pitem->breakEvenPoint);
			m_spinBEQty.SetWindowNumber(pitem->Qty);
			m_DealingKeyID_BreakE = pitem->keyID;

			//按钮状态变更
			if (osSuspend == pitem->state)
			{
				ShowStartBtn(true);
				//**Enable
				EnableWindow(m_spinBEQty.GetEditHwnd(), true);
				EnableWindow(m_spinBEQty.GetSpinHwnd(), true);
				EnableWindow(m_spinBEPrice.GetEditHwnd(), true);
				EnableWindow(m_spinBEPrice.GetSpinHwnd(), true);
			}
			else
			{
				ShowStartBtn(false);
				//**Disable
				EnableWindow(m_spinBEQty.GetEditHwnd(), false);
				EnableWindow(m_spinBEQty.GetSpinHwnd(), false);
				EnableWindow(m_spinBEPrice.GetEditHwnd(), false);
				EnableWindow(m_spinBEPrice.GetSpinHwnd(), false);
			}
		}
		break;
	}
	default:
		break;
	}
	UpdatePositionPrice();
	RefreshReminder();//刷新提示
}


void TStopLPView::OnCheckBox(LPARAM lParam)
{
	if ((HWND)lParam == m_ckFloat.GetHwnd())
	{
		if (m_ckFloat.GetCheck())
		{
			EnableWindow(m_spinFloatPrice.GetEditHwnd(),true);
			EnableWindow(m_spinFloatPrice.GetSpinHwnd(), true);
			EnableWindow(m_spinSLPrice.GetEditHwnd(), false);
			EnableWindow(m_spinSLPrice.GetSpinHwnd(), false);
		}
		else
		{
			EnableWindow(m_spinFloatPrice.GetEditHwnd(), false);
			EnableWindow(m_spinFloatPrice.GetSpinHwnd(), false);
			EnableWindow(m_spinSLPrice.GetEditHwnd(), true);
			EnableWindow(m_spinSLPrice.GetSpinHwnd(), true);
		}
	}
}

//重发和新发单子
void TStopLPView::OnStart()
{
	//1、新建
	if (m_iSelPage == Page_StopLoss&&m_cbStopLoss.GetSelect() == 0 || m_iSelPage == Page_StopProfit&&m_cbStopProfit.GetSelect() == 0 || m_iSelPage == Page_BreakEven&&m_cbBreakEven.GetSelect() == 0)
	{
		
		TStopLPInsertItem InsertItem;
		InsertItem.MaxCount = g_pGlobalModel->m_stStopLossConfig.iTriggerCount;
		InsertItem.trigBase = tmbLatest;
		memcpy_s(InsertItem.userNo, sizeof(InsertItem.userNo), m_pData->UserNo.c_str(), sizeof(InsertItem.userNo));
		memcpy_s(InsertItem.sign, sizeof(InsertItem.sign), m_pData->sign.c_str(), sizeof(InsertItem.sign));
		memcpy_s(InsertItem.code, sizeof(InsertItem.code), m_StrCode.c_str(), sizeof(InsertItem.code));
		if (m_pData->direct == dBuy)
		{
			InsertItem.direct = dSell;
		}
		else if (m_pData->direct == dSell)
		{
			InsertItem.direct = dBuy;
		}
		else
		{
			InsertItem.direct = m_pData->direct;
		}

		InsertItem.openPrice = m_pData->OpenPrice;//开仓均价
		//填结构体
		if (m_iSelPage == Page_StopLoss)
		{
			InsertItem.orderPriceType = g_pGlobalModel->m_stStopLossConfig.iStopLossOrderPrice;
			if (m_ckFloat.GetCheck())
			{
				InsertItem.orderType = otFloat;
				InsertItem.floatPoint = m_spinFloatPrice.GetDoubleData();
			}
			else
			{
				InsertItem.orderType = otLoss;
			}
			InsertItem.triggerPrice = m_spinSLPrice.GetDoubleData();
			InsertItem.Qty = m_spinSLQty.GetWindowNumber();
		}
		else if (m_iSelPage == Page_StopProfit)
		{
			InsertItem.orderPriceType = g_pGlobalModel->m_stStopLossConfig.iStopProfitOrderPrice;
			InsertItem.orderType = Page2OrderLPType(m_iSelPage);
			InsertItem.triggerPrice = m_spinSPPrice.GetDoubleData();
			InsertItem.Qty = m_spinSPQty.GetWindowNumber();
		}
		else if (m_iSelPage == Page_BreakEven)
		{
			InsertItem.orderPriceType = g_pGlobalModel->m_stStopLossConfig.iStopLossOrderPrice;
			InsertItem.orderType = Page2OrderLPType(m_iSelPage);
			InsertItem.breakEvenPoint = m_spinBEPrice.GetDoubleData();
			InsertItem.Qty = m_spinBEQty.GetWindowNumber();
			InsertItem.triggerPrice = m_pData->OpenPrice + InsertItem.breakEvenPoint;//触发价为最新价+保本价差
		}
		m_Mediator->Send(CMD_StopLPView_InsertOrder, &InsertItem, this);//insert有memcpy，所以没问题
		UpdatePositionPrice();
		return;
	}
	//2、修改
	else
	{
		int DealingKeyID = -1;
		if (m_iSelPage == Page_StopLoss)
		{
			DealingKeyID = m_DealingKeyID_StopL;
		}
		else if (m_iSelPage == Page_StopProfit)
		{
			DealingKeyID = m_DealingKeyID_StopP;
		}
		else if (m_iSelPage == Page_BreakEven)
		{
			DealingKeyID = m_DealingKeyID_BreakE;
		}
		for (uint i = 0; i < m_pComboxData->size(); i++)
		{
			TStopLPitem* pitem = (TStopLPitem*)m_pComboxData->at(i);
			if (pitem == nullptr)
			{
				continue;
			}
			if (DealingKeyID == pitem->keyID)
			{
				TStopLPitem PauseItem;
				//填结构体
				memcpy_s(&PauseItem, sizeof(TStopLPitem), pitem, sizeof(TStopLPitem));
				if (osSuspend == PauseItem.state)
				{
					PauseItem.state = osGeneral;
				}
				if (m_iSelPage == Page_StopLoss)
				{
					if (m_ckFloat.GetCheck())
					{
						PauseItem.orderType = otFloat;
						PauseItem.floatPoint = m_spinFloatPrice.GetDoubleData();
					}
					else
					{
						PauseItem.orderType = otLoss;
					}
					PauseItem.triggerPrice = m_spinSLPrice.GetDoubleData();
					PauseItem.Qty = m_spinSLQty.GetWindowNumber();
				}
				else if (m_iSelPage == Page_StopProfit)
				{
					PauseItem.orderType = Page2OrderLPType(m_iSelPage);
					PauseItem.triggerPrice = m_spinSPPrice.GetDoubleData();
					PauseItem.Qty = m_spinSPQty.GetWindowNumber();
				}
				else if (m_iSelPage == Page_BreakEven)
				{
					PauseItem.orderType = Page2OrderLPType(m_iSelPage);
					PauseItem.breakEvenPoint = m_spinBEPrice.GetDoubleData();
					PauseItem.Qty = m_spinBEQty.GetWindowNumber();
					PauseItem.triggerPrice = m_pData->LastPrice + PauseItem.breakEvenPoint;//触发价为最新价+保本价差
				}

				m_Mediator->Send(CMD_StopLPView_ModifyOrder, &PauseItem, this);
				break;
			}
		}
	}

	

}

void TStopLPView::OnStop()
{
	int DealingKeyID = -1;
	if (m_iSelPage == Page_StopLoss)
	{
		DealingKeyID = m_DealingKeyID_StopL;
	}
	else if (m_iSelPage == Page_StopProfit)
	{
		DealingKeyID = m_DealingKeyID_StopP;
	}
	else if (m_iSelPage == Page_BreakEven)
	{
		DealingKeyID = m_DealingKeyID_BreakE;
	}
	for (uint i = 0; i < m_pComboxData->size(); i++)
	{
		TStopLPitem* pitem = (TStopLPitem*)m_pComboxData->at(i);
		if (pitem == nullptr)
		{
			continue;
		}
		if (DealingKeyID == pitem->keyID)
		{
			TStopLPitem PauseItem;
			memcpy_s(&PauseItem, sizeof(TStopLPitem), pitem, sizeof(TStopLPitem));
			PauseItem.state = osSuspend;
			PauseItem.TriggerCount = 0;
			PauseItem.LastTirgger = false;//暂停，状态挂起，触发次数清空。
			//填结构体
			m_Mediator->Send(CMD_StopLPView_ModifyOrder, &PauseItem, this);
		}
	}
}

void TStopLPView::PageOneSH(bool bShow)
{
	int swCmd = 5 * bShow;
	ShowWindow(m_cbStopLoss.GetHwnd(), swCmd);
	
	ShowWindow(m_spinSLPrice.GetEditHwnd(), swCmd);
	ShowWindow(m_spinSLPrice.GetSpinHwnd(), swCmd);
	ShowWindow(m_spinSLQty.GetEditHwnd(), swCmd);
	ShowWindow(m_spinSLQty.GetSpinHwnd(), swCmd);
	ShowWindow(m_spinFloatPrice.GetEditHwnd(), swCmd);
	ShowWindow(m_spinFloatPrice.GetSpinHwnd(), swCmd);
	ShowWindow(m_scFloatPrice.GetHwnd(), swCmd);
	ShowWindow(m_ckFloat.GetHwnd(), swCmd);
	if (bShow)
	{
		m_scPrice.SetText(_GetString_StopLPView(22));
	}
}
void TStopLPView::PageTwoSH(bool bShow)
{
	int swCmd = 5 * bShow;
	ShowWindow(m_cbStopProfit.GetHwnd(), swCmd);

	ShowWindow(m_spinSPPrice.GetEditHwnd(), swCmd);
	ShowWindow(m_spinSPPrice.GetSpinHwnd(), swCmd);
	ShowWindow(m_spinSPQty.GetEditHwnd(), swCmd);
	ShowWindow(m_spinSPQty.GetSpinHwnd(), swCmd);
	if (bShow)
	{
		m_scPrice.SetText(_GetString_StopLPView(23));
	}
}
void TStopLPView::PageThreeSH(bool bShow)
{
	int swCmd = 5 * bShow;
	ShowWindow(m_cbBreakEven.GetHwnd(), swCmd);

	ShowWindow(m_spinBEPrice.GetEditHwnd(), swCmd);
	ShowWindow(m_spinBEPrice.GetSpinHwnd(), swCmd);
	ShowWindow(m_spinBEQty.GetEditHwnd(), swCmd);
	ShowWindow(m_spinBEQty.GetSpinHwnd(), swCmd);
	if (bShow)
	{
		m_scPrice.SetText(_GetString_StopLPView(24));
	}
}

void TStopLPView::CloseWnd()
{
	if (IsWindowVisible(m_Hwnd))
	{
		ShowWindow(m_Hwnd, SW_HIDE);
	}
}

void TStopLPView::ShowStartBtn(bool show)
{
	if (show)
	{
		ShowWindow(m_btnStart.GetHwnd(), SW_SHOW);
		ShowWindow(m_btnPause.GetHwnd(), SW_HIDE);
	}
	else
	{
		ShowWindow(m_btnStart.GetHwnd(), SW_HIDE);
		ShowWindow(m_btnPause.GetHwnd(), SW_SHOW);
	}
}


void TStopLPView::RefreshReminder()
{
	if (m_pData == nullptr)return;
	string str;
	switch (m_iSelPage)
	{
	case Page_StopLoss:
	{
		if (BST_CHECKED == m_ckFloat.GetCheck())
		{
			str = "";
			break;
		}
		else
		{
			double SLPrice = m_spinSLPrice.GetDoubleData();
			int	Qty = m_spinSLQty.GetWindowNumber();
			double sign = (m_pData->direct == dBuy) ? 1 : -1;
			double Spreadl = sign * (m_pData->LastPrice - SLPrice);
			double Spread = sign * (m_pData->OpenPrice - SLPrice);//价差
			if (Spreadl <0)
			{
				str = _GetString_StopLPView(25);
				break;
			}
			else if (Spread <= 0)
			{
				str = "";
				break;
			}
			else
			{
				str += _GetString_StopLPView(26);
				char temp[26];
				char format[10];
				sprintf_s(format, "%%.%dlf", m_pData->iPrecision);
				sprintf_s(temp, format, Spread);
				str += temp;
				str += _GetString_StopLPView(27);
				double delta = Spread * m_pData->ProductDot * Qty;
				sprintf_s(temp, format, delta);
				str += temp;
				if (m_pData->InitDeposit != 0)
				{
					str += _GetString_StopLPView(28);
					double Percentage = Spread * m_pData->Qty * m_pData->ProductDot * 100 / m_pData->InitDeposit;
					sprintf_s(temp, "%.1lf%%", Percentage);
					str += temp;
				}
				else
				{
					if (g_language == ENU)
						str += " Yuan";
					else
						str += "元";
				}
			}
		}
		break;
	}
	case Page_StopProfit:
	{
		double SPPrice = m_spinSPPrice.GetDoubleData();
		int	Qty = m_spinSPQty.GetWindowNumber();
		double sign = (m_pData->direct == dSell) ? 1 : -1;
		double Spreadl = sign * (m_pData->LastPrice - SPPrice);
		double Spread = sign * (m_pData->OpenPrice - SPPrice);//价差
		if (Spreadl < 0)
		{
			str = _GetString_StopLPView(29);
			break;
		}
		else if (Spread <= 0)
		{
			str = "";
			break;
		}
		else
		{
			str += _GetString_StopLPView(26);
			char temp[26];
			char format[10];
			sprintf_s(format, "%%.%dlf", m_pData->iPrecision);
			sprintf_s(temp, format, Spread);
			str += temp;
			str += _GetString_StopLPView(30);
			double delta = Spread * m_pData->ProductDot * Qty;
			sprintf_s(temp, format, delta);
			str += temp;
			str += _GetString_StopLPView(28);
			double Percentage = Spread * m_pData->Qty * m_pData->ProductDot * 100 / m_pData->InitDeposit;
			sprintf_s(temp, "%.1lf%%", Percentage);
			str += temp;
		}
		break;
	}
	case Page_BreakEven:
	{
		double BEPrice = m_spinBEPrice.GetDoubleData();
		int	Qty = m_spinBEQty.GetWindowNumber();
		double delta = BEPrice * m_pData->ProductDot * Qty;
		char temp[26];
		char format[10];
		sprintf_s(format, "%%.%dlf", m_pData->iPrecision);
		sprintf_s(temp, format, delta);
		str += _GetString_StopLPView(31);
		str += temp;
		str += _GetString_StopLPView(28);
		double Percentage = BEPrice * m_pData->Qty * m_pData->ProductDot * 100 / m_pData->InitDeposit;
		sprintf_s(temp, "%.1lf%%", Percentage);
		str += temp;
		break;
	}

	default:
		break;
	}
	m_scReMinder.SetText(str.c_str());
}

OrderLPType TStopLPView::Page2OrderLPType(const int &iPage)
{
	switch (iPage)
	{
	case Page_StopLoss:
		return otLoss;
	case Page_StopProfit:
		return otProfit;
	case Page_BreakEven:
		return otBreakEven;
	default:
		return otVoid;
	}
}
int TStopLPView::OrderLPType2Page(const OrderLPType &Ordertype)
{
	switch (Ordertype)
	{
	case otLoss:
	case otFloat:
		return Page_StopLoss;
	case otProfit:
		return Page_StopProfit;
	case otBreakEven:
		return Page_BreakEven;
	default:
		return Page_StopLoss;
	}
}