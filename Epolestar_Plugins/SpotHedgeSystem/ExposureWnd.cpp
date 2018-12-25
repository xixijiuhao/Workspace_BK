#include "PreInclude.h"

extern ISpotSystemApi		*g_pSpotSystem;				//套保平台
extern G_COLOREF			g_ColorRefData;				//色彩
extern G_UIFONT				g_FontData;					//字体
extern IStarApi            *g_pStarApi;					//行情 

#define cCAPTION_H			45

ExposureWnd::ExposureWnd()
{
	m_hQryMatch = CreateEvent(NULL, TRUE, FALSE, NULL);
}

ExposureWnd::~ExposureWnd()
{
}

void ExposureWnd::Create(LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle)
{
	CreateFrm(classname, parent, style, exstyle);
}

void ExposureWnd::Register(SystemFrame * pdata)
{
	if (!pdata)
		return;
	m_System = pdata;
}

void ExposureWnd::AddExposureToList(string agreeno, double spotRemain, int futureCanOpen/*, double tradedots*/, int index)
{
	int count = 0;
	if (index == -1)
		count = m_listExposure.GetItemCount();
	else
		count = index;
	//合同编号
	m_listExposure.AddItem(agreeno.c_str(), count, 0, DT_CENTER, g_ColorRefData.GetColorBlack());
	//现货剩余
	//if(spotRemain == DBL_MAX)
	//	m_listExposure.AddItem("-----", count, 1,  DT_CENTER, g_ColorRefData.GetColorBlack());
	//else
	m_listExposure.AddItem(spotRemain, count, 1,0 , DT_CENTER, g_ColorRefData.GetColorBlack());
	//期货剩余
	m_listExposure.AddItem(futureCanOpen, count, 2, 0, DT_CENTER, g_ColorRefData.GetColorBlack());

	//char temp[128] = { 0 };
	//if (tradedots != 0)
	//{
	//	sprintf_s(temp, "%.0f(%d手)", futureRemain, int(futureRemain / tradedots));
	//	m_listExposure.AddItem(temp, count, 2, DT_CENTER, g_ColorRefData.GetColorBlack());
	//}else
	//	m_listExposure.AddItem("获取每手乘数错误", count, 2, DT_CENTER, g_ColorRefData.GetColorBlack());

}

LRESULT ExposureWnd::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		OnCreate();
		return PROCESSED;
	case WM_COMMAND:
		OnCommand(wParam, lParam);
		return PROCESSED;
	case SSWM_STATIC_BUTTON_CLICKDOWN:
		OnBtnClickDown(wParam, lParam);
		return PROCESSED;
	case WM_NOTIFY:
		OnNotify(wParam, lParam);
		return PROCESSED;

	case WM_SIZE:
		OnSize(wParam, lParam);
		return PROCESSED;
	case WM_CTLCOLORSTATIC:
		return OnCtlColor(wParam, lParam);
	case WM_PAINT:
		OnPaint();
		return PROCESSED;
	case WM_DRAWITEM:
		OnDrawItem(wParam, lParam);
		return PROCESSED;
	case WM_MEASUREITEM://==7
		OnMeasureItem(wParam, lParam);
		return PROCESSED;
	case WM_NCDESTROY:
		OnNcDestroy();
		break;
	case WM_DESTROY:
		OnDestroy();
		break;
	default:
		return NOT_PROCESSED;
	}
	return NOT_PROCESSED;
}

void ExposureWnd::OnCreate()
{
	InitCommonControls();
	m_hInstance = GetModuleHandle(NULL);
	///////////////////////////////Create//////////////////////////////////
	//合同编号
	//m_labOpenAgreementNo.Create(m_Hwnd, L"采购合同", DT_CENTER, ID_LabOpenAgreementNo);
	//m_cbOpenAgreementNo.Create(m_Hwnd, ID_CbOpenAgreementNo);
	//查询
	m_btnQry.Create(m_Hwnd, "刷  新", ID_BtnQry);
	//表格
	m_listExposure.CreateList(m_Hwnd, LVS_REPORT | LVS_OWNERDRAWFIXED | WS_BORDER, 25, 25, LVS_EX_FULLROWSELECT, ID_ListView_Exposure);
	//////////////////////////////Init////////////////////////////////////
	m_listExposure.AddColumn(L"合同编号", 200);
	m_listExposure.AddColumn(L"现货剩余", 200);
	m_listExposure.AddColumn(L"期货剩余（手）", 200);

	//m_labOpenAgreementNo.SetFont(g_FontData.GetFontWord13());
	//m_cbOpenAgreementNo.SetFont(g_FontData.GetFontNum15());

	/////////////////////POS///////////////////////////////////
	int leftstart = 10;
	int topstart = 10;
	int editWidth = 120;	//edit控件宽度
	int labWidth = 60;		//lab控件宽度
	int btnWidth = 80;		//btn控件宽度
	int Height = 25;		//控件高度
	int htap = 10;			//横向间隔

	POINT pt;
	pt.x = leftstart;
	pt.y = topstart;
	//m_labOpenAgreementNo.MoveWindow(pt.x, pt.y, labWidth, Height);
	//pt.x += labWidth + htap;
	//m_cbOpenAgreementNo.MoveWindow(pt.x, pt.y, editWidth * 3, Height * 10);
	//pt.x +=htap * 3;
	m_btnQry.MoveWindow(pt.x, pt.y, btnWidth, Height);
	pt.x = 0; pt.y = cCAPTION_H;
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	m_listExposure.MoveWindow(rect.left, pt.y, rect.right - rect.left, rect.bottom - rect.top);
}

