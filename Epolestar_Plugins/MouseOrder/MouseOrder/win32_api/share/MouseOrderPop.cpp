#include "..\PreInclude.h"
//色彩
extern G_COLOREF g_ColorRefData;
extern G_UIFONT g_FontData;
extern vector<CMousewin*>	g_MouseVec;
extern IMainFrame	*g_mainframe;
extern CMouseOrderPopWin g_MousePopwin;
extern IStarApi *g_pStarApi;
extern ICspAccessApi *g_pAccessApi;
extern ILanguageApi	*g_pLanguage;

extern void OnLinkageAction(HWND From, HWND To, const char* Sender, const char* Action, const char* Content);

#define N_TITLE_HEIGHT 11
void OnLinkageActionPop(HWND From, HWND To, const char* Sender, const char* Action, const char* Content)
{
	if (g_MousePopwin.GetHwnd() == To)
	{
		if ((!strcmp(Sender, "MainFrame")||!strcmp(Sender, "PolestarQuote"))&& !strcmp(Action, "SwitchOrder"))//下单切换
			g_MousePopwin.ShowPopWin();
		else if (!strcmp("TSystemLockWnd", Sender) || !strcmp(Sender, "MainFrame"))
		{
            if (!strcmp("LockApplication", Action)|| !strcmp(Action, "HideMin"))
            {
                g_MousePopwin.SetLock(IsWindowVisible(g_MousePopwin.GetHwnd())?true:false);
                ShowWindow(g_MousePopwin.GetHwnd(), SW_HIDE);
            }
            else if (!strcmp("RestoreApplication", Action) || !strcmp(Action, "ShowOld"))
            {
                if (g_MousePopwin.IsLock())
                {
                    ShowWindow(g_MousePopwin.GetHwnd(), SW_SHOW);
                    g_MousePopwin.SetLock(false);
                }
            }
        }
	}
}
void GetPropertyByStr(WinProperty &wProperty,char(&cProperty)[MAX_PATH])
{
	vector<NoticeContent> vInfo;
	CConvert::SplitNotice(cProperty, ';', '=', vInfo);
	for (vector<NoticeContent>::iterator iter = vInfo.begin(); iter != vInfo.end(); iter++)
	{
		if (!strcmp(iter->strKey.c_str(), _FastCheck))
			wProperty.bCheckFast = atoi(iter->strContent.c_str()) ? true : false;
		if (!strcmp(iter->strKey.c_str(), _AutoCover))
			wProperty.bCheckAutoCOver = atoi(iter->strContent.c_str()) ? true : false;
		if (!strcmp(iter->strKey.c_str(), _CoverMode))
			wProperty.cCoverMode = iter->strContent.at(0);
		if (!strcmp(iter->strKey.c_str(), _AddOne))
			wProperty.bAddOne = atoi(iter->strContent.c_str()) ? true : false;
		if (!strcmp(iter->strKey.c_str(), _MouseHead))
			wProperty.nMouseHead = atoi(iter->strContent.c_str());
		if (!strcmp(iter->strKey.c_str(), _ChildHead))
			wProperty.nChildHead = atoi(iter->strContent.c_str());
		if (!strcmp(iter->strKey.c_str(), _ChaseOrder))
			wProperty.bChaseOrder = atoi(iter->strContent.c_str()) ? true : false;
		if (!strcmp(iter->strKey.c_str(), _Advanced))
			wProperty.bAdvanced = atoi(iter->strContent.c_str()) ? true : false;
		if (!strcmp(iter->strKey.c_str(), _HeadItems))
			strcpy_s(wProperty.HeadItemsInfo, iter->strContent.c_str());
		if (!strcmp(iter->strKey.c_str(), _ExchangeSupport))
			wProperty.bExchangeSup = atoi(iter->strContent.c_str()) ? true : false;
        if (!strcmp(iter->strKey.c_str(), _RightCancel))
            wProperty.bRightCancel = atoi(iter->strContent.c_str()) ? true : false;
        if (strcmp(iter->strKey.c_str(), _CoverOpen) == 0)
            wProperty.bCoverOpen = atoi(iter->strContent.c_str()) ? true : false;
        if (strcmp(iter->strKey.c_str(), _BatchCondition) == 0)
            wProperty.bCondition = atoi(iter->strContent.c_str()) ? true : false;
	}
}
CMouseOrderPopWin::CMouseOrderPopWin() 
	:m_gmouseopr(NULL)
	, m_bInCloseRect(false)
    , m_bLock(false)
    , m_bCompleted(false)
{
   
}

