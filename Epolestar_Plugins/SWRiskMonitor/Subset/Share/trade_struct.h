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
//__VA_ARGS__�����Զ�����ɱ�����ĺ꣬"##"���ŵ��������ڿɱ�����ĸ���Ϊ0ʱ����������ǰ��Ķ���
#define GetKey(count, ...) StrFun::MergeString(KEY_SPLIT, count, ##__VA_ARGS__)
/*****************************************************************************************************/


//�����������ͣ�����ÿ�����͵�������ϵͳ��ֻ�����һ���ṹ��ʵ��
enum SingleDataType
{
	sdtAllPosition			= 0 ,	//�ܳ����ܳ���
	sdtOptionRiskParam		,		//��Ȩ���ղ���
	sdtTradableQty			,		//���µ�����
	sdtPreFreeze			,		//�µ�Ԥ�����ʽ�
	sdtTTradableQtySell		,
	sdtPreFreezeSell		,
	sdtBankBalance			,		//���п�ת���
	sdtFutureBalance		,		//�ڻ���ת���
	sdtCfmmcToken					//��֤������������
};

#pragma pack(push, 1)


//���׺�̨������Ϣ
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
	char					Sign[21];					//��ʾ��������ʶ����ip��portΪ����, �ڶ��˻����̨�����ʹ��						
	char 					IP[40];						//����һ�㣬�Ա����������ַ��ʽ
	ushort					Port;
	TServerType				ServerType;					//��¼�ķ���������
public:
	_InitStruct(TAddress);
};

//���ӡ��Ͽ�			
struct TConnect : public TAddress
{
	char					SiteAddrNo[21];				//վ����
	double					LoadValue;					//����ֵ
	//Ӧ���ֶ�			
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

//�û��޸�����ͳ�ʼ���ص�ʱʹ��			
struct TLoginBase : public TConnect
{
	char					CompanyNo[11];				//��˾���
	char					BrokerNo[11];				//���͹�˾���
	char					LoginUser[21];				//��¼�˺�
	TPswType				ChangePswType;				//�ϴ��޸ĵ���������
public:
	_InitStruct(TLoginBase);
	string Key() const
	{
		return GetKey(2, LoginUser, Sign);
	}
};

//��¼��Ϣ
//key=LoginUser+Sign		
struct TLogin : public TLoginBase
{
	char 					LoginPsw[31];
	char 					InitPsw[31];				//ǿ�Ƴ�ʼ������
	char 					CaPsw[31];					//CA����
	char					LoginClassName[51]; 		//���̨�Ľ�������

	//Ӧ���ֶ�	
	TUserIdentity 			UserType;
	char 					LoginName[21];
	union
	{
		char 				QuoteTempPsw[51];
		struct
		{
			char			GroupNo;					//Ͷ���߷���
			double			Principal;					//����
			double			PriorityFund;				//�����ʽ�
			//��ʷ����ߵ���������ӯ��
			double			HisDayProfit1;				//�vʷ����ӯ��1
			double			HisDayProfit2;				//�vʷ����ӯ��2
			double			HisDayProfit3;				//�vʷ����ӯ��3
		};	
	};
	char 					PreLoginIP[40];
	ushort 					PreLoginProt;
	char 					PreLoginTime[21];
	char 					PreLogoutTime[21];
	char 					PreLoginAddress[201];		//�ϴε�¼������Ϣ
	char 					TradeDate[11];
	char 					LastSettleTime[21];
	char 					StartTime[21];
	char 					InitTime[21];
	char 					CurrDateTime[25];			//ϵͳ��ǰʱ��(�ͻ���Уʱ)��ȷ������

	//�����ֶ�			
	TradeInitProgress		InitProgress;				//���³�ʼ������
	TBool					IsInitPsw;					//�Ƿ���Ҫǿ���޸����룬bYes:bNo, ��¼Ӧ��ʱ��ʹ�ã��޸ĳɹ�����Զ���ΪbNo
	TBool 					Logined;					//�Ƿ��ѵ�¼
	uint 					ReadyCount;					//���û��ĳ�ʼ���ɹ�����
	TradeUserRight			Rights[16];					//�û�Ȩ�ޱ�
	uint					LocalDataVersion;			//�������ݰ汾�ţ���д�����ļ�ʱʹ��
	uint					PswValidDay;				//�ͻ�������Ч����
	char					PswDate[21];				//�ͻ������޸�����
	TBool					ConfirmState;				//�Ƿ��Ѿ����й�����ȷ��
	TBool					LastQryed;					//���һ�β�ѯ�Ƿ����

	//�����ֶ�
	ushort					PortEx;						//��ѯ�˿�
	char					LinkType[21];				//���ӷ�ʽ tcp ssl ...

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
	bool PswOutDate()//�����Ƿ񳬹���Ч��
	{
		if (CurrDateTime[0] == 0 || PswDate[0] == 0 || PswValidDay == 0)
			return false;

		time_t t1 = TimeFun::StrToTime(CurrDateTime, "%Y-%m-%d");
		time_t t2 = TimeFun::StrToTime(PswDate, "%Y-%m-%d");
		//CTimeSpan ts(t1-t2);
		return 0;//ts.GetDays()>PswValidDay;
	}
	void UpdatePsw()							//�����޸ĳɹ�
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

//�������ؼ���
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

//��Ʒ�ؼ���
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

//���Ⱥ�Լ�ؼ���
//key=Sign+ExchangeNo+CommodityNo+CommodityType+ContractNo+StrikePrice+OptionType
struct TContractSingleKey : public TCommodityKey
{
	char					ContractNo[11];				//�·�
	char					StrikePrice[11];			//ִ�м�
	TOptionType				OptionType;					//����Ȩ TOptionType
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
			if (*q != '\\' && *p == '%')					//��ת���ַ�
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
		if (bword)											//��ʽ����
			result.append("%" + word);

		return result;
	}
public:
	string GetCode(const char * fmt_str = nullptr) const
	{
		if (fmt_str != nullptr && fmt_str[0] != 0)
			return FmtCode(*this, fmt_str);

		//Ĭ�ϴ���
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

//��Լ�ؼ���
//key=Sign+ExchangeNo+CommodityNo+CommodityType+ContractNo+StrikePrice+OptionType+ContractNo2+StrikePrice2+OptionType2
struct TContractKey : public TContractSingleKey
{
	char					ContractNo2[11];			//�·�2
	char					StrikePrice2[11];			//ִ�м�2
	TOptionType				OptionType2;				//����Ȩ2 TOptionType
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

		//Ĭ�ϴ���
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
				if (StrikePrice2[0])		//��Ȩ���
					return FmtCode(*this, "%c%%m1%%ot1%%p1%/%m2%%ot2%%p2%");
				else if (ContractNo2[0])	//��Ȩ�ڻ����
					return FmtCode(*this, "%c%%m1%%ot1%%p1%/%m2%");
				else						//��Ȩ
					return FmtCode(*this, "%c%%m1%%ot1%%p1%");
			}
			else
			{
				if (StrikePrice2[0])		//�ڻ���Ȩ���
					return FmtCode(*this, "%c%%m1%/%m2%%ot2%%p2%");
				else if (ContractNo2[0])	//�ڻ����
					return FmtCode(*this, "%c%%m1%/%m2%");
				else						//�ڻ�
					return FmtCode(*this, "%c%%m1%");
			}
		}
			break;
		}
	}
};

