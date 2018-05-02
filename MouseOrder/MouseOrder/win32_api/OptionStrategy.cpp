#include "PreInclude.h"

extern G_COLOREF	g_ColorRefData;
extern G_UIFONT		g_FontData;
//extern IQuoteApi	*g_QuoteApi;
extern IStarApi     *g_pStarApi;
extern ITradeApi	*g_pTradeApi;
extern ITradeData	*g_pTradeData;
extern IMainFrame	*g_mainframe;

#define N_ROW_HEIGHT	25	//行高
#define N_SPIN_WIDTH	18	//
#define N_40_WIDTH		40
#define N_65_WIDTH		65	//买卖
#define N_80_WIDTH		80	//价格类型 价格 数量 Delta
#define N_100_WIDTH		100	//按钮宽度

//价格 数量 编辑框 输入控制
LRESULT OrderOptionEditWndProc(HWND hwnd, UINT message, WPARAM w, LPARAM l)
{
	int id = GetWindowLong(hwnd, GWL_ID);
	COrderOption *p_orderoption = (COrderOption *)GetWindowLong(hwnd, GWL_USERDATA);
	if (!p_orderoption)
		return FALSE;
	switch (message)
	{
		case WM_CHAR://字符过滤
		{
			switch (id)
			{
			case OC_Price_Edit:
				if (w == '\b')
					break;
				if ((w >= '0'&&w <= '9') || w == '.')
				{
					int nStart = -1, nEnd = -1;
					bool bIsValidNum = false;
					string stemp = GetWindowTextAnsic(hwnd);
					DWORD dValue = SendMessage(hwnd, EM_GETSEL, 0, 0);
					nStart = LOWORD(dValue);
					nEnd = HIWORD(dValue);
					if (nStart != -1 && nStart < nEnd)
					{
						TMouPriceStrA str = { 0 };
						string sleft = stemp.substr(0, nStart);
						string sright = stemp.substr(nEnd, stemp.size() - nEnd);
						sprintf_s(str, "%s%c%s", sleft.c_str(), w, sright.c_str());
						stemp = str;
					}
					else if (nStart == nEnd)
						stemp.insert(nEnd, 1, w);
		
					if (p_orderoption->m_OptionCommodityInfo.iPrecision == 0)//整数
						bIsValidNum = CConvert::IsValidNumber(stemp.c_str(), true, false);
					else//可以有小数点
						bIsValidNum = CConvert::IsValidNumber(stemp.c_str(), false, false);
					
					if (bIsValidNum)
						break;
					else
						return FALSE;
				}
				else
					return FALSE;
			case OC_Qty_Edit:
				if ((w >= '0'&&w <= '9') || w == '\b')
					break;
				else
					return FALSE;
			default:
				break;
			}
			break;
		}
		default:
			break;
	}
	if (id == OC_Price_Edit)
		return CallWindowProc(p_orderoption->m_EditProc, hwnd, message, w, l);//多个对象
	else if (id == OC_Qty_Edit)
		return CallWindowProc(p_orderoption->m_EditProc2, hwnd, message, w, l);//多个对象
	else
		return FALSE;
}
COrderOption::COrderOption()
: m_SyncPriceTypeBtn(IDB_BITMAP_SYNC)
, m_SyncQtyBtn(IDB_BITMAP_SYNC)
, m_nleft(0)
, m_ntop(0)
, m_nWidth(0)
, m_nHeight(0)
, m_bTracking(false)
, m_nOptionRows(0)
, m_nCurHoverID(-1)
, m_CurType(T_AutoGroup)
{
	m_LClickItem.nRow = -1;
	m_LClickItem.nCol = -1;
	m_CurShowItem.nRow = -1;
	m_CurShowItem.nCol = -1;
}
COrderOption::~COrderOption()
{
	if (g_pStarApi)
        g_pStarApi->UnsubAllQuote(this);
}
void COrderOption::CreateWin(CMousewin* opr, HWND parent, int nleft, int ntop, bool bUseFor)
{
	m_opr = opr;
	m_nleft = nleft;
	m_ntop = ntop;
	m_nOptionRows = N_Head_row;
	if (bUseFor)
	{
		m_ExColor[EDC_Sell] = g_ColorRefData.GetColorBuyRed();
		m_ExColor[EDC_Buy] = g_ColorRefData.GetColorTextGreen();
		m_ExBrush[EDC_Sell] = g_ColorRefData.GetHbrBuyRed();
		m_ExBrush[EDC_Buy] = g_ColorRefData.GetHbrTextGreen();
	}
	else
	{
		m_ExColor[EDC_Buy] = g_ColorRefData.GetColorBuyRed();
		m_ExColor[EDC_Sell] = g_ColorRefData.GetColorTextGreen();
		m_ExBrush[EDC_Buy] = g_ColorRefData.GetHbrBuyRed();
		m_ExBrush[EDC_Sell] = g_ColorRefData.GetHbrTextGreen();
	}
	CreateFrm(L"OrderOption", parent, WS_CHILD| WS_CLIPSIBLINGS, 0);	
}
void COrderOption::OnQuote(const SContract* cont, const SQuoteUpdate* quote)
{
	PostMessage(m_Hwnd, WM_DELOPTIONQUOTE,0,0);
}
int COrderOption::GetTypeStrId(int nType)
{
	switch (nType)
	{
	case T_Bullish:			//大涨
		return IDS_Bullish;
	case T_Bearish:			//大跌
		return IDS_Bearish;
	case T_NotRise:			//不涨
		return  IDS_NotRise;
	case T_NotFall:			//不跌
		return  IDS_NotFall;
	case T_Break:			//突破	
		return IDS_Break;
	case T_Dull:			//盘整
		return IDS_Dull;
	case T_BreakStrangle:	//突破（宽）
		return IDS_BreakStrangle;
	case T_DullStrangle:	//盘整（宽）
		return IDS_DullStrangle;
	case T_DullRiseCall:	//盘涨Call
		return IDS_DullRiseCall;
	case T_DullRisePut:		//盘涨Put
		return IDS_DullRisePut;
	case T_DullFallCall:	//盘跌Call
		return IDS_DullFallCall;
	case T_DullFallPut:		//盘跌Put
		return IDS_DullFallPut;
	case T_AutoGroup:		//自组
		return IDS_AutoGroup;
	default:
		return IDS_GroupType;
	}
}
bool COrderOption::GetSeriNo(const char* sQuote)
{
	string  sStr = sQuote;
	size_t npos =  sStr.find_last_of('|');
	if (npos != string::npos)
	{
		size_t npos1 = sStr.find('P', npos);
		if (npos1 != string::npos)
			memcpy(m_SeriesNo, (sStr.substr(0, npos1)).c_str(), sizeof(SOptionSeriesNoType));
		else
		{
			npos1 = sStr.find('C', npos);
			if (npos1 != string::npos)
				memcpy(m_SeriesNo, (sStr.substr(0, npos1)).c_str(), sizeof(SOptionSeriesNoType));
			else
			{
				memset(m_SeriesNo, 0, sizeof(SOptionSeriesNoType));
				return false;
			}
		}
	}
	else
	{
		memset(m_SeriesNo, 0, sizeof(SOptionSeriesNoType));
		return false;
	}
	return true;
}
void COrderOption::UpdateOptionInfo(int nSize, TOptionInfo info[N_5_row], int nType)
{	
	//原有信息
	set<wstring> OldOptionInfoMap;
	for (int i=0;i<m_nOptionRows;i++)
		OldOptionInfoMap.insert(m_strItem[i + 1][COO_Code]);
	int OldType = m_CurType;
	wstring OldOptionSerious = m_strSpecialItem[SPI_Option];

	//更新
    g_pStarApi->UnsubAllQuote(this);
	memset(m_QuoteTradeCode, 0, sizeof(m_QuoteTradeCode));
	memset(m_strSpecialItem[SPI_Option], 0, sizeof(TMouPriceTypeStrW));
	m_CurType = nType;
	
	//情境类型
	wcscpy_s(m_strSpecialItem[SPI_Type], LoadResWchar_t(GetTypeStrId(m_CurType)));

	for (int i=0;i<nSize&&i<N_5_row;i++)
	{
		string sCode = "";
		string  sQuote = info[i].Contract;
		CConvert::QContractNoTypeToStr(sQuote, sCode);
		size_t nPos = -1;
		nPos = sCode.find_last_of(' ');
		if (nPos!=string::npos)
		{
			//看涨看跌 执行价
			wcscpy_s(m_strItem[i + 1][COO_Code], LoadRC::ansi_to_unicode(sCode.substr(nPos+1, sCode.size()-nPos)).c_str());
			//标的合约
			if (i == 0)
			{
				wcscpy_s(m_strSpecialItem[SPI_Option], LoadRC::ansi_to_unicode(sCode.substr(0, nPos)).c_str());
				UpdateCommodityInfo(sCode.c_str());
				GetSeriNo(sQuote.c_str());
			}
			//买卖
			wcscpy_s(m_strItem[i + 1][COO_Direct], info[i].Direct == dBuy ? LoadResWchar_t(IDS_BuyList) : LoadResWchar_t(IDS_SellList));
			//行情 交易合约
			strcpy_s(m_QuoteTradeCode[EQT_Quote][i], info[i].Contract);
			strcpy_s(m_QuoteTradeCode[EQT_Trade][i], sCode.c_str());		
		}	
	}
	m_nOptionRows = nSize < N_5_row ? nSize : N_5_row;

	int nret = g_pStarApi->SubQuote(m_QuoteTradeCode[EQT_Quote], m_nOptionRows, this);

	if ((m_nOptionRows == 0 || m_CurType != T_AutoGroup) && m_OptionTarget.bAdd)
		m_OptionTarget.bAdd = false;			

	//跨式宽跨式判断
	m_nSTDSTG = N_OPTION_NONE;
	if (m_CurType == T_AutoGroup)
	{
		if (nSize == 2 && info[0].Direct == info[1].Direct)
		{
			if (m_strItem[1][COO_Code][0]>m_strItem[2][COO_Code][0])
			{
				if (wcscmp(m_strItem[1][COO_Code] + 1, m_strItem[2][COO_Code] + 1) < 0)
					m_nSTDSTG = N_OPTION_STG;
				else if (wcscmp(m_strItem[1][COO_Code] + 1, m_strItem[2][COO_Code] + 1) == 0)
					m_nSTDSTG = N_OPTION_STD;
			}
			if(m_strItem[1][COO_Code][0]<m_strItem[2][COO_Code][0])
			{
				if (wcscmp(m_strItem[1][COO_Code] + 1, m_strItem[2][COO_Code] + 1) > 0)
					m_nSTDSTG = N_OPTION_STG;
				else if (wcscmp(m_strItem[1][COO_Code] + 1, m_strItem[2][COO_Code] + 1) == 0)
					m_nSTDSTG = N_OPTION_STD;
			}
		}
	}
	else if (m_CurType>= T_Dull&&m_CurType <= T_BreakStrangle)
		m_nSTDSTG = (m_CurType == T_Dull || m_CurType == T_Break)? N_OPTION_STD: N_OPTION_STG;
	//更新标的行情
	LayOut();

	//前次信息比较 更新界面数据
	if (OldOptionInfoMap.size()== m_nOptionRows && OldType==m_CurType)
	{
		for (int i = 0; i<m_nOptionRows; i++)
		{
			set<wstring>::iterator it = OldOptionInfoMap.find(m_strItem[i + 1][COO_Code]);
			if (it== OldOptionInfoMap.end())
			{
				ResetSyncState();
				break;
			}
		}
	}
	else
		ResetSyncState();
	//刷新
	::RedrawWindow(m_Hwnd, 0, 0, RDW_INVALIDATE | RDW_UPDATENOW);	
}
bool COrderOption::GetOrderInfo(int i,TSendOrder &sOrder, bool bShowMessage)
{
	CConvert::DefaultSendOrder(sOrder);
	strcpy_s(sOrder.Sign, m_Sign);
	sOrder.Direct = wcscmp(m_strItem[i][COO_Direct], LoadResWchar_t(IDS_BuyList)) == 0 ? dBuy : dSell;
	if(i==N_Target_row)
		CConvert::TContractNoToTContractKey(m_OptionTarget.Code[EQT_Trade], sOrder);
	else
		CConvert::TContractNoToTContractKey(m_QuoteTradeCode[EQT_Trade][i - 1], sOrder);
	if (!CConvert::IsValidTradeCommodity(sOrder))
	{
        swprintf_s(m_strInfo, L"%s:%s", m_strItem[i][COO_Code], LoadResWchar_t(IDS_STRING1109));
        if (bShowMessage)
        {
            MessageBox(m_Hwnd, LoadResWchar_t(IDS_STRING1109), m_strItem[i][COO_Code], NULL);
        }
		
		return false;
	}

	if (!GetOrderPrice(i, sOrder.OrderPrice, bShowMessage))
		return false;

	if (!GetOrderQty(i, sOrder.OrderQty, bShowMessage))
		return false;

	if (!GetOffset(i, sOrder.Offset))
		return false;

	GetValidType(sOrder.ValidType);

	sOrder.OrderType = otLimit;

	if (IsWindowVisible(m_AddOne.GetHwnd()) && m_AddOne.GetCheck() == BST_CHECKED)
		sOrder.AddOneIsValid = tsNight;

	return true;
}
void COrderOption::SendOptionStrategy()
{
	//联动登录窗口
	if (m_opr->TradeLoginLinkage())
	{
		m_bTracking = false;
		m_nCurHoverID = -1;
		InvalidateRect(m_Hwnd, NULL, TRUE);
		UpdateWindow(m_Hwnd);
		return;
	}

	//获取服务器标识 判断交易合约有效性
	m_opr->GetSign(m_Sign);
	vector<TSendOrder> vOrder;
	TSendOrder sOrder;
	
	if (IsWindowVisible(m_ExchangeSupport.GetHwnd())&&m_ExchangeSupport.GetCheck()&& m_nSTDSTG && m_nOptionRows == N_2_row)
	{
		if (!GetOrderInfo(N_1_row, sOrder))
			return;
		TSendOrder sOrder2;
		if (!GetOrderInfo(N_2_row, sOrder2))
			return;
		if (sOrder2.OrderQty != sOrder.OrderQty)
		{
            wcscpy_s(m_strInfo, LoadResWchar_t(IDS_QtyNeedSame));
			MessageBox(m_Hwnd, LoadResWchar_t(IDS_QtyNeedSame), LoadResWchar_t(IDS_TIP), NULL);
			return;
		}

		TSendOrder sOrderTemp;

		if (sOrder.OptionType == otCall)
		{
			memcpy(&sOrderTemp, &sOrder, sizeof(TSendOrder));
			sOrderTemp.OptionType2 = sOrder2.OptionType;
			strcpy_s(sOrderTemp.StrikePrice2, sOrder2.StrikePrice);
			strcpy_s(sOrderTemp.ContractNo2, sOrder2.ContractNo);
		}
		else
		{
			memcpy(&sOrderTemp, &sOrder2, sizeof(TSendOrder));
			sOrderTemp.OptionType2 = sOrder.OptionType;
			strcpy_s(sOrderTemp.StrikePrice2, sOrder.StrikePrice);
			strcpy_s(sOrderTemp.ContractNo2, sOrder.ContractNo);
		}
		sOrderTemp.OrderPrice = sOrder.OrderPrice + sOrder2.OrderPrice;

		if (m_nSTDSTG == N_OPTION_STD)
			sOrderTemp.CommodityType = ctSTD;
		else
			sOrderTemp.CommodityType = ctSTG;

		if (CConvert::IsValidTradeCommodity(sOrderTemp))
			vOrder.push_back(sOrderTemp);
		else
		{
			string sCode = "";
			CConvert::TContractKeyToTContractNo(sOrderTemp, sCode);
			TMouFormatPriceStrW wstr = { 0 };
			swprintf_s(wstr, L"%s%s", LoadRC::ansi_to_unicode(sCode).c_str(), LoadResWchar_t(IDS_NotExchangeSupport));
            swprintf_s(m_strInfo, L"%s%s", LoadRC::ansi_to_unicode(sCode).c_str(), LoadResWchar_t(IDS_NotExchangeSupport));
			MessageBox(m_Hwnd, wstr, LoadResWchar_t(IDS_TIP), NULL);
			return;
		}
	}
	else
	{
		for (int i = N_1_row; i <= m_nOptionRows; i++)
		{
			if (!GetOrderInfo(i, sOrder))
				return;
			vOrder.push_back(sOrder);
		}
	}

	if (m_OptionTarget.bAdd)
	{	
		if (!GetOrderInfo(N_Target_row, sOrder))
			return;
		vOrder.push_back(sOrder);
	}
	
	for (vector<TSendOrder>::iterator it= vOrder.begin();it!= vOrder.end();it++)
		m_opr->OptionSendOrder(*it);
	
}
bool COrderOption::GetOrderPrice(int nRow,double &dprice, bool bShowMessage)//重新取（实时）  还是  直接从表格取（直观）
{
	if (wcscmp(m_strItem[nRow][COO_PriceType],LoadResWchar_t(IDS_LIMIT_PRICE))==0)
	{
		dprice = _wtof(m_strItem[nRow][COO_Price]);
		if (CConvert::DoubleSameValue(0, dprice, FLOAT_SAME_ERRORDIFF))
		{
            swprintf_s(m_strInfo, L"%s:%s", m_strItem[nRow][COO_Code], LoadResWchar_t(IDS_STRING1076));
            if (bShowMessage)
            {
                MessageBox(m_Hwnd, LoadResWchar_t(IDS_STRING1076), m_strItem[nRow][COO_Code], NULL);
            }
			return false;
		}
	}
	else
	{
		dprice = _wtof(m_strItem[nRow][COO_Price]);
		if (CConvert::DoubleSameValue(0, dprice, FLOAT_SAME_ERRORDIFF))
		{
            swprintf_s(m_strInfo, L"%s:%s", m_strItem[nRow][COO_Code], LoadResWchar_t(IDS_STRING1076));
            if (bShowMessage)
            {
                MessageBox(m_Hwnd, LoadResWchar_t(IDS_STRING1076), m_strItem[nRow][COO_Code], NULL);
            }
			return false;
		}
	}
	return true;
}
bool COrderOption::GetOrderQty(int nRow, uint &uQty, bool bShowMessage)
{
	uQty = _wtoi(m_strItem[nRow][COO_Qty]);
	if (uQty == 0)
	{
        swprintf_s(m_strInfo, L"%s:%s", m_strItem[nRow][COO_Code], LoadResWchar_t(IDS_STRING1110));
        if (bShowMessage)
        {
            MessageBox(m_Hwnd, LoadResWchar_t(IDS_STRING1110), m_strItem[nRow][COO_Code], NULL);
        }
		return false;
	}
	else
		return true;
}
bool COrderOption::GetOffset(int nRow, TOffset &Offset)
{
	if (m_OptionCommodityInfo.cCoverMode == oNone)
		Offset = oNone;
	else if (wcscmp(m_strSpecialItem[SPI_OC], LoadResWchar_t(IDS_OPEN))==0)
		Offset = oOpen;
	else if (wcscmp(m_strSpecialItem[SPI_OC], LoadResWchar_t(IDS_CLOSE))==0)
		Offset = oCover;
	else if (wcscmp(m_strSpecialItem[SPI_OC], LoadResWchar_t(IDS_CLOSE_DAY))==0)
		Offset = oCoverT;
	else
		return false;
	return true;
}
void COrderOption::GetValidType(TValidType &type)
{
	type = CMouseString::WStrToValidType(m_strSpecialItem[SPI_Valid]);
}