CMouseOrderPopWin::~CMouseOrderPopWin()
{
	CloseFrame();
    g_pStarApi->UnregEventSpi(this);
}

void CMouseOrderPopWin::Init()
{
    g_pStarApi->RegEventSpi(this);
    g_pTradeData->Register(this);

	char str[_MAX_PATH];
	GetModuleFileNameA(NULL, str, sizeof(str));
	string sRoot = str;
	int npos = sRoot.find_last_of('\\');
	sRoot = sRoot.substr(0, npos);
	string str2 = sRoot + ("\\config\\MouseOrder.Popwin.pri");
	strcpy_s(m_strPopwinProperty, sizeof(m_strPopwinProperty), str2.c_str());

	Show();

	g_mainframe->AddLinkage(m_Hwnd, 0, OnLinkageActionPop);

	ShowWindow(m_Hwnd, SW_HIDE);
}

void CMouseOrderPopWin::Deinit()
{
	//取消联动
	g_mainframe->DelLinkage(m_Hwnd);
	if (m_gmouseopr&&IsWindow(m_gmouseopr->GetHwnd()))
	{
		g_mainframe->DelLinkage(m_gmouseopr->GetHwnd());
		char cProperty[MAX_PATH] = { 0 };
		m_gmouseopr->GetProperty(cProperty);

        RECT rect;
        GetWindowRect(m_Hwnd, &rect);
        sprintf_s(cProperty, "%s;left=%d;top=%d;right=%d;bottom=%d", cProperty, rect.left, rect.top, rect.right, rect.bottom);

        SaveProperty(cProperty);
	}
    ShowWindow(m_Hwnd, SW_HIDE); /// 避免主窗口关闭 弹出窗口还在显示
}

void CMouseOrderPopWin::ShowPopWin()
{
	if (!IsWindowVisible(m_Hwnd))
		ShowWindow(m_Hwnd, SW_SHOW);
	else
		ShowWindow(m_Hwnd, SW_HIDE);
	if (!m_gmouseopr)
	{	
		char cProperty[MAX_PATH] = {0};
		LoadProperty(cProperty);

		WinProperty wProperty;
		GetPropertyByStr(wProperty, cProperty);

		m_gmouseopr = new CMousewin(wProperty);
		m_gmouseopr->Create(L"MouseOrder", m_Hwnd, WS_CHILD | WS_VISIBLE| WS_CLIPCHILDREN, 0);
		g_mainframe->AddLinkage(m_gmouseopr->GetHwnd(), 0, OnLinkageAction);
		g_MouseVec.push_back(m_gmouseopr);

        RECT rect;
        if (GetRestorePos(cProperty, &rect))
        {
            RestorePos(&rect);
        }
        else
        {
            AdjustChildrenPos();
            SetCenterShow(m_Hwnd);
        }
	}
}

bool CMouseOrderPopWin::GetRestorePos(char * property, RECT *rect)
{
    vector<NoticeContent> vInfo;
    CConvert::SplitNotice(property, ';', '=', vInfo);
    bool bSaved = false;
    for (vector<NoticeContent>::iterator iter = vInfo.begin(); iter != vInfo.end(); iter++)
    {
        if (!strcmp(iter->strKey.c_str(), "left"))
        {
            rect->left = atoi(iter->strContent.c_str());
            bSaved = true;
        }
        if (!strcmp(iter->strKey.c_str(), "top"))
        {
            rect->top = atoi(iter->strContent.c_str());
            bSaved = true;
        }
        if (!strcmp(iter->strKey.c_str(), "right"))
        {
            rect->right = atoi(iter->strContent.c_str());
            bSaved = true;
        }
        if (!strcmp(iter->strKey.c_str(), "bottom"))
        {
            rect->bottom = atoi(iter->strContent.c_str());
            bSaved = true;
        }
    }
    return bSaved;
}

