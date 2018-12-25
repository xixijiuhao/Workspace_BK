#include "PreInclude.h"

#define widgetLeft 15
#define widgetHeight 20
#define cbWidth  120
#define labWidth 55
#define widgetTop 50
#define widgetVGap  (widgetHeight + 10)
#define btnWidth 60

extern ITradeData* g_pTradeData;
extern ITradeApi* g_pTradeApi;
extern IStarApi  *g_QuoteApi;


char szChsSpos[][10] = { "移仓", "确定", "取消", "移仓合约", "移仓手数", "提示", "移仓单" };
char szChtSpos[][10] = { "移倉", "確定", "取消", "移倉合約", "移倉手數", "提示", "移倉單" };
char szEnuSpos[][20] = { "Shift Position", "OK", "Cancel", "Contract", "Lots", "Waring", "Shift Order" };
enum{
	ShiftPos_Index,
	OK_Index,
	Cancel_Index,
	Contract_Index,
	Lots_Index,
	Warning_Index,
	ShiftOrder_Index,
};

char * _GetTextSpos(int i)
{
	if (g_language == ENU)return szEnuSpos[i];
	else if (g_language == CHT)return szChtSpos[i];
	return szChsSpos[i];
}
TShiftPosition::TShiftPosition()
{
	m_iIndex = 0;
}
TShiftPosition::~TShiftPosition()
{

}
void TShiftPosition::SetOrderInfo(const TPosition* pPosition)
{	
	if (pPosition == NULL)
		return;
	EnableWindow(::GetParent(m_Hwnd), FALSE);
	m_pstPosition = pPosition;

	InitCombox();

	m_spinQty.SetWindowNumber((int)pPosition->PositionQty);
	m_spinQty.SetRange(0, pPosition->PositionQty);
}
void TShiftPosition::Create(HWND hParent)
{
	ShowFrm(hParent, L"ShiftPosition", _GetTextSpos(ShiftPos_Index));
	POINT pst;
	GetCursorPos(&pst);
	MoveWindow(pst.x - 260, pst.y - 180, 230, 150);
	m_pstPosition = nullptr;
}