void COrderOption::UpdatePreErrorMsg(int nErrCode, std::string strContractCode)
{
    switch (nErrCode)
    {
    case -1:
        swprintf_s(m_strInfo, L"%s%s", LoadRC::ansi_to_unicode(strContractCode).c_str(), LoadResWchar_t(IDS_UnsupportType));
        break;
    case -2:
        swprintf_s(m_strInfo, L"%s%s", LoadRC::ansi_to_unicode(strContractCode).c_str(), LoadResWchar_t(IDS_NonParam));
        break;
    case -3:
        swprintf_s(m_strInfo, L"%s%s", LoadRC::ansi_to_unicode(strContractCode).c_str(), LoadResWchar_t(IDS_NonCommodity));
        break;
    case -4:
        swprintf_s(m_strInfo, L"%s%s", LoadRC::ansi_to_unicode(strContractCode).c_str(), LoadResWchar_t(IDS_NonQuote));
        break;
    case -6:
        swprintf_s(m_strInfo, L"%s%s", LoadRC::ansi_to_unicode(strContractCode).c_str(), LoadResWchar_t(IDS_InvalidTrade));
        break;
    default:
        wcscpy_s(m_strInfo, L"");
    }
    
}

void COrderOption::UpdatePreFreeze()
{
    /// 获取服务器标识，判断合约有效性
    if (m_nOptionRows == 0 || (wcscmp(m_strSpecialItem[SPI_OC], LoadResWchar_t(IDS_OPEN)) != 0))
    {
        wcscpy_s(m_strInfo, L"");
        InvalidateRect(m_Hwnd, &m_rcSpecialItem[SPI_Info], TRUE);
        return;
    }

    //OutputDebugString(L"update prefreeze");

    m_opr->GetSign(m_Sign);
    double dPreFreeze = 0;
    TSendOrder sOrder;
    m_bInfoError = true;
    if (IsWindowVisible(m_ExchangeSupport.GetHwnd()) &&
        m_ExchangeSupport.GetCheck() &&
        m_nSTDSTG && m_nOptionRows == N_2_row)
    {
        if (!GetOrderInfo(N_1_row, sOrder, false))
        {
            return;
        }

        TSendOrder sOrder2;
        if (!GetOrderInfo(N_2_row, sOrder2, false))
        {
            return;
        }
        if (sOrder.OrderQty != sOrder2.OrderQty)
        {
            wcscpy_s(m_strInfo, LoadResWchar_t(IDS_QtyNeedSame));
            InvalidateRect(m_Hwnd, &m_rcSpecialItem[SPI_Info], TRUE);
            return;
        }

        TSendOrder sOrderTemp;
        if (sOrder.OptionType == otCall)
        {
            memcpy(&sOrderTemp, &sOrder, sizeof(TSendOrder));
            sOrderTemp.OptionType2 = sOrder2.OptionType;
            strcpy_s(sOrderTemp.StrikePrice2, sOrder2.StrikePrice);
            strcpy_s(sOrderTemp.ContractNo2, sOrder2.ContractNo);
        }
        else
        {
            memcpy(&sOrderTemp, &sOrder2, sizeof(TSendOrder));
            sOrderTemp.OptionType2 = sOrder.OptionType;
            strcpy_s(sOrderTemp.StrikePrice2, sOrder.StrikePrice);
            strcpy_s(sOrderTemp.ContractNo2, sOrder.ContractNo);
        }
        sOrderTemp.OrderPrice = sOrder.OrderPrice + sOrder2.OrderPrice;

        if (m_nSTDSTG == N_OPTION_STD)
            sOrderTemp.CommodityType = ctSTD;
        else
            sOrderTemp.CommodityType = ctSTG;

        if (CConvert::IsValidTradeCommodity(sOrderTemp))
        {
            m_opr->GetOrderUser(sOrderTemp.UserNo, false);

            if (sOrderTemp.Offset == otNone)
            {
                wcscpy_s(m_strInfo, L"");
                InvalidateRect(m_Hwnd, &m_rcSpecialItem[SPI_Info], TRUE);
                return;
            }

            int result = g_pTradeData->GetPreFrozen(sOrderTemp, dPreFreeze);
            if (result == 0)
            {
                swprintf_s(m_strInfo, L"%s: %0.2lf", LoadResWchar_t(IDS_PreFreeze), dPreFreeze);
                //OutputDebugString(m_strInfo);
                m_bInfoError = false;
            }
            else
            {
                string sCode = "";
                CConvert::TContractKeyToTContractNo(sOrderTemp, sCode);
                UpdatePreErrorMsg(result, sCode);
                InvalidateRect(m_Hwnd, &m_rcSpecialItem[SPI_Info], TRUE);
                return;
            }
        }
        else
        {
            string sCode = "";
            CConvert::TContractKeyToTContractNo(sOrderTemp, sCode);
            TMouFormatPriceStrW wstr = { 0 };
            swprintf_s(m_strInfo, L"%s%s", LoadRC::ansi_to_unicode(sCode).c_str(), LoadResWchar_t(IDS_NotExchangeSupport));
            InvalidateRect(m_Hwnd, &m_rcSpecialItem[SPI_Info], TRUE);
            return;
        }
    }
    else
    {
        dPreFreeze = 0;
        double dValue = 0;
        for (int i = N_1_row; i <= m_nOptionRows; i++)
        {
            if (!GetOrderInfo(i, sOrder, false))
            {
                return;
            }

            if (sOrder.Offset == otNone)
            {
                wcscpy_s(m_strInfo, L"");
                InvalidateRect(m_Hwnd, &m_rcSpecialItem[SPI_Info], TRUE);
                return;
            }
            
            m_opr->GetOrderUser(sOrder.UserNo, false);
            int result = g_pTradeData->GetPreFrozen(sOrder, dValue);
            if (result == 0)
            {
                dPreFreeze += dValue;
            }
            else
            {
                string sCode = "";
                CConvert::TContractKeyToTContractNo(sOrder, sCode);
                UpdatePreErrorMsg(result, sCode);
                InvalidateRect(m_Hwnd, &m_rcSpecialItem[SPI_Info], TRUE);
                return;
            }
        }
        if (m_OptionTarget.bAdd)
        {
            if (!GetOrderInfo(N_Target_row, sOrder, false))
            {
                return;
            }
            m_opr->GetOrderUser(sOrder.UserNo, false);
            int result = g_pTradeData->GetPreFrozen(sOrder, dValue);
            if (result == 0)
            {
                dPreFreeze += dValue;
            }
            else
            {
                string sCode = "";
                CConvert::TContractKeyToTContractNo(sOrder, sCode);
                UpdatePreErrorMsg(result, sCode);
                InvalidateRect(m_Hwnd, &m_rcSpecialItem[SPI_Info], TRUE);
                return;
            }
        }
        swprintf_s(m_strInfo, L"%s: %0.2lf", LoadResWchar_t(IDS_PreFreeze), dPreFreeze);
        m_bInfoError = false;
    }

    InvalidateRect(m_Hwnd, &m_rcSpecialItem[SPI_Info], TRUE);
}

