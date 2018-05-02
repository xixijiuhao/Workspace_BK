#include "BaseInclude.h"
using namespace std;
extern ITradeApi *g_pTradeApi;
extern ITradeData *g_pTradeData;

WNDPROC g_CodelistWndProc;		//合约代码列表 子类型化窗口处理过程
bool g_MouseTrack = TRUE;		//鼠标跟踪标示变量 

LRESULT CALLBACK CodeProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int id = GetWindowLong(hwnd, GWL_ID);
	CPopWinCodeList *pcodebox = (CPopWinCodeList *)GetWindowLong(hwnd, GWL_USERDATA);
	static POINT pt;
	switch (message)
	{
	case WM_LBUTTONDOWN:
		{
			POINT point;
			point.x = LOWORD(lParam);
			point.y = HIWORD(lParam);
			RECT rect;
			GetWindowRect(hwnd, &rect);
			ScreenToClient(hwnd, LPPOINT(&rect));
			ScreenToClient(hwnd, LPPOINT(&rect) + 1);
			if (PtInRect(&rect, point))
				SendMessage(GetParent(hwnd), WM_CODE_SELECTED, 0, 0);
			ShowWindow(GetParent(hwnd), SW_HIDE);
		}
		break;
	case WM_MOUSEMOVE:
		{
			if (g_MouseTrack)    //若允许追踪，则。
			{
				TRACKMOUSEEVENT csTME;
				csTME.cbSize = sizeof(csTME);
				csTME.dwFlags = TME_LEAVE | TME_HOVER;
				csTME.hwndTrack = hwnd;//指定要追踪的窗口
				csTME.dwHoverTime = 10;  //鼠标在按钮上停留超过10ms，才认为状态为HOVER
				::TrackMouseEvent(&csTME); //开启Windows的WM_MOUSELEAVE，WM_MOUSEHOVER事件支持
				g_MouseTrack = FALSE;   //若已经追踪，则停止追踪
			}
			POINT point;
			point.x = LOWORD(lParam);
			point.y = HIWORD(lParam);
			if (point.x != pt.x&&pcodebox!=NULL)
			{
				UINT uItem = SendMessage(hwnd, LB_ITEMFROMPOINT, 0, lParam);
				if (uItem != pcodebox->m_nCurSel)
				{
					SendMessage(hwnd, LB_SETCURSEL, uItem, 0);
					pcodebox->m_nCurSel = uItem;
				}
				pt = point;	
			}	
		}
		break;
	case WM_MOUSELEAVE:
		g_MouseTrack = TRUE;
		break;
	default:
		break;
	}
	return CallWindowProc(g_CodelistWndProc, hwnd, message, wParam, lParam);
}

CPopWinCodeList::CPopWinCodeList() :
m_opr(NULL),
m_nWidth(0),
m_nHeight(0),
m_sCode(""),
m_nLen(0),
m_nLastInputLen(0),
m_nCurSel(0)
{
	
}

CPopWinCodeList::~CPopWinCodeList()
{
	ClearList();
}

void CPopWinCodeList::CreatePopWin(IOperator *opr, LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle, int nleft, int ntop, UINT nWidth, UINT nrow)
{
	m_nWidth = nWidth;
	m_nRow = nrow;
	m_nLeft = nleft;
	m_nTop = ntop;
	m_opr = opr;
	CreateFrm(classname, parent, style , exstyle);
}

//加上这个为了排序 
enum eCommodityType
{
	Goods_Type = 1,
	Future_Type,
	SpreadMonth_Type,
	SpreadCommodity,
	Option_Type,
	BUL_Type,
	BER_Type,
	STD_Type,
	STG_Type
};

