#ifndef _TRADE_STRUCT_
#define _TRADE_STRUCT_

#include "TradeApi_Type.h"
#include "StarApi_API.h"
#include <xstring>
#include <string>
#include "Global.h"
#include "_base.h"

using namespace std;

/*****************************************************************************************************/
//__VA_ARGS__，可以定义带可变参数的宏，"##"符号的作用是在可变参数的个数为0时，消除参数前面的逗号
#define GetKey(count, ...) StrFun::MergeString(KEY_SPLIT, count, ##__VA_ARGS__)
/*****************************************************************************************************/


//单笔数据类型，其中每种类型的数据在系统中只会出现一个结构体实例
enum SingleDataType
{
	sdtAllPosition			= 0 ,	//总持买、总持卖
	sdtOptionRiskParam		,		//期权风险参数
	sdtTradableQty			,		//可下单数量
	sdtPreFreeze			,		//下单预冻结资金
	sdtTTradableQtySell		,
	sdtPreFreezeSell		,
	sdtBankBalance			,		//银行可转金额
	sdtFutureBalance		,		//期货可转金额
	sdtCfmmcToken					//保证金监控中心令牌
};

#pragma pack(push, 1)


//交易后台基本信息
struct TRawTradePlugin
{
	char					PluginNo[21];
	char					Version[21];
	char					PluginName[31];
public:
	_InitStruct(TRawTradePlugin);
};

//key=Sing
struct TAddress
{
	char					Sign[21];					//表示服务器标识，以ip，port为依据, 在多账户多后台情况下使用						
	char 					IP[40];						//留长一点，以便兼容域名地址格式
	ushort					Port;
	TServerType				ServerType;					//登录的服务器类型
public:
	_InitStruct(TAddress);
};

//连接、断开			
struct TConnect : public TAddress
{
	char					SiteAddrNo[21];				//站点编号
	double					LoadValue;					//负载值
	//应答字段			
	char					LocalIP[40];
	ushort					LocalPort;
	char					LocalMac[24];
public:
	_InitStruct(TConnect);
	string Key() const
	{
		return Sign;
	}
};

//用户修改密码和初始化回调时使用			
struct TLoginBase : public TConnect
{
	char					CompanyNo[11];				//公司编号
	char					BrokerNo[11];				//经纪公司编号
	char					LoginUser[21];				//登录账号
	TPswType				ChangePswType;				//上次修改的密码类型
public:
	_InitStruct(TLoginBase);
	string Key() const
	{
		return GetKey(2, LoginUser, Sign);
	}
};

//登录信息
//key=LoginUser+Sign		
struct TLogin : public TLoginBase
{
	char 					LoginPsw[31];
	char 					InitPsw[31];				//强制初始化密码
	char 					CaPsw[31];					//CA密码
	char					LoginClassName[51]; 		//多后台的交易类名

	//应答字段	
	TUserIdentity 			UserType;
	char 					LoginName[21];
	union
	{
		char 				QuoteTempPsw[51];
		struct
		{
			char			GroupNo;					//投资者分组
			double			Principal;					//本金
			double			PriorityFund;				//优先资金
			//曆史上最高的三個單日盈利
			double			HisDayProfit1;				//歷史單日盈利1
			double			HisDayProfit2;				//歷史單日盈利2
			double			HisDayProfit3;				//歷史單日盈利3
		};	
	};
	char 					PreLoginIP[40];
	ushort 					PreLoginProt;
	char 					PreLoginTime[21];
	char 					PreLogoutTime[21];
	char 					PreLoginAddress[201];		//上次登录机器信息
	char 					TradeDate[11];
	char 					LastSettleTime[21];
	char 					StartTime[21];
	char 					InitTime[21];
	char 					CurrDateTime[25];			//系统当前时间(客户端校时)精确到毫秒

	//本地字段			
	TradeInitProgress		InitProgress;				//最新初始化进度
	TBool					IsInitPsw;					//是否需要强制修改密码，bYes:bNo, 登录应答时中使用，修改成功后会自动变为bNo
	TBool 					Logined;					//是否已登录
	uint 					ReadyCount;					//该用户的初始化成功次数
	TradeUserRight			Rights[16];					//用户权限表
	uint					LocalDataVersion;			//本地数据版本号，读写本地文件时使用
	uint					PswValidDay;				//客户密码有效天数
	char					PswDate[21];				//客户密码修改日期
	TBool					ConfirmState;				//是否已经进行过风险确认
	TBool					LastQryed;					//最后一次查询是否完成

	//新增字段
	ushort					PortEx;						//查询端口
	char					LinkType[21];				//链接方式 tcp ssl ...

public:
	TLogin(){ FillZero(this); LocalDataVersion = LocalVersion; LastQryed = true; }
	bool Online() const
	{
		return InitProgress != tipNone || Logined == bYes;
	}
	bool IsReady() const
	{
		return InitProgress == tipReady;
	}
	bool PswOutDate()//密码是否超过有效期
	{
		if (CurrDateTime[0] == 0 || PswDate[0] == 0 || PswValidDay == 0)
			return false;

		time_t t1 = TimeFun::StrToTime(CurrDateTime, "%Y-%m-%d");
		time_t t2 = TimeFun::StrToTime(PswDate, "%Y-%m-%d");
		//CTimeSpan ts(t1-t2);
		return 0;//ts.GetDays()>PswValidDay;
	}
	void UpdatePsw()							//密码修改成功
	{
		if (InitPsw[0] != 0)
		{
			strncpy_s(LoginPsw, InitPsw);
			InitPsw[0] = 0;
		}
	}
	bool SameOf(const TLogin ALogin) const
	{
		return (0 == strcmp(LoginUser, ALogin.LoginUser) && UserType == ALogin.UserType && 0 == strcmp(Sign,ALogin.Sign) && 
			0 == strcmp(StartTime, ALogin.StartTime) && 0 == strcmp(InitTime, ALogin.InitTime) && 
			0 == strcmp(TradeDate, ALogin.TradeDate) && 0 == strcmp(LastSettleTime, ALogin.LastSettleTime) && 
			LocalDataVersion == ALogin.LocalDataVersion);
	}
	bool UpdateRight(TradeUserRight Right)
	{
		if (Right == turUnDefine || Right == turNone)
			return false;

		int Count = sizeof(Rights) / sizeof(TradeUserRight);
		for (int i = 0; i<Count; i++)
		{
			if (Rights[i] == 0)
			{
				Rights[i] = Right;
				return true;
			}
			else if (Rights[i] == Right)
				return true;
		}
		return false;
	}
	bool DelRight(TradeUserRight Right)
	{
		bool bDeleted = false;
		int Count = sizeof(Rights) / sizeof(TradeUserRight);
		for (int i = 0; i<Count; i++)
		{
			if (Rights[i] == 0)
				break;
			if (Rights[i] == Right && !bDeleted)
			{
				Rights[i] = turNone;
				bDeleted = true;
			}
			if (bDeleted && i<Count - 1)
				Rights[i] = Rights[i + 1];
		}
		return bDeleted;
	}
	bool HasRightOf(TradeUserRight Right) const
	{
		int Count = sizeof(Rights) / sizeof(TradeUserRight);
		for (int i = 0; i<Count; i++)
		{
			if (Rights[i] == 0)
				break;
			if (Rights[i] == Right)
				return true;
		}
		return false;
	}
	void Update(TLogin & login, ushort ntype)
	{
		switch (ntype)
		{
		case tntConnect:
		{
			strncpy_s(IP, login.IP);
			Port = login.Port;
			strncpy_s(Sign, login.Sign);

			InitProgress = tipNone;
			LoadValue = login.LoadValue;
			strncpy_s(LocalIP, login.LocalIP);
			LocalPort = login.LocalPort;
			strncpy_s(LocalMac, login.LocalMac);

			strncpy_s(LoginUser, login.LoginUser);
			ChangePswType = login.ChangePswType;
			//InitProgress = login.InitProgress;

			strncpy_s(LoginPsw, login.LoginPsw);
			strncpy_s(InitPsw, login.InitPsw);
			strncpy_s(LoginClassName, login.LoginClassName);

			UserType = login.UserType;
		}
			break;
		case tntLoginInfo:
		case tntLogin:
		{
			Logined = login.Logined;
			UserType = login.UserType;

			IsInitPsw = login.IsInitPsw;
			//InitProgress = login.InitProgress;
			strncpy_s(LoginName, login.LoginName);
			memcpy_s(QuoteTempPsw, sizeof(QuoteTempPsw), login.QuoteTempPsw, sizeof(login.QuoteTempPsw));
			strncpy_s(PreLoginIP, login.PreLoginIP);
			PreLoginProt = login.PreLoginProt;
			strncpy_s(PreLoginTime, login.PreLoginTime);
			strncpy_s(PreLogoutTime, login.PreLogoutTime);
			strncpy_s(PreLoginAddress, login.PreLoginAddress);
			strncpy_s(TradeDate, login.TradeDate);
			strncpy_s(LastSettleTime, login.LastSettleTime);
			strncpy_s(StartTime, login.StartTime);
			strncpy_s(InitTime, login.InitTime);
			strncpy_s(CurrDateTime, login.CurrDateTime);
			strncpy_s(LoginPsw, login.LoginPsw);
			strncpy_s(InitPsw, login.InitPsw);
		}
			break;
		case tntLogout:
		case tntDisconnect:
		{
			InitProgress = tipNone;
			Logined = login.Logined;
		}
			break;
		case tntIniting:
		{
			InitProgress = login.InitProgress;
			if (IsReady())
				ReadyCount += 1;
		}
			break;
		case tntChangePsw:
			ChangePswType = login.ChangePswType;
			break;
		}
	}
};

//交易所关键字
//key=Sign+ExchangeNo
struct TExchangeKey
{
	char					Sign[21];
	char					ExchangeNo[11];
public:
	_InitStruct(TExchangeKey);
	string excKey() const
	{
		return GetKey(2, Sign, ExchangeNo);
	}
};

//商品关键字
//key=Sign+ExchangeNo+CommodityNo+CommodityType
struct TCommodityKey : public TExchangeKey
{
	TCommodityType			CommodityType;
	char					CommodityNo[11];
public:
	_InitStruct(TCommodityKey);
	string comKey() const
	{
		return GetKey(3, Sign, ExchangeNo, CommodityNo) + KEY_SPLIT + CommodityType;
	}
	string GetDeposidKey(const char * UserNo, const char * GroupNo = nullptr) const
	{
		if (GroupNo==nullptr || GroupNo[0]==0)
			return GetKey(2, UserNo, comKey().c_str());
		else			
			return GetKey(2, UserNo, GroupNo);
	}
};

