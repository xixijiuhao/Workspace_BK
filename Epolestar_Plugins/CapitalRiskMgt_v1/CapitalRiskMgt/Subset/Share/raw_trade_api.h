#pragma once

#include "PluginMgr_API.h"
#include "TradeApi_API.h"
#include "RawTradeApi_API.h"
#include "Logging_API.h"
#include "string_ex.h"
#include "crtdbg.h"
#include "log_type.h"


#define	EID_BASE			(-1)
#define	EID_Unsupport_Api	EID_BASE

class RawTradeApi : public IRawTradeApi
{
protected:
	IRawTradeSpi *	p_raw_spi;
	ITradeData *	p_trade_data;						//TradeApi
	ILoggingApi *	p_log;
	uint			m_log_fid;
	
	TLogin 			m_LoginUser;						//µÇÂ¼ÕÊºÅ
public:
	RawTradeApi(): p_raw_spi(nullptr){ }		
	virtual ~RawTradeApi(){ p_raw_spi = nullptr; }
	virtual int init(TPluginMgrApi * plug_manage)
	{
		_ASSERT(plug_manage);

		ITradeApi* p_trade_api = (ITradeApi*)plug_manage->CreatePluginApi(PLUG_TRADEAPI_NAME, PLUG_TRADEAPI_VERSION);
		if (p_trade_api)
			p_trade_data = p_trade_api->GetTradeData();
		_ASSERT(p_trade_data);

		p_log = (ILoggingApi*)plug_manage->CreatePluginApi(PLUG_LOGGINGAPI_NAME, PLUG_LOGGINGAPI_VERSION);
		if (p_log)
		{
			wstring log_file;
			StrFun::format(log_file, L"%s\\%s %08X %s.bin", FileFun::GetSelfNameW().c_str(), TimeFun::GetDateW().c_str(), GetCurrentProcessId(), FileFun::GetSelfVersionW().c_str());
			//m_log_fid = p_log->CreateLogging(log_file.c_str(), false, false, false);
			m_log_fid = p_log->CreateLogging(log_file.c_str(), true, false, false);
		}
	
		return 0;
	}	
	bool is_out(bool right, const char * fmt, ...)
	{
		if (right && p_log)
		{
			va_list ap;
			va_start(ap, fmt);
 
			char buffer[1024*4] = { 0 };
			int len = vsnprintf_s(buffer, sizeof(buffer), fmt, ap);
 
			va_end(ap);

			p_log->OutputLog(m_log_fid, ltString, llError, buffer, len);
		}
 
		return right;
	}
	template<typename RECORD>
	void log_data(TLogType log_data_type, RECORD & data)
	{
#ifdef _DEBUG
		p_log->OutputLog(m_log_fid, log_data_type, llInfo, (void*)&data, sizeof(data));
#endif
	}
	void log_str(const string & str)
	{
		p_log->OutputLog(m_log_fid, ltString, llInfo, (void*)str.c_str(), str.length());
	}
public:
	virtual int __cdecl set_spi(IRawTradeSpi * spi)										{ p_raw_spi = spi;	return 0; };	
	virtual const TLogin * __cdecl get_login_info()										{ return &m_LoginUser; };

	virtual int __cdecl open(const TLogin * pdata)										{ return EID_Unsupport_Api; };
	virtual int __cdecl close()															{ return EID_Unsupport_Api; };
	virtual int __cdecl ref_data()														{ return EID_Unsupport_Api; };
	virtual int __cdecl insert_order(const TSendOrder * pdata, const int session_id)	{ return EID_Unsupport_Api; };
	virtual int __cdecl modify_order(const TModifyOrder * pdata)						{ return EID_Unsupport_Api; };
	virtual int __cdecl cancel_order(const uint order_id)								{ return EID_Unsupport_Api; };
	virtual int __cdecl mark_order(const TOrder * pdata)								{ return EID_Unsupport_Api; };
	virtual int __cdecl suspend_order(const uint order_id)								{ return EID_Unsupport_Api; };
	virtual int __cdecl resume_order(const uint order_id)								{ return EID_Unsupport_Api; };
	virtual int __cdecl input_order(const TOrder * pdata)								{ return EID_Unsupport_Api; };
	virtual int __cdecl remove_order(const TOrder * pdata)								{ return EID_Unsupport_Api; };
	virtual int __cdecl input_match(const TInputMatch * pdata)							{ return EID_Unsupport_Api; };
	virtual int __cdecl remove_match(const TMatch * pdata)								{ return EID_Unsupport_Api; };
	virtual int __cdecl input_execute(const TExecuteInput * pdata)						{ return EID_Unsupport_Api; };
	virtual int __cdecl change_psw(const TLogin * pdata)								{ return EID_Unsupport_Api; };
	virtual int __cdecl agreement_sign()												{ return EID_Unsupport_Api; };
	virtual int __cdecl check_user_proxy(const TUserProxy * pdata)						{ return EID_Unsupport_Api; };
	virtual int __cdecl bank_transfer(const TTransferReq * pdata)						{ return EID_Unsupport_Api; };
	virtual int __cdecl get_tradeable_qty(const TSendOrder * pdata)						{ return EID_Unsupport_Api; };
	virtual int __cdecl get_pre_freeze(const TSendOrder * pdata)						{ return EID_Unsupport_Api; };
																								  
	virtual int __cdecl qry_money(const TUserInfo * pdata)								{ return EID_Unsupport_Api; };
	virtual int __cdecl qry_his_order(const THisTradeQry * pdata)						{ return EID_Unsupport_Api; };
	virtual int __cdecl qry_his_order_proc(const TOrder * pdata)						{ return EID_Unsupport_Api; };
	virtual int __cdecl qry_his_match(const THisTradeQry * pdata)						{ return EID_Unsupport_Api; };
	virtual int __cdecl qry_his_position(const THisTradeQry * pdata)					{ return EID_Unsupport_Api; };
	virtual int __cdecl qry_his_delivery(const THisTradeQry * pdata)					{ return EID_Unsupport_Api; };
	virtual int __cdecl qry_his_cash(const THisTradeQry * pdata)						{ return EID_Unsupport_Api; };
	virtual int __cdecl qry_bill(const TBillQry * pdata)								{ return EID_Unsupport_Api; };
	virtual int __cdecl qry_user_proxy()												{ return EID_Unsupport_Api; };
	virtual int __cdecl qry_position_desposit()											{ return EID_Unsupport_Api; };
	virtual int __cdecl qry_transfer_detail(const THisTradeQry * pdata)					{ return EID_Unsupport_Api; };
	virtual int __cdecl qry_bank_balance(const TBankBalanceQry * pdata)					{ return EID_Unsupport_Api; };
	virtual int __cdecl qry_future_balance(const TFutureBalanceQry * pdata)				{ return EID_Unsupport_Api; };
	virtual int __cdecl qry_shfe_deep_quote(const TContractKey * pdata)					{ return EID_Unsupport_Api; };
	virtual int __cdecl qry_exchange_state()											{ return EID_Unsupport_Api; };
	virtual int __cdecl qry_cfmmc_token(const TUserInfo * pdata)						{ return EID_Unsupport_Api; };
	virtual int __cdecl qry_order_check(const TOrderLinkInfoReq * pdata)				{ return EID_Unsupport_Api; };																							  
	virtual int __cdecl qry_calc_param(const TFundCalcParamReq * pdata)					{ return EID_Unsupport_Api; };

	virtual int __cdecl qry_verificationcode()											{ return EID_Unsupport_Api; };
	virtual int __cdecl check_verificationcode(const char* verification)				{ return EID_Unsupport_Api; };
};