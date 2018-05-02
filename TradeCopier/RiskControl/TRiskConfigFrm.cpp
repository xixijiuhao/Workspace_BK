#include "PreInclude.h"

TRiskConfigFrm*  g_pRiskConfigFrm = NULL;

extern G_COLOREF g_ColorRefData;
extern G_UIFONT  g_FontData;

extern ITradeData* g_pTradeData;

extern RiskControlAPI      *g_pRiskControl;

#define WM_RISKCONTROL  (WM_USER+200)

#define  configFilePath  ".\\config\\RiskConfigFrm.pri"
#define  iniAppName      "RiskConfig"

#define widgetHgap  10
#define cbWidth     80
#define cbLeft      275
#define cbRight     (cbLeft + cbWidth)

#define rowNum      5
#define rowGap      35
#define widgetHeight 20
#define widgetTop    30

#define ckLeft      7
#define ckWidth     120
#define ckRight     (ckLeft + ckWidth)

#define editLeft    (ckRight+10)
#define editWidth   40
#define editRight   (editLeft + editWidth)

#define textAfterEditLeft (editRight+5)
#define textAfterEditWidth 18
#define textAfterEditRight (textAfterEditLeft + textAfterEditWidth)

#define textBefotreCbLeft  (textAfterEditRight+25)
#define textBeforeCbWidth  50
#define textBefotreCbLeftRight (textBefotreCbLeft +textBeforeCbWidth)

#define spinLeft     (cbRight +20)
#define spinRight    (spinLeft+editWidth)

#define textAfterSpinLeft  (spinRight +5)
#define textAfterSpinRight (textAfterSpinLeft + 30)

#define btnLeft    140
#define btnWidth   80
#define btnHeight  25

#define chapterPeroid 15 
#define secondsPerMinute 60

bool    g_bIsRiskRun = false;

TRiskConfigFrm::TRiskConfigFrm()
{
	for (int i = 0; i < 5; i++)
	{
		m_stDefaultData.Choose[i] = true;
		m_stDefaultData.time_m[i] = 0;
	}
	m_stDefaultData.consecutive_loss_count = 5;
	m_stDefaultData._Rc_action[0] = RC_ACTION_STOP_CHARPTER;
	m_stDefaultData.s_consecutive_loss_count = 3;
	m_stDefaultData._Rc_action[1] = RC_ACTION_STOP_CHARPTER;
	m_stDefaultData.max_loss_percent = 1.0;
	m_stDefaultData._Rc_action[2] = RC_ACTION_STOP_WHOLE_DAY;
	m_stDefaultData.max_single_loss_percent = 0.5;
	m_stDefaultData._Rc_action[3] = RC_ACTION_STOP_HALF_DAY;
	m_stDefaultData.max_drawdown = 0.5;
	m_stDefaultData._Rc_action[4] = RC_ACTION_STOP_WHOLE_DAY;
	strcpy_s(m_szCbText[0], "一刻");
	strcpy_s(m_szCbText[1], "半天");
	strcpy_s(m_szCbText[2], "全天");
	strcpy_s(m_szCbText[3], "任意");

	m_hbrStatic = CreateSolidBrush(RGB(255, 255, 255));
}
TRiskConfigFrm::~TRiskConfigFrm()
{
	DeleteObject(m_hbrStatic);
}
HWND TRiskConfigFrm::create_frame(const wchar_t * sub_title, const HWND parent)
{
	g_pRiskConfigFrm = new TRiskConfigFrm;
	g_pRiskConfigFrm->CreateFrm(L"TestConfigFrame", parent, WS_CHILD | WS_VISIBLE|WS_CLIPSIBLINGS);
	return g_pRiskConfigFrm->m_Hwnd;
}