void CMouseOrderPopWin::RestorePos(RECT *rect)
{
    if (m_gmouseopr)
    {
        RECT sr;
        sr.left = 2;
        sr.right = rect->right - rect->left - 2;
        sr.top = N_TITLE_HEIGHT;
        sr.bottom = rect->bottom - rect->top - 2;

        MoveWindow(m_gmouseopr->GetHwnd(), sr.left, sr.top, sr.right - sr.left, sr.bottom - sr.top, true);
		
        //SetWindowPos(m_gmouseopr->GetHwnd(), 0, sr.left, sr.top, sr.right - sr.left, sr.bottom - sr.top, SWP_NOZORDER | SWP_NOSENDCHANGING);	//restore
    }
    m_rtClose.left = rect->right - rect->left - N_TITLE_HEIGHT - 2;
    m_rtClose.right = rect->right - rect->left - 2;
    m_rtClose.top = /*rect->top +*/ 2;
    m_rtClose.bottom = /*rect->top +*/ N_TITLE_HEIGHT;

    SetWindowPos(m_Hwnd, HWND_TOPMOST, rect->left, rect->top, rect->right - rect->left, rect->bottom - rect->top, SWP_NOACTIVATE | SWP_SHOWWINDOW);
	CheckAndSetMinimize();
}

void CMouseOrderPopWin::AdjustChildrenPos()
{
	RECT sr;
	GetWindowRect(m_Hwnd, &sr);

	if (m_gmouseopr)
	{
		RECT cr;
		cr.left = 2;
		cr.right = sr.right - sr.left - 2;
		cr.top = N_TITLE_HEIGHT;
		cr.bottom = sr.bottom - sr.top - 2;

		SetWindowPos(m_gmouseopr->GetHwnd(), 0, cr.left, cr.top, cr.right-cr.left, cr.bottom-cr.top, SWP_NOZORDER | SWP_NOSENDCHANGING);	//restore
	}
	m_rtClose.left = sr.right - sr.left - N_TITLE_HEIGHT-2;
	m_rtClose.right = sr.right - sr.left - 2;
	m_rtClose.top = 2;
	m_rtClose.bottom = N_TITLE_HEIGHT;
}

