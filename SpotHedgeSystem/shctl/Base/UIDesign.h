#pragma once
#include "windows.h"
#include <xstring>
#include "wingdi.h"
#include <vector>
#include "PluginMgr_API.h"

//bool IsFontExit(LPCWSTR lptext);
int CALLBACK EnumFontsProc(LOGFONT* lplf, TEXTMETRIC* lptm, DWORD dwType, LPARAM lpData);
class G_COLOREF 
{
public:
	G_COLOREF(){};
	virtual ~G_COLOREF();
private:
	COLORREF g_ColorWhite;		//白色
	COLORREF g_ColorBlack;		//黑色
	COLORREF g_ColorBtnFace;	//按钮色 浅灰
	COLORREF g_ColorBackground;	//背景色(控件无效色)
	COLORREF g_ColorRGB100;
	COLORREF g_ColorRGB150;		//横向边框
	COLORREF g_ColorRGB200;		//无效边框

	COLORREF g_ColorGray;		//灰色
	COLORREF g_ColorGrayTitle;	//灰色
	COLORREF g_ColorBlue;		//蓝色
	COLORREF g_ColorLightBlue;	//浅蓝  普通边框高亮
	COLORREF g_ColorLightRed;	//浅红	买边框
	COLORREF g_ColorSellBlue;	//淡蓝
	COLORREF g_ColorBuyRed;		//淡红
	COLORREF g_ColorTextGreen;	//卖绿色
	COLORREF g_ColorTextRed;	//买红色	

	HBRUSH g_HbrWhite;		//白色
	HBRUSH g_HbrBlack;		//黑色
	HBRUSH g_HbrBtnFace;	//按钮色 浅灰
	HBRUSH g_HbrBackground;	//背景色(控件无效色)
	HBRUSH g_HbrRGB100;
	HBRUSH g_HbrRGB150;		//横向边框
	HBRUSH g_HbrRGB200;		//无效边框

	HBRUSH g_HbrGray;		//灰色
	HBRUSH g_HbrGrayTitle;	//灰色
	HBRUSH g_HbrBlue;		//蓝色
	HBRUSH g_HbrLightBlue;	//浅蓝  普通边框高亮
	HBRUSH g_HbrLightRed;	//浅红	买边框
	HBRUSH g_HbrSellBlue;	//淡蓝
	HBRUSH g_HbrBuyRed;		//淡红
	HBRUSH g_HbrTextGreen;	//卖绿色
	HBRUSH g_HbrTextRed;	//买红色	
public:
	void Init();
	
	COLORREF GetColorWhite(){ return g_ColorWhite; };
	COLORREF GetColorBlack(){ return g_ColorBlack; };
	COLORREF GetColorGray(){ return g_ColorGray; };
	COLORREF GetColorGrayTitle(){ return g_ColorGrayTitle; };
	COLORREF GetColorBlue(){ return g_ColorBlue; };
	COLORREF GetColorLightBlue(){ return g_ColorLightBlue; };
	COLORREF GetColorLightRed(){ return g_ColorLightRed; };
	COLORREF GetColorSellBlue(){ return g_ColorSellBlue; };
	COLORREF GetColorBuyRed(){ return g_ColorBuyRed; };
	COLORREF GetColorTextGreen(){ return g_ColorTextGreen; };
	COLORREF GetColorTextRed(){ return g_ColorTextRed; };
	COLORREF GetColorBtnFace(){ return g_ColorBtnFace; };
	COLORREF GetColorBackground(){ return g_ColorBackground; };
	COLORREF GetColorRGB100() { return g_ColorRGB100; };
	COLORREF GetColorRGB150(){ return g_ColorRGB150; };
	COLORREF GetColorRGB200(){ return g_ColorRGB200; };

