#ifndef _TRADE_QUOTE_STRING_
#define _TRADE_QUOTE_STRING_

typedef int		N_SwitchType;
#define NST_NONE	0	//无转换
#define NST_T2Q	1		//交易合约转行情合约
#define NST_Q2T	2		//行情合约转交易合约

namespace CConvert
{
	//初始化订单信息
	  void DefaultSendOrder(TSendOrder&SendOrder);//***
	//LME判断
	  bool IsLME(const TExchangeKey&Exchange);
	//判断是否是港交所期货
	  bool IsHK(const char *szCode);//*
	//是否是内盘
	  bool IsInnerDiskExchange(const char *szExchange);
	  //判断内外盘
	  int  ReadVersionType();
	  //判断是否是组合合约
	  bool IsCombContract(const char CommodityType);
	  //是否期权合约
	  bool IsOptionContract(const char CommodityType);
      //判断是否是有效的交易品种
	  bool IsValidTradeCommodity(const TCommodityKey & ContractKey);//
	  //是否区分平今还是平昨(上海) 返回-1有问题  只有区分开仓平仓的才需要调用此函数
	  bool IsDistinguishCoverType(const TCommodityKey*pCommoditykey);
	  //是否是行情合约
	  bool IsValidQuotContract(const  char * ContractId);
	  //是否是套利合约
	  bool IsValidSpreadContract(const  char * ContractId);
	  
	//字符串转交易合约
	  bool TContractNoToTContractKey(const char * szCode, TContractKey & ContractKey);//保证传过来的字符串左右两端都去掉空格了***
      //交易合约转字符串 信息显示
      bool TContractKeyToTContractNo(const TContractKey & ContractKey, std::wstring & strCode, std::wstring str = L"");//
      //交易合约转字符串
      bool TContractKeyToTContractNo(const TContractKey & ContractKey, std::string & strCode, bool bOnlyContract = false);//
	//单腿交易合约转字符串 默认不带交易所
	  bool SingTradeContractToStr(std::string & strCode, const TContractSingleKey & SingContractKey, bool bHaveExchange = false, bool bHaveSign = true);
	//单腿交易合约转字符串
	  bool PosContractToStr(std::string & strCode, const TContractSingleKey & SingContractKey);
	//格式化交易合约结构体
	  void FormatTradeContract(const TContractKey & ContractKey, std::string &str);

	  //行情品种类型转交易
	  TCommodityType QuoteTypeToTradeType(char cType);
	  //交易品种类型转行情
	  TCommodityType TradeTypeToQuoteType(char cType);

      //行情品种ID转交易关键字
      bool SCommodityNoToTCommodityKey(SCommodityNoType commodityno, TCommodityKey &CommodityKey);
	  //行情ID转交易关键字
      void SContractNoTypeToTradeContract(const char *szSign, const std::string &id, TContractKey &ContractKey);
      //行情ID转品种关键字
      void SContractNoTypeToTradeCommodity(const char *szSign, const std::string &id, TCommodityKey &CommodityKey);

	  //交易合约转行情字符串
	  void TradeContractToQContractNoType(const TContractKey &ContractKey, char(&ContractId)[101]);
	  //单腿交易合约转行情合约
	  void SingleTradeContractToQContractNoType(const TContractSingleKey &ContractKey, char(&ContractId)[101]);
	  //期权交易合约转期权序列
	  bool TradeContractToOptionTarget(const TContractKey &ContractKey, char(&ContractId)[101]);
	  //行情合约转交易合约字符串
	  void QContractNoTypeToStr(std::string &sQuoteCode, std::string &str, bool bOnlyContract = false);
	  //交易字符串到行情id
	  bool StrToQContractNoType(const char *code, char(&ContractId)[101]);
	  //套利合约字符串分解
	  bool SpreadStrToSpreadInfo(const char *code, char& cRateMode,char(&sFirst)[101], char(&sSecond)[101],int N_SwitchType =NST_NONE);//ntype :0:  1: 2:Q2T
	  //填充期权字段
	  bool ParseOptionContractStr(const std::string& strValue, char(&ContractNo)[11], TOptionType &OptionType, char(&StrikePrice)[11]);

	  //组合变单腿
      void CombTContractKeyToTContractSingleKey(const TContractKey &ContractKey, TContractSingleKey &SigCont1, TContractSingleKey &SigCont2);
	  //组合合约是否以第一腿为准，1是，0以第二腿为准
	  bool CombContractIsFirst(const TCommodityKey & CommodityKey);

	  //交易上获取交易品种信息 最小变动价 精度 价格类型等
	  bool GetCommodityInf(const TCommodityKey &sCommodityKey, SOFCommodityInf & sCommodityInfo);
	  //行情上获取交易品种信息 最小变动价 精度 价格类型等
	  bool GetQuoteCommodityInf(const char*code, SOFCommodityInf & sCommodityInfo);
	  //获取最小变动价格
	  double GetMinChangePrice(const SOFCommodityInf & sCommodityInfo);

