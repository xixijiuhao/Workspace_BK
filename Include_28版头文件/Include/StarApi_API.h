#ifndef _STARAPI_API_
#define _STARAPI_API_

const wchar_t PLUG_STARAPI_NAME[] = L"StarApi";
const wchar_t PLUG_STARAPI_VERSION[] = L"9.3.3.3";

#include "StarApi_TYPE.h"
//========================================================================================================================================================
//行情服务回调
class ISQuoteEventSpi
{
public:
	//连接成功
	virtual void __stdcall OnConnect() = 0;
	//连接断开
	virtual void __stdcall OnDisconnect() = 0;
	//登录应答
	virtual void __stdcall OnLogin(const SUserNoType user, const SErrCodeType err) = 0;
	//初始化完成（第二次登录不再初始化）
	virtual void __stdcall OnInitComplete() = 0;
};

//行情数据回调
class ISQuoteDataSpi
{
public:
	//普通行情数据 注意深度行情回调时 quote = NULL
	virtual void __stdcall OnQuote(const SContract* cont, const SQuoteUpdate* quote) = 0;
	//套利行情数据
	virtual void __stdcall OnSpreadQuote(const SSpreadContract* scont) = 0;
};

//历史行情回调
class ISHisQuoteSpi
{
public:
	//历史行情通知 仅通知数据结束变化，使用get函数获取数据
	virtual void __stdcall OnHisQuote(const SContractNoType contractno, const SKLineTypeType klinetype, const SSessionIdType sessionid) = 0;

	//历史波动率通知 仅通知数据结束变化，使用get函数获取数据
	virtual void __stdcall OnHisVolatility(const SContractNoType contractno, const SKLineTypeType klinetype, const SSessionIdType sessionid) = 0;
};

//业务接口--------------------------------------------------------------------------------------------------------------
class IStarApi
{
public:
	//可注册和取消多个
	virtual SRetType __stdcall RegEventSpi(ISQuoteEventSpi* spi) = 0;
	virtual SRetType __stdcall UnregEventSpi(ISQuoteEventSpi* spi) = 0;

	//开始自动登录，二次登录，是否准备好，切换地址，获取连接地址
	virtual SRetType __stdcall AutoLogin() = 0;
	virtual SRetType __stdcall ReLogin(const SUserNoType user, const SPassType pass) = 0;
	virtual SRetType __stdcall IsReady() = 0; //0-失败 1-实名登录 2-匿名登录
	virtual SRetType __stdcall GetCurrQuoteDest(SQuoteLink* link) = 0;
	virtual SRetType __stdcall GetCurrHisQuoteDest(SQuoteLink* link) = 0;
	////接入服务获取（包号 交易地址 板块 板块品种） ---------------------------------------------------------------------------------

	////获取当前包号
	//virtual SRetType __stdcall GetPackageNo(SPackageNoType pkgno) = 0;

	////获取配置控制信息
	//virtual SRetType __stdcall GetConfigSwitchCount() = 0;
	//virtual SRetType __stdcall GetConfigSwitchInfo(const SConfigSwitchNoType begin_configno, SConfigSwitchInfo* out[], int len, bool next) = 0;

	////获取直连交易地址
	//virtual SRetType __stdcall GetDirectTradeAddrCount() = 0;
	//virtual SRetType __stdcall GetDirectTradeAddr(const SCompanyNoType begin_company, const SSiteAddrNoType begin_site, 
	//												SDirectTradeLink out[], SLenType len, SNextType next) = 0;

	////行情板块
	//virtual SRetType __stdcall GetPlateInfoCount() = 0;
	//virtual SRetType __stdcall GetPlateInfo(const SIndexType begin_index, SPlate* out[], SLenType len) = 0;

	////板块品种
	//virtual SRetType __stdcall GetPlateCodeCount(const SPlateNoType plateno) = 0;
	//virtual SRetType __stdcall GetPlateCode(const SPlateNoType plateno, const SIndexType begin_index, SPlateContent* out[], SLenType len) = 0;

	//交易所 品种 合约 币种--------------------------------------------------------------------------------------------------------

	//交易所
	virtual SRetType __stdcall GetExchangeCount() = 0;
	virtual SRetType __stdcall GetExchangeData(const SExchangeNoType begin, SExchange* out[], int len, bool next) = 0;

	//品种
	virtual SRetType __stdcall GetCommodityCount(const SExchangeNoType exchangeno) = 0;
	virtual SRetType __stdcall GetCommodityData(const SExchangeNoType exchangeno, const SCommodityNoType begin, SCommodity* out[], int len, bool next) = 0;

