#ifndef _GLOBAL_DATA_H_
#define _GLOBAL_DATA_H_

#define SSWM_STATIC_BUTTON_CLICKDOWN   WM_USER + 1
#define SSWM_TAB_SWITCH_FOCUS          WM_USER + 2
#define SSWM_EDIT_PRESS_ENTER          WM_USER + 3
#define SSWM_CLICK_CHECKBOX			   WM_USER + 4
#define SSWM_ORDERBUTTON_UP            WM_USER + 5
#define SSWM_COMBOX_CHECK_CHANAGE      WM_USER + 6
#define SSWM_COMBOX_SELECT             WM_USER + 7
#define SSWM_TABTAG_SELECTED           WM_USER + 8
#define SSWM_HTTP_SPI					WM_USER + 9




#define UIDesgin_INIT   g_FontData.Init();g_ColorRefData.Init()


#define WM_RESETUSERNO                      WM_USER + 18
#define WM_NOTICE                           WM_USER + 19
#define WM_ONQUOTE                          WM_USER + 20
#define WM_POSITION                         WM_USER + 21
#define WM_SUPERPRICE                       WM_USER + 22
#define WM_ONCALLBACK                       WM_USER + 23
#define WM_BETALIMIT						WM_USER + 24

#define SDWM_ONCONTRACT_SELECT				WM_USER + 25  //self define window message
#define SDWM_CLICK_LISTBOX_ITEM				WM_USER + 26
#define MainFrmName L"class TMainFrame"
#define mulContractFilePath "\\Config\\MulContract.pri"

//#define 
#define atNone             'N'
#define atInnerOne        'o'
#define atForeignOne	  'O'
#define atOnlyInnerLess3   'i'
#define atOnlyForeignLess3 'f'
#define atInnerForeignLess3 's'
#define atOnlyInner  'I'
#define atOnlyForeign 'F'
#define atInnerForeign 'S'






extern G_COLOREF		g_ColorRefData;
extern G_UIFONT			g_FontData;
extern ITradeApi *		g_pTradeApi;
extern ITradeData *		g_pTradeData;
extern IMainFrame*		g_pMainFrm;
extern IStarApi*		g_pQuoteApi;
extern TPluginMgrApi*	g_pPluginMgrApi;
extern ILanguageApi*	g_pLanguageApi;
extern LanguageID		g_language;

enum StrID
{
	HighFrequencyStrID = 0,   //"0","高频15Min
	IntradayStrID, 			  //"1","当日30Min"
	ShortTermStrID,			  //"2","短期1Day"
	MidTermStrID,			  //"3","中期1Week"
	TitleStrID,				  //"4","策略"
	CommodityStrID,			  //"5","商品"
	ForeignExchangeStrID,	  //"6","外汇"
	NoDataTipStrID,			  //"7","AutoChartist数据源暂时没有该商品分析，请稍后重试"
	GettingDataStrID,		  //"8","正在获取AutoChartist数据……"
	InternalID,				  //"9","内盘"
	ExternalID,				  //"10","外盘"

	Commodity,
	ChartPattern,
	FibonacciPattern,
	KeyLevels,

	Exchange,				//15交易所
	Symbol,					//16商品
	Interval,				//17时段
	Pattern,				//18形态
	Length,					//19长度
	Direction,				//20方向
	IdentifiedTime,         //21发现时间
	Quality,				//22质量

	TrendChange,            //23趋势改变
	InitialTrend,           //24初始趋势
	Volume,					//25成交量
	Uniformity,				//26一致
	Clarity1,				//27清晰
	Breakout,				//28最小突破力度
	ForecastPrice1,          //29预计价格

	TimeSymmetry,           //30时间对称性
	PriceSymmetry,          //31比率对称性
	Clarity2,            //32清晰
	Target0,            //33目标0
	Target3,            //34目标3
	Target5,            //35目标5
	Target6,            //36目标6
	Target7,            //37目标7
	Target10,           //38目标10
	Target12,           //39目标12
	Target16,           //40目标16

	ResistanceLevel,	//41阻力位
	ForecastPrice2,		//42预计价格
	Type,				//43类型
	Completed,			//44
	Emerging,			//45
	Breakouts,			//46
	Approaches,			//47

	TradingOpportunities,   //48
	BigMovement,			//49
	ConsecutiveCandles,		//50

	CZCE,			//51
	DCE,			//52
	SHFE,			//53
	CFFEX,			//54

	FallingWedge,		   //55
	Pennant,			   //56
	RisingWedge,		   //57
	ChannelDown,		   //58
	Triangle,			   //59
	Flag,				   //60
	RectanglePatter,			   //61
	AscendingTriangle,	   //62
	ChannelUp,			   //63
	ThreePointExtension,   //64
	ABCD,				   //65
	Gartley,			   //66
	Resistance,			   //67
	Support				   //68
};



#endif