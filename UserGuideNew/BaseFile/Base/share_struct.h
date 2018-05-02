
#pragma once
#include <windows.h>
#include <string>
#include <vector>
using namespace std;

typedef vector<string> CStringsVec;

struct SOFCommodityInf
{
	char		Sign[21];
	bool		bCanNegative;					//小数时是否能够为负数 
	bool		bIfFractionPrice;				//是否分数报价
	char		cCoverMode;						//根据此来决定是否显示开平界面
	double		dMiniChangePrice;				//bIfFractionPrice若为1代表分数的分子
	int			iPrecision;						//bIfFractionPrice若为1代表分数的分母
	uint		cUserIdentity;					//交易登陆用户身份，是在登陆回调函数和初始化窗口中赋值,默认为单客户(uiUser) 下单人(uiProxy) 经纪人(uiBroker) 交易员(uiTrader) 行情客户(uiQUser)
	bool		bAddOne;
public:
	SOFCommodityInf()
	{
		Default();
	}
	void Default()
	{
		memset(this, 0, sizeof(SOFCommodityInf));
		//默认
		dMiniChangePrice = 1.00;
		iPrecision = 1;
		cCoverMode = cmCover;
	}
};
struct SFractionPrice
{
	int	iInteger;			//整数部分
	int	iMolecules;			//分子部分
	int	idenominator;		//分母部分
public:
	SFractionPrice() :iInteger(0), iMolecules(0), idenominator(0){}
};
struct PosCount
{
	//第一腿持买持卖 第二腿持买持卖
	uint BuyOpenCount;	//买入开仓 持仓量(第一腿持买) 第一腿买开 第二腿卖平 
	uint BuyCoverCount;	//买入平仓 持仓量(第二腿持卖) 第一腿平开 第二腿卖开

	uint SellOpenCount;	//卖出开仓 持仓量(第一腿持卖) 第一腿卖开 第二腿买平	
	uint SellCoverCount;//卖出平仓 持仓量(第二腿持买)  第一腿卖平 第二腿买开
public:
	PosCount()
	{
		memset(this, 0, sizeof(PosCount));
	}
};
struct NoticeContent
{
	string strKey;
	string strContent;
};

//可平 非互换
struct CanCoverQty
{
	uint uCanCoverQty;
	uint uCanCoverTQty;
public:
	CanCoverQty()
	{
		memset(this, 0, sizeof(CanCoverQty));
	}
};

//排序
class UserNoOrder
{
public:
	bool operator()(CStringsVec &vecStr1, CStringsVec &vecStr2)
	{
		return vecStr1[0] < vecStr2[0];
	}
};

//交易字段类型
enum TradeFieldType
{
	TFT_STRING,
	TFT_NUMERIC,
	TFT_DATE
};

//期货下单区域字段 //新增的只能往后加 不能从中间插入 否则会导致与老客户版本不兼容 中间插入导致IAF_ACCOUNT与配置文件保存的值不对应
enum FutureInputArea
{
	IAF_TJCHECK,			//条件选择区 不能参与排序
	IAF_ACCOUNT,			//客户账号
	IAF_COMMODITY,			//合约代码
	IAF_DIRECT,				//买卖
	IAF_OFFSET,				//开平
	IAF_PRICE,				//委托价格  后面加上 + 分子 /分母 四个 这四个要显示必须跟在委托价格之后
	IAF_PRICE_ADD,
	IAF_PRICE_FENZI,
	IAF_PRICE_CHUHAO,
	IAF_PRICE_FENMU,
	IAF_VOL,				//委托数量
	IAF_TRIGGERMODE,		//触发方式
	IAF_TRIGGERCONDITION,	//触发条件
	IAF_TRIGGERPRICE,		//触发价格
	IAF_VOLBL,				//数量倍率
	//IAF_STOPPRICE,			//止损价格
	//IAF_MATCHCONDITION,		//成交属性
	IAF_VALIDTIME,			//有效日期