void TRiskConfigFrm::OnCreate()
{
	char  ckName[][20] = { "连续亏损", "同方向连续亏损", "最大总亏损", "最大单笔亏损", "最大盈利回撤" };
	
	UINT editStyle[5] = { TEdit::TEDIT_INT, TEdit::TEDIT_INT, TEdit::TEDIT_FLOAT, TEdit::TEDIT_FLOAT, TEdit::TEDIT_FLOAT };
	
	RECT rcEdit = { 0 };
	rcEdit.left = editLeft; rcEdit.right = editRight;
	RECT rcSpin;
	rcSpin.left = spinLeft;//combobox右边15个像素
	rcSpin.right = spinRight;
	int top = 0;
	for (int i = 0; i < rowNum; i++)
	{
		top = widgetTop + rowGap*i;
		m_ckAction[i].Create(m_Hwnd, ckName[i], TCheckBox::TCS_CHECK_HOOK, i);  //check box
		m_ckAction[i].SetFont(g_FontData.g_FontWord12);
		m_ckAction[i].MoveWindow(ckLeft, top, ckWidth, widgetHeight);
		if (i % 2)
			m_ckAction[i].SetBkColor(RGB(238, 238, 238));
		else
			m_ckAction[i].SetBkColor(RGB(255, 255, 255));
		m_cbCloseTime[i].Create(m_Hwnd, i);

		m_cbCloseTime[i].MoveWindow(cbLeft, top, cbWidth, widgetHeight); //combobox
		for (int j = 0; j < 4; j++)
		{
			m_cbCloseTime[i].AddString(m_szCbText[j]);
		}
		rcSpin.top = top;
		rcSpin.bottom = rcSpin.top + widgetHeight;
		TEdit::CreateEdit(m_Hwnd, m_peditMinutes[i], rcSpin, TEdit::TEDIT_INT); //选择分钟
		m_peditMinutes[i].SetFont(g_FontData.g_FontNum13);

		rcEdit.top = widgetTop + i*rowGap;
		rcEdit.bottom = rcEdit.top + widgetHeight;
		TEdit::CreateEdit(m_Hwnd, m_editParam[i], rcEdit, editStyle[i], ES_AUTOHSCROLL); //创建edit
		m_editParam[i].SetFont(g_FontData.g_FontNum13);
	}
	RECT rcBtn;
	rcBtn.left = 180; rcBtn.right = rcBtn.left + btnWidth;
	rcBtn.top = top + rowGap + 20; rcBtn.bottom = rcBtn.top + btnHeight;
	TStaticButton::CreateButton(m_Hwnd, m_btnExecute[0], rcBtn, "加载默认", id_btnDefault);
	rcBtn.left += 100; rcBtn.right = rcBtn.left + btnWidth;
	TStaticButton::CreateButton(m_Hwnd, m_btnExecute[1], rcBtn, "应用", id_btnApply);
	rcBtn.left += 100; rcBtn.right = rcBtn.left + btnWidth;
	TStaticButton::CreateButton(m_Hwnd, m_btnExecute[2], rcBtn, "取消风控", id_cancelRisk);

	m_riskState.Create(m_Hwnd, L"风控未运行.",100);
	m_riskState.MoveWindow(30, rcBtn.top, 100, rcBtn.bottom - rcBtn.top);
	if (g_bIsRiskRun)
		m_riskState.SetText("风控运行中...");
	Init();
}
void TRiskConfigFrm::Init()
{
	ReadData(m_stRiskData);
	SetData(m_stRiskData);
	for (int i = 0; i < rowNum; i++)
	{
		if (!m_ckAction[i].GetValue())
			Enable(false, i);
	}
}
void TRiskConfigFrm::SetData(Rc_ui_input_para &data)
{
	for (UINT i = 0; i < rowNum; i++)
	{
		m_ckAction[i].SetValue(data.Choose[i]);
		if (data.Choose[i])
			Enable(true, i);
		m_peditMinutes[i].SetWindowTextW(data.time_m[i]);
		m_cbCloseTime[i].SetSelect(data._Rc_action[i] -1);
	}
	m_editParam[0].SetWindowTextW(data.consecutive_loss_count);
	m_editParam[1].SetWindowTextW(data.s_consecutive_loss_count);
	m_editParam[2].SetWindowTextW(data.max_loss_percent);
	m_editParam[3].SetWindowTextW(data.max_single_loss_percent);
	m_editParam[4].SetWindowTextW(data.max_drawdown);
}
int TRiskConfigFrm::GetSelectIndex(UINT iCombox)
{
	if (iCombox > rowNum)
		return -1;
	char *pText = m_cbCloseTime[iCombox].GetText();
	if (pText == NULL)
		return -1;
	for (UINT i = 0; i < sizeof(m_szCbText) / sizeof(m_szCbText[0]); i++)
	{
		if (strcmp(pText, m_szCbText[i]) == 0)
		{
			return i;
		}
	}
	return -1;
}
void TRiskConfigFrm::GetData(Rc_ui_input_para &data)
{
	for (UINT i = 0; i < rowNum; i++)
	{
		data.Choose[i] = m_ckAction[i].GetValue();
		data._Rc_action[i] = GetCbSelect(i);
		m_peditMinutes[i].GetWindowNumber(data.time_m[i]);
	}
	m_editParam[0].GetWindowNumber(data.consecutive_loss_count);
	m_editParam[1].GetWindowNumber(data.s_consecutive_loss_count);
	char szText[100] = { 0 };
	vector<double>vecEditData;
	for (UINT i = 2; i < rowNum; i++)
	{
		m_editParam[i].GetWindowTextW(szText, sizeof(szText));
		vecEditData.push_back(atof(szText));
	}
	data.max_loss_percent = vecEditData[0];
	data.max_single_loss_percent = vecEditData[1];
	data.max_drawdown = vecEditData[2];
}
RC_ACTION TRiskConfigFrm::GetCbSelect(UINT index)
{
	if (index > rowNum)
		return RC_ACTION_NONE;
	char *pText = m_cbCloseTime[index].GetText();
	if (pText == NULL)
		return RC_ACTION_NONE;
	if (strcmp(pText, m_szCbText[0]) == 0)
		return RC_ACTION_STOP_CHARPTER;
	else if (strcmp(pText, m_szCbText[1]) == 0)
		return RC_ACTION_STOP_HALF_DAY;
	else if (strcmp(pText, m_szCbText[2]) == 0)
		return RC_ACTION_STOP_WHOLE_DAY;
	else if (strcmp(pText, m_szCbText[3]) == 0)
		return RC_ACTION_STOP_ANY_MIN;
	else
		return RC_ACTION_NONE;
}
void TRiskConfigFrm::Enable(bool bEnable, int index)
{
	if (index < 0 || index > rowNum)
		return;
	m_editParam[index].EnableEdit(bEnable);
	EnableWindow(m_cbCloseTime[index].GetHwnd(), bEnable);
	if (!bEnable)
		m_peditMinutes[index].EnableEdit(false);
	else
		EnableSpinEdit(index);
}
void TRiskConfigFrm::SaveData(Rc_ui_input_para &data)
{
	int ckValue[5] = { 0 };
	int cbIndex[5] = { 0 };
	for (UINT i = 0; i < rowNum; i++)
	{
		if (data.Choose[i])
			ckValue[i] = 1;
		else
			ckValue[i] = 0;
		cbIndex[i] = data._Rc_action[i];
	}
	char szKeyName[10] = { 0 };
	char szString[100] = { 0 };
	sprintf_s(szKeyName, "%d", filed_ck);
	sprintf_s(szString, "%d,%d,%d,%d,%d", ckValue[0], ckValue[1], ckValue[2], ckValue[3], ckValue[4]);
	WritePrivateProfileStringA(iniAppName, szKeyName, szString, configFilePath);//ck value

	sprintf_s(szKeyName, "%d", filed_cb);
	sprintf_s(szString, "%d,%d,%d,%d,%d", cbIndex[0], cbIndex[1], cbIndex[2], cbIndex[3], cbIndex[4]);
	WritePrivateProfileStringA(iniAppName, szKeyName, szString, configFilePath); // cb index

	sprintf_s(szKeyName, "%d", filed_spin);
	sprintf_s(szString, "%d,%d,%d,%d,%d", data.time_m[0], data.time_m[1], data.time_m[2], data.time_m[3], data.time_m[4]);
	WritePrivateProfileStringA(iniAppName, szKeyName, szString, configFilePath); // minute

	sprintf_s(szKeyName, "%d", filed_clc);
	sprintf_s(szString, "%d", data.consecutive_loss_count);
	WritePrivateProfileStringA(iniAppName, szKeyName, szString, configFilePath);// clc

	sprintf_s(szKeyName, "%d", filed_sclc);
	sprintf_s(szString, "%d", data.s_consecutive_loss_count);
	WritePrivateProfileStringA(iniAppName, szKeyName, szString, configFilePath);//sclc

	sprintf_s(szKeyName, "%d", filed_mlp);
	sprintf_s(szString, "%.2f", data.max_loss_percent);
	WritePrivateProfileStringA(iniAppName, szKeyName, szString, configFilePath);

	sprintf_s(szKeyName, "%d", filed_mslp);
	sprintf_s(szString, "%.2f", data.max_single_loss_percent);
	WritePrivateProfileStringA(iniAppName, szKeyName, szString, configFilePath);

	sprintf_s(szKeyName, "%d", filed_md);
	sprintf_s(szString, "%.2f", data.max_drawdown);
	WritePrivateProfileStringA(iniAppName, szKeyName, szString, configFilePath);

}
void TRiskConfigFrm::ReadData(Rc_ui_input_para &data)
{
	char szKeyName[10] = { 0 };
	szKeyName[0] = '0' + filed_ck;
	char szString[100] = { 0 };
	vector<int> Value;
	if (GetPrivateProfileStringA(iniAppName, szKeyName, "", szString, sizeof(szString), configFilePath))
	{
		SplitString(szString, Value);
		for (UINT i = 0; i < Value.size(); i++)
			data.Choose[i] = Value[i] == 1;
	}
	szKeyName[0] = '0' + filed_cb;
	if (GetPrivateProfileStringA(iniAppName, szKeyName, "", szString, sizeof(szString), configFilePath))
	{
		SplitString(szString, Value);
		for (UINT i = 0; i < Value.size(); i++)
			data._Rc_action[i] = (RC_ACTION)Value[i];
	}
	szKeyName[0] = '0' + filed_spin;
	if (GetPrivateProfileStringA(iniAppName, szKeyName, "", szString, sizeof(szString), configFilePath))
	{
		SplitString(szString, Value);
		for (UINT i = 0; i < Value.size(); i++)
			data.time_m[i] = Value[i];
	}
	szKeyName[0] = '0' + filed_clc;
	if (GetPrivateProfileStringA(iniAppName, szKeyName, "", szString, sizeof(szString), configFilePath))
	{
		data.consecutive_loss_count = atoi(szString);
	}
	szKeyName[0] = '0' + filed_sclc;
	if (GetPrivateProfileStringA(iniAppName, szKeyName, "", szString, sizeof(szString), configFilePath))
	{
		data.s_consecutive_loss_count = atoi(szString);
	}
	szKeyName[0] = '0' + filed_mlp;
	if (GetPrivateProfileStringA(iniAppName, szKeyName, "", szString, sizeof(szString), configFilePath))
	{
		data.max_loss_percent = atof(szString);
	}
	szKeyName[0] = '0' + filed_mslp;
	if (GetPrivateProfileStringA(iniAppName, szKeyName, "", szString, sizeof(szString), configFilePath))
	{
		data.max_single_loss_percent = atof(szString);
	}
	szKeyName[0] = '0' + filed_md;
	if (GetPrivateProfileStringA(iniAppName, szKeyName, "", szString, sizeof(szString), configFilePath))
	{
		data.max_drawdown = atof(szString);
	}
}
void TRiskConfigFrm::SplitString(char*szString, vector<int>&data)
{
	data.clear();
	string tempStr = szString;
	string::size_type isplit = tempStr.find(',');
	while (string::npos != isplit)
	{
		string strtmp = tempStr.substr(0, isplit++);
		tempStr = tempStr.substr(isplit, string::npos);
		isplit = tempStr.find(',');
		data.push_back(atoi(strtmp.c_str()));
	}
	data.push_back(atoi(tempStr.c_str()));
}
LRESULT TRiskConfigFrm::WndProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CREATE:
		OnCreate();
		return PROCESSED;
	case WM_PAINT:
		OnPaint();
		return PROCESSED;
	case WM_COMMAND:
		OnCommand(wParam, lParam);
		return PROCESSED;
	case SSWM_COMBOX_SELECT:
		OnCbSelect(wParam, lParam);
		return PROCESSED;
	case SSWM_STATIC_BUTTON_CLICKDOWN:
		OnClickBtn(wParam, lParam);
		return PROCESSED;
	case WM_DESTROY:
		OnDestroy();
		return PROCESSED;
	case WM_NCDESTROY:
		delete this;
		return PROCESSED;
	case WM_CTLCOLORSTATIC:
		SetTextColor(HDC(wParam), RGB(51, 136, 255));
		return (LRESULT)m_hbrStatic;
	default:
		return NOT_PROCESSED;
	}
}
void TRiskConfigFrm::OnDestroy()
{
	GetData(m_stRiskData);
	SaveData(m_stRiskData);
	(TDealRisk::Instance())->SetData(m_stRiskData);
}
void TRiskConfigFrm::OnClickBtn(WPARAM wParm, LPARAM lParam)
{
	if (g_pRiskControl == NULL)
		return;
	if (lParam == id_btnApply)
	{
		GetData(m_stRiskData);
		TDataContainer user;
		g_pTradeData->GetAllUser(user);
		if (user.size())
		{ 
			g_pRiskControl->Rc_start(&m_stRiskData);
			g_bIsRiskRun = true;
			m_riskState.SetText("风控运行中...");
		}
		else
			m_riskState.SetText("用户未登录！");
	}
	else if (lParam == id_btnDefault)
	{
		SetData(m_stDefaultData);
	}
	else if (lParam == id_cancelRisk)
	{
		g_pRiskControl->Rc_stop();
		g_bIsRiskRun = false;
		m_riskState.SetText("已停止风控.");
	}
}
void TRiskConfigFrm::EnableSpinEdit(int index)
{
	char* pszText = m_cbCloseTime[index].GetText();
	if (strcmp(pszText, "任意")==0)
		m_peditMinutes[index].EnableEdit(true);
	else
		m_peditMinutes[index].EnableEdit(false);
}
void TRiskConfigFrm::OnCbSelect(WPARAM wParam, LPARAM lParam)
{
	int iCount = sizeof(m_szCbText) / sizeof(m_szCbText[0]);
	for (int i = 0; i <iCount ; i++)
	{
		if ((HWND)lParam == m_cbCloseTime[i].GetHwnd())
		{
			EnableSpinEdit(i);
			break;
		}
	}
}
void TRiskConfigFrm::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if (LOWORD(wParam) == BN_CLICKED)
	{
		for (int i = 0; i < rowNum; i++)
		{
			if ((HWND)lParam == m_ckAction[i].GetHwnd())
			{
				Enable(m_ckAction[i].GetValue(), i);
				break;
			}
		}
	}
}
void TRiskConfigFrm::OnPaint()
{
	TMemDC memdc(m_Hwnd);
	RECT rect;
	SetBkMode(memdc.GetHdc(), TRANSPARENT);
	GetClientRect(m_Hwnd, &rect);
	HBRUSH hbr = CreateSolidBrush(RGB(255, 255, 255));
	FillRect(memdc.GetHdc(), &rect, hbr);
	DrawGrid(&memdc);
	DrawText(&memdc);
//	DrawHeader(&memdc);
	DeleteObject(hbr);
}
void TRiskConfigFrm::DrawGrid(TMemDC *pMemDC)
{
	HBRUSH hbrWhite = CreateSolidBrush(RGB(255, 255, 255));
	HBRUSH hbrGray = CreateSolidBrush(RGB(238, 238, 238));
	RECT rc = { ckLeft-2, widgetTop, textAfterSpinRight + 10, 0 };
	for (int i = 0; i < rowNum; i++)
	{
		rc.top = widgetTop + rowGap*i-7;
		rc.bottom = rc.top + rowGap;
		if (i % 2)
			FillRect(pMemDC->GetHdc(), &rc, hbrGray);
		else
			FillRect(pMemDC->GetHdc(), &rc, hbrWhite);
	}
	DeleteObject(hbrWhite);
	DeleteObject(hbrGray);
}
void TRiskConfigFrm::DrawTextW(TMemDC *pMemDC)
{
	RECT rcAfterEdit; //Edit后面的文字;
	rcAfterEdit.left = textAfterEditLeft;
	rcAfterEdit.right = textAfterEditRight;
	char  szText[][10] = { "笔", "笔", "%", "%", "%" };
	SelectObject(pMemDC->GetHdc(), g_FontData.g_FontWord12);
	RECT rcBeforeCb;
	rcBeforeCb.left = textBefotreCbLeft;
	rcBeforeCb.right = textBefotreCbLeftRight;
	RECT rcMinute = { 0 };
	rcMinute.left = textAfterSpinLeft;
	rcMinute.right = textAfterSpinRight;
	for (int i = 0; i < rowNum; i++)
	{
		rcAfterEdit.top = widgetTop + rowGap*i;
		rcAfterEdit.bottom = rcAfterEdit.top + widgetHeight;
		DrawTextA(pMemDC->GetHdc(), szText[i], -1, &rcAfterEdit, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		rcBeforeCb.top = rcAfterEdit.top;
		rcBeforeCb.bottom = rcAfterEdit.bottom;
		DrawTextA(pMemDC->GetHdc(), "暂停交易", -1, &rcBeforeCb, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		rcMinute.top = rcAfterEdit.top;
		rcMinute.bottom = rcAfterEdit.bottom;
		DrawTextA(pMemDC->GetHdc(), "分钟", -1, &rcMinute, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	}
}


void TRiskConfigFrm::DrawHeader(TMemDC *pMemDC)
{

}



TLockWindow::TLockWindow(HWND hwnd, char* szTitle, int lockSecond) :TModalWnd(hwnd,szTitle)
{
	m_iLockSecond = lockSecond;
	SetTimeText();
}
TLockWindow::~TLockWindow()
{

}
void TLockWindow::CreateSubCtrl()
{

}
void TLockWindow::SetModalRect()
{
	MoveWindow(m_Hwnd, (GetSystemMetrics(SM_CXSCREEN) - 300) / 2, (GetSystemMetrics(SM_CYSCREEN) - 150) / 2, 300, 150, true);
}
void TLockWindow::SetInfo(RC_RULE rcRule, Rc_ui_input_para &data)
{
	SetEscClose(false);  //不需要ESC关闭窗口功能
	m_rcRule = rcRule;
	memcpy(&m_stRiskData, &data, sizeof(Rc_ui_input_para));
	char szText1[30] = { 0 };
	switch (rcRule)
	{
	case RC_RULE_CONSECUTIVE_LOSS:
		sprintf_s(szText1, "连续亏损%d笔", data.consecutive_loss_count);
		break;
	case RC_RULE_S_CONSECUTIVE_LOSS:
		sprintf_s(szText1, "同方向连续亏损%d笔", data.s_consecutive_loss_count);
		break;
	case RC_RULE_MAX_LOSS:
		sprintf_s(szText1, "最大亏损超过%.2f%%", data.max_loss_percent);
		break;
	case RC_RULE_MAX_SINGLE_LOSS:
		sprintf_s(szText1, "最大单笔亏损超过%.2f%%", data.max_single_loss_percent);
		break;
	case RC_RULE_MAX_DRAWDOWN:
		sprintf_s(szText1, "最大盈利回撤超过%.2f%%", data.max_drawdown);
		break;
	default:
		szText1[0] = '\0';
		break;
	}
	char szText2[20] = {0};
	switch (data._Rc_action[rcRule - 1])
	{
	case RC_ACTION_STOP_WHOLE_DAY:
		strcpy_s(szText2, "交易暂停一天");
		break;
	case RC_ACTION_STOP_ANY_MIN:
		sprintf_s(szText2, "交易暂停%d分钟", data.time_m[rcRule - 1]);
		break;
	case RC_ACTION_STOP_CHARPTER:
		strcpy_s(szText2, "交易暂停一刻");
		break;
	case RC_ACTION_STOP_HALF_DAY:
		strcpy_s(szText2, "交易暂停半天");
		break;
	default:
		szText2[0] = '\0';
	}
	sprintf_s(m_strInfo, "%s, %s", szText1, szText2);
}
void TLockWindow::DrawMain(TMemDC* pmemdc)
{
	RECT rc = { 0 };
	GetClientRect(m_Hwnd, &rc);
	rc.top = CAPTION_HEIGHT;
	rc.top += 30;
	rc.left = 10;
	rc.bottom = rc.top + 20;
	SetTextColor(pmemdc->GetHdc(), RGB(255, 0, 0));
	DrawTextA(pmemdc->GetHdc(), m_strInfo, -1, &rc, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	rc.top += 25;
	rc.bottom += 25;
	DrawTextA(pmemdc->GetHdc(), "剩余时间: ", -1, &rc, DT_VCENTER | DT_SINGLELINE | DT_LEFT);
	rc.left += 72;
	DrawTextA(pmemdc->GetHdc(), m_lastTime, -1, &rc, DT_VCENTER | DT_SINGLELINE|DT_LEFT);
}
void TLockWindow::OnCreate()
{
	SetTimer(m_Hwnd, 1, 1000, 0);
}
void TLockWindow::OnTimer()
{
	if (m_iLockSecond > 0)
	{
		m_iLockSecond--;
		SetTimeText();
		InvalidateRect(m_Hwnd, 0, true);
	}
	else
	{
		KillTimer(m_Hwnd, 1);
		DestroyWindow(m_Hwnd);
	}
}
void TLockWindow::SetTimeText()
{
	m_lastTime[0] = m_iLockSecond / 36000 + 0x30;
	m_lastTime[1] = m_iLockSecond % 36000 / 3600 + 0x30;
	m_lastTime[2] = ':';
	m_lastTime[3] = m_iLockSecond % 3600 / 600 + 0x30;
	m_lastTime[4] = m_iLockSecond % 600 / 60 + 0x30;
	m_lastTime[5] = ':';
	m_lastTime[6] = m_iLockSecond % 60 / 10 + 0x30;
	m_lastTime[7] = m_iLockSecond % 10 + 0x30;
	m_lastTime[8] = '\0';
}
LRESULT	TLockWindow::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		OnCreate();
		return PROCESSED;
	case WM_TIMER:
		OnTimer();
		return PROCESSED;
	case WM_LBUTTONUP:
		return PROCESSED;
	default:
		return __super::WndProc(message, wParam, lParam);
	}
}


TDealRisk* TDealRisk::pDealRisk = NULL;
string TDealRisk::strNoticeContent = "";
void TDealRisk::Run()
{
	CreateFrm(L"TDealRiskFrm", NULL, 0);
}
TDealRisk::~TDealRisk()
{
	DestroyWindow(m_Hwnd);
}
TDealRisk *TDealRisk::Instance()
{
	if (pDealRisk == NULL)
	{
		pDealRisk = new TDealRisk;
		pDealRisk->Run();
	}
	return pDealRisk;
}
void TDealRisk::SetData(Rc_ui_input_para &data)
{
	memcpy(&m_stRiskParam, &data, sizeof(m_stRiskParam)); 
}
void TDealRisk::OnReceiveNotice(const char* sender, const char* action, const char* content)
{
	if (strcmp(sender, RC_NOTICE_SENDER) == 0 && content)
	{
		strNoticeContent = content;
		PostMessage(Instance()->GetHwnd(), WM_RISKCONTROL, 0, 0);
	}
}
LRESULT TDealRisk::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_RISKCONTROL:
		OnRiskControl();
		return PROCESSED;
	}
	return NOT_PROCESSED;
}