//单腿合约关键字
//key=Sign+ExchangeNo+CommodityNo+CommodityType+ContractNo+StrikePrice+OptionType
struct TContractSingleKey : public TCommodityKey
{
	char					ContractNo[11];				//月份
	char					StrikePrice[11];			//执行价
	TOptionType				OptionType;					//买卖权 TOptionType
public:
	_InitStruct(TContractSingleKey);
	string conSingleKey() const
	{
		return 	GetKey(3, Sign, ExchangeNo, CommodityNo) + KEY_SPLIT + CommodityType + KEY_SPLIT +
			GetKey(2, ContractNo, StrikePrice) + KEY_SPLIT + OptionType;
	}
private:
	string GetFmtItem(string & mask) const
	{
		string result;
		if (mask == "c" || mask == "c1")
			return CommodityNo;
		else if (mask == "m1")
			return ContractNo;
		else if (mask == "p1")
			return StrikePrice;
		else if (mask == "ot1")
		{
			result.append(1, OptionType);
			return result;
		}
		else if (mask == "ct" || mask == "ct1")
		{
			result.append(1, CommodityType);
			return result;
		}
		else if (mask == "e" || mask == "e1")
			return ExchangeNo;
		else if (mask == "e2" || mask == "c2" || mask == "ct2" || mask == "ot2" || mask == "p2")
			return "";
		else
		{
			StrFun::format(result, "%%%s%%", mask.c_str());
			return result;
		}
	}
protected:
	template<typename CONTRACT> string FmtCode(CONTRACT & contract, const char * mask) const
	{
		string result;
		string word;
		bool bword = false;
		char * p = (char *)mask;
		char * q = p;
		while (*p)
		{
			if (*q != '\\' && *p == '%')					//非转意字符
			{
				if (bword)
				{
					result.append(contract.GetFmtItem(word));
					word.clear();
				}
				bword = !bword;
			}
			else if (bword)
				word.append(1, *p);
			else
				result.append(1, *p);

			q = p++;
		}
		if (bword)											//格式错误
			result.append("%" + word);

		return result;
	}
public:
	string GetCode(const char * fmt_str = nullptr) const
	{
		if (fmt_str != nullptr && fmt_str[0] != 0)
			return FmtCode(*this, fmt_str);

		//默认处理
		switch(CommodityType)
		{
		case ctFutures			:	return FmtCode(*this, "%c%%m1%");
		case ctOption			:	return FmtCode(*this, "%c%%m1%%ot1%%p1%");
		default:
			{
				if (StrikePrice[0])
					return FmtCode(*this, "%c%%m1%%ot1%%p1%");
				else
					return FmtCode(*this, "%c%%m1%");
			}
		}
	}
};

//合约关键字
//key=Sign+ExchangeNo+CommodityNo+CommodityType+ContractNo+StrikePrice+OptionType+ContractNo2+StrikePrice2+OptionType2
struct TContractKey : public TContractSingleKey
{
	char					ContractNo2[11];			//月份2
	char					StrikePrice2[11];			//执行价2
	TOptionType				OptionType2;				//买卖权2 TOptionType
public:
	_InitStruct(TContractKey);
	string conKey() const
	{
		return 	GetKey(3, Sign, ExchangeNo, CommodityNo) + KEY_SPLIT + CommodityType + KEY_SPLIT +
			GetKey(2, ContractNo, StrikePrice) + KEY_SPLIT + OptionType + KEY_SPLIT +
			GetKey(2, ContractNo2, StrikePrice2) + KEY_SPLIT + OptionType2;
	}
private:
	friend struct TContractSingleKey;
	string GetFmtItem(string & mask) const
	{
		string result;
		if (mask == "c" || mask == "c1" || mask == "c2")
			return CommodityNo;
		else if (mask == "m1")
			return ContractNo;
		else if (mask == "p1")
			return StrikePrice;
		else if (mask == "ot1")
		{
			result.append(1, OptionType);
			return result;
		}
		else if (mask == "m2")
			return ContractNo2;
		else if (mask == "p2")
			return StrikePrice2;
		else if (mask == "ot2")
		{
			result.append(1, OptionType2);
			return result;
		}
		else if (mask == "ct" || mask == "ct1" || mask == "ct2")
		{
			result.append(1, CommodityType);
			return result;
		}
		else if (mask == "e" || mask == "e1" || mask == "e2")
			return ExchangeNo;
		else
		{
			StrFun::format(result, "%%%s%%", mask.c_str());
			return result;
		}
	}
public:
	string GetCode(const char * fmt_str = nullptr) const
	{
		if (fmt_str != nullptr && fmt_str[0] != 0)
			return FmtCode(*this, fmt_str);

		//默认处理
		switch (CommodityType)
		{
		case ctFutures			:	return FmtCode(*this, "%c%%m1%");
		case ctOption			:	return FmtCode(*this, "%c%%m1%%ot1%%p1%");
		case ctSpreadMonth		:	return FmtCode(*this, "%c%%m1%/%m2%");
		case ctSpreadCommodity	:	return FmtCode(*this, "%c%%m1%");
		default:
		{
			if (StrikePrice[0])
			{
				if (StrikePrice2[0])		//期权组合
					return FmtCode(*this, "%c%%m1%%ot1%%p1%/%m2%%ot2%%p2%");
				else if (ContractNo2[0])	//期权期货组合
					return FmtCode(*this, "%c%%m1%%ot1%%p1%/%m2%");
				else						//期权
					return FmtCode(*this, "%c%%m1%%ot1%%p1%");
			}
			else
			{
				if (StrikePrice2[0])		//期货期权组合
					return FmtCode(*this, "%c%%m1%/%m2%%ot2%%p2%");
				else if (ContractNo2[0])	//期货组合
					return FmtCode(*this, "%c%%m1%/%m2%");
				else						//期货
					return FmtCode(*this, "%c%%m1%");
			}
		}
			break;
		}
	}
};

//1 市场信息
//struct TExchange : public TExchangeKey
//{
//	wchar_t					ExchangeName[21];
//	//增加--------------------------------------
//	TCoverMode				CoverMode;					//平倉類型
//	bool					HasOption;					//是否包含期权商品
//public:
//	_InitStruct(TExchange);
//};

//市场状态
//key=ExchangeNo+CommodityNo
struct TExchangeState : public TExchangeKey
{
	TCommodityType			CommodityType;
	char					CommodityNo[10];
	char					ExchangeDateTime[21];
	TTradeState				TradeState;
public:
	_InitStruct(TExchangeState);
	string Key() const
	{
		return StrFun::format("%d%s%s%s%s", tntExchangeState, KEY_SPLIT, ExchangeNo, KEY_SPLIT, CommodityNo);
	}
};

//品种信息 Key=ExchangeNo+CommodityType+CommodityNo
//struct TCommodity : public TCommodityKey
//{
//	wchar_t					CommodityName[21];
//	double					ProductDot;					//每手乘数
//	double					UpperTick;					//最小变动价分子
//	uint					LowerTick;					//最小变动价分母
//	TSpreadDirect			CmbDirectFlag;				//组合买卖标记
//	uint					OnlyCanCloseDays;			//合约只可平仓提前天数
//	char					CurrencyNo[11];				//商品使用币种
//	TDeliveryMode			DeliveryMode;				//交割行权方式 
//	uint					DeliveryDays;				//交割日偏移
//	char					AddOneTime[9];				//T+1分割时间,大于此时间为T+1数据
//	TCoverMode				CoverMode;					//平仓模式
//	double					ExecuteDot;					//行权乘数，期权执行时，1手期权对应的标的手数（执行价倍数）
//	uint					TimeZone;					//品种时区
//	TBool					IsOnlyQuoteCommodity;		//是否单行情品种 
//
//	uint					MaxSingleOrderQty;			//单笔最大下单量
//	uint					MaxPositionQty;				//最大持仓量
//
//	//依赖商品1		
//	char					RelateExchangeNo[11];
//	TCommodityType			RelateCommodityType;
//	char					RelateCommodityNo[11];		//商品关键字
//	//依赖商品2		
//	char					RelateExchangeNo2[11];
//	TCommodityType			RelateCommodityType2;
//	char					RelateCommodityNo2[11];		//商品关键字
//
//	//增加--------------------------------
//	char					PriceFormat[6];				//价格格式化字符串
//	char					AvgPriceFormat[6];			//均价格式化字符串+2
//	char					SpdPriceFormat[6];			//价差成交价格格式化字符串+1
//	char					CarryPriceFormat[6];		//Carry价格格式化字符串+2
//	uint					Precision;					//小数位精度
//
//	//////	
//	char					GroupNo[11];				//大边的保证金组的组号
//
//	//后台原始品种
//	char					OriginExchangeNo[11];		//后台系统原始交易所
//	char					OriginCommodityType;		//后台系统原始交易所
//	char					OriginCommodityNo[11];		//后台系统原始交易所
//	double					PriceFactor;				//后台系统价格到客户端价格的换算系数
//public:
//	_InitStruct(TCommodity);
//	int GetPrecision(char PrecisionType = ptDefault) const
//	{
//		switch (PrecisionType)
//		{
//		case ptAverage	: return Precision+2;
//		case ptCarry	: return Precision+2;
//		case ptSpread	: return (CoverMode == cmNone || CoverMode == cmUnfinish) ? Precision+1 : Precision;
//		default			: return Precision;
//		}
//	}
//	const char * GetPrecisionFormat(char PrecisionType = ptDefault) const
//	{
//		switch (PrecisionType)
//		{
//		case ptAverage	: return AvgPriceFormat;
//		case ptCarry	: return CarryPriceFormat;
//		case ptSpread	: return SpdPriceFormat;
//		default			: return PriceFormat;
//		}
//	}
//	void Update()
//	{
//		ExecuteDot = ExecuteDot == 0 ? 1 : ExecuteDot;  //行权倍数 尚未增加,增加后删除这句
//
//		char sPrecision[KEY_LENGTH];
//		memset(sPrecision, 0, sizeof(sPrecision));
//		int iPrecision = 0;
//		if (LowerTick>0.0000000001)
//			_snprintf_s(sPrecision, "%.10f", UpperTick / LowerTick);
//		else
//			_snprintf_s(sPrecision, "%.10f", UpperTick);
//		for (int i = sizeof(sPrecision)-1; i >= 0; i--)
//		{
//			if (iPrecision == 0 && sPrecision[i]>'0')
//				iPrecision = i;
//			if (sPrecision[i] == '.' && iPrecision>i)
//			{
//				iPrecision = iPrecision - i;
//				break;
//			}
//		}
//		Precision = iPrecision;
//
//		_snprintf_s(PriceFormat, "%%.%df", GetPrecision());
//		_snprintf_s(AvgPriceFormat, "%%.%df", GetPrecision(ptAverage));
//		_snprintf_s(SpdPriceFormat, "%%.%df", GetPrecision(ptSpread));
//		_snprintf_s(CarryPriceFormat, "%%.%df", GetPrecision(ptCarry));
//	}
//	bool IsSpread() const//是否组合
//	{
//		switch (CommodityType)
//		{
//		case ctSpreadMonth		:	//跨期套利
//		case ctSpreadCommodity	:	//品种套利
//		case ctBUL				:	//垂直看涨套利
//		case ctBER				:	//垂直看跌套利
//		case ctBLT				:	//水平看涨套利	
//		case ctBRT				:	//水平看跌套利	
//		case ctSTD				:	//跨式套利
//		case ctSTG				:	//宽跨式套利
//									return true;
//		}
//		return false;
//	}
//};

