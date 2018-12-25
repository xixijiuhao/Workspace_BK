#include "PreInclude.h"

static int g_ntitlesize[FormProcessCol] = {
	100, 90, 100, 90, 100, 100, 110, 90, 90, 100,
	90, 100, 110, 100, 100, 100, 110, 90, 90, 100,
	100, 90, 100, 90, 100, 100, 110, 90, 90, 100,
	90, 100, 110, 100, 100, 100, 110, 90, 90, 100,
	100, 90, 100, 90, 90, 100, 90, 110
};
static bool g_bcolshow[FormProcessCol] = {
	true, true, true, true, true, true, true, true, true, true,
	true, true, true, true, true, true, true, true, true, true,
	true, true, true, true, true, true, true, true, true, true,
	true, true, true, true, true, true, true, true, true, true,
	true, true, true, true, true, true, true
};

TFormProcess::TFormProcess()
{
	m_pcontainer = NULL;
}

TFormProcess::~TFormProcess()
{
	TDeleteMem(m_pcontainer);
}

//property
bool TFormProcess::Create(HWND hparent, LPCTSTR pstr/* = NULL*/, int nCol/* = 0*/)
{
	char szconfig[MAX_PATH];
	WCharToMByte(pstr, szconfig, MAX_PATH);
	m_szconfig = szconfig;

	__super::Create(hparent, _T("TFormProcess"), FormProcessCol);

	m_pcontainer = new TVirtualContainer;

	LoadConfig();
	CreateList();
	SetColumnWidth();
	return true;
}
void TFormProcess::SetData(THisOrderProcContainer* pdata)
{
	m_pcontainer->SetContainerMap(pdata);
	GetLocaldata();
	int npos = m_pgrid->GetVertPos();
	UpdateList(npos);
}

void TFormProcess::LoadConfig()
{
	bool bhavecolwidth;
	TListConfig cpconfig;
	std::vector<int>::iterator ignor;
	cpconfig.LoadConfig(m_szconfig, FormProcessCol, m_stlconfig, bhavecolwidth);
	cpconfig.LoadIgnore(m_szconfig.c_str(), m_stlignore);

	int nsize = m_stlconfig.size();
	std::vector<ConfigData>::iterator itor = m_stlconfig.begin();
	for (int i = 0; i < nsize; itor++, i++)
	{
		ignor = std::find(m_stlignore.begin(), m_stlignore.end(), itor->nrealindex);
		if (m_stlignore.end() != ignor)
			itor->bignore = true;
		itor->szcol = GetFormProcessTitle(itor->nrealindex);
		if (!bhavecolwidth)itor->ncolwidth = g_ntitlesize[i];
	}

	for (int i = nsize; i < FormProcessCol; i++)
	{
		ConfigData stconfig;
		stconfig.bshow = g_bcolshow[i];
		stconfig.szcol = GetFormProcessTitle(i);
		if (i == 0){ stconfig.nformat = 0; stconfig.sztype = GetPositionString(0); }
		else if (i == 8 || i == 9 || i == 11 || i == 12 || i == 15){ stconfig.nformat = 2; stconfig.sztype = GetPositionString(2); }
		else { stconfig.nformat = 1; stconfig.sztype = GetPositionString(1); }
		stconfig.nrealindex = i;
		stconfig.ncolwidth = g_ntitlesize[i];

		ignor = std::find(m_stlignore.begin(), m_stlignore.end(), stconfig.nrealindex);
		if (m_stlignore.end() != ignor)
			stconfig.bignore = true;
		m_stlconfig.push_back(stconfig);
	}

	ChangeColConfig();
}

void TFormProcess::GetLocaldata()
{
	Filter stfilter;
	m_pcontainer->RetrieveForm(&stfilter);
	m_pcontainer->SortForm(FormKey_StreamId);
}

