#ifndef _STARAPI_API_
#define _STARAPI_API_

const wchar_t PLUG_STARAPI_NAME[] = L"StarApi";
const wchar_t PLUG_STARAPI_VERSION[] = L"9.3.3.3";

#include "StarApi_TYPE.h"
//========================================================================================================================================================
//�������ص�
class ISQuoteEventSpi
{
public:
	//���ӳɹ�
	virtual void __stdcall OnConnect() = 0;
	//���ӶϿ�
	virtual void __stdcall OnDisconnect() = 0;
	//��¼Ӧ��
	virtual void __stdcall OnLogin(const SUserNoType user, const SErrCodeType err) = 0;
	//��ʼ����ɣ��ڶ��ε�¼���ٳ�ʼ����
	virtual void __stdcall OnInitComplete() = 0;
};

//�������ݻص�
class ISQuoteDataSpi
{
public:
	//��ͨ�������� ע���������ص�ʱ quote = NULL
	virtual void __stdcall OnQuote(const SContract* cont, const SQuoteUpdate* quote) = 0;
	//������������
	virtual void __stdcall OnSpreadQuote(const SSpreadContract* scont) = 0;
};

//��ʷ����ص�
class ISHisQuoteSpi
{
public:
	//��ʷ����֪ͨ ��֪ͨ���ݽ����仯��ʹ��get������ȡ����
	virtual void __stdcall OnHisQuote(const SContractNoType contractno, const SKLineTypeType klinetype, const SSessionIdType sessionid) = 0;

	//��ʷ������֪ͨ ��֪ͨ���ݽ����仯��ʹ��get������ȡ����
	virtual void __stdcall OnHisVolatility(const SContractNoType contractno, const SKLineTypeType klinetype, const SSessionIdType sessionid) = 0;
};

//ҵ��ӿ�--------------------------------------------------------------------------------------------------------------
class IStarApi
{
public:
	//��ע���ȡ�����
	virtual SRetType __stdcall RegEventSpi(ISQuoteEventSpi* spi) = 0;
	virtual SRetType __stdcall UnregEventSpi(ISQuoteEventSpi* spi) = 0;

	//��ʼ�Զ���¼�����ε�¼���Ƿ�׼���ã��л���ַ����ȡ���ӵ�ַ
	virtual SRetType __stdcall AutoLogin() = 0;
	virtual SRetType __stdcall ReLogin(const SUserNoType user, const SPassType pass) = 0;
	virtual SRetType __stdcall IsReady() = 0; //0-ʧ�� 1-ʵ����¼ 2-������¼
	virtual SRetType __stdcall GetCurrQuoteDest(SQuoteLink* link) = 0;
	virtual SRetType __stdcall GetCurrHisQuoteDest(SQuoteLink* link) = 0;
	////��������ȡ������ ���׵�ַ ��� ���Ʒ�֣� ---------------------------------------------------------------------------------

	////��ȡ��ǰ����
	//virtual SRetType __stdcall GetPackageNo(SPackageNoType pkgno) = 0;

	////��ȡ���ÿ�����Ϣ
	//virtual SRetType __stdcall GetConfigSwitchCount() = 0;
	//virtual SRetType __stdcall GetConfigSwitchInfo(const SConfigSwitchNoType begin_configno, SConfigSwitchInfo* out[], int len, bool next) = 0;

	////��ȡֱ�����׵�ַ
	//virtual SRetType __stdcall GetDirectTradeAddrCount() = 0;
	//virtual SRetType __stdcall GetDirectTradeAddr(const SCompanyNoType begin_company, const SSiteAddrNoType begin_site, 
	//												SDirectTradeLink out[], SLenType len, SNextType next) = 0;

	////������
	//virtual SRetType __stdcall GetPlateInfoCount() = 0;
	//virtual SRetType __stdcall GetPlateInfo(const SIndexType begin_index, SPlate* out[], SLenType len) = 0;

	////���Ʒ��
	//virtual SRetType __stdcall GetPlateCodeCount(const SPlateNoType plateno) = 0;
	//virtual SRetType __stdcall GetPlateCode(const SPlateNoType plateno, const SIndexType begin_index, SPlateContent* out[], SLenType len) = 0;

	//������ Ʒ�� ��Լ ����--------------------------------------------------------------------------------------------------------

	//������
	virtual SRetType __stdcall GetExchangeCount() = 0;
	virtual SRetType __stdcall GetExchangeData(const SExchangeNoType begin, SExchange* out[], int len, bool next) = 0;

	//Ʒ��
	virtual SRetType __stdcall GetCommodityCount(const SExchangeNoType exchangeno) = 0;
	virtual SRetType __stdcall GetCommodityData(const SExchangeNoType exchangeno, const SCommodityNoType begin, SCommodity* out[], int len, bool next) = 0;