void CPopWinCodeList::Less_Commodity::GetKey(const TContract* pContract, string &strKey)
{
	char sKey[80];
	if (pContract->pCommodity == NULL)	return;
	if (ctGoods == pContract->pCommodity->CommodityType)
		sprintf_s(sKey,"%d$%s", Goods_Type, pContract->pCommodity->CommodityNo);
	else if (ctFutures == pContract->pCommodity->CommodityType)
		sprintf_s(sKey, "%d$%s$%s", Future_Type, pContract->pCommodity->CommodityNo, pContract->ContractNo);
	else if (ctSpreadMonth == pContract->pCommodity->CommodityType)
		sprintf_s(sKey, "%d$%s$%s$%s", SpreadMonth_Type, pContract->pCommodity->RelateCommodityNo, pContract->ContractNo, pContract->ContractNo2);
	else if (ctSpreadCommodity == pContract->pCommodity->CommodityType)
		sprintf_s(sKey, "%d$%s$%s$%s", SpreadCommodity, pContract->pCommodity->RelateCommodityNo, pContract->pCommodity->RelateCommodityNo2, pContract->ContractNo);
	else if (ctOption == pContract->pCommodity->CommodityType)
		sprintf_s(sKey, "%d$%s$%s$%c%s", Option_Type, pContract->pCommodity->RelateCommodityNo, pContract->ContractNo, pContract->OptionType, pContract->StrikePrice);
	else if (ctBUL == pContract->pCommodity->CommodityType)
		sprintf_s(sKey, "%d$%s$%s$%c%s/%s$%c%s", BUL_Type, pContract->pCommodity->RelateCommodityNo, pContract->ContractNo, pContract->OptionType, pContract->StrikePrice, pContract->ContractNo2, pContract->OptionType2, pContract->StrikePrice2);
	else if (ctBER == pContract->pCommodity->CommodityType)
		sprintf_s(sKey, "%d$%s$%s$%c%s/%s$%c%s", BER_Type, pContract->pCommodity->RelateCommodityNo, pContract->ContractNo, pContract->OptionType, pContract->StrikePrice, pContract->ContractNo2, pContract->OptionType2, pContract->StrikePrice2);
	else if (ctSTD == pContract->pCommodity->CommodityType)
		sprintf_s(sKey, "%d$%s$%s$%c%s/%s$%c%s", STD_Type, pContract->pCommodity->RelateCommodityNo, pContract->ContractNo, pContract->OptionType, pContract->StrikePrice, pContract->ContractNo2, pContract->OptionType2, pContract->StrikePrice2);
	else if (ctSTG == pContract->pCommodity->CommodityType)
		sprintf_s(sKey, "%d$%s$%s$%c%s/%s$%c%s", STG_Type, pContract->pCommodity->RelateCommodityNo, pContract->ContractNo, pContract->OptionType, pContract->StrikePrice, pContract->ContractNo2, pContract->OptionType2, pContract->StrikePrice2);

	strKey = sKey;
}

bool CPopWinCodeList::IsOptionType(char CommodityType)
{
	return CommodityType == ctOption;
}

bool CPopWinCodeList::IsOpthinCmbType(char CommodityType)
{
	return ctBUL == CommodityType || ctBER == CommodityType || ctSTD == CommodityType || ctSTG == CommodityType;
}

bool CPopWinCodeList::IsSpreadType(char CommodityType)
{
	return ctSpreadMonth == CommodityType || ctSpreadCommodity == CommodityType;
}

void CPopWinCodeList::GetContractByOption(TDataContainer & ConDataCon, set<string> &setContract)
{
	for (uint i = 0; i < ConDataCon.size(); i++)
	{
		TContract* pContract = ((TContract*)ConDataCon.at(i));
		if (IsOptionType(pContract->CommodityType) || IsOpthinCmbType(pContract->CommodityType))
		{
			char str[25];
			sprintf_s(str,"%s %s", pContract->CommodityNo, pContract->ContractNo);
			setContract.insert(str);
		}
	}
}

