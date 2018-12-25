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
	HighFrequencyStrID = 0,   //"0","��Ƶ15Min
	IntradayStrID, 			  //"1","����30Min"
	ShortTermStrID,			  //"2","����1Day"
	MidTermStrID,			  //"3","����1Week"
	TitleStrID,				  //"4","����"
	CommodityStrID,			  //"5","��Ʒ"
	ForeignExchangeStrID,	  //"6","���"
	NoDataTipStrID,			  //"7","AutoChartist����Դ��ʱû�и���Ʒ���������Ժ�����"
	GettingDataStrID,		  //"8","���ڻ�ȡAutoChartist���ݡ���"
	InternalID,				  //"9","����"
	ExternalID,				  //"10","����"

	Commodity,
	ChartPattern,
	FibonacciPattern,
	KeyLevels,

	Exchange,				//15������
	Symbol,					//16��Ʒ
	Interval,				//17ʱ��
	Pattern,				//18��̬
	Length,					//19����
	Direction,				//20����
	IdentifiedTime,         //21����ʱ��
	Quality,				//22����

	TrendChange,            //23���Ƹı�
	InitialTrend,           //24��ʼ����
	Volume,					//25�ɽ���
	Uniformity,				//26һ��
	Clarity1,				//27����
	Breakout,				//28��Сͻ������
	ForecastPrice1,          //29Ԥ�Ƽ۸�

	TimeSymmetry,           //30ʱ��Գ���
	PriceSymmetry,          //31���ʶԳ���
	Clarity2,            //32����
	Target0,            //33Ŀ��0
	Target3,            //34Ŀ��3
	Target5,            //35Ŀ��5
	Target6,            //36Ŀ��6
	Target7,            //37Ŀ��7
	Target10,           //38Ŀ��10
	Target12,           //39Ŀ��12
	Target16,           //40Ŀ��16

	ResistanceLevel,	//41����λ
	ForecastPrice2,		//42Ԥ�Ƽ۸�
	Type,				//43����
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