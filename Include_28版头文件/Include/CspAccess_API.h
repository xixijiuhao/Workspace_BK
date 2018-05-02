#ifndef _CSPACCESSAPI_API_
#define _CSPACCESSAPI_API_

const wchar_t PLUG_ACCESSAPI_NAME[] = L"CspAccessApi";
const wchar_t PLUG_ACCESSAPI_VERSION[] = L"9.3.0.1";

#include "StarApi_TYPE.h"

//接入服务回调
class ICspAccessSpi
{
public:
	//初始化完成 仅一次
	virtual void __stdcall OnInitCompleteCsp() = 0;
};


//业务接口--------------------------------------------------------------------------------------------------------------
class ICspAccessApi
{
public:
	//可注册和取消多个
	virtual SRetType __stdcall RegEventSpi(ICspAccessSpi* spi) = 0;
	virtual SRetType __stdcall UnregEventSpi(ICspAccessSpi* spi) = 0;

	//请求接入服务信息
	virtual SRetType __stdcall RequireAccessInfo(SLangType LangId) = 0;

	//获取当前包号
	virtual SRetType __stdcall GetPackageNo(SPackageNoType pkgno) = 0;

	//获取白名单
	virtual SRetType __stdcall GetWhiteListCount() = 0;
	virtual SRetType __stdcall GetWhiteListInfo(const SWhiteListNameType begin_configno, SWhiteListInfo* out[], int len, bool next) = 0;

	//获取配置控制信息
	virtual SRetType __stdcall GetConfigSwitchCount() = 0;
	virtual SRetType __stdcall GetConfigSwitchInfo(const SConfigSwitchNoType begin_configno, SConfigSwitchInfo* out[], int len, bool next) = 0;

	//获取行情地址
	virtual SRetType __stdcall GetQuoteAddrCount() = 0;
	virtual SRetType __stdcall GetQuoteAddr(const SCompanyNoType begin_company, const SSiteAddrNoType begin_site,
		SQuoteLink out[], SLenType len, SNextType next) = 0;

	//获取历史行情地址
	virtual SRetType __stdcall GetHisQuoteAddrCount() = 0;
	virtual SRetType __stdcall GetHisQuoteAddr(const SCompanyNoType begin_company, const SSiteAddrNoType begin_site,
		SQuoteLink out[], SLenType len, SNextType next) = 0;

	//行情板块
	virtual SRetType __stdcall GetPlateInfoCount() = 0;
	virtual SRetType __stdcall GetPlateInfo(const SIndexType begin_index, SPlate* out[], SLenType len) = 0;

	//板块品种
	virtual SRetType __stdcall GetPlateCodeCount(const SPlateNoType plateno) = 0;
	virtual SRetType __stdcall GetPlateCode(const SPlateNoType plateno, const SIndexType begin_index, SPlateContent* out[], SLenType len) = 0;

	//获取直连交易地址
	virtual SRetType __stdcall GetDirectTradeAddrCount() = 0;
	virtual SRetType __stdcall GetDirectTradeAddr(const SCompanyNoType begin_company, const SSiteAddrNoType begin_site,
		SDirectTradeLink out[], SLenType len, SNextType next) = 0;

	//获取屏蔽代码
	virtual SRetType __stdcall GetCodeForbidCount() = 0;
	virtual SRetType __stdcall GetCodeForbid(const SContractNoType begin_no, SContractNoType out[], SLenType len, SNextType next) = 0;

	//获取代码转换信息
	virtual SRetType __stdcall GetCodeMappingCount() = 0;
	virtual SRetType __stdcall GetCodeMapping(const SContractNoType begin_no, SCodeMappingInfo out[], SLenType len, SNextType next) = 0;

	//获取期权执行价参数
	virtual SRetType __stdcall  GetOptionPrecisionCount() = 0;
	virtual SRetType __stdcall  GetOptionPrecision(const SCommodityNoType begin_no, SOptionPrecisionInfo out[], SLenType len, SNextType next) = 0;
};

#endif