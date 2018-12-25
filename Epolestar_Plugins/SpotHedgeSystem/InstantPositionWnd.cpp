#include "PreInclude.h"
extern ISpotSystemApi		*g_pSpotSystem;				//套保平台
#define cCAPTION_H			45
extern G_COLOREF			g_ColorRefData;				//色彩
extern G_UIFONT				g_FontData;					//字体
extern IStarApi            *g_pStarApi;					//行情 

InstantPositionWnd::InstantPositionWnd()
	:m_tradeDot(0.0), m_cursel(0), m_bReqry(true)
{
	m_hQryMatch = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hQrySpecPrice = CreateEvent(NULL, TRUE, FALSE, NULL);
}

InstantPositionWnd::~InstantPositionWnd()
{
}

void InstantPositionWnd::Create(LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle)
{
	CreateFrm(classname, parent, style, exstyle);
}

void InstantPositionWnd::Register(SystemFrame * pdata)
{
	if (!pdata)
		return;
	m_System = pdata;
}

void InstantPositionWnd::AddPosToList(string openAgreeNo, vector<InstantPosInfo>& v_pos)
{
	//int count = 0;
	//if (index == -1)
	//	count = m_listInstantPosition.GetItemCount();
	//else
	//	count = index;

	m_listInstantPosition.DeleteAllItems();

	if (v_pos.empty())
		return;

	double totalPreferPrice = 0.0;
	double totalPreferQty = 0;

	for (int i = 0; i < v_pos.size(); i++)
	{	
		//资金账号
		m_listInstantPosition.AddItem(v_pos.at(i).UserNo, i, 0,  DT_CENTER, g_ColorRefData.GetColorBlack());
		//合约
		string sCode = v_pos.at(i).ContractNo;
		string tCode;
		CConvert::SContractNoToTContractNo(sCode, tCode);
		m_listInstantPosition.AddItem(tCode.c_str(), i, 1, DT_CENTER, g_ColorRefData.GetColorBlack());
		//买卖方向
		if (v_pos.at(i).Direct == SMS_DIRECT_BUY)
			m_listInstantPosition.AddItem("买", i, 2, DT_LEFT, g_ColorRefData.GetColorBuyRed());
		else if (v_pos.at(i).Direct == SMS_DIRECT_SELL)
			m_listInstantPosition.AddItem("卖", i, 2, DT_RIGHT, g_ColorRefData.GetColorSellBlue());
		//持仓量
		m_listInstantPosition.AddItem((double)v_pos.at(i).PositionQty, i, 3, 0, DT_CENTER, g_ColorRefData.GetColorBlack());
		//均价
		m_listInstantPosition.AddItem((double)v_pos.at(i).AvePostionPrice, i, 4, 2, DT_CENTER, g_ColorRefData.GetColorBlack());
		//期货参考盈亏
		double profit = 0.0;
		if (v_pos.at(i).Direct == SMS_DIRECT_BUY)
		{
			profit = (v_pos.at(i).LastPrice - v_pos.at(i).AvePostionPrice)*v_pos.at(i).PositionQty * m_tradeDot;
		}
		else if (v_pos.at(i).Direct == SMS_DIRECT_SELL)
		{
			profit = (v_pos.at(i).AvePostionPrice - v_pos.at(i).LastPrice)*v_pos.at(i).PositionQty * m_tradeDot;
		}
		m_listInstantPosition.AddItem(profit, i, 5, 2, DT_CENTER, g_ColorRefData.GetColorBlack());
		//最新价
		m_listInstantPosition.AddItem((double)v_pos.at(i).LastPrice, i, 6, 2, DT_CENTER, g_ColorRefData.GetColorBlack());

		totalPreferPrice += v_pos.at(i).PreferPrice * v_pos.at(i).PositionQty;
		totalPreferQty += v_pos.at(i).PositionQty;
	}

	//获取合同号
	string key = m_System->get_curkey();
	SmsAgreementDataRsp	openagree;
	memset(&openagree, 0, sizeof(SmsAgreementDataRsp));
	memcpy_s(&openagree, sizeof(SmsAgreementDataRsp), &(m_System->m_map_mapAgreement[key][openAgreeNo]), sizeof(SmsAgreementDataRsp));

	int count = m_listInstantPosition.GetItemCount();
	m_listInstantPosition.AddItem("资金账号", count, 0, DT_CENTER, g_ColorRefData.GetColorBlack());
	m_listInstantPosition.AddItem("规格", count, 1, DT_CENTER, g_ColorRefData.GetColorBlack());
	m_listInstantPosition.AddItem("销售/采购", count, 2, DT_CENTER, g_ColorRefData.GetColorBlack());
	m_listInstantPosition.AddItem("现货数量", count, 3, DT_CENTER, g_ColorRefData.GetColorBlack());
	m_listInstantPosition.AddItem("现货价格", count, 4, DT_CENTER, g_ColorRefData.GetColorBlack());
	m_listInstantPosition.AddItem("现货持仓盈亏", count, 5, DT_CENTER, g_ColorRefData.GetColorBlack());
	m_listInstantPosition.AddItem("现货最新价", count, 6, DT_CENTER, g_ColorRefData.GetColorBlack());
	m_listInstantPosition.AddItem("现货推荐价", count, 7, DT_CENTER, g_ColorRefData.GetColorBlack());
	count++;
	m_listInstantPosition.AddItem(openagree.UserNo, count, 0, DT_CENTER, g_ColorRefData.GetColorBlack());
	m_listInstantPosition.AddItem(openagree.SpecificationNo, count, 1, DT_CENTER, g_ColorRefData.GetColorBlack());
	m_listInstantPosition.AddItem(openagree.AgreementType == SMS_AGREEMENT_PURCHASE ? "采购" : "销售", count, 2, DT_CENTER, g_ColorRefData.GetColorBlack());
	//现货剩余量
	int spotremain = CalculatSpotRemain(openAgreeNo);
	m_listInstantPosition.AddItem(spotremain, count, 3, 0, DT_CENTER, g_ColorRefData.GetColorBlack());
	m_listInstantPosition.AddItem(openagree.SpotPrice, count, 4, 2, DT_CENTER, g_ColorRefData.GetColorBlack());
	//现货盈亏
	//现货最新价
	double spot_lastprice = 0.0;
	auto it_spec_price = m_map_specPrice.find(openagree.SpecificationNo);
	if (it_spec_price != m_map_specPrice.end())
	{
		if (!m_map_specPrice[openagree.SpecificationNo].empty())
			spot_lastprice = m_map_specPrice[openagree.SpecificationNo].begin()->SpecificationPirce;
	}

	double spotProfit = 0.0;
	if (openagree.AgreementType == SMS_AGREEMENT_PURCHASE)
		spotProfit = spotremain * (spot_lastprice - openagree.SpotPrice);
	else
		spotProfit = spotremain * (openagree.SpotPrice - spot_lastprice);
	m_listInstantPosition.AddItem(spotProfit, count, 5, 2, DT_CENTER, g_ColorRefData.GetColorBlack());

	m_listInstantPosition.AddItem(spot_lastprice, count, 6, 2, DT_CENTER, g_ColorRefData.GetColorBlack());
	//推荐价
	char  s_preferprice[32] = { 0 };
	if (openagree.AgreementType == SMS_AGREEMENT_PURCHASE)
	{
		sprintf_s(s_preferprice, ">=%.2f", totalPreferQty == 0 ? 0.0 : totalPreferPrice / totalPreferQty);
	}
	else if (openagree.AgreementType == SMS_AGREEMENT_SALE)
	{
		sprintf_s(s_preferprice, "<=%.2f", totalPreferQty == 0 ? 0.0 : totalPreferPrice / totalPreferQty);
	}
	m_listInstantPosition.AddItem(s_preferprice, count, 7, DT_CENTER, g_ColorRefData.GetColorBlack());
}