bool CPopWinCodeList::UpdateData()
{
	ClearList();
	m_VecCom.clear();
	m_VecExc.clear();
	
	TDataContainer tempExcDataCon;
	TDataContainer tempComDataCon;
	TDataContainer tempContract;
	g_pTradeData->GetAllExchange(tempExcDataCon);
	for (uint i = 0; i < tempExcDataCon.size(); i++)											//所有交易所
	{
		tempComDataCon.clear();
		g_pTradeData->GetCommoditys((TExchange*)tempExcDataCon.at(i), tempComDataCon);
		for (uint j = 0; j < tempComDataCon.size(); j++)										//所有商品
		{
			if (IsOptionType(((TCommodity*)tempComDataCon.at(j))->CommodityType) || IsOpthinCmbType(((TCommodity*)tempComDataCon.at(j))->CommodityType))
				continue;
			tempContract.clear();
			g_pTradeData->GetContracts((TCommodity*)tempComDataCon.at(j), tempContract);
			for (uint k = 0; k < tempContract.size(); k++)
			{
				map<char, vector<const TContract*>>::iterator iter = m_VecCom.find(((TContract*)tempContract.at(k))->CommodityNo[0]);
				if (iter != m_VecCom.end())
					iter->second.push_back((TContract*)tempContract.at(k));
				else
				{
					vector<const TContract*> vec;
					vec.push_back((TContract*)tempContract.at(k));
					m_VecCom.insert(pair<char, vector<const TContract*>>(((TContract*)tempContract.at(k))->CommodityNo[0], vec));
				}

				map<char, vector<const TContract*>>::iterator iter2 = m_VecExc.find(((TContract*)tempContract.at(k))->ExchangeNo[0]);
				if (iter2 != m_VecExc.end())
					iter2->second.push_back((TContract*)tempContract.at(k));
				else
				{
					vector<const TContract*> vec;
					vec.push_back((TContract*)tempContract.at(k));
					m_VecExc.insert(pair<char, vector<const TContract*>>(((TContract*)tempContract.at(k))->ExchangeNo[0], vec));
				}
			}
		}
	}
	
	return true;
}

void CPopWinCodeList::EditChange(const string str, const string strSign)
{
	
	m_sCode = str;//m_strInput保存下来
	m_nLen = strlen(m_sCode.c_str());
	if (m_nLen == 0)
	{
		ClearList();
		DelListDataPtr();
		ShowWindow(m_Hwnd,SW_HIDE);
		m_nLastInputLen = 0;
	}
	else
	{
		if (m_nLen == 1 /*&& m_nLen > m_nLastInputLen*/)
		{
			ClearList();
			GetAllList(m_sCode.at(0), strSign);
			FillListByLen();
		}
		else if (m_nLen < m_nLastInputLen)
		{
			FillListByLen(TRUE);
		}
		else if (m_itemList.size() > 0 && m_nLen > m_nLastInputLen)
		{
			FillListByLen(TRUE);
		}
	}

	//品种没找到 就按交易所关键字找了
	FillListByExchange(strSign);

	m_nLastInputLen = m_nLen;

	if ((m_itemListByLen.empty() && m_itemExchangeList.empty()) && IsWindowVisible(m_Hwnd))
		ShowWindow(m_Hwnd,SW_HIDE);
	else if ((!m_itemListByLen.empty() || !m_itemExchangeList.empty()) /*&& !IsWindowVisible(m_Hwnd)*/)
	{
		ShowWindow(m_Hwnd, SW_SHOWNOACTIVATE);
	}
	else
		ShowWindow(m_Hwnd, SW_HIDE);
}

void CPopWinCodeList::ClearList()
{
	vector<Result_FuzzyQuery*>::size_type nCount = m_itemList.size();

	for (size_t i = 0; i < nCount; i++)
		delete m_itemList[i];

	vector<Result_FuzzyQuery*>::size_type nCount2 = m_itemExchangeList.size();
	for (size_t i = 0; i < nCount2; i++)
		delete m_itemExchangeList[i];

	m_itemList.clear();//清理
	m_itemListByLen.clear();

	m_itemExchangeList.clear();//清理
	m_itemExchangeListByLen.clear();
}

void CPopWinCodeList::DelListDataPtr()
{
	SendMessage(m_hListBox,LB_RESETCONTENT,0,0);
}