void TFormProcess::RefreshFilterData()
{
	ChangeColConfig();
	SetColumnWidth();
}
//MSG
void TFormProcess::OnCommand(WPARAM wParam, LPARAM lParam)
{
	__super::OnCommand(wParam, lParam);

	int ntype = HIWORD(wParam);
	if (ntype != 0) return;

	int nmenuid = LOWORD(wParam);
	if (nmenuid == 103)
	{
		std::wstring szfile;
		DWORD dwsize = MAX_PATH; wchar_t sztext[MAX_PATH];
		MByteToWChar(GetConfigColStringInfo(10), sztext, dwsize);
		if (SaveAs(sztext, szfile, NULL, m_Hwnd))
		{
			char sztemp[MAX_PATH + 1];
			WCharToMByte(szfile.c_str(), sztemp, MAX_PATH);
			TSaveData::ExportTOrder(m_pcontainer->GetContainer(), m_stlconfig, sztemp, true);
		}
	}
}
void TFormProcess::OnGridCtrlAction(WPARAM wParam, LPARAM lParam)
{
	GV_CLKINFO* pinfo = (GV_CLKINFO*)lParam;
	TCell cellid;
	cellid = pinfo->cellid;

	if (pinfo->naction == GVA_RCLK)
		PostMessage(m_Hwnd, SSWM_DATA_INFO_SHOW_MENU, 0, 0);
}
LRESULT TFormProcess::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
		OnCommand(wParam, lParam);
		break;
	case SSWM_GRID_CTRL_ACTION:
		OnGridCtrlAction(wParam, lParam);
		break;
	default:
		return __super::WndProc(message, wParam, lParam);
	}

	return PROCESSED;
}
void TFormProcess::UpdateList(int nstartrow)
{
	if (!m_pgrid) return;
	m_pgrid->SetRowCount(m_pcontainer->GetSize() + 1);

	DataRange strange;
	m_pgrid->GetNonfixDataRange(strange, (nstartrow == 0));
	for (_uint nrow = strange.nbeginrow; nrow <= strange.nendrow; nrow++)
	{
		TOrder* porder = NULL;
		porder = (TOrder*)m_pcontainer->ElementAt(nrow - 1);
		if(!porder) break;
	
		for (_uint ncol = 0; ncol < m_pgrid->GetColumnCount(); ncol++)
		{
			GV_ITEM item;
			memset(&item, 0, sizeof(GV_ITEM));
			item.cellid.ncol = ncol;
			item.cellid.nrow = nrow;
			item.lgfont = GetCurFont()/*g18_font*/;
			if (g_language == ENU) item.nvmargin = 3;
			else item.nvmargin = 5;
			item.nhmargin = 3;
			item.dwmask = GVIF_TEXT | GVIF_FONT | GVIF_FORMAT | GVIF_FGCLR | GVIF_VMARGIN | GVIF_HMARGIN;
			item.dwformat = DT_VCENTER;

			ConfigData stcofig = m_stlconfig[ncol];
			if (stcofig.nformat == 0)
				item.dwformat |= DT_LEFT;
			else if (stcofig.nformat == 1)
				item.dwformat |= DT_CENTER;
			else
				item.dwformat |= DT_RIGHT;
			
			int ntmpcol = stcofig.nrealindex;
			if (porder->Deleted == dtYes)
				item.clrfg = g_color_gray;
			else if ((ntmpcol >= 3 && ntmpcol <= 7) || ntmpcol == 10)
			{
				if (porder->Direct == dBuy) //item.clrbk = RGB(255, 222, 226);
				{
					if (g_color_style == 0) item.clrbk = RGB(255, 222, 226);
					else item.clrbk = RGB(221, 245, 226);
				}
				else if (porder->Direct == dSell)// item.clrbk = RGB(221, 245, 226);
				{
					if (g_color_style == 0) item.clrbk = RGB(221, 245, 226);
					else item.clrbk = RGB(255, 222, 226);
				}
			}
			else item.clrfg = g_color_listtext;

			switch (ntmpcol)
			{
			case 6:
				if (porder->Deleted != dtYes&&porder->Direct == dBuy) //item.clrfg = RGB(255, 0, 0);
				{
					if (g_color_style == 0) item.clrfg = RGB(255, 0, 0);
					else item.clrfg = RGB(0, 128, 0);
				}
				else if (porder->Deleted != dtYes&&porder->Direct == dSell)// item.clrfg = RGB(0, 128, 0);
				{
					if (g_color_style == 0) item.clrfg = RGB(255, 0, 0);
					else item.clrfg = RGB(0, 128, 0);
				}
				break;
			}
			UpdateListText(ntmpcol, porder, item);
			m_pgrid->SetItem(&item);
		}
	}
}