	HBRUSH GetHbrWhite(){ return g_HbrWhite; };
	HBRUSH GetHbrBlack(){ return g_HbrBlack; };
	HBRUSH GetHbrGray(){ return g_HbrGray; };
	HBRUSH GetHbrGrayTitle(){ return g_HbrGrayTitle; };
	HBRUSH GetHbrBlue(){ return g_HbrBlue; };
	HBRUSH GetHbrLightBlue(){ return g_HbrLightBlue; };
	HBRUSH GetHbrLightRed(){ return g_HbrLightRed; };
	HBRUSH GetHbrSellBlue(){ return g_HbrSellBlue; };
	HBRUSH GetHbrBuyRed(){ return g_HbrBuyRed; };
	HBRUSH GetHbrTextGreen(){ return g_HbrTextGreen; };
	HBRUSH GetHbrTextRed(){ return g_HbrTextRed; };
	HBRUSH GetHbrBtnFace(){ return g_HbrBtnFace; };
	HBRUSH GetHbrBackground(){ return g_HbrBackground; };
	HBRUSH GetHbrRGB100() { return g_HbrRGB100; };
	HBRUSH GetHbrRGB150(){ return g_HbrRGB150; };
	HBRUSH GetHbrRGB200(){ return g_HbrRGB200; };
};
class G_UIFONT
{
public:
	G_UIFONT(){ m_bfontload = false; };
	~G_UIFONT(){};
private:
	HFONT g_FontWordRichedit;//设置
	HFONT g_FontWord9;//设置
    HFONT g_FontWord10;//设置
	HFONT g_FontWord12;//预冻结保证金
	HFONT g_FontWord13;//录单、popwin、popwindate、内盘竖向买卖、Cbutton、
	HFONT g_FontWordEx13;//	
	HFONT g_FontWord15;//title
	HFONT g_FontWord17;//
	HFONT g_FontWordEx15;//
	HFONT g_FontWord19;//买卖跟盘
	HFONT g_FontWord22;//外盘买卖

	HFONT g_FontNum9;//
	HFONT g_FontNum13;//持仓 价格
	HFONT g_FontNumEx13;
	HFONT g_FontNum15;//popwincode
	HFONT g_FontNumEx15;
	HFONT g_FontNumEx17;
	HFONT g_FontNum19;
	HFONT g_FontNumEx19;
	HFONT g_FontNumEx22;
	
	std::wstring g_FontWordName;
	std::wstring g_FontNumName;

	bool m_bfontload;
	std::vector<std::wstring> m_sFont;
public:
	void Init(LANGID LanguageID = NONE);
	bool IsFontExit(LPCWSTR lptext);

	HFONT GetFontWordRichEdit() { return g_FontWordRichedit; };
	HFONT GetFontWord9(){ return g_FontWord9; };
    HFONT GetFontWord10() { return g_FontWord10; };
	HFONT GetFontWord12(){ return g_FontWord12; };
	HFONT GetFontWord13() { return g_FontWord13; };
	HFONT GetFontWordEx13() { return g_FontWordEx13; };	
	HFONT GetFontWord15(){ return g_FontWord15; };
	HFONT GetFontWordEx15() { return g_FontWordEx15; };
	HFONT GetFontWord17() { return g_FontWord17; };	
	HFONT GetFontWord19(){ return g_FontWord19; };
	HFONT GetFontWord22(){ return g_FontWord22; };

	HFONT GetFontNum9(){ return g_FontNum9; };
	HFONT GetFontNum13(){ return g_FontNum13; };
	HFONT GetFontNumEx13(){ return g_FontNumEx13; };
	HFONT GetFontNum15(){ return g_FontNum15; };
	HFONT GetFontNumEx15(){ return g_FontNumEx15; };
	HFONT GetFontNumEx17(){ return g_FontNumEx17; };
	HFONT GetFontNum19(){ return g_FontNum19; };
	HFONT GetFontNumEx19(){ return g_FontNumEx19; };
	HFONT GetFontNumEx22() { return g_FontNumEx22; };


	std::wstring GetFontWordName(){ return g_FontWordName; };
	std::wstring GetFontNumName(){ return g_FontNumName; };
};