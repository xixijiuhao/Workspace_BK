#include "UIDesign.h"

using namespace std;

int CALLBACK EnumFontsProc(LOGFONT* lplf, TEXTMETRIC* lptm, DWORD dwType, LPARAM lpData)
{
	vector<wstring>* pstlfont = (vector<wstring>*)lpData;
	pstlfont->push_back(lplf->lfFaceName);
	return 1;
};

G_COLOREF::~G_COLOREF()
{
	DeleteObject(g_HbrWhite);
	DeleteObject(g_HbrBlack);
	DeleteObject(g_HbrGray);
	DeleteObject(g_HbrGrayTitle);
	DeleteObject(g_HbrBlue);
	DeleteObject(g_HbrLightBlue);
	DeleteObject(g_HbrLightRed);
	DeleteObject(g_HbrSellBlue);
	DeleteObject(g_HbrBuyRed);
	DeleteObject(g_HbrTextRed);
	DeleteObject(g_HbrTextGreen);
	DeleteObject(g_HbrBtnFace);
	DeleteObject(g_HbrBackground);
	DeleteObject(g_HbrRGB150);
	DeleteObject(g_HbrRGB200);
};
void G_COLOREF::Init()
{
	g_ColorWhite = RGB(255, 255, 255);		//白色
	g_ColorBlack = RGB(0, 0, 0);			//黑色
	g_ColorGray = RGB(96, 96, 96);			//灰色		
	g_ColorGrayTitle = RGB(96, 96, 96);		//灰色		//录单标题
	g_ColorBlue = RGB(25, 121, 202);		//蓝色
	g_ColorLightBlue = RGB(73, 172, 231);	//浅蓝		//所有
	g_ColorLightRed = RGB(231, 83, 73);		//浅红
	g_ColorSellBlue = RGB(26, 129, 237);	//卖按钮色	//竖向
	g_ColorBuyRed = RGB(201, 51, 39);		//买按钮色
	g_ColorTextRed = RGB(236, 6, 6);		//买文本色
	g_ColorTextGreen = RGB(7, 121, 6);		//卖文本色 //横向
	g_ColorBtnFace = RGB(240, 240, 240);	//按钮色 浅灰
	g_ColorBackground = RGB(243, 243, 243);	//背景色
	g_ColorRGB150 = RGB(150, 150, 150);		//横向边框
	g_ColorRGB200 = RGB(200, 200, 200);		//填充

	g_HbrWhite = CreateSolidBrush(g_ColorWhite);		//白色
	g_HbrBlack = CreateSolidBrush(g_ColorBlack);;		//黑色
	g_HbrGray = CreateSolidBrush(g_ColorGray);;		//灰色
	g_HbrGrayTitle = CreateSolidBrush(g_ColorGrayTitle);;	//灰色
	g_HbrBlue = CreateSolidBrush(g_ColorBlue);;		//蓝色
	g_HbrLightBlue = CreateSolidBrush(g_ColorLightBlue);;	//浅蓝  普通边框高亮
	g_HbrLightRed = CreateSolidBrush(g_ColorLightRed);;	//浅红	买边框
	g_HbrSellBlue = CreateSolidBrush(g_ColorSellBlue);;	//淡蓝
	g_HbrBuyRed = CreateSolidBrush(g_ColorBuyRed);;		//淡红
	g_HbrTextRed = CreateSolidBrush(g_ColorTextRed);;		//买红色	
	g_HbrTextGreen = CreateSolidBrush(g_ColorTextGreen);;	//卖绿色	
	g_HbrBtnFace = CreateSolidBrush(g_ColorBtnFace);;	//按钮色 浅灰
	g_HbrBackground = CreateSolidBrush(g_ColorBackground);;	//背景色(控件无效色)
	g_HbrRGB150 = CreateSolidBrush(g_ColorRGB150);;		//横向边框
	g_HbrRGB200 = CreateSolidBrush(g_ColorRGB200);;		//无效边框
}