void COrderOption::LayOut()
{
	for (int i=0;i<m_nOptionRows+1;i++)
	{
		int nLeft = 1;
		for (int j=0;j<COO_END;j++)
		{
			m_rcItem[i][j].top = (i+1)*N_ROW_HEIGHT + 1;
			m_rcItem[i][j].bottom = (i+2)*N_ROW_HEIGHT;
			m_rcItem[i][j].left = nLeft;
			int nWidth = N_40_WIDTH;
			switch (j)
			{		
			case COO_Price:	
				nWidth = N_100_WIDTH;
				break;
			case COO_Code:
			case COO_Qty:
			case COO_PriceType:	
			case COO_Delta:	
			case COO_Gamma:
				nWidth = N_65_WIDTH;
				break;
			}
			m_rcItem[i][j].right = nLeft + nWidth;
			nLeft = m_rcItem[i][j].right + 1;
		}
		m_nWidth = nLeft;
	}
	int nAdd = 2;
	if (m_nOptionRows>0&&m_OptionTarget.bAdd)
	{		
		memcpy(m_rcItem[N_Target_row], m_rcItem[N_1_row], sizeof(RECT)*COO_END);
		for (int j = 0; j<COO_END; j++)
		{
			m_rcItem[N_Target_row][j].top = (m_nOptionRows + nAdd)*N_ROW_HEIGHT + 1;
			m_rcItem[N_Target_row][j].bottom = (m_nOptionRows + 1 + nAdd)*N_ROW_HEIGHT;
		}
		nAdd++;
	}
	int nInterval = 5;
	for (int i=SPI_Reset;i>= SPI_Type;i--)
	{
		switch (i)
		{
		case SPI_Reset:
			m_rcSpecialItem[i].right = m_nWidth;
			m_rcSpecialItem[i].left = m_nWidth - N_80_WIDTH;
			m_rcSpecialItem[i].top = 1;
			m_rcSpecialItem[i].bottom = N_ROW_HEIGHT - 1;
			break;
		case SPI_BackDirect:
			m_rcSpecialItem[i] = m_rcSpecialItem[i + 1];
			m_rcSpecialItem[i].right = m_rcSpecialItem[i + 1].left - nInterval;
			m_rcSpecialItem[i].left = m_rcSpecialItem[i].right - N_65_WIDTH;
			break;
		case SPI_Type:
			m_rcSpecialItem[i] = m_rcSpecialItem[i + 1];
			m_rcSpecialItem[i].left = 0;
			m_rcSpecialItem[i].right = N_65_WIDTH;
			break;
		case SPI_Option:
			m_rcSpecialItem[i] = m_rcSpecialItem[i+1];
			m_rcSpecialItem[i].right = m_rcSpecialItem[i + 1].left - nInterval;
			m_rcSpecialItem[i].left = m_rcSpecialItem[i].right - 150 - 10;
			break;
		default:
			m_rcSpecialItem[i] = m_rcSpecialItem[i+1];
			m_rcSpecialItem[i].right = m_rcSpecialItem[i+1].left - nInterval;
			m_rcSpecialItem[i].left = m_rcSpecialItem[i].right - N_80_WIDTH ;
			break;
		}
	}

	m_rcSpecialItem[SPI_Order].left = m_nWidth- N_80_WIDTH;
	m_rcSpecialItem[SPI_Order].right = m_nWidth;
	m_rcSpecialItem[SPI_Order].top = (m_nOptionRows + nAdd)*N_ROW_HEIGHT + 2;
	m_rcSpecialItem[SPI_Order].bottom = m_rcSpecialItem[SPI_Order].top + N_ROW_HEIGHT-2;
	m_rcSpecialItem[SPI_Valid] = m_rcSpecialItem[SPI_Order];
	m_rcSpecialItem[SPI_Valid].left = m_rcSpecialItem[SPI_Order].left - N_80_WIDTH - nInterval;
	m_rcSpecialItem[SPI_Valid].right = m_rcSpecialItem[SPI_Valid].left + N_80_WIDTH;
	m_rcSpecialItem[SPI_OC] = m_rcSpecialItem[SPI_Valid];
	m_rcSpecialItem[SPI_OC].left = m_rcSpecialItem[SPI_Valid].left - N_65_WIDTH - nInterval;
	m_rcSpecialItem[SPI_OC].right = m_rcSpecialItem[SPI_OC].left + N_65_WIDTH;
    m_rcSpecialItem[SPI_Info].left = 4;
    m_rcSpecialItem[SPI_Info].top = m_rcSpecialItem[SPI_Order].bottom + 2;
    m_rcSpecialItem[SPI_Info].right = m_nWidth;
    m_rcSpecialItem[SPI_Info].bottom = m_rcSpecialItem[SPI_Info].top + N_ROW_HEIGHT - 2;
	
	if (m_OptionCommodityInfo.cCoverMode==oCover&&(m_nSTDSTG && !m_OptionTarget.bAdd)&&strstr(m_SeriesNo, exZCE))
	{
		MoveWindow(m_hControl[OC_Support_Check], 1, m_rcSpecialItem[SPI_Order].top+2, N_100_WIDTH + N_40_WIDTH, 20, TRUE);
		ShowWindow(m_hControl[OC_Support_Check], SW_SHOW);
        EnableWindow(m_SyncQtyBtn.GetHwnd(), m_ExchangeSupport.GetCheck() != BST_CHECKED);    /// "交易所支持"要求数量必须同步，当OC_Support_Check显示且checked时禁用数量同步button
		if (wcscmp(m_strSpecialItem[SPI_Valid], LoadResWchar_t(IDS_IMMEDIATE_ALL)) && wcscmp(m_strSpecialItem[SPI_Valid], LoadResWchar_t(IDS_IMMEDIATE_PART)))
			wcscpy_s(m_strSpecialItem[SPI_Valid], LoadResWchar_t(IDS_IMMEDIATE_ALL));
	}
	else
	{
		ShowWindow(m_hControl[OC_Support_Check], SW_HIDE);
        EnableWindow(m_SyncQtyBtn.GetHwnd(), TRUE);     /// 不是组合类型时Enable数量同步button
		if (wcscmp(m_strSpecialItem[SPI_Valid], LoadResWchar_t(IDS_IMMEDIATE_ALL))==0 || wcscmp(m_strSpecialItem[SPI_Valid], LoadResWchar_t(IDS_IMMEDIATE_PART))==0)
			wcscpy_s(m_strSpecialItem[SPI_Valid], LoadResWchar_t(IDS_EFFECTIVE_THEDAY));
	}

	if (m_OptionCommodityInfo.bAddOne)
	{
		MoveWindow(m_hControl[OC_AddOne_Check], 1, m_rcSpecialItem[SPI_Order].top + 2, N_100_WIDTH, 20, TRUE);
		ShowWindow(m_hControl[OC_AddOne_Check], SW_SHOW);
	}
	else
		ShowWindow(m_hControl[OC_AddOne_Check], SW_HIDE);

	RECT rc(m_rcItem[N_Head_row][COO_PriceType]);
	MoveWindow(m_hControl[OC_PriceSync_Btn], rc.right - N_SPIN_WIDTH -1, rc.top+3, N_SPIN_WIDTH, N_SPIN_WIDTH, TRUE);
	rc = m_rcItem[N_Head_row][COO_Qty];
	MoveWindow(m_hControl[OC_QtySync_Btn], rc.right - N_SPIN_WIDTH -1, rc.top+3, N_SPIN_WIDTH, N_SPIN_WIDTH, TRUE);	
	ShowWindow(m_hControl[OC_PriceSync_Btn], SW_SHOW);
	ShowWindow(m_hControl[OC_QtySync_Btn], SW_SHOW);

    /// 高度加上下单和信息栏的高度
	SetWindowPos(m_Hwnd, NULL, 0, 0, m_nWidth, (m_nOptionRows + nAdd+2)*N_ROW_HEIGHT + 1, SWP_NOZORDER|SWP_NOMOVE);
}