/*合约编码格式化掩码字符串
%e%		交易所
%e1%	关联交易所1
%e2%	关联交易所2
%ct%	品种类型
%ct1%	关联品种类型1
%ct2%	关联品种类型2
%c%		品种代码
%c1%	关联品种代码1
%c2%	关联品种代码2
%m1%	合约月份或日期1
%m2%	合约月份或日期2
%p1%	执行价格1
%p2%	执行价格2
%ot1%	期权涨跌类型1
%ot2%	期权涨跌类型2
%oe%	后台系统原始交易所
%oc%	后台系统原始品种代码
%oct%	后台系统原始品种类型
举例:
一、
期货单腿 %c%%m1%
期货组合 %c1%%m1%/%c2%%m2%
期权单腿 %c%%m1%%ot1%%p1%
期权组合 %c1%%m1%%ot1%%p1%/%c2%%m2%%ot2%%p2%
期货期权 %c1%%m1%/%c2%%m2%%ot2%%p2%
期权期货 %c1%%m1%%ot1%%p1%/%c2%%m2%
二、
期货单腿 %c% %m1%
期货组合 %c% %m1%/%m2%
期权单腿 %c% %m1% %ot1%%p1%
期权组合 %c% %m1% %ot1%%p1%/%m2% %ot2%%p2%
期货期权 %c% %m1%/%m2% %ot2%%p2%
期权期货 %c %%m1% %ot1%%p1%/%m2%
注意：
如果要输出%则需要以%%的形式转意

如果商品代码上需要有交易所原始代码则可实现合约代码统一编号
*/

//合约信息
//增加客户端显示代码，且是关键字
//以便客户交易使用的合约代码的自由定制，并且保持客户与公司交流的一致性
//struct TContract: public TContractKey
//{			  					
//	wchar_t					ContractName[21];			//合约名称
//	char					ExpiryDate[11];				//到期日
//	char					LastTradeDate[11];			//最后交易日
//	char					FirstNoticeDate[11];		//首次通知日	
//	char					RelateContractNo[11];		//标的合约的月份，默认和ContractNo相同, 不用月份地期权对应同一月份的标的时和ContractNo不同
//
//	//增加----------------------------------------------------------------------
//	char					OnlyCoverDate[11];			//只可平仓日
//	int						OnlyCoverDateDx;			//距离只可平仓日的时间，天数
//	char					DefCode[31];				//默认合约代码
//	const TCommodity *		pCommodity;
//private:
//	friend struct TContractSingleKey;
//	string GetFmtItem(string & mask) const
//	{
//		string result;
//		if (mask == "c")
//			return CommodityNo;
//		else if (mask == "m1")
//			return ContractNo;
//		else if (mask == "p1")
//			return StrikePrice;
//		else if (mask == "ot1")
//		{
//			result.append(1, OptionType);
//			return result;
//		}
//		else if (mask == "c1")
//			return pCommodity->RelateCommodityNo;
//		else if (mask == "c2")
//			return pCommodity->RelateCommodityNo2;
//		else if (mask == "m2")
//			return ContractNo2;
//		else if (mask == "p2")
//			return StrikePrice2;
//		else if (mask == "ot2")
//		{
//			result.append(1, OptionType2);
//			return result;
//		}
//		else if (mask == "ct")
//		{
//			result.append(1, CommodityType);
//			return result;
//		}
//		else if (mask == "ct1")
//		{
//			result.append(1, pCommodity->RelateCommodityType);
//			return result;
//		}
//		else if (mask == "ct2")
//		{
//			result.append(1, pCommodity->RelateCommodityType2);
//			return result;
//		}
//		else if (mask == "e")
//			return ExchangeNo;
//		else if (mask == "e1")
//			return pCommodity->RelateExchangeNo;
//		else if (mask == "e2")
//			return pCommodity->RelateExchangeNo2;
//		else if (mask == "oc")
//			return pCommodity->OriginCommodityNo;
//		else if (mask == "oct")
//		{
//			result.append(1, pCommodity->OriginCommodityType);
//			return result;
//		}
//		else if (mask == "oe")
//			return pCommodity->OriginExchangeNo;
//		else
//		{
//			StrFun::format("%%%s%%", mask.c_str());
//			return result;
//		}
//	}
//public:
//	_InitStruct(TContract);
//	string GetCode(const char * fmt_str = nullptr) const
//	{
//		if (fmt_str!=nullptr && fmt_str[0]!=0)
//			return FmtCode(*this, fmt_str);
//		else if (DefCode[0])
//			return DefCode;
//
//		//默认处理
//		string ret;
//		switch(CommodityType)
//		{
//		case ctFutures			:	ret = FmtCode(*this, "%c%%m1%"			); break;
//		case ctOption			:	ret = FmtCode(*this, "%c%%m1%%ot1%%p1%"	); break;
//		case ctSpreadMonth		:	ret = FmtCode(*this, "%c%%m1%/%m2%"		); break;
//		case ctSpreadCommodity	:	ret = FmtCode(*this, "%c%%m1%"			); break;
//		default:
//			{
//				if (StrikePrice[0])
//				{
//					if (StrikePrice2[0])		//期权组合
//						ret = FmtCode(*this, "%c%%m1%%ot1%%p1%/%m2%%ot2%%p2%");
//					else if (ContractNo2[0])	//期权期货组合
//						ret = FmtCode(*this, "%c%%m1%%ot1%%p1%/%m2%");
//					else						//期权
//						ret = FmtCode(*this, "%c%%m1%%ot1%%p1%");
//				}
//				else
//				{
//					if (StrikePrice2[0])		//期货期权组合
//						ret = FmtCode(*this, "%c%%m1%/%m2%%ot2%%p2%");
//					else if (ContractNo2[0])	//期货组合
//					{							//跨商品
//						if (strcmp(pCommodity->RelateCommodityNo, pCommodity->RelateCommodityNo2))
//							ret = FmtCode(*this, "%c%%m1%");		
//						else					//跨期
//							ret = FmtCode(*this, "%c%%m1%/%m2%");	
//					}
//					else						//期货
//						ret = FmtCode(*this, "%c%%m1%");
//				}
//			}
//			break;
//		}	
//
//		if (!ret.empty())
//			strncpy_s((char *)&DefCode, sizeof(DefCode), ret.c_str());
//		return ret;
//	}
//	bool MakeUnderlyingContract(TContractSingleKey & contract) const
//	{
//		if (pCommodity == nullptr || StrikePrice[0] == 0)
//			return false;
//		strncpy_s(contract.Sign, Sign);
//		strncpy_s(contract.ExchangeNo, ExchangeNo);
//
//		strncpy_s(contract.CommodityNo, pCommodity->RelateCommodityNo);
//		contract.CommodityType = pCommodity->RelateCommodityType;
//
//		strncpy_s(contract.ContractNo, RelateContractNo);
//		contract.StrikePrice[0] = 0;
//		contract.OptionType = otNone;
//
//		return true;
//	}
//};

//上期所深度行情
struct TDeepQuote : public TContractKey
{
	TDirect					Direct;						//买卖方向
	double					Price;						//价格
	uint					Qty;						//数量
	TBool					IsLast;						//是否最后一笔
public:
	_InitStruct(TDeepQuote);
};

//定单关联信息查询请求
struct TOrderLinkInfoReq
{
	uint					OrderID;
	TLinkType				LinkType;					//关联信息类型
public:
	_InitStruct(TOrderLinkInfoReq);
};

struct TOrder;
//定单审核			
//key=OrderID			
struct TOrderCheck
{
	uint					OrderID;
	TCheckMode				CheckMode;					//审核类型
	TCheckState				CheckState;					//审核状态
	char					CheckInfo[51];				//审核信息
	char					CheckerNo[21];				//审核操作员
	char					CheckTime[21];				//审核时间

	const TOrder *			pOrder;
public:
	_InitStruct(TOrderCheck);
	uint Key() const
	{
		return OrderID;
	}
};

//下单 根据sign和userno可以找到loginUser，根据LoginUser和Sign可以确定连接信息，进而通过此连接发出委托
//是否要发出委托交给ITrans的实例去判断吧
//本地套利时 commodityType表示本地套利第一腿的品种类型，本地套利类型填在策略类型字段中
struct TSendOrder : public TContractKey
{
	char					UserNo[21];
	TOrderType				OrderType;					//定单类型 
	TOrderWay				OrderWay;					//委托来源 
	TValidType				ValidType;					//有效类型 
	char					ValidTime[21];				//有效日期时间(GTD情况下使用)
	TBool					IsRiskOrder;				//风险报单 
	TDirect					Direct;						//买卖方向 
	TOffset					Offset;						//开仓平仓 或 应价买入开平 
	THedge					Hedge;						//投机保值 
	double					OrderPrice;					//委托价格 或 期权应价买入价格
	double					TriggerPrice;				//触发价格
	TTriggerMode			TriggerMode;				//触发模式
	TTriggerCondition		TriggerCondition;			//触发条件
	uint					OrderQty;					//委托数量 或 期权应价数量
	uint					MinMatchQty;				//最小成交量	
	uint					MinOrderQty;				//冰山单最小随机量
	uint					MaxOrderQty;				//冰山单最大随机量

