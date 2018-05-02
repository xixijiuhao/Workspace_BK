#include "PreInclude.h"

extern LanguageID g_language;

static char szCHS[][50] = {
	"提示", "下单数量不能为0!", "下单价格不能为0!", "没有可用持仓!", "投保不明!",
	"未选中持仓!", "未选中挂单!", "所选持仓不是上期所合约!", "该合约没有昨仓!", "要把所有的合约都平掉?", 
	"非组合单!", "没有找到组合持仓!", "无效合约!", "未选中数据", "是否对%s%s多头%d手持仓，进行对价全平?",
	"是否对%s%s空头%d手持仓，进行对价全平?", "警告"
};
static char szCHT[][50] = {
	"提示", "下單數量不能為0!", "下單價格不能為0!", "沒有可用持倉!", "投保不明!",
	"未選中持倉!", "未選中掛單!", "所選持倉不是上期所合約!", "該合約沒有昨倉!", "要把所有的合約都平掉?",
	"非組合單!", "沒有找到組合持倉!", "無效合約!", "未選中數據", "是否對%s%s多頭%d手持倉，進行對價全平?",
	"是否對%s%s空頭%d手持倉，進行對價全平?", "警告"
};
static char szENU[][75] = {
	"Warning", "Order Quantity cannot be zero!", "Price cannot be zero!", "No valid Position!", "Spec/Hedg unknown!",
	"Position unselected!", "Pending order unselected!", "Selected contract is not SHFE Contract!", "Selected contract have no Pre Position!", "Close All Position?",
	"Not Combo Order!", "Combo Position Not Found!", "Invalid Contract!", "Data unselected", "%s%s Long Position %d Lots,Close all at counter price?",
	"%s%s Short Position %d Lots,Close all at counter price?","Warning"
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

extern IStopLP* g_pStopLP;
//发送消息

#define ID_TimeConditionOrder 1

TPresenter::TPresenter(IMediator *pMediator) :IColleague(pMediator), m_clUserData(this)
{
	g_pGlobalModel->ReadConfig();
	m_clTradeInfoData.SetPresenter(this);
	m_clUserData.SetPresenter(this);
	m_clStopLPData.SetPresenter(this);

	m_bCanShowDetail = false;
}

void TPresenter::Init()
{
	CreateFrm(L"presenter", FindWindow(MainFrmName, NULL), 0);
	SetTimer(m_Hwnd, ID_TimeConditionOrder, 1000, nullptr);
}

void TPresenter::Send(unsigned int CommandID, const void* content)
{
	m_Mediator->Send(CommandID, content, this);
}

LRESULT TPresenter::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case SSWM_NOTICE:
		OnNotice(wParam);
		return PROCESSED;
	case SSWM_QUOTE:
		OnQuoteBack((SContract*)wParam);
		return PROCESSED;
	case SSWM_ONRESETUSERNO:
		OnResetUserNo();
		return PROCESSED;
	case SSWM_MONEY:
		if (m_clTKeyData.CanUpdateMoney((const TMoney*)wParam))
			m_Mediator->Send(CMD_Presenter_OnMoney, (const void*)wParam, this);
		return PROCESSED;
    case  SSWM_ONORDER:
        OnOrderBack((const TOrder*)wParam);
        return PROCESSED;
    case  SSWM_ONMATCH:
        OnMatchBack((const TMatch*)wParam);
        return PROCESSED;        
	case SSWM_POSITIIONTOTAL:
		OnTotalPosition((const TPosition*)wParam);
		return PROCESSED;
	case WM_DESTROY:
		g_pGlobalModel->WriteConfig();
		m_clConOrderData.SaveOrder();
		return PROCESSED;
	case WM_TIMER:
		if (wParam == ID_TimeConditionOrder)
		{
			m_clConOrderData.OnTimer();

			//每5s查一次保证金
			static int iCount = 0;
			iCount++;
			if (iCount % 4 == 0)
			{
				g_pTradeApi->SubscibePositionDeposit(dstQuery);
			}
			else if ((iCount - 1) % 4 == 0)
			{
				Send(CMD_Presenter_UpdateDeposit, 0);
			}
		}
		break;
	}
	return NOT_PROCESSED;
}

