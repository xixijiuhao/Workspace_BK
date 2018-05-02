#ifndef _TRADE_STATISTIC_API_
#define _TRADE_STATISTIC_API_
//#include "QuoteApi_API.h"
#include "TradeApi_API.h"
#include <tchar.h>

const wchar_t PLUG_TRADESTATISTIC_NAME[] = L"TradeStatisticApi";
const wchar_t PLUG_TRADESTATISTIC_VERSION[] = L"1.0.0.1";

const char WHOLE_CONTRACT[] ="whole";


enum ST_TYPE
{
	WHOLE=1,
	BUY,
	SELL,
};


enum TIME_PERIOD
{
	TIME_CHARPTER_1 = 0,  
	TIME_CHARPTER_2,	
	TIME_CHARPTER_3,	
	TIME_CHARPTER_4,	
	TIME_CHARPTER_5,
	TIME_CHARPTER_6,
	TIME_DAY,
	TIME_NIGHT,
	TIME_WHOLE_DAY,
};


class TradeStatisticAPI
{
public:
	virtual void  __cdecl Ts_Init()=0;
	virtual void  __cdecl Ts_UnInit()=0;
	virtual void __cdecl Get_Match_Data_File(IDataContainer & Datas)=0;
	virtual double __cdecl Ts_Get_Total_Profit(const char*contract_id,ST_TYPE _st_type,TIME_PERIOD _time)=0;
	virtual double __cdecl Ts_Get_Total_Commision(const char*contract_id,ST_TYPE _st_type,TIME_PERIOD _time)=0;
	virtual double __cdecl Ts_Get_Maxdrawdown(const char*contract_id,ST_TYPE _st_type,TIME_PERIOD _time)=0;
	virtual uint __cdecl Ts_Get_Count(const char*contract_id,ST_TYPE _st_type,TIME_PERIOD _time)=0;
	virtual uint __cdecl Ts_Get_Profit_Loss_Count(bool profit_loss,const char*contract_id,ST_TYPE _st_type,TIME_PERIOD _time)=0;
	virtual double __cdecl Ts_Get_Profit(const char*contract_id,ST_TYPE _st_type,TIME_PERIOD _time)=0;
	virtual double __cdecl Ts_Get_Loss(const char*contract_id,ST_TYPE _st_type,TIME_PERIOD _time)=0;
	virtual double __cdecl Ts_Get_PLRatio(const char*contract_id,ST_TYPE _st_type,TIME_PERIOD _time)=0;
	virtual double __cdecl Ts_Get_WinRate(const char*contract_id,ST_TYPE _st_type,TIME_PERIOD _time)=0;
	virtual double __cdecl Ts_Get_Biggest_Loss(const char*contract_id,ST_TYPE _st_type,TIME_PERIOD _time)=0;
	virtual double __cdecl Ts_Get_Biggest_Profit(const char*contract_id,ST_TYPE _st_type,TIME_PERIOD _time)=0;	
	virtual uint __cdecl Ts_Get_Consecutive_Loss_count(const char*contract_id,ST_TYPE _st_type,TIME_PERIOD _time)=0;
	virtual uint __cdecl Ts_Get_Consecutive_Win_count(const char*contract_id,ST_TYPE _st_type,TIME_PERIOD _time)=0;
	virtual double __cdecl Ts_Get_Consecutive_Win_Money(const char*contract_id,ST_TYPE _st_type,TIME_PERIOD _time)=0;
	virtual double __cdecl Ts_Get_Consecutive_Loss_Money(const char*contract_id,ST_TYPE _st_type,TIME_PERIOD _time)=0;	
	virtual double  __cdecl Ts_Get_Pos_Hold_Time(const char*contract_id, TIME_PERIOD _time)=0;
	virtual uint __cdecl Ts_Get_Profit_Max_Pos_Hold_Time(const char*contract_id, TIME_PERIOD _time)=0;
	virtual double __cdecl Ts_Get_Win_Pos_Hold_Time(const char*contract_id,TIME_PERIOD _time)=0;	
	virtual double __cdecl Ts_Get_Loss_Pos_Hold_Time(const char*contract_id,TIME_PERIOD _time)=0;	
	virtual uint __cdecl Ts_Get_Loss_Max_Pos_Hold_Time(const char*contract_id,TIME_PERIOD _time)=0;
	virtual uint __cdecl Ts_Get_Same_Consecutive_Loss_count(const char*contract_id,ST_TYPE _st_type,TIME_PERIOD _time)=0;
	virtual uint __cdecl Ts_Get_Cur_Consecutive_Loss_count(const char*contract_id,ST_TYPE _st_type,TIME_PERIOD _time)=0;
	//virtual uint __cdecl Ts_Get_Single_Interval_Count(uint interval,TIME_PERIOD _time)=0;
	//virtual uint __cdecl Ts_Get_Whole_Interval_Count(uint interval,TIME_PERIOD _time)=0;
		
private:

};

#endif