	//合约
	virtual SRetType __stdcall GetContractCount(const SCommodityNoType commodityno) = 0;
	virtual SRetType __stdcall GetContractData(const SCommodityNoType commodityno, const SContractNoType begin, SContract* out[], int len, bool next) = 0;
	virtual SContract* __stdcall GetContractUnderlay(const SContractNoType contractno) = 0;
	virtual SRetType __stdcall GetContractSrc(const SContractNoType contractno, SContractNoType srccontractno) = 0;
	virtual SRetType __stdcall AddContract(const SContractNoType contractno) = 0;
	virtual SRetType __stdcall GetContractCode(const SContractNoType contractno, SContractCodeType code) = 0;
	virtual SRetType __stdcall GetContractName(const SContractNoType contractno, SContractNameType name) = 0;
	
	//币种
	virtual SRetType __stdcall GetCurrencyCount() = 0;
	virtual SRetType __stdcall GetCurrencyData(const SCurrencyNoType begin, SCurrency* out[], int len, bool next) = 0;

	//期权系列---------------------------------------------------------------------------------------------------------------------

	//期权交易所
	virtual SRetType __stdcall GetOptionExchangeCount() = 0;
	virtual SRetType __stdcall GetOptionExchangeData(const SExchangeNoType begin, SExchange* out[], int len, bool next) = 0;

	//期权品种
	virtual SRetType __stdcall GetOptionCommodityCount(const SExchangeNoType exchangeno) = 0;
	virtual SRetType __stdcall GetOptionCommodityData(const SExchangeNoType exchangeno, const SCommodityNoType begin, SCommodity* out[], int len, bool next) = 0;

	//期权系列
	virtual SRetType __stdcall GetOptionSeriesCount(const SCommodityNoType commodityno) = 0;
	virtual SRetType __stdcall GetOptionSeriesData(const SCommodityNoType commodityno, const SOptionSeriesNoType begin, SOptionSeries* out[], int len, bool next) = 0;
	virtual SRetType __stdcall GetOptionSeriesTarget(const SOptionSeriesNoType seriesno, const SContractNoType begin, SContract* out[], int len, bool next) = 0;
	virtual SRetType __stdcall GetOptionSeriesName(const SOptionSeriesNoType seriesno, SOptionSeriesNameType name) = 0;

	//期权系列合约对
	virtual SRetType __stdcall GetOptionContractPairCount(const SOptionSeriesNoType seriesno) = 0;
	virtual SRetType __stdcall GetOptionContractPairData(const SOptionSeriesNoType seriesno, const SStrikePriceType begin, SOptionContractPair* out[], int len, bool next) = 0;
	virtual SRetType __stdcall GetOptionContractPairData(const SOptionSeriesNoType seriesno, const int begin, SOptionContractPair* out[], int len) = 0;

	//带附加参数的期权价格计算函数 dv是波动率增加量，dd是到期日增加量
	virtual SRetType __stdcall CalcOptionPriceWithParam(const SOptionSeriesNoType seriesno, const SStrikePriceType strikeprice, const SPriceType targetprice, const SVolatilityType dv, const SDaysType dd, SOptionPriceRet& result) = 0;

	//即时行情---------------------------------------------------------------------------------------------------------------------

	//订阅和退订行情，组合订阅退订-分割
	virtual SRetType __stdcall SubQuote(const SContractNoType contractno[], int len, ISQuoteDataSpi* spi) = 0;
	virtual SRetType __stdcall UnsubQuote(const SContractNoType contractno[], int len, ISQuoteDataSpi* spi) = 0;
	virtual SRetType __stdcall UnsubAllQuote(ISQuoteDataSpi* spi) = 0;

	//获取快照
	virtual SRetType __stdcall GetSnapshot(const SContractNoType contractno, SQuoteSnapShot* snapshot) = 0;
	virtual SRetType __stdcall GetSnapshotL2(const SContractNoType contractno, SQuoteSnapShotL2* buy, SQuoteSnapShotL2* sell) = 0;

	//获取隐含价格
	virtual SRetType __stdcall GetImpliedField(const SContract* contract, const SFidMeanType fidmean, SQuoteField* implied) = 0;	//仅计算买卖1档
	virtual SRetType __stdcall GetImpliedSnapshot(const SContract* contract, SQuoteSnapShotL2* buy, SQuoteSnapShotL2* sell, SImpliedDepthL2& implied) = 0;

	//格式化价格
	virtual SRetType __stdcall FormatPrice(const SPriceType price, const SCommodityPrecType prec, const SCommodityDenoType deno, SPriceStrType out, bool showdeno, bool isthousand) = 0;
	virtual SRetType __stdcall FormatPrice(const SPriceType price, const SCommodity* commodity, SPriceStrType out, bool showdeno, bool isthousand) = 0;

	//历史行情---------------------------------------------------------------------------------------------------------------------