//持仓回调
void TPresenter::OnTotalPosition(const TPosition* pPosition)
{
	//判断是否是当前用户
	if (pPosition && strcmp(pPosition->UserNo, g_pGlobalModel->m_strUserNo.c_str()))//回调用户和当前用户不一致，则直接返回
	{
		return;
	}
	//重新设置持仓，判断是否需要刷新
	bool bRet = m_clTradeInfoData.SetPosition(pPosition);
	if (bRet)
	{
		if (pPosition->PositionQty)
			m_clUserData.SetContractNo(pPosition->UserNo, pPosition->Direct);
		else
			m_clUserData.SetContractNo(pPosition->UserNo, dNone);
	}
	m_clUserData.UpdateOrderData(false);
	m_Mediator->Send(CMD_Presenter_OnPositionTotal, (void*)bRet, this);
	//设置选中持仓
	m_Mediator->Send(CMD_Presenter_SetSelectedPosition, pPosition, this);
}

void TPresenter::OnQuoteBack(SContract* quote)
{
	if (m_clUserData.OnQuote(quote))
		m_Mediator->Send(CMD_Presenter_OnQuote, NULL, this);

	m_clConOrderData.OnQuote(quote);
}
void TPresenter::OnOrderBack(const TOrder* pOrder)
{
	m_clConOrderData.OnOrder(pOrder);

	if (pOrder && strcmp(pOrder->UserNo, g_pGlobalModel->m_strUserNo.c_str()))//回调用户和当前用户不一致，则直接返回
	{
		return;
	}
	m_clTradeInfoData.SetOrder();
    m_clTradeInfoData.SetParOrder();
	m_clTradeInfoData.SetLogInfo();
	m_Mediator->Send(CMD_Presenter_OnParOrder, (void*)m_clTradeInfoData.GetSelectParOrderID(), this);
    m_Mediator->Send(CMD_Presenter_OnOrder, (void*)pOrder, this);
}

void TPresenter::OnMatchBack(const TMatch* pMatch)
{
	if (pMatch&& strcmp(pMatch->UserNo, g_pGlobalModel->m_strUserNo.c_str()))//回调用户和当前用户不一致，则直接返回
	{
		return;
	}
    m_clTradeInfoData.SetMatch();
    m_Mediator->Send(CMD_Presenter_OnMatch, (void*)pMatch, this);
}
void TPresenter::OnResetUserNo(bool bNeedSetUser)
{
	int iSelect = m_clTKeyData.OnUserLogin(bNeedSetUser);

	m_Mediator->Send(CMD_Presenter_UserLogin, (void*)iSelect, this);   //更新用户
	OnUserChange();

	bool bLogin = true;
	if (m_clTKeyData.GetLoginUserCount() == 0)
	{
		bLogin = false;
		ShowWindow(m_hTKeyOrder, SW_HIDE);
	}
	g_pStatusBar->UserLogin(bLogin);
}

void TPresenter::OnNotice(WPARAM wParam)
{
	string strContractID = "";
	if (m_clUserData.SplitNotice(m_strNoticeContent, wParam, strContractID))
	{
		int iIndex = m_clTKeyData.IsUserChange(m_clUserData.GetUserNo());
		if (iIndex != -1)
		{
			m_Mediator->Send(CMD_Presenter_UserLogin, (void*)iIndex, this);   //更新用户
			OnUserChange();
		}
		bool bSetPositionQty = false;
		if (wParam == fromTradeD)
			bSetPositionQty = true;
		m_clUserData.DealContractNo(strContractID, bSetPositionQty);
        m_clTradeInfoData.SetContractNo(strContractID);

		//查看用户是否有对应持仓
		const TPosition * pTempPos = m_clUserData.GetPosition();
		//设置当前持仓
		
		if (pTempPos)//更新列表
		{
			m_Mediator->Send(CMD_Presenter_SetSelectedPosition, pTempPos, this);
			m_clTradeInfoData.SetCurrentPosition(pTempPos);
		}

		UpdatePrice();
		m_Mediator->Send(CMD_Presenter_Notice_OrderView, 0, this);		
	}
}

void TPresenter::DealContractCode(string sCode, TDirect direct)
{
	m_clUserData.DealContractNo(sCode, false, direct);
    m_clTradeInfoData.SetContractNo(sCode);
	m_Mediator->Send(CMD_Presenter_Notice_OrderView, 0, this);


	UpdatePrice();
}

void TPresenter::InsertOrder(TSendOrder * pOrder)
{
	m_clUserData.InsertOrder(pOrder);
}