//1 �г���Ϣ
//struct TExchange : public TExchangeKey
//{
//	wchar_t					ExchangeName[21];
//	//����--------------------------------------
//	TCoverMode				CoverMode;					//ƽ�}���
//	bool					HasOption;					//�Ƿ������Ȩ��Ʒ
//public:
//	_InitStruct(TExchange);
//};

//�г�״̬
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

//Ʒ����Ϣ Key=ExchangeNo+CommodityType+CommodityNo
//struct TCommodity : public TCommodityKey
//{
//	wchar_t					CommodityName[21];
//	double					ProductDot;					//ÿ�ֳ���
//	double					UpperTick;					//��С�䶯�۷���
//	uint					LowerTick;					//��С�䶯�۷�ĸ
//	TSpreadDirect			CmbDirectFlag;				//����������
//	uint					OnlyCanCloseDays;			//��Լֻ��ƽ����ǰ����
//	char					CurrencyNo[11];				//��Ʒʹ�ñ���
//	TDeliveryMode			DeliveryMode;				//������Ȩ��ʽ 
//	uint					DeliveryDays;				//������ƫ��
//	char					AddOneTime[9];				//T+1�ָ�ʱ��,���ڴ�ʱ��ΪT+1����
//	TCoverMode				CoverMode;					//ƽ��ģʽ
//	double					ExecuteDot;					//��Ȩ��������Ȩִ��ʱ��1����Ȩ��Ӧ�ı��������ִ�м۱�����
//	uint					TimeZone;					//Ʒ��ʱ��
//	TBool					IsOnlyQuoteCommodity;		//�Ƿ�����Ʒ�� 
//
//	uint					MaxSingleOrderQty;			//��������µ���
//	uint					MaxPositionQty;				//���ֲ���
//
//	//������Ʒ1		
//	char					RelateExchangeNo[11];
//	TCommodityType			RelateCommodityType;
//	char					RelateCommodityNo[11];		//��Ʒ�ؼ���
//	//������Ʒ2		
//	char					RelateExchangeNo2[11];
//	TCommodityType			RelateCommodityType2;
//	char					RelateCommodityNo2[11];		//��Ʒ�ؼ���
//
//	//����--------------------------------
//	char					PriceFormat[6];				//�۸��ʽ���ַ���
//	char					AvgPriceFormat[6];			//���۸�ʽ���ַ���+2
//	char					SpdPriceFormat[6];			//�۲�ɽ��۸��ʽ���ַ���+1
//	char					CarryPriceFormat[6];		//Carry�۸��ʽ���ַ���+2
//	uint					Precision;					//С��λ����
//
//	//////	
//	char					GroupNo[11];				//��ߵı�֤��������
//
//	//��̨ԭʼƷ��
//	char					OriginExchangeNo[11];		//��̨ϵͳԭʼ������
//	char					OriginCommodityType;		//��̨ϵͳԭʼ������
//	char					OriginCommodityNo[11];		//��̨ϵͳԭʼ������
//	double					PriceFactor;				//��̨ϵͳ�۸񵽿ͻ��˼۸�Ļ���ϵ��
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
//		ExecuteDot = ExecuteDot == 0 ? 1 : ExecuteDot;  //��Ȩ���� ��δ����,���Ӻ�ɾ�����
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
//	bool IsSpread() const//�Ƿ����
//	{
//		switch (CommodityType)
//		{
//		case ctSpreadMonth		:	//��������
//		case ctSpreadCommodity	:	//Ʒ������
//		case ctBUL				:	//��ֱ��������
//		case ctBER				:	//��ֱ��������
//		case ctBLT				:	//ˮƽ��������	
//		case ctBRT				:	//ˮƽ��������	
//		case ctSTD				:	//��ʽ����
//		case ctSTG				:	//���ʽ����
//									return true;
//		}
//		return false;
//	}
//};

/*��Լ�����ʽ�������ַ���
%e%		������
%e1%	����������1
%e2%	����������2
%ct%	Ʒ������
%ct1%	����Ʒ������1
%ct2%	����Ʒ������2
%c%		Ʒ�ִ���
%c1%	����Ʒ�ִ���1
%c2%	����Ʒ�ִ���2
%m1%	��Լ�·ݻ�����1
%m2%	��Լ�·ݻ�����2
%p1%	ִ�м۸�1
%p2%	ִ�м۸�2
%ot1%	��Ȩ�ǵ�����1
%ot2%	��Ȩ�ǵ�����2
%oe%	��̨ϵͳԭʼ������
%oc%	��̨ϵͳԭʼƷ�ִ���
%oct%	��̨ϵͳԭʼƷ������
����:
һ��
�ڻ����� %c%%m1%
�ڻ���� %c1%%m1%/%c2%%m2%
��Ȩ���� %c%%m1%%ot1%%p1%
��Ȩ��� %c1%%m1%%ot1%%p1%/%c2%%m2%%ot2%%p2%
�ڻ���Ȩ %c1%%m1%/%c2%%m2%%ot2%%p2%
��Ȩ�ڻ� %c1%%m1%%ot1%%p1%/%c2%%m2%
����
�ڻ����� %c% %m1%
�ڻ���� %c% %m1%/%m2%
��Ȩ���� %c% %m1% %ot1%%p1%
��Ȩ��� %c% %m1% %ot1%%p1%/%m2% %ot2%%p2%
�ڻ���Ȩ %c% %m1%/%m2% %ot2%%p2%
��Ȩ�ڻ� %c %%m1% %ot1%%p1%/%m2%
ע�⣺
���Ҫ���%����Ҫ��%%����ʽת��

�����Ʒ��������Ҫ�н�����ԭʼ�������ʵ�ֺ�Լ����ͳһ���
*/