	TOffset					SellOffset;					//应价卖出开平 
	double					SellOrderPrice;				//应价卖出价格
	uint					SellOrderQty;				//应价卖出委托数量 
	THedge					SellHedge;					//应价卖出投保方向
	uint					StayTime;					//应价停留时间（秒）
	char					EnquiryNo[21];				//询价请求号，应价时用

	TStrategyType			StrategyType;				//策略类型

	char					Remark[51];					//下单备注字段，只有下单时生效。如果需要唯一标识一个或一组定单，最好以GUID来标识，否则可能和其他下单途径的ID重复
	TTradeSection			AddOneIsValid;				//T+1时段有效(仅港交所)
	uchar					MarketLevel;				//市价撮合深度,目前只有中金所支持，取值为0、1、5
	
	//增加
	uint					ParentOrderID;				//父单号,可以是TOrder::OrderID也可以是TSpreadOrder::OrderID等
	TBool					FutureAutoCloseFlag;		//行权后期货自对冲标记
public:
	TSendOrder()
	{
		memset(this, 0, sizeof(TSendOrder));

		CommodityType = ctNone;
		OptionType = otNone;
		OptionType2 = otNone;
		OrderType = otUnDefine;
		OrderWay = owUndefine;
		ValidType = vtNone;
		IsRiskOrder = bNo;
		Direct = dNone;
		Offset = oNone;
		SellOffset = oNone;
		Hedge = hNone;
		TriggerMode = tmNone;
		TriggerCondition = tcNone;
		StrategyType = stNone;
		AddOneIsValid = tsDay;
		SellHedge = hNone;
		FutureAutoCloseFlag = bNo;
	}
	char IsInputOrder() const
	{
		return (OrderWay == owCarry || OrderWay == owManual) ? bYes : bNo;
	}
	string GetCode(const char * fmt_str = nullptr) const
	{
		return TContractKey::GetCode(fmt_str);
	}
};

//定单修改
struct TModifyOrder : public TSendOrder
{
	uint					OrderID;					//报单在客户端的编号，即应答号
	char					ServerFlag;					//服务器标识
public:
	_InitStruct(TModifyOrder);
};

struct SCommodity;
//委托回报
//key1=orderid		
//key2=orderno+Sign		
struct TOrder : public TModifyOrder
{
	char					OrderNo[21];				//服务器编写的委托唯一编号
	char					LocalNo[21];				//本地号
	char					SystemNo[51];
	char					ExchangeSystemNo[51];		//交易所系统号
	char					TradeNo[11];				//交易编码
	
	char					UpperNo[11];				//使用上手(会员号)
	char					UpperChannelNo[11];			//上手通道号(席位号)

	char					InsertNo[21];				//下单人
	char					InsertDateTime[21];			//下单时间
	char					UpdateNo[21];				//最后一次变更人
	char					UpdateDateTime[21];			//最后一次变更时间

	TOrderState				OrderState;					//委托状态 

	double					MatchPrice;					//成交价
	uint					MatchQty;					//成交量
	uint					PreWorkingQty;				//上次排队量，动态计算排队数量时使用	

	double					SellMatchPrice;				//应价卖出成交量
	uint					SellMatchQty;				//应价买入成交量

	int						ErrorCode;					//最新信息码				
	char					ErrorText[51];				//最新错误信息

	TBool					IsInput;
	TDeleteType				Deleted;					//是否已删 //可否放在定单状态上
	TBool					AddOne;						//T+1 TBool
	TOrderMark				OrderMark;					//定单标记 omAttention， omIgnore

	uint					StreamID;					//委托流号

	char					Currency[11];				//商品币种
	char					FeeCurrency[11];			//手续费币种
	double					FrozenFee;					//冻结手续费
	double					FrozenDeposit;				//冻结保证金 合并计算的此项为

	//历史委托 需要自己更新pCommodity
	char					SettleDate[11];

	//新增--------------------------------------
	const SCommodity *		pCommodity;					//关联品种指针
	const TOrderCheck *		pCheckInfo;					//定单审核信息
public:
	_InitStruct(TOrder);
	union FP_TYPE
	{
		double				DBL;		//声明第一个，便于初始化
		unsigned long long	LL;
	};
	string Key() const
	{
		return GetKey(2, OrderNo, Sign);
	}
	uint KeyID() const
	{
		return OrderID;
	}
	string OrdID() const
	{
		return StrFun::format("%d", OrderID);
	}
	string KeyNo() const
	{
		return GetKey(2, OrderNo, Sign);
	}
	static string GetHisKey(const char * order_no, const char * settle_date, const char * sign)
	{		
		return GetKey(3, order_no, settle_date, sign);
	}
	string HisKey() const
	{
		return GetHisKey(OrderNo, SettleDate, Sign);
	}
	string InsertDate() const
	{
		return string(InsertDateTime, 10);
	}
	string InsertTime() const
	{
		return &InsertDateTime[10];
	}
	string UpdateDate() const
	{
		return string(UpdateDateTime, 10);
	}
	string UpdateTime() const
	{
		return &UpdateDateTime[10];
	}
	char IsInputOrder() const
	{
		return (OrderWay == owCarry || OrderWay == owManual  || IsInput == bYes) ? bYes : bNo;
	}
	bool CanCancel() const
	{
		return (OrderState == osTriggering || OrderState == osActive || OrderState == osQueued || OrderState == osPartFilled || OrderState == osCanceling || OrderState == osModifying || 
			OrderState == osChecking || OrderState == osSuspended || OrderState == osApply || OrderState == osPartTriggered) && Deleted != bYes;
	}
	bool IsEnd()
	{
		return OrderState == osFilled || OrderState == osCanceled || OrderState == osPartCanceled || OrderState == osFail || OrderState == osPartFailed || OrderState == osInvalid|| Deleted == bYes;
	}
	bool CanModify() const
	{
		return CanCancel();
	}
	bool CanCommit() const
	{
		return StrategyType == stPreOrder && OrderState == osTriggering && Deleted != bYes;
	}
	bool CanParent()
	{
		return (otIceberg == OrderType || otGhost == OrderType) && SystemNo[0] != 0;
	}
	int ParentLevel() const
	{
		return 0;
	}
	string Code(const char * fmt_str = nullptr) const
	{
		return GetCode(fmt_str);
	}
	const SCommodity * Commodity() const
	{
		return (nullptr == pCommodity) ? nullptr : pCommodity;
	}
	uint GetWorkingQty() const
	{
		if (CanCancel() && OrderState!=osTriggering)
			return OrderQty - MatchQty + SellOrderQty - SellMatchQty;
		else
			return 0;
	}
	bool IsMatched() const
	{
		return MatchQty>0 || OrderState == osFilled || OrderState == osPartFilled || OrderState == osPartCanceled;
	}
	const char CheckMode() const	//审核方式
	{
		return (pCheckInfo == nullptr) ? cmNone : pCheckInfo->CheckMode;
	};
	const char CheckState() const//审核状态
	{
		return (pCheckInfo == nullptr) ? csNone : pCheckInfo->CheckState;
	};
	const char * CheckInfo() const//审核信息
	{
		return (pCheckInfo == nullptr) ? EMPTY_STR : pCheckInfo->CheckInfo;
	};
	const char * CheckerNo() const//审核操作员
	{
		return (pCheckInfo == nullptr) ? EMPTY_STR : pCheckInfo->CheckerNo;
	};
	const string CheckDate() const
	{
		return (pCheckInfo == nullptr) ? EMPTY_STR : string(pCheckInfo->CheckTime, 10);
	};
	const string CheckTime() const 	//审核时间
	{
		return (pCheckInfo == nullptr) ? EMPTY_STR : &pCheckInfo->CheckTime[10];
	};
	static int FormatPrice(const SPriceType price, const SCommodityPrecType prec, const SCommodityDenoType deno, SPriceStrType out, bool showdeno)
	{
		SCommodityPrecType tmp_prec(prec);
		if (tmp_prec > 8)
			tmp_prec = 8;

		//判断浮点数有效性，过大或过小，或NAN，或无穷大，均无效
		FP_TYPE fp = { price };
		unsigned short exp((fp.LL >> 52) & 0x7FF);
		if (0 != price)
			if (exp > 1100 || exp < 990)
				return 0;

		//处理符号位
		size_t pos(0);
		if (fp.LL >> 63 != 0)
		{
			fp.LL &= 0x7FFFFFFFFFFFFFFF;
			if (0 != fp.LL)		//处理负零
				out[pos++] = '-';
		}

		//修正小数位，在去除符号位之后处理 
		if (deno <= 1)
		{
			//2016-05-19修正为四舍五入方式
			fp.DBL += (5 * pow(0.1, tmp_prec + 1));
		}

		//处理整数部分
		unsigned int num((unsigned int)fp.DBL);
		if (deno <= 1 || 0 != num)	//整数部分为0的分数不输出整数部分
		{
			_ultoa_s(num, &out[pos], sizeof(SPriceStrType) - pos, 10);
			pos = strnlen_s(out, sizeof(SPriceStrType) - 1);
		}

		//处理小数部分(小数形式 或 分数形式)
		if (deno <= 1)
		{
			if (0 == tmp_prec)
				return 1;

			out[pos++] = '.';

			while (tmp_prec > 0)
			{
				fp.DBL *= 10;
				out[pos++] = '0' + (wchar_t)((unsigned int)fp.DBL % 10);
				tmp_prec--;
			}

			out[pos] = '\0';
		}
		else			//分数
		{
			//分数不能修正，分母过大，会导致分子计算错误
			//fp.DBL += 0.000000001;	//修正数值
			//fp.DBL += pow(0.1, prec+1);	//修正数值

			if (0 != num)
				out[pos++] = '+';

			fp.DBL -= (unsigned int)fp.DBL;
			fp.DBL *= deno;

			fp.DBL += 0.5;	//四舍五入分子部分 2016-06-30修改

			_ultoa_s((unsigned int)fp.DBL, &out[pos], sizeof(SPriceStrType) - pos, 10);

			if (showdeno)
			{
				pos = strnlen_s(out, sizeof(SPriceStrType) - 1);

				out[pos++] = '/';
				_ultoa_s(deno, &out[pos], sizeof(SPriceStrType) - pos, 10);
			}
		}

		return 1;
	}