	//获取基础时间模版，基础模版数量为零，则此品种无历史数据，自定义组合请使用第一腿品种的模版
	virtual SRetType __stdcall GetBaseTimeBucketCount(const SCommodityNoType commodityno) = 0;
	virtual SHisQuoteTimeBucket* __stdcall GetBaseTimeBucketData(const SCommodityNoType commodityno, const STimeBucketIndexType index) = 0;

	//获取计算时间模版
	virtual SRetType __stdcall GetCalTimeBucketCount(const SCommodityNoType commodityno) = 0;
	virtual SHisQuoteTimeBucket* __stdcall GetCalTimeBucketData(const SCommodityNoType commodityno, const STimeBucketIndexType index) = 0;

	//订阅历史数据，0表示合约无效，1表示成功 等待sessionid结束，2表示订阅已存在
	virtual SRetType __stdcall SubHisQuote(const SContractNoType contractno, const SKLineTypeType klinetype, const SKLineSliceType klineslice,
		const SKLineCountType reqcount, SSessionIdType& sessionid, ISHisQuoteSpi* spi) = 0;
	virtual SRetType __stdcall UnsubHisQuote(const SContractNoType contractno, ISHisQuoteSpi* spi) = 0;

	//获取本地历史数据，end参数结尾数据为0，返回数据data按照正向数据保存，此处可能后续调整
	virtual SRetType __stdcall GetHisQuote(const SContractNoType contractno, const SKLineTypeType klinetype, const SKLineSliceType klineslice,
		const SKLineSampleRateType samplerate, const SKLineIndexType endindex, SHisQuoteData rdata[], int rsize) = 0;

	//重载指定合约的历史数据
	virtual SRetType __stdcall ReloadHisQuote(const SContractNoType contractno, const SKLineTypeType klinetype, SSessionIdType& sessionid) = 0;

	//波动率历史数据---------------------------------------------------------------------------------------------------------------

	//订阅历史数据，0表示合约无效，1表示成功 等待sessionid结束，2表示订阅已存在
	virtual SRetType __stdcall SubHisVolatility(const SContractNoType contractno, const SKLineTypeType klinetype, const SKLineSliceType klineslice,
		const SKLineCountType reqcount, SSessionIdType& sessionid, ISHisQuoteSpi* spi) = 0;
	virtual SRetType __stdcall UnsubHisVolatility(const SContractNoType contractno, ISHisQuoteSpi* spi) = 0;

	//获取本地历史数据，end参数结尾数据为0，返回数据data按照正向数据保存，此处可能后续调整
	virtual SRetType __stdcall GetHisVolatility(const SContractNoType contractno, const SKLineTypeType klinetype, const SKLineSliceType klineslice,
		const SKLineSampleRateType samplerate, const SDateTimeType endtime, SHisVolatilityData rdata[], int rsize) = 0;

	//套利合约操作-----------------------------------------------------------------------------------------------------------------

	//增加、删除 
	virtual SRetType __stdcall AddSpreadInfo(const SAddSPreadInfo &info) = 0;
	virtual SRetType __stdcall DelSpreadInfo(const SContractNoType contractno) = 0;

	//获取套利合约参数
	virtual SRetType __stdcall GetSpreadInfoCount(SSpreadSrcType filter = S_SPREADSRC_NONE) = 0;
	virtual SRetType __stdcall GetSpreadInfo(const SContractNoType begin, SSpreadContract out[], int len, bool next, SSpreadSrcType filter = S_SPREADSRC_NONE) = 0;

	//获取汇率折抵参数
	virtual SRetType __stdcall GetMortgageCount() = 0;
	virtual SRetType __stdcall GetMortgageData(const SCurrencyNoType mortgagebegin, const SCurrencyNoType currencybegin, SMortgageData* out[], int len, bool next) = 0;

	//更新 27版仅开放优先腿2018-1-5
	virtual SRetType __stdcall UpdateSpreadInfo(const SContractNoType spdContractNo,const SAddSPreadInfo &info) = 0;

	//获取自设套利时间模版2018-1-5
	virtual SRetType __stdcall GetSelfSpdBaseTimeBucketCount(const SContractNoType contractno) = 0;
	virtual SHisQuoteTimeBucket* __stdcall GetSelfSpdBaseTimeBucketData(const SContractNoType contractno, const STimeBucketIndexType index) = 0;

	//获取收费品种
	virtual SRetType __stdcall GetChargeCommodityCount() = 0;
	virtual SRetType __stdcall GetChargeCommodityData(const SCommodityNoType begin, SCommodityNoType out[], int len, bool next) = 0;

	//获取个股期货每手乘数 到合约
	virtual SRetType __stdcall GetStockContData(const SContractNoType cont, SCommodityDotType &dot) = 0;
};

#endif