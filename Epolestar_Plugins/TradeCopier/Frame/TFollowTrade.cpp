#include "PreInclude.h"

TFollowTrade * g_pFollowTrader = NULL;


#define  fcfAppName  "FollowTrader"

#define WM_USERLOGIN  (WM_USER + 200)
#define WM_ONORDER    (WM_USER + 201)
#define WM_ONMATCH    (WM_USER + 202)
#define WM_ONPOSITION (WM_USER + 203)

#define frameWidth   ( 785+60)
#define frameHeight  500

#define captionHeight  28
#define  captionColor   RGB(76,112,177)

#define labelTop  (5+captionHeight)
#define labelWidth 100
#define labelHeight 23

#define lvTop     (labelTop + labelHeight + 5)
#define lvMainLeft    8
#define lvMainWidth 200

#define lvHeight  100
#define lvBottom  (lvTop+lvHeight)

#define lvFollowLeft (lvMainLeft+lvMainWidth+18)
#define lvFollowWidth   (551+60)  //545



#define timerID   111

#define configFrmWidth      270
#define configFrmHeight     260

#define frmTradeInfoTop    (lvBottom + 3)
#define frmTradeInfoHeight 200
#define frmTradeInfoWidth  (frameWidth -4)
#define frmTradeInfoBottom (frmTradeInfoTop + frmTradeInfoHeight)

#define labLogInfoTop (frmTradeInfoBottom + 3)
#define labLogInfoBottom (labLogInfoTop + labelHeight)

#define editLeft  (lvMainLeft +1)
#define editTop  (labLogInfoBottom + 5)
#define editWidth (frameWidth-130-editLeft)
#define editHeight (frameHeight-editTop -10)
#define editRight  (editLeft + editWidth)
#define editBottom (editTop + editHeight);


#define btnLeft   (editRight +10)
#define btnTop   editTop
#define btnWidth  112
#define btnHeight editHeight

#define  closeWidth        11

#define MainFrmName L"class TMainFrame"
TFollowTrade::TFollowTrade()
{
	m_cAddUser = addNone;
	m_rcClose.right = frameWidth - 8;
	m_rcClose.left = m_rcClose.right - closeWidth;
	m_rcClose.top = 8;
	m_rcClose.bottom = m_rcClose.top + closeWidth;
	m_bCloseClick = false;

	m_bHasStartFollow = false;

	m_iAllDisconnect = notUse;

	m_bUserLogin = false;
}
TFollowTrade::~TFollowTrade()
{

}
void TFollowTrade::ShowFrm()
{
	HWND hwnd = FindWindow(MainFrmName, NULL);
	CreateFrm(L"TFollowTrader", hwnd, WS_VISIBLE | WS_POPUP | WS_CLIPCHILDREN|WS_SYSMENU|WS_CLIPSIBLINGS , WS_EX_TOOLWINDOW | WS_EX_CONTROLPARENT);
	RECT rect = { 0 };
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);
	SetWindowPos(m_Hwnd, 0, rect.left+(rect.right-rect.left-frameWidth)/2, rect.top+(rect.bottom-rect.top-frameHeight)/2, frameWidth, frameHeight, SWP_SHOWWINDOW);

	g_pMainFrm->AddLinkage(m_Hwnd, 0, TFollowTrade::OnReceiveNotice);
}
void TFollowTrade::OnCreate()
{
	m_labelMain.Create(m_Hwnd, L"主账号：", id_labelMain);
	m_labelMain.SetFont(g_FontData.g_FontWord19);
	m_labelMain.MoveWindow(lvMainLeft, labelTop, labelWidth, labelHeight);

	m_lvMainAccount.CreateList(m_Hwnd, LVS_REPORT | LVS_NOSORTHEADER | LVS_OWNERDRAWFIXED | WS_BORDER, 20, 18, LVS_EX_FULLROWSELECT, id_lvMainAccount);
	m_lvMainAccount.MoveWindow(lvMainLeft, lvTop, lvMainWidth, lvHeight);

	m_labelFollow.Create(m_Hwnd, L"跟随账号：", id_labelFollow);
	m_labelFollow.SetFont(g_FontData.g_FontWord19);
	m_labelFollow.MoveWindow(lvFollowLeft, labelTop, labelWidth, labelHeight);


	m_lvFoloowAccount.CreateList(m_Hwnd, LVS_REPORT | LVS_NOSORTHEADER | LVS_OWNERDRAWFIXED | WS_BORDER, 20, 18, LVS_EX_FULLROWSELECT, id_lvFollowAccount);
	m_lvFoloowAccount.MoveWindow(lvFollowLeft, lvTop, lvFollowWidth, lvHeight);

	m_frmTradeInfo.Create(m_Hwnd);
	m_frmTradeInfo.MoveWindow(2, frmTradeInfoTop, frmTradeInfoWidth,  frmTradeInfoHeight);
	m_frmTradeInfo.SetUser(&m_mainUser, &m_followUser);
	m_iTradeInfoHeight = frmTradeInfoHeight;
	//m_labOrderState.Create(m_Hwnd, L"委托信息：");
	//m_labOrderState.SetFont(g_FontData.g_FontWord19);
	//m_labOrderState.MoveWindow(lvMainLeft, labOrderStateTop, labelWidth, labelHeight);

	m_labLogInfo.Create(m_Hwnd, L"日志信息：");
	m_labLogInfo.SetFont(g_FontData.g_FontWord19);
	m_labLogInfo.MoveWindow(lvMainLeft, labLogInfoTop, labelWidth, labelHeight);

	m_editLog.Create(m_Hwnd);
	m_editLog.MoveWindow(editLeft, editTop, editWidth, editHeight);
	m_iEditHeight = editHeight;
	m_iEditTop = editTop;
	m_editLog.SetFont(g_FontData.g_FontWord12);
	RECT rect = { btnLeft, btnTop, btnLeft + btnWidth, btnTop + btnHeight };
	m_btnStart.CreateButton(m_Hwnd,  "跟单未启动", id_btnStart);
	m_btnStart.MoveWindow(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
	Init();
	

	m_iLogFileID = g_pLogging->CreateLogging(L"TradeCopier", false, true,false);

	AddLog(welcomeUse, NULL, NULL);
}
void TFollowTrade::Init()
{
	m_lvMainAccount.AddColumn(L"账号", 110);
	m_lvMainAccount.AddColumn(L"登录状态", 88); 

	m_lvFoloowAccount.AddColumn(L"账号", 85);
	m_lvFoloowAccount.AddColumn(L"状态", 70);
	m_lvFoloowAccount.AddColumn(L"跟单方向", 75);
	m_lvFoloowAccount.AddColumn(L"跟单价", 80);
	m_lvFoloowAccount.AddColumn(L"数量比率", 75);
	m_lvFoloowAccount.AddColumn(L"追平仓时间", 80);
	m_lvFoloowAccount.AddColumn(L"追单次数", 80);
	m_lvFoloowAccount.AddColumn(L"持仓检测", 60);


	m_hFollowMenu = CreatePopupMenu();
	AppendMenu(m_hFollowMenu, MF_STRING, idm_loginFollow, L"登录账户");
	AppendMenu(m_hFollowMenu, MF_STRING, idm_logoutFollow, L"登出账户");
	AppendMenu(m_hFollowMenu, MF_SEPARATOR, 0, 0);
	AppendMenu(m_hFollowMenu, MF_STRING, idm_setFollow, L"设置");

	m_hMainMenu = CreatePopupMenu();
	AppendMenu(m_hMainMenu, MF_STRING, idm_loginMain, L"登录账户");
	AppendMenu(m_hMainMenu, MF_STRING, idm_logoutMain, L"登出账户");

}
void TFollowTrade::ReadConfigData(TFollowUser * pData)
{
	char keyName[2] = { 0 };
	keyName[0] = '0';
	char szValue[200] = { 0 };
	char szFilePath[100] = { 0 };
	GetModulePath(szFilePath, sizeof(szFilePath), FollowConfigFilePath);

	if (GetPrivateProfileStringA(pData->UserNo, keyName, "", szValue, sizeof(szValue), szFilePath))
	{
		vector<string>vecContent;
		CConvert::SplitString(szValue, ',', vecContent);
		if (vecContent.size() == 7)
		{
			pData->FollowPriceType = atoi(vecContent[0].c_str()); 
			pData->DirectSameToMain = atoi(vecContent[1].c_str()); 
			pData->FollowCoverTime = atoi(vecContent[2].c_str());
			pData->ChaseCount = atoi(vecContent[3].c_str());
			pData->CheckPositionTime = atoi(vecContent[4].c_str());
			pData->bUseCheckPosition = (bool)atoi(vecContent[5].c_str());
			pData->QtyRate = atof(vecContent[6].c_str());
		}
	}

}
void TFollowTrade::UpdateConfig(int i)
{
	int iBeginCol = 2;

	char szText[50] = "同向";

	if (!m_followUser[i].DirectSameToMain)
		strcpy_s(szText, "反向");
	m_lvFoloowAccount.AddItem(szText, i, iBeginCol++);

	strcpy_s(szText, "同主账号");
	if (m_followUser[i].FollowPriceType == fptConsideration)
		strcpy_s(szText, "对手价");
	else if (m_followUser[i].FollowPriceType == fptQueuePrice)
		strcpy_s(szText, "即时买卖价(排队价)");
	else if (m_followUser[i].FollowPriceType == fptMarket)
		strcpy_s(szText, "涨跌停价(市价)");
	m_lvFoloowAccount.AddItem(szText, i, iBeginCol++);

	sprintf_s(szText, "%.3f", m_followUser[i].QtyRate);
	m_lvFoloowAccount.AddItem(szText, i, iBeginCol++);

	sprintf_s(szText, "%d s", m_followUser[i].FollowCoverTime);
	m_lvFoloowAccount.AddItem(szText, i, iBeginCol++);

	sprintf_s(szText, "%d", m_followUser[i].ChaseCount);
	m_lvFoloowAccount.AddItem(szText, i, iBeginCol++);

	if (m_followUser[i].bUseCheckPosition)
		sprintf_s(szText, "%d s", m_followUser[i].CheckPositionTime);
	else
		strcpy_s(szText, "不检测");
	m_lvFoloowAccount.AddItem(szText, i, iBeginCol++);
}
void TFollowTrade::OnMenuClick(const unsigned int MenuIndex, const HWND PastLife)
{
	TFollowTrade::OnHotKeyDown(false, false, false, VK_F3, false, NULL);
}
void TFollowTrade::OnHotKeyDown(const bool Alt, const bool Ctrl, const bool Shift, const unsigned char VKey, const bool Exclusive, HWND source)
{
	TCHAR name[50] = { 0 };
	GetClassName(source, name, sizeof(name) / sizeof(TCHAR));
	if (lstrcmp(name, WC_EDIT) == 0)
		return;

	//权限认证

	//static bool bRet = false;
	//if (!bRet)
	//	bRet = TFollowTrade::IsAuthent();
	//if (!bRet) return;

	//end


	if (g_pFollowTrader == NULL)
	{
		g_pFollowTrader = new TFollowTrade;
		g_pFollowTrader->ShowFrm();
		return;
	}
	if (IsWindowVisible(g_pFollowTrader->GetHwnd()))
		ShowWindow(g_pFollowTrader->GetHwnd(), SW_HIDE);
	else
		ShowWindow(g_pFollowTrader->GetHwnd(), SW_SHOW);
}
LRESULT TFollowTrade::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		OnCreate();
		return PROCESSED;
	case WM_PAINT:
		OnPaint();
		return PROCESSED;
	case WM_ONORDER:
		OnOrder(wParam, lParam);
		return PROCESSED;
	case WM_TIMER:
		OnTimer(wParam, lParam);
		return PROCESSED;
	case WM_DRAWITEM:
		OnDrawItem(wParam, lParam);
		return PROCESSED;
	case WM_COMMAND:
		OnCommand(wParam, lParam);
		return PROCESSED;
	case WM_NOTIFY:
		OnNotify(wParam, lParam);
		return PROCESSED;
	case WM_CTLCOLORSTATIC:
		return OnCtlColor(wParam, lParam);
	case SSWM_STATIC_BUTTON_CLICKDOWN:
		OnClickBtn(lParam);
		return PROCESSED;
	case WM_LBUTTONDOWN:
		OnLButtonDown(wParam, lParam);
		return PROCESSED;
	case WM_LBUTTONUP:
		OnLButtonUp(wParam, lParam);
		return PROCESSED;
	case WM_USERLOGIN:
		OnUserLogin(lParam);
		return PROCESSED;
	case WM_MEASUREITEM:
		OnMeasureItem(wParam, lParam);
		return PROCESSED;
	case WM_DESTROY:
		OnDestroy();
		return PROCESSED;
	case WM_NCDESTROY:
		delete this;
		return PROCESSED;
	case WM_NCHITTEST: //鼠标的所有消息都有这个产生
		return OnNcHitTest(wParam, lParam); //OnNcHiTest处理鼠标所在区域，返回相应消息
	case WM_SETCURSOR:
		OnSetCursor(wParam, lParam); //设置鼠标形态，根据鼠标所在边框
		return PROCESSED;
	case WM_NCLBUTTONDOWN:          //发送消息，实现窗口的缩放，消息由DefWindowProc处理
		OnNcLButtonDown(wParam, lParam);
		return NOT_PROCESSED;
	case WM_SIZE:
		OnSize(HIWORD(lParam));
		return PROCESSED;
	case WM_SIZING:
		OnSizing(wParam, lParam);
		return PROCESSED;
	case WM_ONPOSITION:
		m_frmTradeInfo.OnPosition();
		return PROCESSED;
	default:
		return NOT_PROCESSED;
	}
}
void TFollowTrade::OnDestroy()
{
	char keyName[2] = { 0 };
	keyName[0] = '0';
	char szText[200] = { 0 };
	char szFilePath[100] = { 0 };

	GetModulePath(szFilePath, sizeof(szFilePath), FollowConfigFilePath);
	int iCount = m_followUser.size();
	for (int i = 0; i < iCount; i++)
	{
		sprintf_s(szText, "%d,%d,%d,%d,%d,%d,%.4lf", (int)m_followUser[i].FollowPriceType, (int)m_followUser[i].DirectSameToMain,
			(int)m_followUser[i].FollowCoverTime,m_followUser[i].ChaseCount,
			m_followUser[i].CheckPositionTime,(int)m_followUser[i].bUseCheckPosition, m_followUser[i].QtyRate);
		WritePrivateProfileStringA(m_followUser[i].UserNo, keyName, szText, szFilePath);
	}
}

