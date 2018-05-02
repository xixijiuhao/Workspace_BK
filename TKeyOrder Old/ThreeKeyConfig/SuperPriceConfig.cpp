#include "PreInclude.h"
extern bool g_MouseTrack;
#define LEFT 10
#define TOP  5
#define RIGHT 130
#define BOTTOM 20
#define WIDGETH 20
#define WIDGETV 120
#define X_VAP  10
#define Y_VAP  5
#define ListViewV 440
#define ListViewH 300
#define path "..\\Debug\\config\\ThreeKeyDefaultQty.pri"


//@String
static char szCHS[][75] = {
	"合约号", "最小变动价", "买超价/最小变动价", "增加", "删除",
	"卖超价/最小变动价", "买超价", "卖超价", "注意：", "1.超价基准价是对手价，在对价的基础上增减N个最小变动价", 
	"2.建议:买入的参数设正值, 卖出的参数设负值", "提示", "不存在该合约"
};
static char szCHT[][75] = {
	"合約號", "最小變動價", "買超價/最小變動價", "增加", "刪除",
	"賣超價/最小變動價", "買超價", "賣超價", "註意：", "1.超價基準價是對手價，在對價的基礎上增減N個最小變動價",
	"2.建議:買入的參數設正值, 賣出的參數設負值", "提示", "不存在該合約"
};
static char szENU[][100] = {
	"ContractNo", "MinChg Price", "Bid Exceed/MinChg Price", "Add", "Delete",
	"Ask Exceed/MinChg Price", "Bid Exceed Price", "Ask Exceed Price", "Attention:", "1.The Exceed Price is based on Counter Price, Add/Min N*MinChg Price.",
	"2.Suggestion:Bid Param set positive,Ask Param set negative", "Tips", "Invalid Contract"
};

static char* _GetStringById(int i)
{
	if (g_language == CHT)
	{
		if (i + 1 > (sizeof(szCHT) / sizeof(szCHT[0])))
			return 0;
		return szCHT[i];
	}
	else if (g_language == ENU)
	{
		if (i + 1 > (sizeof(szENU) / sizeof(szENU[0])))
			return 0;
		return szENU[i];
	}
	else
	{
		if (i + 1 > (sizeof(szCHS) / sizeof(szCHS[0])))
			return 0;
		return szCHS[i];
	}
}

SuperPriceConfig::SuperPriceConfig()
{
	m_bshowCodeList = true;
	m_PriSize = 0;
}
SuperPriceConfig::~SuperPriceConfig()
{

}
extern TGlobalModel* g_pGlobalModel;
void SuperPriceConfig::ShowFrm(HWND hParent, RECT rect, UINT show)
{
	if (!m_Hwnd)
	{
		//deal userno and sign
		CreateFrm(_T("StopLossParamFrm"), hParent, WS_CHILD | WS_CLIPCHILDREN | WS_VISIBLE);
	}
	strcpy_s(m_Sign1, g_pGlobalModel->m_strSign.c_str());
	SetWindowPos(m_Hwnd, 0, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, show);
	m_PopupCodeList.UpdateData(g_pGlobalModel->m_strSign.c_str());
}
//void SuperPriceConfig::Create(HWND hParent, int index)
//{
//	g_pTradeData->GetAllUser(m_clUserContainer);
//	uint count = m_clUserContainer.size();
//	if (count)
//	{
//		const TUserInfo*pUser = (const TUserInfo*)m_clUserContainer.at(0);
//		strcpy_s(m_UserNo, pUser->UserNo);
//		strcpy_s(m_Sign1, pUser->Sign);
//	}
//	CreateFrm(L"SuperPriceConfig", hParent, WS_CHILD | WS_VISIBLE | WS_CHILDWINDOW | WS_CLIPSIBLINGS);
//	SetWindowLongPtr(m_Hwnd, GWL_ID, index);
//}
void SuperPriceConfig::MoveWindow(int x, int y, int cx, int cy)
{
	SetWindowPos(m_Hwnd, 0, x, y, cx, cy, SWP_NOZORDER );
}
void SuperPriceConfig::RegistSt(std::map<std::string, stSuperPrice >* pmapSuperPrice)
{
	m_mapSuperPrice = pmapSuperPrice;
}
void SuperPriceConfig::UpdateConfig()
{
	ShowList();
}
LRESULT SuperPriceConfig::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		OnCreate();
		return PROCESSED;
	case WM_SIZE:
		OnSize(wParam, lParam);
		return PROCESSED;
	case WM_PAINT:
		OnPaint();
		return PROCESSED;
	case WM_NOTIFY:
		return OnNotify(wParam, lParam);
	case WM_COMMAND:
		return OnCommand(wParam, lParam);
	case SSWM_STATIC_BUTTON_CLICKDOWN:
		OnBtnClick(wParam, lParam);
		return PROCESSED;
	case WM_MEASUREITEM:
		OnMeasureListView(lParam);
		return PROCESSED;
	case WM_DRAWITEM:
		OnDrawListView(wParam, lParam);
		return PROCESSED;
	case WM_MOUSEWHEEL:
		OnMouseWheel(wParam, lParam);
		return PROCESSED;
	case SSWM_EDIT_PRESS_ENTER:
		if ((HWND)lParam == m_editContractNo.GetHwnd())
			m_PopupCodeList.OnReturn();
		return PROCESSED;
	case WM_CTLCOLORSTATIC:
		return OnCtrlColor(wParam, lParam);
	default:
		return NOT_PROCESSED;
	}
}