void ExposureWnd::OnPaint()
{
	TMemDC mdc(m_Hwnd);
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	FillRect(mdc.GetHdc(), &rect, g_ColorRefData.GetHbrBackground());
}

void ExposureWnd::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if (HIWORD(wParam) == 0 && lParam == 0)//表明是菜单
	{
		//switch (LOWORD(wParam))
		//{
		//default:
		//	break;
		//}
	}
}

void ExposureWnd::OnDrawItem(WPARAM wParam, LPARAM lParam)
{
	LPDRAWITEMSTRUCT p = (LPDRAWITEMSTRUCT)lParam;
	switch (p->CtlID)
	{
	case ID_ListView_Exposure:
		m_listExposure.OnDrawItem(lParam);
		break;
	default:
		break;
	}
}

void ExposureWnd::OnNotify(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case ID_ListView_Exposure:
		DealMatchList(lParam);
		break;
	default:
		break;
	}
}

LRESULT ExposureWnd::OnCtlColor(WPARAM wParam, LPARAM lParam)
{
	SetBkMode((HDC)wParam, TRANSPARENT);
	return (LRESULT)g_ColorRefData.GetColorBackground();
}

void ExposureWnd::OnNcDestroy()
{

}

void ExposureWnd::OnDestroy()
{
}

void ExposureWnd::OnSize(WPARAM wParam, LPARAM lParam)
{
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	m_listExposure.MoveWindow(rect.left, cCAPTION_H, rect.right - rect.left, rect.bottom - rect.top - cCAPTION_H);
}

void ExposureWnd::OnMeasureItem(WPARAM wParam, LPARAM lParam)
{
	LPMEASUREITEMSTRUCT p = (LPMEASUREITEMSTRUCT)lParam;
	m_listExposure.OnMeasureItem(p->itemHeight);
}

void ExposureWnd::OnBtnClickDown(WPARAM wParam, LPARAM lParam)
{
	switch (lParam)
	{
	case ID_BtnQry:
		OnBtnQry();
		break;
	default:
		break;
	}
}

void ExposureWnd::DealMatchList(LPARAM lParam)
{
	//LPNMITEMACTIVATE p = (LPNMITEMACTIVATE)lParam;
	//if (p->hdr.code == NM_RCLICK)
	//{
	//	EnableMenu(p->hdr.idFrom, true);
	//	if (p->iItem >= m_listExposure.GetItemCount() || p->iItem == -1)
	//	{
	//		EnableMenuItem(m_menuMatch, IDM_ModMatch, MF_GRAYED);
	//		EnableMenuItem(m_menuMatch, IDM_DelMatch, MF_GRAYED);
	//		EnableMenuItem(m_menuMatch, IDM_ManageManualClose, MF_GRAYED);
	//	}
	//	else {
	//		//平仓才显示
	//		char	offset[21] = { 0 };
	//		m_listExposure.GetSubItem(p->iItem, MATCH_Offset, offset, sizeof(offset));
	//		if (strcmp(offset, "开仓") == 0)
	//		{
	//			EnableMenuItem(m_menuMatch, IDM_ManageManualClose, MF_GRAYED);
	//		}
	//	}
	//	ShowMenu(m_menuMatch);
	//}
}

void ExposureWnd::ShowMenu(HMENU hmenu)
{
	POINT pt;
	GetCursorPos(&pt);
	TrackPopupMenu(hmenu, TPM_LEFTALIGN | TPM_TOPALIGN, pt.x, pt.y, 0, m_Hwnd, NULL);
}

void ExposureWnd::EnableMenu(uint id, bool bEnable)
{
	//UINT flage = MF_GRAYED;
	//if (bEnable)
	//	flage = MF_ENABLED;

	//for (uint i = IDM_AddMatch; i <= IDM_ManageManualClose; i++)
	//	EnableMenuItem(m_menuMatch, i, flage);
}