void TFollowTrade::OnTimer(WPARAM wParam, LPARAM lParam)
{
	m_tradeModel.IncrementTime();

	if (wParam == timerID && m_bHasStartFollow)
	{
		TDataContainer parOrders;
		g_pTradeData->GetParOrder(parOrders);//首先追平仓单

		for (int i = 0; i < m_followUser.size();i++)
		{
			m_followUser[i].iTimerCount++;
			if (parOrders.size() == 0) continue;
			if (m_followUser[i].FollowCoverTime != 0 && m_followUser[i].iTimerCount%m_followUser[i].FollowCoverTime == 0)
			{
				FollowCover(parOrders, m_followUser[i]);//追平仓单
			}
		}
		CheckPositionBalance();//检测持仓平衡

		static int iCount = 0;
		iCount++;
		if (iCount % 3 == 0)//3s 清理一次
		{
			DeleteOrderKey();
		}
		////Just For test
		//TCHAR szText[20] = { 0 };
		//wsprintf(szText, L"%d\n", m_vOrderKey.size());
		//OutputDebugString(szText);
	}
	AfterUserLogin();
}
void TFollowTrade::AfterUserLogin()
{
	if (m_bUserLogin)
	{
		static int iCount = 0;
		iCount++;
		if (iCount == 10)//用户登录成功后10s重置此变量
		{
			m_cAddUser = addNone;
			iCount = 0;
			m_bUserLogin = false;
		}
	}
	if (m_iAllDisconnect == userReConnect)
	{
		static int iCount = 0;
		iCount++;
		if (iCount == 30)
		{
			m_iAllDisconnect = notUse;
			iCount = 0;
		}
	}
}
void TFollowTrade::CheckPositionBalance()
{
	vector<const TPosition*>mainPosition;
	vector<const TPosition*>followPosition;
	bool bGetData = false;
	for (int i = 0; i < m_followUser.size(); i++)//循环检测每个跟随账号的持仓
	{
		m_followUser[i].iCheckPositionCount++;
		if (m_followUser[i].bUseCheckPosition && //必须要启用且检测时间不为0
			(m_followUser[i].CheckPositionTime != 0 && m_followUser[i].iCheckPositionCount % m_followUser[i].CheckPositionTime == 0))
		{
			if (!bGetData)
			{
				GetData(mainPosition, followPosition);//获取挂单、总持仓，将持仓分为主账号和跟随账号两部分
				bGetData = true;
			//	g_pTradeApi->QryExchangeState(m_followUser[i].UserNo, m_followUser[i].Sign);
			}
			//新增了挂单量检测，如果单子已经发出还未成交，那么要判断这个量
			CheckFollowUserPos(m_followUser[i], mainPosition, followPosition); //查看是否平衡，如果不平衡则进行开平操作
			DealFollowMore(followPosition, m_followUser[i]);//处理主账号没有而跟随账号有的持仓
		}
	}
}

//处理跟随账号多的持仓
//要判断主账号、跟随账号的挂单量
//如果主账号开仓，跟随账号先成交了，那么此时不能追，要看看主账号是否有挂单还没成交
void TFollowTrade::DealFollowMore(vector<const TPosition*>&followPos, TFollowUser &stUser)
{
	for (int i = 0; i < followPos.size();i++)//处理跟随账号有而主账号没有的，那么要确认主账号是否是真的没有
	{
		
		if (!m_tradeModel.CanTrade(followPos[i]))continue; //当前交易所合约不能交易

		if (strcmp(stUser.UserNo, followPos[i]->UserNo) == 0) //找到该跟随账号有而主账号没有的持仓，再次确认
		{
			const TPosition* pMainPosition = nullptr;
			TDirect direct = followPos[i]->Direct;  //默认主账号和跟随账号同方向
			if (!stUser.DirectSameToMain)
				direct = ReverseDirect(direct);     //跟随账号和主账号方向不一致，则方向还原出主账号的方向

			TContractSingleKey singleKey;
			memcpy(&singleKey, followPos[i], sizeof(TContractSingleKey));
			for (auto iter = m_mainUser.begin(); iter != m_mainUser.end(); iter++) //在这里根据跟随账号的持仓查找主账号的持仓，要注意Sign
			{
				//如果找到主账号有持仓则直接返回就ok，如果主账号是真的没有那么要进行相应的处理
				strcpy_s(singleKey.Sign, iter->Sign);
				pMainPosition = g_pTradeData->GetPositionTotal(iter->UserNo, &singleKey,direct);
				if (pMainPosition)break;
			}
			if (!pMainPosition|| pMainPosition->PositionQty==0)
			{
			//	int iMainParOrderQty = GetMainParOrderQty(followPos[i], stUser);
				int diffQty = followPos[i]->PositionQty;// - floor( iMainParOrderQty * stUser.QtyRate); //减去挂单量
				if (diffQty > 0)
				{
					DealFollowPos(followPos[i], diffQty);
				}
			}
		}
	}
}


