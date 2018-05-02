#ifndef _HISQUOTESHELL_API_
#define _HISQUOTESHELL_API_

const wchar_t PLUG_HISQUOTESHELL_NAME[] = L"HisQuoteShell";
const wchar_t PLUG_HISQUOTESHELL_VERSION[] = L"9.3.3.2";


#pragma pack(push, 1)

typedef int								ShellKLineIndexType;	
typedef unsigned long long				ShellDateTimeType;					//行情日期时间类型，到毫秒
typedef unsigned int					ShellDateType;						//行情日期类型
typedef unsigned long long				ShellQtyFieldType;	
typedef double							ShellPriceFieldType;	
typedef unsigned int					ShellLastQtyType;								//明细现手变化
typedef int								ShellPositionChgType;

const char KLINE_TYPE_TICK				= 'T';						//分笔明细
const char KLINE_TYPE_MINUTE			= 'M';						//分钟线
const char KLINE_TYPE_DAY				= 'D';						//日线

struct HisQuoteShellData
{
    ShellKLineIndexType					KLineIndex;                 //K线索引    tick每笔连续序号，min交易分钟序号，day无效
    ShellDateType						TradeDate;                  //交易日     tick无效，min可能和时间戳不同，day和时间戳相同
    ShellDateTimeType					DateTimeStamp;				//时间戳，不同数据类型，精度不同
    ShellQtyFieldType					QTotalQty;                  //行情快照 总成交量        
    ShellQtyFieldType					QPositionQty;				//行情快照 持仓量  
    ShellPriceFieldType					QLastPrice;					//最新价（收盘价）   
    
    union
    {
        struct
        {
            ShellQtyFieldType			QKLineQty;              //K线成交量     day  min     
            ShellPriceFieldType			QOpeningPrice;          //开盘价		day  min    
            ShellPriceFieldType			QHighPrice;				//最高价		day  min       
            ShellPriceFieldType			QLowPrice;				//最低价		day  min    
            ShellPriceFieldType			QSettlePrice;		    //结算价		day  min    
            
        };
        struct
        {
            ShellLastQtyType			QLastQty;               //明细现手      tick    
            ShellPositionChgType		QPositionChg;           //持仓量变化	tick
            ShellPriceFieldType			QBuyPrice;              //买价			tick        
            ShellPriceFieldType			QSellPrice;             //卖价			tick          
            ShellQtyFieldType			QBuyQty;                //买量			tick       
            ShellQtyFieldType			QSellQty;               //卖量			tick
        };
    };    
};

#pragma pack(pop)

class IHisQuoteShellSpi
{
public:
	virtual void __cdecl OnHisQuote(const char* contractid, const char klinetype, const unsigned int sessionid) = 0;
};

class IHisQuoteShellApi
{
public:
	virtual void RegSpi(IHisQuoteShellSpi* spi) = 0;

	//订阅历史数据，0表示合约无效，1表示成功 等待sessionid结束，2表示订阅已存在
	virtual int __cdecl SubHisQuote(const char* contractid, const char klinetype, unsigned int& sessionid) = 0;	
	virtual int __cdecl UnsubHisQuote(const char* contractid) = 0;	
	
	//获取本地历史数据，end参数结尾数据为0，返回数据data按照正向数据保存，此处可能后续调整
	virtual int __cdecl GetHisQuote(const char* contractid, const char klinetype, HisQuoteShellData rdata[], int rsize) = 0;

};















#endif