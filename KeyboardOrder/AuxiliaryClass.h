#pragma once
#include "AuxiliaryClassStruct.h"
#include <string>
#include "TradeApi_Struct.h"
#include "TradeApi_API.h"
#include <vector>
#include "SysConfig_Struct.h"
#include "share_struct.h"
#include "load_rc.h"
using namespace std;

typedef vector<int> IntVec;
typedef vector<string> CStringsVec;

//排序
class UserNoOrder
{
public:
	bool operator()(CStringsVec &vecStr1, CStringsVec &vecStr2)
	{
		return vecStr1[0]<vecStr2[0];
	}
};


//挂单 针对内盘 并且是不区分开平的 因为互换目前上期所并没有 有了就要重新考虑
struct PartOrderQty
{
	//买入
	uint	BuyOpenOpenQty;//买入开仓 开仓挂单量 第一腿买开 第二腿卖平 
	uint	BuyOpenCoverQty;//买入开仓 平仓挂单量 第一腿买开 第二腿卖开
	uint	BuyCoverOpenQty;//买入平仓 开仓挂单量 第一腿买平 第二腿卖开
	uint	BuyCoverCoverQty;//买入平仓 平仓挂单量 第一腿买平 第二腿卖开

	//卖出
	uint	SellOpenOpenQty;//卖出开仓 开仓挂单量 第一腿卖开 第二腿买平	
	uint	SellOpenCoverQty;//卖出开仓 平仓挂单量 第一腿卖开 第二腿买平
	uint	SellCoverOpenQty;//卖出开仓 开仓挂单量 第一腿卖平 第二腿买开
	uint	SellCoverCoverQty;//卖出平仓 平仓挂单量 第一腿卖平 第二腿买开
public:
	PartOrderQty()
	{
		memset(this, 0, sizeof(PartOrderQty));
	}
};

//交易所
enum Exchange_Type
{
	NONE_TYPE,		//无效
	ZCE_TYPE,		//郑商所
	DCE_TYPE,		//大商所
	SHFE_TYPE,		//上期所
	CFFEF_TYPE		//中金所
};



enum eCancelOrderType
{
	AUTOCANCEL_ZUBI,
	AUTOCANCEL_QUANBU,
	CANCEL_BUY,				//撤买入
	CANCEL_SELL,			//撤卖出
	CANCEL_KAICANG,			//撤开仓
	CANCEL_PINGCANG,		//撤平仓
	CANCEL_ALL,				//撤所有
	CANCEL_QINGCANG			//快速用
};

//平仓自动撤单所用类型：撤平全部（今仓和昨仓）、撤平今仓、撤平仓
enum AutoCancel_Type
{
	NoCancel_Type,	//不需要撤单 判断等于此就不需要执行撤单了
	Total_Type,		//全部
	Today_Type,		//撤今仓排队单
	Yday_Type		//撤昨仓排队单
};

//平仓数量类型，根据此判断要下几笔单子
enum eCoverNumType
{
	SendOrder_One_NoDistinguish = 1,	//下一笔单子 ，不区分 取offset字段
	SendOrder_One_Distinguish,		//下一笔单子 ，不区分 取offsetT字段
	SendOrder_Two,					//下两笔单子
	SendOrder_Three					//下三笔单子
};

