#include "PreInclude.h"
using namespace std;

void G_COLOREF::Init()
{
	g_ColorWhite = RGB(255, 255, 255);		//白色
	g_ColorBlack = RGB(0, 0, 0);			//黑色
	g_ColorGray = RGB(150, 150, 150);			//灰色	
	g_ColorGray2 = RGB(200, 200, 200);
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

	g_brush_white = CreateSolidBrush(g_ColorWhite);
	g_brush_gray = CreateSolidBrush(g_ColorGray);
	g_brush_bk = CreateSolidBrush(g_ColorBackground);
}

void G_UIFONT::Init(LANGID LanguageID)
{
	wstring sWord = TEXT(""), sNum = TEXT("");
	if (IsFontExit(TEXT("宋体")))
		sWord = TEXT("宋体");
	else
		sWord = TEXT("MS Shell Dlg");
	if (IsFontExit(TEXT("Arial")))
		sNum = TEXT("Arial");
	else
		sNum = TEXT("MS Shell Dlg");

	if (LanguageID == CHT || LanguageID == CHS)
	{
		g_FontWordName = sWord;
		g_FontNumName = sNum;
		g_FontWord9 = CreateFont(-9, 0, 0, 0, 0, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 0, 0, 0, 0, sWord.c_str());
		g_FontWord11 = CreateFont(-10, 0, 0, 0, 0, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 0, 0, 0, 0, sWord.c_str());
		g_FontWord12 = CreateFont(-12, 0, 0, 0, 0, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 0, 0, 0, 0, sWord.c_str());
		g_FontWord13 = CreateFont(-13, 0, 0, 0, 0, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 0, 0, 0, 0, sWord.c_str());
		g_FontWord15 = CreateFont(-15, 0, 0, 0, 0, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 0, 0, 0, 0, sWord.c_str());
		g_FontWord19 = CreateFont(-19, 0, 0, 0, 0, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 0, 0, 0, 0, sWord.c_str());
		g_FontWord22 = CreateFont(-22, 0, 0, 0, 0, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 0, 0, 0, 0, sWord.c_str());
        g_FontWord50 = CreateFont(-50, 0, 0, 0, 0, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 0, 0, 0, 0, sWord.c_str());

		g_FontNum9 = CreateFont(-9, 0, 0, 0, 0, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 0, 0, 0, 0, sNum.c_str());
		g_FontNum13 = CreateFont(-13, 0, 0, 0, 0, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 0, 0, 0, 0, sNum.c_str());
		g_FontNum15 = CreateFont(-15, 0, 0, 0, 0, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 0, 0, 0, 0, sNum.c_str());
		g_FontNum17 = CreateFont(-17, 0, 0, 0, 0, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 0, 0, 0, 0, sNum.c_str());

		g_FontNum19 = CreateFont(-19, 0, 0, 0, 0, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 0, 0, 0, 0, sNum.c_str());

		g_FontWord18_2 = CreateFont(-18, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 0, 0, 0, 0, sNum.c_str());
		g_fontWord28_2 = CreateFont(-35, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 0, 0, 0, 0, sNum.c_str());
	}
	else
	{
		g_FontWordName = sWord;
		g_FontNumName = sNum;
		g_FontWord9 = CreateFont(-9, 0, 0, 0, 0, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 0, 0, 0, 0, sNum.c_str());
		g_FontWord11 = CreateFont(-10, 0, 0, 0, 0, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 0, 0, 0, 0, sNum.c_str());
		g_FontWord12 = CreateFont(-11, 0, 0, 0, 0, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 0, 0, 0, 0, sNum.c_str());
		g_FontWord13 = CreateFont(-13, 0, 0, 0, 0, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 0, 0, 0, 0, sNum.c_str());
		g_FontWord15 = CreateFont(-15, 0, 0, 0, 0, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 0, 0, 0, 0, sNum.c_str());
		g_FontWord19 = CreateFont(-19, 0, 0, 0, 0, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 0, 0, 0, 0, sNum.c_str());
		g_FontWord22 = CreateFont(-22, 0, 0, 0, 0, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 0, 0, 0, 0, sNum.c_str());

		g_FontNum9 = CreateFont(-9, 0, 0, 0, 0, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 0, 0, 0, 0, sNum.c_str());
		g_FontNum13 = CreateFont(-13, 0, 0, 0, 0, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 0, 0, 0, 0, sNum.c_str());
		g_FontNum15 = CreateFont(-15, 0, 0, 0, 0, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 0, 0, 0, 0, sNum.c_str());
		g_FontNum17 = CreateFont(-17, 0, 0, 0, 0, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 0, 0, 0, 0, sNum.c_str());
		g_FontNum19 = CreateFont(-19, 0, 0, 0, 0, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 0, 0, 0, 0, sNum.c_str());

		g_FontWord18_2 = CreateFont(-18, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 0, 0, 0, 0, sNum.c_str());
		g_fontWord28_2 = CreateFont(-35, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 0, 0, 0, 0, sNum.c_str());
	}
};
int CALLBACK EnumFontsProc(LOGFONT* lplf, TEXTMETRIC* lptm, DWORD dwType, LPARAM lpData)
{
	std::vector<std::wstring>* pstlfont = (std::vector<std::wstring>*)lpData;
	std::wstring sztext;
	sztext = lplf->lfFaceName;
	pstlfont->push_back(sztext);
	return 1;
}
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