#include "PreInclude.h"

#define captionHeight  30
#define captionColor   RGB(128,77,77)

#define label_num    4
#define label_left   7
#define label_height 20
#define lable_width  75
#define label_vgap   30

#define widgetLeft   (label_left+lable_width+5)
#define widgetWidth  150
#define widgetHeight 20

#define btnWidth     60
#define btnHeight    20
#define btnHgap      30


#define mainPrice    L"跟随主账号委托价"
#define queuePrice   L"即时买卖价"
#define rivalPrice   L"对手价"
#define marketPrice  L"涨跌停价(市价)"
TFollowConfig::TFollowConfig(HWND hParent, const char* pszTitle) :TModalWnd(hParent,pszTitle)
{
	m_pConfigData = NULL;
}
TFollowConfig::~TFollowConfig()
{

}
void TFollowConfig::SetModalRect()
{
	SetWindowPos(m_Hwnd, 0, m_rcPosition.left, m_rcPosition.top, m_rcPosition.right, m_rcPosition.bottom, SWP_SHOWWINDOW);
}
void TFollowConfig::MoveWindow(int x, int y, int cx, int cy)
{
	m_rcPosition.left = x;
	m_rcPosition.top = y;
	m_rcPosition.right = cx;
	m_rcPosition.bottom = cy;
}
void TFollowConfig::CreateSubCtrl()
{
	RECT rect = { 0 };

	m_cbPriceType.Create(m_Hwnd,id_PriceType);
	m_cbDirect.Create(m_Hwnd, id_Direct);

	m_editQtyRate.Create(m_Hwnd, CxEdit::floatStyle);

	m_followCoverTime.Create(m_Hwnd);
	m_followCoverTime.SetRange(2, 1000);

	m_spinChaseCount.Create(m_Hwnd);
	m_spinChaseCount.SetRange(1, 1000);

	m_spinCheckPositionTime.Create(m_Hwnd);
	m_spinCheckPositionTime.SetRange(5, 1000);
	m_ckUseCheckPosition.Create(m_Hwnd, id_ckUserCheckPosition, L"启用");
	m_ckUseCheckPosition.SetCheck(true);

	int top = m_rctitle.bottom + 10;
	wchar_t labelName[][10] = {L"跟单方向:", L"价格类型:", L"数量比率:", L"追平仓时间:", L"追平仓次数:",L"检测持仓平衡:"};
	for (int i = 0; i < ARRAYSIZE(labelName); i++)
	{
		m_label[i].Create(m_Hwnd, labelName[i], i, SS_RIGHT);
		m_label[i].MoveWindow(label_left, top, lable_width, label_height);
		top += label_vgap;
	}
	top = m_rctitle.bottom + 10;
	//top += label_vgap;
	//new start
	m_cbDirect.AddString(L"同向");
	m_cbDirect.AddString(L"反向");
	m_cbDirect.SetSelect(0);
	m_cbDirect.MoveWindow(widgetLeft, top, widgetWidth, widgetHeight * 5);
	top += label_vgap;
	//new end
	m_cbPriceType.MoveWindow(widgetLeft, top, widgetWidth, widgetHeight*5);
	m_cbPriceType.AddString(mainPrice);
	m_cbPriceType.AddString(queuePrice);
	m_cbPriceType.AddString(rivalPrice);
	m_cbPriceType.AddString(marketPrice);
	m_cbPriceType.SetSelect(0);
	top += label_vgap;
	m_editQtyRate.MoveWindow(widgetLeft, top, widgetWidth, widgetHeight);
	top += label_vgap;
	m_followCoverTime.MoveWindow(widgetLeft, top, widgetWidth, widgetHeight);
	top += label_vgap;
	m_spinChaseCount.MoveWindow(widgetLeft, top, widgetWidth, widgetHeight);
	//new
	top += label_vgap;
	int width = widgetWidth-50;
	m_spinCheckPositionTime.MoveWindow(widgetLeft, top, width, widgetHeight);
	int left = widgetLeft + width + 5;
	width = 45;
	m_ckUseCheckPosition.MoveWindow(left, top, width, widgetHeight);
	//new end

	top += label_vgap + 5;
	int btnLeft = (m_rcPosition.right - btnWidth * 2 - btnHgap) / 2;
	RECT rcBtn = { btnLeft, top, btnLeft + btnWidth, top + btnHeight };
	m_btnApply.CreateButton(m_Hwnd, "确定", id_btnApply);
	m_btnApply.MoveWindow(rcBtn.left, rcBtn.top, rcBtn.right - rcBtn.left, rcBtn.bottom - rcBtn.top);
	rcBtn.left += btnWidth + btnHgap;
	rcBtn.right = rcBtn.left + btnWidth;
	m_btnCancel.CreateButton(m_Hwnd, "取消", id_btnCancel);
	m_btnCancel.MoveWindow(rcBtn.left, rcBtn.top, rcBtn.right - rcBtn.left, rcBtn.bottom - rcBtn.top);
	Init();

	m_editQtyRate.SetFont(g_FontData.GetFontNum13());
}