void TPresenter::UserChange(int index)
{
	m_clTKeyData.SetCurUser(index);
	OnUserChange();
}

void TPresenter::OnUserChange()
{
	m_Mediator->Send(CMD_Presenter_OnMoney, m_clTKeyData.GetUserMoney(), this);   //更新资金
	m_clTradeInfoData.OnUserChange();
	m_clUserData.OnUserChange();
	m_Mediator->Send(CMD_Presenter_TViewUpdate, 0, this);
	m_Mediator->Send(CMD_Presenter_OnParOrder, 0, this);
	m_Mediator->Send(CMD_Presenter_OnPositionTotal, (const void*)true, this);
    m_Mediator->Send(CMD_Presenter_OnOrder, 0, this);
    m_Mediator->Send(CMD_Presenter_OnMatch, 0, this);
	m_Mediator->Send(CMD_Presenter_OnMoney, m_clTKeyData.GetUserMoney(), this);
	m_clConOrderData.OnUserChange();
}

void TPresenter::OnHotKeyDown(const bool Alt, const bool Ctrl, const bool Shift, const unsigned char VKey, const bool Exclusive, HWND source)
{
	TCHAR name[50] = { 0 };
	GetClassName(source, name, sizeof(name));
	if (lstrcmp(name, WC_EDIT) == 0 || lstrcmp(name, MSFTEDIT_CLASS) == 0||lstrcmp(name,DATETIMEPICK_CLASS)==0)
		return;

	Show();
}

void TPresenter::OnReceiveNotice(HWND From, HWND To, const char* sender, const char* action, const char* content)
{
	if (!g_Presenter)return;
	if (content == NULL) return;
	if (g_Presenter->m_strNoticeContent == content) return;
	bool bValid = false;
	UINT type = 0;
	if (!strcmp(sender, "PolestarQuote") && (!strcmp(action, "LButtonDown") || !strcmp(action, "VK_UP") || !strcmp(action, "VK_DOWN") || !strcmp(action, "SetContract")))
	{
		bValid = true;
		type = fromQuote;
	}
	else if (!strcmp(sender, "TradeDisplay") && !strcmp(action, "Bn_Click"))
	{
		bValid = true;
		type = fromTradeD;
	}
	if (bValid)
	{
		g_Presenter->m_strNoticeContent = content;
		PostMessage(g_Presenter->GetHwnd(), SSWM_NOTICE, type, 0);
	}
}

void TPresenter::SetContract(int index)
{
	const TPosition *pPosition = m_clTradeInfoData.GetPosition(index);
	if (pPosition != NULL)
	{
		char szCode[50] = { 0 };
		if (pPosition->CommodityType != ctOption)
			sprintf_s(szCode, "%s %s %s", pPosition->ExchangeNo, pPosition->CommodityNo, pPosition->ContractNo);
		else
			sprintf_s(szCode, "%s %s %s %c%s", pPosition->ExchangeNo, pPosition->CommodityNo, pPosition->ContractNo, pPosition->OptionType, pPosition->StrikePrice);
		string strCode = szCode;
		m_clUserData.SetContractNo(pPosition->UserNo,pPosition->Direct);
		m_clUserData.DealContractNo(strCode,true);
		m_clTradeInfoData.SetContractNo(strCode);
		m_Mediator->Send(CMD_Presenter_Notice_OrderView, 0, this);

		UpdatePrice();
	}
}

void TPresenter::UpdatePrice()
{
	m_clUserData.OnQuote();
	m_Mediator->Send(CMD_Presenter_OnQuote, NULL, this);

}
void TPresenter::OnClickBtn(WPARAM wParam,LPARAM lParam)
{
	//获取TradeInfo挂单区域的当前选中情况
	//
	m_clTradeInfoData.OnClickBtn(wParam, lParam); 
}

void TPresenter::OnQuote(const SContract* cont, const SQuoteUpdate* quote)
{
	//暂时不判断行情合约id
	PostMessage(m_Hwnd, SSWM_QUOTE, (WPARAM)cont, 0);
}

void TPresenter::OnInitProgress(const int ErrorCode, const TradeInitProgress CurrProgress, const TLoginBase * pLoginBase)
{
	if (ErrorCode == 0 && CurrProgress == tipReady)
	{
		if (m_bCanShowDetail)
		{
			ShowDetail();
			m_bCanShowDetail = false;
		}
		PostMessage(m_Hwnd, SSWM_ONRESETUSERNO, 0, 0);
	}
}