	static int FormatPrice(const SPriceType price, const SCommodity* commodity, SPriceStrType out, bool showdeno)
	{
		if (NULL == commodity)
			return 0;

		SCommodityPrecType prec(commodity->PricePrec);
		SCommodityDenoType deno(commodity->PriceDeno);

		return FormatPrice(price, prec, deno, out, showdeno);
	}
};

//行权录入
struct TExecuteInput :public TOrder
{
	char					FutureContractNo[11];				//标的月份
};

//排队单数量，关键字为UserNo+SingleContractKey
//key=UserNo+Sign+ExchangeNo+CommodityNo+CommodityType+ContractNo+StrikePrice+OptionType
struct TOrderQty :public TContractSingleKey
{
	char					UserNo[21];

	//不区分开平的时候只有下面两个字段
	uint					BuyQty;
	uint					SellQty;

	//区分开平的时候下边字段才有效
	uint					BuyOpenQty;
	uint					BuyCoverQty;		//平昨
	uint					BuyCoverDayQty;		//平今
	uint					SellOpenQty;
	uint					SellCoverQty;		//平昨
	uint					SellCoverDayQty;	//平今
public:
	_InitStruct(TOrderQty);
	string Key() const
	{
		return GetKey(2, UserNo, TContractSingleKey::conSingleKey().c_str());
	}
	static string MakeKey(const char * UserNo, const TContractSingleKey & contract)
	{
		return GetKey(2, UserNo, contract.conSingleKey().c_str());
	}
	void Update(const int AddQty, const char Direct, const char Offset)
	{
		if (Direct == dBuy)
		{
			BuyQty += AddQty;
			switch (Offset)
			{
			case oOpen: BuyOpenQty += AddQty; break;
			case oCover: BuyCoverQty += AddQty; break;
			case oCoverT: BuyCoverDayQty += AddQty; break;
			}
		}
		else
		{
			SellQty += AddQty;
			switch (Offset)
			{
			case oOpen: SellOpenQty += AddQty; break;
			case oCover: SellCoverQty += AddQty; break;
			case oCoverT: SellCoverDayQty += AddQty; break;
			}
		}
	}
};

//询价通知
//key=EnquiryNo+Sign
struct TEnquiry : public TContractKey
{
	char					EnquiryNo[21];				//请求序号
	TDirect					ReqDirect;					//请求方向 
	TEnquirerType			ReqBy;						//请求方   
	char					EnquiryDateTime[21];		//询价时间
public:
	_InitStruct(TEnquiry);
	string Key()
	{
		return GetKey(2, EnquiryNo, Sign);
	}	
	string EnquiryDate() const
	{
		return string(EnquiryDateTime, 10);
	}
	string EnquiryTime() const
	{
		return &EnquiryDateTime[10];
	}
};

//成交回报 关键字需要调整，现在这样不能满足唯一标识的要求 交易所+上手+成交号+买卖
//key=MatchNo+Sign
struct TMatch :public TContractSingleKey
{
	char					MatchNo[21];				//成交序号

	char					UserNo[21];

	TOrderWay				MatchWay;
	TDirect					Direct;
	TOffset					Offset;
	THedge					Hedge;

	uint					StreamID;
	char					ServerFlag;
	char					OrderNo[21];				//关联定单编号
	char					ExchangeMatchNo[51];		//原始成交号
	char					MatchDateTime[21];
	char					UpperMatchDateTime[21];

	double					MatchPrice;
	uint					MatchQty;

	TDeleteType				Deleted;					//是否删除 

	char					Currency[11];				//商品币种
	TBool					ManualFee;					//手工手续费 
	char					FeeCurrency[11];			//手续费币种
	double					MatchFee;					//成交手续费
	double					Premium;					//权利金收支，收入为正，支出为负  = 成交价 * 成交量 * 乘数
	double					CoverProfit;				//平仓盈亏 或 未到期平赢

	char					SettleDate[11];				//历史成交用

	char					UpperNo[11];				//使用上手(会员号)
	TBool					AddOne;						//T+1标记 
	double					CoverPrice;					//平仓时优先平掉该价格的持仓
	//新增------------------------------------------------------------
	const TOrder *			pOrder;
	const SCommodity *		pCommodity;					//关联品种指针
public:
	_InitStruct(TMatch);
	string Key() const
	{
		return GetKey(2, MatchNo, Sign);
	}
	string HisKey() const
	{
		return GetKey(3, OrderNo, SettleDate, Sign);
	}
	string OrderCode() const
	{
		return pOrder ? pOrder->GetCode() : "";
	}
	string Code(const char * fmt_str = nullptr) const
	{
		return GetCode(fmt_str);
	}
	char OrderType() const
	{
		return pOrder ? pOrder->OrderType : otNone;
	}
	const TContractKey * OrderContractKey() const
	{
		return pOrder ? pOrder : NULL;
	}
	const SCommodity * OrderCommodity() const
	{
		return pOrder ? pOrder->Commodity() : Commodity();
	}
	const SCommodity * Commodity() const
	{
		return pCommodity ? pCommodity : nullptr;
	}
	string CommodityName() const
	{
		return Commodity() ? Commodity()->CommodityName: "";
	}
	string InsertNo() const
	{
		return pOrder ? pOrder->InsertNo : "";
	}
	string SystemNo() const
	{
		return pOrder ? pOrder->SystemNo : "";
	}
	string MatchDate() const
	{
		return string(MatchDateTime, 10);
	}
	string MatchTime() const
	{
		return &MatchDateTime[10];
	}
	string UpperMatchDate() const
	{
		return string(UpperMatchDateTime, 10);
	}
	string UpperMatchTime() const
	{
		return &UpperMatchDateTime[10];
	}
};
//成交录入
struct TInputMatch :public TMatch
{
	TBool					IsCarry;					//是否carry单
	char					SystemNo[51];				//系统号		
public:
	_InitStruct(TInputMatch);
};

//风险参数
struct TOptionRiskParam
{
	double					Delta;
	double					Gamma;
	double					Theta;
	double					Vega;
	double					Rho;
	double					IV;
public:
	_InitStruct(TOptionRiskParam);
	static uint Key()
	{
		return sdtOptionRiskParam;
	}
};

//持仓保证金
//key=PositionNo, Sign
struct TPositionDeposit
{
	TDepositVertion			Version;					//版本号	

	char					Sign[21];
	char					PositionNo[51];				//持仓关键字

	double					PreSettle;					//前结算价	ver3
	double 					Deposit;					//保证金		ver3
	double					KeepDeposit;				//维持保证金	ver3

	double					LastPrice;					//最新价
	double					FloatProfit;				//浮盈		ver9
	double					FloatProfitTBT;				//逐笔浮赢 trade by trade
	double					OptionMarketValue;			//期权市值	ver9

	char					UpdateTime[9];				//更新时间
	double					UnderlyingPrice;			//期权最新价，取不到可以不填
public:
	_InitStruct(TPositionDeposit);
	string Key()
	{
		return GetKey(2, PositionNo, Sign);
	}
};

//持仓明细
//key=PositionNo+Sign
struct TPosition : public TContractSingleKey
{
	char					PositionNo[51];

	char					UserNo[21];

	TDirect					Direct;
	THedge					Hedge;						//投保 

	uint					StreamID;
	char					ServerFlag;
	TBool					IsCmb;						//是否组合持仓
	char					MatchNo[21];				//成交关键字
	char					MatchDateTime[21];

	double					PositionPrice;				//价格
	uint					PositionQty;				//总持仓量	
	
	char					UpperNo[11];				//使用上手(会员号)
	char					CurrencyNo[11];

	double					ProfitCalcPrice;			//浮盈计算价
	double					FloatProfit;				//浮盈
	double					FloatProfitTBT;				//逐笔浮赢 trade by trade

	double					DepositCalcPrice;			//老仓保证金计算价（昨结算），新仓为成交价
	double					Deposit;					//客户初始保证金
	double					KeepDeposit;				//客户维持保证金			
	double					MarketValue;				//期权市值 = 最新价 * 持仓量 * 乘数
	char					DepositUpdateTime[9];		//保证金更新时间					

	uint					CanCoverQty;				//可平量(暂未使用)
	uint					ParOrderQty;				//平仓挂单量(暂未使用)

