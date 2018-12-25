#pragma once
#include <vector>
#include "PluginMgr_API.h"
using namespace std;
bool IsFontExit(LPCWSTR lptext);
int CALLBACK EnumFontsProc(LOGFONT* lplf, TEXTMETRIC* lptm, DWORD dwType, LPARAM lpData); 
class G_COLOREF 
{
public:
	G_COLOREF(){};
	~G_COLOREF(){};
public:
	COLORREF g_ColorWhite;		//白色
	COLORREF g_ColorBlack;		//黑色
	COLORREF g_ColorBtnFace;	//按钮色 浅灰
	COLORREF g_ColorBackground;	//背景色(控件无效色)
	COLORREF g_ColorRGB150;		//横向边框
	COLORREF g_ColorRGB200;		//无效边框

	COLORREF g_ColorGray;		//灰色
	COLORREF g_ColorGray2;
	COLORREF g_ColorGrayTitle;	//灰色
	COLORREF g_ColorBlue;		//蓝色
	COLORREF g_ColorLightBlue;	//浅蓝  普通边框高亮
	COLORREF g_ColorLightRed;	//浅红	买边框
	COLORREF g_ColorSellBlue;	//淡蓝
	COLORREF g_ColorBuyRed;		//淡红
	COLORREF g_ColorTextGreen;	//卖绿色
	COLORREF g_ColorTextRed;	//买红色
	
	HBRUSH   g_brush_white;
	HBRUSH   g_brush_gray;
	HBRUSH   g_brush_bk;
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
	COLORREF GetColorRGB150(){ return g_ColorRGB150; };
	COLORREF GetColorRGB200(){ return g_ColorRGB200; };
};
class G_UIFONT
{
public:
	G_UIFONT(){ m_bfontload = false; };
	~G_UIFONT(){};
public:
	
	HFONT g_FontWord9;//设置
	HFONT g_FontWord11;
	HFONT g_FontWord12;//预冻结保证金
	HFONT g_FontWord13;//录单、popwin、popwindate、内盘竖向买卖、Cbutton、
	HFONT g_FontWord15;
	HFONT g_FontWord19;//买卖跟盘
	HFONT g_FontWord22;//外盘买卖
    HFONT g_FontWord50;//Tips

	HFONT g_FontNum9;//
    HFONT g_FontNum13;//
	HFONT g_FontNum15;//popwincode
	HFONT g_FontNum17;
    HFONT g_FontNum50;
	HFONT g_FontNum19;
	wstring g_FontWordName;
	wstring g_FontNumName;

	HFONT g_FontWord18_2;
	HFONT g_fontWord28_2;

	bool m_bfontload;
	vector<wstring> m_sFont;
public:
	void Init(LANGID LanguageID = NONE);
	bool IsFontExit(LPCWSTR lptext);
	
	HFONT GetFontWord9(){ return g_FontWord9; };
	HFONT GetFontWord11(){ return g_FontWord11; }
	HFONT GetFontWord12(){ return g_FontWord12; };
	HFONT GetFontWord13(){ return g_FontWord13; };
	HFONT GetFontWord19(){ return g_FontWord19; };
	HFONT GetFontWord22(){ return g_FontWord22; };
    HFONT GetFontWord50(){ return g_FontWord50; };

	HFONT GetFontNum9(){ return g_FontNum9; };
	HFONT GetFontNum13(){ return g_FontNum13; };
	HFONT GetFontNum15(){ return g_FontNum15; };
	HFONT GetFontNum17(){ return g_FontNum17; };
	HFONT GetFontNum19(){ return g_FontNum19; };

	wstring GetFontWordName(){ return g_FontWordName; };
	wstring GetFontNumName(){ return g_FontNumName; };
};
