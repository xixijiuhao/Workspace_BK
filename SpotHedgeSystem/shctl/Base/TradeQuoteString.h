#ifndef _TRADE_QUOTE_STRING_
#define _TRADE_QUOTE_STRING_

#include <windows.h>
#include <windowsx.h>
#include <vector>
#include <regex>
#include <string>

#include "TradeApi_API.h"
#include "CommonModule_API.h"
#include "StarApi_API.h"
#include"LanguageApi_API.h"

using namespace std;


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

class TVecInt:public IVecInt
{
public:
	//------------------------------继承begin--------------------------------------//
	virtual unsigned int __cdecl size()
	{
		return m_VecData.size();
	}
	virtual int __cdecl at(unsigned int i)
	{
		return m_VecData.at(i);
	}
	virtual void __cdecl clear()
	{
		m_VecData.clear();
	}
	virtual void __cdecl push_back(int data)
	{
		m_VecData.push_back(data);
	}
	//-------------------------------继承end---------------------------------------//

	//------------------------------begin--------------------------------------//
	VecIntData & GetData()
	{
		return m_VecData;
	}
	//-------------------------------end---------------------------------------//
public:
	VecIntData m_VecData;
};
//品种信息
struct SOFCommodityInf
{
	char		Sign[21];
	bool		bCanNegative;					//小数时是否能够为负数 
	bool		bIfFractionPrice;				//是否分数报价
	char		cCoverMode;						//根据此来决定是否显示开平界面
	double		dMiniChangePrice;				//bIfFractionPrice若为1代表分数的分子
	int			iPrecision;						//bIfFractionPrice若为1代表分数的分母
	uint		cUserIdentity;					//交易登陆用户身份，是在登陆回调函数和初始化窗口中赋值,默认为单客户(uiUser) 下单人(uiProxy) 经纪人(uiBroker) 交易员(uiTrader) 行情客户(uiQUser)
	bool		bAddOne;
	double		ProductDot;						//每手乘数
public:
	SOFCommodityInf()
	{
		Default();
	}
	void Default()
	{
		memset(this, 0, sizeof(SOFCommodityInf));
		//默认
		dMiniChangePrice = 1.00;
		iPrecision = 1;
		cCoverMode = cmCover;
		ProductDot = 1;
	}
};

//分数价格
struct SFractionPrice
{
	int	iInteger;			//整数部分
	int	iMolecules;			//分子部分
	int	idenominator;		//分母部分
public:
	SFractionPrice() :iInteger(0), iMolecules(0), idenominator(0) {}
};

//持仓信息
struct PosCount
{
	//第一腿持买持卖 第二腿持买持卖
	uint BuyOpenCount;	//买入开仓 持仓量(第一腿持买) 第一腿买开 第二腿卖平 
	uint BuyCoverCount;	//买入平仓 持仓量(第二腿持卖) 第一腿平开 第二腿卖开

	uint SellOpenCount;	//卖出开仓 持仓量(第一腿持卖) 第一腿卖开 第二腿买平	
	uint SellCoverCount;//卖出平仓 持仓量(第二腿持买)  第一腿卖平 第二腿买开
public:
	PosCount()
	{
		memset(this, 0, sizeof(PosCount));
	}
};

//字符串分割信息
struct NoticeContent
{
	std::string strKey;
	std::string strContent;
};

//可平 非互换
struct CanCoverQty
{
	uint uCanCoverQty;
	uint uCanCoverTQty;
public:
	CanCoverQty()
	{
		memset(this, 0, sizeof(CanCoverQty));
	}
};

namespace CConvert
{
	  //初始化订单信息
	  void DefaultSendOrder(TSendOrder&SendOrder);//***
	  //是否交易员登录
	  bool IsTraderLogined();
	  //LME判断
	  bool IsLME(const TExchangeKey &Exchange);
	  //判断是否是组合合约
	  bool IsCombContract(const char CommodityType);
	  //是否区分平今还是平昨(上海) 返回-1有问题  只有区分开仓平仓的才需要调用此函数
	  bool IsDistinguishCoverType(const TCommodityKey*pCommoditykey);
	  //判断是否是有效的交易品种
	  bool IsValidTradeCommodity(const TCommodityKey &CommodityKey);//
	  //判断是否是有效的行情品种
	  bool IsValidQuoteCommodity(const char * CommodityId);//
	  //判断是否是有效的行情合约
	  bool IsValidQuotContract(const  char * ContractId);
	  //是否开平合约
	  bool IsCoverOrCoverTCommodity(const TCommodityKey*pCommoditykey);

	  //行情品种类型转交易
	  TCommodityType QuoteTypeToTradeType(char cType);
	  //交易品种类型转行情
	  TCommodityType TradeTypeToQuoteType(char cType);

	  //字符串转交易合约
	  bool TContractNoToTContractKey(const char * szCode, TContractKey & ContractKey);//保证传过来的字符串左右两端都去掉空格了***
	  //交易合约转字符串 bOnlyContract为true时只需要格式化合约部分 不需要交易所和商品
	  bool TContractKeyToTContractNo(const TContractKey & ContractKey, std::string & strCode);//
	//交易合约转字符串
	  bool TContractKeyToTContractNo2(const TContractKey & ContractKey, std::string & strCode, bool bOnlyContract = false);//