void ExposureWnd::OnBtnQry()
{
	if (!m_System)return;

	m_listExposure.DeleteAllItems();

	//遍历所有合同
	string key = m_System->get_curkey();
	//现货剩余
	double	totalSpotRemain = 0.0;
	//double  totalFutureRemain = 0.0;
	int		totalCanOpen = 0;

	for (auto it_agree : m_System->m_map_mapAgreement[key])
	{
		//SmsMatchQryReq matchReq;
		//memset(&matchReq, 0, sizeof(SmsMatchQryReq));

		//strncpy_s(matchReq.CompanyNo, m_System->m_curLogin.CompanyNo);
		//strncpy_s(matchReq.UserNo, m_System->m_curLogin.UserNo);

		////解析合同
		//strncpy_s(matchReq.AgreeementNo, it_agree.second.AgreementNo);

		//ResetEvent(m_hQryMatch);

		//m_System->m_mapMatch.clear();
		//g_pSpotSystem->QryMatch(&matchReq);

		//WaitForSingleObject(m_hQryMatch, 1000);

		//==================================计算现货剩余============================================
		int sqotPair = 0;
		SmsAgreementDataRsp targetAgree = { 0 };
		vector<string>	v_relAgree;
		v_relAgree.clear();
		string key = m_System->get_curkey();
		memcpy(&targetAgree, &(m_System->m_map_mapAgreement[key][it_agree.second.AgreementNo]), sizeof(SmsAgreementDataRsp));

		if (targetAgree.AgreementType == SMS_AGREEMENT_PURCHASE)
		{
			for (auto it_rel : m_System->m_mapAgreementRel[key])
			{
				if (strcmp(it_rel.BuyAgreementNo, it_agree.second.AgreementNo) == 0)
				{
					v_relAgree.push_back(it_rel.SellAgreementNo);
				}
			}
		}
		else
		{
			for (auto it_rel : m_System->m_mapAgreementRel[key])
			{
				if (strcmp(it_rel.SellAgreementNo, it_agree.second.AgreementNo) == 0)
				{
					v_relAgree.push_back(it_rel.BuyAgreementNo);
				}
			}
		}

		for (auto it : v_relAgree)
		{
			sqotPair += m_System->m_map_mapAgreement[key][it].SpotQty;
		}


		//===============================================期货剩余=================================
		//double tradeDots = 0.0;
		//string target("");
		//target = m_System->m_mapSpec[targetAgree.SpecificationNo].CommodityNo;

		////每手乘数
		//SCommodity* commod[1] = { 0 };
		//g_pStarApi->GetCommodityData("", target.c_str(), commod, 1, false);
		//if (commod[0])
		//	tradeDots = commod[0]->TradeDot;
		double futureCanOpen = 0.0;
		futureCanOpen = CalculateFutureCanOpen(targetAgree.AgreementNo);

		//for (auto it_match : m_System->m_mapMatch)
		//{
		//	futureLots += it_match.second.MatchQty;
		//}

		//=================================添加到列表=========================================
		//if (targetAgree.AgreementType == SMS_AGREEMENT_PURCHASE)
		//{
		double spotRemain = int(targetAgree.SpotQty) - sqotPair; 
		spotRemain = spotRemain < 0 ? 0.0 : spotRemain;

		AddExposureToList(targetAgree.AgreementNo, spotRemain < 0 ? 0.0 : spotRemain, futureCanOpen);
		totalSpotRemain += spotRemain;
		//totalFutureRemain += (targetAgree.SpotQty - futureLots * tradeDots);
		totalCanOpen += futureCanOpen;

		//}
		//else
		//{
		//	AddExposureToList(targetAgree.AgreementNo, DBL_MAX, targetAgree.SpotQty - futureLots * tradeDots, tradeDots);
		//	if (tradeDots != 0)
		//	{
		//		totalFutureRemain += (targetAgree.SpotQty - futureLots * tradeDots);
		//		totalLotsRemain += (int(targetAgree.SpotQty / tradeDots) - futureLots);
		//	}
		//}	
	}
	//合计
	int count = m_listExposure.GetItemCount();
	m_listExposure.AddItem("合计", count, 0, DT_CENTER, g_ColorRefData.GetColorBlack());
	m_listExposure.AddItem(totalSpotRemain, count, 1, 0, DT_CENTER, g_ColorRefData.GetColorBlack());
	//char temp[128] = { 0 };
	//sprintf_s(temp, "%.0f(%d手)", totalFutureRemain, totalLotsRemain);
	m_listExposure.AddItem(totalCanOpen, count, 2, 0, DT_CENTER, g_ColorRefData.GetColorBlack());
}