	//��Լ
	virtual SRetType __stdcall GetContractCount(const SCommodityNoType commodityno) = 0;
	virtual SRetType __stdcall GetContractData(const SCommodityNoType commodityno, const SContractNoType begin, SContract* out[], int len, bool next) = 0;
	virtual SContract* __stdcall GetContractUnderlay(const SContractNoType contractno) = 0;
	virtual SRetType __stdcall GetContractSrc(const SContractNoType contractno, SContractNoType srccontractno) = 0;
	virtual SRetType __stdcall AddContract(const SContractNoType contractno) = 0;
	virtual SRetType __stdcall GetContractCode(const SContractNoType contractno, SContractCodeType code) = 0;
	virtual SRetType __stdcall GetContractName(const SContractNoType contractno, SContractNameType name) = 0;
	
	//����
	virtual SRetType __stdcall GetCurrencyCount() = 0;
	virtual SRetType __stdcall GetCurrencyData(const SCurrencyNoType begin, SCurrency* out[], int len, bool next) = 0;

	//��Ȩϵ��---------------------------------------------------------------------------------------------------------------------

	//��Ȩ������
	virtual SRetType __stdcall GetOptionExchangeCount() = 0;
	virtual SRetType __stdcall GetOptionExchangeData(const SExchangeNoType begin, SExchange* out[], int len, bool next) = 0;

	//��ȨƷ��
	virtual SRetType __stdcall GetOptionCommodityCount(const SExchangeNoType exchangeno) = 0;
	virtual SRetType __stdcall GetOptionCommodityData(const SExchangeNoType exchangeno, const SCommodityNoType begin, SCommodity* out[], int len, bool next) = 0;

	//��Ȩϵ��
	virtual SRetType __stdcall GetOptionSeriesCount(const SCommodityNoType commodityno) = 0;
	virtual SRetType __stdcall GetOptionSeriesData(const SCommodityNoType commodityno, const SOptionSeriesNoType begin, SOptionSeries* out[], int len, bool next) = 0;
	virtual SRetType __stdcall GetOptionSeriesTarget(const SOptionSeriesNoType seriesno, const SContractNoType begin, SContract* out[], int len, bool next) = 0;
	virtual SRetType __stdcall GetOptionSeriesName(const SOptionSeriesNoType seriesno, SOptionSeriesNameType name) = 0;

	//��Ȩϵ�к�Լ��
	virtual SRetType __stdcall GetOptionContractPairCount(const SOptionSeriesNoType seriesno) = 0;
	virtual SRetType __stdcall GetOptionContractPairData(const SOptionSeriesNoType seriesno, const SStrikePriceType begin, SOptionContractPair* out[], int len, bool next) = 0;
	virtual SRetType __stdcall GetOptionContractPairData(const SOptionSeriesNoType seriesno, const int begin, SOptionContractPair* out[], int len) = 0;

	//�����Ӳ�������Ȩ�۸���㺯�� dv�ǲ�������������dd�ǵ�����������
	virtual SRetType __stdcall CalcOptionPriceWithParam(const SOptionSeriesNoType seriesno, const SStrikePriceType strikeprice, const SPriceType targetprice, const SVolatilityType dv, const SDaysType dd, SOptionPriceRet& result) = 0;

	//��ʱ����---------------------------------------------------------------------------------------------------------------------

	//���ĺ��˶����飬��϶����˶�-�ָ�
	virtual SRetType __stdcall SubQuote(const SContractNoType contractno[], int len, ISQuoteDataSpi* spi) = 0;
	virtual SRetType __stdcall UnsubQuote(const SContractNoType contractno[], int len, ISQuoteDataSpi* spi) = 0;
	virtual SRetType __stdcall UnsubAllQuote(ISQuoteDataSpi* spi) = 0;

	//��ȡ����
	virtual SRetType __stdcall GetSnapshot(const SContractNoType contractno, SQuoteSnapShot* snapshot) = 0;
	virtual SRetType __stdcall GetSnapshotL2(const SContractNoType contractno, SQuoteSnapShotL2* buy, SQuoteSnapShotL2* sell) = 0;

	//��ȡ�����۸�
	virtual SRetType __stdcall GetImpliedField(const SContract* contract, const SFidMeanType fidmean, SQuoteField* implied) = 0;	//����������1��
	virtual SRetType __stdcall GetImpliedSnapshot(const SContract* contract, SQuoteSnapShotL2* buy, SQuoteSnapShotL2* sell, SImpliedDepthL2& implied) = 0;

	//��ʽ���۸�
	virtual SRetType __stdcall FormatPrice(const SPriceType price, const SCommodityPrecType prec, const SCommodityDenoType deno, SPriceStrType out, bool showdeno, bool isthousand) = 0;
	virtual SRetType __stdcall FormatPrice(const SPriceType price, const SCommodity* commodity, SPriceStrType out, bool showdeno, bool isthousand) = 0;

	//��ʷ����---------------------------------------------------------------------------------------------------------------------

