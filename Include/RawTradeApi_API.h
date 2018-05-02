#pragma once

#include "RawTradeApi_Struct.h"

//启明星
#define PLUG_DAYSTARTRADEAPI_NAME				L"DaystarTradeApi"
#define PLUG_DAYSTARTRADEAPI_VERSION			L"1.2.0.1"
//北斗星
#define PLUG_DIPPERTRADEAPI_NAME				L"DipperTradeApi"
#define PLUG_DIPPERTRADEAPI_VERSION				L"1.0.0.1"
//极星
#define PLUG_POLESTARTRADEAPI_NAME				L"PolestarTradeApi"
#define PLUG_POLESTARTRADEAPI_VERSION			L"1.0.0.1"
//CTP
#define PLUG_CTPTRADEAPI_NAME					L"CtpTradeApi"
#define PLUG_CTPTRADEAPI_VERSION				L"1.0.0.1"
//CTP mini
#define PLUG_CTPMINITRADEAPI_NAME				L"CtpminiTradeApi"
#define PLUG_CTPMINITRADEAPI_VERSION			L"1.0.0.1"
//CTP
#define PLUG_CTPETFTRADEAPI_NAME				L"CtpetfTradeApi"
#define PLUG_CTPETFTRADEAPI_VERSION				L"1.0.0.1"
//飞马
#define PLUG_FEMASTRADEAPI_NAME					L"FemasTradeApi"
#define PLUG_FEMASTRADEAPI_VERSION				L"1.0.0.1"
//金仕达
#define PLUG_KINGSTARTRADEAPI_NAME				L"KingstarTradeApi"
#define PLUG_KINGSTARTRADEAPI_VERSION			L"1.0.0.1"
//内盘8.0
#define PLUG_ESUNNYTRADEAPI_NAME				L"EsunnyTradeApi"
#define PLUG_ESUNNYTRADEAPI_VERSION				L"1.0.0.1"
//外盘3.0
#define PLUG_FOREIGNTRADEAPI_NAME				L"ForeignTradeApi"
#define PLUG_FOREIGNTRADEAPI_VERSION			L"1.0.0.1"
//雁联黄金现货 广发
#define PLUG_GOLDTRADEAPI_NAME					L"GoldTradeApi"
#define PLUG_GOLDTRADEAPI_VERSION				L"1.0.0.1"
//雁联黄金现货 深圳
#define PLUG_YLINKTRADEAPI_NAME					L"YlinkTradeApi"
#define PLUG_YLINKTRADEAPI_VERSION				L"1.0.0.1"
//飞鼠黄金现货
#define PLUG_FSEXTREMETRADEAPI_NAME				L"FSExtremeTradeApi"
#define PLUG_FSEXTREMETRADEAPI_VERSION			L"1.0.0.1"
//量投科技
#define PLUG_QDAMTRADEAPI_NAME					L"QdamTradeApi"
#define PLUG_QDAMTRADEAPI_VERSION				L"1.0.0.1"
//恒生
#define PLUG_HUNDSUNTRADEAPI_NAME				L"HundsunTradeApi"
#define PLUG_HUNDSUNTRADEAPI_VERSION			L"1.0.0.1"
//云端
#define PLUG_ESTARTRADEAPI_NAME					L"EstarTradeApi"
#define PLUG_ESTARTRADEAPI_VERSION				L"1.0.0.1"


/////////////////////////////////////////////////////////////////////////////
class IRawTradeApi;