void G_UIFONT::Init(LANGID LanguageID)
{
	wchar_t sWord[15] ={0};
	if (IsFontExit(L"宋体"))
		wcscpy_s(sWord, 15,L"宋体");
	else
		wcscpy_s(sWord,15, L"MS Shell Dlg");

	wchar_t sNum[15] = { 0 };
	if (IsFontExit(L"Calibri") && LanguageID == ENU)
		wcscpy_s(sNum, 15, L"Calibri"); 
	else if (IsFontExit(L"Arial"))
		wcscpy_s(sNum ,15,L"Arial");
	else
		wcscpy_s(sNum,15, L"MS Shell Dlg");

	if (LanguageID == CHT || LanguageID == CHS)
	{
		g_FontWordName = sWord;
		g_FontNumName = sNum;
		g_FontWord9 = CreateFont(-9, 0, 0, 0, 0, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 0, 0, 0, 0, sWord);
		g_FontWord12 = CreateFont(-12, 0, 0, 0, 0, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 0, 0, 0, 0, sWord);
		g_FontWord13 = CreateFont(-13, 0, 0, 0, 0, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 0, 0, 0, 0, sWord);
		g_FontWordEx13 = CreateFont(-15, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY, DEFAULT_PITCH, sWord);
		g_FontWord15 = CreateFont(-15, 0, 0, 0, 0, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 0, 0, 0, 0, sWord);
		g_FontWord17 = CreateFont(-17, 0, 0, 0, FALSE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 0, 0, 0, 0, sWord);
		g_FontWordEx15 = CreateFont(-18, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY, DEFAULT_PITCH, sWord);
		g_FontWord19 = CreateFont(-19, 0, 0, 0, 0, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 0, 0, 0, 0, sWord);
		g_FontWord22 = CreateFont(-22, 0, 0, 0, 0, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 0, 0, 0, 0, sWord);

		g_FontNum9 = CreateFont(-9, 0, 0, 0, 0, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 0, 0, 0, 0, sNum);
		g_FontNum13 = CreateFont(-13, 0, 0, 0, 0, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 0, 0, 0, 0, sNum);
		g_FontNum15 = CreateFont(-15, 0, 0, 0, 0, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 0, 0, 0, 0, sNum);
		g_FontNumEx13 = CreateFont(-13, 0, 0, 0, FW_EXTRABOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY, DEFAULT_PITCH, sNum);
		g_FontNumEx15 = CreateFont(-15, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY, DEFAULT_PITCH, sNum);
		g_FontNumEx17 = CreateFont(-17, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY, DEFAULT_PITCH, sNum);
		
		g_FontNum19 = CreateFont(-19, 0, 0, 0, 0, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 0, 0, 0, 0, sNum);
		g_FontNumEx19 = CreateFont(-19, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY, DEFAULT_PITCH, sNum);
		g_FontNumEx22 = CreateFont(-22, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY, DEFAULT_PITCH, sNum);
	}
	else
	{
		g_FontWordName = sWord;
		g_FontNumName = sNum;
		g_FontWord9 = CreateFont(-9, 0, 0, 0, 0, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 0, 0, 0, 0, sNum);
		g_FontWord12 = CreateFont(-11, 0, 0, 0, 0, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 0, 0, 0, 0, sNum);
		g_FontWord13 = CreateFont(-13, 0, 0, 0, 0, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 0, 0, 0, 0, sNum);
		g_FontWordEx13 = CreateFont(-13, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY, DEFAULT_PITCH, sNum);
		g_FontWord15 = CreateFont(-15, 0, 0, 0, 0, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 0, 0, 0, 0, sNum);
		g_FontWord17 = CreateFont(-15, 0, 0, 0, 0, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 0, 0, 0, 0, sNum);
		g_FontWordEx15 = CreateFont(-15, 0, 0, 0, 0, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY, DEFAULT_PITCH, sNum);
		g_FontWord19 = CreateFont(-19, 0, 0, 0, 0, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 0, 0, 0, 0, sNum);
		g_FontWord22 = CreateFont(-22, 0, 0, 0, 0, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 0, 0, 0, 0, sNum);

		g_FontNum9 = CreateFont(-9, 0, 0, 0, 0, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 0, 0, 0, 0, sNum);
		g_FontNum13 = CreateFont(-13, 0, 0, 0, 0, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 0, 0, 0, 0, sNum);
		g_FontNumEx13 = CreateFont(-13, 0, 0, 0, FW_EXTRABOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY, DEFAULT_PITCH, sNum);
		g_FontNum15 = CreateFont(-15, 0, 0, 0, 0, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 0, 0, 0, 0, sNum);
		g_FontNumEx15 = CreateFont(-15, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY, DEFAULT_PITCH, sNum);
		g_FontNumEx17 = CreateFont(-17, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY, DEFAULT_PITCH, sNum);
		g_FontNum19 = CreateFont(-19, 0, 0, 0, 0, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 0, 0, 0, 0, sNum);
		g_FontNumEx19 = CreateFont(-19, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY, DEFAULT_PITCH, sNum);
		g_FontNumEx22 = CreateFont(-22, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY, DEFAULT_PITCH, sNum);
	}
};

bool G_UIFONT::IsFontExit(LPCWSTR lptext)
{
	if (!m_bfontload)
	{
		HDC hdc = GetDC(NULL);
		EnumFonts(hdc, NULL, (FONTENUMPROC)EnumFontsProc, (LPARAM)&m_sFont);
		ReleaseDC(NULL, hdc);
		m_bfontload = true;
	}
	find(m_sFont.begin(), m_sFont.end(), lptext);
	if (find(m_sFont.begin(), m_sFont.end(), lptext) != m_sFont.end())
		return true;
	return false;
};