LRESULT InstantPositionWnd::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
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
	//case WM_INSTANTPOSITION_REFRESH:
	//	CalculatePosition();
	//	return PROCESSED;
	case WM_TIMER:
		OnTimer();
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

void InstantPositionWnd::OnCreate()
{
	InitCommonControls();
	m_hInstance = GetModuleHandle(NULL);
	///////////////////////////////Create//////////////////////////////////
	//合同编号
	m_labAgreementNo.Create(m_Hwnd, L"合同编号", DT_CENTER, ID_LabAgreementNo);
	m_cbAgreementNo.Create(m_Hwnd, ID_CbAgreementNo);
	//查询
	m_btnQry.Create(m_Hwnd, "查  询", ID_BtnQry);
	//平仓
	m_btnCover.Create(m_Hwnd, "平  仓", ID_BtnCover);
	//表格
	m_listInstantPosition.CreateList(m_Hwnd, LVS_REPORT | LVS_OWNERDRAWFIXED | WS_BORDER, 25, 25, LVS_EX_FULLROWSELECT, ID_ListView_InstantPosition);
	//////////////////////////////Init////////////////////////////////////

	m_listInstantPosition.AddColumn(L"资金账号", 80);
	m_listInstantPosition.AddColumn(L"合约", 80);
	m_listInstantPosition.AddColumn(L"买卖", 80);
	m_listInstantPosition.AddColumn(L"持仓量", 80);
	m_listInstantPosition.AddColumn(L"持仓均价", 80);
	m_listInstantPosition.AddColumn(L"期货持仓盈亏", 80);
	m_listInstantPosition.AddColumn(L"最新价", 80);
	m_listInstantPosition.AddColumn(L"", 80);

	m_labAgreementNo.SetFont(g_FontData.GetFontWord13());
	m_cbAgreementNo.SetFont(g_FontData.GetFontNum15());

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
	m_labAgreementNo.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_cbAgreementNo.MoveWindow(pt.x, pt.y, editWidth*3, Height * 10);
	pt.x += editWidth *3 + htap*3;
	m_btnQry.MoveWindow(pt.x, pt.y, btnWidth, Height);
	pt.x += btnWidth + htap * 3;
	m_btnCover.MoveWindow(pt.x, pt.y, btnWidth, Height);
	pt.x = 0; pt.y = cCAPTION_H;
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	m_listInstantPosition.MoveWindow(rect.left, pt.y, rect.right - rect.left, rect.bottom - rect.top);

	//定时器
	SetTimer(m_Hwnd, 0, 1000, NULL);
}