class IRawTradeSpi
{
public:
	virtual void __cdecl on_connect(IRawTradeApi * raw_api, TLogin * pdata) = 0;
	virtual void __cdecl on_logout(IRawTradeApi * raw_api, TLogin * pdata, const int error_id = 0) = 0;
	virtual void __cdecl on_disconnect(IRawTradeApi * raw_api, TLogin * pdata, const int error_id = 0) = 0;
	virtual void __cdecl on_login_info(IRawTradeApi * raw_api, TLogin * pdata, const int error_id = 0) = 0;
	virtual void __cdecl on_login(IRawTradeApi * raw_api, TLogin * pdata, const int error_id = 0) = 0;
	virtual void __cdecl on_initing(IRawTradeApi * raw_api, TLogin * pdata, const int error_id = 0, const TBool is_last = bYes) = 0;
	virtual void __cdecl on_agreement_info(IRawTradeApi * raw_api, TAgreementInfo * pdata, const int error_id = 0) = 0;
	virtual void __cdecl on_user(IRawTradeApi * raw_api, TUserInfo * pdata, const int error_id = 0, const TBool is_last = bYes) = 0;
	virtual void __cdecl on_bank(IRawTradeApi * raw_api, TUserBank * pdata, const int error_id = 0, const TBool is_last = bYes) = 0;
	virtual void __cdecl on_bank_balance(IRawTradeApi * raw_api, TBankBalance * pdata, const int error_id = 0, const TBool is_last = bYes) = 0;
	virtual void __cdecl on_future_balance(IRawTradeApi * raw_api, TFutureBalance * pdata, const int error_id = 0, const TBool is_last = bYes) = 0;
	virtual void __cdecl on_transfer_detail(IRawTradeApi * raw_api, TTransfer * pdata, const int error_id = 0, const TBool is_last = bYes) = 0;
	virtual void __cdecl on_tradable_qty(IRawTradeApi * raw_api, TTradableQty * pdata, const int error_id = 0, const TBool is_last = bYes) = 0;
	virtual void __cdecl on_pre_freeze(IRawTradeApi * raw_api, TPreFreeze * pdata, const int error_id = 0, const TBool is_last = bYes) = 0;
	virtual void __cdecl on_cfmmc_token(IRawTradeApi * raw_api, TCfmmcToken * pdata, const int error_id = 0, const TBool is_last = bYes) = 0;
	virtual void __cdecl on_upper(IRawTradeApi * raw_api, TUpper * pdata, const int error_id = 0, const TBool is_last = bYes) = 0;
	virtual void __cdecl on_currency(IRawTradeApi * raw_api, TCurrency * pdata, const int error_id = 0, const TBool is_last = bYes) = 0;	
	virtual void __cdecl on_commodity(IRawTradeApi * raw_api, TCommodityKey * pdata, const int error_id = 0, const TBool is_last = bYes) = 0;	
	virtual void __cdecl on_3m_expdate(IRawTradeApi * raw_api, T3MEndDate * pdata, const int error_id = 0) = 0;
	virtual void __cdecl on_user_proxy(IRawTradeApi * raw_api, TUserProxy * pdata, const int error_id = 0, const TBool is_last = bYes) = 0;			
	virtual void __cdecl on_order(IRawTradeApi * raw_api, TOrder * pdata, const int error_id = 0, const TBool is_last = bYes) = 0;
	virtual void __cdecl on_match(IRawTradeApi * raw_api, TMatch * pdata, const int error_id = 0, const TBool is_last = bYes) = 0;
	virtual void __cdecl on_his_position(IRawTradeApi * raw_api, TPosition * pdata, const int error_id = 0, const TBool is_last = bYes) = 0;
	virtual void __cdecl on_position(IRawTradeApi * raw_api, TPosition * pdata, const int error_id = 0, const TBool is_last = bYes) = 0;
	virtual void __cdecl on_position_deposit(IRawTradeApi * raw_api, TPositionDeposit * pdata, const int error_id = 0, const TBool is_last = bYes) = 0;
	virtual void __cdecl on_liquidate(IRawTradeApi * raw_api, TLiquidate * pdata, const int error_id = 0, const TBool is_last = bYes) = 0;
	virtual void __cdecl on_money(IRawTradeApi * raw_api, TMoney * pdata, const int error_id = 0, const TBool is_last = bYes) = 0;
	virtual void __cdecl on_msg(IRawTradeApi * raw_api, TMessage * pdata, const int error_id = 0, const TBool is_last = bYes) = 0;
	virtual void __cdecl on_enquiry(IRawTradeApi * raw_api, TEnquiry * pdata, const int error_id = 0, const TBool is_last = bYes) = 0;
	virtual void __cdecl on_change_psw(IRawTradeApi * raw_api, TLogin * pdata, const int error_id = 0) = 0;
	virtual void __cdecl on_check_user_proxy(IRawTradeApi * raw_api, TUserProxy * pdata, const int error_id = 0) = 0;
	virtual void __cdecl on_bill(IRawTradeApi * raw_api, TBill * pdata, const int error_id = 0) = 0;
	virtual void __cdecl on_his_order(IRawTradeApi * raw_api, TOrder * pdata, const int error_id = 0, const TBool is_last = bYes) = 0;
	virtual void __cdecl on_his_order_proc(IRawTradeApi * raw_api, TOrder * pdata, const int error_id = 0, const TBool is_last = bYes) = 0;
	virtual void __cdecl on_his_match(IRawTradeApi * raw_api, TMatch * pdata, const int error_id = 0, const TBool is_last = bYes) = 0;
	virtual void __cdecl on_his_delivery(IRawTradeApi * raw_api, TDelivery * pdata, const int error_id = 0, const TBool is_last = bYes) = 0;
	virtual void __cdecl on_his_cash(IRawTradeApi * raw_api, TCash * pdata, const int error_id = 0, const TBool is_last = bYes) = 0;
	virtual void __cdecl on_error(IRawTradeApi * raw_api, TErrorInfo * pdata) = 0;
	virtual void __cdecl on_order_check(IRawTradeApi * raw_api, TOrderCheck * pdata, const int error_id = 0, const TBool is_last = bYes) = 0;
	virtual void __cdecl on_shfe_deep_quote(IRawTradeApi * raw_api, TDeepQuote * pdata, const int error_id = 0, const TBool is_last = bYes) = 0;
	virtual void __cdecl on_exechange_state(IRawTradeApi * raw_api, TExchangeState * pdata, const int error_id = 0, const TBool is_last = bYes) = 0;
	virtual void __cdecl on_calc_param(IRawTradeApi * raw_api, TFundCalcParam * pdata, const int error_id = 0, const TBool is_last = bYes) = 0;
	virtual void __cdecl on_order_price_range(IRawTradeApi * raw_api, TOrderPriceRange * pdata, const int error_id = 0, const TBool is_last = bYes) = 0;
};