	//��ȡ����ʱ��ģ�棬����ģ������Ϊ�㣬���Ʒ������ʷ���ݣ��Զ��������ʹ�õ�һ��Ʒ�ֵ�ģ��
	virtual SRetType __stdcall GetBaseTimeBucketCount(const SCommodityNoType commodityno) = 0;
	virtual SHisQuoteTimeBucket* __stdcall GetBaseTimeBucketData(const SCommodityNoType commodityno, const STimeBucketIndexType index) = 0;

	//��ȡ����ʱ��ģ��
	virtual SRetType __stdcall GetCalTimeBucketCount(const SCommodityNoType commodityno) = 0;
	virtual SHisQuoteTimeBucket* __stdcall GetCalTimeBucketData(const SCommodityNoType commodityno, const STimeBucketIndexType index) = 0;

	//������ʷ���ݣ�0��ʾ��Լ��Ч��1��ʾ�ɹ� �ȴ�sessionid������2��ʾ�����Ѵ���
	virtual SRetType __stdcall SubHisQuote(const SContractNoType contractno, const SKLineTypeType klinetype, const SKLineSliceType klineslice,
		const SKLineCountType reqcount, SSessionIdType& sessionid, ISHisQuoteSpi* spi) = 0;
	virtual SRetType __stdcall UnsubHisQuote(const SContractNoType contractno, ISHisQuoteSpi* spi) = 0;

	//��ȡ������ʷ���ݣ�end������β����Ϊ0����������data�����������ݱ��棬�˴����ܺ�������
	virtual SRetType __stdcall GetHisQuote(const SContractNoType contractno, const SKLineTypeType klinetype, const SKLineSliceType klineslice,
		const SKLineSampleRateType samplerate, const SKLineIndexType endindex, SHisQuoteData rdata[], int rsize) = 0;

	//����ָ����Լ����ʷ����
	virtual SRetType __stdcall ReloadHisQuote(const SContractNoType contractno, const SKLineTypeType klinetype, SSessionIdType& sessionid) = 0;

	//��������ʷ����---------------------------------------------------------------------------------------------------------------

	//������ʷ���ݣ�0��ʾ��Լ��Ч��1��ʾ�ɹ� �ȴ�sessionid������2��ʾ�����Ѵ���
	virtual SRetType __stdcall SubHisVolatility(const SContractNoType contractno, const SKLineTypeType klinetype, const SKLineSliceType klineslice,
		const SKLineCountType reqcount, SSessionIdType& sessionid, ISHisQuoteSpi* spi) = 0;
	virtual SRetType __stdcall UnsubHisVolatility(const SContractNoType contractno, ISHisQuoteSpi* spi) = 0;

	//��ȡ������ʷ���ݣ�end������β����Ϊ0����������data�����������ݱ��棬�˴����ܺ�������
	virtual SRetType __stdcall GetHisVolatility(const SContractNoType contractno, const SKLineTypeType klinetype, const SKLineSliceType klineslice,
		const SKLineSampleRateType samplerate, const SDateTimeType endtime, SHisVolatilityData rdata[], int rsize) = 0;

	//������Լ����-----------------------------------------------------------------------------------------------------------------

	//���ӡ�ɾ�� 
	virtual SRetType __stdcall AddSpreadInfo(const SAddSPreadInfo &info) = 0;
	virtual SRetType __stdcall DelSpreadInfo(const SContractNoType contractno) = 0;

	//��ȡ������Լ����
	virtual SRetType __stdcall GetSpreadInfoCount(SSpreadSrcType filter = S_SPREADSRC_NONE) = 0;
	virtual SRetType __stdcall GetSpreadInfo(const SContractNoType begin, SSpreadContract out[], int len, bool next, SSpreadSrcType filter = S_SPREADSRC_NONE) = 0;

	//��ȡ�����۵ֲ���
	virtual SRetType __stdcall GetMortgageCount() = 0;
	virtual SRetType __stdcall GetMortgageData(const SCurrencyNoType mortgagebegin, const SCurrencyNoType currencybegin, SMortgageData* out[], int len, bool next) = 0;

	//���� 27�������������2018-1-5
	virtual SRetType __stdcall UpdateSpreadInfo(const SContractNoType spdContractNo,const SAddSPreadInfo &info) = 0;

	//��ȡ��������ʱ��ģ��2018-1-5
	virtual SRetType __stdcall GetSelfSpdBaseTimeBucketCount(const SContractNoType contractno) = 0;
	virtual SHisQuoteTimeBucket* __stdcall GetSelfSpdBaseTimeBucketData(const SContractNoType contractno, const STimeBucketIndexType index) = 0;

	//��ȡ�շ�Ʒ��
	virtual SRetType __stdcall GetChargeCommodityCount() = 0;
	virtual SRetType __stdcall GetChargeCommodityData(const SCommodityNoType begin, SCommodityNoType out[], int len, bool next) = 0;

	//��ȡ�����ڻ�ÿ�ֳ��� ����Լ
	virtual SRetType __stdcall GetStockContData(const SContractNoType cont, SCommodityDotType &dot) = 0;
};

#endif