void TShiftPosition::OnCreate()
{	
	int top = widgetTop;
	int left = widgetLeft;
	m_labContract.Create(m_Hwnd, L"");
	m_labContract.SetText(_GetTextSpos(Contract_Index));
	m_labContract.MoveWindow(widgetLeft, widgetTop, labWidth, widgetHeight);

	
	top += widgetVGap;
	m_labQty.Create(m_Hwnd, L"");
	m_labQty.SetText(_GetTextSpos(Lots_Index));
	m_labQty.MoveWindow(left, top, labWidth, widgetHeight);


	top = widgetTop;
	left = widgetLeft + labWidth;
	m_comContract.Create(m_Hwnd);
	m_comContract.MoveWindow(left, top, cbWidth, widgetHeight*5);
	m_comContract.SetFont(g_FontData.g_FontWord13);

	top += widgetVGap;
	m_spinQty.Create(m_Hwnd);
	m_spinQty.MoveWindow(left, top, cbWidth, widgetHeight);
	m_spinQty.SetFont(g_FontData.g_FontNum15);

	top += widgetVGap;
	m_btnOK.Create(m_Hwnd, _GetTextSpos(OK_Index), ID_BTNOK);
	m_btnOK.MoveWindow(left, top, btnWidth, widgetHeight);

	left += btnWidth + 10;
	m_btnCancle.Create(m_Hwnd, _GetTextSpos(Cancel_Index), ID_BTNCANCLE);
	m_btnCancle.MoveWindow(left, top, btnWidth, widgetHeight);
	
}
void TShiftPosition::OnBtnClick(WPARAM wParam, LPARAM lParam)
{
	switch (lParam)
	{
	case ID_BTNOK:
		OnBtnOk();
		break;
	case ID_BTNCANCLE:
		OnBtnCancle();
		break;
	default:
		break;
	}
}
void TShiftPosition::OnBtnOk()
{
	TSendOrder stOrder;
	CConvert::DefaultSendOrder(stOrder);

	if (m_pstPosition->PositionQty == 0)
	{
		TCHAR szInfo[50] = {0};
		if (g_language == CHT)wsprintf(szInfo, L"持倉不存在！");
		else if (g_language) wsprintf(szInfo, L"持仓不存在！");
		else wsprintf(szInfo, L"Position not exist!");
		_TMessageBox_Domodal(m_Hwnd, _GetTextSpos(Warning_Index), szInfo, TMB_OK);
		return;
	}

	PositionToSendOrder(stOrder, m_pstPosition);

	stOrder.OrderQty =m_spinQty.GetWindowNumber();
	if (stOrder.OrderQty > m_pstPosition->PositionQty)
		stOrder.OrderQty = m_pstPosition->PositionQty;
	CConvert::GetPrice(&stOrder, stOrder.Direct, ptMarketPrice, stOrder.OrderPrice);
	
	strcpy_s(stOrder.Remark, _GetTextSpos(ShiftOrder_Index));
	vector<string> contract = CConvert::SplitString(m_comContract.GetText(), ' ');
	if (contract.size() >= 2)
		g_pTradeAction->InsertShiftOrder(stOrder, contract[1].c_str());

	OnBtnCancle();
}
void TShiftPosition::OnBtnCancle()
{
	CloseWnd();
}
void TShiftPosition::InitCombox()
{
	m_comContract.Clear();
	string strCodeContract = "";
	CConvert::PosContractToStr(strCodeContract, *m_pstPosition);
	TDataContainer stContracts;
	SContract * contract[100] = {};

	const SCommodity * pCommodity = g_pTradeData->GetCommodity(m_pstPosition);
	uint iCount = 0;
	if (pCommodity)
	{
		iCount = g_pStarApi->GetContractData(pCommodity->CommodityNo, "", contract, sizeof(contract) / sizeof(contract[0]), true);
	}
	else
	{
		SCommodityNoType commodityNo = {};
		sprintf_s(commodityNo, "%s|%c|%s", m_pstPosition->ExchangeNo, m_pstPosition->CommodityType, m_pstPosition->CommodityNo);
		iCount = g_pStarApi->GetContractData(commodityNo, "", contract, sizeof(contract) / sizeof(contract[0]), true);
	}
		

	string Code = "";

	char szContent[21] = { 0 };
	for (int i = 0; i < iCount; i++)
	{
		TContractKey contractKey;
		CConvert::SContractNoTypeToTradeContract("", contract[i]->ContractNo, contractKey);
		CConvert::PosContractToStr(Code, contractKey);
		if (Code == strCodeContract)
			continue;
		sprintf_s(szContent, "%s %s", contractKey.CommodityNo, contractKey.ContractNo);
		m_comContract.AddString(ANSIToUnicode(szContent).c_str());
	}
	m_comContract.SetSelect(0);
}

LRESULT TShiftPosition::OnNotify(WPARAM wParam, LPARAM lParam)
{ 
	return NOT_PROCESSED;
}
LRESULT TShiftPosition::OnCommand(WPARAM wParam, LPARAM lParam)
{ 
	return NOT_PROCESSED;
};
LRESULT TShiftPosition::OnDealMsg(UINT message, WPARAM wParam, LPARAM lParam)
{ 
	return NOT_PROCESSED; 
}
void TShiftPosition::DrawMain(TMemDC& pmemdc)
{
// 	int iOld = SaveDC(pmemdc.GetHdc());
// 	RECT rect = { LEFT, 170  - WIDGETH - 3, 260 - LEFT, 170 - 3  };
// 	SelectFont(pmemdc.GetHdc(), g_font_15);
// 	DrawTextA(pmemdc.GetHdc(), "说明:移仓最大手数1000.", -1, &rect, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
// 	RestoreDC(pmemdc.GetHdc(), iOld);
}
void TShiftPosition::PositionToSendOrder(TSendOrder& stOrder, const TPosition* p)
{
	CConvert::DefaultDealClose(p, stOrder);
	stOrder.Offset = CConvert::GetOffset(p->UserNo, p, stOrder.Direct);
}