void CMouseOrderPopWin::OnInitComplete()
{
    if (m_bCompleted)
    {
        return;
    }
    m_nLoginType = TYPE_Inner;

    SConfigSwitchInfo *sInfo = { 0 };
    if (g_pAccessApi->GetConfigSwitchInfo(S_CONFIGNO_INNER_FOREIGN, &sInfo, 1, false) != 0 &&
        strcmp(sInfo->ConfigSwitchNo, S_CONFIGNO_INNER_FOREIGN) == 0 &&
        sInfo->ConfigSwitchValue == S_CONFIGVALUE_INNER)
    {
        m_nLoginType = TYPE_Inner;       // 内盘
    }
    else
    {
        m_nLoginType = TYPE_For;         // 外盘
    }

    /// 点价是否显示
    bool bShowLadder = true;
    if (g_pAccessApi->GetConfigSwitchInfo(S_CONFIGNO_LADDERORDER, &sInfo, 1, false) > 0)
    {
        bShowLadder = sInfo->ConfigSwitchValue == 0;
    }
    
    /// 批量
    bool bShowBatchOrder = true;
    if (g_pAccessApi->GetConfigSwitchInfo(S_CONFIGNO_BATCHORDER, &sInfo, 1, false) > 0)
    {
        bShowBatchOrder = sInfo->ConfigSwitchValue == 0;
    }
    
    /// 影子单是否显示
    bool bShowShadow = true;
    if (g_pAccessApi->GetConfigSwitchInfo(S_CONFIGNO_GHOST, &sInfo, 1, false) > 0)
    {
        bShowShadow = sInfo->ConfigSwitchValue == 0 ? 1 : 0;
    }

    /// 冰山单
    bool bShowIce = true;
    if (g_pAccessApi->GetConfigSwitchInfo(S_CONFIGNO_ICEBERG, &sInfo, 1, false) > 0)
    {
        bShowIce = sInfo->ConfigSwitchValue == 0 ? 1 : 0;
    }
    
    /// 行权 询价 期权
    bool bShowOption = true;
    if (g_pAccessApi->GetConfigSwitchInfo(S_CONFIGNO_EXE_ENQUIRY, &sInfo, 1, false) > 0)
    {
        bShowOption = sInfo->ConfigSwitchValue == 0 ? 1 : 0;
    }
    
    /// 应价
    bool bShowReply = true;
    if (g_pAccessApi->GetConfigSwitchInfo(S_CONFIGNO_OFFER, &sInfo, 1, false) > 0)
    {
        bShowReply = sInfo->ConfigSwitchValue == 0 ? 1 : 0;
    }

	/// 套利
	bool bShowSpread = true;
	if (g_pAccessApi->GetConfigSwitchInfo(S_CONFIGNO_LOCALSPREAD, &sInfo, 1, false) > 0)
	{
		bShowSpread = sInfo->ConfigSwitchValue == 0 ? 1 : 0;
	}

	/// 客户端止损
	bool bShowStop = true;
	if (g_pAccessApi->GetConfigSwitchInfo(S_CONFIGNO_STOP_PROFIT, &sInfo, 1, false) > 0)
	{
		bShowStop = sInfo->ConfigSwitchValue == 0 ? 1 : 0;
	}

    m_vecMenuItemVert.clear();
    m_vecMenuItemStrategy.clear();
    m_vecMenuItemInner.clear();
    m_vecMenuItemForeign.clear();

    //Normal
    m_vecMenuItemVert.push_back(wstring(g_pLanguage->LangText(IDS_VertOrder)));
    if (bShowLadder)
    {
        m_vecMenuItemVert.push_back(wstring(g_pLanguage->LangText(IDS_LadderOrder)));
    }
    m_vecMenuItemVert.push_back(wstring(g_pLanguage->LangText(IDS_OptionHead)));

    //策略
	if (bShowSpread)
	{
		m_vecMenuItemStrategy.push_back(wstring(g_pLanguage->LangText(IDS_STRING1178)));
		m_vecMenuItemStrategy.push_back(wstring(g_pLanguage->LangText(IDS_STRING1179)));
	}
    
    if (bShowBatchOrder)
        m_vecMenuItemStrategy.push_back(wstring(g_pLanguage->LangText(IDS_BatchOrder)));
    
	if(bShowStop)
		m_vecMenuItemStrategy.push_back(wstring(g_pLanguage->LangText(IDS_LocalStop)));

    //内盘
    m_vecMenuItemInner.push_back(wstring(g_pLanguage->LangText(IDS_Condition)));
    m_vecMenuItemInner.push_back(wstring(g_pLanguage->LangText(IDS_PreOrder)));
    m_vecMenuItemInner.push_back(wstring(g_pLanguage->LangText(IDS_AutoOrder)));
    m_vecMenuItemInner.push_back(wstring(g_pLanguage->LangText(IDS_SWAP)));
    m_vecMenuItemInner.push_back(wstring(g_pLanguage->LangText(IDS_LIMIT_STOP_PRICE)));
    m_vecMenuItemInner.push_back(wstring(g_pLanguage->LangText(IDS_STOP_PRICE)));
    if (bShowOption)
    {
        m_vecMenuItemInner.push_back(wstring(g_pLanguage->LangText(IDS_EXECUTE_ABANDON)));
        m_vecMenuItemInner.push_back(wstring(g_pLanguage->LangText(IDS_ASK_PRICE)));
        m_vecMenuItemInner.push_back(wstring(g_pLanguage->LangText(IDS_REPLY_PRICE)));
    }
    m_vecMenuItemInner.push_back(wstring(g_pLanguage->LangText(IDS_ARBAPP)));
    m_vecMenuItemInner.push_back(wstring(g_pLanguage->LangText(IDS_HEDGEAPP)));
    if (bShowOption)
    {
        m_vecMenuItemInner.push_back(wstring(g_pLanguage->LangText(IDS_Auto_Close)));
    }

    //外盘
    if (bShowIce)
    {
        m_vecMenuItemForeign.push_back(wstring(g_pLanguage->LangText(IDS_ICEBERG)));
    }
    if (bShowShadow)
    {
        m_vecMenuItemForeign.push_back(wstring(g_pLanguage->LangText(IDS_SHADOW)));
    }
    m_vecMenuItemForeign.push_back(wstring(g_pLanguage->LangText(IDS_LIMIT_STOP_PRICE)));
    m_vecMenuItemForeign.push_back(wstring(g_pLanguage->LangText(IDS_STOP_PRICE)));
    if (bShowOption)
    {
        m_vecMenuItemForeign.push_back(wstring(g_pLanguage->LangText(IDS_ASK_PRICE)));
    }

    UpdateMenuItem();
    m_bCompleted = true;
}