//��Լ��Ϣ
//���ӿͻ�����ʾ���룬���ǹؼ���
//�Ա�ͻ�����ʹ�õĺ�Լ��������ɶ��ƣ����ұ��ֿͻ��빫˾������һ����
//struct TContract: public TContractKey
//{			  					
//	wchar_t					ContractName[21];			//��Լ����
//	char					ExpiryDate[11];				//������
//	char					LastTradeDate[11];			//�������
//	char					FirstNoticeDate[11];		//�״�֪ͨ��	
//	char					RelateContractNo[11];		//��ĺ�Լ���·ݣ�Ĭ�Ϻ�ContractNo��ͬ, �����·ݵ���Ȩ��Ӧͬһ�·ݵı��ʱ��ContractNo��ͬ
//
//	//����----------------------------------------------------------------------
//	char					OnlyCoverDate[11];			//ֻ��ƽ����
//	int						OnlyCoverDateDx;			//����ֻ��ƽ���յ�ʱ�䣬����
//	char					DefCode[31];				//Ĭ�Ϻ�Լ����
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
//		//Ĭ�ϴ���
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
//					if (StrikePrice2[0])		//��Ȩ���
//						ret = FmtCode(*this, "%c%%m1%%ot1%%p1%/%m2%%ot2%%p2%");
//					else if (ContractNo2[0])	//��Ȩ�ڻ����
//						ret = FmtCode(*this, "%c%%m1%%ot1%%p1%/%m2%");
//					else						//��Ȩ
//						ret = FmtCode(*this, "%c%%m1%%ot1%%p1%");
//				}
//				else
//				{
//					if (StrikePrice2[0])		//�ڻ���Ȩ���
//						ret = FmtCode(*this, "%c%%m1%/%m2%%ot2%%p2%");
//					else if (ContractNo2[0])	//�ڻ����
//					{							//����Ʒ
//						if (strcmp(pCommodity->RelateCommodityNo, pCommodity->RelateCommodityNo2))
//							ret = FmtCode(*this, "%c%%m1%");		
//						else					//����
//							ret = FmtCode(*this, "%c%%m1%/%m2%");	
//					}
//					else						//�ڻ�
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

//�������������
struct TDeepQuote : public TContractKey
{
	TDirect					Direct;						//��������
	double					Price;						//�۸�
	uint					Qty;						//����
	TBool					IsLast;						//�Ƿ����һ��
public:
	_InitStruct(TDeepQuote);
};

//����������Ϣ��ѯ����
struct TOrderLinkInfoReq
{
	uint					OrderID;
	TLinkType				LinkType;					//������Ϣ����
public:
	_InitStruct(TOrderLinkInfoReq);
};

struct TOrder;
//�������			
//key=OrderID			
struct TOrderCheck
{
	uint					OrderID;
	TCheckMode				CheckMode;					//�������
	TCheckState				CheckState;					//���״̬
	char					CheckInfo[51];				//�����Ϣ
	char					CheckerNo[21];				//��˲���Ա
	char					CheckTime[21];				//���ʱ��

	const TOrder *			pOrder;
public:
	_InitStruct(TOrderCheck);
	uint Key() const
	{
		return OrderID;
	}
};

//�µ� ����sign��userno�����ҵ�loginUser������LoginUser��Sign����ȷ��������Ϣ������ͨ�������ӷ���ί��
//�Ƿ�Ҫ����ί�н���ITrans��ʵ��ȥ�жϰ�
//��������ʱ commodityType��ʾ����������һ�ȵ�Ʒ�����ͣ����������������ڲ��������ֶ���
struct TSendOrder : public TContractKey
{
	char					UserNo[21];
	TOrderType				OrderType;					//�������� 
	TOrderWay				OrderWay;					//ί����Դ 
	TValidType				ValidType;					//��Ч���� 
	char					ValidTime[21];				//��Ч����ʱ��(GTD�����ʹ��)
	TBool					IsRiskOrder;				//���ձ��� 
	TDirect					Direct;						//�������� 
	TOffset					Offset;						//����ƽ�� �� Ӧ�����뿪ƽ 
	THedge					Hedge;						//Ͷ����ֵ 
	double					OrderPrice;					//ί�м۸� �� ��ȨӦ������۸�
	double					TriggerPrice;				//�����۸�
	TTriggerMode			TriggerMode;				//����ģʽ
	TTriggerCondition		TriggerCondition;			//��������
	uint					OrderQty;					//ί������ �� ��ȨӦ������
	uint					MinMatchQty;				//��С�ɽ���	
	uint					MinOrderQty;				//��ɽ����С�����
	uint					MaxOrderQty;				//��ɽ����������

	TOffset					SellOffset;					//Ӧ��������ƽ 
	double					SellOrderPrice;				//Ӧ�������۸�
	uint					SellOrderQty;				//Ӧ������ί������ 
	THedge					SellHedge;					//Ӧ������Ͷ������
	uint					StayTime;					//Ӧ��ͣ��ʱ�䣨�룩
	char					EnquiryNo[21];				//ѯ������ţ�Ӧ��ʱ��

	TStrategyType			StrategyType;				//��������

	char					Remark[51];					//�µ���ע�ֶΣ�ֻ���µ�ʱ��Ч�������ҪΨһ��ʶһ����һ�鶨���������GUID����ʶ��������ܺ������µ�;����ID�ظ�
	TTradeSection			AddOneIsValid;				//T+1ʱ����Ч(���۽���)
	uchar					MarketLevel;				//�м۴�����,Ŀǰֻ���н���֧�֣�ȡֵΪ0��1��5
	
	//����
	uint					ParentOrderID;				//������,������TOrder::OrderIDҲ������TSpreadOrder::OrderID��
	TBool					FutureAutoCloseFlag;		//��Ȩ���ڻ��ԶԳ���
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

//�����޸�
struct TModifyOrder : public TSendOrder
{
	uint					OrderID;					//�����ڿͻ��˵ı�ţ���Ӧ���
	char					ServerFlag;					//��������ʶ
public:
	_InitStruct(TModifyOrder);
};

struct SCommodity;
//ί�лر�
//key1=orderid		
//key2=orderno+Sign		
struct TOrder : public TModifyOrder
{
	char					OrderNo[21];				//��������д��ί��Ψһ���
	char					LocalNo[21];				//���غ�
	char					SystemNo[51];
	char					ExchangeSystemNo[51];		//������ϵͳ��
	char					TradeNo[11];				//���ױ���
	
	char					UpperNo[11];				//ʹ������(��Ա��)
	char					UpperChannelNo[11];			//����ͨ����(ϯλ��)

	char					InsertNo[21];				//�µ���
	char					InsertDateTime[21];			//�µ�ʱ��
	char					UpdateNo[21];				//���һ�α����
	char					UpdateDateTime[21];			//���һ�α��ʱ��

	TOrderState				OrderState;					//ί��״̬ 