int ExposureWnd::CalculateFutureCanOpen(string targetAgreeNo)
{
	if (targetAgreeNo.empty())
		return -2;
	string key = m_System->get_curkey();

	//合同数据
	SmsAgreementDataRsp targetAgreeData;
	memset(&targetAgreeData, 0, sizeof(SmsAgreementDataRsp));
	memcpy(&targetAgreeData, &(m_System->m_map_mapAgreement[key][targetAgreeNo]), sizeof(SmsAgreementDataRsp));
	//合同数据
	map<string, SmsMatchDataRsp>  targetMatchData;
	targetMatchData.clear();

	//查成交
	SmsMatchQryReq matchReq;
	memset(&matchReq, 0, sizeof(SmsMatchQryReq));
	strcpy_s(matchReq.CompanyNo, m_System->m_curLogin.CompanyNo);
	strcpy_s(matchReq.UserNo, m_System->m_curLogin.UserNo);
	strncpy_s(matchReq.AgreementNo, targetAgreeNo.c_str());

	ResetEvent(m_hQryMatch);
	m_System->m_mapMatch.clear();

	g_pSpotSystem->QryMatch(&matchReq);
	DWORD signal = WaitForSingleObject(m_hQryMatch, 2000);
	if (signal == WAIT_TIMEOUT)
	{
		_TMessageBox_Domodal(m_Hwnd, "提示", L"查询成交超时", TMB_OK);
		return -2;
	}
	if (!m_System->m_mapMatch.empty())
	{
		targetMatchData = m_System->m_mapMatch;
	}

	//自对冲
	vector<HedgeInfo>				v_selfHedge;	//自对冲
	v_selfHedge.clear();
	{
		for (auto& it_closeMatch : targetMatchData)
		{
			if (it_closeMatch.second.Offset == SMS_O_COVER)
			{
				//平仓明细没有平完
				while (it_closeMatch.second.MatchQty > 0)
				{
					//平此合同开仓成交
					for (auto &it_openMatch : targetMatchData)
					{
						if (strcmp(it_openMatch.second.ContractNo, it_closeMatch.second.ContractNo) != 0)
							continue;
						if (it_openMatch.second.Offset != SMS_O_OPEN)
							continue;

						if (it_openMatch.second.MatchQty > 0)
						{
							HedgeInfo tempinfo;

							if (it_openMatch.second.MatchQty >= it_closeMatch.second.MatchQty)
							{
								tempinfo.CloseQty = it_closeMatch.second.MatchQty;
								it_openMatch.second.MatchQty -= it_closeMatch.second.MatchQty;
								it_closeMatch.second.MatchQty = 0;
							}
							else
							{
								tempinfo.CloseQty = it_openMatch.second.MatchQty;
								it_closeMatch.second.MatchQty -= it_openMatch.second.MatchQty;
								it_openMatch.second.MatchQty = 0;
							}

							strncpy_s(tempinfo.OpenAgreementNo, targetAgreeNo.c_str());
							strncpy_s(tempinfo.CloseAgreementNo, targetAgreeNo.c_str());
							strncpy_s(tempinfo.OpenNo, it_openMatch.second.MatchNo);
							strncpy_s(tempinfo.CloseNo, it_closeMatch.second.MatchNo);
							v_selfHedge.push_back(tempinfo);
							break;
						}
					}
					//如果目标合同开仓成交数量都为零就可以break
					bool b_canBreak = true;
					for (auto &it_openMatch_b : targetMatchData)
					{
						if (strcmp(it_openMatch_b.second.ContractNo, it_closeMatch.second.ContractNo) == 0 && it_openMatch_b.second.Offset == SMS_O_OPEN)
							if (it_openMatch_b.second.MatchQty > 0)
								b_canBreak = false;
					}

					if (b_canBreak) break;
				}
			}
		}
	}
	//统计期货总开或者总平
	int sumFuture = 0;
	for (auto& it_match : targetMatchData)
	{
		if (it_match.second.MatchQty != 0)
		{
			sumFuture += it_match.second.MatchQty;
		}
	}

	double tradeDot = 0.0;								//每手乘数
	string scommod = m_System->m_mapSpec[targetAgreeData.SpecificationNo].CommodityNo;
	SCommodity* commod[1] = { 0 };
	g_pStarApi->GetCommodityData("", scommod.c_str(), commod, 1, false);
	if (commod[0])
		tradeDot = commod[0]->TradeDot;
	if (tradeDot == 0)
		return -1;

	int canopen = 0;
	canopen = int((targetAgreeData.SpotQty / tradeDot) - sumFuture);
	if (canopen <= 0)
		return 0;
	else
		return canopen;
}
