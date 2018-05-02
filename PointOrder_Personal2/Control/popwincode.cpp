#include "PreInclude.h"
//色彩
extern G_COLOREF g_ColorRefData;
extern G_UIFONT g_FontData;
using namespace std;
enum eSeleType
{
	ExchangeSele_Type = 0,
	CommoditySele_Type,
	ContractSele_Type

};

const char* FIRST_EXCHANGE_ARRAY[] = {
    "ZCE", "DCE", "SHFE","INE", "CFFEX", "SSE", "SZSE", "SGE",
    "CBOT", "CME", "COMEX", "NYMEX",
    "ICUS", "ICEU", "HKEX", "LME",
    "EUREX", "SGX", "TOCOM"
};

extern ITradeApi *g_pTradeApi;
extern ITradeData *g_pTradeData;
extern IStarApi *g_pStarApi;

static void ProcessMessages()
{
	MSG  msg;
	if (PeekMessage(&msg, 0, 0, 0, PM_NOREMOVE))
	{
		if (!PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
			return;
		if (msg.message == WM_QUIT)
			PostMessage(nullptr, -1, 0, 0);
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}
LRESULT CALLBACK ListViewProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int id = GetWindowLong(hwnd, GWL_ID);
	CPopWinCode *pWinCode = (CPopWinCode*)GetWindowLong(hwnd, GWL_USERDATA);
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
				if (pWinCode->m_sel_row != temp.iItem || pWinCode->m_sel_col != temp.iSubItem)
				{
					RECT r;
					//原位置刷新
					if (pWinCode->m_sel_col == 0)
						ListView_GetItemRect(hwnd, pWinCode->m_sel_row, &r, LVIR_BOUNDS);
					else
						ListView_GetSubItemRect(hwnd, pWinCode->m_sel_row, pWinCode->m_sel_col, LVIR_BOUNDS, &r);
					InvalidateRect(hwnd, &r, TRUE);

					pWinCode->m_sel_row = temp.iItem;
					pWinCode->m_sel_col = temp.iSubItem;

					//新位置刷新
					if (pWinCode->m_sel_col == 0)
						ListView_GetItemRect(hwnd, pWinCode->m_sel_row, &r, LVIR_BOUNDS);
					else
						ListView_GetSubItemRect(hwnd, pWinCode->m_sel_row, pWinCode->m_sel_col, LVIR_BOUNDS, &r);
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

CPopWinCode::CPopWinCode() 
: m_opr(NULL),m_nWidth(0)
, m_nHeight(0)
, m_nUintx(N_CODE_WIDTH)
, m_nUinty(N_CODE_HEIGHT)
, m_bIniCompleted(false)
{
	//char path_buffer[_MAX_PATH];
	//GetModuleFileNameA(NULL/*hInst*/, path_buffer, sizeof(path_buffer));
	//char drive[_MAX_DRIVE];
	//char dir[_MAX_DIR];
	//char fname[_MAX_FNAME];
	//char ext[_MAX_EXT];
	//_splitpath_s(path_buffer, drive, dir, fname, ext);
	//string configFilePath;
	//configFilePath = drive;
	//configFilePath = configFilePath + dir + "config\\TradeApi\\ForeignTradeApi.Default.pub";

	//m_Show_LME_Contract = GetPrivateProfileIntA("Code", "LMECodeShow", 0, configFilePath.c_str()) ? true : false;
}

CPopWinCode::~CPopWinCode()
{
	DelLParamPtr();
	if (IsWindow(m_Hwnd))
		DestroyWindow(m_Hwnd);
}

void CPopWinCode::CreatePopWin(IOperator* opr,LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle, int nleft, int ntop, UINT nWidth, UINT nrow)
{
	m_nWidth = nWidth;
	m_nRow = nrow;
	m_nLeft = nleft;
	m_nTop = ntop;
	m_opr  = opr;
	CreateFrm(classname, parent, style | WS_POPUP | WS_BORDER, exstyle);
}

bool CPopWinCode::GetCurOption(string & str)
{
	str = m_sExCom;
	return true;
}

void CPopWinCode::SelectCode()
{
	if (m_opr)
		m_opr->SetContractNo(m_sCode.c_str());
	
	ShowWindow(m_Hwnd,SW_HIDE);
}
//加上这个为了排序 
enum eCommodityType
{
	Goods_Type = 'A',
	Defer_Type,
	Future_Type,
	SpreadMonth_Type,
	SpreadCommodity,
	Option_Type,
	BUL_Type,
	BER_Type,
	STD_Type,
	STG_Type
};

void CPopWinCode::CommodityAndKey::UpdateKey()
{
    TCommodityKey tkey;
    CConvert::SContractNoTypeToTradeCommodity("", pCommodity->CommodityNo, tkey);

    if (ctGoods == tkey.CommodityType)
        sprintf_s(sKey, "%s$%c", tkey.CommodityNo, Goods_Type);
    else if (ctDefer == tkey.CommodityType)
        sprintf_s(sKey, "%s$%c", tkey.CommodityNo, Defer_Type);
    else if (ctFutures == tkey.CommodityType)
        sprintf_s(sKey, "%s$%c", tkey.CommodityNo, Future_Type);
    else if (ctSpreadMonth == tkey.CommodityType)
        sprintf_s(sKey, "%s$%c", tkey.CommodityNo, SpreadMonth_Type);
    else if (ctSpreadCommodity == tkey.CommodityType)
        sprintf_s(sKey, "%s$%c", tkey.CommodityNo, SpreadCommodity);
    else if (ctOption == tkey.CommodityType)
        sprintf_s(sKey, "%s$%c", tkey.CommodityNo, Option_Type);
    else if (ctBUL == tkey.CommodityType)
        sprintf_s(sKey, "%s$%c", tkey.CommodityNo, BUL_Type);
    else if (ctBER == tkey.CommodityType)
        sprintf_s(sKey, "%s$%c", tkey.CommodityNo, BER_Type);
    else if (ctSTD == tkey.CommodityType)
        sprintf_s(sKey, "%s$%c", tkey.CommodityNo, STD_Type);
    else if (ctSTG == tkey.CommodityType)
        sprintf_s(sKey, "%s$%c", tkey.CommodityNo, STG_Type);
}

bool CPopWinCode::IsOptionType(char CommodityType)
{
	return CommodityType == ctOption;
}

bool CPopWinCode::IsOpthinCmbType(char CommodityType)
{
	return ctBUL == CommodityType || ctBER == CommodityType || ctSTD == CommodityType || ctSTG == CommodityType;
}

bool CPopWinCode::IsSpreadType(char CommodityType)
{
	return ctSpreadMonth == CommodityType || ctSpreadCommodity == CommodityType;
}

bool CPopWinCode::IsLMEFutures(const char *szExchange, const char *szSign)
{
	if (m_CurVarietyType == ctFutures)
	{
        return strcmp(szExchange, "LME") == 0;
	}
	return false;
}

void CPopWinCode::UpdateListCtrl(uint nrow)
{
	ClearListCtrl();
	int nxO = 85;
	int nxF = 170;
	
	if (IsOptionType(m_CurVarietyType))
	{
		if (nrow > LINE_VALID)
		{
			nxO -= GetSystemMetrics(SM_CXVSCROLL) / 2;
			nxF -= GetSystemMetrics(SM_CXVSCROLL);
		}
		LONG lStyle;
		lStyle = GetWindowLong(m_hListView, GWL_STYLE);//获取当前窗口style  
		lStyle |= LVS_NOCOLUMNHEADER;//lStyle &= ~LVS_NOCOLUMNHEADER; //lStyle |= LVS_NOCOLUMNHEADER; //设置style lStyle &= ~LVS_NOCOLUMNHEADER; //清除列头隐藏
		SetWindowLong(m_hListView, GWL_STYLE, lStyle);

		LV_COLUMN LvCol;
		memset(&LvCol, 0, sizeof(LvCol));							// Zero Members
		LvCol.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;				// Type of mask
		LvCol.fmt = LVCFMT_CENTER;
		
		LvCol.pszText = TEXT("0");
		LvCol.cx = 0;												
		ListView_InsertColumn(m_hListView, 0, &LvCol);
		LvCol.pszText = TEXT("Call");
		LvCol.cx = nxO;
		ListView_InsertColumn(m_hListView, 1, &LvCol);
		LvCol.pszText = TEXT("Put");
		LvCol.cx = nxO;
		ListView_InsertColumn(m_hListView, 2, &LvCol);
	}
	else if (IsSpreadType(m_CurVarietyType)|| IsOpthinCmbType(m_CurVarietyType))
	{
		LONG lStyle;
		lStyle = GetWindowLong(m_hListView, GWL_STYLE);//获取当前窗口style  
		lStyle |= LVS_NOCOLUMNHEADER; //设置style  
		SetWindowLong(m_hListView, GWL_STYLE, lStyle);

		LV_COLUMN LvCol;
		memset(&LvCol, 0, sizeof(LvCol));							// Zero Members
		LvCol.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;				// Type of mask
		LvCol.fmt = LVCFMT_CENTER;

		LvCol.pszText = L"";										
		LvCol.cx = 0;												// width of column
		ListView_InsertColumn(m_hListView, 0, &LvCol);
		LvCol.cx = nxF;
		ListView_InsertColumn(m_hListView, 1, &LvCol);	
	}
	else
	{
		if (nrow <= LINE_VALID)
		{
			LONG lStyle;
			lStyle = GetWindowLong(m_hListView, GWL_STYLE);//获取当前窗口style  
			lStyle |= LVS_NOCOLUMNHEADER; //设置style  
			SetWindowLong(m_hListView, GWL_STYLE, lStyle);

			LV_COLUMN LvCol;
			memset(&LvCol, 0, sizeof(LvCol));							// Zero Members
			LvCol.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;				// Type of mask
			LvCol.fmt = LVCFMT_CENTER;

			LvCol.pszText = L"";
			LvCol.cx = 0;												// width of column
			ListView_InsertColumn(m_hListView, 0, &LvCol);
			LvCol.cx = nxF;
			ListView_InsertColumn(m_hListView, 1, &LvCol);
		}
		else if (nrow > LINE_VALID && nrow <= LINE_VALID*2)
		{
			LONG lStyle;
			lStyle = GetWindowLong(m_hListView, GWL_STYLE);//获取当前窗口style  
			lStyle |= LVS_NOCOLUMNHEADER; //设置style lStyle &= ~LVS_NOCOLUMNHEADER; //清除列头隐藏
			SetWindowLong(m_hListView, GWL_STYLE, lStyle);

			LV_COLUMN LvCol;
			memset(&LvCol, 0, sizeof(LvCol));							// Zero Members
			LvCol.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;				// Type of mask
			LvCol.fmt = LVCFMT_CENTER;

			LvCol.pszText = TEXT("0");
			LvCol.cx = 0;
			ListView_InsertColumn(m_hListView, 0, &LvCol);
			LvCol.pszText = L"";
			LvCol.cx = nxO;
			ListView_InsertColumn(m_hListView, 1, &LvCol);
			LvCol.pszText = L"";
			LvCol.cx = nxO;
			ListView_InsertColumn(m_hListView, 2, &LvCol);
		}
		else
		{
			nxO -= GetSystemMetrics(SM_CXVSCROLL) / 2;
			nxF -= GetSystemMetrics(SM_CXVSCROLL);
			LONG lStyle;
			lStyle = GetWindowLong(m_hListView, GWL_STYLE);//获取当前窗口style  
			lStyle |= LVS_NOCOLUMNHEADER; //设置style lStyle &= ~LVS_NOCOLUMNHEADER; //清除列头隐藏
			SetWindowLong(m_hListView, GWL_STYLE, lStyle);

			LV_COLUMN LvCol;
			memset(&LvCol, 0, sizeof(LvCol));							// Zero Members
			LvCol.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;				// Type of mask
			LvCol.fmt = LVCFMT_CENTER;

			LvCol.pszText = TEXT("0");
			LvCol.cx = 0;
			ListView_InsertColumn(m_hListView, 0, &LvCol);
			LvCol.pszText = L"";
			LvCol.cx = nxO;
			ListView_InsertColumn(m_hListView, 1, &LvCol);
			LvCol.pszText = L"";
			LvCol.cx = nxO;
			ListView_InsertColumn(m_hListView, 2, &LvCol);
		}
		
	}

}

void CPopWinCode::DelLParamPtr()
{
	if (::IsWindow(m_hTreeView))
	{
		HTREEITEM   hRoot = TreeView_GetRoot(m_hTreeView);
		HTREEITEM hParent = TreeView_GetParent(m_hTreeView,hRoot);
		TravelItem(hParent);
	}
}

char* CPopWinCode::GetParam(const char *szInput, char CommodityType)
{
	//约定格式，通过约定格式知道单击的项是交易所、类型、品种或者合约
	char str[50];
	if ('\0' != CommodityType)
		sprintf_s(str,"%s %c", szInput, CommodityType);
	else
		sprintf_s(str, "%s", szInput);
	char *pNew = new char[strlen(str) + 1];
	strncpy_s(pNew, strlen(str) + 1, str, _TRUNCATE);
	return pNew;
}

void CPopWinCode::TravelItem( HTREEITEM hItem)
{
	HTREEITEM hCurItem = TreeView_GetChild(m_hTreeView,hItem);
	HTREEITEM hNextItem;
	while (hCurItem)
	{
		{
			TVITEM item;
			memset(&item, 0, sizeof(item));
			item.hItem = hCurItem;
			item.mask = /*TVIF_TEXT | TVIF_HANDLE |*/ TVIF_PARAM;
			char a[101] = { 0 };
			if (NULL != ((char *)item.lParam))
			{
				char *pParam = (char *)item.lParam;
				delete[] pParam;
			}
		}
		hNextItem = hCurItem;
		TravelItem( hNextItem);
		hCurItem = TreeView_GetNextSibling(m_hTreeView,hCurItem);
	}
}

bool CPopWinCode::GetItemByH(HTREEITEM hItem, TVITEM & item)
{
	memset(&item, 0, sizeof(item));
	item.hItem = hItem;
	item.mask = TVIF_TEXT | TVIF_HANDLE | TVIF_PARAM;
	wchar_t a[101] = { 0 };
	item.pszText = a;

	item.cchTextMax = sizeof(a) / sizeof(a[0]);
	TreeView_GetItem(m_hTreeView, &item);
	return true;
}

void GetSignExchange(const char *szParam, string & strEx, string &strSign)
{
	string str(szParam);
	int index = str.find(' ');
	if (index > 0)
	{
		strEx = str.substr(0, index);
		strSign = str.substr(index + 1, str.size() - index - 1);
	}
}

int  CPopWinCode::GetTreeParam(HTREEITEM hItem)
{
	//清空数据
	m_strCurExchange.clear();
	m_strCurCommodity.clear();
	m_CurVarietyType = '\0';
	m_strCurContract.clear();

	int type = 0;
	vector<string> vecStr;
	if (hItem)
	{
		TVITEM  item;
		GetItemByH(hItem, item);
		char *pText = (char *)item.lParam;  //获取当前field
		vecStr.push_back(pText);
	}
	HTREEITEM hParent = TreeView_GetParent(m_hTreeView,hItem);
	while (hParent)
	{
		type++;
		TVITEM  item;
		GetItemByH(hParent, item);
		char *pText = (char *)item.lParam;  //获取当前field
		vecStr.push_back(pText);
		hParent = TreeView_GetParent(m_hTreeView, hParent);
	}
	if (vecStr.size() == 0)
		return -1;
	if (type == ExchangeSele_Type)
	{
		GetSignExchange(vecStr[0].c_str(), m_strCurExchange, m_strSign);	
	}
	else if (type == CommoditySele_Type)
	{
		char Commodity[11] = { 0 };
		sscanf_s(vecStr[0].c_str(), "%s %c", Commodity, sizeof(Commodity), &m_CurVarietyType,1);
		m_strCurCommodity = Commodity;
		
		GetSignExchange(vecStr[1].c_str(), m_strCurExchange, m_strSign);
		
	}
	else if (type == ContractSele_Type)
	{
		char Commodity[11] = { 0 };
		sscanf_s(vecStr[1].c_str(), "%s %c", Commodity, sizeof(Commodity), &m_CurVarietyType,1);
		m_strCurCommodity = Commodity;
		string str(vecStr[0].c_str());
		int index = str.find(' ');
		m_strCurContract = str.substr(index+1,str.size() - index - 1);

		GetSignExchange(vecStr[2].c_str(), m_strCurExchange, m_strSign);
	}
	return type;
}

void CPopWinCode::ClearListCtrl()
{
	ListView_DeleteAllItems(m_hListView);//清行
	while (ListView_DeleteColumn(m_hListView, 0));//清列
	m_sel_row = -1;
	m_sel_col = -1;
}

void CPopWinCode::UpdateContract(int Type)
{
	if (!IsOptionType(m_CurVarietyType) && !IsOpthinCmbType(m_CurVarietyType))
		ClearListCtrl();
	else if ((IsOptionType(m_CurVarietyType) || IsOpthinCmbType(m_CurVarietyType)) && Type == ContractSele_Type)
		m_AllOptionContract.clear();
	else
	{
		ClearListCtrl();
		m_AllOptionContract.clear();
		InvalidateRect(m_hListView,NULL,TRUE);
		return;
	}

    if (!IsOptionType(m_CurVarietyType) && !IsOpthinCmbType(m_CurVarietyType))
    {
        string sCommodityNo = m_strCurExchange + "|" + CConvert::TradeTypeToQuoteType(m_CurVarietyType) + "|" + m_strCurCommodity;
        int nContract = g_pStarApi->GetContractCount(sCommodityNo.c_str());
        UpdateListCtrl(nContract);

        if (nContract > 0)
        {
            SContract **tContract = new SContract*[nContract];
            int nretCon = g_pStarApi->GetContractData(sCommodityNo.c_str(), "", tContract, nContract, true);
            if (nretCon <= LINE_VALID || IsSpreadType(m_CurVarietyType))
            {
                for (int i = 0; i < nretCon; i++)
                {
                    SContract *contract = (SContract *)tContract[i];
                    if (!contract)
                    {
                        break;
                    }

                    LVITEM item;
                    item.mask = LVIF_TEXT;
                    item.iItem = i;
                    item.iSubItem = 0;
                    item.pszText = TEXT("");
                    SendMessage(m_hListView, LVM_INSERTITEM, 0, LPARAM(&item));
                    item.iSubItem = 1;

                    string sContractNo = contract->ContractNo;
                    std::string strContract = "";
                    CConvert::QContractNoTypeToStr(sContractNo, strContract, true);
                    wstring wstr = LoadRC::ansi_to_unicode(strContract);
                    item.pszText = (LPWSTR)wstr.c_str();
                    ListView_SetItem(m_hListView, &item);
                }
            }
            else
            {
                for (int i = 0; i < nretCon; i++)
                {
                    SContract *Contract = (SContract *)tContract[i];
                    if (!Contract)
                        break;
                    LVITEM item;
                    item.mask = LVIF_TEXT;
                    item.iItem = i / 2;
                    {
                        if (i % 2 == 0)
                        {
                            item.iSubItem = 0;
                            item.pszText = TEXT("");
                            SendMessage(m_hListView, LVM_INSERTITEM, 0, LPARAM(&item));
                            item.iSubItem = 1;
                        }
                        else
                            item.iSubItem = 2;

                        string sContractNo = Contract->ContractNo;
                        std::string strContract = "";
                        CConvert::QContractNoTypeToStr(sContractNo, strContract, true);
                        wstring wstr = LoadRC::ansi_to_unicode(strContract);
                        item.pszText = (LPWSTR)wstr.c_str();
                        ListView_SetItem(m_hListView, &item);
                    }
                }
            }

            delete[]tContract;
            InvalidateRect(m_hListView, NULL, TRUE);
        }
    }
    else if (IsOptionType(m_CurVarietyType) && Type == ContractSele_Type)
    {
        string sCommodityNo = m_strCurExchange + "|" + CConvert::TradeTypeToQuoteType(m_CurVarietyType) + "|" + m_strCurCommodity + "|" + m_strCurContract;
        int nOptionSerino = g_pStarApi->GetOptionContractPairCount(sCommodityNo.c_str());
        UpdateListCtrl(nOptionSerino);
        if (nOptionSerino > 0)
        {
            SOptionContractPair **tOContract = new SOptionContractPair*[nOptionSerino];
            nOptionSerino = g_pStarApi->GetOptionContractPairData(sCommodityNo.c_str(), "", tOContract, nOptionSerino, true);
            
            for (int i = 0; i < nOptionSerino; i++)
            {
                SOptionContractPair * oContract = (SOptionContractPair *)tOContract[i];
                if (oContract)
                {
                    m_AllOptionContract.push_back(oContract->StrikePrice);
                }
            }

            sort(m_AllOptionContract.begin(), m_AllOptionContract.end());
            vector<string>::iterator newResult = unique(m_AllOptionContract.begin(), m_AllOptionContract.end());
            m_AllOptionContract.erase(newResult, m_AllOptionContract.end());//删除重复的

            if (m_AllOptionContract.size()>0)
            {
                LVITEM item;
                item.mask = LVIF_TEXT;
                for (uint i = 0; i < m_AllOptionContract.size(); i++)
                {
                    char str[12];//执行价+1
                    wstring wstr;
                    sprintf_s(str, "C%s", m_AllOptionContract[i].c_str());
                    item.iItem = i;
                    item.iSubItem = 0;
                    item.pszText = L"";
                    SendMessage(m_hListView, LVM_INSERTITEM, 0, LPARAM(&item));
                    item.iSubItem = 1;
                    wstr = LoadRC::ansi_to_unicode(str);
                    item.pszText = (LPWSTR)wstr.c_str();
                    ListView_SetItem(m_hListView, &item);
                    sprintf_s(str, "P%s", m_AllOptionContract[i].c_str());
                    item.iSubItem = 2;
                    wstr = LoadRC::ansi_to_unicode(str);
                    item.pszText = (LPWSTR)wstr.c_str();
                    ListView_SetItem(m_hListView, &item);
                }
                InvalidateRect(m_hListView, NULL, TRUE);
            }
        }
    }
    else if (IsOpthinCmbType(m_CurVarietyType) && Type == ContractSele_Type)
    {
        string sCommodityNo = m_strCurExchange + "|" + CConvert::TradeTypeToQuoteType(m_CurVarietyType) + "|" + m_strCurCommodity;
        int nOptionSerino = g_pStarApi->GetContractCount(sCommodityNo.c_str());
        UpdateListCtrl(nOptionSerino);
        if (nOptionSerino>0)
        {
            SContract **tOContract = new SContract*[nOptionSerino];
            nOptionSerino = g_pStarApi->GetContractData(sCommodityNo.c_str(), "", tOContract, nOptionSerino, true);
            int nLen = sCommodityNo.length();

            uint j = 0;
            for (int i = 0; i<nOptionSerino; i++)
            {
                SContract * oContract = (SContract *)tOContract[i];
                std::string tempstr = &string(oContract->ContractNo)[nLen + 1];
                unsigned int pos = tempstr.find('|');
                if (pos == string::npos)
                {
                    continue;
                }
                std::string str1 = tempstr.substr(0, pos);
                std::string str2 = tempstr.substr(pos + 1);
                std::string strContract = "";
                std::string strStrike1 = "";
                std::string strStrike2 = "";
                if ((pos = str1.find('C')) != string::npos || (pos = str1.find('P')) != string::npos)
                {
                    strContract = str1.substr(0, pos);
                    if (strContract != m_strCurContract)
                    {
                        continue;
                    }
                    strStrike1 = str1.substr(pos);
                }
                else
                {
                    continue;
                }

                if ((pos = str2.find('C')) != string::npos || (pos = str2.find('P')) != string::npos)
                {
                    strContract = str2.substr(0, pos);
                    if (strContract != m_strCurContract)
                    {
                        continue;
                    }
                    strStrike2 = str2.substr(pos);
                }
                else
                {
                    continue;
                }

                char str[25];//执行价*2+char*2
                wstring wstr;
                sprintf_s(str, "%s %s/%s %s", m_strCurContract.c_str(), strStrike1.c_str(),
                    m_strCurContract.c_str(), strStrike2.c_str());
                /*sprintf_s(str, "%s %s/%s %s", m_strCurContract.c_str(), &string(cKey.Contract1->ContractNo)[nLen],
                    m_strCurContract.c_str(), &string(cKey.Contract2->ContractNo)[nLen]);*/
                LVITEM item;
                item.mask = LVIF_TEXT;
                item.iItem = j++;
                
                item.iSubItem = 0;
                item.pszText = L"";
                SendMessage(m_hListView, LVM_INSERTITEM, 0, LPARAM(&item));
                item.iSubItem = 1;
                wstr = LoadRC::ansi_to_unicode(str);
                item.pszText = (LPWSTR)wstr.c_str();
                ListView_SetItem(m_hListView, &item);
            }
            //InvalidateRect(m_hListView, NULL, TRUE);
        }
    }
}

bool CPopWinCode::UpdateData(const char * cSign)
{
	if (cSign != ""&&m_strSign != cSign)
	{
		m_bIniCompleted = false;
		m_strSign = cSign;
	}
	if (m_bIniCompleted)//服务器标示未改变
	{
		HTREEITEM item = TreeView_GetSelection(m_hTreeView);
		TreeView_Select(m_hTreeView, item, TVGN_DROPHILITE);
		return true;
	}
	ClearListCtrl();
	DelLParamPtr();
	TreeView_DeleteAllItems(m_hTreeView);

    int iExchangeCount = 0;
	TDataContainer exchangeData;
    TDataContainer commodityData;
    set<string> setOContract;
    HTREEITEM hItemExchange = 0, hSubItemCommodity = 0, hSubItemContract = 0;

    g_pTradeData->GetAllExchange(exchangeData, cSign);
    //优先处理
    vector<const char*> vExchangeNot;
    vector<int> vIdx;
    int nCount = sizeof(FIRST_EXCHANGE_ARRAY) / sizeof(char*);
    for (size_t i = 0; i < exchangeData.size(); i++)
    {
        const char * strExchange = (const char *)exchangeData.at(i);
        int j = 0;
        for (int j = 0; j < nCount; j++)
        {
            if (strcmp(FIRST_EXCHANGE_ARRAY[j], strExchange) == 0)
            {
                vIdx.push_back(j);
                break;
            }
        }
        if (j == nCount)
        {
            vExchangeNot.push_back(strExchange);
        }
    }

    sort(vIdx.begin(), vIdx.end());

    vector<const char *> vSortedExchange;
    for (int i = 0; i < vIdx.size(); i++)
    {
        vSortedExchange.push_back(FIRST_EXCHANGE_ARRAY[vIdx.at(i)]);
    }
    for (auto it : vExchangeNot)
    {
        vSortedExchange.push_back(it);
    }

    for (size_t i = 0; i < vSortedExchange.size(); i++)//遍历交易所 交易所数量等于1自动展开
    {
        const char * strExchange = vSortedExchange.at(i);
        SExchange* pExchange = NULL;
        if (g_pStarApi->GetExchangeData(strExchange, &pExchange, 1, false) && pExchange)
        {
            wchar_t wstrExchange[40];
            wstring wExchangeName = CConvert::UTF8ToUnicode(pExchange->ExchangeName);
            swprintf_s(wstrExchange, TEXT("%s[%s]"), LoadRC::ansi_to_unicode(pExchange->ExchangeNo).c_str(), wExchangeName.c_str());

            TExchangeKey exKey = { 0 };
            strcpy_s(exKey.Sign, cSign);
            strcpy_s(exKey.ExchangeNo, pExchange->ExchangeNo);

            g_pTradeData->GetCommoditys(commodityData, &exKey);

            std::vector<CommodityAndKey *> compareData;
            for (size_t j = 0; j < commodityData.size(); j++)
            {
                SCommodity *pCommodity = NULL;
                const char *strCommodity = (const char *)commodityData.at(j);
                if (g_pStarApi->GetCommodityData(pExchange->ExchangeNo, strCommodity, &pCommodity, 1, false) && pCommodity)
                {
                    compareData.push_back(new CommodityAndKey(pCommodity));
                }
            }

            sort(compareData.begin(), compareData.end(), Commodity_Compare());

            for (size_t j = 0; j < compareData.size(); j++)
            {
                SCommodity *pCommodity = compareData.at(j)->pCommodity;
                if (!pCommodity)
                {
                    break;
                }
                setOContract.clear();
                TCommodityKey tkey;
                CConvert::SContractNoTypeToTradeCommodity("", pCommodity->CommodityNo, tkey);

                wchar_t strCommodity[201];
                wchar_t wCommodityName[101] = { 0 };
                wcscpy_s(wCommodityName, CConvert::UTF8ToUnicode(pCommodity->CommodityName).c_str());
                if (IsSpreadType(tkey.CommodityType))
                {
                    //wchar_t *p = wcsstr(wCommodityName, LoadResWchar_t(IDS_Spread));
                    //wchar_t *q = wcsstr(wCommodityName, LoadResWchar_t(IDS_SpreadM));
                    //if (!p && !q)
                    //    swprintf_s(strCommodity, L"%s[%s %s]", LoadRC::ansi_to_unicode(tkey.CommodityNo).c_str(), wCommodityName, LoadResWchar_t(IDS_Spread));
                    //else
                        swprintf_s(strCommodity, L"%s[%s]", LoadRC::ansi_to_unicode(tkey.CommodityNo).c_str(), wCommodityName);

                }
                else if (IsOptionType(tkey.CommodityType))
                {
                    //wchar_t *p = wcsstr(wCommodityName, LoadResWchar_t(IDS_Option));
                    //if (!p)
                    //    swprintf_s(strCommodity, L"%s[%s %s]", LoadRC::ansi_to_unicode(tkey.CommodityNo).c_str(), wCommodityName, LoadResWchar_t(IDS_Option));
                    //else
                        swprintf_s(strCommodity, L"%s[%s]", LoadRC::ansi_to_unicode(tkey.CommodityNo).c_str(), wCommodityName);
                }
                else if ((IsOpthinCmbType(tkey.CommodityType)))
                {
                    if (tkey.CommodityType == ctSTD)
                    {
/*                        wchar_t *p = wcsstr(wCommodityName, LoadResWchar_t(IDS_Straddle));
                        if (!p)
                            swprintf_s(strCommodity, L"%s[%s %s]", LoadRC::ansi_to_unicode(tkey.CommodityNo).c_str(), wCommodityName, LoadResWchar_t(IDS_Straddle));
                        else*/
                            swprintf_s(strCommodity, L"%s[%s] ", LoadRC::ansi_to_unicode(tkey.CommodityNo).c_str(), wCommodityName);
                    }
                    else if (tkey.CommodityType == ctSTG)
                    {
                        //wchar_t *p = wcsstr(wCommodityName, LoadResWchar_t(IDS_Strangle));
                        //if (!p)
                        //    swprintf_s(strCommodity, L"%s[%s %s]", LoadRC::ansi_to_unicode(tkey.CommodityNo).c_str(), wCommodityName, LoadResWchar_t(IDS_Strangle));
                        //else
                            swprintf_s(strCommodity, L"%s[%s]  ", LoadRC::ansi_to_unicode(tkey.CommodityNo).c_str(), wCommodityName);
                    }
                }
                else
                {
                    swprintf_s(strCommodity, L"%s[%s]", LoadRC::ansi_to_unicode(tkey.CommodityNo).c_str(), wCommodityName);
                }

                if (IsOptionType(tkey.CommodityType))
                {
                    int nContract = g_pStarApi->GetOptionSeriesCount(pCommodity->CommodityNo);
                    if (nContract <= 0)
                    {
                        continue;
                    }

                    SOptionSeries **tContract = new SOptionSeries*[nContract];
                    int retCon = g_pStarApi->GetOptionSeriesData(pCommodity->CommodityNo, "", tContract, nContract, true);
                    for (int k = 0; k < retCon; k++)
                    {
                        SOptionSeries *Contract = (SOptionSeries *)tContract[k];
                        if (!Contract)
                            break;

                        string sContract = Contract->SeriesNo;
                        size_t npo = sContract.find_last_of('|');
                        if (npo != string::npos)
                        {
                            string str = tkey.CommodityNo;
                            str = str + " " + sContract.substr(npo + 1, sContract.length() - npo - 1);
                            setOContract.insert(str);
                        }
                    }
                    delete[] tContract;
                }
                else if (IsOpthinCmbType(tkey.CommodityType))
                {
                    char sCommodityNo[100];
                    sprintf_s(sCommodityNo, "%s|O|%s", tkey.ExchangeNo, /*CConvert::TradeTypeToQuoteType(ctOption)*/ tkey.CommodityNo);
                    int nContract = g_pStarApi->GetOptionSeriesCount(sCommodityNo);
                    if (nContract <= 0)
                    {
                        continue;
                    }

                    SOptionSeries **tContract = new SOptionSeries*[nContract];
                    int retCon = g_pStarApi->GetOptionSeriesData(sCommodityNo, "", tContract, nContract, true);
                    for (int k = 0; k < retCon; k++)
                    {
                        SOptionSeries *Contract = (SOptionSeries *)tContract[k];
                        if (!Contract)
                            break;

                        string sContract = Contract->SeriesNo;
                        size_t npo = sContract.find_last_of('|');
                        if (npo != string::npos)
                        {
                            string str = tkey.CommodityNo;
                            str = str + " " + sContract.substr(npo + 1, sContract.length() - npo - 1);
                            setOContract.insert(str);
                        }
                    }
                    delete[] tContract;
                }

                string strSign(pExchange->ExchangeNo);
                strSign = strSign + " " + " ";
                wstring wstr;
                //期货
                if (!hItemExchange)
                {
                    TVINSERTSTRUCT tvis = { 0 };
                    tvis.item.mask = TVIF_TEXT | TVIF_PARAM;
                    wstr = wstrExchange;
                    tvis.item.pszText = (LPWSTR)wstr.c_str();

                    tvis.hInsertAfter = TVI_LAST;
                    tvis.hParent = TVI_ROOT;
                    tvis.item.lParam = (LPARAM)GetParam(strSign.c_str(), '\0');
                    hItemExchange = reinterpret_cast<HTREEITEM>(SendMessage(m_hTreeView,
                        TVM_INSERTITEM, 0, reinterpret_cast<LPARAM>(&tvis)));
                    iExchangeCount++;
                }
                if (!hSubItemCommodity)
                {
                    TVINSERTSTRUCT tvis = { 0 };
                    tvis.item.mask = TVIF_TEXT | TVIF_PARAM;
                    wstr = strCommodity;
                    tvis.item.pszText = (LPWSTR)wstr.c_str();

                    tvis.hInsertAfter = TVI_LAST;
                    tvis.hParent = hItemExchange;
                    tvis.item.lParam = (LPARAM)GetParam(tkey.CommodityNo, tkey.CommodityType);
                    hSubItemCommodity = reinterpret_cast<HTREEITEM>(SendMessage(m_hTreeView,
                        TVM_INSERTITEM, 0, reinterpret_cast<LPARAM>(&tvis)));
                }
                for (set<string>::iterator it = setOContract.begin(); it != setOContract.end(); it++)
                {
                    char sContract[25];
                    strcpy_s(sContract, 25, it->c_str());
                    TVINSERTSTRUCT tvis = { 0 };
                    tvis.item.mask = TVIF_TEXT | TVIF_PARAM;
                    wstr = LoadRC::ansi_to_unicode(sContract);
                    tvis.item.pszText = (LPWSTR)wstr.c_str();

                    tvis.hInsertAfter = TVI_LAST;
                    tvis.hParent = hSubItemCommodity;
                    tvis.item.lParam = (LPARAM)GetParam(sContract, '\0');
                    hSubItemContract = reinterpret_cast<HTREEITEM>(SendMessage(m_hTreeView,
                        TVM_INSERTITEM, 0, reinterpret_cast<LPARAM>(&tvis)));
                }
                m_bIniCompleted = true;
                
                hSubItemCommodity = 0;
                ProcessMessages();
            }
            for (size_t j = 0; j < compareData.size(); j++)
            {
                delete compareData.at(j);
            }
            compareData.clear();

            hItemExchange = 0;
        }
    }

    return true;
}

LRESULT CPopWinCode::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC			hdc;
	switch (message)
	{
	case WM_CREATE:
		{
			hdc = GetDC(m_Hwnd);
			
			ReleaseDC(m_Hwnd, hdc);
			m_nHeight = m_nRow*N_CODE_HEIGHT + 1;
			//////////////////////////////////////////////////////////////////////////TreeView
			m_hTreeView = CreateWindowEx(0, WC_TREEVIEW,
				TEXT("Tree View"), WS_VISIBLE | WS_CHILD,
				0, 0, m_nWidth*3/5, m_nHeight,
				m_Hwnd, (HMENU)ID_TREE_VIEW, GetModuleHandle(NULL), NULL);
			DWORD dwStyle = GetWindowLong(m_hTreeView, GWL_STYLE);
			dwStyle |= TVS_HASBUTTONS | TVS_LINESATROOT | TVS_HASLINES;
			SetWindowLongPtr(m_hTreeView, GWL_STYLE, dwStyle);
			TreeView_SetItemHeight(m_hTreeView, 24);

			//////////////////////////////////////////////////////////////////////////ListView
			m_hListView = CreateWindowEx(0, WC_LISTVIEW,
				TEXT("List view"), WS_VISIBLE | WS_CHILD | LVS_REPORT,
				m_nWidth *3/ 5, 0, m_nWidth *2/ 5, m_nHeight,
				m_Hwnd, (HMENU)ID_LIST_VIEW, GetModuleHandle(NULL), NULL);
			m_ListViewProc = (WNDPROC)SetWindowLong(m_hListView, GWL_WNDPROC, (LONG)ListViewProc);
			SetWindowLong(m_hListView, GWL_USERDATA, LONG(this));
			
			ListView_SetExtendedListViewStyleEx(m_hListView, NULL, LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);


			//控制行高
			HIMAGELIST kk = ImageList_Create(1, 24, ILC_COLOR32, 1, 1);
			ListView_SetImageList(m_hListView, kk, LVSIL_SMALL);
			//ImageList_Destroy(kk);

			UpdateListCtrl();
			SendMessage(m_hTreeView, WM_SETFONT, (WPARAM)g_FontData.GetFontNum15(), (LPARAM)TRUE);
			SendMessage(m_hListView, WM_SETFONT, (WPARAM)g_FontData.GetFontNum15(), (LPARAM)TRUE);
			//MoveWindow(m_Hwnd, m_nLeft, m_nTop, m_nWidth, m_nHeight, TRUE);
			SetWindowPos(m_Hwnd, NULL, m_nLeft, m_nTop, m_nWidth, m_nHeight, SWP_SHOWWINDOW);
		}
		break;
	case WM_NOTIFY:
		if ((int)wParam == ID_TREE_VIEW)
		{
			NMHDR *pNMHDR = (NMHDR *)lParam;
			LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
			if (NM_CLICK == pNMHDR->code)
			{
				POINT pt;
				::GetCursorPos(&pt);
				ScreenToClient(pNMHDR->hwndFrom, &pt);
				TVHITTESTINFO info;
				info.pt = pt;
				HTREEITEM item = TreeView_HitTest(pNMHDR->hwndFrom, &info);
				TreeView_Select(m_hTreeView, item, TVGN_DROPHILITE);
				UpdateListCtrl();
				HTREEITEM hChild = TreeView_GetChild(pNMHDR->hwndFrom, info.hItem);//m_TreeCtrl.GetChildItem(hClickItem);
				if (hChild ==NULL/*m_TreeCtrl.GetNextSiblingItem(hChild)*/)
				{
					m_sExCom.clear();
					m_sCode.clear();
					bool bLME = false;
					int Type = GetTreeParam(info.hItem);
					if (Type == -1)
						break;
					UpdateContract(Type);
					char strCode[51];
					if (IsOptionType(m_CurVarietyType))
						sprintf_s(strCode, "%s %s %s", m_strCurExchange.c_str(), m_strCurCommodity.c_str(), m_strCurContract.c_str());
					else if (IsLMEFutures(m_strCurExchange.c_str(), m_strSign.c_str()) /*&& !m_Show_LME_Contract*/)
					{
						sprintf_s(strCode, "%s %s 3M", m_strCurExchange.c_str(), m_strCurCommodity.c_str());
						bLME = true;
					}
					else
						sprintf_s(strCode, "%s %s", m_strCurExchange.c_str(), m_strCurCommodity.c_str());
					m_sCode = strCode;
					m_sExCom = strCode;
					if (bLME || m_CurVarietyType == ctGoods || m_CurVarietyType==ctDefer)
					{
						SelectCode();
						ClearListCtrl();
					}
				}
				else
				{
					if (info.flags & TVHT_ONITEMBUTTON)//点击展开节点按钮
					{}
					else//点击节点文本
						TreeView_Expand(pNMHDR->hwndFrom, info.hItem, TVE_TOGGLE);
					if (NULL == TreeView_GetNextSibling(pNMHDR->hwndFrom, hChild))
					{
						HTREEITEM hChild1 = TreeView_GetChild(pNMHDR->hwndFrom, hChild);
						TreeView_Expand(pNMHDR->hwndFrom, hChild, TVE_TOGGLE);
						if (hChild1&&NULL == TreeView_GetNextSibling(pNMHDR->hwndFrom, hChild))
							TreeView_Expand(pNMHDR->hwndFrom, hChild1, TVE_TOGGLE);
					}
				}		
			}
		}
		if ((int)wParam == ID_LIST_VIEW)
		{
			NMHDR *pNMHDR = (NMHDR *)lParam;
			LPNMLISTVIEW pNMListView = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
			switch (pNMHDR->code)
			{
				case NM_CLICK:
				{
					LPNMITEMACTIVATE lpnmitem = (LPNMITEMACTIVATE)lParam;
					string text;
					wchar_t wtext[100] = { 0 };
					LVITEM lvitem;
					lvitem.mask = LVIF_TEXT;
					lvitem.iItem = lpnmitem->iItem;
					lvitem.iSubItem = lpnmitem->iSubItem;
					lvitem.cchTextMax = sizeof(wtext) / sizeof(wtext[0]);
					lvitem.pszText = wtext;
					ListView_GetItem(m_hListView, &lvitem);
					text = LoadRC::unicode_to_ansi(wtext);

					string str;
					if (GetCurOption(str) && lvitem.iItem != -1)
					{
						str = str + " " + text;
						m_sCode = str;
						SelectCode();
						//ClearListCtrl();
					}
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
							if (lpDraw->iSubItem == m_sel_col&&lpDraw->nmcd.dwItemSpec == m_sel_row)
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
		m_bIniCompleted = false;
		break;
	default:                      /* 不感兴趣的消息交给DefWindowProc()处理 */
		return NOT_PROCESSED;
	}

	return PROCESSED;
}