	double					MatchPrice;					//�ɽ���
	uint					MatchQty;					//�ɽ���
	uint					PreWorkingQty;				//�ϴ��Ŷ�������̬�����Ŷ�����ʱʹ��	

	double					SellMatchPrice;				//Ӧ�������ɽ���
	uint					SellMatchQty;				//Ӧ������ɽ���

	int						ErrorCode;					//������Ϣ��				
	char					ErrorText[51];				//���´�����Ϣ

	TBool					IsInput;
	TDeleteType				Deleted;					//�Ƿ���ɾ //�ɷ���ڶ���״̬��
	TBool					AddOne;						//T+1 TBool
	TOrderMark				OrderMark;					//������� omAttention�� omIgnore

	uint					StreamID;					//ί������

	char					Currency[11];				//��Ʒ����
	char					FeeCurrency[11];			//�����ѱ���
	double					FrozenFee;					//����������
	double					FrozenDeposit;				//���ᱣ֤�� �ϲ�����Ĵ���Ϊ

	//��ʷί�� ��Ҫ�Լ�����pCommodity
	char					SettleDate[11];

	//����--------------------------------------
	const SCommodity *		pCommodity;					//����Ʒ��ָ��
	const TOrderCheck *		pCheckInfo;					//���������Ϣ
public:
	_InitStruct(TOrder);
	union FP_TYPE
	{
		double				DBL;		//������һ�������ڳ�ʼ��
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
	const char CheckMode() const	//��˷�ʽ
	{
		return (pCheckInfo == nullptr) ? cmNone : pCheckInfo->CheckMode;
	};
	const char CheckState() const//���״̬
	{
		return (pCheckInfo == nullptr) ? csNone : pCheckInfo->CheckState;
	};
	const char * CheckInfo() const//�����Ϣ
	{
		return (pCheckInfo == nullptr) ? EMPTY_STR : pCheckInfo->CheckInfo;
	};
	const char * CheckerNo() const//��˲���Ա
	{
		return (pCheckInfo == nullptr) ? EMPTY_STR : pCheckInfo->CheckerNo;
	};
	const string CheckDate() const
	{
		return (pCheckInfo == nullptr) ? EMPTY_STR : string(pCheckInfo->CheckTime, 10);
	};
	const string CheckTime() const 	//���ʱ��
	{
		return (pCheckInfo == nullptr) ? EMPTY_STR : &pCheckInfo->CheckTime[10];
	};
	static int FormatPrice(const SPriceType price, const SCommodityPrecType prec, const SCommodityDenoType deno, SPriceStrType out, bool showdeno)
	{
		SCommodityPrecType tmp_prec(prec);
		if (tmp_prec > 8)
			tmp_prec = 8;

		//�жϸ�������Ч�ԣ�������С����NAN��������󣬾���Ч
		FP_TYPE fp = { price };
		unsigned short exp((fp.LL >> 52) & 0x7FF);
		if (0 != price)
			if (exp > 1100 || exp < 990)
				return 0;

		//�������λ
		size_t pos(0);
		if (fp.LL >> 63 != 0)
		{
			fp.LL &= 0x7FFFFFFFFFFFFFFF;
			if (0 != fp.LL)		//������
				out[pos++] = '-';
		}

		//����С��λ����ȥ������λ֮���� 
		if (deno <= 1)
		{
			//2016-05-19����Ϊ�������뷽ʽ
			fp.DBL += (5 * pow(0.1, tmp_prec + 1));
		}

		//������������
		unsigned int num((unsigned int)fp.DBL);
		if (deno <= 1 || 0 != num)	//��������Ϊ0�ķ����������������
		{
			_ultoa_s(num, &out[pos], sizeof(SPriceStrType) - pos, 10);
			pos = strnlen_s(out, sizeof(SPriceStrType) - 1);
		}

		//����С������(С����ʽ �� ������ʽ)
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
		else			//����
		{
			//����������������ĸ���󣬻ᵼ�·��Ӽ������
			//fp.DBL += 0.000000001;	//������ֵ
			//fp.DBL += pow(0.1, prec+1);	//������ֵ

			if (0 != num)
				out[pos++] = '+';

			fp.DBL -= (unsigned int)fp.DBL;
			fp.DBL *= deno;

			fp.DBL += 0.5;	//����������Ӳ��� 2016-06-30�޸�

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

//��Ȩ¼��
struct TExecuteInput :public TOrder
{
	char					FutureContractNo[11];				//����·�
};

//�Ŷӵ��������ؼ���ΪUserNo+SingleContractKey
//key=UserNo+Sign+ExchangeNo+CommodityNo+CommodityType+ContractNo+StrikePrice+OptionType
struct TOrderQty :public TContractSingleKey
{
	char					UserNo[21];

	//�����ֿ�ƽ��ʱ��ֻ�����������ֶ�
	uint					BuyQty;
	uint					SellQty;

	//���ֿ�ƽ��ʱ���±��ֶβ���Ч
	uint					BuyOpenQty;
	uint					BuyCoverQty;		//ƽ��
	uint					BuyCoverDayQty;		//ƽ��
	uint					SellOpenQty;
	uint					SellCoverQty;		//ƽ��
	uint					SellCoverDayQty;	//ƽ��
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

//ѯ��֪ͨ
//key=EnquiryNo+Sign
struct TEnquiry : public TContractKey
{
	char					EnquiryNo[21];				//�������
	TDirect					ReqDirect;					//������ 
	TEnquirerType			ReqBy;						//����   
	char					EnquiryDateTime[21];		//ѯ��ʱ��
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

//�ɽ��ر� �ؼ�����Ҫ����������������������Ψһ��ʶ��Ҫ�� ������+����+�ɽ���+����
//key=MatchNo+Sign
struct TMatch :public TContractSingleKey
{
	char					MatchNo[21];				//�ɽ����

	char					UserNo[21];

	TOrderWay				MatchWay;
	TDirect					Direct;
	TOffset					Offset;
	THedge					Hedge;

	uint					StreamID;
	char					ServerFlag;
	char					OrderNo[21];				//�����������
	char					ExchangeMatchNo[51];		//ԭʼ�ɽ���
	char					MatchDateTime[21];
	char					UpperMatchDateTime[21];

	double					MatchPrice;
	uint					MatchQty;

	TDeleteType				Deleted;					//�Ƿ�ɾ�� 

	char					Currency[11];				//��Ʒ����
	TBool					ManualFee;					//�ֹ������� 
	char					FeeCurrency[11];			//�����ѱ���
	double					MatchFee;					//�ɽ�������
	double					Premium;					//Ȩ������֧������Ϊ����֧��Ϊ��  = �ɽ��� * �ɽ��� * ����
	double					CoverProfit;				//ƽ��ӯ�� �� δ����ƽӮ

	char					SettleDate[11];				//��ʷ�ɽ���

	char					UpperNo[11];				//ʹ������(��Ա��)
	TBool					AddOne;						//T+1��� 
	double					CoverPrice;					//ƽ��ʱ����ƽ���ü۸�ĳֲ�
	//����------------------------------------------------------------
	const TOrder *			pOrder;
	const SCommodity *		pCommodity;					//����Ʒ��ָ��
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
//�ɽ�¼��
struct TInputMatch :public TMatch
{
	TBool					IsCarry;					//�Ƿ�carry��
	char					SystemNo[51];				//ϵͳ��		
public:
	_InitStruct(TInputMatch);
};

//���ղ���
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

//�ֱֲ�֤��
//key=PositionNo, Sign
struct TPositionDeposit
{
	TDepositVertion			Version;					//�汾��	

	char					Sign[21];
	char					PositionNo[51];				//�ֲֹؼ���

	double					PreSettle;					//ǰ�����	ver3
	double 					Deposit;					//��֤��		ver3
	double					KeepDeposit;				//ά�ֱ�֤��	ver3

	double					LastPrice;					//���¼�
	double					FloatProfit;				//��ӯ		ver9
	double					FloatProfitTBT;				//��ʸ�Ӯ trade by trade
	double					OptionMarketValue;			//��Ȩ��ֵ	ver9

	char					UpdateTime[9];				//����ʱ��
	double					UnderlyingPrice;			//��Ȩ���¼ۣ�ȡ�������Բ���
public:
	_InitStruct(TPositionDeposit);
	string Key()
	{
		return GetKey(2, PositionNo, Sign);
	}
};

//�ֲ���ϸ
//key=PositionNo+Sign
struct TPosition : public TContractSingleKey
{
	char					PositionNo[51];

	char					UserNo[21];

	TDirect					Direct;
	THedge					Hedge;						//Ͷ�� 

	uint					StreamID;
	char					ServerFlag;
	TBool					IsCmb;						//�Ƿ���ϳֲ�
	char					MatchNo[21];				//�ɽ��ؼ���
	char					MatchDateTime[21];

	double					PositionPrice;				//�۸�
	uint					PositionQty;				//�ֲܳ���	
	
	char					UpperNo[11];				//ʹ������(��Ա��)
	char					CurrencyNo[11];

	double					ProfitCalcPrice;			//��ӯ�����
	double					FloatProfit;				//��ӯ
	double					FloatProfitTBT;				//��ʸ�Ӯ trade by trade

	double					DepositCalcPrice;			//�ϲֱ�֤�����ۣ�����㣩���²�Ϊ�ɽ���
	double					Deposit;					//�ͻ���ʼ��֤��
	double					KeepDeposit;				//�ͻ�ά�ֱ�֤��			
	double					MarketValue;				//��Ȩ��ֵ = ���¼� * �ֲ��� * ����
	char					DepositUpdateTime[9];		//��֤�����ʱ��					

	uint					CanCoverQty;				//��ƽ��(��δʹ��)
	uint					ParOrderQty;				//ƽ�ֹҵ���(��δʹ��)

	char					SettleDate[11];				//��ʷ�ֲ���	
	//add--------------------------------------		
	double					OptionValue;				//��Ȩ��ʵֵ
	uint					PrePositionQty;				//�������
	double					StrikeProfit;				//��Ȩִ��ӯ�� = (������¼� - ִ�м�) * �ֲ��� * ����;
	double					Delta;
	double					Gamma;
	TPositionType			SourcePositionType;			//�ֲ���Դ�ĺϼ�����
	const SCommodity *		pCommodity;
	const TMatch *			pMatch;						//���û�гɽ���Ϣ
public:
	_InitStruct(TPosition);
	string Key() const	//��ϸ�Ĺؼ����ֺ�̨���ɣ� С�ơ��ϼƵĹؼ��������ݹ������� ͨ�õ�
	{
		return GetKey(2, PositionNo, Sign);
	}
	string SubKey() const
	{
		if (SourcePositionType == ptTotal)
			return TotalKey();

		SPriceStrType sPrice = { 0 };
		TOrder::FormatPrice(PositionPrice, Commodity(), sPrice, true);//PrecisionType()); ��������������ֲ�PrecisionType()��ƽ��ֺ��PrecisionType()��һ�£����¹ؼ��ֲ�ͬ���޷�ƽ��
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
	//���ڳֲ���ϸ
	void UpdateDeposit(TPositionDeposit & deposit)
	{		
		if (strcmp(PositionNo, deposit.PositionNo)!=0)
			return;

		strncpy_s(DepositUpdateTime, deposit.UpdateTime);

		//3.0�ı�֤��ȥ��̨�ģ�9.0���̵ı�֤��ûֵ�����ֲ̳��Դ���֤���Ҳ���仯
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
			//�����������뵽��Լ���ȣ���Ȼ�˹���Լ��ֵ��������������ܴ�
			if (SourcePositionType != ptTotal)
			{
				dFloatCalcPrc		= MathFun::SimpleRoundTo(dFloatCalcPrc		, Commodity()->CoverMode == cmUnfinish ? Precision() + 1 : Precision());
				dPositionPrc		= MathFun::SimpleRoundTo(dPositionPrc		, Commodity()->CoverMode == cmUnfinish ? Precision() + 1 : Precision());
				DepositCalcPrice	= MathFun::SimpleRoundTo(DepositCalcPrice	, Commodity()->CoverMode == cmUnfinish ? Precision() + 1 : Precision());
			}

			//��ӯ
			FloatProfitTBT			= (dFloatCalcPrc - dPositionPrc) * PositionQty * ProductDot;
			FloatProfit				= (dFloatCalcPrc - dPositionPrc) * ProductDot * (PositionQty - PrePositionQty) + (dFloatCalcPrc - DepositCalcPrice) * ProductDot * PrePositionQty;
			if (Direct == dSell)
			{
				FloatProfitTBT		= -FloatProfitTBT;
				FloatProfit			= -FloatProfit;
			}

			//��Ȩ
			if (CommodityType==ctOption)
			{
				if (deposit.Version == dvDaystar)
					Deposit			= deposit.Deposit;

				//��ֵ
				MarketValue			= dFloatCalcPrc * ProductDot * PositionQty;
				if (Direct == dSell)
					MarketValue		= -MarketValue;

				if (deposit.UnderlyingPrice == 0)
					return;
				//ִ��ӯ��
				double dStrikePrice = atof(StrikePrice);
				StrikeProfit		= (deposit.UnderlyingPrice - dStrikePrice * pCommodity->ExerciseDot) * PositionQty * ProductDot;
				if (Direct == dBuy && OptionType == otPut || Direct == dSell && OptionType == otCall)
					StrikeProfit	= StrikeProfit;
				StrikeProfit		+= (Direct == dBuy ? -1 : 1) * dPositionPrc * PositionQty * ProductDot;

				//��ʵֵ
				OptionValue			= deposit.UnderlyingPrice - dStrikePrice;
				if (OptionType == otPut)
					OptionValue		= -OptionValue;
			}
		}
	}	
	//���ڳֲֺϼơ�С��
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
			MarketValue			+= (detail->MarketValue		- old_detail->MarketValue	);		//��ֵ
			StrikeProfit		+= (detail->StrikeProfit	- old_detail->StrikeProfit	);		//ִ��ӯ��
			Delta				+= (detail->Delta			- old_detail->Delta			);
			Gamma				+= (detail->Gamma			- old_detail->Gamma			);
		}
	}
};

typedef TPosition TSubPosition;
typedef TPosition TTotalPosition;

//���к�Լ�ֲֵܳĶ�ͷ����ͷ
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

//ƽ�ֻر�
//key=LiquidateNo+Sign
struct TLiquidate : public TContractSingleKey
{
	char					LiquidateNo[21];			//ƽ�ֱ��

	char					OpenMatchNo[21];			//���ֳɽ���
	char					CoverMatchNo[21];			//ƽ�ֳɽ���

	char					UserNo[21];					//�ʽ��ʺ�

	TDirect					CoverDirect;				//���� 
	THedge					Hedge;						//Ͷ�� 
	double					OpenPrice;					//���ּ�
	double					CoverPrice;					//ƽ�ּ�
	uint					CoverQty;					//����

	uint					StreamID;
	char					OpenServerFlag;
	char					CloseServerFlag;

	char					Currency[11];				//Ʒ�ֱ���	
	double 					Premium;					//Ȩ����
	double					CloseProfit;				//ƽ��ӯ��   ����
	double					PreSettle;					//�����
	double					CloseProfitTBT;				//ƽ����ӯ�� ���ּ� - ƽ�ּ� ���
	double					UnExpProfit;				//δ����ƽӯ
	//����--------------------------
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

//�û���Ϣ
//key=UserNo+Sign		
struct TUserInfo
{
	char					Sign[21];
	char					LoginUser[21];				//��¼�˺�         //��ѯ���˻����̨���ݵ�ʱ���ҵ���½�û����Ҿ�������

	char					UserNo[21];
	char					UserName[21];
	TUserType				UserType;					//�û����ͣ����ͻ���������
	TBool					IsPermittee;				//�Ƿ��֤��
	TBool					IsMarketMaker;				//�Ƿ�������

	char					FullName[51];				//ȫ��
	char					FullEngName[51];
	char					Phone[51];					//�绰
	char					IdentigyNo[21];				//֤����
	char					Address[101];				//��ַ
	char					Remark[182];				//��ע
	char					SettleGroup[11];			//�������
	double					DepositThreshold;			//��֤����ֵ

	bool					QryMoneyed;					//�ʽ��Ƿ��Ƴɹ�
	bool					QryDeliveryed;				//�Ƿ��Ѳ����������

	const TLogin *			pLogin;						//������¼�˺�
public:
	_InitStruct(TUserInfo);
	string Key() const
	{
		return GetKey(2, UserNo, Sign);
	}
};

//�ʽ�ر�
//key=UserNo+CurrencyNo+Sign
struct TMoney
{
	char					Sign[21];
	char					UserNo[21];
	char					CurrencyGroupNo[11];		//��������
	char					CurrencyNo[11];				//���ֺ�(Currency_Base��ʾ����������ʽ�)

	double					ExchangeRate;				//���ֻ���

	TFuturesAlg				FutureAlg;					//�ڻ��㷨
	TOptionAlg				OptionAlg;					//��Ȩ�㷨

	double					PreAvailable;				//�����
	double					PreBalance;					//������
	double					PreEquity;					//��Ȩ��
	double					PreMarketEquity;			//����ֵȨ��
	double					PreUnExpProfit;				//��δ��ƽӯ
	double					PreLmeFloatExpProfit;		//��lme��ӯ

	double					Adjust;						//�ʽ����
	double					CashIn;						//���
	double					CashOut;					//����

	double					FrozenFee;					//����������20
	double					FrozenDeposit;				//���ᱣ֤��19

	double					Fee;						//������(��������������)
	double					TradeFee;					//����������17
	double					DeliveryFee;				//����������17
	double					ExchangeFee;				//���������

	double					PremiumIncome;				//Ȩ������ȡ
	double					PremiumPay;					//Ȩ����֧��
	double					CoverProfit;				//ƽӯ ����
	double					CoverProfitTBT;				//���ƽӯ
	double					DeliveryProfit;				//����ӯ��
	double					UnExpProfit;				//δ��ƽӯ
	double					ExpProfit;					//����ƽ��ӯ��

	double					FloatProfit;				//����LME�ֲ�ӯ��,���� market to market
	double					LmeFloatProfit;				//LME�ֲ�ӯ��
	double					FloatProfitTBT;				//��ʸ�Ӯ trade by trade

	double					MarketValue;				//��Ȩ��ֵ23

	double					Premium;					//Ȩ����
	double					Deposit;					//��֤��
	double					KeepDeposit;				//ά�ֱ�֤��
	double					Discount;					//LME���ֽ��15

	double					AuthMoney;					//�����ʽ�

	double					Balance;					//���ʽ�=PreBalance+Adjust+CashIn-CashOut-Fee(TradeFee+DeliveryFee+ExchangeFee)+CoverProfitTBT+Premium 
	double					Equity;						//��Ȩ��=Balance+FloatProfitTBT(NewFloatProfit+LmeFloatProfit)+UnExpiredProfit
	double					Available;					//�����=Equity-Deposit-Frozen(FrozenDeposit+FrozenFee)
	double					CanCashOut;					//�����=min(Money, Available);//Equity-Deposit-max(UnExpiredProfit)-max(NewFloatProfit+LmeFloatProfit��0)	
	double					AccoutMarketValue;			//�˻���ֵ=Equity+OptionMarketValue			
	double					OriCash;					//����ԭʼ�����24(���Զ�����ʽ�)
	double					FrozenRisk;					//���ն���
	//�����Ʒ------------------------
	double					PreFundPledgedIn;			//���������
	double					PreFundPledgedOut;			//������ʳ�
	double					FundPledgedIn;				//���������
	double					FundPledgedOut;				//������ʳ�
	double					FundCashPledged;			//������Ѻ���
	double					PledgedableFund;			//����Ѻ����
	double					PositionPledged;			//�ֵ���Ѻ
	double					SpecDeposit;				//�����Ʒ��֤��
	double					SpecFrozenDeposit;			//�����Ʒ���ᱣ֤��
	double					SpecFee;					//�����Ʒ������
	double					SpecFrozenFee;				//�����Ʒ����������
	double					SpecFloatProfit;			//�����Ʒ��ӯ
	double					SpecCoverProfit;			//�����Ʒƽӯ
	//����----------------------------
	double					RiskRate;					//������=(Deposit+Frozen(FrozenDeposit+FrozenFee))/Equity
	double					RiskRateDown;				//�����ʵ���
	double					SelfRiskRate;				//���з�����=KeepDeposit/����Ȩ��-�����ʽ�
	char					UpdateTime[11];				//����ʱ��
	double					NetProfit;					//��ӯ�� = CoverProfitTBT - Fee
	double					ProfitRate;					//ӯ���� = NetProfit / PreAvailable + CashIn - CashOut + Adjust
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

//��ʷ�����
struct TCash
{
	char					Sign[21];
	char					UserNo[21];
	TCashType				CashType;
	TCashMode				CashMode;
	char					CurrencyNo[11];
	double					OperAmount;
	char					Remark[101];

	char					UserBank[3];				//���б�ʶ
	char					UserAccount[51];			//�����˻�
	TBool					UsertAccountType;			//�Ƿ񱾱� ������˻���ʶ 
	char					CompanyBank[3];				//���б�ʶ
	char					CompanyAccount[51];			//�����˻�
	TBool					CompanyAccountType;			//�Ƿ񱾱� ������˻���ʶ 

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

//��������
//key=DeliveryNo+Sign
struct TDelivery : public TContractSingleKey
{
	char					DeliveryNo[21];

	char					OpenDate[11];				//��������
	char					DelivDate[11];				//��������
	char					UserNo[21];					//�ͻ�
	char					OpenNo[51];					//�����óɽ���ţ����� + �ɽ�����

	TOrderWay				MatchWay;					//�ɽ���Դ 
	TDirect					OpenDirect;					//���ַ��� 
	uint					DelivQty;					//��������
	uint					FrozenQty;					//��������
	double					OpenPrice;					//���ּ۸�
	double					DelivPrice;					//����۸�
	double					Fee;						//������
	double					UpperFee;					//����������
	double					DelivProfit;				//ӯ��
	double					Deposit;					//��֤��
	double					KeepDeposit;				//ά�ֱ�֤��
	double					UpperDeposit;				//���ֱ�֤��
	double					UpperKeepDeposit;			//����ά�ֱ�֤��
	char					UpperNo[11];				//����
	char					FeeCurrencyNo[11];			//�����ѱ���
	char					CurrencyNo[11];				//Ʒ�ֱ���
	TDeliveryMode			DeliveryMode;				//���ʽ 
	char					UnderlyingContractNo[11];	//��Ȩ��ĺ�Լ���·ݣ�
	//����----------------------------
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

//�˵���ѯ
struct TBillQry
{
	char					UserNo[21];
	TBillType				BillType;
	char					BillDate[11];				//���˵������˵���ʾ�ĳ��Ȳ�һ��
	TBillFormatType			FormatType;					//�˵���ʽ
public:
	_InitStruct(TBillQry);
};

//�ʵ��ر�
//key=UserNo+BillDate+Sign
struct TBill :public TBillQry
{
	char					Sign[21];
	int						ContentSize;				//���Ĵ�С
	char					Content[1];					//�ʵ�����
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
	int						ContentSize;				//���Ĵ�С
	char 					Content[1];					//�ʵ�����
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
	char					UserNo[21];					//�ڻ��ʽ��˺�
	char					BankNo[11];					//���б�־
	char					BankAccount[51];			//���п���
	char					Currency[11];				//���ֱ��
public:
	_InitStruct(TBankBase);
	string Key() const
	{
		return GetKey(5, UserNo, BankNo, BankAccount, Currency, Sign);
	}
};

//�˻�ת����Ϣ
struct TUserBank : public TBankBase
{
	char					BankName[51];				//��������
	TBool					SwapState;      			//ת�˻���״̬
	TBool					TransState;  				//ת��״̬
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

//��������ԃ
struct TBankBalanceQry : public TBankBase
{
	char					TransAuth[21];				//����ת����֤����
	char					BankPsw[21];				//��������
public:
	_InitStruct(TBankBalanceQry);
};

//��������ԃ����
struct TBankBalance : public TBankBase
{
	double					Balance;                    //��ת�ʽ�
	uint					ErrorCode;					//������
	char	 				ErrorText[51];				//������Ϣ
public:
	_InitStruct(TBankBalance);
	static uint Key()
	{
		return sdtBankBalance;
	}
};

//�ڻ���ת�ʽ��ѯ
struct TFutureBalanceQry
{
	char					Sign[21];
	char					UserNo[21];
	char					Currency[11];
public:
	_InitStruct(TFutureBalanceQry);
};

//�ڻ���ת�ʽ�Ӧ��
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

//�y���D�~Ո��
struct  TTransferReq : public TBankBalanceQry
{
	TTransDirect			TransDirect;				//ת�˷���
	double					TransFund;					//ת�˽��
public:
	_InitStruct(TTransferReq);
};

//����ת��Ӧ��
//key=FutureSerialID+Sign
struct  TTransfer : public TBankBase
{
	TTransDirect			TransDirect;				//ת�˷���
	double					TransFund;					//ת�˽��
	TTransState				TransState;					//ת��״̬

	uint					BankSerialID;				//������ˮ��
	uint					FutureSerialID;				//�ڻ���ˮ��
	char					TransTime[21];				//ת��ʱ��

	double					BankFee;					//������
	double					FutureFee;					//��˾������

	uint					ErrorCode;					//������
	char	 				ErrorText[51];				//������Ϣ

	TTransInitiator 		Initiator;					//ת�˷���
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

//��֤�������ĵ�¼����
struct TCfmmcToken
{
	char					Sign[21];

	char					CompanyNo[11];				//�ڻ���˾����(��Ա��)
	char					UserNo[21];					//�ʽ��˺�
	uint					TokenID;					//���ƣɣ�
	char					Token[21];					//������֤��

	uint					ErrorCode;					//������
	char	 				ErrorText[51];				//������Ϣ
public:
	_InitStruct(TCfmmcToken);
	static uint Key()
	{
		return sdtCfmmcToken;
	}
};

//�ͻ���ϵ��
//key=UserNo+UserProxyNo+UserProxyType+Sign
struct TUserProxy
{
	char					Sign[21];
	char					UserNo[21];						//�����ͻ�
	char					UserProxyNo[21];				//��ϵ��
	char					UserProxyName[51];				//��ϵ������
	TUserProxyType			UserProxyType;					//��ϵ������
	char					IdentityType[21];				//֤������
	char					IdentityId[51];					//֤��ID
	char					IdExpireDate[11];				//֤��������
	char					Cert[21];						//��֤��
	char					CertExpireDate[11];				//CA������
	char					Country[11];					//��ַ�Ĺ���
	char					Province[21];					//��ַ��ʡ/������/ֱϽ��
	char					City[51];						//��ַ����/��/��
	char					Address[101];					//��ַ
	char					PostNo[21];						//��������
	char					TelPhoneNo[101];				//�绰
	char					Birthday[11];					//����
		
	//�޶�8
	char					CertificatesIssuedAddress[41];	//֤�����е�
	TFATCAState				FATCAStatus;					//FATCA״̬
	char					Nationality[41];				//����
	char					CountryOfResidence[41];			//��ס��
	char					IsUSBeneficiary;				//�Ƿ�������������
	char					IsUSTaxPayers;					//�Ƿ���������˰����
	char					TINTaxNo[33];					//TIN˰���
	char					IsWaiver;						//�Ƿ�ȡ��Waiver
	TTaxType				TaxFormType;					//˰������
	char					SignedTaxFromDate[11];			//ǩ��˰������
	char					MarginStandard[11];				//��֤��ˮƽ
	TTradePlatFrom			TradePlatFrom;					//����ƽ̨
	char					Fax[101];						//����
public:
	_InitStruct(TUserProxy);
	string Key() const
	{
		return GetKey(3, UserNo, UserProxyNo, Sign) + KEY_SPLIT + UserProxyType;
	}
};

//������Ϣ
//key=CurrencyNo+Sign
struct TCurrency
{
	char					Sign[21];

	char					CurrencyNo[11];
	char					CurrencyName[21];
	char					CurrencyGroupNo[11];		//�������� 
	TBool					IsShare;					//�Ƿ����������ֹ������ 
	double					ExchangeRate;				//���׻���
public:
	_InitStruct(TCurrency);
	string Key() const
	{
		return GetKey(2, CurrencyNo, Sign);
	}
};

//���µ�����
struct TTradableQty : public TSendOrder
{
	uint					OpenQty;					//�ɿ�
	uint					CoverQty;					//��ƽ

	int						MaxOrdID;					//ԭʼ������	�ж��Ƿ������µ�
	double					Available;					//ԭʼ�����ʽ�	�жϿ����ʽ��Ƿ����仯
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

//�µ������ʽ�
struct TPreFreeze : public TSendOrder
{
	double					Deposit;					//���ұ�֤��

	int						MaxOrdID;					//ԭʼ������	�ж��Ƿ������µ�
	double					Available;					//ԭʼ�����ʽ�	�жϿ����ʽ��Ƿ����仯
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

//����
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

//��Ϣ
//Key2=MsgID+Sign
struct TMessage
{
	char					Sign[21];
	uint					MsgID;
	char					UserNo[21];					//������
	TMessageType			Type;						//��Ϣ����  ��������
	TMessageLevel			Level;						//��Ϣ����
	char					SendNo[11];					//������
	char					SendDateTime[21];			//����ʱ��
	char					ValidDateTime[21];			//��Чʱ��
	char					Title[101];					//����
	uint					ContentSize;
	char					Content[1];					//����
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

//3M������,key=Sign
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

//��ʷ���ײ�ѯ: ��ʷί�С���ʷ�ɽ�����ʷ�������ʷ�ʽ����
struct THisTradeQry
{
	char					UserNo[21];
	char					BeginDate[11];
	char					EndDate[11];
public:
	_InitStruct(THisTradeQry);
};

//������Ϣ
struct TErrorInfo
{
	char					Sign[21];					
	char					LoginUser[21];				
	char					module[21];
	TradeNotifyType			ErrorSource;	//������Դ
	int						error_id;
	char					error_text[51];	//������Ϣ
public:
	_InitStruct(TErrorInfo);
};


//�ʽ��˺Ŷ�Ӧ���������ѯ�ṹ
struct	TFundCalcParamReq : public TContractSingleKey
{
	char					UserNo[21];					//�ʽ��˻�
public:
	string Key()
	{
		return GetKey(2, UserNo, TContractSingleKey::conSingleKey().c_str());
	}
	_InitStruct(TFundCalcParamReq);
};

//�ʽ��˺Ŷ�Ӧ�������Ӧ��ṹ
struct	TFundCalcParam : public TFundCalcParamReq
{
	char					CurrencyNo[11];				//���ֱ��

	double					OpenFeeRatio;				//���������ѱ���
	double					CoverFeeRatio;				//ƽ�������ѱ���
	double					CoverTodayFeeRatio;			//ƽ�������ѱ���
	double					OpenFeeAmount;				//��������������
	double					CoverFeeAmount;				//ƽ������������
	double					CoverTodayFeeAmount;		//ƽ������������
	
	double					StrikeFeeRatio;				//��Ȩ�����ѱ���	
	double					StrikeFeeAmount;			//��Ȩ����������

	double                  SLongDepositRatio;			//��ͷͶ����֤�����
	double                  SLongDepositAmount;			//��ͷͶ����֤������
	double                  SShortDepositRatio;			//��ͷͶ����֤�����
	double                  SShortDepositAmount;		//��ͷͶ����֤������
	double                  HLongDepositRatio;			//��ͷ�ױ���֤�����
	double                  HLongDepositAmount;			//��ͷ�ױ���֤������
	double                  HShortDepositRatio;			//��ͷ�ױ���֤�����
	double                  HShortDepositAmount;		//��ͷ�ױ���֤������
	double                  ALongDepositRatio;			//��ͷ������֤�����
	double                  ALongDepositAmount;			//��ͷ������֤������
	double                  AShortDepositRatio;			//��ͷ������֤�����
	double                  AShortDepositAmount;		//��ͷ������֤������
	double                  MLongDepositRatio;			//��ͷ���б�֤�����
	double                  MLongDepositAmount;			//��ͷ���б�֤������
	double                  MShortDepositRatio;			//��ͷ���б�֤�����
	double                  MShortDepositAmount;		//��ͷ���б�֤������

	const TCurrency *		pCurrency;
public:
	_InitStruct(TFundCalcParam);
	string Key()
	{
		return TFundCalcParamReq::Key();
	}
};

//��Ʒ��֤�𣬵�������
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


//ί��ƫ�뷶Χ
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