	  //单腿交易合约转字符串
	  bool TContractSingleKeyToTContractNo(const TContractSingleKey & SingContractKey,std::string & strCode);

	  //行情品种ID转交易关键字
	  bool SCommodityNoToTCommodityKey(SCommodityNoType commodityno, TCommodityKey &CommodityKey);
	  //行情ID转交易关键字
	  void SContractNoToTContractKey(const char *szSign, const std::string &id, TContractKey &ContractKey);
	  //行情ID转品种关键字
	  void SContractNoToTCommodityKey(const char *szSign, const std::string &id, TCommodityKey &CommodityKey);

	  //交易合约转行情字符串
	  void TContractKeyToSContractNo(const TContractKey &ContractKey, char(&ContractId)[101]);
	  //单腿交易合约转行情合约
	  void TContractSingleKeyToSContractNo(const TContractSingleKey &ContractKey, char(&ContractId)[101]);

	  //行情合约转交易合约字符串
	  void SContractNoToTContractNo(std::string &sQuoteCode, std::string &str);
	  //行情合约转交易合约字符串
	  void QContractNoTypeToStr(std::string &sQuoteCode, std::string &str, bool bOnlyContract = false);
	  //交易字符串到行情id
	  bool TContractNoToSContractNo(const char *code, char(&ContractId)[101]);

	//填充期权字段
	  bool ParseOptionContractStr(const std::string& strValue, char(&ContractNo)[11], TOptionType &OptionType, char(&StrikePrice)[11]);

	  //组合变单腿
	  void CombTContractKeyToTContractSingleKey(const TContractKey &ContractKey, TContractSingleKey &SigCont1, TContractSingleKey &SigCont2);
	  //组合合约是否以第一腿为准，1是，0以第二腿为准
	  bool CombContractIsFirst(const TCommodityKey & CommodityKey);

	  //交易上获取交易品种信息 最小变动价 精度 价格类型等
	  bool GetTradeCommodityInf(const TCommodityKey &sCommodityKey, SOFCommodityInf & sCommodityInfo);
	  //行情上获取交易品种信息 最小变动价 精度 价格类型等
	  bool GetQuoteCommodityInf(const char*code, SOFCommodityInf & sCommodityInfo);
	  //获取最小变动价格
	  double GetMinChangePrice(const SOFCommodityInf & sCommodityInfo);

	  //API 与配置无关 返回0成功 返回负号失败  查询参数失败
	  int QrySingleContractCanOpen(const TSendOrder &SendOrder);
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
	  
	  //价格转字符串
	  std::string GetStrPriceByPrice(double price, bool bFraction, int nPrecision);
	  //小数变分数
	  bool DecimalsToFractions(double inPrice, int denominator, SFractionPrice & out);
	  //格式化价格字符串
	  string FormatPriceToStr(double inPrice, SOFCommodityInf &mInfo);
	  //是否整除
	  bool IsDiv(const double Value, const double SubValue, const double ErrorDiff = 1e-10);
	  //正则匹配
	  bool MatchPattern(const char *pattern, const char *target);
	  //小数等值判断
	  bool DoubleSameValue(const double d1, const double d2, double Epsilon);

	  //分割字符串
	  bool SplitNotice(const std::string &strContent, char cSplit, std::vector<NoticeContent> &vecstr);

	  bool SplitNotice(const std::string & strContent, char cSplit, char cSplit2, std::vector<NoticeContent>& vecstr);

	  bool SplitNoticePos(const std::string &strContent, char cSplit, std::vector<NoticeContent> &vecstr, int nContNum);//nContNum  5 持仓合计  6 持仓明细

	  std::vector<std::string> SplitString(const std::string & str, char cSplit);

	  void TrimString(std::string &str);

	  void GetPriceQty(const SQuoteSnapShot& QuoteSnap, double& dbuy, double& dsell, SQtyType& nbuy, SQtyType& nsell);

	  long Round(double value,double dDif=0.5);

	  bool SameValue(const double A, const double B, double Epsilon = 0);
	 
	  int CmpValue(const double A, const double B, double Epsilon = 0);

	  SPriceType GetPriceByContract(const SContract* SContract, SFidMeanType nField);//（含隐含）

	  SQtyType GetQtyByContract(const SContract* SContract, SFidMeanType nField);//（含隐含）

	  SPriceType GetPriceBySnapShot(const SQuoteSnapShot* QuoteSnap, SFidMeanType nField);//

	  SQtyType GetQtyBySnapShot(const SQuoteSnapShot* QuoteSnap, SFidMeanType nField);//

	  double FormatPriceToDouble(double dPrice,int nPrecision,double dMinChange);

	  wstring LoadResString(int strID);

	  wstring UTF8ToUnicode(const string& str);
	  //是否有效的数字
	  bool IsValidNumber(const char * sValue, bool bOnlyInt = false, bool bCanNegative = false);
	  //正则匹配
	  bool MatchPattern(const wchar_t *pattern, const wchar_t *target);
	  //判断是否是合法的数字输入
	  bool IsValidNumber(const wchar_t * sValue, bool bOnlyInt, bool bCanNegative);//是否有效的数字
};
#endif