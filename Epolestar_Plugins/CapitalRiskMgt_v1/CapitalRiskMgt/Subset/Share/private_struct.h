#pragma once

#include "_base.h"
#include "trade_const.h"
#include "RawTradeApi_API.h"

#pragma pack(push, 1)


//交易数据回调
struct TTradeData
{	
	uchar					data_type;		//数据类型
	TBool					is_last;		//是否最后一包 bYes:是， bYes:否
	int						error_id;		//错误码

	const void * 			pdata;			//数据
	uint					data_size;		//数据大小，数据缓存时需要
	IRawTradeApi *			psender;
public:
	TTradeData()
	{
		FillZero(this);
		is_last = bYes;
	}
	inline bool can_aggregation()
	{
		return data_type < tntIniting && data_type != tntAuthenticate && (data_type!=tntOrder || pdata==nullptr || ((TOrder*)pdata)->OrderState != osSended);
	}
};


#pragma pack(pop)