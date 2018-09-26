#include "PreInclude.h"

extern ISpotSystemApi		*g_pSpotSystem;				//套保平台
extern G_COLOREF			g_ColorRefData;				//色彩
extern G_UIFONT				g_FontData;					//字体
#define V_TITLE				45							//抬头高度	

PartAccountWnd::PartAccountWnd()
	:m_System(nullptr)
{
	m_hQryFund = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hQryPosition = CreateEvent(NULL, TRUE, FALSE, NULL);
}

PartAccountWnd::~PartAccountWnd()
{

}

void PartAccountWnd::Create(LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle)
{
	CreateFrm(classname, parent, style, exstyle);
}

void PartAccountWnd::Register(SystemFrame * pdata)
{
	if (!pdata)
		return;
	m_System = pdata;
}

LRESULT PartAccountWnd::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		OnCreate();
		return PROCESSED;
	case WM_COMMAND:
		OnCommand(wParam, lParam);
		return PROCESSED;
		//case WM_NOTIFY:
		//	OnNotify(wParam, lParam);
		//	return PROCESSED;
	case SSWM_STATIC_BUTTON_CLICKDOWN:
		OnBtnClickDown(wParam, lParam);
		return PROCESSED;
	case WM_LIST_LBUTTONDBCLK:
		//OnListDoubleClick(wParam, lParam);
		return PROCESSED;
	case WM_SIZE:
		OnSize(wParam, lParam);
		return PROCESSED;
	case WM_DRAWITEM:
		OnDrawItem(wParam, lParam);
		return PROCESSED;
	case WM_MEASUREITEM://==7
		OnMeasureItem(wParam, lParam);
		return PROCESSED;
	case WM_CTLCOLORSTATIC:
		return OnCtlColor(wParam, lParam);
	case WM_PAINT:
		OnPaint();
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

void PartAccountWnd::OnCreate()
{
	InitCommonControls();
	m_hInstance = GetModuleHandle(NULL);

	//
	m_labDate.Create(m_Hwnd, L"指定日期", DT_RIGHT, ID_LabDate);
	m_TimePick = CreateWindowEx(0, DATETIMEPICK_CLASS, TEXT("DateTime"), WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | DTS_SHORTDATEFORMAT,
		0, 0, 150, 25, m_Hwnd, (HMENU)ID_CbtDate, m_hInstance, NULL);

	//查询
	m_btnQry.Create(m_Hwnd, "查  询", ID_BtnQry);
	//导入分账单
	m_btnRead.Create(m_Hwnd, "导入分账单", ID_BtnRead);
	//表格
	m_listPartAccount.CreateList(m_Hwnd, LVS_REPORT | LVS_OWNERDRAWFIXED | WS_BORDER, 25, 25, LVS_EX_FULLROWSELECT, ID_ListPart);

	/////////////////////////Init///////////////////////////
	m_listPartAccount.AddColumn(L"核对项目", 200, -1);
	m_listPartAccount.AddColumn(L"套保系统", 200, -1);
	m_listPartAccount.AddColumn(L"期货系统", 200, -1);
	m_listPartAccount.AddItem("今日权益", 0, 0, DT_CENTER, g_ColorRefData.GetColorBlack());
	m_listPartAccount.AddItem("今日可用", 1, 0, DT_CENTER, g_ColorRefData.GetColorBlack());
	m_listPartAccount.AddItem("盈亏总计", 2, 0, DT_CENTER, g_ColorRefData.GetColorBlack());
	m_listPartAccount.AddItem("平仓盈亏", 3, 0, DT_CENTER, g_ColorRefData.GetColorBlack());

	m_labDate.SetFont(g_FontData.GetFontWord13());
	//m_richeditAll.SetFont(g_FontData.GetFontWord15());

	m_cbtDate.Init(m_TimePick);
	SendMessage(m_TimePick, WM_SETFONT, (WPARAM)g_FontData.GetFontNum15(), (LPARAM)TRUE);
	DateTime_SetFormat(m_TimePick, L"yyyy-MM-dd");
	SYSTEMTIME time;
	GetLocalTime(&time);
	DateTime_SetSystemtime(m_cbtDate.GetHwnd(), GDT_VALID, &time);

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
	m_labDate.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_cbtDate.MoveWindow(pt.x, pt.y, editWidth, Height);
	pt.x += editWidth + htap * 3;
	m_btnQry.MoveWindow(pt.x, pt.y, btnWidth, Height);
	pt.x += btnWidth + htap * 3;
	m_btnRead.MoveWindow(pt.x, pt.y, btnWidth, Height);

	pt.x = 0; pt.y = V_TITLE;
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	m_listPartAccount.MoveWindow(pt.x, pt.y, rect.right - rect.left, rect.bottom - rect.top - V_TITLE);
}