bool TFollowTrade::IsMainUserOrder(const TOrder* pOrder)
{
	auto iter = find(m_mainUser.begin(), m_mainUser.end(), pOrder->UserNo);

	if (iter != m_mainUser.end())
	{
		if (strcmp(pOrder->Sign, iter->Sign) == 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	return false;

}

int TFollowTrade::GetMainParOrderQty(const TPosition* pFollowPos, TFollowUser & stFollowUser) //利用跟随账号的持仓来查找主账号的挂单
{
	TDataContainer parOrders;

	g_pTradeData->GetParOrder(parOrders);

	int iCount = parOrders.size();

	TDirect direct = pFollowPos->Direct;

	if (!stFollowUser.DirectSameToMain) //如果方向不一致，还原出主账号的挂单方向
	{
		direct = ReverseDirect(direct);
	}

	for (int i = 0; i < iCount; i++)
	{
		const TOrder* pOrder = (const TOrder*)parOrders.at(i);

		if (pOrder->Offset == oOpen &&
			pOrder->Direct == direct &&
			IsMainUserOrder(pOrder) &&
			IsSameContract(pOrder,pFollowPos)
			)
		{
			return (pOrder->OrderQty - pOrder->MatchQty);
		}

	}
	return 0;
}

bool TFollowTrade::IsSameContract(const TOrder* pOrder, const TPosition* pPosition)
{
	if (pOrder->CommodityType == pPosition->CommodityType &&
		pOrder->OptionType == pPosition->OptionType &&
		!strcmp(pOrder->ExchangeNo, pPosition->ExchangeNo) &&
		!strcmp(pOrder->CommodityNo, pPosition->CommodityNo) &&
		!strcmp(pOrder->ContractNo, pPosition->ContractNo) &&
		!strcmp(pOrder->StrikePrice, pPosition->StrikePrice)
		)
	{
		return true; 
	}
	return false;
}


bool TFollowTrade::CanTrade(const TPosition* pPosition)
{
	char key[100] = { 0 };
	strcat_s(key, pPosition->ExchangeNo);
	strcat_s(key, "+");
	strcat_s(key, pPosition->CommodityNo);
	auto iter = m_mapExchangeState.find(key);
	if (iter != m_mapExchangeState.end())
		return iter->second == tsTradeing;
	return false;
}

//根据主账号的持仓来处理 跟随账号的 持仓
//要考虑跟随账号的挂单量
void TFollowTrade::CheckFollowUserPos(TFollowUser& stUser, vector<const TPosition*>&mainPos, vector<const TPosition*>&followPos)
{
	for (uint i = 0; i < mainPos.size(); i++) //根据主账号持仓来进行比较
	{

		if (!m_tradeModel.CanTrade(mainPos[i]))continue; //当前交易所合约 不能交易则调到下一个循环

		TDirect direct = mainPos[i]->Direct;
		if (!stUser.DirectSameToMain)
			direct = ReverseDirect(direct);

		TContractSingleKey singleKey;
		memcpy(&singleKey, mainPos[i], sizeof(TContractSingleKey));
		strcpy_s(singleKey.Sign, stUser.Sign);

		const TPosition* pPosition = g_pTradeData->GetPositionTotal(stUser.UserNo, &singleKey, direct); //查看该跟随用户是否有和主账号对应的持仓

		auto iterFollowPos = find(followPos.begin(), followPos.end(), pPosition);
		if (iterFollowPos != followPos.end())
			followPos.erase(iterFollowPos); //这样剩下的就是跟随账号有而主账号没有的，假设是pPosition和followPos中的值是一样的

		int diffQty = 0;
		if (pPosition)//如果找到持仓
		{
			diffQty = pPosition->PositionQty - floor(mainPos[i]->PositionQty*stUser.QtyRate);//因为开仓是向下取整，所以这时检测也要向下取整
			if (diffQty != 0)
			{
				DealFollowPos(pPosition, diffQty);//跟随者开仓应用对手价，平仓采用市价
			}
		}
		else//没有找到持仓，说明主账号有，而跟随账号没有那么要补上去,开仓
		{
			diffQty = floor(mainPos[i]->PositionQty*stUser.QtyRate); //这里要判断是否已经发出了挂单，如果不判断的话，会导致一直发单，这样是错误的
			//检查开仓挂单
			//diffQty = floor(mainPos[i]->PositionQty*stUser.QtyRate) - iParOrderQty; //减去已经发出还没成交的委托数量
			if (diffQty != 0)
			{
				DealFollowPos(mainPos[i], -diffQty, &stUser);//借用主账号的持仓，userno,sign要还原
			}
		}
	}
}
void TFollowTrade::DealFollowPos(const TPosition*pPosition, int diffQty, TFollowUser*pFollowUser)
{
	TSendOrder stOrder;
	memset(&stOrder, 0, sizeof(TSendOrder));
	CConvert::DefaultDealClose(pPosition, stOrder);

	TDirect direct = pPosition->Direct;
	if (pFollowUser)
	{
		strcpy_s(stOrder.UserNo, pFollowUser->UserNo);
		strcpy_s(stOrder.Sign, pFollowUser->Sign);
		if (!pFollowUser->DirectSameToMain)
			direct = ReverseDirect(direct);
	}

	char szLog[200] = { 0 };
	char szDriect[20] = "买入";
	char szLongShort[20] = "多头";
	if (diffQty > 0)
	{
		stOrder.Direct = ReverseDirect(direct);
		stOrder.Offset = CConvert::GetOffset(stOrder.UserNo, &stOrder, stOrder.Direct);
		if (stOrder.Offset == oOpen)return;

		if (stOrder.Direct == dSell)
			strcpy_s(szDriect, "卖出");
		else
			strcpy_s(szLongShort, "空头");
		sprintf_s(szLog, "跟随账号%s %s%s %s 持仓，比主账号多%d手，进行%s平仓操作", stOrder.UserNo, stOrder.CommodityNo, stOrder.ContractNo,
			szLongShort, diffQty, szDriect);
		CConvert::GetPrice(&stOrder, stOrder.Direct, ptMarketPrice, stOrder.OrderPrice);

		//stOrder.OrderQty = abs(GetRealDiffQty(stOrder, diffQty));
		stOrder.OrderQty = abs(diffQty);

		vector<uint> orderID = CConvert::CoverPos(pPosition, stOrder, stOrder.OrderQty);
		
		for (int i = 0; i < orderID.size(); i++)
		{
			m_coverOrderNow.push_back(orderID[i]);
		}
	}
	else if (diffQty < 0)
	{
		stOrder.Direct = direct;
		stOrder.Offset = oOpen;
		if (stOrder.Direct == dSell)
		{
			strcpy_s(szDriect, "卖出");
			strcpy_s(szLongShort, "空头");
		}
		sprintf_s(szLog, "跟随账号%s %s%s %s 持仓，比主账号少%d手，进行%s开仓操作", stOrder.UserNo, stOrder.CommodityNo, stOrder.ContractNo,
			szLongShort, abs(diffQty), szDriect);
		CConvert::GetPrice(&stOrder, stOrder.Direct, ptCounterPrice, stOrder.OrderPrice);

	//	stOrder.OrderQty = abs(GetRealDiffQty(stOrder, diffQty));
		stOrder.OrderQty = abs(diffQty);

		if (stOrder.OrderQty == 0)return;
		//uint orderId = g_pTradeApi->InsertOrder(stOrder);
		//m_coverOrderNow.push_back(orderId);
		g_pTradeApi->InsertOrder(stOrder);
	}
	else return;


	AddLog(posNotBalance, szLog, NULL);
}

int TFollowTrade::GetParOrderQty(TSendOrder & stOrder)
{
	int iParOrderQty = 0;

	TDataContainer parOrders;
	g_pTradeData->GetParOrder(parOrders, ftValid, stOrder.Sign);

	int iCount = parOrders.size();

	for (int i = 0; i < parOrders.size(); i++)
	{
		const TOrder* pOrder = (const TOrder*)parOrders.at(i);

		if (pOrder->Offset == stOrder.Offset &&
			pOrder->Direct == stOrder.Direct &&
			strcmp(pOrder->UserNo, stOrder.UserNo) == 0 &&
			memcmp(pOrder, &stOrder, sizeof(TContractSingleKey))
			) //是跟随者发出的挂单
		{
			iParOrderQty = pOrder->OrderQty - pOrder->MatchQty;
			break;
		}
	}
	return iParOrderQty;
}

int TFollowTrade::GetRealDiffQty(TSendOrder &stOrder, int diffQty)
{
	int iParOrderQty = GetParOrderQty(stOrder);

	if (diffQty < 0) //多余的要平仓
	{
		diffQty += iParOrderQty;
		
		if(diffQty>=0)
		{
			return 0;//如果加上挂单量之后大于0了，说明不用平仓了
		}
	}
	else  //>0要开仓
	{
		diffQty -= iParOrderQty;

		if (diffQty <= 0)
		{
			return 0; //如果减去开仓量之后小于0，那么就不用开仓了
		}
	}
	return diffQty;
}


void TFollowTrade::GetData(vector<const TPosition*> & mainPos, vector<const TPosition*> & followPos)
{
	TDataContainer allPosSub;
	g_pTradeData->GetAllPositionTotal(allPosSub);

	for (uint i = 0; i < allPosSub.size(); i++)
	{
		const TPosition* pPosition = (const TPosition*)allPosSub.at(i);
		if (find(m_mainUser.begin(), m_mainUser.end(), pPosition->UserNo) != m_mainUser.end())
		{
			mainPos.push_back(pPosition);
		}
		else if (find(m_followUser.begin(), m_followUser.end(), pPosition->UserNo) != m_followUser.end())
		{
			followPos.push_back(pPosition);
		}
	}
}
void TFollowTrade::FollowCover(TDataContainer &data, TFollowUser & stFollowUser)
{
	TModifyOrder  stModiftOrder;
	memset(&stModiftOrder, 0, sizeof(TModifyOrder));

	vector<TChaseInfo>::iterator iterChase;
	for (UINT i = 0; i < data.size(); i++)
	{
		const TOrder* pOrder = (const TOrder*)data.at(i);
		const SCommodity *pCommodity = g_pTradeData->GetCommodity(pOrder);
		if (!pCommodity)
			continue;

		if (strcmp(pOrder->UserNo, stFollowUser.UserNo) == 0 && (pOrder->Offset == oCover || pOrder->Offset == oCoverT))//平仓
		{
			iterChase = find(m_coverOrderNow.begin(), m_coverOrderNow.end(), pOrder->OrderID);
			if (iterChase != m_coverOrderNow.end())
			{
				memcpy(&stModiftOrder, pOrder, sizeof(TModifyOrder));
				if (pOrder->Direct == dSell) //卖的话，要便宜一点才更容易卖出去
					stModiftOrder.OrderPrice -= pCommodity->PriceTick;
				else if (pOrder->Direct == dBuy)
					stModiftOrder.OrderPrice += pCommodity->PriceTick;
				iterChase->OrderID = g_pTradeApi->ModifyOrder(stModiftOrder);
				iterChase->ChaseCount++;

				if (iterChase->ChaseCount > stFollowUser.ChaseCount)
				{
					m_coverOrderNow.erase(iterChase);
					AddLog(chaseEnd, NULL, pOrder);
				}
				else
				{
					AddLog(chaseCoverAction, stFollowUser.UserNo, pOrder);
					data.erase(i);
				}
				break;
			}
		}

	}
}
void TFollowTrade::OnClickBtn(LPARAM lParam)
{
	if (lParam == id_btnStart)
	{
		if (m_mainUser.size() == 0 || m_followUser.size()==0)
		{
			_TMessageBox_Domodal(m_Hwnd, "提示", L"账号未登录, 请先登录！", TMB_OK);
			return;
		}
		m_bHasStartFollow = !m_bHasStartFollow;
		if (m_bHasStartFollow)
		{
			m_btnStart.SetButtonText("跟单已启动");
			AddLog(startFollow,NULL, NULL);
			SetTimer(m_Hwnd, timerID, 1000, 0);
			m_tradeModel.SetTime();
		}
		else
		{
			m_btnStart.SetButtonText("跟单已暂停");
			m_coverOrderNow.clear();
			m_coverOrderWait.clear();
			AddLog(stopFollow, NULL, NULL);
			KillTimer(m_Hwnd, timerID);
		}
	}
}
void TFollowTrade::GetTime(char *pszText, int legnth)
{
	SYSTEMTIME st = { 0 };
	GetLocalTime(&st);
	sprintf_s(pszText, legnth, "<%02d:%02d:%02d> ", st.wHour, st.wMinute, st.wSecond);
}
void TFollowTrade::AddLog(UINT Action, const char * pszText, const TOrder* pOrder)
{
	char szTime[20] = { 0 };
	GetTime(szTime, sizeof(szTime));
	char szLog[150] = { 0 };
	COLORREF logColor = RGB(0, 0, 0);
	char szDirect[20] = "";
	if (pOrder)
	{
		if(pOrder->Direct == dSell)
			strcpy_s(szDirect, "卖出");
		else
			strcpy_s(szDirect, "买入");
	}
	switch (Action)
	{
	case openAction:
		sprintf_s(szLog, "%s 主账号%s%s开仓,%s%s, %d手, 跟随账号%s跟开仓.", szTime, pOrder->UserNo,szDirect,
			pOrder->CommodityNo, pOrder->ContractNo, pOrder->OrderQty, pszText);
		logColor = RGB(204, 0, 0);
		break;
	case coverAction:
		sprintf_s(szLog, "%s 主账号%s%s平仓,%s%s, %d手, 跟随账号%s跟平仓.", szTime,pOrder->UserNo,szDirect,
			pOrder->CommodityNo, pOrder->ContractNo, pOrder->OrderQty, pszText);
		logColor = RGB(10, 150, 10);
		break;
	case cancelAction:
		sprintf_s(szLog, "%s 主账号%s撤单,%s%s, %d手, 跟随账号%s启动撤单.", szTime, pOrder->UserNo,
			pOrder->CommodityNo, pOrder->ContractNo, pOrder->OrderQty, pszText);
		logColor = RGB(0, 102, 102);
		break;
	case chaseCoverAction:
		sprintf_s(szLog, "%s %s%s, 跟随账号%s追平仓单.", szTime,
			pOrder->CommodityNo, pOrder->ContractNo, pszText);
		logColor = RGB(0, 153, 153);
		break;
	case cancelCover:
		sprintf_s(szLog, "%s 主账号撤单, %s%s, 跟随账号%s平仓.", szTime,
			pOrder->CommodityNo, pOrder->ContractNo, pszText);
		logColor = RGB(102, 102, 102);
		break;
	case stopFollow:
		sprintf_s(szLog, "%s 停止跟单", szTime);
		logColor = RGB(204, 0, 0);
		break;
	case startFollow:
		sprintf_s(szLog, "%s 启动跟单", szTime);
		logColor = RGB(10, 150, 10);
		break;
	case welcomeUse:
		strcpy_s(szLog, "欢迎使用跟单助手");
		logColor = RGB(0, 153, 153);
		break;
	case chaseEnd:
		sprintf_s(szLog, "%s 账号%s追%s%s次数超过上限，追单停止.", szTime,
			pOrder->UserNo,pOrder->CommodityNo, pOrder->ContractNo);
		logColor = RGB(204, 0, 0);
		break;
	case followUserLogout:
		sprintf_s(szLog, "%s %s", szTime, pszText);
		logColor = RGB(204, 0, 0);
		break;
	case mainUserLogout:
		sprintf_s(szLog, "%s %s", szTime, pszText);
		logColor = RGB(204, 0, 0);
		break;
	case coverByUser:
		sprintf_s(szLog, "%s %s", szTime, pszText);
		logColor = RGB(51, 51, 102);
		break;
	case posNotBalance:
		sprintf_s(szLog, "%s %s", szTime, pszText);
		logColor = RGB(51, 51, 102);
		break;
	case mainUserRelogin:
		sprintf_s(szLog, "%s 主账号%s重新连接", szTime, pszText);
		logColor = RGB(10, 150, 10);
		break;
	case followUserRelogin:
		sprintf_s(szLog, "%s 跟随账号%s重新连接", szTime, pszText);
		logColor = RGB(10, 150, 10);
		break;
	case logMainUserLogin:
		sprintf_s(szLog, "%s 主账号%s登录", szTime, pszText);
		logColor = RGB(204, 0, 0);
		break;
	case logFollowUserLogin:
		sprintf_s(szLog, "%s 跟随账号%s登录", szTime, pszText);
		logColor = RGB(204, 0, 0);
		break;
	case logMainUserLogout:
		sprintf_s(szLog, "%s 主账号%s登出", szTime, pszText);
		logColor = RGB(51, 51, 102);
		break;
	case logFollowUserLogout:
		sprintf_s(szLog, "%s 跟随账号%s登出", szTime, pszText);
		logColor = RGB(51, 51, 102);
		break;

	}	
	if (strlen(szLog))
	{
		m_editLog.AddString(szLog, logColor);
		int length = strlen(szLog);
		if (length+2<sizeof(szLog))
		{
			szLog[length] = '\r';
			szLog[length+1] = '\n';
			szLog[length + 2] = 0;
		}
		g_pLogging->OutputLog(m_iLogFileID, ltString, llInfo, szLog, length + 2);
	}
}
void TFollowTrade::OnOrder(WPARAM wParam, LPARAM lParam)
{
	if (!lParam || !m_bHasStartFollow) //如果lParam为空或者没有启动跟单，就直接返回
		return;
	TOrder *pOrder = (TOrder*)lParam;
	m_frmTradeInfo.PushBackOrder(pOrder);
	if (strcmp(pOrder->Remark, RemarkCoverByUser) == 0)return;  //如果是用户主动平的仓，那么是不用跟的，所以直接返回
	if (pOrder->OrderState == osFail ||pOrder->OrderState==osInvalid||pOrder->OrderState==osPartFailed)//如果是无效单，则直接返回
		return;
	TUser stUser = { 0 };
	strcpy_s(stUser.UserNo, pOrder->UserNo);
	if (find(m_mainUser.begin(), m_mainUser.end(), stUser) != m_mainUser.end())//如果回调是来自主账号
	{   	
		OnMainOrder(pOrder);//可能是开仓，平仓，撤单，立即跟的
		FollowMain(pOrder);//处理主账号成交、撤单、失败等的状态
	}
	else if (find(m_followUser.begin(), m_followUser.end(), stUser) != m_followUser.end())//跟单账号的单
	{	
		OnFollowOrder(pOrder);
	}	
}
void TFollowTrade::FollowMain(const TOrder* pOrder)
{
	if (pOrder->OrderState == osFilled ||pOrder->OrderState==osPartFilled)
	{
		DealMainFilled(pOrder, true);
	}
	else if (pOrder->OrderState == osCanceling || pOrder->OrderState == osCanceled || pOrder->OrderState == osFail || pOrder->OrderState == osInvalid)
	{
		DealMainFilled(pOrder, false);
	}

}
//主账号成交
//主账号平仓时，follow账号应撤的单，应平的仓
//follow要等主账号成交后才能采取动作
void TFollowTrade::DealMainFilled(const TOrder* pOrder, bool bDoAction)
{
	vector<TParOrderKey>::iterator iterCover;
	vector<TCancelOrderKey>::iterator iterCancel;
	uint deleteFlag = -1;

	for (iterCancel = m_followCancel.begin(); iterCancel != m_followCancel.end();)
	{
		if (pOrder == iterCancel->pMainOrder)
		{
		//	if (bDoAction)//主账号成功，则撤单否则不用采取行动
		//	{
				g_pTradeApi->CancelOrder(iterCancel->FollowOrderID);
				const TOrder* pParOrder = g_pTradeData->GetOrder(iterCancel->FollowOrderID);
				if (pParOrder)
					AddLog(cancelAction, pParOrder->UserNo, pOrder);
		//	}
			iterCancel = m_followCancel.erase(iterCancel);
		}
		else
		{
			iterCancel++;
		}
	}
	for (iterCover = m_coverOrderWait.begin(); iterCover != m_coverOrderWait.end();)
	{
		if (pOrder == iterCover->pMainOrder)
		{
			const TOrder* pParOrder = g_pTradeData->GetOrder(iterCover->FollowOrderID);
			if (bDoAction)//主账号成功，则立即追平仓，把应平的仓加入追平仓容器中
			{
				m_coverOrderNow.push_back(iterCover->FollowOrderID);
				//if (pParOrder)
				//	AddLog(coverAction, pOrder, pParOrder->UserNo);
			}
			else
			{
				g_pTradeApi->CancelOrder(iterCover->FollowOrderID);
				AddLog(cancelAction, pParOrder->UserNo, pOrder);
			}
			iterCover = m_coverOrderWait.erase(iterCover);
		}
		else
		{
			iterCover++;
		}
	}
}
void TFollowTrade::OnFollowOrder(TOrder *pOrder)
{
	for (UINT i = 0; i < m_vOrderKey.size(); i++)
	{
		if (pOrder->OrderID == m_vOrderKey[i].followOrderID) //要进行测试
		{
			m_vOrderKey[i].followOrderState = pOrder->OrderState;
			m_vOrderKey[i].pFollowOrder = pOrder;
			break;
		}
	}

	if (pOrder->OrderState == osFilled || pOrder->OrderState == osPartFilled || pOrder->OrderState == osFail)
	{
		vector<TChaseInfo>::iterator iterChase;
		iterChase = find(m_coverOrderNow.begin(), m_coverOrderNow.end(), pOrder->OrderID);
		if (iterChase != m_coverOrderNow.end() &&iterChase->OrderID==pOrder->OrderID)
		{
			m_coverOrderNow.erase(iterChase);
		}
	}
}
void TFollowTrade::DeleteOrderKey()
{
	for (auto iter =m_vOrderKey.begin(); iter != m_vOrderKey.end();)
	{
		const TOrder* pMainOrder = iter->pMainOrder;
		const TOrder* pFollowOrder = iter->pFollowOrder;

		if (pMainOrder!=NULL && pFollowOrder!=NULL)
		{
			if ((pMainOrder->OrderState == osFilled&&pFollowOrder->OrderState == osFilled)
				|| (pMainOrder->OrderState == osFail&&pFollowOrder->OrderState == osFail)
				|| (pMainOrder->OrderState == osCanceled&&pFollowOrder->OrderState == osCanceled))
			{
				iter = m_vOrderKey.erase(iter);
			}
			else
			{
				iter++;
			}
		}
		else
		{
			iter++;
		}
	}
}
bool TFollowTrade::IsOrderIDExist(uint orderID)
{
	for (UINT i = 0; i < m_vOrderKey.size(); i++)
	{
		if (m_vOrderKey[i].mainOrderID == orderID)
			return true;
	}
	return false;
}
void TFollowTrade::CreateGUID(char *pszRemark, int length)
{
	GUID guid = { 0 };
	CoCreateGuid(&guid);
	sprintf_s(pszRemark, length, "%08X-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X"
		, guid.Data1, guid.Data2, guid.Data3
		, guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3]
		, guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
}

/********************************************
Name: OnMainOrder
Func: 主账号Order回调处理，进行跟随账号的跟单操作
跟开仓，跟开仓，要判断方向
跟平仓，跟平仓，撤单+平仓，都成交的话好处理，
跟撤单，
主要分三大块处理
********************************************/
void TFollowTrade::OnMainOrder(TOrder *pOrder)
{	
	if (!IsOrderIDExist(pOrder->OrderID) && g_pTradeApi) //如果orderid不存在，说明是回调第一次回来
	{
		TSendOrder stOrder;
		memcpy(&stOrder, pOrder, sizeof(TSendOrder));

		TOrderKey orderKey = { 0 };
		orderKey.mainOrderID = pOrder->OrderID;
		orderKey.mainFirstCallBack = true;
		orderKey.mainOrderState = pOrder->OrderState;
		orderKey.pMainOrder = pOrder;
		       
		if (pOrder->Offset == oOpen)               // 如果是开仓，那就开仓，开仓要注意数量，要设置比率
		{
			OnMainOpen(pOrder, stOrder, orderKey);          // 插入新的数据，要分开仓和平仓，撤单
		}
		else if (pOrder->Offset == oCover || pOrder->Offset == oCoverT) //平仓的时候要看跟随账户的状态，是已成交还是在排队，还是部分成交
		{
			DealFollowCover(pOrder, stOrder, orderKey);//主账号成交后在平仓，跟平仓单
		}
	}
	else if ((pOrder->OrderState == osCanceling || pOrder->OrderState == osCanceled))//撤单, OrderID 已经存在，说明不是第一次进入这个条件
	{  
		OnMainCancel(pOrder);
	}
}
//主账号撤单，跟随账号撤单 平仓
void TFollowTrade::OnMainCancel(const TOrder* pOrder)
{
	vector<TCancelQty> vecCancelQty;
	bool bFirstBack = false;
	for (UINT i = 0; i < m_vOrderKey.size(); i++)
	{
		if (m_vOrderKey[i].mainOrderID == pOrder->OrderID && m_vOrderKey[i].mainFirstCallBack)//标记是否进入过撤单
		{
			m_vOrderKey[i].mainFirstCallBack = false;
			bFirstBack = true;

			const TOrder* pParOrder = g_pTradeData->GetOrder(m_vOrderKey[i].followOrderID);
			if (pParOrder /*&& (pParOrder->OrderState == osQueued || pParOrder->OrderState == osPartFilled)*/)
			{
				int iQty = pParOrder->OrderQty - pParOrder->MatchQty; //撤单数量， 未成交的数量，
				g_pTradeApi->CancelOrder(m_vOrderKey[i].followOrderID);
				AddLog(cancelAction, pParOrder->UserNo, pOrder);
				TCancelQty stCancelQty;
				strcpy_s(stCancelQty.UserNo, pParOrder->UserNo);
				stCancelQty.iQty = iQty;
				vecCancelQty.push_back(stCancelQty); //撤单数量
			}
			//break; 不能break，因为可能有很多跟随帐号
		}
	}
	if(bFirstBack)
		DealFollowCancel(pOrder,vecCancelQty); //跟随账号要马上撤，马上平
}
//主账号撤单，跟随账号撤单+平仓
//撤单+平仓数量等同 floor(mainOrder.Qty * Rate);
void TFollowTrade::DealFollowCancel(const TOrder*pOrder, vector<TCancelQty>& vecQty)
{
	TSendOrder stOrder;  //如果跟随账号已经成交，则需要平仓，平仓不成要追单，直到完全平掉
	memcpy(&stOrder, pOrder, sizeof(TSendOrder));

	vector<TFollowUser>::iterator iter;
	for (iter = m_followUser.begin(); iter != m_followUser.end(); iter++)
	{
		int iQty = 0;
		vector<TCancelQty>::iterator iterQty = find(vecQty.begin(), vecQty.end(), iter->UserNo);
		if (iterQty != vecQty.end())//否则说明没有撤单
			iQty = iterQty->iQty;//这个是 跟随账号的撤单量

		//new 四舍五入
		iQty = ceil((pOrder->OrderQty - pOrder->MatchQty)*iter->QtyRate)- iQty; //算出主账号的撤单量，乘以系数得到跟随账号应该 撤单+平仓的数量
		if (iQty <= 0)continue;

		//处理direct
		stOrder.Direct = pOrder->Direct;
		if (!iter->DirectSameToMain)
			stOrder.Direct = ReverseDirect(stOrder.Direct);
		//
		strcpy_s(stOrder.Sign, iter->Sign);
		const TPosition *pPosition = g_pTradeData->GetPositionTotal(iter->UserNo, &stOrder, stOrder.Direct);

		if (pPosition)//说明有持仓
		{
			stOrder.Direct = ReverseDirect(stOrder.Direct);
		//	memcpy(&stOrder, pOrder->pContract, sizeof(TContractKey));
			strcpy_s(stOrder.UserNo, iter->UserNo);


			stOrder.OrderPrice = GetPrice(pOrder, fptMarket, stOrder.Direct); //市价平仓
			//按照比例平
			stOrder.OrderQty = iQty;
			if (stOrder.OrderQty > pPosition->PositionQty)
				stOrder.OrderQty = pPosition->PositionQty;

			CreateGUID(stOrder.Remark, sizeof(stOrder.Remark));

			//g_pTradeApi->InsertOrder(stOrder);   //这个肯定要追平，主账号撤单，follow账号平跟的仓位
		//	stOrder.Offset = CConvert::GetOffset(iter->UserNo, &stOrder, stOrder.Direct);
		//	m_coverOrderNow.push_back(g_pTradeApi->InsertOrder(stOrder));        //将平仓单保存起来

			vector<uint> orderID = CConvert::CoverPos(pPosition, stOrder, stOrder.OrderQty);//平今平昨自适应
			for (uint i = 0; i < orderID.size(); i++)
			{
				m_coverOrderNow.push_back(orderID[i]);
			}

			AddLog(cancelCover, stOrder.UserNo, pOrder);
		}
	}
}
void TFollowTrade::OnMainOpen(const TOrder* pOrder, TSendOrder &stOrder, TOrderKey &orderKey)
{
	vector<TFollowUser>::iterator iterFollow;

	for (iterFollow = m_followUser.begin(); iterFollow != m_followUser.end(); iterFollow++) //跟开仓
	{
		//处理direct
		stOrder.Direct = pOrder->Direct;
		if (!iterFollow->DirectSameToMain)
			stOrder.Direct = ReverseDirect(stOrder.Direct);
		//end

		stOrder.OrderPrice = GetPrice(pOrder, iterFollow->FollowPriceType, stOrder.Direct);
		strcpy_s(stOrder.UserNo, iterFollow->UserNo);
		strcpy_s(stOrder.Sign, iterFollow->Sign);
		//new 四舍五入
		uint orderQty = floor(pOrder->OrderQty*iterFollow->QtyRate);
		if (orderQty)
		{
			CreateGUID(stOrder.Remark, sizeof(stOrder.Remark));
			stOrder.OrderQty = orderQty;
			//new 要进行测试
			orderKey.followQty = orderQty;
			orderKey.followOrderID = g_pTradeApi->InsertOrder(stOrder);//follow账号跟开仓

			//orderKey.mainTotalQty = pOrder->OrderQty;
			//orderKey.mainMatchQty = pOrder->MatchQty;
			m_vOrderKey.push_back(orderKey);
			//添加日志信息
			AddLog(openAction, iterFollow->UserNo, pOrder);
		}
	}
}


/*******************************************************
Func:跟平仓单
如果有持仓则全平，如果有挂单，则全撤
主账号平仓，跟随账号先 撤单 后 平仓， 要考虑数量
*******************************************************/
void TFollowTrade::DealFollowCover(TOrder *pOrder, TSendOrder &stOrder, TOrderKey &orderKey)
{
	vector<TFollowUser>::iterator iter;

	/*	TParOrderKey parOrderKey = { 0 };
		parOrderKey.pMainOrder = pOrder*/;
	vector<TCancelQty> vecQty;
	for (iter = m_followUser.begin(); iter != m_followUser.end(); iter++)
	{
		//处理direct
		TDirect direct = pOrder->Direct;
		if (iter->DirectSameToMain)
			direct = ReverseDirect(pOrder->Direct);
		//1. 是否有挂单，要等主账号成交后在撤单，主账号平仓，先检测跟随账号的跟随单是否还在排队中
		TDataContainer parOrders;

		//new 向下取整
		int iQty = 0;// floor(pOrder->OrderQty*iter->QtyRate); //撤单+平仓量总的
		g_pTradeData->GetParOrder(parOrders, ftValid, iter->Sign);
		for (UINT i = 0; i < parOrders.size(); i++)
		{
			const TOrder * pParOrder = (const TOrder*)parOrders.at(i);
			if (strcmp(pParOrder->UserNo, iter->UserNo) == 0  //判断用户
				&& memcmp(pOrder->ExchangeNo, pParOrder->ExchangeNo, sizeof(TContractKey) - sizeof(pOrder->Sign)) == 0  //挂单和主账号的平仓单，合约key相等
				&& direct == pParOrder->Direct
				&&pParOrder->Offset == oOpen
				&&CheckFollowParOrder(pParOrder))//是跟随者的挂单，则撤掉
			{
				g_pTradeApi->CancelOrder(pParOrder->OrderID);//直接撤单

				iQty = /*iQty - pParOrder->OrderQty +*/ pParOrder->MatchQty; //已经成交的数量
				TCancelQty stQty;
				strcpy_s(stQty.UserNo, iter->UserNo);
				stQty.iQty = iQty;
				vecQty.push_back(stQty);
				break;
			}
		}
	}

	for (iter = m_followUser.begin(); iter != m_followUser.end(); iter++)
	{
		vector<TCancelQty>::iterator iterQty;
		iterQty = find(vecQty.begin(), vecQty.end(), iter->UserNo);
		int iQty = 0;
		if (iterQty != vecQty.end()) //如果找到则说明撤了挂单
			iQty = iterQty->iQty;//这个数量是已经成交的数量，也就是要平仓的数量
		else  //没找到则没有撤挂单
			iQty = ceil(pOrder->OrderQty*iter->QtyRate);
		if (iQty <= 0)continue;
		//2. 平跟随者的持仓，如果有的话
		//处理direct
		TDirect direct = pOrder->Direct; 
		if (iter->DirectSameToMain)
			direct = ReverseDirect(pOrder->Direct);
		//end

		strcpy_s(stOrder.Sign, iter->Sign);
		const TPosition* pPosition = g_pTradeData->GetPositionTotal(iter->UserNo, &stOrder, direct);
		if (pPosition)
		{
			stOrder.Direct = ReverseDirect(direct);
			strcpy_s(stOrder.UserNo, iter->UserNo);
			stOrder.OrderQty = iQty;
			if (stOrder.OrderQty > pPosition->PositionQty)
				stOrder.OrderQty = pPosition->PositionQty;

			CreateGUID(stOrder.Remark, sizeof(stOrder.Remark));
			stOrder.OrderPrice = GetPrice(pOrder, iter->FollowPriceType, stOrder.Direct);
			if (stOrder.OrderPrice == 0)//这边处理的不是很好，获取不到价格，应该订阅行情获取，然后再进行操作
				stOrder.OrderPrice = pOrder->OrderPrice;
			//new
			//方案1，主账号怎么平，跟随账号怎么跟
			//orderKey.followOrderID = g_pTradeApi->InsertOrder(stOrder); //主账号平仓，follow账号跟平仓单
			//orderKey.followQty = iQty;
			//m_vOrderKey.push_back(orderKey);
	
			//parOrderKey.Remark = stOrder.Remark;//如果平仓不成功，等主账号成交后要追单
			//parOrderKey.FollowOrderID = orderKey.followOrderID;
			//m_coverOrderWait.push_back(parOrderKey);        //将平仓单保存起来，要在主账号平仓成功后再进行跟进操作
			//方案1 END
	
			//方案2 不管主账号怎么发，跟随账号优先平今，在平昨仓
			FollowMainCover(pPosition, pOrder, stOrder, orderKey); //平今平昨自适应

			AddLog(coverAction, iter->UserNo, pOrder);
		}
	}
}

void TFollowTrade::FollowMainCover(const TPosition* pPosition, const TOrder* pOrder, TSendOrder & stOrder, TOrderKey & orderKey)
{
	vector<uint> orderID = CConvert::CoverPos(pPosition, stOrder, stOrder.OrderQty);

	for (int i = 0; i < orderID.size(); i++)
	{
		orderKey.followOrderID = orderID[i];
		orderKey.followQty = stOrder.OrderQty;
		m_vOrderKey.push_back(orderKey);

		TParOrderKey parOrderKey;
		parOrderKey.pMainOrder = pOrder;
		parOrderKey.FollowOrderID = orderID[i];
		parOrderKey.Remark = stOrder.Remark;

		m_coverOrderWait.push_back(parOrderKey);
	}
}

double TFollowTrade::GetPrice(const TOrder* pOrder, int priceType, TDirect direct)
{
	if (priceType == fptFollowMain)
		return pOrder->OrderPrice;

	SQuoteSnapShot QuoteData;
	memset(&QuoteData, 0, sizeof(QuoteData));
	SContractNoType qNo;
	CConvert::SingleTradeContractToQContractNoType(*pOrder, qNo);
	double dBuyPrice = 0.0;
	double dSellPrice = 0.0;
	double dUpPrice = 0.0;
	double dDownPrice = 0.0;
	if (g_pStarApi->GetSnapshot(qNo, &QuoteData))
	{
		dBuyPrice = QuoteData.Data[S_FID_BESTBIDPRICE].Price;//对手价  买价
		dSellPrice = QuoteData.Data[S_FID_BESTASKPRICE].Price;//对手价， 卖价
		dUpPrice = QuoteData.Data[S_FID_LIMITUPPRICE].Price;
		dDownPrice = QuoteData.Data[S_FID_LIMITDOWNPRICE].Price;
		if (direct == dSell)
		{
			if (priceType == fptConsideration)
				return dBuyPrice;
			else if (priceType == fptQueuePrice)
				return dSellPrice;
			else if (priceType == fptMarket)
				return dDownPrice;
		}
		else if (direct == dBuy)
		{
			if (priceType == fptConsideration)
				return dSellPrice;
			else if (priceType == fptQueuePrice)
				return dBuyPrice;
			else if (priceType == fptMarket)
				return dUpPrice;
		}
	}
	return pOrder->OrderPrice;
}


void TFollowTrade::OnUserLogin(LPARAM lParam)
{
	bool bAddLog = false;
	if (m_cAddUser == addNone)
	{
		//判断用户是否是短线重连，看是主账号还是跟随账号
		if (m_iAllDisconnect==notUse)
			return;
		if (m_iAllDisconnect == userAllDisconnect)
			m_iAllDisconnect = userReConnect;
		if (lParam)
		{
			TLoginBase * login = (TLoginBase*)lParam;
			auto iter = find(m_vecAllUser.begin(), m_vecAllUser.end(), login->LoginUser);
			if (iter != m_vecAllUser.end())
			{
				if (iter->State == 0)
				{
					m_cAddUser = addMainUser;
					AddLog(mainUserRelogin, iter->UserNo, NULL);
					bAddLog = true;
				}
				else if (iter->State == 1)
				{
					m_cAddUser = addFollowUser;
					AddLog(followUserRelogin, iter->UserNo, NULL);
					bAddLog = true;
				}
				else return;

			}
			else return;

		}
	}
	TLoginBase* loginInfo = (TLoginBase*)lParam;
	TFollowUser stUser;
	
	g_pTradeApi->QryExchangeState(loginInfo->LoginUser, loginInfo->Sign);

	string strUserNo = loginInfo->LoginUser;
	string::size_type pos = strUserNo.find(loginInfo->BrokerNo);
	if (pos != string::npos)
		strUserNo = strUserNo.substr(strlen(loginInfo->BrokerNo), string::npos);
	strcpy_s(stUser.UserNo, strUserNo.c_str());
	strcpy_s(stUser.Sign, loginInfo->Sign);
	stUser.State = HasLogin;
	vector<TUser>::iterator iter;
	vector<TFollowUser>::iterator iterFollow;
	for (iterFollow = m_followUser.begin(); iterFollow != m_followUser.end(); iterFollow++)
	{
		if (strcmp(stUser.UserNo, iterFollow->UserNo) == 0)
			return;
	}
	for (iter = m_mainUser.begin(); iter != m_mainUser.end(); iter++)
	{
		if (strcmp(stUser.UserNo, iter->UserNo) == 0)
			return;
	}
	if (m_cAddUser == addFollowUser)
	{
		stUser.iTimerCount = 0;
		stUser.iCheckPositionCount = 0;
		m_followUser.push_back(stUser);
		
		PushUser(stUser, 1); //1是follow user

		ReadConfigData(&m_followUser[m_followUser.size()-1]);
		UpdateFollowLV();
		if (!bAddLog)
			AddLog(logFollowUserLogin, stUser.UserNo, NULL);
	}
	else if (m_cAddUser == addMainUser)
	{
		m_mainUser.push_back(stUser);
		PushUser(stUser, 0); //0是main user
		UpdateMainLV();
		if (!bAddLog)
			AddLog(logMainUserLogin, stUser.UserNo, NULL);
	}
	
	m_bUserLogin = true;
	if (m_iAllDisconnect == userReConnect)
	{
		m_cAddUser = addNone;
	}
//	m_cAddUser = addNone;
}
void TFollowTrade::PushUser(TFollowUser & stUser, int type)
{
	TUser user;
	strcpy_s(user.UserNo, stUser.UserNo);
	user.State = type;
	auto iter = find(m_vecAllUser.begin(), m_vecAllUser.end(), user.UserNo);
	if (iter != m_vecAllUser.end())
	{
		iter->State = type;
	}
	else
	{
		m_vecAllUser.push_back(user);
	}
}
void TFollowTrade::UpdateFollowLV()
{
	UINT size = m_followUser.size();
	if (size > 0)
	{
		m_lvFoloowAccount.AddItem(m_followUser[size - 1].UserNo, size - 1, 0);
		m_lvFoloowAccount.AddItem("已登录", size - 1, 1);
	}
	UpdateConfig(size-1);
}
void TFollowTrade::UpdateMainLV()
{
	UINT size = m_mainUser.size();
	if (size > 0)
	{
		m_lvMainAccount.AddItem(m_mainUser[size - 1].UserNo, size - 1, 0);
		m_lvMainAccount.AddItem("已登录", size - 1, 1);
	}
}
void TFollowTrade::OnNotify(WPARAM wParam, LPARAM lParam)
{
	LPNMITEMACTIVATE p = (LPNMITEMACTIVATE)lParam;
	switch (wParam)
	{
	case id_lvMainAccount:
		OnDealMainLV(lParam);
		break;
	case id_lvFollowAccount:
		OnDealFollowLV(lParam);
		break;
	default:
		break;
	}

}
void TFollowTrade::ShowMenu(HMENU hMenu)
{
	POINT pt;
	GetCursorPos(&pt);
	TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_TOPALIGN, pt.x, pt.y, 0, m_Hwnd, NULL);
}
void TFollowTrade::OnDealFollowLV(LPARAM lParam)
{
	LPNMITEMACTIVATE p = (LPNMITEMACTIVATE)lParam;
	if (p &&p->hdr.code == NM_RCLICK)
	{
		if (p->iItem != -1)
		{
			EnableMenuItem(m_hFollowMenu, idm_logoutFollow, MF_ENABLED);
			EnableMenuItem(m_hFollowMenu, idm_setFollow, MF_ENABLED);
		}
		else
		{
			EnableMenuItem(m_hFollowMenu, idm_logoutFollow, MF_GRAYED);
			EnableMenuItem(m_hFollowMenu, idm_setFollow, MF_GRAYED);
		}

		ShowMenu(m_hFollowMenu);
	}
	else if (p->hdr.code == NM_CLICK)
	{
		if (p->iItem == -1)
			m_lvFoloowAccount.SelectItem(-1);
	}
}
void TFollowTrade::OnDealMainLV(LPARAM lParam)
{
	LPNMITEMACTIVATE p = (LPNMITEMACTIVATE)lParam;
	if (p &&p->hdr.code == NM_RCLICK)
	{
		if (p->iItem!=-1)
			EnableMenuItem(m_hMainMenu, idm_logoutMain, MF_ENABLED);
		else
			EnableMenuItem(m_hMainMenu, idm_logoutMain, MF_GRAYED);

		ShowMenu(m_hMainMenu);
	}
	else if (p->hdr.code == NM_CLICK)
	{
		if (p->iItem == -1)
			m_lvMainAccount.SelectItem(-1);
	}
}
LRESULT TFollowTrade::OnCtlColor(WPARAM wParam, LPARAM lParam)
{
	SetBkMode(HDC(wParam), TRANSPARENT);
	return (LRESULT)g_ColorRefData.g_brush_white;
}
void TFollowTrade::OnDrawItem(WPARAM wParam, LPARAM lParam)
{
	LPDRAWITEMSTRUCT p = (LPDRAWITEMSTRUCT)lParam;
	if (p&&p->CtlID == id_lvMainAccount)
		m_lvMainAccount.OnDrawItem(lParam);
	else if (p&&p->CtlID == id_lvFollowAccount)
		m_lvFoloowAccount.OnDrawItem(lParam);
}
void TFollowTrade::OnMeasureItem(WPARAM wParam, LPARAM lParam)
{
	LPMEASUREITEMSTRUCT p = (LPMEASUREITEMSTRUCT)lParam;
	if (p&&p->CtlID == id_lvMainAccount)
		m_lvMainAccount.OnMeasureItem(p->itemHeight);
	else if (p&&p->CtlID == id_lvFollowAccount)
		m_lvFoloowAccount.OnMeasureItem(p->itemHeight);
}
void TFollowTrade::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if (HIWORD(wParam) == 0 && lParam == 0)//表明是菜单
	{
		switch (LOWORD(wParam))
		{
			//跟随账户控制
		case idm_loginFollow:
			m_cAddUser = addFollowUser;
			ShowLogin();
			break;
		case  idm_addFollow:
			break;
		case idm_logoutFollow:
			OnLogout(&m_lvFoloowAccount);
			break;
		case  idm_deleteFollow:
			break;
		case idm_setFollow:
		{
			int index = m_lvFoloowAccount.GetSelectItem();
			if (index>=0 && index<m_followUser.size())
				ShowConfigFrm(index);
			break;
		}

			
			//主账户控制
		case idm_loginMain:
			m_cAddUser = addMainUser;
			ShowLogin();
			break;
		case idm_addMain:
			break;
		case idm_logoutMain:
			OnLogout(&m_lvMainAccount);
			break;
		case idm_deleteMain:
			break;
		default:
			break;
		}
	}
}
void TFollowTrade::ShowConfigFrm(int index)
{
//	m_bHasStartFollow = false;

	TFollowConfig config(m_Hwnd, "跟单设置");
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	POINT pt;
	pt.x = (rect.right - rect.left - configFrmWidth) / 2;
	pt.y = (rect.bottom - rect.top - configFrmHeight) / 2;
	ClientToScreen(m_Hwnd, &pt);
	config.MoveWindow(pt.x, pt.y, configFrmWidth, configFrmHeight);

	config.SetData(&m_followUser[index]);

	config.Domodal();
	UpdateConfig(index);

//	m_bHasStartFollow = true;
}
void TFollowTrade::OnLogout(TListView *listView)
{
	TFollowUser stUser;
	int index = listView->GetSelectItem(stUser.UserNo, sizeof(stUser.UserNo), 0);
	if (listView == &m_lvMainAccount)
	{
		vector<TUser>::iterator iter;
		iter = find(m_mainUser.begin(), m_mainUser.end(), stUser);
		if (iter != m_mainUser.end())
		{
			g_pTradeApi->Close(iter->UserNo, iter->Sign);
			listView->DeleteItem(index);
			AddLog(logMainUserLogout, iter->UserNo, NULL);
			m_mainUser.erase(iter);
		}
	}
	else if (listView == &m_lvFoloowAccount)
	{
		vector<TFollowUser>::iterator iter;
		iter = find(m_followUser.begin(), m_followUser.end(), stUser);
		if (iter != m_followUser.end())
		{
			g_pTradeApi->Close(iter->UserNo, iter->Sign);
			listView->DeleteItem(index);
			AddLog(logFollowUserLogout, iter->UserNo, NULL);
			m_followUser.erase(iter);
		}
	}

}