void TFormProcess::UpdateListText(int nrealindex, TOrder* porder, GV_ITEM& item)
{
	switch (nrealindex)
	{
	case 0:strcpy_s(item.szparam, porder->SettleDate); break;
	case 1:strcpy_s(item.szparam, porder->UserNo); break;
	case 2:
	{
		SetOrderStateColor(porder->OrderState, item);
		TOrderState2Char(porder->OrderState, porder->StrategyType, item.szparam);
		break;
	}
	case 3:strcpy_s(item.szparam, porder->ExchangeNo); break;
	case 4:
		if (porder->pCommodity)
			strcpy_s(item.szparam, porder->pCommodity->CommodityName);
		MByteToMByte_Lang(item.szparam, sizeof(item.szparam), CP_UTF8);
		break;
	case 5:Contract2Str(porder, item.szparam, MAX_PATH);break;
	case 6://TDirect2Char(porder->Direct, item.szparam); break;
		TDirect2Char(porder->Direct, item.szparam);
		break;
	case 7:TOffset2Char(porder->Offset, item.szparam); break;
	case 8:
		if (porder->pCommodity)
			SFormatPrice(porder->pCommodity, porder->OrderPrice, item.szparam, sizeof(item.szparam));
		else
			TDouble2Char(porder->OrderPrice, "%0.1f", item.szparam);
		break;
	case 9:TInt2Char((int)porder->OrderQty, "%d", item.szparam); break;
	case 10:TOrderType2Char(porder->OrderType, item.szparam); break;
	case 11:
		if (porder->pCommodity)
			SFormatPrice(porder->pCommodity, porder->MatchPrice, item.szparam, sizeof(item.szparam), false, ptAverage);
		else
			TDouble2Char(porder->MatchPrice, "%0.2f", item.szparam);
		break;
	case 12:TInt2Char((int)porder->MatchQty, "%d", item.szparam); break;
	case 13:TTriggerMode2Char(porder->TriggerMode, item.szparam); break;
	case 14:TTriggerCondition2Char(porder->TriggerCondition, item.szparam); break;
	case 15:
		if (porder->pCommodity)
			SFormatPrice(porder->pCommodity, porder->TriggerPrice, item.szparam, sizeof(item.szparam));
		else
			TDouble2Char(porder->TriggerPrice, "%0.0f", item.szparam);
		break;
	case 16:THedge2Char(porder->Hedge, item.szparam); break;
	case 17:strcpy_s(item.szparam, porder->EnquiryNo); break;
	case 18:TTradeSection2Char(porder->AddOneIsValid, item.szparam); break;
	case 19:strcpy_s(item.szparam, porder->OrderNo); break;
	case 20:strcpy_s(item.szparam, porder->LocalNo); break;
	case 21:strcpy_s(item.szparam, porder->SystemNo); break;
	case 22:strcpy_s(item.szparam, porder->InsertNo); break;
	case 23:TStrokeTime2Char(porder->InsertDateTime, 0, 10, item.szparam); break;
	case 24:TStrokeTime2Char(porder->InsertDateTime, 11, 8, item.szparam); break;

	case 25:strcpy_s(item.szparam, porder->UpdateNo); break;

	case 26:TStrokeTime2Char(porder->UpdateDateTime, 0, 10, item.szparam); break;
	case 27:TStrokeTime2Char(porder->UpdateDateTime, 11, 8, item.szparam); break;

	case 28:TValidType2Char(porder->ValidType, item.szparam); break;
	case 29:
		if (porder->ValidType == vtGTD)
		{
			strcpy_s(item.szparam, porder->ValidTime);
		}
		break;
	case 30:TOrderWay2Char(porder->OrderWay, item.szparam); break;
	case 31:TCommodityType2Char(porder->CommodityType, item.szparam); break;
	case 32:TErrorInfo2Char(porder->ErrorCode, porder->ErrorText, item.szparam); MByteToMByte_Lang(item.szparam, sizeof(item.szparam)); break;
	case 33:TStrategyType2Char(porder->StrategyType, item.szparam); break;
	case 34:TBool2Char(porder->IsRiskOrder, item.szparam); break;//Ç¿Æ½
	case 35:TBool2Char(porder->AddOne, item.szparam); break;//
	case 36:strcpy_s(item.szparam, porder->UpperNo); break;//
	case 37:strcpy_s(item.szparam, porder->UpperChannelNo); break;//
	case 38:TBool2Char(porder->IsInput, item.szparam); break;//Â¼µ¥
	case 39:TInt2Char((int)porder->ParentOrderID, "%d", item.szparam); break;
	case 40:strcpy_s(item.szparam, porder->ExchangeSystemNo); break;//
	case 41:TInt2Char((int)porder->MinOrderQty, "%d", item.szparam); break;
	case 42:TInt2Char((int)porder->MaxOrderQty, "%d", item.szparam); break;
	case 43:sprintf_s(item.szparam, "%d", porder->MarketLevel); break;
	case 44:TInt2Char((int)porder->StreamID, "%d", item.szparam); break;
	case 45:strcpy_s(item.szparam, porder->Remark); break;
	case 46:
// 		{
// 			const TUserInfo* pinfo;
// 			pinfo = G_TradeData->GetUser(porder->UserNo, porder->Sign);
// 			if (pinfo&&pinfo->pLogin)
// 				strcpy_s(item.szparam, pinfo->pLogin->LoginUser);
// 			MByteToMByte_Lang(item.szparam, sizeof(item.szparam));
// 		}
		break;
	case 47:
		{
			const TUserInfo* pinfo;
			pinfo = G_TradeData->GetUser(porder->UserNo, porder->Sign);
			if (pinfo)strcpy_s(item.szparam, pinfo->UserName);
			MByteToMByte_Lang(item.szparam, sizeof(item.szparam));
			break;
		}
	}
}