void PartAccountWnd::OnPaint()
{
	TMemDC mdc(m_Hwnd);
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	FillRect(mdc.GetHdc(), &rect, g_ColorRefData.GetHbrBackground());

	HPEN  hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
	SelectObject(mdc.GetHdc(), hPen);
	MoveToEx(mdc.GetHdc(), rect.left, V_TITLE - 1, 0);
	LineTo(mdc.GetHdc(), rect.right, V_TITLE - 1);
	DeleteObject(hPen);
}

void PartAccountWnd::OnCommand(WPARAM wParam, LPARAM lParam)
{

}

LRESULT PartAccountWnd::OnCtlColor(WPARAM wParam, LPARAM lParam)
{
	SetBkMode((HDC)wParam, TRANSPARENT);
	return (LRESULT)g_ColorRefData.GetColorBackground();
}

void PartAccountWnd::OnNcDestroy()
{
}

void PartAccountWnd::OnDestroy()
{
	//if (IsWindow(m_addPartAccountWnd.GetHwnd()))
	//	DestroyWindow(m_addPartAccountWnd.GetHwnd());
}

void PartAccountWnd::OnSize(WPARAM wParam, LPARAM lParam)
{
	POINT pt;
	pt.x = 0; pt.y = V_TITLE;
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	m_listPartAccount.MoveWindow(pt.x, pt.y, rect.right - rect.left, rect.bottom - rect.top - V_TITLE);

}

void PartAccountWnd::OnBtnClickDown(WPARAM wParam, LPARAM lParam)
{
	switch (lParam)
	{
	case ID_BtnQry:
		OnBtnQry();
		break;
	case ID_BtnRead:
		OnBtnRead();
		break;
	default:
		break;
	}
}

void PartAccountWnd::OnDrawItem(WPARAM wParam, LPARAM lParam)
{
	LPDRAWITEMSTRUCT p = (LPDRAWITEMSTRUCT)lParam;
	switch (p->CtlID)
	{
	case ID_ListPart:
		m_listPartAccount.OnDrawItem(lParam);
		break;
	default:
		break;
	}
}

void PartAccountWnd::OnMeasureItem(WPARAM wParam, LPARAM lParam)
{
	LPMEASUREITEMSTRUCT p = (LPMEASUREITEMSTRUCT)lParam;
	m_listPartAccount.OnMeasureItem(p->itemHeight);
}