void InstantPositionWnd::OnPaint()
{
	TMemDC mdc(m_Hwnd);
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	FillRect(mdc.GetHdc(), &rect, g_ColorRefData.GetHbrBackground());
}

void InstantPositionWnd::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if (HIWORD(wParam) == 0 && lParam == 0)//表明是菜单
	{
		switch (LOWORD(wParam))
		{
		default:
			break;
		}
	}
}

void InstantPositionWnd::OnDrawItem(WPARAM wParam, LPARAM lParam)
{
	LPDRAWITEMSTRUCT p = (LPDRAWITEMSTRUCT)lParam;
	switch (p->CtlID)
	{
	case ID_ListView_InstantPosition:
		m_listInstantPosition.OnDrawItem(lParam);
		break;
	default:
		break;
	}
}

void InstantPositionWnd::OnNotify(WPARAM wParam, LPARAM lParam)
{
	//switch (wParam)
	//{

	//default:
	//	break;
	//}
}

LRESULT InstantPositionWnd::OnCtlColor(WPARAM wParam, LPARAM lParam)
{
	SetBkMode((HDC)wParam, TRANSPARENT);
	return (LRESULT)g_ColorRefData.GetColorBackground();
}

void InstantPositionWnd::OnNcDestroy()
{

}

void InstantPositionWnd::OnDestroy()
{
}