void TDealRisk::OnRiskControl()
{
	RC_RULE rcRule = RC_RULE_NONE;
	const char * pContent = strNoticeContent.c_str();
	if (strcmp(pContent, RC_RULE_PRE_CAUTION) == 0)
	{
		_TMessageBox_Domodal(m_Hwnd, "预警", L"请注意,已经快达到预警临界点！", TMB_OK);
	}
	else if (strcmp(pContent, RC_RULE_1) == 0)
	{
		rcRule = RC_RULE_CONSECUTIVE_LOSS;
	}
	else if (strcmp(pContent, RC_RULE_2) == 0)
	{
		rcRule = RC_RULE_S_CONSECUTIVE_LOSS;
	}
	else if (strcmp(pContent, RC_RULE_3) == 0)
	{
		rcRule = RC_RULE_MAX_LOSS;
	}
	else if (strcmp(pContent, RC_RULE_4) == 0)
	{
		rcRule = RC_RULE_MAX_SINGLE_LOSS;
	}
	else if (strcmp(pContent, RC_RULE_5) == 0)
	{
		rcRule = RC_RULE_MAX_DRAWDOWN;
	}
	if (rcRule != RC_RULE_NONE)
	{
		int  stopSeconds = DealRisk(rcRule, m_stRiskParam);
		if (stopSeconds > 0)
		{
			HWND hwnd = GetActiveWindow();
			TLockWindow lock(hwnd, "警告", stopSeconds);
			lock.SetInfo(rcRule, m_stRiskParam);
			lock.Domodal();
		}
	}
}
int TDealRisk::DealRisk(RC_RULE rcRule, Rc_ui_input_para &data)
{
	RC_ACTION rcAction = data._Rc_action[rcRule - 1];
	int stopSeconds = -1;
	SYSTEMTIME stTime;
	GetLocalTime(&stTime);
	switch (rcAction)
	{
	case RC_ACTION_STOP_CHARPTER:  //15 minutes
		stopSeconds = chapterPeroid *secondsPerMinute;
		break;
	case RC_ACTION_STOP_HALF_DAY://停半天，晚上的话，停晚上，白天的话停白天
		if (stTime.wHour >= 21 && stTime.wHour < 24)//
			stopSeconds = (((23 - stTime.wHour) * 60 + 60 - stTime.wMinute)*secondsPerMinute - stTime.wSecond) + 150 * 60;//后面的是后两个半小时
		else if (stTime.wHour >= 0 && stTime.wHour < 3 && !(stTime.wHour == 2 && stTime.wMinute > 30))//00:00 -02:30之间
			stopSeconds = 150 * 60 - (stTime.wHour * 60 + stTime.wMinute) * 60 - stTime.wSecond;
		else if (stTime.wHour >= 9 && stTime.wHour < 16)
			stopSeconds = 390 * 60 - ((stTime.wHour - 9) * 60 + stTime.wMinute) * 60 - stTime.wSecond;
		break;
	case RC_ACTION_STOP_WHOLE_DAY://任何时候触发，停全天
		if (stTime.wHour >= 21 && stTime.wHour < 24)//
			stopSeconds = (((24 - stTime.wHour) * 60 - stTime.wMinute)*secondsPerMinute - stTime.wSecond) + 15 * 60 * 60 + 30 * 60;
		else if (stTime.wHour < 16)
			stopSeconds = 15 * 60 * 60 + 30 * 60 - stTime.wHour * 60 * 60 - stTime.wMinute * 60 - stTime.wSecond;
		break;
	case RC_ACTION_STOP_ANY_MIN:
		stopSeconds = data.time_m[rcRule - 1] * secondsPerMinute;
		break;
	case RC_ACTION_NONE:
		break;
	default:
		break;
	}
	return stopSeconds;
}