void SuperPriceConfig::OnCreate()
{
	//合约号
	m_labContractNo.Create(m_Hwnd, ANSIToUnicode(_GetStringById(0)).c_str());
	m_labContractNo.MoveWindow(LEFT, TOP, WIDGETV, WIDGETH);

	//合约号TEdit
	RECT rect = { LEFT, BOTTOM + Y_VAP, RIGHT, BOTTOM + Y_VAP + WIDGETH };
	m_editContractNo.Create(m_Hwnd, 0, ES_UPPERCASE, ID_EDITCONTRACTNO);
	m_editContractNo.MoveWindow(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
	m_editContractNo.SetFont(g_FontData.g_FontNum15);
	//合约选择按钮
	rect.left = rect.right + 1;
	rect.right = rect.right + 14;
	m_btnSearch.Create(m_Hwnd, " ", ID_BTNSEARCH);
	m_btnSearch.SetFont(g_FontData.g_FontWord13);
	m_btnSearch.MoveWindow(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
	//合约筛选下拉框
	m_PopupContractCode.CreatePopWin(this, L"popupCode", m_Hwnd, WS_CLIPSIBLINGS, 0);
	ShowWindow(m_PopupContractCode.GetHwnd(), SW_HIDE);
	//合约List控件
	DWORD dwStyle = WS_CLIPSIBLINGS | WS_BORDER | WS_POPUP;
	DWORD dwStyleeEx = WS_EX_RIGHTSCROLLBAR | WS_EX_NOACTIVATE;
	m_PopupCodeList.CreatePopWin(this, L"popupcodeList", m_Hwnd, dwStyle, dwStyleeEx, 0, 0);
	ShowWindow(m_PopupCodeList.GetHwnd(), SW_HIDE);

	//最小变动价
	m_labMinChangePrice.Create(m_Hwnd, ANSIToUnicode(_GetStringById(1)).c_str());
	m_labMinChangePrice.MoveWindow(rect.right + X_VAP, TOP, WIDGETV, WIDGETH);
	ShowWindow(m_labMinChangePrice.GetHwnd(), SW_HIDE);
	//最小变动价Edit
	rect.left = rect.right + X_VAP; rect.right = rect.left + WIDGETV;
	m_editMinChangePrice.Create(m_Hwnd, CxEdit::floatStyle);
	m_editMinChangePrice.SetFont(g_FontData.g_FontNum15);
	m_editMinChangePrice.MoveWindow(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
	ShowWindow(m_editMinChangePrice.GetHwnd(), SW_HIDE);

	//买超价
	rect.left = LEFT; rect.top = rect.bottom; rect.right = rect.left + WIDGETV; rect.bottom = rect.top + WIDGETH;
	m_labLongSuperPrice.Create(m_Hwnd, ANSIToUnicode(_GetStringById(2)).c_str());
	m_labLongSuperPrice.MoveWindow(LEFT, rect.top, WIDGETV, WIDGETH);
	//买超价spin
	rect.left = LEFT; rect.top = rect.bottom; rect.right = rect.left + WIDGETV; rect.bottom = rect.top + WIDGETH;
	m_spinLongSuperPrice.Create(m_Hwnd, CxEdit::intStyle);
	m_spinLongSuperPrice.SetWindowNumber(1);
	m_spinLongSuperPrice.SetRange(-100, 100);
	m_spinLongSuperPrice.MoveWindow(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
	//增加按钮
	rect.left = LEFT;  rect.top = rect.bottom + Y_VAP; rect.bottom = rect.top + WIDGETH;
	m_btnAdd.Create(m_Hwnd, _GetStringById(3), ID_BTNADD);	
	m_btnAdd.MoveWindow(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);

	rect.left = rect.right + X_VAP + 15; rect.right = rect.left + WIDGETV;
	m_btnDelete.Create(m_Hwnd, _GetStringById(4), ID_BTNDELETE);
	m_btnDelete.MoveWindow(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);

	rect.top = rect.top - WIDGETH - Y_VAP; rect.bottom = rect.bottom - WIDGETH - Y_VAP;
	m_spinShortSuperPrice.Create(m_Hwnd);
	m_spinShortSuperPrice.SetWindowNumber(-1);
	m_spinShortSuperPrice.SetRange(-100, 100);
	m_spinShortSuperPrice.MoveWindow(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);

	rect.top = rect.top - WIDGETH; rect.bottom = rect.bottom - WIDGETH;
	m_labShortSuperPrice.Create(m_Hwnd, ANSIToUnicode(_GetStringById(5)).c_str());
	m_labShortSuperPrice.MoveWindow(rect.left, rect.top, WIDGETV, WIDGETH);

	rect.left = rect.right + 3*Y_VAP; rect.right = rect.left + ListViewV; rect.top = TOP;
	rect.bottom = rect.top + ListViewH;
	m_listSuperPrice.CreateList(m_Hwnd, LVS_REPORT | LVS_NOSORTHEADER | LVS_OWNERDRAWFIXED | WS_BORDER, 20, 18, LVS_EX_FULLROWSELECT, ID_LISTDEFAULTQTY);
	m_listSuperPrice.MoveWindow(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
	m_listSuperPrice.AddColumn(ANSIToUnicode(_GetStringById(0)).c_str(), 200);
//	m_listSuperPrice.AddColumn(L"最小变动价", 80);
	m_listSuperPrice.AddColumn(ANSIToUnicode(_GetStringById(6)).c_str(), 80);
	m_listSuperPrice.AddColumn(ANSIToUnicode(_GetStringById(7)).c_str(), 80);

	UpdateConfig();
}


void SuperPriceConfig::OnPaint()
{
	TMemDC memdc(m_Hwnd);
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	FillRect(memdc.GetHdc(), &rect, g_ColorRefData.g_brush_bk);
	SelectFont(memdc.GetHdc(), g_FontData.g_FontWord12);
	SetBkMode(memdc.GetHdc(), TRANSPARENT);
	rect.left = LEFT; rect.right = LEFT + 2 * WIDGETV + 15 + 2*X_VAP;
	rect.top = TOP + 10 * WIDGETH + 3*Y_VAP;
	rect.bottom = rect.top + WIDGETH;
	DrawText(memdc.GetHdc(), ANSIToUnicode(_GetStringById(8)).c_str(), -1, &rect, DT_WORDBREAK);

	rect.top = rect.bottom +  Y_VAP;
	rect.bottom = rect.top + 2 * WIDGETH - 10 ;
	DrawText(memdc.GetHdc(), ANSIToUnicode(_GetStringById(9)).c_str(), -1, &rect, DT_WORDBREAK);

	rect.top = rect.bottom;
	rect.bottom = rect.top + 2 * WIDGETH;
	DrawText(memdc.GetHdc(), ANSIToUnicode(_GetStringById(10)).c_str(), -1, &rect, DT_WORDBREAK);

}
void SuperPriceConfig::OnSize(WPARAM wParam, LPARAM lParam)
{
	
}
LRESULT SuperPriceConfig::OnNotify(WPARAM wParam, LPARAM lParam)
{
	LPNMHDR p = (LPNMHDR)lParam;
	if (p->hwndFrom == m_listSuperPrice.GetListHwnd() && p->code == NM_CLICK && m_listSuperPrice.GetSelectItem() != -1)
	{
		int index = m_listSuperPrice.GetSelectItem();
		if (m_listSuperPrice.GetSelectItem() < m_listSuperPrice.GetItemCount())
		{
			stSuperPrice st;
			if (!FindItem(index, st))
				return PROCESSED;
			SetContent(st);
		}
		return PROCESSED;
	}
	return NOT_PROCESSED;
}
LRESULT SuperPriceConfig::OnCommand(WPARAM wParam, LPARAM lParam)
{
	
	if ((HWND)lParam == m_editContractNo.GetHwnd())
	{
		if (m_bshowCodeList &&  HIWORD(wParam) == EN_CHANGE)
		{
			string sCode = "";
			GetWindowTextAnsic(m_editContractNo.GetHwnd(), sCode);
			POINT pt = { 10, 45 };
			ClientToScreen(m_Hwnd, &pt);
			if (!IsWindowVisible(m_PopupCodeList.GetHwnd()))
			{
				SetWindowPos(m_PopupCodeList.GetHwnd(), 0, pt.x, pt.y, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
			}
			m_PopupCodeList.EditChange(sCode, m_Sign1);
		}
		else if (HIWORD(wParam) == EN_KILLFOCUS)
		{
			if (IsWindowVisible(m_PopupCodeList.GetHwnd()) && g_MouseTrack)
				ShowWindow(m_PopupCodeList.GetHwnd(), SW_HIDE);
		}
	}
	return NOT_PROCESSED;
}
void SuperPriceConfig::OnBtnClick(WPARAM wParam, LPARAM lParam)
{
	switch (lParam)
	{
	case ID_BTNSEARCH:
		OnSearchCode();
		break;
	case ID_BTNADD:
		OnBtnAdd();
		break;
	case ID_BTNDELETE:
		OnBtnDelete();
		break;
	}
}
void SuperPriceConfig::OnMouseWheel(WPARAM wParam, LPARAM lParam)
{
	UINT id = GetWindowLong(GetFocus(), GWL_ID);
	if (id == ID_EDITCONTRACTNO)
	{
		if (!IsWindowVisible(m_PopupCodeList.GetHwnd()))
			return;
		int nDelta = GET_WHEEL_DELTA_WPARAM(wParam);
		if (nDelta > 0)
			m_PopupCodeList.DelNum();
		else
			m_PopupCodeList.AddNum();
	}
}
void SuperPriceConfig::OnSearchCode()
{
	POINT pt = { 10, 45 };
	ClientToScreen(m_Hwnd, &pt);
	m_PopupContractCode.UpdateData(m_Sign1);
	SetWindowPos(m_PopupContractCode.GetHwnd(), 0, pt.x, pt.y, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
}
void SuperPriceConfig::OnBtnAdd()
{
	InsertMap();
	ShowList();
}
void SuperPriceConfig::OnBtnDelete()
{
	DeleteMap();
	ShowList();
}
void SuperPriceConfig::ShowList()
{
	m_listSuperPrice.DeleteAllItems();
	std::map<std::string, stSuperPrice>::iterator iter;
	std::string ContractCode;
	int iRow = 0;
	int iCol = 0;
	for (iter = m_mapSuperPrice->begin(); iter != m_mapSuperPrice->end(); iter++)
	{
		iCol = 0;
		if (!CConvert::TContractKeyToTContractNo(iter->second.stContract, ContractCode))
			return;
		m_listSuperPrice.AddItem(ContractCode.c_str(), iRow, iCol++);
		//m_listSuperPrice.AddItem(iter->second.dMinChangePrice, iRow, iCol++);
		m_listSuperPrice.AddItem(iter->second.iLongSuperPrice, iRow, iCol++);
		m_listSuperPrice.AddItem(iter->second.iShortSuperPrice, iRow, iCol++);
		iRow++;
	}
}
void SuperPriceConfig::OnMeasureListView(LPARAM lParam)
{
	LPMEASUREITEMSTRUCT pmi = LPMEASUREITEMSTRUCT(lParam);
	if (pmi->CtlType == ODT_LISTVIEW && pmi->CtlID == ID_LISTDEFAULTQTY)
		m_listSuperPrice.OnMeasureItem(pmi->itemHeight);
}
void SuperPriceConfig::OnDrawListView(WPARAM wParam, LPARAM lParam)
{
	LPDRAWITEMSTRUCT pDIS = LPDRAWITEMSTRUCT(lParam);
	if (pDIS->CtlType == ODT_LISTVIEW && pDIS->CtlID == ID_LISTDEFAULTQTY)
		m_listSuperPrice.OnDrawItem(lParam);
}
void SuperPriceConfig::SetContractNo(string sCode)
{
	m_bshowCodeList = false;
	m_sCode = sCode;
	m_editContractNo.SetText(m_sCode.c_str());
	EnableWindow(m_editMinChangePrice.GetHwnd(), true);
	TContractKey stContract;
	memset(&stContract, 0, sizeof(stContract));
	memcpy(&stContract.Sign, g_pGlobalModel->m_strSign.c_str(), sizeof(stContract.Sign));
	if (!CConvert::TContractNoToTContractKey(sCode.c_str(), stContract))
		return;
	//GetMinChangePrice(stContract);
	m_spinLongSuperPrice.SetWindowNumber(1);
	m_spinShortSuperPrice.SetWindowNumber(-1);
	m_bshowCodeList = true;
}
bool SuperPriceConfig::GetContract(TContractKey& ContractKey)
{
	memcpy(ContractKey.Sign, m_Sign1, sizeof(m_Sign1));
	return CConvert::TContractNoToTContractKey(m_sCode.c_str(), ContractKey);
}
void SuperPriceConfig::InsertMap()
{
	stSuperPrice stSuper;
	memset(&stSuper.stContract, 0, sizeof(stSuper.stContract));
	stSuper.iLongSuperPrice = m_spinLongSuperPrice.GetWindowNumber();
	stSuper.iShortSuperPrice = m_spinShortSuperPrice.GetWindowNumber();
	stSuper.dMinChangePrice = m_editMinChangePrice.GetDoubleData();

	if (!GetContract(stSuper.stContract))
		return;
	char cContractKey[150] = { 0 };
	GetContractKey(cContractKey, stSuper.stContract);
	std::map<std::string, stSuperPrice>::iterator iter = m_mapSuperPrice->find(cContractKey);
	if (iter != m_mapSuperPrice->end())
	{
		m_mapSuperPrice->erase(cContractKey);
		m_mapSuperPrice->insert(std::make_pair(cContractKey, stSuper));
	}
	else
		m_mapSuperPrice->insert(std::make_pair(cContractKey, stSuper));

}
void SuperPriceConfig::DeleteMap()
{
	TContractKey stContractKey;
	memset(&stContractKey, 0, sizeof(stContractKey));
	if (!GetContract(stContractKey))
		return;
	char cContractKey[150] = { 0 };
	GetContractKey(cContractKey, stContractKey);

	std::map<std::string, stSuperPrice>::iterator iter = m_mapSuperPrice->find(cContractKey);
	if (iter != m_mapSuperPrice->end())
		m_mapSuperPrice->erase(cContractKey);
	else
	{
		_TMessageBox_Domodal(m_Hwnd, _GetStringById(11), ANSIToUnicode(_GetStringById(12)).c_str(), TMB_OK);

		return;
	}
}
bool SuperPriceConfig::FindItem(int index, stSuperPrice& st)
{
	if (index < 0)
		return false;
	std::map<std::string, stSuperPrice >::iterator iter;
	int i = 0;
	for (iter = m_mapSuperPrice->begin(); iter != m_mapSuperPrice->end(); iter++)
	{
		if (i == index)
		{
			memcpy(&st, &iter->second, sizeof(stSuperPrice));
			return true;
		}
		i++;
	}
	return true;
}
void SuperPriceConfig::SetContent(stSuperPrice st)
{
	m_bshowCodeList = false;
	std::string ContractCode;
	if (!CConvert::TContractKeyToTContractNo(st.stContract, ContractCode))
		return;
	m_sCode = ContractCode;
	m_editContractNo.SetText(ContractCode.c_str());
	m_spinLongSuperPrice.SetWindowNumber(st.iLongSuperPrice);
	m_spinShortSuperPrice.SetWindowNumber(st.iShortSuperPrice);
	//m_editMinChangePrice.SetDoubleData(st.dMinChangePrice);
	m_bshowCodeList = true;
}
void SuperPriceConfig::GetContractKey(char* cContractKey, TContractSingleKey stContract)
{
	sprintf_s(cContractKey, 150, "%s%c%s%s%s%c", 
		stContract.ExchangeNo, stContract.CommodityType, stContract.CommodityNo,
		stContract.ContractNo, stContract.StrikePrice, stContract.OptionType);
}
void SuperPriceConfig::GetPriKey(char* cContractKey, stSuperPrice stQty)
{
	sprintf_s(cContractKey, 150, "%s|%c|%s|%s|%s|%c|%s|%s|%c,%d,%d,%f",
		stQty.stContract.ExchangeNo, stQty.stContract.CommodityType, stQty.stContract.CommodityNo,
		stQty.stContract.ContractNo, stQty.stContract.StrikePrice, stQty.stContract.OptionType,
		stQty.stContract.ContractNo2, stQty.stContract.StrikePrice2, stQty.stContract.OptionType2, stQty.iLongSuperPrice, stQty.iShortSuperPrice, stQty.dMinChangePrice);
}

bool SuperPriceConfig::GetMinChangePrice(TContractKey& key)
{
	const SCommodity* pstCommodityInfo = g_pTradeData->GetCommodity(&key);
	if (!pstCommodityInfo)
		return false;

	m_editMinChangePrice.SetDoubleData(pstCommodityInfo->PriceTick);
	EnableWindow(m_editMinChangePrice.GetHwnd(), false);
	return true;
}