void CMouseOrderPopWin::OnInitProgress(const int ErrorCode, const TradeInitProgress CurrProgress, const TLoginBase * pLoginBase)
{
    if (ErrorCode == 0 && CurrProgress == tipReady)
    {
        bool bUpdate = false;
        bool bTraderLogined = CConvert::IsTraderLogined();
        if (bTraderLogined && m_vecMenuItemStrategy.size() > 0)
        {
            m_vecMenuItemStrategy.clear();
            bUpdate = true;
        }
        int nType = CConvert::ReadVersionType();
        if (m_nLoginType != TYPE_Both && m_nLoginType != nType)
        {
            m_nLoginType = m_nLoginType | nType;
            bUpdate = true;
        }

        if (bUpdate)
        {
            UpdateMenuItem();
        }
    }
}

void CMouseOrderPopWin::OnHotKeyDown(const bool Alt, const bool Ctrl, const bool Shift, const unsigned char VKey, const bool Exclusive, HWND source)
{
    if (VKey == VK_F12)
    {
        if (IsWindowVisible(m_Hwnd))
        {
            ShowPopWin();
        }
        else
        {
            TDataContainer tUsers;
            g_pTradeData->GetAllUser(tUsers, "");
            if (tUsers.size() == 0)
            {
                if (g_mainframe)
                    g_mainframe->Linkage(m_Hwnd, _MouseOrder, W_USER_LOGIN, "");
            }
            else
            {
                ShowPopWin();
            }
        }
    }
}

LRESULT CMouseOrderPopWin::OnMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CLOSE:
        ShowWindow(g_MousePopwin.GetHwnd(), SW_HIDE);
        return PROCESSED;
    default:
        return NOT_PROCESSED;
    }
}

void CMouseOrderPopWin::OnPaint(const HDC hdc, const RECT& cr, const RECT& ur)
{
	//画背景
	FillRect(hdc, &cr, g_ColorRefData.GetHbrBackground());

	//拖动
	LOGBRUSH logbrush;
	logbrush.lbColor = g_ColorRefData.GetColorRGB150();
	logbrush.lbStyle = PS_SOLID;
	logbrush.lbHatch = 1;
	HPEN hpen = ExtCreatePen(PS_GEOMETRIC | PS_DOT, 1, &logbrush, 0, NULL);
	HPEN hold = (HPEN)SelectObject(hdc, hpen);
	HBRUSH hbr = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
	for (int i = 0; i < 4; i++)
	{
		MoveToEx(hdc, 4, N_TITLE_HEIGHT - 1 - i * 2, NULL);
		LineTo(hdc, cr.right -cr.left - N_TITLE_HEIGHT -2, N_TITLE_HEIGHT - 1 - i * 2);
	}
	SelectObject(hdc, hold);
	SelectObject(hdc, hbr);
	DeleteObject(hpen);

	//关闭
	HPEN hpen2 = CreatePen(PS_SOLID, 1, m_bInCloseRect?g_ColorRefData.GetColorTextRed(): g_ColorRefData.GetColorRGB150());
	HGDIOBJ OldPen = SelectObject(hdc, hpen2);
	MoveToEx(hdc, cr.right - N_TITLE_HEIGHT, 4, NULL);
	LineTo(hdc, cr.right - 3, N_TITLE_HEIGHT+1);
	MoveToEx(hdc, cr.right - 5, 4, NULL);
	LineTo(hdc, cr.right - N_TITLE_HEIGHT-1, N_TITLE_HEIGHT);
	SelectObject(hdc, OldPen);
	DeleteObject(hpen2);

	//画边框
	RECT r(cr);	
	FrameRect(hdc, &r, g_ColorRefData.GetHbrRGB200());
	InflateRect(&r, -1, -1);
	FrameRect(hdc, &r, g_ColorRefData.GetHbrRGB150());
}

void CMouseOrderPopWin::OnLButtonDown(WPARAM wparam, LPARAM lparam)
{
	POINT pointB;
	pointB.x = LOWORD(lparam);
	pointB.y = HIWORD(lparam);
	if (PtInRect(&m_rtClose, pointB))
		ShowWindow(m_Hwnd, SW_HIDE);
	else
		TBaseFrame::OnLButtonDown(wparam, lparam);
}

void CMouseOrderPopWin::OnMouseMove(WPARAM wparam, LPARAM lparam)
{
	POINT pointM;
	pointM.x = LOWORD(lparam);
	pointM.y = HIWORD(lparam);
	bool bUpdate = false;
	//关闭按扭
	if (PtInRect(&m_rtClose, pointM))
	{
		if (!m_bInCloseRect)
			InvalidateRect(m_Hwnd, &m_rtClose, TRUE);
		m_bInCloseRect = true;
	}
	else
	{
		if (m_bInCloseRect)
			InvalidateRect(m_Hwnd, &m_rtClose, TRUE);
		m_bInCloseRect = false;
	}
	TBaseFrame::OnMouseMove(wparam, lparam);
}