void PartAccountWnd::OnBtnQry()
{
	ResetEvent(m_hQryFund);
	ResetEvent(m_hQryPosition);
	//ResetEvent(m_hQry[0]);
	//ResetEvent(m_hQry[1]);

	//查成交
	SmsMatchQryReq matchReq;
	memset(&matchReq, 0, sizeof(SmsMatchQryReq));
	//查资金
	SmsFundQryReq fundReq;
	memset(&fundReq, 0, sizeof(SmsFundQryReq));
	//查持仓
	SmsPositionQryReq positionReq;
	memset(&positionReq, 0, sizeof(SmsPositionQryReq));

	char temp[MAX_PATH] = { 0 };
	//序列号
	matchReq.SerialId = 0;
	fundReq.SerialId = 0;
	positionReq.SerialId = 0;

	if (!m_System)return;

	//公司
	strncpy_s(matchReq.CompanyNo, m_System->m_curLogin.CompanyNo);
	strncpy_s(fundReq.CompanyNo, m_System->m_curLogin.CompanyNo);
	strncpy_s(positionReq.CompanyNo, m_System->m_curLogin.CompanyNo);

	//用户
	strncpy_s(matchReq.UserNo, m_System->m_curLogin.UserNo);
	strncpy_s(fundReq.UserNo, m_System->m_curLogin.UserNo);
	strncpy_s(positionReq.UserNo, m_System->m_curLogin.UserNo);

	wstring tempDate;
	m_cbtDate.CxGetWindowText(tempDate);
	//开始日期
	strncpy_s(matchReq.BeginDate, LoadRC::unicode_to_ansi(tempDate).c_str());
	strncpy_s(fundReq.BeginDate, LoadRC::unicode_to_ansi(tempDate).c_str());
	strncpy_s(positionReq.BeginDate, LoadRC::unicode_to_ansi(tempDate).c_str());
	//终止日期
	strncpy_s(matchReq.EndDate, LoadRC::unicode_to_ansi(tempDate).c_str());
	strncpy_s(fundReq.EndDate, LoadRC::unicode_to_ansi(tempDate).c_str());
	strncpy_s(positionReq.EndDate, LoadRC::unicode_to_ansi(tempDate).c_str());

	m_System->m_matchWnd->m_listMatch.DeleteAllItems();
	m_System->m_fundWnd->m_listFund.DeleteAllItems();
	m_System->m_positionWnd->m_listPosition.DeleteAllItems();

	
	m_System->m_mapMatch.clear();

	m_System->m_vecFund.clear();
	m_System->m_vecPosition.clear();


	//g_pSpotSystem->QryMatch(&matchReq);
	//WaitForMultipleObjects(2, m_hQry, TRUE, 10000);
	g_pSpotSystem->QryFund(&fundReq);
	WaitForSingleObject(m_hQryFund, 2000);
	if (m_System->m_vecFund.empty())
	{
		_TMessageBox_Domodal(m_Hwnd, "提示", L"指定日期资金不存在！", TMB_OK);
		return;
	}

	g_pSpotSystem->QryPosition(&positionReq);
	WaitForSingleObject(m_hQryPosition, 2000);
	if (m_System->m_vecPosition.empty())
	{
		_TMessageBox_Domodal(m_Hwnd, "提示", L"指定日期持仓不存在！", TMB_OK);
		return;
	}

	//生成表格
	GenerateList();
}