namespace CAuxiliary
{
	//初始化TSendOrder
	void DefaultSendOrder(TSendOrder&SendOrder);
	//判断是否是港交所期货
	bool IsHK(const char *szCode);
	//判断内盘外盘
	int  ReadVersionType();
	//字符串转交易合约
	bool StrToTradeContract(const char * szCode, TContractKey & ContractKey);
	//交易合约转字符串 bOnlyContract为true时只需要格式化合约部分 不需要交易所和商品
	bool TradeContractToStr(const TContractKey & ContractKey, string & strCode, bool bOnlyContract = false, bool bName = false, bool bOnlyName = false);
	//交易合约转字符串 只要品种和合约 不要交易所
	bool TradeContractToStr(string & strCode, const TContractKey & ContractKey);
	//交易上获取交易品种信息 最小变动价 精度 价格类型等
	bool GetCommodityInf(const TCommodityKey &sCommodityKey, SOFCommodityInf & sCommodityInfo);
	//行情上获取品种信息 最小变动价 精度 价格类型等
	bool GetQuotCommodityInf(const char*code, SOFCommodityInf & sCommodityInfo);
	//获取最小变动价格
	double GetMinChangePrice(const SOFCommodityInf & sCommodityInfo);
	//填充期权字段
	bool ParseOptionContractStr(const std::string& strValue, char(&ContractNo)[11], TOptionType &OptionType, char(&StrikePrice)[11]);
	//分割字符串
	void StringSplit(const char* strSrc, char Separator, vector<string>& vecDest);
	//交易合约转为行情ID
	void TradeContractToRawContractIdType(const TContractKey &ContractKey, char(&ContractId)[51]);
	//行情ID转为交易合约
	void RawContractIdTypeToTradeContract(const char *szSign, const string &id, TContractKey &ContractKey);
	//交易字符串到行情id
	bool StrToRawContractIdType(const char *code, char(&ContractId)[51]);
	//行情id到交易字符串
	void RawContractIdTypeToStr(char(&ContractId)[51], string &str);
	//判断是否是有效的行情合约
	bool IsValidQuotContract(const  char * ContractId);
	//检查是否有交易员登录过
	bool IsTraderLogined();
	//判断是否是组合合约
	bool IsCombContract(const char CommodityType);
	//获取缺省默认数量
	int  GetDefaultQty(const char *szSign, int iCommodityVol, CommodityVol *pCommodityVol,const TContractKey &ContractKey, int iType);
	//判断是否是有效的交易合约
	bool IsValidTradeContract(const TContractKey & ContractKey);
	//格式化交易合约结构体
	void FormatTradeContract(const TContractKey & ContractKey, string &str);
	//获取资金账户信息
	bool GetAllUserNo(vector<CStringsVec>&vecUserNo);
	//是否区分平今还是平昨(上海) 返回-1有问题  只有区分开仓平仓的才需要调用此函数
	bool IsDistinguishCoverType(const TCommodityKey*pCommoditykey);
	//API 与配置无关 返回0成功 返回负号失败  查询参数失败
	int QrySingleContractCanOpen(const TSendOrder &SendOrder);
	//获取可开 根据系统配置信息、持仓信息 
	int GetCanOpenQty(char cCoverMode, const TSendOrder &SendOrder, bool bOpenMaxAutoCancel);
	//获取可平 根据系统配置信息、持仓信息 
	void GetCanCoverQty(char cCoverMode, const TContractKey &ContractKey, const char *UserNo, char Direct,CanCoverQty& CanCoverQty, bool bLiquidateAutoCancel);
	//把组合合约转为两个单腿合约
	void CombContractToSingleContract(const TContractKey &ContractKey, TContractSingleKey &SigCont1, TContractSingleKey &SigCont2);
	//组合合约是否以第一腿为准，1是，0以第二腿为准
	bool CombContractIsFirst(const TCommodityKey & CommodityKey);
	//得到持仓、可平 根据客户 、内盘所用  非互换
	void GetPosCanCoverNum(const char *UserNo, const TContractKey * ContractKey, char Direct, bool bDisCover,
		uint &iPosT, uint &iPos, uint &CanCoverNum, uint&CanCoverTNum);
	//下单->针对非快速平仓下单（开仓较为简单无需提取）   若想兼容快速下单 需进一步提取（较为繁琐）得到指定账户、指定合约、指定方向的持仓 
	void  GetPosNum(const char *UserNo, const TContractKey * ContractKey, char Direct,
		uint &iPosT, uint &iPos);
	//持仓数量 互换跟普通组合算法不一样 互换是一开一平 算持仓数量只需要算平仓的那一腿
	void GetPositionVol(const char * UserNo, const TContractKey * ContractKey
		, uint &LongCount, uint &ShortCount, uint &DayLongCount, uint &DayShortCount);
	//判断是否是期权合约
	bool IsOptionContract(const char CommodityType);
	//单腿交易合约转字符串 默认不带交易所
	bool SingTradeContractToStr(string & strCode, const TContractSingleKey & SingContractKey, bool bHaveExchange);
	//获取swap持仓
	void GetSwapPositionVol(const char * UserNo, TContractSingleKey &SigCont1, TContractSingleKey&SigCont2, PosCount&posCount);
	//是内盘吗
	bool IsInnerDiskExchange(const AbnormalTradeLimit& AbnormalTradeLimit, const char *szSign, const char *szExchange, Exchange_Type &ExchangeType);
	//行情转交易 合约类型
	TCommodityType QuoteTypeToTradeType(char cType);
}