void TFollowTrade::ShowLogin()
{
	if (g_pPluginMgrApi)
	{
		g_pMainFrm->Linkage(m_Hwnd, "", "TradeLogin", "");
	}
}
void TFollowTrade::OnPaint()
{
	TMemDC memdc(m_Hwnd);
	SetBkMode(memdc.GetHdc(), TRANSPARENT);
	RECT rc;
	GetClientRect(m_Hwnd, &rc);
	RECT rect = rc;
	rect.bottom = captionHeight;
	HBRUSH hbrCaption = CreateSolidBrush(captionColor);
	FillRect(memdc.GetHdc(), &rect, hbrCaption);
	HBRUSH hbr = CreateSolidBrush(g_ColorRefData.g_ColorWhite);
	rc.top = captionHeight;
	FillRect(memdc.GetHdc(), &rc, hbr);
	rc.top = 0;
	FrameRect(memdc.GetHdc(), &rc, hbrCaption);
	InflateRect(&rc, -1, -1);
	FrameRect(memdc.GetHdc(), &rc, hbrCaption);
	rect.left = 5;
	SetTextColor(memdc.GetHdc(), RGB(255, 255, 255));
	SelectObject(memdc.GetHdc(), g_FontData.g_FontWord12);
	DrawTextA(memdc.GetHdc(), "跟单小助手", -1, &rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	RECT rcLog = { editLeft - 1, m_iEditTop - 1, editLeft + editWidth + 1, m_iEditTop + m_iEditHeight + 1 };
	FrameRect(memdc.GetHdc(), &rcLog, g_ColorRefData.g_brush_gray);

	DeleteObject(hbrCaption);
	DeleteObject(hbr);

	DrawClose(&memdc);
}

void TFollowTrade::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	POINT pt;
	pt.x = LOWORD(lParam);
	pt.y = HIWORD(lParam);
	rect.bottom = captionHeight;
	if (PtInRect(&m_rcClose, pt))
	{
		m_bCloseClick = true;
		RECT rect = m_rcClose;
		InflateRect(&rect, 2, 2);
		::RedrawWindow(m_Hwnd, &rect, 0, RDW_INVALIDATE | RDW_UPDATENOW | RDW_NOCHILDREN);
	}
	else if (PtInRect(&rect, pt))
	{
		PostMessage(m_Hwnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
	}
}

void TFollowTrade::OnInitProgress(const int ErrorCode, const TradeInitProgress CurrProgress, const TLoginBase * pLoginBase)
{
	if (CurrProgress == tipReady)
	{
		//PostMessage(m_Hwnd, WM_USERLOGIN, 0, (LPARAM)pLoginBase);
		//PostMessage(m_Hwnd, WM_USERLOGIN, 0, (LPARAM)pLoginBase);
		OnUserLogin((LPARAM)pLoginBase);
	}
}
void TFollowTrade::OnOrder(const TOrder * pOrder)
{
	if (pOrder)
		PostMessage(m_Hwnd, WM_ONORDER, 0, (LPARAM)pOrder);
}

void TFollowTrade::OnPositionTotal(const TTotalPosition * pPosition)
{
	if (pPosition)
		PostMessage(m_Hwnd, WM_ONPOSITION, 0, (LPARAM)pPosition);
}

bool TFollowTrade::CheckFollowParOrder(const TOrder* pOrder)
{
	for (int i = 0; i < m_vOrderKey.size(); i++)
	{
		if (pOrder->OrderID == m_vOrderKey[i].followOrderID)
			return true;
	}
	return false;
}


void TFollowTrade::DrawClose(TMemDC *pMemDC)
{
	HPEN hPen = CreatePen(PS_SOLID, 1, RGB(230, 230, 230));
	HPEN hOldPen = (HPEN)SelectObject(pMemDC->GetHdc(), hPen);

	int offset = m_bCloseClick ? 1 : 0;
	POINT ptCloseBegin[4] = { 0 };
	POINT ptCloseEnd[4] = { 0 };
	ptCloseBegin[0] = { m_rcClose.left + offset, m_rcClose.top + offset };
	ptCloseBegin[1] = { m_rcClose.left + 1 + offset, m_rcClose.top + offset };
	ptCloseBegin[2] = { m_rcClose.left + offset, m_rcClose.bottom + offset };
	ptCloseBegin[3] = { m_rcClose.left + 1 + offset, m_rcClose.bottom + offset };

	ptCloseEnd[0] = { m_rcClose.right + 1 + offset, m_rcClose.bottom + 1 + offset };
	ptCloseEnd[1] = { m_rcClose.right + 2 + offset, m_rcClose.bottom + 1 + offset };
	ptCloseEnd[2] = { m_rcClose.right + 1 + offset, m_rcClose.top - 1 + offset };
	ptCloseEnd[3] = { m_rcClose.right + 2 + offset, m_rcClose.top - 1 + offset };
	for (int i = 0; i < 4; i++)
	{
		MoveToEx(pMemDC->GetHdc(), ptCloseBegin[i].x, ptCloseBegin[i].y, 0);
		LineTo(pMemDC->GetHdc(), ptCloseEnd[i].x, ptCloseEnd[i].y);
	}

	SelectObject(pMemDC->GetHdc(), hOldPen);
	DeleteObject(hPen);
}

void TFollowTrade::OnLButtonUp(WPARAM wParam, LPARAM lParam)
{
	POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

	if (PtInRect(&m_rcClose, pt))
	{
		ShowWindow(m_Hwnd, SW_HIDE);
		m_bCloseClick = false;
	}
}

TDirect TFollowTrade::ReverseDirect(TDirect direct)
{
	if (direct == dBuy)
		return dSell;
	return dBuy;
}


bool TFollowTrade::IsAuthent()
{
	if (!g_pAuthent)
	{
		_TMessageBox_Domodal(FindWindow(MainFrmName, NULL), "提示", L"认证出错！", TMB_OK);
		return false;
	}

	UserAuthentEx stAuthent;
	memset(&stAuthent, 0, sizeof(UserAuthentEx));
	bool bRet = g_pAuthent->GetAuthent("Esunny", "es2002", stAuthent); //这两个都需要改

	wchar_t errorInfo[200] = {0};
	g_pAuthent->GetErrorInfo(errorInfo, 200);

	if (!bRet)
	{
		TAccessAlarm alarmFrm(FindWindow(MainFrmName, NULL), "认证未通过", TAccessAlarm::noAuthent);
		alarmFrm.Domodal();
		return false;
	}
	if (stAuthent.serverdate < stAuthent.auth.value[0].date) //在授权日期内
	{
		TAccessAlarm alarmFrm(FindWindow(MainFrmName, NULL), "使用时间提醒", TAccessAlarm::daysLeftWarn);
		if (alarmFrm.SetDate(stAuthent.serverdate, stAuthent.auth.value[0].date))
			alarmFrm.Domodal();
		return true;
	}
	else
	{
		TAccessAlarm alarmFrm(FindWindow(MainFrmName, NULL), "使用时间过期", TAccessAlarm::userTimeOut);
		alarmFrm.Domodal();
	}

	return false;
}


UINT TFollowTrade::OnNcHitTest(WPARAM wParam, LPARAM lParam)
{
	POINT point;
	point.x = GET_X_LPARAM(lParam);
	point.y = GET_Y_LPARAM(lParam);
	RECT rect;
	GetWindowRect(m_Hwnd, &rect);

	if (point.y >= rect.bottom - 5)
	{
		return HTBOTTOM;
	}

	return NOT_PROCESSED;
}
void TFollowTrade::OnSetCursor(WPARAM wParam, LPARAM lParam)
{
	if ((HWND)wParam != m_Hwnd)
		return;
	UINT nHitTest = LOWORD(lParam);
	switch (nHitTest)
	{
	case HTTOP:
	case HTBOTTOM:
		SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZENS)));
		break;
	default:
		SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
		break;
	}
}
void TFollowTrade::OnNcLButtonDown(WPARAM wParam, LPARAM lParam)
{
	if (wParam==HTBOTTOM)
		SendMessage(m_Hwnd, WM_SYSCOMMAND, SC_SIZE | WMSZ_BOTTOM, lParam);
}