void InstantPositionWnd::OnSize(WPARAM wParam, LPARAM lParam)
{
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	m_listInstantPosition.MoveWindow(rect.left, cCAPTION_H, rect.right - rect.left, rect.bottom - rect.top - cCAPTION_H);
}

void InstantPositionWnd::OnMeasureItem(WPARAM wParam, LPARAM lParam)
{
	LPMEASUREITEMSTRUCT p = (LPMEASUREITEMSTRUCT)lParam;
	m_listInstantPosition.OnMeasureItem(p->itemHeight);
}

void InstantPositionWnd::OnBtnClickDown(WPARAM wParam, LPARAM lParam)
{
	switch (lParam)
	{
	case ID_BtnQry:
		OnBtnQry();
		break;
	case ID_BtnCover:
		OnBtnCover();
		break;
	default:
		break;
	}
}

void InstantPositionWnd::OnTimer()
{
	int index = m_cbAgreementNo.GetSelect();
	if (index != m_cursel)
		m_bReqry = true;
	else
		m_bReqry = false;
	m_cursel = index;
	
	OnBtnQry(m_bReqry);
}

void InstantPositionWnd::OnBtnQry(bool b_reqry)
{
	if (!m_System)return;

	//解析开仓合同
	string opneAgreeNo("");
	wchar_t	cb_txt_agree[MAX_PATH] = { 0 };
	m_cbAgreementNo.GetText(cb_txt_agree);
	string agreeTxt = LoadRC::unicode_to_ansi(cb_txt_agree);
	vector<string> v_agree = m_System->SplitString(agreeTxt, ' ');
	if (!v_agree.empty())
		opneAgreeNo = v_agree.at(0);
	if (opneAgreeNo.empty())
		return;

	vector<InstantPosInfo> v_posinfo;
	v_posinfo.clear();

	CalculatePosition(opneAgreeNo, v_posinfo, b_reqry);

	AddPosToList(opneAgreeNo,v_posinfo);

}

void InstantPositionWnd::OnBtnCover()
{
	wchar_t	wtemp[128] = { 0 };
	char	stemp[128] = { 0 };
	//合同
	string agreement("");
	m_cbAgreementNo.GetText(wtemp);
	agreement = LoadRC::unicode_to_ansi(wtemp);
	vector<string> v_str;
	v_str.clear();
	v_str = m_System->SplitString(agreement, ' ');
	if (v_str.size() > 0)
		agreement = v_str.at(0);

	//合约
	string contract("");
	memset(&stemp, 0, sizeof(stemp));
	m_listInstantPosition.GetSubItem(0, 1, stemp, sizeof(stemp));
	contract = stemp;

	//数量
	int futureqty = 0;
	memset(&stemp, 0, sizeof(stemp));
	m_listInstantPosition.GetSubItem(0, 3, stemp, sizeof(stemp));
	futureqty = atoi(stemp);

	//跳转
	m_System->m_sendOrderWnd->JumpOrderWnd(agreement, contract, futureqty);
}


