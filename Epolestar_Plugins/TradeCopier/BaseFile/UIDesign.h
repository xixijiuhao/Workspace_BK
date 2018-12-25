#pragma once

using namespace std;

int CALLBACK EnumFontsProc1(LOGFONT* lplf, TEXTMETRIC* lptm, DWORD dwType, LPARAM lpData); 
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
	COLORREF g_ColorBlue;		//蓝色
	COLORREF g_ColorLightBlue;	//浅蓝  普通边框高亮
	COLORREF g_ColorTextGreen;	//卖绿色
	COLORREF g_ColorTextRed;	//买红色
	
	HBRUSH   g_brush_white;
	HBRUSH   g_brush_gray;

public:
	
	void Init();
	
	COLORREF GetColorWhite(){ return g_ColorWhite; };
	COLORREF GetColorBlack(){ return g_ColorBlack; };
	COLORREF GetColorGray(){ return g_ColorGray; };
	COLORREF GetColorBlue(){ return g_ColorBlue; };
	COLORREF GetColorLightBlue(){ return g_ColorLightBlue; };
	COLORREF GetColorTextGreen(){ return g_ColorTextGreen; };
	COLORREF GetColorTextRed(){ return g_ColorTextRed; };
	COLORREF GetColorBtnFace(){ return g_ColorBtnFace; };
	COLORREF GetColorBackground(){ return g_ColorBackground; };
	COLORREF GetColorRGB150(){ return g_ColorRGB150; };
	COLORREF GetColorRGB200() { return g_ColorRGB200; };
};
class G_UIFONT
{
public:
	G_UIFONT(){ m_bfontload = false; };
	~G_UIFONT(){};
public:
	
	HFONT g_FontWord12;//预冻结保证金
	HFONT g_FontWord13;//录单、popwin、popwindate、内盘竖向买卖、Cbutton、
	HFONT g_FontWord19;


	HFONT g_FontNum13;//持仓 价格
	HFONT g_FontNum15;//popwincode

	wstring g_FontWordName;
	wstring g_FontNumName;

	bool m_bfontload;
	vector<wstring> m_sFont;
public:
	void Init(LANGID LanguageID = NONE);
	bool IsFontExit(LPCWSTR lptext);
	
	HFONT GetFontWord12(){ return g_FontWord12; };
	HFONT GetFontWord13(){ return g_FontWord13; };

	HFONT GetFontNum13(){ return g_FontNum13; };
	HFONT GetFontNum15(){ return g_FontNum15; };

};