void TFollowTrade::OnSize(int cy)
{
	int diff = cy -frameHeight;
	if (diff > -5)
	{
		//m_iEditHeight = editHeight + diff;
		//SetWindowPos(m_editLog.GetHwnd(), 0, 0, 0, editWidth, m_iEditHeight, SWP_NOMOVE | SWP_NOZORDER);
		//m_btnStart.MoveWindow(btnLeft, btnTop + diff, btnWidth, btnHeight);
		m_iTradeInfoHeight = frmTradeInfoHeight + diff;
		m_iEditTop =editTop+ diff;
		m_labLogInfo.MoveWindow(lvMainLeft, labLogInfoTop+diff, labelWidth, labelHeight);
		SetWindowPos(m_frmTradeInfo.GetHwnd(), 0, 0, 0, frmTradeInfoWidth, m_iTradeInfoHeight,SWP_NOMOVE|SWP_NOZORDER );
		SetWindowPos(m_editLog.GetHwnd(), 0, editLeft, m_iEditTop, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		m_btnStart.MoveWindow(btnLeft, btnTop + diff, btnWidth, btnHeight);
	}
}
void TFollowTrade::OnSizing(WPARAM wParam, LPARAM lParam)
{
	LPRECT pRect = (LPRECT)lParam;
	if (pRect->bottom - pRect->top < frameHeight)
	{
		if (wParam == WMSZ_BOTTOM || wParam == WMSZ_BOTTOMLEFT || wParam == WMSZ_BOTTOMRIGHT)
			pRect->bottom = pRect->top + frameHeight;
		else
			pRect->top = pRect->bottom - frameHeight;
	}
}

void TFollowTrade::OnDisconnect(const TSocketDisconnectReason ErrorCode, const TLogin * pLogin)
{
	if (ErrorCode >=sdrInitiative &&ErrorCode<=sdrConnectOuttime && pLogin)
	{
		char szReason[50] = { 0 };
		GetDisconnectReason(ErrorCode, szReason, sizeof(szReason));
		for (uint i = 0; i < m_followUser.size(); i++)
		{
			if (strcmp(m_followUser[i].UserNo, pLogin->LoginUser) == 0)
			{
				m_followUser.erase(m_followUser.begin() + i);
				m_lvFoloowAccount.DeleteItem(i);
				sprintf_s(m_szDisconnectReason, "跟随账号%s：%s", pLogin->LoginUser, szReason);
				AddLog(followUserLogout, m_szDisconnectReason, NULL);
				//_TMessageBox_Domodal(m_Hwnd, "警告", const_cast<wchar_t*>(LoadRC::ansi_to_unicode(m_szDisconnectReason).c_str()), TMB_OK);
				return;
			}
		}

		for (uint i = 0; i < m_mainUser.size(); i++)
		{
			if (strcmp(m_mainUser[i].UserNo, pLogin->LoginUser) == 0)
			{
				m_mainUser.erase(m_mainUser.begin() + i);
				m_lvMainAccount.DeleteItem(i);
				sprintf_s(m_szDisconnectReason, "主账号%s：%s", pLogin->LoginUser, szReason);
				AddLog(mainUserLogout, m_szDisconnectReason, NULL);
				//_TMessageBox_Domodal(m_Hwnd, "警告", const_cast<wchar_t*>(LoadRC::ansi_to_unicode(m_szDisconnectReason).c_str()), TMB_OK);
				return;
			}
		}

		if (m_mainUser.size() == 0 && m_followUser.size() == 0)
		{
			m_iAllDisconnect = userAllDisconnect;
		}
	}
}


void TFollowTrade::OnExchangeState(const TExchangeState * pExchangeState)
{
	char exNo[11];
	memcpy(exNo,pExchangeState->ExchangeNo, 11);
	exNo[10] = '\0';
	if (!strcmp(exNo, "CFFEX")
		||!strcmp(exNo, "DCE")
		||!strcmp(exNo, "SHFE")
		||!strcmp(exNo, "ZCE"))
	{

		//默认key : ExchangeNo+CommodityNo
		char key[100] = { 0 };
		strcpy_s(key, exNo);
		strcat_s(key, "+");
		strcat_s(key, pExchangeState->CommodityNo);
		auto iter = m_mapExchangeState.find(key);
		if (iter == m_mapExchangeState.end())
			m_mapExchangeState.insert(make_pair(key, pExchangeState->TradeState));
		else
			iter->second = pExchangeState->TradeState;
		//备用key: ExchangeNo
		
	}

}