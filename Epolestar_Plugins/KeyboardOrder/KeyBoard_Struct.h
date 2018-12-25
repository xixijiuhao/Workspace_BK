#pragma once

struct C_KeyBoardOrderCfg
{
	bool				bSetDefaultPos;		//默认光标
	unsigned int		uiFocusPos;			//默认焦点位于第几项

	bool				bUpDownEqualTab;	//上下键
	bool				bRightLeftEqualTab;	//左右键

	unsigned int		uiDoOrderStyle;		//横向下单风格	0 确定  1 买卖  2 确定 埋单 自动单

	unsigned int		uiKeyBoardFont;		//横向下单字体大小 0小  1中 2大
	bool				bShowOrderValidType;//显示有效类型 定单类型 内盘

	uchar				ucBuy;
	uchar				ucSell;
	uchar				ucOpen;
	uchar				ucCover;
	uchar				ucCoverT;
	bool				bEnterSendOrder;	//Enter发单

	int					OutPriceType;		//默认价额超价类型
	int					OutPriceTick;		//默认价额超价点数
	bool				bStartOrderAssistant;//开启追单助手
	unsigned int		uiAssistType;		//追单类型 0 都追 1 只追开仓 2 只追平仓
	std::map<std::wstring, std::wstring> mapContractHotkey;//合约快捷键
	C_KeyBoardOrderCfg()
	{
		memset(this, 0, sizeof(C_KeyBoardOrderCfg)-sizeof(std::map<std::wstring, std::wstring>));
	}
};
struct C_KeyBoardFontInfoCfg 
{
	LOGFONT			lgBtnFont;			//按钮字体
	LOGFONT			lgCheckFont;		//CheckBox字体
	LOGFONT			lgStaticFont1;		//静态文本字体
	LOGFONT			lgStaticFont2;		//状态提示字体
	LOGFONT			lgComboFont;		//可编辑字体
	LOGFONT			lgPositionFont;		//持仓字体

	int				iLeftMargin;		//控件左边距
	int				iTopMargin;			//控件顶边距

	int				iHorCtrlInterval;	//水平控件间距
	int				iVerCtrlInterval1;	//垂直控件间距1 静态文本与水平控件的间距
	int				iVerCtrlInterval2;	//垂直控件间距2 水平控件与状态提示的间距
	int				iVerCtrlInverval3;	//垂直控件间距2 按钮之间的垂直间距

	COLORREF		rgbBkPosition;		//持仓的背景色
	COLORREF		rgbBuy;				//多头的持仓颜色
	COLORREF		rgbSell;			//空头的持仓颜色

	C_KeyBoardFontInfoCfg()
	{
		memset(this, 0, sizeof(C_KeyBoardFontInfoCfg));
	}
};

struct C_OrderField
{
	int		id;						// 字段编号，对应的TradeOrderField里的内容 FutureInputArea
	int		width;					// 显示宽度(像素数) 自动算出 跟配置无关
	int		height;					// 显示宽度(像素数) 自动算出 跟配置无关
	bool	bShow;					// 是否显示
	C_OrderField()
	{
		memset(this, 0, sizeof(C_OrderField));
	}
};

struct C_KeyBoardSeqCfg
{
	int				nFieldCount;
	C_OrderField	*pOrderField;
	C_KeyBoardSeqCfg()
	{
		memset(this, 0, sizeof(C_KeyBoardSeqCfg));
	}
};

class IKeyOrderCfg
{
public:
	virtual void __cdecl OnKeyBoardCfgChange(const C_KeyBoardOrderCfg* kbcfg) = 0;

	virtual void __cdecl OnKeyBoardCfgFontChange(const C_KeyBoardFontInfoCfg* kbcfg) = 0;

	virtual void __cdecl OnKeyBoardSeqChange(const C_KeyBoardSeqCfg* kbcfg) = 0;
};