	char					SettleDate[11];				//历史持仓用	
	//add--------------------------------------		
	double					OptionValue;				//期权虚实值
	uint					PrePositionQty;				//昨仓数量
	double					StrikeProfit;				//期权执行盈亏 = (标的最新价 - 执行价) * 持仓量 * 乘数;
	double					Delta;
	double					Gamma;
	TPositionType			SourcePositionType;			//持仓来源的合计类型
	const SCommodity *		pCommodity;
	const TMatch *			pMatch;						//昨仓没有成交信息
public:
	_InitStruct(TPosition);
	string Key() const	//明细的关键字又后台生成， 小计、合计的关键字有数据管理生成 通用的
	{
		return GetKey(2, PositionNo, Sign);
	}
	string SubKey() const
	{
		if (SourcePositionType == ptTotal)
			return TotalKey();

		SPriceStrType sPrice = { 0 };
		TOrder::FormatPrice(PositionPrice, Commodity(), sPrice, true);//PrecisionType()); 这里如果是套利持仓PrecisionType()和平完仓后的PrecisionType()不一致，导致关键字不同而无法平仓
		return GetSubKey(UserNo, *this, Direct, sPrice);
	}
	string TotalKey() const
	{
		return GetTotalKey(UserNo, *this, Direct);
	}
	string CommodityKey() const
	{
		return GetCommodityKey(UserNo, *this, Direct);
	}
	static string GetSubKey(const char * AUserNo, const TContractSingleKey & AContractKey, const char ADirect, const char * APrice, const TPositionType	ASourcePositionType = ptDetail)
	{
		if (ASourcePositionType == ptTotal)
			return GetTotalKey(AUserNo, AContractKey, ADirect);

		char direct[2] = { ADirect, 0 };
		return GetKey(4, AUserNo, AContractKey.conSingleKey().c_str(), direct, APrice);
	}
	static string GetTotalKey(const char * AUserNo, const TContractSingleKey & AContractKey, const char ADirect)
	{
		char direct[2] = { ADirect, 0 };
		return GetKey(3, AUserNo, AContractKey.conSingleKey().c_str(), direct);
	}
	static string GetCommodityKey(const char * AUserNo, const TContractSingleKey & AContractKey, const char ADirect)
	{
		char direct[2] = { ADirect, 0 };
		return GetKey(3, AUserNo, AContractKey.comKey().c_str(), direct);
	}
	string Code(const char * fmt_str = nullptr) const
	{
		return GetCode(fmt_str);
	}
	const SCommodity * Commodity() const
	{
		if (nullptr == pCommodity)
			return pMatch == nullptr ? nullptr : pMatch->Commodity();
		else if (pCommodity)
			return pCommodity;
		else
			return nullptr;
	}
	string CommodityName() const
	{
		if (nullptr == Commodity())
			return "";
		else
			return Commodity()->CommodityName;
	}
	string CommodityGroupNo()const
	{
		if (nullptr == Commodity())
			return "";
		else
			return Commodity()->DepositGroupNo;
	}
	string MatchDate() const
	{
		return string(MatchDateTime, 10);
	}
	string PositionDate() const
	{
		return string(MatchDateTime, 10);
	}
	string PositionTime() const
	{
		return &MatchDateTime[10];
	}
	int Precision(bool bAvg = false) const
	{
		if (Commodity() != nullptr)
			return Commodity()->PricePrec;
		else
			return 2;
	}
	string PositionPriceStr()
	{
		char sPrice[KEY_LENGTH];
		TOrder::FormatPrice(PositionPrice, Commodity(), sPrice, true);
		return sPrice;
	}
	//用于持仓明细
	void UpdateDeposit(TPositionDeposit & deposit)
	{		
		if (strcmp(PositionNo, deposit.PositionNo)!=0)
			return;

		strncpy_s(DepositUpdateTime, deposit.UpdateTime);

		//3.0的保证金去后台的，9.0外盘的保证金没值，内盘持仓自带保证金，且不会变化
		if (deposit.Version == dvEsunny3)
		{
			Deposit			= deposit.Deposit;
			KeepDeposit		= deposit.KeepDeposit;
		}	

		ProfitCalcPrice		= deposit.LastPrice;
		DepositCalcPrice	= deposit.PreSettle;

		double dFloatCalcPrc= ProfitCalcPrice;
		double dPositionPrc	= PositionPrice;
		if (pCommodity!=nullptr)
		{
			double ProductDot		= pCommodity->TradeDot;
			//必须四舍五入到合约精度，不然乘过合约点值和数量后结果误差会很大
			if (SourcePositionType != ptTotal)
			{
				dFloatCalcPrc		= MathFun::SimpleRoundTo(dFloatCalcPrc		, Commodity()->CoverMode == cmUnfinish ? Precision() + 1 : Precision());
				dPositionPrc		= MathFun::SimpleRoundTo(dPositionPrc		, Commodity()->CoverMode == cmUnfinish ? Precision() + 1 : Precision());
				DepositCalcPrice	= MathFun::SimpleRoundTo(DepositCalcPrice	, Commodity()->CoverMode == cmUnfinish ? Precision() + 1 : Precision());
			}

			//浮盈
			FloatProfitTBT			= (dFloatCalcPrc - dPositionPrc) * PositionQty * ProductDot;
			FloatProfit				= (dFloatCalcPrc - dPositionPrc) * ProductDot * (PositionQty - PrePositionQty) + (dFloatCalcPrc - DepositCalcPrice) * ProductDot * PrePositionQty;
			if (Direct == dSell)
			{
				FloatProfitTBT		= -FloatProfitTBT;
				FloatProfit			= -FloatProfit;
			}

			//期权
			if (CommodityType==ctOption)
			{
				if (deposit.Version == dvDaystar)
					Deposit			= deposit.Deposit;

				//市值
				MarketValue			= dFloatCalcPrc * ProductDot * PositionQty;
				if (Direct == dSell)
					MarketValue		= -MarketValue;

				if (deposit.UnderlyingPrice == 0)
					return;
				//执行盈亏
				double dStrikePrice = atof(StrikePrice);
				StrikeProfit		= (deposit.UnderlyingPrice - dStrikePrice * pCommodity->ExerciseDot) * PositionQty * ProductDot;
				if (Direct == dBuy && OptionType == otPut || Direct == dSell && OptionType == otCall)
					StrikeProfit	= StrikeProfit;
				StrikeProfit		+= (Direct == dBuy ? -1 : 1) * dPositionPrc * PositionQty * ProductDot;

				//虚实值
				OptionValue			= deposit.UnderlyingPrice - dStrikePrice;
				if (OptionType == otPut)
					OptionValue		= -OptionValue;
			}
		}
	}	
	//用于持仓合计、小计
	void Update(const TPosition * old_detail, const TPosition * detail)
	{		
		OptionValue					= detail->OptionValue;
		strncpy_s(DepositUpdateTime	, detail->DepositUpdateTime);
		DepositCalcPrice			= detail->DepositCalcPrice	;
		ProfitCalcPrice				= detail->ProfitCalcPrice	;

		if (old_detail==nullptr)
		{
			Deposit				+= detail->Deposit			;
			KeepDeposit			+= detail->KeepDeposit		;
			FloatProfit			+= detail->FloatProfit		;
			FloatProfitTBT		+= detail->FloatProfitTBT	;
			MarketValue			+= detail->MarketValue		;
			StrikeProfit		+= detail->StrikeProfit		;
			Delta				+= detail->Delta			;
			Gamma				+= detail->Gamma			;
		}
		else
		{
			Deposit				+= (detail->Deposit			- old_detail->Deposit		);
			KeepDeposit			+= (detail->KeepDeposit		- old_detail->KeepDeposit	);
			FloatProfit			+= (detail->FloatProfit		- old_detail->FloatProfit	);
			FloatProfitTBT		+= (detail->FloatProfitTBT	- old_detail->FloatProfitTBT);
			MarketValue			+= (detail->MarketValue		- old_detail->MarketValue	);		//市值
			StrikeProfit		+= (detail->StrikeProfit	- old_detail->StrikeProfit	);		//执行盈亏
			Delta				+= (detail->Delta			- old_detail->Delta			);
			Gamma				+= (detail->Gamma			- old_detail->Gamma			);
		}
	}
};

typedef TPosition TSubPosition;
typedef TPosition TTotalPosition;

//所有合约总持仓的多头、空头
struct TAllPosition
{
	uint					Long;
	uint					Short;
public:
	_InitStruct(TAllPosition);

	static uint Key()
	{
		return sdtAllPosition;
	}
};

//平仓回报
//key=LiquidateNo+Sign
struct TLiquidate : public TContractSingleKey
{
	char					LiquidateNo[21];			//平仓编号

	char					OpenMatchNo[21];			//开仓成交号
	char					CoverMatchNo[21];			//平仓成交号

	char					UserNo[21];					//资金帐号

	TDirect					CoverDirect;				//买卖 
	THedge					Hedge;						//投保 
	double					OpenPrice;					//开仓价
	double					CoverPrice;					//平仓价
	uint					CoverQty;					//数量

	uint					StreamID;
	char					OpenServerFlag;
	char					CloseServerFlag;

	char					Currency[11];				//品种币种	
	double 					Premium;					//权利金
	double					CloseProfit;				//平仓盈亏   盯市
	double					PreSettle;					//昨结算
	double					CloseProfitTBT;				//平仓总盈亏 开仓价 - 平仓价 逐笔
	double					UnExpProfit;				//未到期平盈
	//新增--------------------------
	const TMatch *			pOpenMatch;
	const TMatch *			pCoverMatch;
public:
	_InitStruct(TLiquidate);
	string Key() const
	{
		return GetKey(2, LiquidateNo, Sign);
	}
	string Code(const char * fmt_str = nullptr) const
	{
		return GetCode(fmt_str);
	}
	string CommodityName() const
	{
		if (nullptr == pOpenMatch && nullptr == pCoverMatch)
			return "";
		else if (pOpenMatch != nullptr)
			return pOpenMatch->CommodityName();
		else
			return pCoverMatch->CommodityName();
	}
	const SCommodity * Commodity() const
	{
		if (nullptr == pOpenMatch && nullptr == pCoverMatch)
			return nullptr;
		else if (pOpenMatch != nullptr)
			return pOpenMatch->pCommodity;
		else
			return pCoverMatch->pCommodity;
	}
	string OpenUpperNo() const
	{
		if (nullptr == pOpenMatch)
			return "";
		else
			return pOpenMatch->UpperNo;
	}
	string CoverUpperNo() const
	{
		if (nullptr == pCoverMatch)
			return "";
		else
			return pCoverMatch->UpperNo;
	}
};

//用户信息
//key=UserNo+Sign		
struct TUserInfo
{
	char					Sign[21];
	char					LoginUser[21];				//登录账号         //查询多账户多后台数据的时候找到登陆用户再找具体数据

	char					UserNo[21];
	char					UserName[21];
	TUserType				UserType;					//用户类型，单客户、机构户
	TBool					IsPermittee;				//是否持证人
	TBool					IsMarketMaker;				//是否做市商

	char					FullName[51];				//全名
	char					FullEngName[51];
	char					Phone[51];					//电话
	char					IdentigyNo[21];				//证件号
	char					Address[101];				//地址
	char					Remark[182];				//备注
	char					SettleGroup[11];			//结算分组
	double					DepositThreshold;			//保证金阈值

	bool					QryMoneyed;					//资金是否定制成功
	bool					QryDeliveryed;				//是否已查过交割数据

	const TLogin *			pLogin;						//所属登录账号
public:
	_InitStruct(TUserInfo);
	string Key() const
	{
		return GetKey(2, UserNo, Sign);
	}
};

//资金回报
//key=UserNo+CurrencyNo+Sign
struct TMoney
{
	char					Sign[21];
	char					UserNo[21];
	char					CurrencyGroupNo[11];		//币种组编号
	char					CurrencyNo[11];				//币种号(Currency_Base表示币种组基币资金)

	double					ExchangeRate;				//币种汇率

	TFuturesAlg				FutureAlg;					//期货算法
	TOptionAlg				OptionAlg;					//期权算法

	double					PreAvailable;				//昨可用
	double					PreBalance;					//昨账面
	double					PreEquity;					//昨权益
	double					PreMarketEquity;			//昨市值权益
	double					PreUnExpProfit;				//昨未结平盈
	double					PreLmeFloatExpProfit;		//昨lme浮盈