string CPopWinCodeList::CreateContractTitle(const TContractKey & ContractKey)
{
	char title[80];
	if (ContractKey.CommodityType == ctFutures || ContractKey.CommodityType == ctGoods)//期货,现货
		sprintf_s(title,"%s %s", ContractKey.CommodityNo, ContractKey.ContractNo);
	else if (ContractKey.CommodityType == ctOption)//期权
		sprintf_s(title, "%s %s %c%s", ContractKey.CommodityNo, ContractKey.ContractNo, ContractKey.OptionType, ContractKey.StrikePrice);
	else if (ContractKey.CommodityType == ctSpreadMonth)//期货的 跨期套利，
		sprintf_s(title, "%s %s/%s", ContractKey.CommodityNo, ContractKey.ContractNo, ContractKey.ContractNo2);
	else if (ContractKey.CommodityType == ctSpreadCommodity)//跨商品套利
		sprintf_s(title, "%s %s", ContractKey.CommodityNo, ContractKey.ContractNo);
	else if (ContractKey.CommodityType == ctBUL || ContractKey.CommodityType == ctBER || ContractKey.CommodityType == ctBLT || ContractKey.CommodityType == ctBRT || ContractKey.CommodityType == ctSTD || ContractKey.CommodityType == ctSTG)//期权的 看涨期权垂直价差，看跌期权垂直价差，跨式组合，宽跨式组合，水平看涨套利，水平看跌套利	
		sprintf_s(title, "%s %s %c%s/%s %c%s", ContractKey.CommodityNo, ContractKey.ContractNo, ContractKey.OptionType, ContractKey.StrikePrice, ContractKey.ContractNo2, ContractKey.OptionType2, ContractKey.StrikePrice2);
	return title;
}

string CPopWinCodeList::CreateContractTitle2(const TContractKey & ContractKey)
{
	char title[80];
	if (ContractKey.CommodityType == ctFutures || ContractKey.CommodityType == ctGoods)//期货,现货
		sprintf_s(title, "%s %s %s", ContractKey.ExchangeNo, ContractKey.CommodityNo, ContractKey.ContractNo);
	else if (ContractKey.CommodityType == ctOption)//期权
		sprintf_s(title, "%s %s %s %c%s", ContractKey.ExchangeNo, ContractKey.CommodityNo, ContractKey.ContractNo, ContractKey.OptionType, ContractKey.StrikePrice);
	else if (ContractKey.CommodityType == ctSpreadMonth)//期货的 跨期套利，跨商品套利
		sprintf_s(title, "%s %s %s/%s", ContractKey.ExchangeNo, ContractKey.CommodityNo, ContractKey.ContractNo, ContractKey.ContractNo2);
	else if (ContractKey.CommodityType == ctSpreadCommodity)//跨商品套利
		sprintf_s(title, "%s %s %s", ContractKey.ExchangeNo, ContractKey.CommodityNo, ContractKey.ContractNo);
	else if (ContractKey.CommodityType == ctBUL || ContractKey.CommodityType == ctBER || ContractKey.CommodityType == ctBLT || ContractKey.CommodityType == ctBRT || ContractKey.CommodityType == ctSTD || ContractKey.CommodityType == ctSTG)//期权的 看涨期权垂直价差，看跌期权垂直价差，跨式组合，宽跨式组合，水平看涨套利，水平看跌套利	
		sprintf_s(title, "%s %s %s %c%s/%s %c%s", ContractKey.ExchangeNo, ContractKey.CommodityNo, ContractKey.ContractNo, ContractKey.OptionType, ContractKey.StrikePrice, ContractKey.ContractNo2, ContractKey.OptionType2, ContractKey.StrikePrice2);
	return title;
}

void CPopWinCodeList::GetAllList(char ch, const string strSign)
{
	// 合约列表，从数据类获取列表 先找对应品种、后找对应市场
	if ((ch >= '0' && ch <= '9') || ch == 'B' || ch == 'I' || (ch >= 'A' && ch <= 'Z'))
	{
		Result_FuzzyQuery *pItem = NULL;
		bool bSearchAll = false;
		if (strSign.empty()) bSearchAll = true;
		MultMap::iterator pos;
		string title;
		map<char,vector<const TContract*>>::iterator it = m_VecCom.find(ch);
		if (it != m_VecCom.end() && !it->second.empty())
		{
			for (vector<const TContract*>::iterator it1 = it->second.begin(); it1 != it->second.end();it1++)
			{
				title = CreateContractTitle(**it1);
				if (bSearchAll || (0 == strcmp(strSign.c_str(), (*it1)->Sign)))
				{
					pItem = new Result_FuzzyQuery;
					strcpy_s(pItem->szName, title.c_str());
					pItem->pContract = (*it1);
					m_itemList.push_back(pItem);
				}
			}		
			sort(m_itemList.begin(), m_itemList.end(), Less_Commodity());//耗时太多
		}	
	}
}