int InstantPositionWnd::CalculatePosition(string targetAgreeNo, vector<InstantPosInfo> & v_posinfo, bool b_reqry)
{
	if (targetAgreeNo.empty())
		return 0;

	string key = m_System->get_curkey();
	if (b_reqry)
	{
		//查现货价格
		SmsSpecPriceQryReq specPriceReq;
		memset(&specPriceReq, 0, sizeof(SmsSpecPriceQryReq));
		strncpy_s(specPriceReq.CompanyNo, m_System->m_curLogin.CompanyNo);
		SYSTEMTIME	curdate;
		memset(&curdate, 0, sizeof(SYSTEMTIME));
		GetSystemTime(&curdate);
		char date[21] = { 0 };
		sprintf_s(date, "%d-%02d-%02d", curdate.wYear, curdate.wMonth, curdate.wDay);
		strncpy_s(specPriceReq.EndDate, date);
		string specNo("");
		specNo = m_System->m_map_mapAgreement[key][targetAgreeNo].SpecificationNo;
		strncpy_s(specPriceReq.SpecificationNo, specNo.c_str());

		g_pSpotSystem->QrySpecPrice(&specPriceReq);
		DWORD signal = WaitForSingleObject(m_hQrySpecPrice, 5000);
		if (signal == WAIT_TIMEOUT)
		{
			_TMessageBox_Domodal(m_Hwnd, "提示", L"查询现货价格超时", TMB_OK);
			return 0;
		}
		m_map_specPrice = m_System->m_mapSpecPrice;

		//查成交
		SmsMatchQryReq matchReq;
		memset(&matchReq, 0, sizeof(SmsMatchQryReq));
		//清数据
		m_map_Agree.clear();
		m_map_mAgreeMatch.clear();
		m_map_sAgreeRel.clear();

		//获取目标合同数据
		m_map_Agree[targetAgreeNo] = m_System->m_map_mapAgreement[key][targetAgreeNo];
		set<SmsAgreementDataRsp> m_setRelAgree;
		m_setRelAgree.clear();
		//获取相关联的合同号
		if (m_map_Agree[targetAgreeNo].AgreementType == SMS_AGREEMENT_PURCHASE)
		{
			for (auto it_rel : m_System->m_mapAgreementRel[key])
			{
				if (strcmp(it_rel.BuyAgreementNo, targetAgreeNo.c_str()) == 0)
				{
					//<合同号，合同数据>
					m_map_Agree[it_rel.SellAgreementNo] = m_System->m_map_mapAgreement[key][it_rel.SellAgreementNo];
					//<相关合同数据>
					m_setRelAgree.insert(m_System->m_map_mapAgreement[key][it_rel.SellAgreementNo]);
				}
			}
		}
		else
		{
			for (auto it_rel : m_System->m_mapAgreementRel[key])
			{
				if (strcmp(it_rel.SellAgreementNo, targetAgreeNo.c_str()) == 0)
				{
					//<合同号，合同数据>
					m_map_Agree[it_rel.BuyAgreementNo] = m_System->m_map_mapAgreement[key][it_rel.BuyAgreementNo];
					//<相关合同数据>
					m_setRelAgree.insert(m_System->m_map_mapAgreement[key][it_rel.BuyAgreementNo]);
				}
			}
		}

		//<开仓数据，<平仓数据>>
		m_map_sAgreeRel[m_map_Agree[targetAgreeNo]] = m_setRelAgree;

		//序列号
		matchReq.SerialId = 0;
		//公司
		strcpy_s(matchReq.CompanyNo, m_System->m_curLogin.CompanyNo);
		//用户名
		strcpy_s(matchReq.UserNo, m_System->m_curLogin.UserNo);

		//查询成交
		for (auto it_req : m_map_Agree)
		{
			ResetEvent(m_hQryMatch);
			strncpy_s(matchReq.AgreementNo, it_req.first.c_str());

			//m_System->m_matchWnd->m_listMatch.DeleteAllItems();
			m_System->m_mapMatch.clear();

			g_pSpotSystem->QryMatch(&matchReq);
			DWORD signal = WaitForSingleObject(m_hQryMatch, 2000);
			if (signal == WAIT_TIMEOUT)
			{
				_TMessageBox_Domodal(m_Hwnd, "提示", L"查询成交超时", TMB_OK);
				return 0;
			}
			if (!m_System->m_mapMatch.empty())
			{
				m_map_mAgreeMatch[matchReq.AgreementNo] = m_System->m_mapMatch;
			}
		}
	}

	//对冲=======================================================================================
	//遍历每个有关联的合同（每个合同先自对冲完，注意合约匹配 且 套保盈亏计算）
	//
	for (auto it_target : m_map_sAgreeRel)
	{
		m_tradeDot = 0.0;								//每手乘数
		string scommod = m_System->m_mapSpec[it_target.first.SpecificationNo].CommodityNo;
		SCommodity* commod[1] = { 0 };
		g_pStarApi->GetCommodityData("", scommod.c_str(), commod, 1, false);
		if (commod[0])
			m_tradeDot = commod[0]->TradeDot;

		vector<HedgeInfo>				v_selfHedge;	//自对冲
		v_selfHedge.clear();
		vector<HedgeInfo>				v_manHedge;		//一个目标合同合同所有指定平仓对冲关系
		v_manHedge.clear();

		//1先自对冲目标合同
		{
			for (auto& it_closeMatch : m_map_mAgreeMatch[it_target.first.AgreementNo])
			{
				if (it_closeMatch.second.Offset == SMS_O_COVER)
				{
					//平仓明细没有平完
					while (it_closeMatch.second.MatchQty > 0)
					{
						//平此合同开仓成交
						for (auto &it_openMatch : m_map_mAgreeMatch[it_target.first.AgreementNo])
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

								strncpy_s(tempinfo.OpenAgreementNo, it_target.first.AgreementNo);
								strncpy_s(tempinfo.CloseAgreementNo, it_target.first.AgreementNo);
								strncpy_s(tempinfo.OpenNo, it_openMatch.second.MatchNo);
								strncpy_s(tempinfo.CloseNo, it_closeMatch.second.MatchNo);
								v_selfHedge.push_back(tempinfo);
								break;
							}
						}
						//如果目标合同开仓成交数量都为零就可以break
						bool b_canBreak = true;
						for (auto &it_openMatch_b : m_map_mAgreeMatch[it_target.first.AgreementNo])
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
		//2遍历所有关联合同（每个关联合同执行人工平仓）
		for (auto it_rel : it_target.second)
		{			
			//遍历指定平仓
			for (auto it_manual : m_System->m_mapManualClose[key])
			{
				if (strcmp(it_manual.CloseAgreementNo, it_rel.AgreementNo) == 0)
				{
					HedgeInfo tempinfo;
					strncpy_s(tempinfo.OpenAgreementNo, it_target.first.AgreementNo);
					strncpy_s(tempinfo.CloseAgreementNo, it_rel.AgreementNo);
					strncpy_s(tempinfo.OpenNo, it_manual.OpenNo);
					strncpy_s(tempinfo.CloseNo, it_manual.CloseNo);
					tempinfo.CloseQty = it_manual.CloseQty;
					v_manHedge.push_back(tempinfo);
				}
			}
			//根据目标合同号，关联合同号，消除对应的成交数量
			for (auto it_reduceNum : v_manHedge)
			{
				//找目标合同成交
				auto it_openAgree = m_map_mAgreeMatch.find(it_target.first.AgreementNo);
				if (it_openAgree != m_map_mAgreeMatch.end())
				{
					auto it_openMatchNo = m_map_mAgreeMatch[it_target.first.AgreementNo].find(it_reduceNum.OpenNo);
					if (it_openMatchNo != m_map_mAgreeMatch[it_target.first.AgreementNo].end())
						m_map_mAgreeMatch[it_target.first.AgreementNo][it_reduceNum.OpenNo].MatchQty -= it_reduceNum.CloseQty;
					else
						continue;
				}
				else
					continue;
				//找关联合同成交
				auto it_closeAgree = m_map_mAgreeMatch.find(it_rel.AgreementNo);
				if (it_closeAgree != m_map_mAgreeMatch.end())
				{
					auto it_closeMatchNo = m_map_mAgreeMatch[it_rel.AgreementNo].find(it_reduceNum.CloseNo);
					if (it_closeMatchNo != m_map_mAgreeMatch[it_rel.AgreementNo].end())
						m_map_mAgreeMatch[it_rel.AgreementNo][it_reduceNum.CloseNo].MatchQty -= it_reduceNum.CloseQty;
					else
						continue;
				}
				else
					continue;
			}
		}
		//3遍历所有关联合同（每个关联合同先关联人工平仓，再进行自对冲，然后目标合同和关联合同相互对冲）
		for (auto it_rel : it_target.second)
		{
			//对冲
			vector<HedgeInfo>								v_matchNum;							//对冲数据
			v_matchNum.clear();
			//把人工平仓关系添加到显示界面
			for (auto it_allmanclose : v_manHedge)
			{
				if (strcmp(it_allmanclose.OpenAgreementNo, it_target.first.AgreementNo) == 0 && strcmp(it_allmanclose.CloseAgreementNo, it_rel.AgreementNo) == 0)
					v_matchNum.push_back(it_allmanclose);
			}

			//自对冲 （可以优化） 
			for (auto& it_closeMatch : m_map_mAgreeMatch[it_rel.AgreementNo])
			{
				if (it_closeMatch.second.Offset == SMS_O_COVER)
				{
					//平仓明细没有平完
					while (it_closeMatch.second.MatchQty > 0)
					{
						//平此合同开仓成交
						for (auto &it_openMatch : m_map_mAgreeMatch[it_rel.AgreementNo])
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

								strncpy_s(tempinfo.OpenAgreementNo, it_rel.AgreementNo);
								strncpy_s(tempinfo.CloseAgreementNo, it_rel.AgreementNo);
								strncpy_s(tempinfo.OpenNo, it_openMatch.second.MatchNo);
								strncpy_s(tempinfo.CloseNo, it_closeMatch.second.MatchNo);
								v_matchNum.push_back(tempinfo);
								break;
							}
						}
						//如果目标合同开仓成交数量都为零就可以break
						bool b_canBreak = true;
						for (auto &it_openMatch_b : m_map_mAgreeMatch[it_rel.AgreementNo])
						{
							if (strcmp(it_openMatch_b.second.ContractNo, it_closeMatch.second.ContractNo) == 0 && it_openMatch_b.second.Offset == SMS_O_OPEN)
								if (it_openMatch_b.second.MatchQty > 0)
									b_canBreak = false;
						}

						if (b_canBreak) break;
					}
				}
			}

			//遍历关联合同成交
			for (auto& it_closeMatch : m_map_mAgreeMatch[it_rel.AgreementNo])
			{
				if (it_closeMatch.second.Offset == SMS_O_COVER)
				{
					//还有没有平完
					while (it_closeMatch.second.MatchQty > 0)
					{
						//平目标合同成交
						for (auto &it_openMatch : m_map_mAgreeMatch[it_target.first.AgreementNo])
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

								strncpy_s(tempinfo.OpenAgreementNo, it_target.first.AgreementNo);
								strncpy_s(tempinfo.CloseAgreementNo, it_rel.AgreementNo);
								strncpy_s(tempinfo.OpenNo, it_openMatch.second.MatchNo);
								strncpy_s(tempinfo.CloseNo, it_closeMatch.second.MatchNo);
								v_matchNum.push_back(tempinfo);
								break;
							}
						}
						//如果开仓合同成交数量都为零就可以break
						bool b_canBreak = true;
						for (auto &it_openMatch_b : m_map_mAgreeMatch[it_target.first.AgreementNo])
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
	}

	//遍历开仓合同剩下的成交生成持仓
	auto it_end = m_map_mAgreeMatch.find(targetAgreeNo);
	if (it_end == m_map_mAgreeMatch.end())
		return 0;
	set<string>		set_contract;
	set_contract.clear();
	for (auto it_open_match : m_map_mAgreeMatch[targetAgreeNo])
	{
		if (it_open_match.second.MatchQty > 0 && it_open_match.second.Offset == SMS_O_OPEN)
		{
			set_contract.insert(it_open_match.second.ContractNo);
		}
	}

	for (auto it_set_contract : set_contract)
	{
		InstantPosInfo tempinfo;
		//合约
		strncpy_s(tempinfo.ContractNo, it_set_contract.c_str());

		double totalPrice = 0.0;

		for (auto it_open_match : m_map_mAgreeMatch[targetAgreeNo])
		{
			if (it_open_match.second.MatchQty > 0 && strcmp(it_set_contract.c_str(), it_open_match.second.ContractNo)==0 && it_open_match.second.Offset == SMS_O_OPEN)
			{
				totalPrice += (it_open_match.second.MatchPrice * it_open_match.second.MatchQty);
				tempinfo.Direct = it_open_match.second.Direct;
				tempinfo.PositionQty += it_open_match.second.MatchQty;
				if (strlen(tempinfo.UserNo) == 0)
					strncpy_s(tempinfo.UserNo, it_open_match.second.UserNo);
			}
		}
		tempinfo.AvePostionPrice = totalPrice / tempinfo.PositionQty;
		v_posinfo.push_back(tempinfo);
	}

	//计算推荐价格
	auto it_agree = m_map_Agree.find(targetAgreeNo);
	if (it_agree != m_map_Agree.end())
	{
		double spotPrice = m_map_Agree[targetAgreeNo].SpotPrice;

		for (auto &it : v_posinfo)
		{
			double preferprice = 0.0;
			double lastprice = 0.0;
			SContract* contract[1] = { 0 };
			g_pStarApi->GetContractData("", it.ContractNo, contract, 1, false);
			if (contract[0] && contract[0]->SnapShot)
			{
				lastprice = contract[0]->SnapShot->Data[S_FID_LASTPRICE].Price;
				if (lastprice == 0)
					lastprice = contract[0]->SnapShot->Data[S_FID_PRECLOSINGPRICE].Price;
			}

			
			if (m_map_Agree[targetAgreeNo].AgreementType == SMS_AGREEMENT_PURCHASE)
				preferprice = lastprice - it.AvePostionPrice + spotPrice;
			else if (m_map_Agree[targetAgreeNo].AgreementType == SMS_AGREEMENT_SALE)
				preferprice = spotPrice - it.AvePostionPrice + lastprice;
			it.PreferPrice = preferprice;
			it.LastPrice = lastprice;
			if (lastprice == 0.0)
				it.PreferPrice = 0.0;
		}
	}

	int position = 0;
	for (auto it : v_posinfo)
	{
		position += it.PositionQty;
	}
	return position;
}

int InstantPositionWnd::CalculatSpotRemain(string targetAgreeNo)
{

	//计算现货剩余
	int spotPair = 0;
	SmsAgreementDataRsp targetAgree = { 0 };
	vector<string>	v_relAgree;
	v_relAgree.clear();
	string key = m_System->get_curkey();
	memcpy(&targetAgree, &(m_System->m_map_mapAgreement[key][targetAgreeNo]), sizeof(SmsAgreementDataRsp));

	if (targetAgree.AgreementType == SMS_AGREEMENT_PURCHASE)
	{
		for (auto it_rel : m_System->m_mapAgreementRel[key])
		{
			if (strcmp(it_rel.BuyAgreementNo, targetAgreeNo.c_str()) == 0)
			{
				v_relAgree.push_back(it_rel.SellAgreementNo);
			}
		}
	}
	else
	{
		for (auto it_rel : m_System->m_mapAgreementRel[key])
		{
			if (strcmp(it_rel.SellAgreementNo, targetAgreeNo.c_str()) == 0)
			{
				v_relAgree.push_back(it_rel.BuyAgreementNo);
			}
		}
	}

	for (auto it : v_relAgree)
	{
		spotPair += m_System->m_map_mapAgreement[key][it].SpotQty;
	}

	return int(targetAgree.SpotQty) - spotPair < 0 ? 0 : int(targetAgree.SpotQty) - spotPair;
}
