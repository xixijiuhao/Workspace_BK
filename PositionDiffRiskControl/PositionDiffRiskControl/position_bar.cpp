#include "PreInclude.h"

extern IConfigFrame *			g_config_api;
extern G_COLOREF				g_ColorRefData;
extern ILanguageApi *			g_pLanguage;
extern IAuthent *				g_pAuthent;
extern bool						flagAuthent;
extern vPosDifCfg				vecTmp;				//配置容器

#define PD_BAR					1
#define COLOR_BG				RGB(60, 60, 60)
#define COLOR_GREEN				RGB(0, 220, 0)
#define COLOR_RED				RGB(255, 0, 0)

CPositionDiffBar::CPositionDiffBar()
{
	m_brushBg = CreateSolidBrush(COLOR_BG);
	//m_textColor = COLOR_GREEN;
	IsActive();
}
CPositionDiffBar::~CPositionDiffBar()
{
	DeleteObject(m_brushBg);
	DeleteObject(m_font);
}
void CPositionDiffBar::Create(LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle )
{
	//CreateFrm(classname, parent, style, exstyle);
	CreateFrm(L"CEFC bar", 0, WS_VISIBLE | WS_POPUP, WS_EX_TOOLWINDOW);
	SetWindowPos(m_Hwnd, 0, NULL, 0, 80, 0, SWP_NOMOVE | SWP_NOZORDER | SWP_SHOWWINDOW);
}

void CPositionDiffBar::IsActive()
{

	if (vecTmp.size() != 0 )
	{
		m_textColor = COLOR_GREEN;
	}
	else
	{
		m_textColor = COLOR_RED;
	}
	InvalidateRect(m_Hwnd, NULL, TRUE);
}

LRESULT CPositionDiffBar::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		OnCreateWindow();
		break;
	case WM_LBUTTONUP:
		OnLButtonUp(wParam, lParam);
		break;
	case WM_PAINT:
		OnPaint(wParam, lParam);
		break;
	case WM_CTLCOLORSTATIC:
	//{
	//	hdcStatic = (HDC)wParam;
	//	SetTextColor((HDC)wParam, RGB(0, 220, 0));//控件文本的字体颜色
	//	SetBkColor((HDC)wParam, RGB(0X41, 0X96, 0X4F));//控件字体的背景颜色，注意跟以上区分 
	//	SetBkMode((HDC)wParam, TRANSPARENT);
	//	return (LONG)m_hStaticBKBrush;//这里必须返回画刷句柄 
	//}
		break;
	case WM_ERASEBKGND:
		break;
	case WM_DESTROY:
		break;
	default:
		return NOT_PROCESSED;
	}
	return PROCESSED;
}

void CPositionDiffBar::OnCreateWindow()
{



	//HINSTANCE hInstance = GetModuleHandle(NULL);
	//SetWindowPos(m_Hwnd,NULL,0,0,80,0,SWP_NOZORDER|SWP_NOMOVE);
	//hwnd = CreateWindowEx(0, L"STATIC", CConvert::LoadResString(IDS_Subtitle).c_str(), WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | SS_CENTERIMAGE | SS_NOTIFY | SS_CENTER,
	//	0, 0, 80, 24, m_Hwnd, (HMENU)PD_BAR, hInstance, NULL);

	LOGFONT lg;
	::GetObject(g_FontData.GetFontWordEx15(), sizeof(LOGFONT), &lg);
	if (g_pLanguage->LangId() == ENU)
	{
		lg.lfHeight = 18;
	}
	else
	{
		lg.lfHeight = 20;
	}
	lg.lfWeight = FW_BOLD;
	wcsncpy_s(lg.lfFaceName, L"Microsoft Yahei", _TRUNCATE);
	m_font = CreateFontIndirect(&lg);


	//SendMessage(hwnd, WM_SETFONT, (WPARAM)g_FontData.GetFontNumEx13(), (LPARAM)TRUE);
	//SendMessage(hwnd, WM_SETFONT, (WPARAM)m_font, (LPARAM)TRUE);
}

void CPositionDiffBar::OnLButtonUp(WPARAM wParam, LPARAM lParam)
{
	//switch (LOWORD(wParam))
	//{
	//case PD_BAR:
	//	g_config_api->show(CConvert::LoadResString(IDS_Subtitle).c_str(), cmtExpand);
	//	break;
	//default:
	//	g_config_api->show(CConvert::LoadResString(IDS_Subtitle).c_str(), cmtExpand);
	//	break;
	//}
	g_config_api->show(CConvert::LoadResString(IDS_Subtitle).c_str(), cmtExpand);
}

void CPositionDiffBar::OnPaint(WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	hdc = BeginPaint(m_Hwnd, &ps);

	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	FillRect(hdc, &rect, m_brushBg);
	SetBkMode(hdc, TRANSPARENT);
	SelectObject(hdc, m_font);
	SetTextColor(hdc, m_textColor);
	DrawText(hdc, CConvert::LoadResString(IDS_Subtitle).c_str(), wcslen(CConvert::LoadResString(IDS_Subtitle).c_str()), &rect, DT_VCENTER | DT_CENTER | DT_SINGLELINE);
	
	EndPaint(m_Hwnd, &ps);
}

void CPositionDiffBar::OnNotify(WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam)) 
	{
	case PD_BAR:
	{
		break;
	}
	default:
		break;
	}
}