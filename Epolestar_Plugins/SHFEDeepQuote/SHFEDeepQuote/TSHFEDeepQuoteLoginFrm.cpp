#include "PreInclude.h"

#define closeWidth 9
#define VGAP  30

#define EMainFrmName L"class TMainFrame"
#define WIDGET_LEFT 12
#define WIDGET_HGAP 5
#define FRM_WIDTH  300
#define FRM_HEIGHT  230
#define MID_WIDTH FRM_WIDTH / 2
#define MID_HEIGHT FRM_HEIGHT / 2
#define WidghtHeight  20
#define WidghtWidth  150

bool CLoginFrm::TestMBLPortConnected()
{
	//创建socket
	SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	//设置socket非阻塞
	unsigned long ub(1);
	ioctlsocket(s, FIONBIO, &ub);

	//设置
	sockaddr_in addr;
	memset(&addr, 0, sizeof(sockaddr_in));
	addr.sin_family = AF_INET;
	InetPtonA(AF_INET, MBL_IP, &addr.sin_addr.s_addr); //不支持xp
													   //InetPton(AF_INET, MBL_IP, &addr.sin_addr.s_addr);
													   //inet_pton(AF_INET, MBL_IP, &addr.sin_addr.s_addr);
	addr.sin_port = htons(MBL_SHFE_Port);

	//测速
	fd_set writeset;
	FD_ZERO(&writeset);
	FD_SET(s, &writeset);

	fd_set exceptset;
	FD_ZERO(&exceptset);
	FD_SET(s, &exceptset);

	timeval tv;
	tv.tv_sec = 1;
	tv.tv_usec = 0;

	::connect(s, (sockaddr*)&addr, sizeof(sockaddr));
	int sret = ::select(0, 0, &writeset, &exceptset, &tv);

	if (FD_ISSET(s, &writeset))
	{
		return true;
	}
	return false;
}


void CLoginFrm::Create(TCHAR* WndName, wstring TitleName)
{
	RECT rect;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);
	m_posRect.left = (rect.right - rect.left) / 2 - FRM_WIDTH / 2;
	m_posRect.top = (rect.bottom - rect.top) / 2 - FRM_HEIGHT / 2;
	m_posRect.right = (rect.right - rect.left) / 2 + FRM_WIDTH / 2;
	m_posRect.bottom = (rect.bottom - rect.top) / 2 + FRM_HEIGHT / 2;

	ShowFrm(FindWindow(EMainFrmName, NULL), WndName, TitleName);
	MoveWindow(m_posRect.left, m_posRect.top, m_posRect.right - m_posRect.left, m_posRect.bottom - m_posRect.top);
}

void CLoginFrm::OnCreate()
{
	RECT rectLabel = { MID_WIDTH - 100, 50, MID_WIDTH - 60, 75};
	RECT rectText = { MID_WIDTH - 50, 50, MID_WIDTH + 100, 75};
	m_UserLable.Create(m_Hwnd, L"行  情:", 4);
	m_UserLable.MoveWindow(rectLabel.left, rectLabel.top, rectLabel.right - rectLabel.left, rectLabel.bottom - rectLabel.top);
	m_comContractType.Create(m_Hwnd);
	m_comContractType.AddString("SHFE");
	m_comContractType.AddString("INE");
	m_comContractType.SetText("SHFE");
	m_comContractType.SetFont(g_FontData.g_FontNum15);
	m_comContractType.SetComboxHeight(rectText.bottom - rectText.top);
	m_comContractType.MoveWindow(rectText.left, rectText.top, rectText.right - rectText.left, rectText.bottom - rectText.top);

	rectLabel.top += 40;
	rectLabel.bottom += 40;
	rectText.top += 40;
	rectText.bottom += 40;
	//用户名
	m_UserLable.Create(m_Hwnd, L"用户名:", 0);
	m_UserLable.MoveWindow(rectLabel.left, rectLabel.top, rectLabel.right - rectLabel.left, rectLabel.bottom - rectLabel.top);
	//m_UserLable.SetFont(g_FontData.g_FontNum15);
	m_UserEdit.Create(m_Hwnd);
	m_UserEdit.SetFont(g_FontData.g_FontNum15);
	m_UserEdit.SetFgColor(g_ColorRefData.GetColorGray());
	m_UserEdit.MoveWindow(rectText.left, rectText.top, rectText.right - rectText.left, rectText.bottom - rectText.top);

	//密码
	rectLabel.top += 40;
	rectLabel.bottom += 40;
	rectText.top += 40;
	rectText.bottom += 40;
	m_PWLable.Create(m_Hwnd, L"密  码:", 2);
	m_PWLable.MoveWindow(rectLabel.left, rectLabel.top, rectLabel.right - rectLabel.left, rectLabel.bottom - rectLabel.top);
	DWORD specialstyle = ES_PASSWORD;
	m_PWEdit.Create(m_Hwnd, specialstyle);
	m_PWEdit.SetFont(g_FontData.g_FontNum15);
	m_PWEdit.SetFgColor(g_ColorRefData.GetColorGray());
	m_PWEdit.MoveWindow(rectText.left, rectText.top, rectText.right - rectText.left, rectText.bottom - rectText.top);

	//登录
	m_btnTest.CreateButton(m_Hwnd, m_btnTest, "登录", 4);
	m_btnTest.SetFont(g_FontData.g_FontNum15);
	m_btnTest.SetBkColor(g_ColorRefData.GetColorGray());
	m_btnTest.MoveWindow(rectLabel.left, rectLabel.top + 45, 200, 25);
}

