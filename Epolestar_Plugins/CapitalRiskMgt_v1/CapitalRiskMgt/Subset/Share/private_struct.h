#pragma once

#include "_base.h"
#include "trade_const.h"
#include "RawTradeApi_API.h"

#pragma pack(push, 1)


//�������ݻص�
struct TTradeData
{	
	uchar					data_type;		//��������
	TBool					is_last;		//�Ƿ����һ�� bYes:�ǣ� bYes:��
	int						error_id;		//������

	const void * 			pdata;			//����
	uint					data_size;		//���ݴ�С�����ݻ���ʱ��Ҫ
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