LRESULT COrderOption::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:				OnCreate(wParam, lParam);
		break;
	case WM_COMMAND:			OnCommond(wParam, lParam);
		break;
	case WM_NOTIFY:				OnNotify(wParam, lParam);
		break;
	case WM_CTLCOLORSTATIC:		return OnCtlColorStatic(wParam, lParam);

	case WM_PAINT:				OnPaint(wParam, lParam);
		break;
	case WM_SIZE:				OnSize(wParam, lParam);
		break;
	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:		OnLButtonDown(wParam, lParam);
		break;
	case WM_LBUTTONUP:			OnLButtonUp(wParam, lParam);
		break;
	case WM_MOUSEMOVE:			OnMouseMove(wParam, lParam);
		break;
	case WM_MOUSELEAVE:			OnMouseLeave(wParam, lParam);
		break;
	case WM_MOUSEWHEEL:			OnMouseWheel(wParam, lParam);
		break;
	case WM_ERASEBKGND:
		break;
	case WM_DESTROY:			OnDestory(wParam, lParam);
		break;
	case WM_DELOPTIONQUOTE:		UpdatePrice();
		break;
	default:
		return NOT_PROCESSED;
	}
	return PROCESSED;
}
void COrderOption::OnCreate(WPARAM wParam, LPARAM lParam)
{	
	//控件
	HINSTANCE	m_hInstance = GetModuleHandle(NULL);
	m_hControl[OC_PriceSync_Btn] = CreateWindowEx(0, L"button", L"", WS_CHILD | WS_CLIPSIBLINGS | WS_TABSTOP, 0, 0, 22, 22, m_Hwnd, (HMENU)OC_PriceSync_Btn, m_hInstance, NULL);
	m_hControl[OC_QtySync_Btn] = CreateWindowEx(0, L"button", L"", WS_CHILD | WS_CLIPSIBLINGS | WS_TABSTOP, 0, 0, 22, 22, m_Hwnd, (HMENU)OC_QtySync_Btn, m_hInstance, NULL);
	m_SyncPriceTypeBtn.Init(m_hControl[OC_PriceSync_Btn],0);	//价格类型
	m_SyncQtyBtn.Init(m_hControl[OC_QtySync_Btn],0);		//数量联动
	
	m_hControl[OC_Price_Edit] = CreateWindowEx(0, L"edit", L"", ES_AUTOHSCROLL | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_CLIPSIBLINGS, 0, 0, 0, 0, m_Hwnd, (HMENU)OC_Price_Edit, m_hInstance, NULL);
	m_hControl[OC_Price_Spin] = CreateWindowEx(0, L"msctls_updown32", L"spin", WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_CLIPSIBLINGS, 0, 0, 0, 0, m_Hwnd, (HMENU)OC_Price_Spin, m_hInstance, NULL);
	m_SpinPrice.Init(m_hControl[OC_Price_Spin]);
	SendMessage(m_hControl[OC_Price_Edit], WM_SETFONT, (WPARAM)g_FontData.GetFontNumEx17(), (LPARAM)TRUE);
	SendMessage(m_hControl[OC_Price_Edit], EM_SETLIMITTEXT, (WPARAM)10, 0);
	m_EditProc = (WNDPROC)SetWindowLong(m_hControl[OC_Price_Edit], GWL_WNDPROC, (LONG)OrderOptionEditWndProc);
	SetWindowLong(m_hControl[OC_Price_Edit], GWL_USERDATA, LONG(this));
	m_hControl[OC_Qty_Edit] = CreateWindowEx(0, L"edit", L"1", ES_AUTOHSCROLL | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_CLIPSIBLINGS, 0, 0, 0, 0, m_Hwnd, (HMENU)OC_Qty_Edit, m_hInstance, NULL);
	m_hControl[OC_Qty_Spin] = CreateWindowEx(0, L"msctls_updown32", L"spin", WS_CHILD | UDS_ALIGNRIGHT | UDS_AUTOBUDDY, 0, 0, 0, 0, m_Hwnd, (HMENU)OC_Qty_Spin, m_hInstance, NULL);
	m_SpinQty.Init(m_hControl[OC_Qty_Spin]);
	SendMessage(m_hControl[OC_Qty_Edit], WM_SETFONT, (WPARAM)g_FontData.GetFontNumEx17(), (LPARAM)TRUE);
	SendMessage(m_hControl[OC_Qty_Edit], EM_SETLIMITTEXT, (WPARAM)3, 0);
	m_EditProc2 = (WNDPROC)SetWindowLong(m_hControl[OC_Qty_Edit], GWL_WNDPROC, (LONG)OrderOptionEditWndProc);
	SetWindowLong(m_hControl[OC_Qty_Edit], GWL_USERDATA, LONG(this));

	m_hControl[OC_Support_Check] = CreateWindowEx(0, L"button", LoadResWchar_t(IDS_ExchengSupport), BS_AUTOCHECKBOX | WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS, 0, 0, 0, 0, m_Hwnd, (HMENU)OC_Support_Check, m_hInstance, NULL);
	m_ExchangeSupport.Init(m_hControl[OC_Support_Check],g_ColorRefData.GetColorBackground());
	SendMessage(m_hControl[OC_Support_Check], WM_SETFONT, (WPARAM)g_FontData.GetFontWord13(), (LPARAM)TRUE);
	
	m_hControl[OC_AddOne_Check] = CreateWindowEx(0, L"button", L"HK T+1", BS_AUTOCHECKBOX | WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS, 0, 0, 0, 0, m_Hwnd, (HMENU)OC_AddOne_Check, m_hInstance, NULL);
	m_AddOne.Init(m_hControl[OC_AddOne_Check], g_ColorRefData.GetColorBackground());
	SendMessage(m_hControl[OC_AddOne_Check], WM_SETFONT, (WPARAM)g_FontData.GetFontWord13(), (LPARAM)TRUE);

	//列头内容
	wcscpy_s(m_strItem[N_Head_row][COO_Direct], LoadResWchar_t(IDS_BS));
	wcscpy_s(m_strItem[N_Head_row][COO_Code], LoadResWchar_t(IDS_CodeShort));
	wcscpy_s(m_strItem[N_Head_row][COO_PriceType], LoadResWchar_t(IDS_Type));
	wcscpy_s(m_strItem[N_Head_row][COO_Price], LoadResWchar_t(IDS_PriceShort));
	wcscpy_s(m_strItem[N_Head_row][COO_Qty], LoadResWchar_t(IDS_QtyShort));
	wcscpy_s(m_strItem[N_Head_row][COO_Delta], L"0");
	wcscpy_s(m_strItem[N_Head_row][COO_Gamma], L"0");

	wcscpy_s(m_strSpecialItem[SPI_Type], LoadResWchar_t(IDS_GroupType));
	wcscpy_s(m_strSpecialItem[SPI_Option], L"");
	wcscpy_s(m_strSpecialItem[SPI_BackDirect], LoadResWchar_t(IDS_BackDirect));
	wcscpy_s(m_strSpecialItem[SPI_AddTarget], LoadResWchar_t(IDS_AddTarget));
	wcscpy_s(m_strSpecialItem[SPI_Reset], LoadResWchar_t(IDS_Reset));

	wcscpy_s(m_strSpecialItem[SPI_Order], LoadResWchar_t(IDS_Order));
	wcscpy_s(m_strSpecialItem[SPI_Valid], LoadResWchar_t(IDS_EFFECTIVE_THEDAY));
	wcscpy_s(m_strSpecialItem[SPI_OC], LoadResWchar_t(IDS_OPEN));
	//初始化 价格模式 数量
	for (int i = N_1_row; i < N_Max_Row; i++)
	{
		wcscpy_s(m_strItem[i][COO_PriceType], LoadResWchar_t(IDS_STRING1154));
		wcscpy_s(m_strItem[i][COO_Qty], L"1");
	}
	
	//价格模式
	TMouPriceTypeStrW m_vstring[10] = { 0 };
	int nsize = 0;
	memcpy_s(m_vstring[nsize++], sizeof(TMouPriceTypeStrW), LoadResWchar_t(IDS_LIMIT_PRICE), sizeof(TMouPriceTypeStrW));
	memcpy_s(m_vstring[nsize++], sizeof(TMouPriceTypeStrW), LoadResWchar_t(IDS_STRING1122), sizeof(TMouPriceTypeStrW));
	memcpy_s(m_vstring[nsize++], sizeof(TMouPriceTypeStrW), LoadResWchar_t(IDS_STRING1154), sizeof(TMouPriceTypeStrW));
	memcpy_s(m_vstring[nsize++], sizeof(TMouPriceTypeStrW), LoadResWchar_t(IDS_STRING1155), sizeof(TMouPriceTypeStrW));
	m_PopListPriceType.CreatePopWin(this, m_Hwnd, N_65_WIDTH, 0, 0, Nref_Pricetype, m_vstring, nsize, nsize, 1);
	nsize = 0;
	memcpy_s(m_vstring[nsize++], sizeof(TMouPriceTypeStrW), LoadResWchar_t(IDS_EFFECTIVE_THEDAY), sizeof(TMouPriceTypeStrW));
	memcpy_s(m_vstring[nsize++], sizeof(TMouPriceTypeStrW), LoadResWchar_t(IDS_IMMEDIATE_ALL), sizeof(TMouPriceTypeStrW));
	memcpy_s(m_vstring[nsize++], sizeof(TMouPriceTypeStrW), LoadResWchar_t(IDS_IMMEDIATE_PART), sizeof(TMouPriceTypeStrW));
	m_PopListValidType.CreatePopWin(this, m_Hwnd, N_80_WIDTH, 0, 0, Nref_Validtype, m_vstring, nsize, nsize, 1);
	nsize = 0;
	memcpy_s(m_vstring[nsize++], sizeof(TMouPriceTypeStrW), LoadResWchar_t(IDS_OPEN), sizeof(TMouPriceTypeStrW));
	memcpy_s(m_vstring[nsize++], sizeof(TMouPriceTypeStrW), LoadResWchar_t(IDS_CLOSE), sizeof(TMouPriceTypeStrW));
	memcpy_s(m_vstring[nsize++], sizeof(TMouPriceTypeStrW), LoadResWchar_t(IDS_CLOSE_DAY), sizeof(TMouPriceTypeStrW));
	m_PopListOCType.CreatePopWin(this, m_Hwnd, N_65_WIDTH, 0, 0, Nref_OCtype, m_vstring, nsize, nsize, 1);

	HDC hdc = GetDC(m_Hwnd);
	m_HdcMem = CreateCompatibleDC(hdc);// bitmap
	ReleaseDC(m_Hwnd, hdc);

	LayOut();

	ResetSyncState();

	SetWindowPos(m_Hwnd, NULL, m_nleft, m_ntop, m_nWidth, (m_nOptionRows + 4)*N_ROW_HEIGHT + 1, SWP_NOZORDER);
}
void COrderOption::OnNotify(WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{
	case OC_Qty_Spin:
		switch (((LPNMHDR)lParam)->code)
		{
		case UDN_DELTAPOS:
			OnSpinDeltaUpDownNormal(-((LPNMUPDOWN)lParam)->iDelta, LOWORD(wParam) - 1);
		}
		break;
	case OC_Price_Spin:	
		switch (((LPNMHDR)lParam)->code)
		{
		case UDN_DELTAPOS:
			OnSpinDeltaUpDownEx(-((LPNMUPDOWN)lParam)->iDelta, LOWORD(wParam) - 1);
		}
		break;
	default:
		break;
	}
}
void COrderOption::OnCommond(WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{
	case OC_Qty_Edit:
		if (HIWORD(wParam) == EN_KILLFOCUS)
			OnKillFocusQtyEdit();
		break;
	case OC_Price_Edit:
		if (HIWORD(wParam) == EN_KILLFOCUS)
			OnKillFocusPriceEdit();
		break;
	case OC_PriceSync_Btn:
		if (HIWORD(wParam) == BN_CLICKED || HIWORD(wParam) == BN_DBLCLK)
			OnButtonPriceSync();
		break;
	case OC_QtySync_Btn:
		if (HIWORD(wParam) == BN_CLICKED || HIWORD(wParam) == BN_DBLCLK)
			OnButtonQtySync();
		break;
	case OC_Support_Check:
		if (HIWORD(wParam) == BN_CLICKED || HIWORD(wParam) == BN_DBLCLK)
			OnCheckSupport();
		break;
	default:
		break;
	}
}
void COrderOption::OnDestory(WPARAM wParam, LPARAM lParam)
{
	DeleteObject(m_hBitmap);
	DeleteDC(m_HdcMem);
}
void COrderOption::OnPaint(WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hDC = BeginPaint(m_Hwnd, &ps);
	SetBkMode(m_HdcMem, TRANSPARENT);

	//背景色
	DrawBackColor();
	//网格线
	DrawGridLine();
	//列表内容
	DrawListContent();
	//特殊区域
	DrawSpecial();

	BitBlt(hDC, 0, 0, m_nWidth, m_nHeight, m_HdcMem, 0, 0, SRCCOPY);

	EndPaint(m_Hwnd, &ps);
}
void COrderOption::OnSize(WPARAM wParam, LPARAM lParam)
{
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	m_nHeight = rect.bottom - rect.top;
	m_nWidth = rect.right - rect.left;
	if (m_hBitmap)
		DeleteObject(m_hBitmap);
	HDC hdc = GetDC(m_Hwnd);
	m_hBitmap = CreateCompatibleBitmap(hdc, m_nWidth, m_nHeight);
	SelectObject(m_HdcMem, m_hBitmap);
	ReleaseDC(m_Hwnd, hdc);
}
LONG COrderOption::OnCtlColorStatic(WPARAM wParam, LPARAM lParam)
{
	SetBkMode((HDC)wParam, TRANSPARENT);
	return (LONG)g_ColorRefData.GetHbrBackground();
}
void COrderOption::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
	POINT pointB;
	pointB.x = GET_X_LPARAM(lParam);
	pointB.y = GET_Y_LPARAM(lParam);

	memset(&m_LClickItem, 0, sizeof(m_LClickItem));
	for (int i=N_1_row;i<=m_nOptionRows;i++)
	{
		if (PtInRect(&m_rcItem[i][COO_PriceType], pointB))
		{
			if ((m_bSyncPrice && N_1_row == i) || !m_bSyncPrice)
			{
				m_LClickItem.nRow = i;
				m_LClickItem.nCol = COO_PriceType;
			}
		}
		else if (PtInRect(&m_rcItem[i][COO_Price], pointB))
		{
			if (wcscmp(LoadResWchar_t(IDS_LIMIT_PRICE), m_strItem[i][COO_PriceType]) == 0)
			{
				m_LClickItem.nRow = i;
				m_LClickItem.nCol = COO_Price;
			}
		}
		else if (PtInRect(&m_rcItem[i][COO_Qty], pointB))
		{
			if ((m_bSyncQty && N_1_row== i) || !m_bSyncQty)
			{
				m_LClickItem.nRow = i;
				m_LClickItem.nCol = COO_Qty;
			}
		}
	}	
	if (m_OptionTarget.bAdd)
	{
		if (PtInRect(&m_rcItem[N_Target_row][COO_PriceType], pointB))
		{
			if (!m_bSyncPrice)
			{
				m_LClickItem.nRow = N_Target_row;
				m_LClickItem.nCol = COO_PriceType;
			}
		}
		else if (PtInRect(&m_rcItem[N_Target_row][COO_Price], pointB))
		{
			if (wcscmp(LoadResWchar_t(IDS_LIMIT_PRICE), m_strItem[N_Target_row][COO_PriceType]) == 0)
			{
				m_LClickItem.nRow = N_Target_row;
				m_LClickItem.nCol = COO_Price;
			}
		}
		else if (PtInRect(&m_rcItem[N_Target_row][COO_Qty], pointB))
		{
			if (!m_bSyncQty)
			{
				m_LClickItem.nRow = N_Target_row;
				m_LClickItem.nCol = COO_Qty;
			}
		}
		else if (PtInRect(&m_rcItem[N_Target_row][COO_Direct], pointB))
		{
			m_LClickItem.nRow = N_Target_row;
			m_LClickItem.nCol = COO_Direct;
		}
	}
	for (int i= SPI_BackDirect;i<SPI_END;i++)
	{
		if (i==SPI_OC&&m_OptionCommodityInfo.cCoverMode == oNone)
			continue;
		if (PtInRect(&m_rcSpecialItem[i], pointB))
		{
			m_LClickItem.nRow = N_Max_Row;
			m_LClickItem.nCol = i;
		}
	}
	
	if (GetFocus() != m_Hwnd)
		SetFocus(m_Hwnd);
}
void COrderOption::OnLButtonUp(WPARAM wParam, LPARAM lParam)
{
	POINT pointB;
	pointB.x = GET_X_LPARAM(lParam);
	pointB.y = GET_Y_LPARAM(lParam);
	if ( m_LClickItem.nRow>0&&(m_LClickItem.nRow<=m_nOptionRows||m_LClickItem.nRow==N_Target_row))
	{
		if (m_LClickItem.nCol == COO_PriceType&&PtInRect(&m_rcItem[m_LClickItem.nRow][COO_PriceType], pointB))
		{
			RECT rect(m_rcItem[m_LClickItem.nRow][COO_PriceType]);
			ClientToScreen(m_Hwnd, LPPOINT(&rect));
			ClientToScreen(m_Hwnd, LPPOINT(&rect) + 1);
			SetWindowPos(m_PopListPriceType.GetHwnd(), NULL, rect.left, rect.bottom, 0, 0, SWP_NOZORDER|SWP_NOSIZE);
			ShowWindow(m_PopListPriceType.GetHwnd(), SW_SHOW);
		}
		else if (m_LClickItem.nCol == COO_Price&&PtInRect(&m_rcItem[m_LClickItem.nRow][COO_Price], pointB))
		{
			SetWindowText(m_hControl[OC_Price_Edit], m_strItem[m_LClickItem.nRow][COO_Price]);
			MoveEdit(m_rcItem[m_LClickItem.nRow][COO_Price].left, m_rcItem[m_LClickItem.nRow][COO_Price].top, m_rcItem[m_LClickItem.nRow][COO_Price].right - m_rcItem[m_LClickItem.nRow][COO_Price].left, m_rcItem[m_LClickItem.nRow][COO_Price].bottom - m_rcItem[m_LClickItem.nRow][COO_Price].top, true);
		}
		else if (m_LClickItem.nCol == COO_Qty&&PtInRect(&m_rcItem[m_LClickItem.nRow][COO_Qty], pointB))
		{
			SetWindowText(m_hControl[OC_Qty_Edit], m_strItem[m_LClickItem.nRow][COO_Qty]);
			MoveEdit(m_rcItem[m_LClickItem.nRow][COO_Qty].left, m_rcItem[m_LClickItem.nRow][COO_Qty].top, m_rcItem[m_LClickItem.nRow][COO_Qty].right - m_rcItem[m_LClickItem.nRow][COO_Qty].left, m_rcItem[m_LClickItem.nRow][COO_Qty].bottom - m_rcItem[m_LClickItem.nRow][COO_Qty].top, false);
		}
		else if (m_LClickItem.nRow == N_Target_row&&m_LClickItem.nCol == COO_Direct&&PtInRect(&m_rcItem[N_Target_row][COO_Direct], pointB))
		{
			if (wcscmp(m_strItem[N_Target_row][COO_Direct], LoadResWchar_t(IDS_BuyList)) == 0)
				wcscpy_s(m_strItem[N_Target_row][COO_Direct], LoadResWchar_t(IDS_SellList));
			else
				wcscpy_s(m_strItem[N_Target_row][COO_Direct], LoadResWchar_t(IDS_BuyList));
			InvalidateRect(m_Hwnd, &m_rcItem[N_Target_row][COO_Direct], TRUE);
			UpdatePrice(N_Target_row);
			if (wcscmp(m_strItem[N_Target_row][COO_Direct],LoadResWchar_t(IDS_BuyList))==0)
			{
				TMouLinkMsgStrA content = { 0 };
				sprintf_s(content, "%s&%s&%c", _AddTarget, m_SeriesNo, dBuy);
				g_mainframe->Linkage(GetParent(), "MouseOrder", _OptionSync, content);
			}
			else
			{
				TMouLinkMsgStrA content = { 0 };
				sprintf_s(content, "%s&%s&%c", _AddTarget, m_SeriesNo, dSell);
				g_mainframe->Linkage(GetParent(), "MouseOrder", _OptionSync, content);
			}
		}
		else 
			memset(&m_LClickItem, 0, sizeof(m_LClickItem));
		
		m_CurShowItem = m_LClickItem;
	}
	else if (m_LClickItem.nRow == N_Max_Row)
	{
		int nCurId = m_LClickItem.nCol;
		RECT rect, rect2;
		HWND hwnd = NULL;
		switch (nCurId)
		{
		case SPI_Valid:
		case SPI_OC:
			rect = m_rcSpecialItem[nCurId];
			ClientToScreen(m_Hwnd, LPPOINT(&rect));
			ClientToScreen(m_Hwnd, LPPOINT(&rect) + 1);
			if (SPI_Valid == nCurId)
				hwnd = m_PopListValidType.GetHwnd();
			else
				hwnd = m_PopListOCType.GetHwnd();
			GetWindowRect(hwnd, &rect2);
			SetWindowPos(hwnd, NULL, rect.left, rect.top - (rect2.bottom - rect2.top), 0, 0, SWP_NOZORDER | SWP_NOSIZE);
			ShowWindow(hwnd, SW_SHOW);
			break;
		case SPI_BackDirect:
			OnBackDirect();
			break;
		case SPI_AddTarget:
			OnAddOptionTarget();
			break;
		case SPI_Reset:
			OnReset();
			break;
		case SPI_Order:
			SendOptionStrategy(); 
			break;
		}
	}
	memset(&m_LClickItem, 0, sizeof(m_LClickItem));
	UpdateWindow(m_Hwnd);
}
void COrderOption::OnMouseMove(WPARAM wParam, LPARAM lParam)
{	
	if (!m_bTracking)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_Hwnd;
		tme.dwFlags = TME_LEAVE | TME_HOVER;
		tme.dwHoverTime = 0;
		m_bTracking = _TrackMouseEvent(&tme) == TRUE;
	}
	POINT pointB;
	pointB.x = GET_X_LPARAM(lParam);
	pointB.y = GET_Y_LPARAM(lParam);

	int nOldID = m_nCurHoverID;
	for (int i= SPI_BackDirect;i<SPI_END;i++)
	{
		if (PtInRect(&m_rcSpecialItem[i], pointB))
		{
			m_nCurHoverID = i;
			if (m_nCurHoverID != nOldID)
			{
				if (nOldID>= SPI_BackDirect&&SPI_Valid<SPI_END)
					InvalidateRect(m_Hwnd,&m_rcSpecialItem[nOldID],TRUE);	
				Redraw(&m_rcSpecialItem[m_nCurHoverID]);
			}
			return;//当前区域
		}
	}
	//仅旧区域刷新
	m_nCurHoverID = -1;
	if (nOldID >= SPI_BackDirect&&nOldID < SPI_END)
		Redraw(&m_rcSpecialItem[nOldID]);
}
void COrderOption::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	m_bTracking = false;
	if (m_nCurHoverID>=0)
	{
		int nOldID = m_nCurHoverID;
		m_nCurHoverID = -1;
		Redraw(&m_rcSpecialItem[nOldID]);
	}
}
void COrderOption::OnMouseWheel(WPARAM wParam, LPARAM lParam)
{
	int nID = GetWindowLong(GetFocus(), GWL_ID);
	switch (nID)
	{
	case OC_Price_Edit:
		OnSpinDeltaUpDownEx(GET_WHEEL_DELTA_WPARAM(wParam), nID);
		break;
	case OC_Qty_Edit:	
		OnSpinDeltaUpDownNormal(GET_WHEEL_DELTA_WPARAM(wParam), nID);
		break;
	}
	return;
}
//价格编辑框失去焦点
void COrderOption::OnKillFocusPriceEdit()
{
	if (IsWindowVisible(m_hControl[OC_Price_Edit]))
	{
		::ShowWindow(m_hControl[OC_Price_Edit], SW_HIDE);
		::ShowWindow(m_hControl[OC_Price_Spin], SW_HIDE);
		TMouPriceTypeStrW str;
		GetWindowText(m_hControl[OC_Price_Edit], str, sizeof(str) / sizeof(wchar_t));
		wcscpy_s(m_strItem[m_CurShowItem.nRow][m_CurShowItem.nCol], str);

        UpdatePreFreeze();
	}
}
//数量编辑框失去焦点
void COrderOption::OnKillFocusQtyEdit()
{
	if (IsWindowVisible(m_hControl[OC_Qty_Edit]))
	{
		::ShowWindow(m_hControl[OC_Qty_Edit], SW_HIDE);
		::ShowWindow(m_hControl[OC_Qty_Spin], SW_HIDE);
		TMouPriceTypeStrW str;
		GetWindowText(m_hControl[OC_Qty_Edit], str, sizeof(str) / sizeof(wchar_t));
		wcscpy_s(m_strItem[m_CurShowItem.nRow][m_CurShowItem.nCol], str);
		if (m_bSyncQty&&m_CurShowItem.nRow== N_1_row)
			UpdateSyncQty();
		else
			UpdatePrice(m_CurShowItem.nRow);

		Redraw();
	}
}
//移动Edit并显示
void COrderOption::MoveEdit(int x, int y, int width, int height, bool bPrice)
{
	if (bPrice)
	{
		if (m_hControl[OC_Price_Edit])
		{
			::MoveWindow(m_hControl[OC_Price_Edit], x, y+2, width - N_SPIN_WIDTH, height-4, TRUE);
			::ShowWindow(m_hControl[OC_Price_Edit], SW_SHOW);
			::MoveWindow(m_hControl[OC_Price_Spin], x + width - N_SPIN_WIDTH, y+1, N_SPIN_WIDTH, height-2, TRUE);
			::ShowWindow(m_hControl[OC_Price_Spin], SW_SHOW);
			if (GetFocus() != m_hControl[OC_Price_Edit])
				SetFocus(m_hControl[OC_Price_Edit]);
		}
	}
	else
	{
		if (m_hControl[OC_Qty_Edit])
		{
			::MoveWindow(m_hControl[OC_Qty_Edit], x, y+2, width - N_SPIN_WIDTH, height-4, TRUE);
			::ShowWindow(m_hControl[OC_Qty_Edit], SW_SHOW);
			::MoveWindow(m_hControl[OC_Qty_Spin], x + width - N_SPIN_WIDTH, y+1, N_SPIN_WIDTH, height-2, TRUE);
			::ShowWindow(m_hControl[OC_Qty_Spin], SW_SHOW);
			if (GetFocus() != m_hControl[OC_Qty_Edit])
				SetFocus(m_hControl[OC_Qty_Edit]);
		}
	}
}
//背景色
void COrderOption::DrawBackColor()
{
	RECT rect;
	::GetClientRect(m_Hwnd, &rect);

	//背景 边框
	FillRect(m_HdcMem, &rect, g_ColorRefData.GetHbrBackground());
	rect.top = N_ROW_HEIGHT;
    rect.bottom = rect.bottom - N_ROW_HEIGHT * 2;
	FrameRect(m_HdcMem, &rect, g_ColorRefData.GetHbrRGB200());

	//列头
	RECT rectpart(m_rcItem[0][0]);
	rectpart.left = rect.left + 1;
	rectpart.right = rect.right - 1;
	FillRect(m_HdcMem, &rectpart, g_ColorRefData.GetHbrBackground());
}
//表格线
void COrderOption::DrawGridLine()
{
	HPEN hPen = CreatePen(PS_SOLID, 1, g_ColorRefData.GetColorRGB200());
	HPEN OldPen = (HPEN)SelectObject(m_HdcMem, hPen);
	for (int j = COO_Code; j < COO_END; j++)
	{
		//竖向分割线
		MoveToEx(m_HdcMem, m_rcItem[0][j].right, N_ROW_HEIGHT, NULL);
		LineTo(m_HdcMem, m_rcItem[0][j].right, m_nHeight-N_ROW_HEIGHT*2);
	}
	for (int i = N_Head_row; i < m_nOptionRows; i++)
	{
		//横向分割线
		MoveToEx(m_HdcMem, 1, (i + 2) * N_ROW_HEIGHT, NULL);
		LineTo(m_HdcMem, m_nWidth - 1, (i + 2) * N_ROW_HEIGHT);
	}
	if (m_OptionTarget.bAdd)
	{
		//横向分割线
		MoveToEx(m_HdcMem, 1, (m_nOptionRows + 2) * N_ROW_HEIGHT, NULL);
		LineTo(m_HdcMem, m_nWidth - 1, (m_nOptionRows + 2) * N_ROW_HEIGHT);
	}
	SelectObject(m_HdcMem, OldPen);
	DeleteObject(hPen);
}
//行
void COrderOption::DrawRow(int i)
{
	for (int j = COO_Code; j < COO_END; j++)
	{
		if (i > N_Head_row)
		{
			switch (j)
			{
			case COO_Direct:
				if (wcscmp(m_strItem[i][j], LoadResWchar_t(IDS_BuyList)) == 0)
					SetTextColor(m_HdcMem, m_ExColor[EDC_Buy]);
				else
					SetTextColor(m_HdcMem, m_ExColor[EDC_Sell]);
				if (i==N_Target_row)
					FillRect(m_HdcMem, &m_rcItem[i][j], g_ColorRefData.GetHbrWhite());
				SelectObject(m_HdcMem, g_FontData.GetFontWordEx15());
				break;
			case COO_Price:
				if (wcscmp(LoadResWchar_t(IDS_LIMIT_PRICE), m_strItem[i][COO_PriceType]) == 0)
					FillRect(m_HdcMem, &m_rcItem[i][j], g_ColorRefData.GetHbrWhite());
				SetTextColor(m_HdcMem, g_ColorRefData.GetColorBlack());
				SelectObject(m_HdcMem, g_FontData.GetFontNumEx17());
				break;
			case COO_PriceType:
				if (!m_bSyncPrice || N_1_row == i)
				{
					FillRect(m_HdcMem, &m_rcItem[i][j], g_ColorRefData.GetHbrWhite());
					DrawTriangle(m_HdcMem, m_rcItem[i][j].right - 11, m_rcItem[i][j].top + 10, 7);
				}
				SetTextColor(m_HdcMem, g_ColorRefData.GetColorBlack());
				SelectObject(m_HdcMem, g_FontData.GetFontWord13());
				break;
			case COO_Qty:
				if (!m_bSyncQty || N_1_row == i)
					FillRect(m_HdcMem, &m_rcItem[i][j], g_ColorRefData.GetHbrWhite());
				SetTextColor(m_HdcMem, g_ColorRefData.GetColorBlack());
				SelectObject(m_HdcMem, g_FontData.GetFontNumEx17());
				break;
			case COO_Code:
				SetTextColor(m_HdcMem, g_ColorRefData.GetColorBlack());
				SelectObject(m_HdcMem, g_FontData.GetFontNumEx17());
				break;
			default:
				SetTextColor(m_HdcMem, g_ColorRefData.GetColorBlack());
				SelectObject(m_HdcMem, g_FontData.GetFontNum13());
				break;
			}
			DrawTextW(m_HdcMem, m_strItem[i][j], wcslen(m_strItem[i][j]), &m_rcItem[i][j], DT_VCENTER | DT_CENTER | DT_SINGLELINE);
		}//if(i > N_Head_row)
		else
		{
			switch (j)
			{
			case COO_Code:
			case COO_Direct:
			case COO_Qty:
			case COO_PriceType:
			case COO_Price:
				SelectObject(m_HdcMem, g_FontData.GetFontWord13());
				DrawTextW(m_HdcMem, m_strItem[i][j], wcslen(m_strItem[i][j]), &m_rcItem[i][j], DT_VCENTER | DT_CENTER | DT_SINGLELINE);
				break;
			case COO_Delta:
			case COO_Gamma:
				{
					SelectObject(m_HdcMem, g_FontData.GetFontNum9());
					RECT rect(m_rcItem[i][j]);
					rect.right = rect.left + (rect.right - rect.left) / 2;
					rect.top = rect.bottom - (rect.bottom - rect.top) / 3-3;
					if (COO_Delta == j)
						DrawTextW(m_HdcMem, L"Delta", wcslen(L"Delta"), &rect, DT_TOP | DT_CENTER | DT_SINGLELINE);
					else				
						DrawTextW(m_HdcMem, L"Gamma", wcslen(L"Gamma"), &rect, DT_TOP | DT_CENTER | DT_SINGLELINE);	
					rect = m_rcItem[i][j];
					rect.bottom = m_rcItem[i][j].top + (rect.bottom - rect.top) *2/ 3;
					SelectObject(m_HdcMem, g_FontData.GetFontNumEx13());
					DrawTextW(m_HdcMem, m_strItem[i][j], wcslen(m_strItem[i][j]), &rect, DT_VCENTER | DT_RIGHT | DT_SINGLELINE);
				}
				break;
			}//switch (j)		
		}//if(i > N_Head_row)--else
	}
}
//列表
void COrderOption::DrawListContent()
{
	SetTextColor(m_HdcMem, g_ColorRefData.GetColorBlack());
	for (int i = N_Head_row; i < m_nOptionRows + 1; i++)
	{
		SelectObject(m_HdcMem, g_FontData.GetFontWord13());
		DrawRow(i);
	}

	if (m_OptionTarget.bAdd)
	{
		SelectObject(m_HdcMem, g_FontData.GetFontWord13());
		DrawRow(N_Target_row);
	}
}
//按钮
void COrderOption::DrawSpecial()
{	
	SetTextColor(m_HdcMem, g_ColorRefData.GetColorGray());
	for (int i = SPI_Type; i <= SPI_Option; i++)
	{
		SelectObject(m_HdcMem, g_FontData.GetFontWordEx15());
		DrawTextW(m_HdcMem, m_strSpecialItem[i], wcslen(m_strSpecialItem[i]), &m_rcSpecialItem[i], DT_VCENTER | DT_CENTER | DT_SINGLELINE);
	}	
	//有效类型	
	for (int i = SPI_OC; i<= SPI_Valid;i++)
	{
		if (m_OptionCommodityInfo.cCoverMode == oNone && SPI_OC==i)
			continue;

		SelectObject(m_HdcMem, g_FontData.GetFontWord13());
		
		if (m_nCurHoverID == i)
		{
			SetTextColor(m_HdcMem, g_ColorRefData.GetColorWhite());
			FillRect(m_HdcMem, &m_rcSpecialItem[i], g_ColorRefData.GetHbrLightBlue());
		}
		else
		{
			SetTextColor(m_HdcMem, g_ColorRefData.GetColorGray());
			FillRect(m_HdcMem, &m_rcSpecialItem[i], g_ColorRefData.GetHbrWhite());
			FrameRect(m_HdcMem, &m_rcSpecialItem[i], g_ColorRefData.GetHbrRGB150());
		}	

		DrawTriangle(m_HdcMem, m_rcSpecialItem[i].right - 11, m_rcSpecialItem[i].top + 10, 7, m_nCurHoverID == i ? g_ColorRefData.GetColorWhite() : g_ColorRefData.GetColorGray());
		DrawTextW(m_HdcMem, m_strSpecialItem[i], wcslen(m_strSpecialItem[i]), &m_rcSpecialItem[i], DT_VCENTER | DT_CENTER | DT_SINGLELINE);
	}
	SetTextColor(m_HdcMem, g_ColorRefData.GetColorWhite());
	for (int i = SPI_BackDirect; i <= SPI_Reset; i++)
	{
		SelectObject(m_HdcMem, g_FontData.GetFontWord13());
		HBRUSH hbrush = g_ColorRefData.GetHbrRGB200();
		if (m_CurType == T_AutoGroup)
			hbrush = g_ColorRefData.GetHbrLightBlue();
		if (m_nCurHoverID == i&&(T_AutoGroup == m_CurType))
		{
			RECT re(m_rcSpecialItem[i]);
			FillRect(m_HdcMem, &re, hbrush);
			InflateRect(&re, -1, -1);
			FrameRect(m_HdcMem, &re, g_ColorRefData.GetHbrWhite());
		}
		else
			FillRect(m_HdcMem, &m_rcSpecialItem[i], hbrush);

		DrawTextW(m_HdcMem, m_strSpecialItem[i], wcslen(m_strSpecialItem[i]), &m_rcSpecialItem[i], DT_VCENTER | DT_CENTER | DT_SINGLELINE);
	}
	//发单
	SelectObject(m_HdcMem, g_FontData.GetFontWordEx15());
	FillRect(m_HdcMem, &m_rcSpecialItem[SPI_Order], g_ColorRefData.GetHbrBuyRed());
	if (m_nCurHoverID == SPI_Order)
	{
		RECT re(m_rcSpecialItem[SPI_Order]);
		InflateRect(&re, -1, -1);
		FrameRect(m_HdcMem, &re, g_ColorRefData.GetHbrWhite());
	}
	RECT re(m_rcSpecialItem[SPI_Order]);
	re.top = re.top + 1;
	DrawTextW(m_HdcMem, m_strSpecialItem[SPI_Order], wcslen(m_strSpecialItem[SPI_Order]), &re/*m_rcSpecialItem[SPI_Order]*/, DT_VCENTER | DT_CENTER | DT_SINGLELINE);

    SelectObject(m_HdcMem, g_FontData.GetFontWord13());
    if (m_bInfoError)
    {
        SetTextColor(m_HdcMem, g_ColorRefData.GetColorBuyRed());
    }
    else
    {
        SetTextColor(m_HdcMem, g_ColorRefData.GetColorGray());
    }
    DrawTextW(m_HdcMem, m_strInfo, wcslen(m_strInfo), &m_rcSpecialItem[SPI_Info], DT_VCENTER | DT_LEFT | DT_SINGLELINE);
}
//响应 联动价格类型状态
void COrderOption::OnButtonPriceSync()
{
	m_SyncPriceTypeBtn.SetLockState(!m_SyncPriceTypeBtn.GetLockState());
	m_bSyncPrice = m_SyncPriceTypeBtn.GetLockState();
	UpdateSyncType();

	Redraw();
}
//响应 联动数量状态
void COrderOption::OnButtonQtySync()
{
	m_SyncQtyBtn.SetLockState(!m_SyncQtyBtn.GetLockState());
	m_bSyncQty = m_SyncQtyBtn.GetLockState();
	UpdateSyncQty();

	Redraw();
}
//重置联动
void COrderOption::ResetSyncState()
{
	if (0 == wcscmp(m_strItem[N_1_row][COO_PriceType], LoadResWchar_t(IDS_LIMIT_PRICE)))
		wcscpy_s(m_strItem[N_1_row][COO_PriceType], LoadResWchar_t(IDS_STRING1154));
	if (0 == wcscmp(m_strItem[N_1_row][COO_Qty], L"1"))
		wcscpy_s(m_strItem[N_1_row][COO_Qty], L"1");

	m_SyncPriceTypeBtn.SetLockState(true);
	m_bSyncPrice = m_SyncPriceTypeBtn.GetLockState();
	UpdateSyncType();

	m_SyncQtyBtn.SetLockState(true);
	m_bSyncQty = m_SyncQtyBtn.GetLockState();
	UpdateSyncQty();

	Redraw();
}
//价格增减处理
void COrderOption::OnSpinDeltaUpDownEx(int nDelta, int nID)
{
	double dMintick = m_OptionCommodityInfo.dMiniChangePrice;
	int nPreicision = m_OptionCommodityInfo.iPrecision;
	if (m_CurShowItem.nRow == N_Target_row)
	{
		dMintick = m_TargetCommodityInfo.dMiniChangePrice;
		nPreicision = m_TargetCommodityInfo.iPrecision;
	}
	
	if (nDelta > 0)
	{
		string sInt = "";
		sInt = GetWindowTextAnsic(m_hControl[nID]);
		double a = atof(sInt.c_str());
		a += dMintick;
		SetWindowDouble(m_hControl[nID], nPreicision, a);
	}
	else
	{
		string sInt = "";
		sInt = GetWindowTextAnsic(m_hControl[nID]);
		double a = atof(sInt.c_str());
		a -= dMintick;
		if (!m_OptionCommodityInfo.bCanNegative&&a < 0)//非负数
			a = 0;
		SetWindowDouble(m_hControl[nID], nPreicision, a);
	}
    //UpdatePreFreeze();
}
//数量增减处理
void COrderOption::OnSpinDeltaUpDownNormal(int nDelta, int nID, int nMax, int nMin)
{
	if (nDelta > 0)
	{
		string sInt = GetWindowTextAnsic(m_hControl[nID]);
		int a = atoi(sInt.c_str());
		if (a < nMax - 1)
			a++;

		SetWindowInt(m_hControl[nID], a);
	}
	else
	{
		string sInt = GetWindowTextAnsic(m_hControl[nID]);
		int a = atoi(sInt.c_str());
		if (a > nMin)
			a--;

		a = a < nMin ? nMin : a;
		SetWindowInt(m_hControl[nID], a);
	}
    //UpdatePreFreeze();
}
bool COrderOption::SetSelect(const wchar_t* sStrategy, int nRef)
{
	switch (nRef)
	{
	case Nref_Validtype:
		wcscpy_s(m_strSpecialItem[SPI_Valid], sStrategy);
		Redraw(&m_rcSpecialItem[SPI_Valid]);
		return true;
	case Nref_Pricetype:
		if (m_CurShowItem.nCol == COO_PriceType && m_CurShowItem.nRow > 0)
		{
			wcscpy_s(m_strItem[m_CurShowItem.nRow][m_CurShowItem.nCol], sStrategy);
			UpdateSyncType(m_CurShowItem.nRow);
			Redraw();
			return true;
		}
		else
			return false;
	case Nref_OCtype:	
		wcscpy_s(m_strSpecialItem[SPI_OC], sStrategy);
		Redraw();
        UpdatePreFreeze();
		return true;	
	default:
		return false;
	}
}
void COrderOption::UpdateSyncQty()
{
	if (m_bSyncQty)
	{
		for (int i = N_2_row; i < m_nOptionRows + 1; i++)
			wcscpy_s(m_strItem[i][COO_Qty], m_strItem[N_1_row][COO_Qty]);
		if (m_OptionTarget.bAdd)
			wcscpy_s(m_strItem[N_Target_row][COO_Qty], m_strItem[N_1_row][COO_Qty]);
		UpdatePrice();
	}
}

