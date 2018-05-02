#pragma once
	
#include "_trade_spi_switch.h"

//实现交易数据回调的路由
class TraderSpiSwitch: public ITradeSpiSwitch
{
private:
	bool m_bFilterNullData;					//是否过滤空数据
	inline bool ValidData(void * pdata)
	{
		return  pdata || !m_bFilterNullData;
	}
public:
	TraderSpiSwitch(bool bFilterNullData = true): m_bFilterNullData(bFilterNullData){};
	virtual bool __cdecl DoSwitch(ITradeSpi * spi, WPARAM wParam, LPARAM lParam)
	{
		if (spi==nullptr)
			return false;

		INT_PTR DataType	= 0;
		int ErrorCode		= 0;
		int Flag			= 0;
		TradeMsgFun::parse_lparam(lParam, DataType, Flag, ErrorCode);
		void * pData		= reinterpret_cast<void *>(wParam);

		switch (DataType)
		{
		case tntConnect			:
			if (ValidData(pData))
				spi->OnConnect((const TLogin *)pData);
			break;
		case tntDisconnect		:
			spi->OnDisconnect(ErrorCode, (const TLogin *)pData);
			break;
		case tntLogin			:
			spi->OnLogin(ErrorCode, (const TLogin *)pData);
			break;
		case tntLogout			:
			spi->OnLogout(ErrorCode, (const TLogin *)pData);
			break;
		case tntIniting			:
			spi->OnInitProgress(ErrorCode, (TradeInitProgress)Flag, (const TLoginBase *)pData);
			break;
		case tntOrder			:
			if (ValidData(pData))
				spi->OnOrder((const TOrder *)pData);
			break;
		case tntMatch			:
			if (ValidData(pData))
				spi->OnMatch((const TMatch *)pData);
			break;
		case tntPosition		:
			if (ValidData(pData))
				spi->OnPosition((const TPosition *)pData);
			break;
		case tntPositionSub		:
			if (ValidData(pData))
				spi->OnPositionSub((const TSubPosition *)pData);
			break;
		case tntPositionTotal	:
			if (ValidData(pData))
				spi->OnPositionTotal((const TTotalPosition *)pData);
			break;
		case tntLiquidate		:
			if (ValidData(pData))
				spi->OnLiquidate((const TLiquidate *)pData);
			break;
		case tntMoney			:
			if (ValidData(pData))
				spi->OnMoney((const TMoney *)pData);
			break;
		case tntMsg				:
			if (ValidData(pData))
				spi->OnMsg((const TMessage *)pData);
			break;
		case tntChangePsw		:
			spi->OnChgPsw(ErrorCode, (const TLoginBase * )pData);
			break;
		case tntBill			:
			if (ValidData(pData))
				spi->OnBill((const TBill *)pData);
			break;
		case tntHisDelivery		:
			if (pData)
				spi->OnHisDelivery((const TDelivery*)pData);
			break;
		case tntHisOrder		:
			if (ValidData(pData))
				spi->OnHisOrder((const TOrder*)pData);
			break;
		case tntHisOrderProc	:
			if (ValidData(pData))
				spi->OnHisOrderProc((const TOrder*)pData);
			break;
		case tntHisMatch		:
			if (ValidData(pData))
				spi->OnHisMatch((const TMatch*)pData);
			break;
		case tntHisCash			:
			if (ValidData(pData))
				spi->OnHisCash((const TCash*)pData);
			break;
		case tntEnquiry			:
			if (ValidData(pData))
				spi->OnEnquiry((const TEnquiry*)pData);
			break;
		case tntUserProxy		:
			if (ValidData(pData))
				spi->OnUserProxy((const TUserProxy*)pData);
			break;
		case tntPreFreeze		:
			if (ValidData(pData))
				spi->OnPreFrozen((const TPreFreeze *)pData);
			break;
		case tntTradableQty		:
			if (ValidData(pData))
				spi->OnTradableQty((const TTradableQty *)pData);
			break;			
		case tntBankBalance		:
			if (ValidData(pData))
				spi->OnBankBalance((const TBankBalance *)pData);
			break;
		case tntTransfer		:
			if (ValidData(pData))
				spi->OnTransfer((const TTransfer *)pData);
			break;
		case tntCfmmcToken		:
			if (ValidData(pData))
				spi->OnCfmmcToken((const TCfmmcToken *)pData);
			break;
		case tntHisPosition		:
			if (ValidData(pData))
				spi->OnHisPosition((const TPosition *)pData);
			break;
		case tntErrorInfo		:
			if (ValidData(pData))
				spi->OnErrorInfo((const TErrorInfo*)pData);
			break;
		case tntRefAllData		:
			if (ValidData(pData))
				spi->OnRefAllData((const TradeNotifyType)wParam);
			break;
		case tntFutureBalance:
			if (ValidData(pData))
				spi->OnFutureBalance((const TFutureBalance *)wParam);
			break;
		case tntDeepQuote:
			if (ValidData(pData))
				spi->OnSHFEDeepQuote((const TDeepQuote *)wParam);
			break;
		case tntExchangeState:
			if (ValidData(pData))
				spi->OnExchangeState((const TExchangeState *)wParam);
			break;
		//case tntOther			:
		default					:
			break;
		}
			
		return true;
	}
};