void TFormProcess::SetAutoWidth()
{
	if (!m_pgrid) return;

	SIZE size;
	HFONT hfont;
	HDC hdc = GetDC(m_Hwnd);
	std::vector<ConfigData>::iterator itor;

	LOGFONT lf = GetCurFont();
	hfont = CreateFontIndirect(&lf/*&g18_font*/);
	SelectObject(hdc, hfont);
	for (itor = m_stlconfig.begin(); itor != m_stlconfig.end(); itor++)
	{
		GetTextExtentPoint32A(hdc, itor->szcol.c_str(), itor->szcol.length(), &size);
		int nwidth;
		nwidth = size.cx + 15;
		itor->ncolwidth = max(0, nwidth);
	}

	DataRange strange;
	m_pgrid->GetNonfixDataRange(strange);
	for (_uint nrow = strange.nbeginrow; nrow <= strange.nendrow; nrow++)
	{
		TOrder* porder = NULL;
		porder = (TOrder*)m_pcontainer->ElementAt(nrow - 1);

		if (!porder)break;
		for (itor = m_stlconfig.begin(); itor != m_stlconfig.end(); itor++)
		{
			GV_ITEM item;

			UpdateListText(itor->nrealindex, porder, item);
			GetTextExtentPoint32A(hdc, item.szparam, strlen(item.szparam), &size);
			int nwidth;
			nwidth = size.cx + 15;
			itor->ncolwidth = max(itor->ncolwidth, nwidth);
		}
	}

	SaveConfigWidth();
	DeleteObject(hfont);
	ReleaseDC(m_Hwnd, hdc);
}

void TFormProcess::ChangeColConfig()
{
	bool busertrays = IsUserTrays();
	bool buserExternal = IsUserExternal();
	bool bexternaltrader = IsUSerExternalTrader();
	int nsize = m_stlconfig.size();
	std::vector<ConfigData>::iterator itor = m_stlconfig.begin();
	for (int i = 0; i < nsize; itor++, i++)
	{
		bool benable = false;
		int nrealindex = itor->nrealindex;
		if (!busertrays && (nrealindex == 7 || nrealindex == 16
			|| nrealindex == 33 || nrealindex == 43))
			benable = true;
		if (!buserExternal && (nrealindex == 18
			|| nrealindex == 35|| (nrealindex >= 38 && nrealindex <= 42)))
			benable = true;

		if (nrealindex == 36 && (!bexternaltrader&&!busertrays))
			benable = true;

		if (benable)
			itor->bauthor = false;
		else
			itor->bauthor = true;
	}
}