void CPopWinCodeList::GetAllExchangeList(char ch, const string strSign)
{
	if ((ch >= '0' && ch <= '9') || ch == 'B' || ch == 'I' || (ch >= 'A' && ch <= 'Z'))
	{
		Result_FuzzyQuery *pItem = NULL;
		bool bSearchAll = false;
		if (strSign.empty()) bSearchAll = true;
		MultMap::iterator pos;
		string title;
		map<char, vector<const TContract*>>::iterator it = m_VecExc.find(ch);
		if (it != m_VecExc.end() && !it->second.empty())
		{
			for (vector<const TContract*>::iterator it1 = it->second.begin(); it1 != it->second.end(); it1++)
			{
				title = CreateContractTitle2(**it1);
				if (bSearchAll || (0 == strcmp(strSign.c_str(), (*it1)->Sign)))
				{
					pItem = new Result_FuzzyQuery;
					strcpy_s(pItem->szName, title.c_str());
					pItem->pContract = (*it1);
					m_itemExchangeList.push_back(pItem);
				}
			}
			sort(m_itemExchangeList.begin(), m_itemExchangeList.end(), Less_Commodity());
		}
	}
}

inline void TrimSpaceAndUpper(const char *source, char *target)
{
	char ch;
	int len = strlen(source);
	int count = 0;
	for (int i = 0; i < len; i++)
	{
		ch = source[i];
		if (ch != ' ')
		{
			target[count] = toupper(ch);
			count++;
		}
	}
	target[count] = '\0';
}

void CPopWinCodeList::FillListByLen(BOOL bCompareLength)
{
	//这里的比较用去除了空格的串来比较
	Result_FuzzyQuery* pItem = NULL;
	vector<Result_FuzzyQuery*>::size_type nCount = m_itemList.size();
	//删除了空格的
	char strInput[128];
	TrimSpaceAndUpper(m_sCode.c_str(), strInput);
	int inputlen = strlen(strInput);
	char temp[128];
	if (bCompareLength)
	{
		if (m_nLen > m_nLastInputLen)
		{
			nCount = m_itemListByLen.size();
			for (int i = nCount - 1; i >= 0; i--)
			{
				pItem = m_itemListByLen[i];
				TrimSpaceAndUpper(pItem->szName, temp);
				if (_strnicmp(temp, strInput, inputlen) != 0)
					m_itemListByLen.erase(m_itemListByLen.begin() + i);
			}
		}
		else if (m_nLen < m_nLastInputLen)
		{
			m_itemListByLen.clear();
			for (size_t i = 0; i < nCount; i++)
			{
				pItem = m_itemList[i];
				TrimSpaceAndUpper(pItem->szName, temp);
				if (_strnicmp(temp, strInput, inputlen) != 0)
					continue;
				m_itemListByLen.push_back(pItem);
			}
		}
	}
	else
	{
		m_itemListByLen.resize(nCount);
		for (size_t i = 0; i < nCount; i++)
			m_itemListByLen[i] = m_itemList[i];
	}

	// 数据显示 
	DelListDataPtr();
	for (vector<Result_FuzzyQuery*>::size_type i = 0; i < (m_itemListByLen.size()>50 ? 50 : m_itemListByLen.size()); i++)
	{
		pItem = m_itemListByLen[i];
		string str = CreateContractTitle2(*pItem->pContract);
		wstring wstr = LoadRC::ansi_to_unicode(str);
		SendMessage(m_hListBox, LB_ADDSTRING, (WPARAM)0, (LPARAM)(LPWSTR)wstr.c_str());	
	}
	SendMessage(m_hListBox,LB_SETCURSEL,0,0);
	m_nCurSel = 0;
}