void TPresenter::OnOrder(const TOrder * pOrder)
{
    PostMessage(m_Hwnd, SSWM_ONORDER, (WPARAM) pOrder, 0);

}
void TPresenter::OnMatch(const TMatch * pMatch)
{
    PostMessage(m_Hwnd, SSWM_ONMATCH, (WPARAM)pMatch, 0);

}
void TPresenter::OnMoney(const TMoney * pMoney)
{
	PostMessage(m_Hwnd, SSWM_MONEY, (WPARAM)pMoney, 0);
}

void TPresenter::OnPositionTotal(const TTotalPosition * pPosition)
{
	PostMessage(m_Hwnd, SSWM_POSITIIONTOTAL, (WPARAM)pPosition, 0);
}

bool TPresenter::ShowMsgBox(int type, char *pszReason)
{
	switch (type)
	{
	case EN_OrderQty_0:
		_TMessageBox_Domodal(m_hTKeyOrder, _GetStringById(0), ANSIToUnicode(_GetStringById(1)).c_str(), TMB_OK);
		return false;
	case EN_OrderPrice_0:
		_TMessageBox_Domodal(m_hTKeyOrder, _GetStringById(0), ANSIToUnicode(_GetStringById(2)).c_str(), TMB_OK);
		return false;
	case EN_NoAva_Position:
		return _TMessageBox_Domodal(m_hTKeyOrder, _GetStringById(0), ANSIToUnicode(_GetStringById(3)).c_str(), TMB_OK);
	case Msg_OKOrCancel:
		return _TMessageBox_Domodal(m_hTKeyOrder, _GetStringById(0), ANSIToUnicode(pszReason).c_str(), TMB_OKCANCEL);
	case Msg_OK:
		return _TMessageBox_Domodal(m_hTKeyOrder, _GetStringById(0), ANSIToUnicode(pszReason).c_str(), TMB_OK);
	case EN_No_Hedge:
		return _TMessageBox_Domodal(m_hTKeyOrder, _GetStringById(0), ANSIToUnicode(_GetStringById(4)).c_str(), TMB_OK);
	case EN_Null_Position:
		return _TMessageBox_Domodal(m_hTKeyOrder, _GetStringById(0), ANSIToUnicode(_GetStringById(5)).c_str(), TMB_OK);
	case EN_Null_ParOrder:
		return _TMessageBox_Domodal(m_hTKeyOrder, _GetStringById(0), ANSIToUnicode(_GetStringById(6)).c_str(), TMB_OK);
	case EN_Not_SHFEPosition:
		return _TMessageBox_Domodal(m_hTKeyOrder, _GetStringById(0), ANSIToUnicode(_GetStringById(7)).c_str(), TMB_OK);
	case EN_No_LastSHFEPosition:
		return _TMessageBox_Domodal(m_hTKeyOrder, _GetStringById(0), ANSIToUnicode(_GetStringById(8)).c_str(), TMB_OK);
	case Msg_CoverAll_Position:
		return _TMessageBox_Domodal(m_hTKeyOrder, _GetStringById(16), ANSIToUnicode(_GetStringById(9)).c_str(), TMB_OKCANCEL);
	case EN_Not_CmbPosition:
		return _TMessageBox_Domodal(m_hTKeyOrder, _GetStringById(0), ANSIToUnicode(_GetStringById(10)).c_str(), TMB_OK);
	case EN_Not_FoundCmb:
		return _TMessageBox_Domodal(m_hTKeyOrder, _GetStringById(0), ANSIToUnicode(_GetStringById(11)).c_str(), TMB_OK);
	case EN_Invalid_ContractNo:
		return _TMessageBox_Domodal(m_hTKeyOrder, _GetStringById(0), ANSIToUnicode(_GetStringById(12)).c_str(), TMB_OK);
	case EN_CoverQty_TooMore:
		return _TMessageBox_Domodal(m_hTKeyOrder, _GetStringById(0), ANSIToUnicode(pszReason).c_str(), TMB_OKCANCEL);
	}
	return true;
}
void TPresenter::InsertStopLPOrder(TStopLPInsertItem* Tstop)
{
	m_clStopLPData.DealInsertOrder(Tstop);
	//g_pStopLP->InsertLPOrder(Tstop);
}