LRESULT CLoginFrm::OnCommand(WPARAM wParam, LPARAM lParam)
{
	HWND hwnd = (HWND)lParam;
	if (hwnd == m_UserEdit.GetEditHwnd())
	{
		m_UserEdit.GetWindowTextW(m_UserName, 32);
	} 
	else if(hwnd == m_PWEdit.GetEditHwnd())
	{
		m_PWEdit.GetWindowTextW(m_Password, 32);
	}
	
	return PROCESSED;
}


void CLoginFrm::OnBtnClick(WPARAM wParam, LPARAM lParam)
{
	bool isConnectedSucess = TestMBLPortConnected();
	if (!isConnectedSucess)
	{
		TCHAR szBuffer[] = L"当前服务器端口已关闭，请在交易时段登陆！";
		MessageBox(0, szBuffer, L"Tips", MB_OK | MB_ICONASTERISK);
		return;
	}
	string scontractType = m_comContractType.GetText();
	if(m_UserName[0] == '\0' || m_Password[0] == '\0')
	{
		TCHAR szBuffer[] = L"登陆失败：用户名和密码不能为空";
		MessageBox(0, szBuffer, L"Tips", MB_OK | MB_ICONASTERISK);
		return;
	}
	else
	{
		CMBLSpiHandler * handler = CMBLSpiHandler::GetMBLQuoteData();
		handler->SetQuoteLogin(this);
		handler->Login(m_UserName, m_Password, scontractType);
		//测试代码 待删除
		//OnQuoteLogin(0, 0);
	}
}

void CLoginFrm::QuoteLogin(MBLErrorInfo errprInfo)
{
	PostMessage(m_Hwnd, WM_QUOTELOGIN, (WPARAM)errprInfo, 0);
}

void CLoginFrm::OnQuoteLogin(WPARAM wParam, LPARAM lParam)
{
	if(wParam != MBL_NO_ERROR)
	{
		TCHAR szBuffer[] = L"登陆失败，请确认用户名密码是否正确";
		MessageBox(0, szBuffer, L"Tips", MB_OK | MB_ICONASTERISK);
		//测试代码 待修改
		return;
	}

	DestroyWindow(m_Hwnd);
	SHFEDeepQuoteFrm::bQuoteLoginSuccess = true;

	if (!g_DeepQuoteFrm)
	{
		g_DeepQuoteFrm = new SHFEDeepQuoteFrm;
		g_DeepQuoteFrm->ShowFrm(FindWindow(L"class TMainFrame", NULL), L"deep_quote", L"国富上期所深度行情");
		g_pMainFrame->AddLinkage(g_DeepQuoteFrm->GetHwnd(), 1, OnLinkageAction);
	}
	//SetQuoteUpdate(g_DeepQuoteFrm);
	ShowWindow(g_DeepQuoteFrm->GetHwnd(), SW_SHOW);
}

LRESULT CLoginFrm::OnNcDestory(WPARAM wParam, LPARAM lParam)
{
	//Gdiplus::GdiplusShutdown(gdiplusStartupToken);
	delete this;
	return PROCESSED;
}

void CLoginFrm::GetImgPath(const wchar_t* pszPath)
{
	GetCurrentDirectory(ARRAYSIZE(m_szImgFilePath), m_szImgFilePath);
	wcscat_s(m_szImgFilePath, pszPath);
	//m_Img = Gdiplus::Image::FromFile(m_szImgFilePath);
}

LRESULT CLoginFrm::OnDealMsg(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_QUOTELOGIN:
		OnQuoteLogin(wParam, lParam);
		return PROCESSED;
	default:
		return NOT_PROCESSED;
	}
}