void TFollowConfig::Init()
{
	if (!m_pConfigData)
		return;

	int cbSelectIndex = 0;
	if (m_pConfigData->FollowPriceType == fptQueuePrice)
		cbSelectIndex = 1;
	else if(m_pConfigData->FollowPriceType==fptConsideration)
		cbSelectIndex = 2;
	else if (m_pConfigData->FollowPriceType == fptMarket)
		cbSelectIndex = 3;
	m_cbPriceType.SetSelect(cbSelectIndex);

	cbSelectIndex = m_pConfigData->DirectSameToMain ? 0 : 1;
	m_cbDirect.SetSelect(cbSelectIndex);

	m_editQtyRate.SetDoubleData(m_pConfigData->QtyRate);
	m_followCoverTime.SetWindowNumber(m_pConfigData->FollowCoverTime);
	m_spinChaseCount.SetWindowNumber(m_pConfigData->ChaseCount);

	m_spinCheckPositionTime.SetWindowNumber(m_pConfigData->CheckPositionTime);
	m_ckUseCheckPosition.SetCheck(m_pConfigData->bUseCheckPosition);

}

LRESULT TFollowConfig::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case SSWM_STATIC_BUTTON_CLICKDOWN:
		OnClickBtn(lParam);
		break;
	case WM_CTLCOLORSTATIC:
		SetBkMode((HDC)wParam, TRANSPARENT);
		return (LRESULT)g_ColorRefData.g_brush_white;
	default:
		return __super::WndProc(message, wParam, lParam);
	}
	return NOT_PROCESSED;
}
void TFollowConfig::OnClickBtn(LPARAM lParam)
{
	if (lParam == id_btnApply)
	{
		GetData();
		DestroyWindow(m_Hwnd);
	}
	else if (lParam == id_btnCancel)
	{
		DestroyWindow(m_Hwnd);
	}
}
void TFollowConfig::GetData()
{
	if (!m_pConfigData)
		return;

	const wchar_t *pText = m_cbPriceType.GetText();

	if (lstrcmp(pText, mainPrice) == 0)
		m_pConfigData->FollowPriceType = fptFollowMain;
	else if (lstrcmp(pText, queuePrice) == 0)
		m_pConfigData->FollowPriceType = fptQueuePrice;
	else if (lstrcmp(pText, rivalPrice) == 0)
		m_pConfigData->FollowPriceType = fptConsideration;
	else if (lstrcmp(pText, marketPrice) == 0)
		m_pConfigData->FollowPriceType = fptMarket;

	m_pConfigData->QtyRate = m_editQtyRate.GetDoubleData();
	int coverTime = m_followCoverTime.GetWindowNumber();
	m_pConfigData->FollowCoverTime = coverTime > 2 ? coverTime : 2; //最小值为2

	m_pConfigData->DirectSameToMain = m_cbDirect.GetSelect() == 0 ? true : false;

	m_pConfigData->ChaseCount = m_spinChaseCount.GetWindowNumber();
	if (m_pConfigData->ChaseCount == 0)
		m_pConfigData->ChaseCount = 1;

	m_pConfigData->bUseCheckPosition = m_ckUseCheckPosition.GetCheck();

	m_pConfigData->CheckPositionTime = m_spinCheckPositionTime.GetWindowNumber();

	if (m_pConfigData->bUseCheckPosition)
	{
		if (m_pConfigData->CheckPositionTime < 1)
			m_pConfigData->CheckPositionTime = 1;
	}
}