void TPresenter::UpdatePositionPrice()
{
	m_clStopLPData.UpdatePositionPrice();
}

void TPresenter::DeleteStopLPOrder(const unsigned int* content)
{
	g_pStopLP->DeleteLPOrder(*content);
}

void TPresenter::ModifyStopLPOrder(const TStopLPitem* TStop)
{
	m_clStopLPData.DealModifyOrder(TStop);
}

void TPresenter::RClkMenuStopLPOrder(const void* content,int SelPageOrKey,bool Mod)
{
	m_clStopLPData.RefreshContract((const TPosition*)content);
	if (Mod)
	{
		m_Mediator->Send(CMD_Presenter_SetStopLPView_FindKeyID, &SelPageOrKey, this);
	}
	else
	{
		m_Mediator->Send(CMD_Presenter_SetStopLPView, &SelPageOrKey, this);
	}
}

void TPresenter::UpdateConfig()
{
	m_clUserData.UpdateConfig();
	m_Mediator->Send(CMD_Presenter_Notice_OrderView, 0, this);
}

void TPresenter::CoverAll()
{
	if (ShowMsgBox(Msg_CoverAll_Position))
		g_pTradeAction->CoverAll((IDataContainer*)m_clTradeInfoData.GetParOrderCon(), (IDataContainer*)m_clTradeInfoData.GetPositionTotal());
}

void TPresenter::OnDisconnect(const TSocketDisconnectReason ErrorCode, const TLogin * pLogin)
{
	OnResetUserNo();
}

void TPresenter::UpdateMoney()
{
	const TMoney* pMoney = m_clTKeyData.GetUserMoney();
	if (pMoney)
		m_Mediator->Send(CMD_Presenter_UpdateTViewMoney, pMoney, this);
}

void TPresenter::CoverOne(int index)
{
	if (g_pGlobalModel->m_stOptionConfig.bDoubleClickPositionClose)
	{
		if (g_pGlobalModel->m_stOptionConfig.bDoublePositionConfirm)
		{
			const TPosition* pPosition = (const TPosition*)m_clTradeInfoData.GetPosition(index);
			if (!pPosition)
			{
				_TMessageBox_Domodal(m_hTKeyOrder, _GetStringById(0), ANSIToUnicode(_GetStringById(13)).c_str(), TMB_OK);
				return;
			}
			else
			{
				char szText[100] = { 0 };
				if (pPosition->Direct == dBuy)
				{
					sprintf_s(szText, _GetStringById(14), pPosition->CommodityNo, pPosition->ContractNo, pPosition->PositionQty);
				}
				else
				{
					sprintf_s(szText, _GetStringById(15), pPosition->CommodityNo, pPosition->ContractNo, pPosition->PositionQty);
				}
				if (_TMessageBox_Domodal(m_hTKeyOrder, _GetStringById(0), ANSIToUnicode(szText).c_str(), TMB_OKCANCEL))
				{
					m_clTradeInfoData.Cover(TTradeInfo::IDM_RIVALCOLSE);
				}
			}
		}
		else
		{
			m_clTradeInfoData.Cover(TTradeInfo::IDM_RIVALCOLSE);
		}
	}
}

bool TPresenter::InsertConOrder(TConditionOrder* pOrder)
{
	return m_clConOrderData.InsertConOrder(pOrder);
}

void TPresenter::UpdateConList()
{
	m_Mediator->Send(CMD_Presenter_UpdateConOrder, 0, this);
}

