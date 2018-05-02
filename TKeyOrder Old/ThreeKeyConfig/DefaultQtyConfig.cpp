#include "PreInclude.h"
extern bool g_MouseTrack;
#define FIRST_LEFT 10
#define FIRST_RIGHT 140
#define FIRST_TOP   5
#define FIRST_BOTTON 25

//@String
static char szCHS[][50] = {
	"合约号", "默认手数", "增加", "删除", "提示", "委托数量不能为0", "不存在该合约"
};
static char szCHT[][50] = {
	"合約號", "默認手數", "增加", "刪除", "提示", "委托數量不能為0", "不存在該合約"
};
static char szENU[][50] = {
	"ContractNo", "Confirm Volume", "Add", "Delete", "Tips", "Order Quantity cannot be zero", "Invalid Contract"
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

DefaultQtyConfig::DefaultQtyConfig()
{
	m_bshowCodeList = true;
	m_PriSize = 0;
}
DefaultQtyConfig::~DefaultQtyConfig()
{

}

void DefaultQtyConfig::ShowFrm(HWND hParent, RECT rect, UINT show)
{
	if (!m_Hwnd)
	{
		//deal userno and sign
		CreateFrm(_T("DefaultQtyConfig"), hParent, WS_CHILD | WS_CLIPCHILDREN | WS_VISIBLE);
	}
	strcpy_s(m_Sign1, g_pGlobalModel->m_strSign.c_str());
	SetWindowPos(m_Hwnd, 0, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, show);
}

//void DefaultQtyConfig::Create(HWND hParent, int index)
//{
//	g_pTradeData->GetAllUser(m_clUserContainer);
//	uint count = m_clUserContainer.size();
//	if (count)
//	{
//		const TUserInfo*pUser = (const TUserInfo*)m_clUserContainer.at(0);
//		strcpy_s(m_UserNo, pUser->UserNo);
//		strcpy_s(m_Sign1, pUser->Sign);
//	}
//	CreateFrm(L"DefaultQtyConfig", hParent, WS_CHILD | WS_VISIBLE | WS_CHILDWINDOW | WS_CLIPSIBLINGS);
//	SetWindowLongPtr(m_Hwnd, GWL_ID, index);
//}
void DefaultQtyConfig::MoveWindow(int x, int y, int cx, int cy)
{
	SetWindowPos(m_Hwnd, 0, x, y, cx, cy, SWP_NOZORDER);
}
void DefaultQtyConfig::RegistSt(std::map<std::string, stDefaultQty>* pmapDefaultQty)
{
	m_mapDefaultQty = pmapDefaultQty;
}
void DefaultQtyConfig::UpdateConfig()
{
	ShowList();
}

LRESULT DefaultQtyConfig::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
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
void DefaultQtyConfig::OnPaint()
{
	TMemDC memdc(m_Hwnd);
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	FillRect(memdc.GetHdc(), &rect, g_ColorRefData.g_brush_bk);
}
void DefaultQtyConfig::OnCreate()
{
	//合约号Edit
	m_labelContractNo.Create(m_Hwnd,ANSIToUnicode(_GetStringById(0)).c_str());
	m_labelContractNo.MoveWindow(10, 5, 50, 25);
	RECT rect = { 10, 30, 130, 50 };

	m_editContractNo.Create(m_Hwnd, 0, ES_UPPERCASE, ID_EDITCONTRACTNO);
	m_editContractNo.SetFont(g_FontData.g_FontNum15);
	m_editContractNo.MoveWindow(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);

	//默认手数
	m_labelDefaultQty.Create(m_Hwnd, ANSIToUnicode(_GetStringById(1)).c_str());
	m_labelDefaultQty.MoveWindow(155, 5, 70, 25);

	//合约选择按钮
	rect.left = 131; rect.right = 145;
	
	m_btnSearch.Create(m_Hwnd, " ", ID_BTNSEARCH);
	m_btnSearch.SetFont(g_FontData.g_FontNum13);
	m_btnSearch.MoveWindow(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
	//合约筛选下拉框
	m_PopupContractCode.CreatePopWin(this, L"popupCode", m_Hwnd, WS_CLIPSIBLINGS, 0);
	ShowWindow(m_PopupContractCode.GetHwnd(), SW_HIDE);
	//合约List控件
	DWORD dwStyle = WS_CLIPSIBLINGS | WS_BORDER | WS_POPUP;
	DWORD dwStyleeEx = WS_EX_RIGHTSCROLLBAR | WS_EX_NOACTIVATE;
	m_PopupCodeList.CreatePopWin(this, L"popupcodeList", m_Hwnd, dwStyle, dwStyleeEx, 0, 0);
	m_PopupCodeList.UpdateData(g_pGlobalModel->m_strSign.c_str());
	ShowWindow(m_PopupContractCode.GetHwnd(), SW_HIDE);

	rect.left = 155; rect.top = 30; rect.right = 275; rect.bottom = 50;
	m_spinDefaultQty.Create(m_Hwnd, ES_NUMBER);
	m_spinDefaultQty.SetWindowNumber(1);
	m_spinDefaultQty.MoveWindow(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);

	rect.left = 10; rect.top = 55; rect.right = 145; rect.bottom = 75;
	m_btnAdd.Create(m_Hwnd, _GetStringById(2), ID_BTNADD);
	m_btnAdd.SetFont(g_FontData.g_FontWord12);
	m_btnAdd.MoveWindow(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);

	rect.left = 155; rect.right = 275;
	m_btnDelete.Create(m_Hwnd, _GetStringById(3), ID_BTNDELETE);
	m_btnDelete.SetFont(g_FontData.g_FontWord12);
	m_btnDelete.MoveWindow(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);

	m_listDefaultQty.CreateList(m_Hwnd, LVS_REPORT | LVS_NOSORTHEADER | LVS_OWNERDRAWFIXED | WS_BORDER, 20, 18, LVS_EX_FULLROWSELECT, ID_LISTDEFAULTQTY);
	m_listDefaultQty.MoveWindow(10, 80, 265, 258);
	m_listDefaultQty.AddColumn(ANSIToUnicode(_GetStringById(0)).c_str(), 198);
	m_listDefaultQty.AddColumn(ANSIToUnicode(_GetStringById(1)).c_str(), 65);

	ShowList();
}
void DefaultQtyConfig::OnSize(WPARAM wParam, LPARAM lParam)
{
}
LRESULT DefaultQtyConfig::OnNotify(WPARAM wParam, LPARAM lParam)
{
	LPNMHDR p = (LPNMHDR)lParam;
	if (p->hwndFrom == m_listDefaultQty.GetListHwnd() && p->code == NM_CLICK && m_listDefaultQty.GetSelectItem() != -1)
	{
		int index = m_listDefaultQty.GetSelectItem();
		if (m_listDefaultQty.GetSelectItem() < m_listDefaultQty.GetItemCount())
		{
			stDefaultQty st;
			if (!FindItem(index, st))
				return PROCESSED;
			SetContent(st);	
		}
		return PROCESSED;
	}
	return NOT_PROCESSED;
}
LRESULT DefaultQtyConfig::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if ((HWND)lParam == m_editContractNo.GetHwnd())
	{
		if (m_bshowCodeList &&  HIWORD(wParam) == EN_CHANGE)
		{
			string sCode = "";

			GetWindowTextAnsic(m_editContractNo.GetHwnd(), sCode);
			POINT pt = { 10, 50 };
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
void DefaultQtyConfig::OnBtnClick(WPARAM wParam, LPARAM lParam)
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
void DefaultQtyConfig::OnMouseWheel(WPARAM wParam, LPARAM lParam)
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
void DefaultQtyConfig::OnSearchCode()
{
	POINT pt = { 10, 50 };
	ClientToScreen(m_Hwnd, &pt);
	m_PopupContractCode.UpdateData(m_Sign1);
	SetWindowPos(m_PopupContractCode.GetHwnd(), 0, pt.x, pt.y, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
}
void DefaultQtyConfig::OnBtnAdd()
{
	InsertMap();
	ShowList();
}
void DefaultQtyConfig::OnBtnDelete()
{
	DeleteMap();
	ShowList();
}
void  DefaultQtyConfig::SetContractNo(string sCode)
{
	m_bshowCodeList = false;
	m_sCode = sCode;
	m_editContractNo.SetText(m_sCode.c_str());
	m_spinDefaultQty.SetWindowNumber(1);
	m_bshowCodeList = true;
}
void DefaultQtyConfig::OnMeasureListView(LPARAM lParam)
{
	LPMEASUREITEMSTRUCT pmi = LPMEASUREITEMSTRUCT(lParam);
	if (pmi->CtlType == ODT_LISTVIEW && pmi->CtlID == ID_LISTDEFAULTQTY)
		m_listDefaultQty.OnMeasureItem(pmi->itemHeight);
}
void DefaultQtyConfig::OnDrawListView(WPARAM wParam, LPARAM lParam)
{
	LPDRAWITEMSTRUCT pDIS = LPDRAWITEMSTRUCT(lParam);
	if (pDIS->CtlType == ODT_LISTVIEW && pDIS->CtlID == ID_LISTDEFAULTQTY)
		m_listDefaultQty.OnDrawItem(lParam);
}

bool DefaultQtyConfig::GetContract(TContractKey& ContractKey)
{
	memcpy(ContractKey.Sign, m_Sign1, sizeof(m_Sign1));
	return CConvert::TContractNoToTContractKey(m_sCode.c_str(), ContractKey);
}
void DefaultQtyConfig::InsertMap()
{
	stDefaultQty   stQty;
	memset(&stQty.stContract, 0, sizeof(stQty.stContract));
	char cDefaultQty[10] = { 0 };
	m_spinDefaultQty.GetText(cDefaultQty, sizeof(DefaultQty));
	int iQty = atof(cDefaultQty);
	if (iQty <= 0)
	{
		_TMessageBox_Domodal(m_Hwnd, _GetStringById(4), ANSIToUnicode(_GetStringById(5)).c_str(), TMB_OK);
		return;
	}
	if (!GetContract(stQty.stContract))
		return;
	stQty.iQty = iQty;
	char cContractKey[150] = { 0 };
	GetContractKey(cContractKey, stQty.stContract);
	std::map<std::string, stDefaultQty>::iterator iter = m_mapDefaultQty->find(cContractKey);
	if (iter != m_mapDefaultQty->end())
	{
		m_mapDefaultQty->erase(cContractKey);
		m_mapDefaultQty->insert(std::make_pair(cContractKey, stQty));
	}
	else
		m_mapDefaultQty->insert(std::make_pair(cContractKey, stQty));
}
void DefaultQtyConfig::DeleteMap()
{
	TContractKey stContractKey;
	memset(&stContractKey, 0, sizeof(stContractKey));
	if (!GetContract(stContractKey))
		return;
	char cContractKey[150] = { 0 };
	GetContractKey(cContractKey, stContractKey);

	std::map<std::string, stDefaultQty>::iterator iter = m_mapDefaultQty->find(cContractKey);
	if (iter != m_mapDefaultQty->end())
		m_mapDefaultQty->erase(cContractKey);
	else
	{
		_TMessageBox_Domodal(m_Hwnd, _GetStringById(4), ANSIToUnicode(_GetStringById(6)).c_str(), TMB_OK);
		return;
	}
}
void DefaultQtyConfig::ShowList()
{
	m_listDefaultQty.DeleteAllItems();
	std::map<std::string, stDefaultQty>::iterator iter;
	std::string ContractCode;
	int iRow = 0;
	int iCol = 0;
	for (iter = m_mapDefaultQty->begin(); iter != m_mapDefaultQty->end(); iter++)
	{
		iCol = 0;
		if (!CConvert::TContractKeyToTContractNo(iter->second.stContract, ContractCode))
			return;
		m_listDefaultQty.AddItem(ContractCode.c_str(), iRow, iCol++);
		m_listDefaultQty.AddItem(iter->second.iQty, iRow, iCol++);
		iRow++;
	}
}
bool DefaultQtyConfig::FindItem(int index, stDefaultQty& st)
{
	if (index < 0)
		return false;
	std::map<std::string, stDefaultQty >::iterator iter;
	int i = 0;
	for (iter = m_mapDefaultQty->begin(); iter != m_mapDefaultQty->end(); iter++)
	{
		if (i == index)
		{
			memcpy(&st, &iter->second, sizeof(stDefaultQty));
			return true;
		}
		i++;
	}
	return true;
}
void DefaultQtyConfig::SetContent(stDefaultQty st)
{
	m_bshowCodeList = false;
	std::string ContractCode;
	if (!CConvert::TContractKeyToTContractNo(st.stContract, ContractCode))
		return;
	m_sCode = ContractCode;
	m_editContractNo.SetText(ContractCode.c_str());
	m_spinDefaultQty.SetWindowNumber(st.iQty);
	m_bshowCodeList = true;
}

void DefaultQtyConfig::GetContractKey(char* cContractKey, TContractSingleKey stContract)
{
	sprintf_s(cContractKey,150, "%s%c%s%s%s%c",
		stContract.ExchangeNo, stContract.CommodityType, stContract.CommodityNo,
		stContract.ContractNo, stContract.StrikePrice, stContract.OptionType);
}
void DefaultQtyConfig::GetPriKey(char* cContractKey, stDefaultQty stQty)
{
	sprintf_s(cContractKey,150, "%s|%c|%s|%s|%s|%c|%s|%s|%c,%d",
		stQty.stContract.ExchangeNo, stQty.stContract.CommodityType, stQty.stContract.CommodityNo,
		stQty.stContract.ContractNo, stQty.stContract.StrikePrice, stQty.stContract.OptionType,
		stQty.stContract.ContractNo2, stQty.stContract.StrikePrice2, stQty.stContract.OptionType2, stQty.iQty);
}