	  //API 与配置无关 返回0成功 返回负号失败  查询参数失败
	  int QrySingleContractCanOpen(const TSendOrder &SendOrder);
	  //API 与配置无关 返回0成功 返回负号失败  查询参数失败
	  int QrySingleContractPreFrozen(const TSendOrder &SendOrder,double& dValue);
	  //下单->针对非快速平仓下单（开仓较为简单无需提取）   若想兼容快速下单 需进一步提取（较为繁琐）得到指定账户、指定合约、指定方向的持仓 
	  void  GetPosNum(const char *UserNo, const TContractKey * ContractKey, char Direct,
		  uint &iPosT, uint &iPos);
	  void  GetPosCanCoverNum(const char *UserNo, const TContractKey * ContractKey, char Direct, bool bDisCover,
		  uint &iPosT, uint &iPos, uint &CanCoverNum, uint&CanCoverTNum);
	  //获取可开 根据系统配置信息、持仓信息 
	  int GetCanOpenQty(char cCoverMode, const TSendOrder &SendOrder, bool bOpenMaxAutoCancel);
	  //获取可平 根据系统配置信息、持仓信息 
	  void GetCanCoverQty(char cCoverMode, const TContractKey &ContractKey, const char *UserNo, char Direct, CanCoverQty& CanCoverQty, bool bLiquidateAutoCancel);
	  //获取互换持仓
	  void GetSwapPositionVol(const char * UserNo, TContractSingleKey &SigCont1, TContractSingleKey&SigCont2, PosCount&posCount);
	  //获取持仓
	  void GetPositionVol(const char * UserNo, const TContractKey * ContractKey
		  , uint &LongCount, uint &ShortCount, uint &DayLongCount, uint &DayShortCount);
	  //判断开平
	  bool OrderOffsetIsOpen(char direct, const char * UserNo, const TContractKey * ContractKey, char OrderType);
	  //开平处理
	  TOffset GetOffset(const char* UserNo, const TContractSingleKey * ContractKey, TDirect direct);
	  TOffset GetOffset(const char* UserNo, const TContractKey * ContractKey, TDirect direct);
	  //获取资金账号
	  bool GetAllUserNo(std::vector<CStringsVec>&vecUserNo);

	  //价格转字符串
	  std::string GetStrPriceByPrice(double price, bool bFraction, int nPrecision);
	  //
	  long Round(double Value);
	  //小数变分数
	  bool DecimalsToFractions(double inPrice, int denominator, SFractionPrice & out);
	  //格式化价格字符串
	  std::string FormatPriceToStr(double inPrice, const SOFCommodityInf &mInfo);
	  //是否整除
	  bool IsDiv(const double Value, const double SubValue, const double ErrorDiff = 1e-10);
	  //正则匹配
	  bool MatchPattern(const char *pattern, const char *target);
	  //是否有效的数字
	  bool IsValidNumber(const char * sValue, bool bOnlyInt = false, bool bCanNegative = false);
	  //小数等值判断
	  bool DoubleSameValue(const double d1, const double d2, double Epsilon);

	  //分割字符串
	  bool SplitNotice(const std::string &strContent, char cSplit, char cSplit2, std::vector<NoticeContent> &vecstr);

	  bool SplitNoticePos(const std::string &strContent, char cSplit, std::vector<NoticeContent> &vecstr, int nContNum);//nContNum  5 持仓合计  6 持仓明细

	  std::vector<std::string> SplitString(const std::string & str, char cSplit);

	  void TrimString(std::string &str);
	
	  bool IsTraderLogined();

	  SPriceType GetPriceByContract(const SContract* qContract, SFidMeanType nField);//（含隐含）

	  SQtyType GetQtyByContract(const SContract* qContract, SFidMeanType nField);//（含隐含）

	  SPriceType GetPriceBySnapShot(const SQuoteSnapShot* QuoteSnap, SFidMeanType nField);//

	  SQtyType GetQtyBySnapShot(const SQuoteSnapShot* QuoteSnap, SFidMeanType nField);//

      wstring UTF8ToUnicode(const string& str, UINT nCode = CP_UTF8);

	  //TKeyOrder
	  //QueuePrice排队价 CounterPrice对手价  SuperPrice超价   MarketPrice市价  LastPrice最新价
	  void GetPrice(const TContractSingleKey * stContract, TDirect direct, int PriceType, double& price);
	  double GetMinChangePrice(const TCommodityKey *pKey);
	  void GetCloseNum(const char* UserNo, TContractSingleKey* ContractKey, TDirect direct, const int iCoverQty, int& iCoverNum, int& iTCoverNum);
	  void DefaultDealClose(const TPosition *stPosition, TSendOrder &stOrder);
	  bool newGUID(char* buf);
	  TKUserType GetUserType(const char* userNo);
	  void GetAvailableQty(const TPosition* pst, int& AvaQty, int& TAvaQty);
	  bool IsSHFE(const char * exNo);
	  bool IsUserLogin();
	  bool IsLogined();
};
#endif