void TPresenter::OnMenuClk(const unsigned int MenuIndex, const HWND PastLife)
{
	Show();
}
void TPresenter::Show(int iShow)
{
 	if (!g_Presenter && iShow == 2)
		return;

	if (g_Presenter)
	{
		if (!g_Presenter->GetHwnd() && iShow == 2)
			return;
	}


	if (!g_Presenter)
	{
		g_pStopLP = new TStopLP;
		g_pMediator = new TMediator;
		g_pGlobalModel = new TGlobalModel;
		g_Presenter = new TPresenter(g_pMediator);
		g_Presenter->m_iShow = iShow;
		g_pTradeAction = new TradeAction;
	}
	
	g_Presenter->m_iShow = iShow;

	if (!CConvert::IsUserLogin()) //未登录
	{
		g_Presenter->m_bCanShowDetail = true;
		g_pMainFrm->Linkage(g_pStatusBar->GetHwnd(), "", "TradeLogin", NULL);
	}
	else //已经登录
	{
		g_Presenter->ShowDetail();
	}

	//Sleep(5);



	//if (g_Presenter->GetHwnd()==NULL)
	//{
	//	g_Presenter->Init();

	//	TMediator *pMediator = g_pMediator;
	//	TViewThreeKey* pViewThreeKey = new TViewThreeKey(pMediator);
	//	pMediator->SetColleaguePre(g_Presenter);
	//	pMediator->SetThreeKeyView(pViewThreeKey);
	//	pViewThreeKey->ShowFrm();

	//	HWND hParent = pViewThreeKey->GetHwnd();
	//	TConfigFrm *pConfigFrm = new TConfigFrm(pMediator, hParent);
	//	pMediator->SetConfigView(pConfigFrm);

	//	TOrderView *pOrderView = new TOrderView(pMediator, hParent);
	//	pMediator->SetOrderView(pOrderView);
	//	pOrderView->ShowFrm(-1, ID_frmTKeyOrder);

	//	TLogView *pLogView = new TLogView(pMediator, hParent);
	//	pLogView->SetViewTop(pOrderView->GetViewBottom());
	//	pMediator->SetLogView(pLogView);
	//	pLogView->ShowFrm(-1, ID_frmTKeyOrder);

	//	TTradeInfo *pTradeView = new TTradeInfo(pMediator, hParent);
	//	pTradeView->SetViewLeft(pOrderView->GetViewRight());
	//	pMediator->SetTradeView(pTradeView);
	//	pTradeView->ShowFrm(-1, ID_frmTKeyOrder);

	//	g_Presenter->m_hTKeyOrder = hParent;

	//	if (g_pGlobalModel->m_strUserNo.empty()) //登录之后，才打开三键下单，需要ResetUserNo
	//	{
	//		g_Presenter->OnResetUserNo(true);
	//	}

	//	TStopLPView* pStopLPView = new TStopLPView(pMediator);
	//	pMediator->SetStopLPView(pStopLPView);
	//	pStopLPView->Create(FindWindow(L"class TMainFrame", nullptr));
	//	ShowWindow(pStopLPView->GetHwnd(), SW_HIDE);
	//}
	//else
	//{
	//	g_Presenter->Send(CMD_Presenter_ShowHide, (const void*)iShow);
	//}
}

void TPresenter::ShowDetail()
{
	if (g_Presenter->GetHwnd() == NULL)
	{
		g_Presenter->Init();

		TMediator *pMediator = g_pMediator;
		TViewThreeKey* pViewThreeKey = new TViewThreeKey(pMediator);
		pMediator->SetColleaguePre(g_Presenter);
		pMediator->SetThreeKeyView(pViewThreeKey);
		pViewThreeKey->ShowFrm();

		HWND hParent = pViewThreeKey->GetHwnd();
		TConfigFrm *pConfigFrm = new TConfigFrm(pMediator, hParent);
		pMediator->SetConfigView(pConfigFrm);

		TOrderView *pOrderView = new TOrderView(pMediator, hParent);
		pMediator->SetOrderView(pOrderView);
		pOrderView->ShowFrm(-1, ID_frmTKeyOrder);

		TLogView *pLogView = new TLogView(pMediator, hParent);
		pLogView->SetViewTop(pOrderView->GetViewBottom());
		pMediator->SetLogView(pLogView);
		pLogView->ShowFrm(-1, ID_frmTKeyOrder);

		TTradeInfo *pTradeView = new TTradeInfo(pMediator, hParent);
		pTradeView->SetViewLeft(pOrderView->GetViewRight());
		pMediator->SetTradeView(pTradeView);
		pTradeView->ShowFrm(-1, ID_frmTKeyOrder);

		g_Presenter->m_hTKeyOrder = hParent;

		if (g_pGlobalModel->m_strUserNo.empty()) //登录之后，才打开三键下单，需要ResetUserNo
		{
			g_Presenter->OnResetUserNo(true);
		}

		TStopLPView* pStopLPView = new TStopLPView(pMediator);
		pMediator->SetStopLPView(pStopLPView);
		pStopLPView->Create(FindWindow(L"class TMainFrame", nullptr));
		ShowWindow(pStopLPView->GetHwnd(), SW_HIDE);
	}
	else
	{
		g_Presenter->Send(CMD_Presenter_ShowHide, (const void*)m_iShow);
	}
}