//所有函数返回0则认为执行成功，否则认为执行失败
class IRawTradeApi
{
public:
	virtual int __cdecl set_spi(IRawTradeSpi * spi) = 0;
	virtual const TLogin * __cdecl get_login_info() = 0;

	virtual int __cdecl open(const TLogin * pdata) = 0;
	virtual int __cdecl close() = 0;
	virtual int __cdecl ref_data() = 0;
	virtual int __cdecl insert_order(const TSendOrder * pdata, const int session_id) = 0;
	virtual int __cdecl modify_order(const TModifyOrder * pdata) = 0;
	virtual int __cdecl cancel_order(const uint order_id) = 0;
	virtual int __cdecl mark_order(const TOrder * pdata) = 0;
	virtual int __cdecl suspend_order(const uint order_id) = 0;
	virtual int __cdecl resume_order(const uint order_id) = 0;
	virtual int __cdecl input_order(const TOrder * pdata) = 0;
	virtual int __cdecl remove_order(const TOrder * pdata) = 0;
	virtual int __cdecl input_match(const TInputMatch * pdata) = 0;
	virtual int __cdecl remove_match(const TMatch * pdata) = 0;
	virtual int __cdecl input_execute(const TExecuteInput * pdata) = 0;
	virtual int __cdecl change_psw(const TLogin * pdata) = 0;
	virtual int __cdecl agreement_sign() = 0;
	virtual int __cdecl check_user_proxy(const TUserProxy * pdata) = 0;
	virtual int __cdecl bank_transfer(const TTransferReq * pdata) = 0;
	virtual int __cdecl get_tradeable_qty(const TSendOrder * pdata) = 0;
	virtual int __cdecl get_pre_freeze(const TSendOrder * pdata) = 0;
	
	virtual int __cdecl qry_money(const TUserInfo * pdata) = 0;
	virtual int __cdecl qry_his_order(const THisTradeQry * pdata) = 0;
	virtual int __cdecl qry_his_order_proc(const TOrder * pdata) = 0;
	virtual int __cdecl qry_his_match(const THisTradeQry * pdata) = 0;
	virtual int __cdecl qry_his_position(const THisTradeQry * pdata) = 0;
	virtual int __cdecl qry_his_delivery(const THisTradeQry * pdata) = 0;
	virtual int __cdecl qry_his_cash(const THisTradeQry * pdata) = 0;
	virtual int __cdecl qry_bill(const TBillQry * pdata) = 0;
	virtual int __cdecl qry_user_proxy() = 0;
	virtual int __cdecl qry_position_desposit() = 0;
	virtual int __cdecl qry_transfer_detail(const THisTradeQry * pdata) = 0;
	virtual int __cdecl qry_bank_balance(const TBankBalanceQry * pdata) = 0;
	virtual int __cdecl qry_future_balance(const TFutureBalanceQry * pdata) = 0;
	virtual int __cdecl qry_shfe_deep_quote(const TContractKey * pdata) = 0;
	virtual int __cdecl qry_exchange_state() = 0;
	virtual int __cdecl qry_cfmmc_token(const TUserInfo * pdata) = 0;
	virtual int __cdecl qry_order_check(const TOrderLinkInfoReq * pdata) = 0;
	virtual int __cdecl qry_calc_param(const TFundCalcParamReq * pdata) = 0;
	
	virtual int __cdecl qry_verificationcode() = 0;
	virtual int __cdecl check_verificationcode(const char* verification) = 0;
};