void COrderOption::UpdateSyncType(int nIndex)
{
	if (m_bSyncPrice)
	{
		for (int i = N_2_row; i < m_nOptionRows + 1; i++)
			wcscpy_s(m_strItem[i][COO_PriceType], m_strItem[N_1_row][COO_PriceType]);
		if (m_OptionTarget.bAdd)
			wcscpy_s(m_strItem[N_Target_row][COO_PriceType], m_strItem[N_1_row][COO_PriceType]);
		UpdatePrice();
	}
	else
		UpdatePrice(nIndex);
}
void COrderOption::UpdatePrice(int nIndex)
{
	if (!IsActive())
		return;

	for (int i = N_1_row; i < m_nOptionRows + 1; i++)
	{
		if (nIndex != 0)
			if (nIndex != i)
				continue;
		UpdateDelta(i);
		if(wcscmp(m_strItem[i][COO_PriceType],LoadResWchar_t(IDS_LIMIT_PRICE))==0)
			continue;
		else
		{
			SContract *pQuote = NULL;
			if (g_pStarApi->GetContractData("", m_QuoteTradeCode[EQT_Quote][i - 1], &pQuote, 1, false))
			{
				double dSellPrice = 0, dBuyPrice = 0, dLastPrice = 0;
				dLastPrice = CConvert::GetPriceBySnapShot(pQuote->SnapShot, S_FID_LASTPRICE);
				dBuyPrice = CConvert::GetPriceByContract(pQuote, S_FID_BESTBIDPRICE);
				dSellPrice = CConvert::GetPriceByContract(pQuote, S_FID_BESTASKPRICE);
				if (wcscmp(m_strItem[i][COO_PriceType], LoadResWchar_t(IDS_STRING1154)) == 0)
				{
					if (wcscmp(m_strItem[i][COO_Direct], LoadResWchar_t(IDS_BuyList)) == 0)
						swprintf_s(m_strItem[i][COO_Price], L"%s", LoadRC::ansi_to_unicode(CConvert::FormatPriceToStr(dSellPrice, m_OptionCommodityInfo)).c_str());
					else
						swprintf_s(m_strItem[i][COO_Price], L"%s", LoadRC::ansi_to_unicode(CConvert::FormatPriceToStr(dBuyPrice, m_OptionCommodityInfo)).c_str());
				}
				else if (wcscmp(m_strItem[i][COO_PriceType], LoadResWchar_t(IDS_STRING1155)) == 0)
				{
					if (wcscmp(m_strItem[i][COO_Direct], LoadResWchar_t(IDS_BuyList)) == 0)
						swprintf_s(m_strItem[i][COO_Price], L"%s", LoadRC::ansi_to_unicode(CConvert::FormatPriceToStr(dBuyPrice, m_OptionCommodityInfo)).c_str());
					else
						swprintf_s(m_strItem[i][COO_Price], L"%s", LoadRC::ansi_to_unicode(CConvert::FormatPriceToStr(dSellPrice, m_OptionCommodityInfo)).c_str());
				}
				else if (wcscmp(m_strItem[i][COO_PriceType], LoadResWchar_t(IDS_STRING1122)) == 0)
					swprintf_s(m_strItem[i][COO_Price], L"%s", LoadRC::ansi_to_unicode(CConvert::FormatPriceToStr(dLastPrice, m_OptionCommodityInfo)).c_str());
				else
					continue;

				InvalidateRect(m_Hwnd, &m_rcItem[i][COO_Price], TRUE);
			}
		}//if(限价)--else
	}
	if (m_OptionTarget.bAdd)
	{
		UpdateTargetDelta();
		if (wcscmp(m_strItem[N_Target_row][COO_PriceType], LoadResWchar_t(IDS_LIMIT_PRICE)) != 0)
		{ 
			SContract *pQuote = NULL;
			if (g_pStarApi && g_pStarApi->GetContractData("", m_OptionTarget.Code[EQT_Quote], &pQuote, 1, false))
			{
				double dSellPrice = 0, dBuyPrice = 0, dLastPrice = 0;
				dLastPrice = CConvert::GetPriceBySnapShot(pQuote->SnapShot, S_FID_LASTPRICE);
				dBuyPrice = CConvert::GetPriceByContract(pQuote, S_FID_BESTBIDPRICE);
				dSellPrice = CConvert::GetPriceByContract(pQuote, S_FID_BESTASKPRICE);
				
				if (wcscmp(m_strItem[N_Target_row][COO_PriceType], LoadResWchar_t(IDS_STRING1154)) == 0)
				{
					if (wcscmp(m_strItem[N_Target_row][COO_Direct], LoadResWchar_t(IDS_BuyList)) == 0)
						swprintf_s(m_strItem[N_Target_row][COO_Price], L"%s", LoadRC::ansi_to_unicode(CConvert::FormatPriceToStr(dSellPrice, m_TargetCommodityInfo)).c_str());
					else
						swprintf_s(m_strItem[N_Target_row][COO_Price], L"%s", LoadRC::ansi_to_unicode(CConvert::FormatPriceToStr(dBuyPrice, m_TargetCommodityInfo)).c_str());
				}
				else if (wcscmp(m_strItem[N_Target_row][COO_PriceType], LoadResWchar_t(IDS_STRING1155)) == 0)
				{
					if (wcscmp(m_strItem[N_Target_row][COO_Direct], LoadResWchar_t(IDS_BuyList)) == 0)
						swprintf_s(m_strItem[N_Target_row][COO_Price], L"%s", LoadRC::ansi_to_unicode(CConvert::FormatPriceToStr(dBuyPrice, m_TargetCommodityInfo)).c_str());
					else
						swprintf_s(m_strItem[N_Target_row][COO_Price], L"%s", LoadRC::ansi_to_unicode(CConvert::FormatPriceToStr(dSellPrice, m_TargetCommodityInfo)).c_str());
				}
				else if (wcscmp(m_strItem[N_Target_row][COO_PriceType], LoadResWchar_t(IDS_STRING1122)) == 0)
					swprintf_s(m_strItem[N_Target_row][COO_Price], L"%s", LoadRC::ansi_to_unicode(CConvert::FormatPriceToStr(dLastPrice, m_TargetCommodityInfo)).c_str());
							
				InvalidateRect(m_Hwnd, &m_rcItem[N_Target_row][COO_Price], TRUE);
			}
		}
	}

    UpdatePreFreeze();

	UpdateTotal();
	UpdateWindow(m_Hwnd);
}
void COrderOption::DrawTriangle(HDC &hdc, int nStartx, int nStarty, int nWidth, COLORREF color)//宽度奇数
{
	if (nWidth % 2 == 0)
		return;
	HPEN   hPen = CreatePen(PS_SOLID, 1, color);
	SelectObject(hdc, hPen);

	for (int i = 0; i < nWidth / 2 + 1; i++)
	{
		MoveToEx(hdc, nStartx + i, nStarty + i, NULL);
		LineTo(hdc, nStartx + nWidth - i, nStarty + i);
	}

	DeleteObject(hPen);
}
void COrderOption::UpdateCommodityInfo(const char* Code, bool bOption)
{
	//更新行情品种信息
	if (bOption)
	{
		if (!CConvert::GetQuoteCommodityInf(Code, m_OptionCommodityInfo))
			return;
		TContractKey	TradeContract;	//交易合约
		memset(&TradeContract, 0, sizeof(TradeContract));
		CConvert::TContractNoToTContractKey(Code, TradeContract);
		if (!CConvert::IsInnerDiskExchange(TradeContract.ExchangeNo))
			m_OptionCommodityInfo.cCoverMode = oNone;
		else
			m_OptionCommodityInfo.cCoverMode = oCover;
	}
	else
	{
		if (!CConvert::GetQuoteCommodityInf(Code, m_TargetCommodityInfo))
			return;
	}	
}
void COrderOption::SetExchangeSupport(bool bCheck)
{
	m_ExchangeSupport.SetCheck(bCheck ? BST_CHECKED : BST_UNCHECKED);
}
void COrderOption::OnBackDirect()
{
	if (m_CurType != T_AutoGroup)
		return;

	//期权反向
	for (int i = N_1_row; i <= m_nOptionRows; i++)
	{
		if (wcscmp(m_strItem[i][COO_Direct], LoadResWchar_t(IDS_BuyList)) == 0)
			wcscpy_s(m_strItem[i][COO_Direct], LoadResWchar_t(IDS_SellList));
		else
			wcscpy_s(m_strItem[i][COO_Direct], LoadResWchar_t(IDS_BuyList));
	}

	//清理价格
	for (int i = N_1_row; i <= N_Target_row; i++)
		memset(m_strItem[i][COO_Price], 0, sizeof(m_strItem[i][COO_Price]));

	//标的反向
	if (m_OptionTarget.bAdd)
	{
		if (wcscmp(m_strItem[N_Target_row][COO_Direct], LoadResWchar_t(IDS_BuyList)) == 0)
			wcscpy_s(m_strItem[N_Target_row][COO_Direct], LoadResWchar_t(IDS_SellList));
		else
			wcscpy_s(m_strItem[N_Target_row][COO_Direct], LoadResWchar_t(IDS_BuyList));
	}

	//重置锁定
	ResetSyncState();

	//发送反向消息
	TMouLinkMsgStrA	str;
	sprintf_s(str, "%s&%s", _BackDirect, m_SeriesNo);
	g_mainframe->Linkage(GetParent(), _MouseOrder, _OptionSync,str);

    //UpdatePreFreeze();
}
void COrderOption::OnAddOptionTarget()
{
	if (m_CurType != T_AutoGroup)
		return;

	if(!m_OptionTarget.bAdd)
	{
		TContractKey key;
		memset(&key, 0, sizeof(key));
		SContract* qContract=NULL;
		if (CConvert::TContractNoToTContractKey(m_QuoteTradeCode[EQT_Trade][0], key))
		{
			if (key.CommodityType == ctOption)
			{
				SContractNoType sQuote = { 0 };
				if (CConvert::TradeContractToOptionTarget(key, sQuote))
				{				
					if (g_pStarApi->GetOptionSeriesTarget(sQuote, "", &qContract, 1, false))
					{
						string sCode = "";
						string  sQuoteTemp = qContract->ContractNo;
						CConvert::QContractNoTypeToStr(sQuoteTemp, sCode);
                        if (sCode.empty())
                        {//暂未处理的类型
                            return;
                        }
						memcpy(m_OptionTarget.Code[EQT_Quote], qContract->ContractNo, sizeof(SContractNoType));
						memcpy(m_OptionTarget.Code[EQT_Trade], sCode.c_str(), sizeof(SContractNoType));
						m_OptionTarget.bAdd = true;
						m_OptionTarget.Direct = dBuy;
						memcpy(m_strItem[N_Target_row][COO_Direct], LoadResWchar_t(IDS_BuyList), sizeof(TMouPriceTypeStrW));
						size_t npos = sCode.find(' ');
						if (npos != string::npos)
							sCode = sCode.substr(npos + 1, sCode.length() - npos);
						memcpy(m_strItem[N_Target_row][COO_Code], LoadRC::ansi_to_unicode(sCode).c_str(), sizeof(TMouPriceTypeStrW));
                        g_pStarApi->SubQuote(&qContract->ContractNo, 1, this);
						UpdateCommodityInfo(m_OptionTarget.Code[EQT_Trade], false);
						if (m_SyncQtyBtn.GetLockState())
							UpdateSyncQty();
						if (m_SyncPriceTypeBtn.GetLockState())
							UpdateSyncType();

						TMouLinkMsgStrA content = { 0 };
						if (wcscmp(m_strItem[N_Target_row][COO_Direct], LoadResWchar_t(IDS_BuyList)) == 0)
							sprintf_s(content, "%s&%s&%c", _AddTarget, m_SeriesNo, dBuy);
						else				
							sprintf_s(content, "%s&%s&%c", _AddTarget, m_SeriesNo, dSell);	
						g_mainframe->Linkage(GetParent(), "MouseOrder", _OptionSync, content);
					}
				}
			}//if(Option)
		}

	}
	else
	{
		m_OptionTarget.bAdd = false;
        g_pStarApi->UnsubQuote(&m_OptionTarget.Code[EQT_Quote],1,this);

		TMouLinkMsgStrA content = { 0 };
		sprintf_s(content, "%s&%s", _DeleteTarget, m_SeriesNo);
		g_mainframe->Linkage(GetParent(), "MouseOrder", _OptionSync, content);	

	}

	LayOut();

	UpdatePrice(N_Target_row);

	Redraw();
}
void COrderOption::OnReset()
{
	if (m_CurType != T_AutoGroup)
		return;

	//退订行情 更新（合约 期权序列 有效行 标的）信息
    g_pStarApi->UnsubAllQuote(this);
	memset(m_QuoteTradeCode, 0, sizeof(m_QuoteTradeCode));
	memset(m_strSpecialItem[SPI_Option], 0, sizeof(TMouPriceTypeStrW));
	m_nOptionRows = 0;	
	m_OptionTarget.bAdd = false;

	//布局更新
	LayOut();

	//清理价格
	for (int i = N_1_row; i <= N_Target_row; i++)
		memset(m_strItem[i][COO_Price], 0, sizeof(m_strItem[i][COO_Price]));

	//重置开平 有效类型
	wcscpy_s(m_strSpecialItem[SPI_Valid], LoadResWchar_t(IDS_EFFECTIVE_THEDAY));
	wcscpy_s(m_strSpecialItem[SPI_OC], LoadResWchar_t(IDS_OPEN));

	//重置锁定
	ResetSyncState();

	//发送消息
	g_mainframe->Linkage(GetParent(), _MouseOrder, _OptionSync, _Reset);

    UpdatePreFreeze();
}
void COrderOption::OnCheckSupport()
{
	if (m_ExchangeSupport.GetCheck() == BST_CHECKED)
	{
        if (!m_SyncQtyBtn.GetLockState())
        {
            OnButtonQtySync();  /// SetLockState不会触发点击事件，通过OnButtonQtySync同步数量
        }
        EnableWindow(m_SyncQtyBtn.GetHwnd(), false);

		if (wcscmp(m_strSpecialItem[SPI_Valid],LoadResWchar_t(IDS_IMMEDIATE_ALL))&& wcscmp(m_strSpecialItem[SPI_Valid], LoadResWchar_t(IDS_IMMEDIATE_PART)))
			wcscpy_s(m_strSpecialItem[SPI_Valid], LoadResWchar_t(IDS_IMMEDIATE_ALL));	
		Redraw(&m_rcSpecialItem[SPI_Valid]);
	}
    else
    {
        EnableWindow(m_SyncQtyBtn.GetHwnd(), true);
    }

    UpdatePreFreeze();

	m_opr->UpdateExchangeSupport(m_ExchangeSupport.GetCheck()?true:false);
}
void COrderOption::UpdateDelta(int i)
{
	//wstring wstr = m_strItem[i][COO_Code];
	//size_t pos = wstr.find(L'C');
	//bool bCall = true;
	//if (pos != wstring::npos)
	//	wstr = wstr.substr(pos+1, wstr.length());
	//else
	//{
	//	pos = wstr.find(L'P');
	//	bCall = false;
	//	if (pos != wstring::npos)
	//		wstr = wstr.substr(pos + 1, wstr.length());	
	//	else
	//		return;
	//}
	//SStrikePriceType cStrkPrice = { 0 };
	//strcpy_s(cStrkPrice, LoadRC::unicode_to_ansi(wstr).c_str());

    SContract *pQuote = NULL;
    if (g_pStarApi->GetContractData("", m_QuoteTradeCode[EQT_Quote][i - 1], &pQuote, 1, false))
	{
        if (pQuote->SnapShot)
        {
            int nQty = _wtoi(m_strItem[i][COO_Qty])*(wcscmp(m_strItem[i][COO_Direct], LoadResWchar_t(IDS_BuyList)) == 0 ? 1 : -1);
            double dValue = pQuote->SnapShot->Data[S_FID_DELTA].FidAttr ? pQuote->SnapShot->Data[S_FID_DELTA].Greek : 0;
            swprintf_s(m_strItem[i][COO_Delta], L"%.4f", dValue * nQty);
            dValue = pQuote->SnapShot->Data[S_FID_GAMMA].FidAttr ? pQuote->SnapShot->Data[S_FID_GAMMA].Greek : 0;
            swprintf_s(m_strItem[i][COO_Gamma], L"%.4f", dValue * nQty);
            //if (bCall)
            //{
            //	swprintf_s(m_strItem[i][COO_Delta], L"%.4f", result.CallDelta*nQty);
            //	swprintf_s(m_strItem[i][COO_Gamma], L"%.4f", result.CallGamma*nQty);
            //}
            //else
            //{
            //	swprintf_s(m_strItem[i][COO_Delta], L"%.4f", result.PutDelta*nQty);
            //	swprintf_s(m_strItem[i][COO_Gamma], L"%.4f", result.PutGamma*nQty);
            //}
            InvalidateRect(m_Hwnd, &m_rcItem[i][COO_Delta], TRUE);
            InvalidateRect(m_Hwnd, &m_rcItem[i][COO_Gamma], TRUE);
        }
	}
	else
	{
		wcscpy_s(m_strItem[i][COO_Delta], L"0");
		wcscpy_s(m_strItem[i][COO_Gamma], L"0");
	}
}
void COrderOption::UpdateTargetDelta()
{
	int  nQty = _wtoi(m_strItem[N_Target_row][COO_Qty])*(wcscmp(m_strItem[N_Target_row][COO_Direct], LoadResWchar_t(IDS_BuyList)) == 0 ? 1 : -1);
	
	swprintf_s(m_strItem[N_Target_row][COO_Delta], L"%d", nQty);
	swprintf_s(m_strItem[N_Target_row][COO_Gamma], L"%d", 0);

	InvalidateRect(m_Hwnd, &m_rcItem[N_Target_row][COO_Delta], TRUE);
	InvalidateRect(m_Hwnd, &m_rcItem[N_Target_row][COO_Gamma], TRUE);
}
void COrderOption::UpdateTotal()
{
	double dTotalDelta = 0, dTotalGamma = 0;
	for (int i = N_1_row; i < m_nOptionRows + 1; i++)
	{
		dTotalDelta += _wtof(m_strItem[i][COO_Delta]);
		dTotalGamma += _wtof(m_strItem[i][COO_Gamma]);
	}
	if (m_OptionTarget.bAdd)
	{
		dTotalDelta += _wtof(m_strItem[N_Target_row][COO_Delta]);
		dTotalGamma += _wtof(m_strItem[N_Target_row][COO_Gamma]);
	}

	swprintf_s(m_strItem[N_Head_row][COO_Delta], L"%.4f", dTotalDelta);
	swprintf_s(m_strItem[N_Head_row][COO_Gamma], L"%.4f", dTotalGamma);
	InvalidateRect(m_Hwnd, &m_rcItem[N_Head_row][COO_Delta], TRUE);
	InvalidateRect(m_Hwnd, &m_rcItem[N_Head_row][COO_Gamma], TRUE);
}
void COrderOption::Redraw(RECT* rc)
{
	RedrawWindow(m_Hwnd, rc, 0, RDW_INVALIDATE | RDW_UPDATENOW);
}
bool COrderOption::IsActive()
{
	return m_opr&&m_opr->IsOptionStrategyLayOut();
}