void CMouseOrderPopWin::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	if (m_bInCloseRect)
	{
		InvalidateRect(m_Hwnd, &m_rtClose, TRUE);
		m_bInCloseRect = false;
	}
}

bool CMouseOrderPopWin::IsGlobalMousePop(HWND hwnd) 
{
	return m_gmouseopr && (m_gmouseopr->GetHwnd() == hwnd);
}

void CMouseOrderPopWin::LoadProperty(char(&cProperty)[MAX_PATH])
{
	FILE *File = NULL;
	errno_t err = fopen_s(&File, m_strPopwinProperty, "rb");
	if (!err)
	{
		fseek(File, 0, SEEK_END);
		int fileSize = ftell(File);
		fseek(File, 0, SEEK_SET);
		fread(&cProperty, fileSize, 1, File);//读取文件头
		fclose(File);
	}
}

void CMouseOrderPopWin::SaveProperty(char(&cProperty)[MAX_PATH])
{
	FILE *File = NULL;
	errno_t err = fopen_s(&File, m_strPopwinProperty, "wb");
	if (err)	return;

	fwrite((void*)&cProperty, sizeof(cProperty), 1, File);
	fclose(File);
}

void CMouseOrderPopWin::UpdateMenuItem()
{
    TMenuItemEx itemList[40];
    
    unsigned int menuorder = MAKE_MENUORDER(0, 10001, 8);

    int n = 0;
    for (int i = 0; i < (int)m_vecMenuItemVert.size(); i++)
    {
        TMenuItemEx item = { 0 };
        item.MenuOrder = menuorder;
        item.SubOrder = 0;
        wcscpy_s(item.ClassName, L"MouseOrder");
        strcpy_s(item.sAction, W_ON_MENUITEM);
        strcpy_s(item.sContent, LoadRC::unicode_to_ansi(m_vecMenuItemVert.at(i).c_str()).c_str());
        wcscpy_s(item.MenuText, m_vecMenuItemVert.at(i).c_str());
        itemList[n++] = item;
    }

    for (int i = 0; i < (int)m_vecMenuItemStrategy.size(); i++)
    {
        TMenuItemEx item = { 0 };
        item.MenuOrder = menuorder;
        item.SubOrder = 1;
        wcscpy_s(item.ClassName, L"MouseOrder");
        strcpy_s(item.sAction, W_ON_MENUITEM);
        strcpy_s(item.sContent, LoadRC::unicode_to_ansi(m_vecMenuItemStrategy.at(i).c_str()).c_str());
        wcscpy_s(item.MenuText, m_vecMenuItemStrategy.at(i).c_str());
        itemList[n++] = item;
    }

    if (m_nLoginType & TYPE_Inner)
    {
        for (int i = 0; i < (int)m_vecMenuItemInner.size(); i++)
        {
            TMenuItemEx item = { 0 };
            item.MenuOrder = menuorder;
            item.SubOrder = 2;
            wcscpy_s(item.ClassName, L"MouseOrder");
            strcpy_s(item.sAction, W_ON_MENUITEM);
            strcpy_s(item.sContent, LoadRC::unicode_to_ansi(m_vecMenuItemInner.at(i).c_str()).c_str());
            wcscpy_s(item.MenuText, m_vecMenuItemInner.at(i).c_str());
            itemList[n++] = item;
        }
    }

    if (m_nLoginType & TYPE_For)
    {
        for (int i = 0; i < (int)m_vecMenuItemForeign.size(); i++)
        {
            TMenuItemEx item = { 0 };
            item.MenuOrder = menuorder;
            item.SubOrder = 3;
            wcscpy_s(item.ClassName, L"MouseOrder");
            strcpy_s(item.sAction, W_ON_MENUITEM);
            strcpy_s(item.sContent, LoadRC::unicode_to_ansi(m_vecMenuItemForeign.at(i).c_str()).c_str());
            wcscpy_s(item.MenuText, m_vecMenuItemForeign.at(i).c_str());
            itemList[n++] = item;
        }
    }

    g_mainframe->AddMenuItemEx(MAKE_MENUORDER(0, 10001, 8), itemList, n);
}