	double					Adjust;						//资金调整
	double					CashIn;						//入金
	double					CashOut;					//出金

	double					FrozenFee;					//冻结手续费20
	double					FrozenDeposit;				//冻结保证金19

	double					Fee;						//手续费(包含交割手续费)
	double					TradeFee;					//交易手续费17
	double					DeliveryFee;				//交割手续费17
	double					ExchangeFee;				//汇兑手续费

	double					PremiumIncome;				//权利金收取
	double					PremiumPay;					//权利金支付
	double					CoverProfit;				//平盈 盯市
	double					CoverProfitTBT;				//逐笔平盈
	double					DeliveryProfit;				//交割盈亏
	double					UnExpProfit;				//未结平盈
	double					ExpProfit;					//到期平仓盈亏

	double					FloatProfit;				//不含LME持仓盈亏,盯市 market to market
	double					LmeFloatProfit;				//LME持仓盈亏
	double					FloatProfitTBT;				//逐笔浮赢 trade by trade

	double					MarketValue;				//期权市值23

	double					Premium;					//权利金
	double					Deposit;					//保证金
	double					KeepDeposit;				//维持保证金
	double					Discount;					//LME贴现金额15

	double					AuthMoney;					//授信资金

	double					Balance;					//今资金=PreBalance+Adjust+CashIn-CashOut-Fee(TradeFee+DeliveryFee+ExchangeFee)+CoverProfitTBT+Premium 
	double					Equity;						//今权益=Balance+FloatProfitTBT(NewFloatProfit+LmeFloatProfit)+UnExpiredProfit
	double					Available;					//今可用=Equity-Deposit-Frozen(FrozenDeposit+FrozenFee)
	double					CanCashOut;					//今可提=min(Money, Available);//Equity-Deposit-max(UnExpiredProfit)-max(NewFloatProfit+LmeFloatProfit，0)	
	double					AccoutMarketValue;			//账户市值=Equity+OptionMarketValue			
	double					OriCash;					//币种原始出入金24(非自动汇兑资金)
	double					FrozenRisk;					//风险冻结
	//特殊产品------------------------
	double					PreFundPledgedIn;			//昨货币质入
	double					PreFundPledgedOut;			//昨货币质出
	double					FundPledgedIn;				//今货币质入
	double					FundPledgedOut;				//今货币质出
	double					FundCashPledged;			//货币质押余额
	double					PledgedableFund;			//可质押货币
	double					PositionPledged;			//仓单质押
	double					SpecDeposit;				//特殊产品保证金
	double					SpecFrozenDeposit;			//特殊产品冻结保证金
	double					SpecFee;					//特殊产品手续费
	double					SpecFrozenFee;				//特殊产品冻结手续费
	double					SpecFloatProfit;			//特殊产品浮盈
	double					SpecCoverProfit;			//特殊产品平盈
	//增加----------------------------
	double					RiskRate;					//风险率=(Deposit+Frozen(FrozenDeposit+FrozenFee))/Equity
	double					RiskRateDown;				//风险率倒数
	double					SelfRiskRate;				//自有风险率=KeepDeposit/（总权益-受信资金）
	char					UpdateTime[11];				//更新时间
	double					NetProfit;					//净盈利 = CoverProfitTBT - Fee
	double					ProfitRate;					//盈利率 = NetProfit / PreAvailable + CashIn - CashOut + Adjust
	const TUserInfo *		pUser;
public:
	_InitStruct(TMoney);
	string Key() const
	{
		return GetKey(3, UserNo, CurrencyNo, Sign);
	}	
	bool isValid() const
	{
		return Balance!=0 || Equity!=0 || Available!=0 || PreBalance!=0;	
	}	
	string UserName() const
	{
		return pUser ? pUser->UserName : "";
	}
};

//历史出入金
struct TCash
{
	char					Sign[21];
	char					UserNo[21];
	TCashType				CashType;
	TCashMode				CashMode;
	char					CurrencyNo[11];
	double					OperAmount;
	char					Remark[101];

	char					UserBank[3];				//银行标识
	char					UserAccount[51];			//银行账户
	TBool					UsertAccountType;			//是否本币 本外币账户标识 
	char					CompanyBank[3];				//银行标识
	char					CompanyAccount[51];			//银行账户
	TBool					CompanyAccountType;			//是否本币 本外币账户标识 

	int						StreamID;
	int						RemoteStreamID;
	TCheckState				CheckState;
	char					OperDateTime[21];
	char					OperatorNo[11];
	char					CheckDateTime[21];
	char					CheckerNo[11];
	char					SettleDate[11];

	const TUserInfo *		pUserInfo;
public:
	_InitStruct(TCash);
};

//交割数据
//key=DeliveryNo+Sign
struct TDelivery : public TContractSingleKey
{
	char					DeliveryNo[21];

	char					OpenDate[11];				//开仓日期
	char					DelivDate[11];				//交割日期
	char					UserNo[21];					//客户
	char					OpenNo[51];					//结算用成交编号，日期 + 成交流号

	TOrderWay				MatchWay;					//成交来源 
	TDirect					OpenDirect;					//开仓方向 
	uint					DelivQty;					//交割数量
	uint					FrozenQty;					//冻结数量
	double					OpenPrice;					//开仓价格
	double					DelivPrice;					//交割价格
	double					Fee;						//手续费
	double					UpperFee;					//上手手续费
	double					DelivProfit;				//盈亏
	double					Deposit;					//保证金
	double					KeepDeposit;				//维持保证金
	double					UpperDeposit;				//上手保证金
	double					UpperKeepDeposit;			//上手维持保证金
	char					UpperNo[11];				//上手
	char					FeeCurrencyNo[11];			//手续费币种
	char					CurrencyNo[11];				//品种币种
	TDeliveryMode			DeliveryMode;				//交割方式 
	char					UnderlyingContractNo[11];	//期权标的合约（月份）
	//增加----------------------------
	const SCommodity *		pCommodity;
public:
	_InitStruct(TDelivery);
	string Key() const
	{
		return GetKey(2, DeliveryNo, Sign);
	}
	string CommodityName() const
	{
		if (nullptr == pCommodity )
			return "";
		else
			return pCommodity->CommodityName;
	}
};

//账单查询
struct TBillQry
{
	char					UserNo[21];
	TBillType				BillType;
	char					BillDate[11];				//月账单和日账单显示的长度不一样
	TBillFormatType			FormatType;					//账单格式
public:
	_InitStruct(TBillQry);
};

//帐单回报
//key=UserNo+BillDate+Sign
struct TBill :public TBillQry
{
	char					Sign[21];
	int						ContentSize;				//正文大小
	char					Content[1];					//帐单正文
public:
	_InitStruct(TBill);
	static string MakeKey(const char * UserNo, const char * Sign, const char *  BillDate, const char BillType)
	{
		return GetKey(3, UserNo, BillDate, Sign) + KEY_SPLIT + BillType;
	}
	string Key() const
	{		
		return MakeKey(UserNo, Sign, BillDate, BillType);
	}
};

//key=LoginUser+Sign
struct TAgreementInfo
{
	char					Sign[21];
	char					LoginUser[21];
	int						ContentSize;				//正文大小
	char 					Content[1];					//帐单正文
public:
	_InitStruct(TAgreementInfo);
	string Key() const
	{
		return GetKey(2, LoginUser, Sign);
	}
};

//key=UserNo+BankNo+BankAccount+Currency+Sign
struct TBankBase
{
	char					Sign[21];
	char					UserNo[21];					//期货资金账号
	char					BankNo[11];					//银行标志
	char					BankAccount[51];			//银行卡号
	char					Currency[11];				//币种编号
public:
	_InitStruct(TBankBase);
	string Key() const
	{
		return GetKey(5, UserNo, BankNo, BankAccount, Currency, Sign);
	}
};

//账户转账信息
struct TUserBank : public TBankBase
{
	char					BankName[51];				//银行名称
	TBool					SwapState;      			//转账换汇状态
	TBool					TransState;  				//转账状态
public:
	_InitStruct(TUserBank);
	string Key() const
	{
		return TBankBase::Key();
	}
	bool Valid()
	{
		return SwapState == bYes || TransState == bYes;
	}
};

//银行余额查詢
struct TBankBalanceQry : public TBankBase
{
	char					TransAuth[21];				//银期转账认证密码
	char					BankPsw[21];				//银行密码
public:
	_InitStruct(TBankBalanceQry);
};

//银行余额查詢應答
struct TBankBalance : public TBankBase
{
	double					Balance;                    //可转资金
	uint					ErrorCode;					//错误码
	char	 				ErrorText[51];				//错误信息
public:
	_InitStruct(TBankBalance);
	static uint Key()
	{
		return sdtBankBalance;
	}
};

//期货可转资金查询
struct TFutureBalanceQry
{
	char					Sign[21];
	char					UserNo[21];
	char					Currency[11];
public:
	_InitStruct(TFutureBalanceQry);
};

//期货可转资金应答
struct TFutureBalance : public TFutureBalanceQry
{
	double					Balance;
public:
	_InitStruct(TFutureBalance);
	static uint Key()
	{
		return sdtFutureBalance;
	}
};

//銀行轉賬請求
struct  TTransferReq : public TBankBalanceQry
{
	TTransDirect			TransDirect;				//转账方向
	double					TransFund;					//转账金额
public:
	_InitStruct(TTransferReq);
};

//银期转账应答
//key=FutureSerialID+Sign
struct  TTransfer : public TBankBase
{
	TTransDirect			TransDirect;				//转账方向
	double					TransFund;					//转账金额
	TTransState				TransState;					//转账状态

	uint					BankSerialID;				//银行流水号
	uint					FutureSerialID;				//期货流水号
	char					TransTime[21];				//转账时间

	double					BankFee;					//手续费
	double					FutureFee;					//公司手续费

	uint					ErrorCode;					//错误码
	char	 				ErrorText[51];				//错误信息

	TTransInitiator 		Initiator;					//转账发起方
public:
	_InitStruct(TTransfer);
	string Key() const
	{
		char strID[21];
		_itoa_s(FutureSerialID, strID, 10);
		return GetKey(2, strID, Sign);
	}
	string TransferDate() const
	{
		return string(TransTime, 10);
	}
	string TransferTime() const
	{
		return &TransTime[10];
	}
};