void CPopWinCodeList::FillListByExchange(const string strSign)
{
	if (m_itemListByLen.empty() && strlen(m_sCode.c_str()) >= 1)
	{
		// 数据显示 
		DelListDataPtr();
		ClearList();
		GetAllExchangeList(m_sCode.at(0), strSign);

		//这里的比较用去除了空格的串来比较
		Result_FuzzyQuery* pItem = NULL;
		vector<Result_FuzzyQuery*>::size_type nCount = m_itemExchangeList.size();
		//删除了空格的
		char strInput[128];
		TrimSpaceAndUpper(m_sCode.c_str(), strInput);
		int inputlen = strlen(strInput);
		char temp[128];

		m_itemExchangeListByLen.clear();
		for (size_t i = 0; i < nCount; i++)
		{
			pItem = m_itemExchangeList[i];
			TrimSpaceAndUpper(pItem->szName, temp);
			if (_strnicmp(temp, strInput, inputlen) != 0)
				continue;
			m_itemExchangeListByLen.push_back(pItem);
		}

		// 数据显示 
		if (m_itemExchangeListByLen.size() > 0)
		{
			DelListDataPtr();
			for (vector<Result_FuzzyQuery*>::size_type i = 0; i < (m_itemExchangeListByLen.size()>50 ? 50 : m_itemExchangeListByLen.size()); i++)
			{
				pItem = m_itemExchangeListByLen[i];
				string str = CreateContractTitle2(*pItem->pContract);
				wstring wstr = LoadRC::ansi_to_unicode(str);
				SendMessage(m_hListBox, LB_ADDSTRING, (WPARAM)0, (LPARAM)(LPWSTR)wstr.c_str());
			}
			SendMessage(m_hListBox, LB_SETCURSEL, 0, 0);
			m_nCurSel = 0;
		}
	}
}

void CPopWinCodeList::AddNum()
{
	int nCount = SendMessage(m_hListBox, LB_GETCOUNT, 0, 0);
	m_nCurSel = SendMessage(m_hListBox, LB_GETCURSEL, 0, 0);
	m_nCurSel++;
	if (m_nCurSel > nCount)
		m_nCurSel = nCount;
	SendMessage(m_hListBox, LB_SETCURSEL, m_nCurSel, 0);
}

void CPopWinCodeList::DelNum()
{
	m_nCurSel = SendMessage(m_hListBox, LB_GETCURSEL, 0, 0);
	m_nCurSel--;
	
	if (m_nCurSel < 0)
		m_nCurSel = 0;
	SendMessage(m_hListBox, LB_SETCURSEL, m_nCurSel, 0);
}

void CPopWinCodeList::OnReturn()
{
	if (!m_hListBox)
		return;
	int nCur = SendMessage(m_hListBox, LB_GETCURSEL, 0, 0);
	if (nCur != -1)
	{
		wchar_t wstr[80] = {0};
		SendMessage(m_hListBox, LB_GETTEXT, nCur, (LPARAM)wstr);

		string str = LoadRC::unicode_to_ansi(wstr);
		if (m_opr)
		{
			m_opr->SetContractNo(str);
		}
		ShowWindow(m_Hwnd, SW_HIDE);
	}
}

LRESULT CPopWinCodeList::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		{
			m_nHeight = m_nRow*N_CODE_LIST_HEIGHT+1;

			m_hListBox = CreateWindowEx(0, WC_LISTBOX,
				TEXT("List box"), WS_VISIBLE | WS_CHILD | WS_VSCROLL,
				0, 0, m_nWidth, m_nHeight,
				m_Hwnd, (HMENU)ID_LIST_BOX, GetModuleHandle(NULL), NULL);
			SendMessage(m_hListBox, LB_SETITEMHEIGHT, 0, N_CODE_LIST_HEIGHT);

			g_CodelistWndProc = (WNDPROC)SetWindowLong(m_hListBox, GWL_WNDPROC, (LONG)CodeProc);	//控件子类化
			SetWindowLong(m_hListBox,GWL_USERDATA,LONG(this));										//控件 父类数据绑定
			MoveWindow(m_Hwnd, m_nLeft, m_nTop, m_nWidth, m_nHeight, TRUE);	
		}
		break;
	case WM_CODE_SELECTED:
		{
			int nCur = SendMessage(m_hListBox, LB_GETCURSEL, 0, 0);
			if (nCur != -1)
			{
				wchar_t wstr[80];
				SendMessage(m_hListBox, LB_GETTEXT, nCur, (LPARAM)wstr);

				string str = LoadRC::unicode_to_ansi(wstr);
				if (m_opr)
				{
					m_opr->SetContractNo(str);
				}
				ShowWindow(m_Hwnd, SW_HIDE);
			}
		}
		break;
	case WM_ERASEBKGND:
		break;
	default:                      /* 不感兴趣的消息交给DefWindowProc()处理 */
		return NOT_PROCESSED;
	}

	return PROCESSED;
}
