#ifndef _TRADE_QUOTE_STRING_
#define _TRADE_QUOTE_STRING_

typedef	int N_ExchangeType;
#define N_ZCE	0	
#define N_DCE	1	
#define N_SHFE	2	
#define N_CFFEX	3	
#define N_NONE	4	

typedef int		N_SwitchType;
#define NST_NONE	0	//无转换
#define NST_T2Q		1	//交易合约转行情合约
#define NST_Q2T		2	//行情合约转交易合约

typedef std::vector<const void *> VecVoidData;

class TDataContainer :public IDataContainer
{
public:
	//------------------------------继承begin--------------------------------------//
	virtual uint __cdecl size()
	{
		return m_VecData.size();
	}
	virtual const void * __cdecl at(uint i)
	{
		return m_VecData.at(i);
	}
	virtual void __cdecl clear()
	{
		m_VecData.clear();
	}
	virtual void __cdecl push_back(const void * data)
	{
		m_VecData.push_back(data);
	}
	//-------------------------------继承end---------------------------------------//

	//------------------------------begin--------------------------------------//
	VecVoidData & GetData()
	{
		return m_VecData;
	}
	//-------------------------------end---------------------------------------//
public:
	VecVoidData m_VecData;
};

typedef std::vector<int> VecIntData;

namespace CConvert
{
    //行情ID转交易关键字
    void SContractNoTypeToTradeContract(const char *szSign, const std::string &id, TContractKey &ContractKey);

    //行情品种类型转交易
    TCommodityType QuoteTypeToTradeType(char cType);
    //交易品种类型转行情
    TCommodityType TradeTypeToQuoteType(char cType);

    std::wstring UTF8ToUnicode(const std::string& str);

    //行情ID转品种关键字
    void SContractNoTypeToTradeCommodity(const char *szSign, const std::string &id, TCommodityKey &CommodityKey);

	 //初始化订单信息
	  void DefaultSendOrder(TSendOrder&SendOrder);//***
	 //LME判断
	  bool IsLME(const TExchangeKey&Exchange);
	 //判断是否是港交所期货
	  bool IsHK(const char *szCode);//*
	  //是否是内盘
	  bool IsInnerDiskExchange(const char *szExchange);
	  //获取交易所类型
	  N_ExchangeType GetExchangeType(const char* sExchangeNo);
	  //判断是否是组合合约
	  bool IsCombContract(const char CommodityType);
	  //是否期权合约
	  bool IsOptionContract(const char CommodityType);
	  //是否是行情合约
	  bool IsValidQuotContract(const  char * ContractId);
	  //是否是有效的行情品种
	  bool IsValidQuoteCommodity(const char * ExchangeNo,const char * CommodityNo);

	  //字符串转交易合约
	  bool TContractNoToTContractKey(const char * szCode, TContractKey & ContractKey);//保证传过来的字符串左右两端都去掉空格了***

	  //交易合约转字符串
      bool TContractKeyToTContractNo(const TContractKey & ContractKey, std::string & strCode);//
	  //期权交易合约转期权序列
	  bool TradeContractToOptionTarget(const TContractSingleKey &ContractKey, char(&ContractId)[101]);
	  //单腿交易合约转字符串 默认不带交易所
	  bool SingTradeContractToStr(std::string & strCode, const TContractSingleKey & SingContractKey, bool bHaveExchange = false, bool bHaveSign = true);
	  //单腿交易合约转字符串
	  bool PosContractToStr(std::string & strCode, const TContractSingleKey & SingContractKey);
	  //格式化交易合约结构体
	  void FormatTradeContract(const TContractKey & ContractKey, std::string &str);

      //行情品种ID转交易关键字
      bool SCommodityNoToTCommodityKey(SCommodityNoType commodityno, TCommodityKey &CommodityKey);

	  //交易合约转行情字符串
	  void TradeContractToSContractNoType(const TContractKey &ContractKey, char(&ContractId)[101]);
	  //交易合约转标的行情字符串
	  void TradeContractToRelateContractNoType(const TContractKey &ContractKey, char(&ContractId)[101]);
	  //行情合约转交易合约字符串
	  void SContractNoTypeToStr(std::string &sQuoteCode, std::string &str);
	  //交易字符串到行情id
	  bool StrToSContractNoType(const char *code, char(&ContractId)[101]);
	  //套利合约字符串分解
	  bool SpreadStrToSpreadInfo(const char *code, char& cRateMode, char(&sFirst)[101], char(&sSecond)[101], int N_SwitchType = NST_NONE);//ntype :0:  1: 2:Q2T
	  //填充期权字段
	  bool ParseOptionContractStr(const std::string& strValue, char(&ContractNo)[11], TOptionType &OptionType, char(&StrikePrice)[11]);

	  long Round(double Value);

	  //是否整除
	  bool IsDiv(const double Value, const double SubValue, const double ErrorDiff = 1e-10);
	  //正则匹配
	  bool MatchPattern(const char *pattern, const char *target);
	  //是否有效的数字
	  bool IsValidNumber(const char * sValue, bool bOnlyInt = false, bool bCanNegative = false);
	  //小数等值判断
	  bool DoubleSameValue(const double d1, const double d2, double Epsilon);

	  std::vector<std::string> SplitString(const std::string & str, char cSplit);

	  void TrimString(std::string &str);

	  SPriceType GetPriceByContract(const SContract* SContract, SFidMeanType nField,int nlevel = 0);//（含隐含）

	  SQtyType GetQtyByContract(const SContract* SContract, SFidMeanType nField, int nlevel = 0);//（含隐含）

	  SPriceType GetPriceBySnapShot(const SQuoteSnapShot* QuoteSnap, SFidMeanType nField);//
	  SPriceType GetPriceBySnapShot(const SQuoteSnapShotL2 QuoteSnap[2], SFidMeanType nField, int nlevel = 0);//

	  SQtyType GetQtyBySnapShot(const SQuoteSnapShot* QuoteSnap, SFidMeanType nField);//
	  SQtyType GetQtyBySnapShot(const SQuoteSnapShotL2 QuoteSnap[2], SFidMeanType nField, int nlevel = 0);//

      
      
};
#endif