	IAF_SELECODE,			//选择合约 始终显示
	IAF_TRIGGERPRICE_ADD,
	IAF_TRIGGERPRICE_FENZI,
	IAF_TRIGGERPRICE_CHUHAO,
	IAF_TRIGGERPRICE_FENMU,
	//IAF_ORDERPRICE_

	IAF_VOLCOUNT,		//下单次数  批次
	IAF_INCREVOL,		//数量增量

	IAF_INCREPRICE,			//价格增量 
	IAF_INCRERPRICE_CHUHAO,	//价格增量除号  显示必须跟在价格增量之后
	IAF_INCREPRICE_FENMU,	//价格增量分母  显示必须跟在价格增量除号之后

	IAF_ORDERTYPE,			//定单类型
	IAF_VALIDTYPE,			//有效类型

	IAF_MAXQTY,				//最大量 冰山
	IAF_MINQTY,				//最小量 冰山

	//IAF_BUTTON,				//设置按钮焦点所用
	//IAF_SUM 				//总数量，添加的字段请放置在前面
};
struct  FacadeSetting
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

	// 字段结构配置
	struct OrderField
	{
		int		id;						// 字段编号，对应的TradeOrderField里的内容 FutureInputArea
		int		width;					// 显示宽度(像素数) 自动算出 跟配置无关
		int		height;					// 显示宽度(像素数) 自动算出 跟配置无关
		//char	text[32];				//字段的标题  
		int		align;					//对齐方式，1：左对齐，2：居中，3：右对齐  暂时未用
		int		type;					//字段的类型也可以配置上，例如是字符串啊，还是数值，还是日期等等  TradeFieldType    暂时未用
		bool	bShow;					//是否显示
	};

	int				iOrderFieldCount;	//字段结构总数	
	OrderField		*pOrderField;		//字段结构指针

	unsigned int	iDefaultKbCfg;		//横向下单配置界面  //0==小字体 1==中字体 2==大字体

};


//new
typedef char TKUserType;
#define utForeign  'F'
#define utInner    'I'


struct TOrderViewData
{
	int     OrderQty;
	uint    iBuyPosQty;   //买持仓量
	uint    iSellPosQty;  //卖持仓量
	const   TContractKey*  pContractKey;
};

struct TQuoteData
{
	double BuyPrice;      //买价
	double SellPrice;     //卖价
	double LastPrice;     //最新价
	double UpPrice;       //涨停价
	double DownPrice;     //跌停价

	uint   BuyQty;
	uint   SellQty;
	uint   LastQty;
};



struct TMenuInfo
{
	uint Index;
	uint MenuID;
	const void *pData;
};

typedef char TRunState;
#define rsRun  'R'       //运行
#define rsStop 'S'       //停止
#define rsTriggered 'T'  //已触发

typedef char  TConOrderType;
#define cotPrice	'P'
#define cotTime		'T'
#define cotPreOrder 'p'
#define cotAutoOrder 'O'

struct TConditionOrder
{
	char				InsertTime[20];
	char				UserNo[21];
	TRunState			RunState;
	TConOrderType		OrderType;
	TOrderType			RealOrderType;
	bool            	Triggered;
	const TContract*	pContract;
	double      		ConPrice;      //价格条件
	int					ConPriceType;
	TDirect     		Direct;
	TOffset     		Offset;
	uint        		iQty;
	uint        		PriceType;     //下单价格类型
	double        		Price;         //价格条件
	int					OrderPriceOffset;
	int                 OrderID;
	int					OrderID2;
	TTriggerCondition   TriggerCondition;
	char				ContractId[51];
	double				dMinChangePrice;
	uint				iPrecision;
};




#define ptQueuePrice   0                //排队价
#define ptCounterPrice 1                //对价
#define ptSuperPrice   2                //超价
#define ptMarketPrice  3                //市价
#define ptLastPrice    4                //最新价
#define ptSpecifiedPrice 5				//指定价
#define ptTrigPrice    6


struct TrigPriceInfo
{
	char BuyPriceType;
	char SellPriceType;
	char OrderPriceType;

	int BuyOffset;
	int SellOffset;
	int OrderOffset;
	char Key[21];
};