void PartAccountWnd::OnBtnRead()
{
	if (!m_System || strlen(m_System->m_curLogin.UserNo) == 0)
	{
		_TMessageBox_Domodal(m_Hwnd, "提示", L"请先登录账号！", TMB_OK);
		return;
	}

	OPENFILENAME ofn = { 0 };
	TCHAR strFilename[MAX_PATH] = { 0 };											//用于接收文件名
	ofn.lStructSize = sizeof(OPENFILENAME);											//结构体大小
	ofn.hwndOwner = NULL;															//拥有着窗口句柄，为NULL表示对话框是非模态的，实际应用中一般都要有这个句柄
	ofn.lpstrFilter = TEXT("文本文件(*.txt)\0*.txt\0\0");			//设置过滤    "所有文件\0*.*\0文本文件(*.txt)\0*.txt\0\0"
	ofn.nFilterIndex = 1;															//过滤器索引
	ofn.lpstrFile = strFilename;													//接收返回的文件名，注意第一个字符需要为NULL
	ofn.nMaxFile = sizeof(strFilename);												//缓冲区长度
	ofn.lpstrInitialDir = NULL;														//初始目录为默认
	ofn.lpstrTitle = TEXT("请选择一个文件");										//使用系统默认标题留空即可
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;			//文件、目录必须存在，隐藏只读选项
	if (GetOpenFileName(&ofn))
	{
		//MessageBox(NULL, strFilename, TEXT("选择的文件"), 0);
		//解析CTP总账单
		std::ifstream infile;
		infile.open(strFilename);   //将文件流对象与文件连接起来 
									//assert(infile.is_open());   //若失败,则输出错误消息,并终止程序运行 
		if (!infile)
		{
			MessageBox(NULL, strFilename, TEXT("打开文件失败"), 0);
			return;
		}
		vector<string> v_tempTxt;
		string s;
		while (getline(infile, s))
		{
			//OutputDebugStringA(s.c_str());.
			v_tempTxt.push_back(s);
		}
		SmsFundDataRsp fund;
		memset(&fund, 0, sizeof(SmsFundDataRsp));
		vector<SmsPositionDataRsp>	v_position;
		v_position.clear();
		////解析数据
		//ParseAccount(v_tempTxt, fund, v_position);
		////增加资金和持仓明细
		//g_pSpotSystem->AddFund(&fund);
		//for (auto it : v_position)
		//{
		//	g_pSpotSystem->AddPosition(&it);
		//}
		//关闭文件输入流 
		infile.close();
	}
}

void PartAccountWnd::GenerateList()
{
	//重置
	m_listPartAccount.DeleteAllItems();
	m_listPartAccount.AddItem("今日权益", 0, 0, DT_CENTER, g_ColorRefData.GetColorBlack());
	m_listPartAccount.AddItem("今日可用", 1, 0, DT_CENTER, g_ColorRefData.GetColorBlack());
	m_listPartAccount.AddItem("盈亏总计", 2, 0, DT_CENTER, g_ColorRefData.GetColorBlack());
	m_listPartAccount.AddItem("平仓盈亏", 3, 0, DT_CENTER, g_ColorRefData.GetColorBlack());

	//套保资金
	m_listPartAccount.AddItem(0.1, 0, 1, 2, DT_CENTER, g_ColorRefData.GetColorBlack());
	m_listPartAccount.AddItem(0.2, 1, 1, 2, DT_CENTER, g_ColorRefData.GetColorBlack());
	m_listPartAccount.AddItem(0.3, 2, 1, 2, DT_CENTER, g_ColorRefData.GetColorBlack());
	m_listPartAccount.AddItem(0.4, 3, 1, 2, DT_CENTER, g_ColorRefData.GetColorBlack());
	//套保持仓

	//期货资金
	SmsFundDataRsp	tempFund = m_System->m_vecFund.at(0);
	m_listPartAccount.AddItem(tempFund.Equity, 0, 2, 2, DT_CENTER, g_ColorRefData.GetColorBlack());
	m_listPartAccount.AddItem(tempFund.Available, 1, 2, 2, DT_CENTER, g_ColorRefData.GetColorBlack());
	m_listPartAccount.AddItem(tempFund.CloseProfit + tempFund.PositionProfit, 2, 2, 2, DT_CENTER, g_ColorRefData.GetColorBlack());
	m_listPartAccount.AddItem(tempFund.CloseProfit, 3, 2, 2, DT_CENTER, g_ColorRefData.GetColorBlack());

	int index = 3;
	//期货持仓
	for (auto it : m_System->m_vecPosition)
	{
		string sCode = it.ContractNo;
		string tCode;
		CConvert::SContractNoToTContractNo(sCode, tCode);
		m_listPartAccount.AddItem(tCode.c_str(), ++index, 0, DT_CENTER, g_ColorRefData.GetColorBlack());
		m_listPartAccount.AddItem(it.PositionProfit, index, 1, 2, DT_CENTER, g_ColorRefData.GetColorBlack());
		m_listPartAccount.AddItem(it.PositionProfit, index, 2, 2, DT_CENTER, g_ColorRefData.GetColorBlack());
	}

	
}