//保证金监控中心登录令牌
struct TCfmmcToken
{
	char					Sign[21];

	char					CompanyNo[11];				//期货公司代码(会员码)
	char					UserNo[21];					//资金账号
	uint					TokenID;					//令牌ＩＤ
	char					Token[21];					//令牌认证码

	uint					ErrorCode;					//错误码
	char	 				ErrorText[51];				//错误信息
public:
	_InitStruct(TCfmmcToken);
	static uint Key()
	{
		return sdtCfmmcToken;
	}
};

//客户关系人
//key=UserNo+UserProxyNo+UserProxyType+Sign
struct TUserProxy
{
	char					Sign[21];
	char					UserNo[21];						//所属客户
	char					UserProxyNo[21];				//关系人
	char					UserProxyName[51];				//关系人名称
	TUserProxyType			UserProxyType;					//关系人类型
	char					IdentityType[21];				//证件类型
	char					IdentityId[51];					//证件ID
	char					IdExpireDate[11];				//证件到期日
	char					Cert[21];						//认证串
	char					CertExpireDate[11];				//CA到期日
	char					Country[11];					//地址的国家
	char					Province[21];					//地址的省/自治区/直辖市
	char					City[51];						//地址的市/县/区
	char					Address[101];					//地址
	char					PostNo[21];						//邮政编码
	char					TelPhoneNo[101];				//电话
	char					Birthday[11];					//生日
		
	//修订8
	char					CertificatesIssuedAddress[41];	//证件发行地
	TFATCAState				FATCAStatus;					//FATCA状态
	char					Nationality[41];				//国籍
	char					CountryOfResidence[41];			//居住地
	char					IsUSBeneficiary;				//是否有美国受益人
	char					IsUSTaxPayers;					//是否有美国缴税义务
	char					TINTaxNo[33];					//TIN税务号
	char					IsWaiver;						//是否取得Waiver
	TTaxType				TaxFormType;					//税表种类
	char					SignedTaxFromDate[11];			//签署税表日期
	char					MarginStandard[11];				//保证金水平
	TTradePlatFrom			TradePlatFrom;					//交易平台
	char					Fax[101];						//传真
public:
	_InitStruct(TUserProxy);
	string Key() const
	{
		return GetKey(3, UserNo, UserProxyNo, Sign) + KEY_SPLIT + UserProxyType;
	}
};

//币种信息
//key=CurrencyNo+Sign
struct TCurrency
{
	char					Sign[21];

	char					CurrencyNo[11];
	char					CurrencyName[21];
	char					CurrencyGroupNo[11];		//币种组编号 
	TBool					IsShare;					//是否与其他币种共享计算 
	double					ExchangeRate;				//交易汇率
public:
	_InitStruct(TCurrency);
	string Key() const
	{
		return GetKey(2, CurrencyNo, Sign);
	}
};

//可下单数量
struct TTradableQty : public TSendOrder
{
	uint					OpenQty;					//可开
	uint					CoverQty;					//可平

	int						MaxOrdID;					//原始定单号	判断是否重新下单
	double					Available;					//原始可用资金	判断可用资金是否发生变化
public:
	_InitStruct(TTradableQty);
	static uint Key()
	{
		return sdtTradableQty;
	}
	bool IsVaild(TSendOrder  * pOrder, int AMaxOrdID, double AAvailable) const
	{
		return pOrder && (MaxOrdID == AMaxOrdID) && MathFun::SameValue(Available, AAvailable, FLOAT_SAME_ERRORDIFF) &&
			strcmp(UserNo, pOrder->UserNo) == 0 && conKey() == pOrder->conKey() && Direct == pOrder->Direct &&
			Offset == pOrder->Offset && Hedge == pOrder->Hedge;
	}
};

struct TTradableQtySell : public TTradableQty
{
public:
	_InitStruct(TTradableQtySell);
	static uint Key()
	{
		return sdtTTradableQtySell;
	}
};

//下单冻结资金
struct TPreFreeze : public TSendOrder
{
	double					Deposit;					//本币保证金

	int						MaxOrdID;					//原始定单号	判断是否重新下单
	double					Available;					//原始可用资金	判断可用资金是否发生变化
	//add	
	const TCurrency *		pCurrency;
public:
	_InitStruct(TPreFreeze);
	static uint Key()
	{
		return sdtPreFreeze;
	}
	bool IsVaild(TSendOrder * pOrder, int AMaxOrdID, double AAvailable) const
	{
		return pOrder && (MaxOrdID == AMaxOrdID) && MathFun::SameValue(Available, AAvailable, FLOAT_SAME_ERRORDIFF) &&
			strcmp(UserNo, pOrder->UserNo) == 0 && conKey() == pOrder->conKey() && Direct == pOrder->Direct &&
			OrderQty == pOrder->OrderQty && Offset == pOrder->Offset && Hedge == pOrder->Hedge;
	}
	double DepositBase() const
	{
		if (pCurrency != nullptr)
			return Deposit * pCurrency->ExchangeRate;
		else
			return 0;
	}
	string CurrencyNo() const
	{
		if (pCurrency != nullptr)
			return pCurrency->CurrencyNo;
		else
			return "";
	}
	bool IsBaseCurrency() const
	{
		return pCurrency != nullptr && pCurrency->ExchangeRate == 1.0;
	}
};

struct TPreFreezeSell : public TPreFreeze
{
public:
	_InitStruct(TPreFreezeSell);
	static uint Key()
	{
		return sdtPreFreezeSell;
	}
};

//上手
//key=UpperNo+Sign
struct TUpper
{
	char					Sign[21];

	char					UpperNo[11];
	char					UpperName[21];
public:
	_InitStruct(TUpper);
	string Key() const
	{
		return GetKey(2, UpperNo, Sign);
	}
};

//消息
//Key2=MsgID+Sign
struct TMessage
{
	char					Sign[21];
	uint					MsgID;
	char					UserNo[21];					//接收者
	TMessageType			Type;						//消息类型  包括错误
	TMessageLevel			Level;						//消息级别
	char					SendNo[11];					//发送者
	char					SendDateTime[21];			//发送时间
	char					ValidDateTime[21];			//有效时间
	char					Title[101];					//标题
	uint					ContentSize;
	char					Content[1];					//正文
public:
	_InitStruct(TMessage);
	uint Key() const
	{
		return MsgID;
	}
	string KeyNo() const
	{
		char str[21];
		_itoa_s(MsgID, str, sizeof(str), 10);
		return GetKey(2, str, Sign);
	}
};

//3M到期日,key=Sign
struct T3MEndDate
{
	char					Sign[21];
	char					ContractNo[11];
public:
	_InitStruct(T3MEndDate);
	string Key() const
	{
		return Sign;
	}
};

//历史交易查询: 历史委托、历史成交、历史出入金、历史资金调整
struct THisTradeQry
{
	char					UserNo[21];
	char					BeginDate[11];
	char					EndDate[11];
public:
	_InitStruct(THisTradeQry);
};

//错误信息
struct TErrorInfo
{
	char					Sign[21];					
	char					LoginUser[21];				
	char					module[21];
	TradeNotifyType			ErrorSource;	//错误来源
	int						error_id;
	char					error_text[51];	//错误信息
public:
	_InitStruct(TErrorInfo);
};


//资金账号对应计算参数查询结构
struct	TFundCalcParamReq : public TContractSingleKey
{
	char					UserNo[21];					//资金账户
public:
	string Key()
	{
		return GetKey(2, UserNo, TContractSingleKey::conSingleKey().c_str());
	}
	_InitStruct(TFundCalcParamReq);
};

//资金账号对应计算参数应答结构
struct	TFundCalcParam : public TFundCalcParamReq
{
	char					CurrencyNo[11];				//币种编号

	double					OpenFeeRatio;				//开仓手续费比例
	double					CoverFeeRatio;				//平仓手续费比例
	double					CoverTodayFeeRatio;			//平今手续费比例
	double					OpenFeeAmount;				//开仓手续费数额
	double					CoverFeeAmount;				//平仓手续费数额
	double					CoverTodayFeeAmount;		//平今手续费数额
	
	double					StrikeFeeRatio;				//行权手续费比例	
	double					StrikeFeeAmount;			//行权手续费数额

	double                  SLongDepositRatio;			//多头投机保证金比例
	double                  SLongDepositAmount;			//多头投机保证金数额
	double                  SShortDepositRatio;			//空头投机保证金比例
	double                  SShortDepositAmount;		//空头投机保证金数额
	double                  HLongDepositRatio;			//多头套保保证金比例
	double                  HLongDepositAmount;			//多头套保保证金数额
	double                  HShortDepositRatio;			//空头套保保证金比例
	double                  HShortDepositAmount;		//空头套保保证金数额
	double                  ALongDepositRatio;			//多头套利保证金比例
	double                  ALongDepositAmount;			//多头套利保证金数额
	double                  AShortDepositRatio;			//空头套利保证金比例
	double                  AShortDepositAmount;		//空头套利保证金数额
	double                  MLongDepositRatio;			//多头做市保证金比例
	double                  MLongDepositAmount;			//多头做市保证金数额
	double                  MShortDepositRatio;			//空头做市保证金比例
	double                  MShortDepositAmount;		//空头做市保证金数额

	const TCurrency *		pCurrency;
public:
	_InitStruct(TFundCalcParam);
	string Key()
	{
		return TFundCalcParamReq::Key();
	}
};

//商品保证金，单项大边用
struct TCommodityDeposit: public TCommodityKey
{
	char					GroupNo[11];
	char					UserNo[21];
	double                  LongDeposit;
	double                  ShortDeposit;
public:
	_InitStruct(TCommodityDeposit);
	string Key()
	{
		return TCommodityKey::GetDeposidKey(UserNo, GroupNo);
	}
	void update(const TPosition * pdetail, const TPosition * old)
	{
		if (pdetail == nullptr)
			return;
		if (pdetail->Direct == dBuy)
			LongDeposit		+= pdetail->Deposit - (old ? old->Deposit : 0);
		else
			ShortDeposit	+= pdetail->Deposit - (old ? old->Deposit : 0);
	}
};


//委价偏离范围
struct TOrderPriceRange: public TCommodityKey
{	
	TRangeType				RangeType;	
	double					Range;
	TActionType				ActionType;
public:
	_InitStruct(TOrderPriceRange);
	string Key()
	{
		return TCommodityKey::comKey();
	}
};


#pragma pack(pop)


#endif