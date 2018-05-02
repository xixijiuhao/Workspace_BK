#pragma once

#ifdef STRUCT_FUNCTION
//trade_struct.h�����¶�������������нṹ�壬�������ƺ��ڴ沼�ֶ��������һ����ֻ��Ϊ�˵��÷��������˳�Ա����
#include "trade_struct.h"
#else

#include "TradeApi_Type.h"


#pragma pack(push, 1)


//���׺�̨������Ϣ
struct TRawTradePlugin
{
	char					PluginNo[21];
	char					Version[21];
	char					PluginName[31];
};

//key=Sing
struct TAddress
{
	char					Sign[21];					//��ʾ��������ʾ����ip��portΪ����, �ڶ��˻����̨�����ʹ��						
	char 					IP[40];						//����һ�㣬�Ա����������ַ��ʽ
	ushort					Port;
	TServerType				ServerType;					//��¼�ķ���������
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
};

//�û��޸�����ͳ�ʼ���ص�ʱʹ��			
struct TLoginBase : public TConnect
{
	char					CompanyNo[11];				//��˾���
	char					BrokerNo[11];				//���͹�˾���
	char					LoginUser[21];				//��¼�˺�
	TPswType				ChangePswType;				//�ϴ��޸ĵ���������
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
};

//�������ؼ���
//key=Sign+ExchangeNo
struct TExchangeKey
{
	char					Sign[21];
	char					ExchangeNo[11];
};

//��Ʒ�ؼ���
//key=Sign+ExchangeNo+CommodityNo+CommodityType
struct TCommodityKey : public TExchangeKey
{
	TCommodityType			CommodityType;
	char					CommodityNo[11];
};

//���Ⱥ�Լ�ؼ���
//key=Sign+ExchangeNo+CommodityNo+CommodityType+ContractNo+StrikePrice+OptionType
struct TContractSingleKey : public TCommodityKey
{
	char					ContractNo[11];				//�·�
	char					StrikePrice[11];			//ִ�м�
	TOptionType				OptionType;					//����Ȩ TOptionType
};

//��Լ�ؼ���
//key=Sign+ExchangeNo+CommodityNo+CommodityType+ContractNo+StrikePrice+OptionType+ContractNo2+StrikePrice2+OptionType2
struct TContractKey : public TContractSingleKey
{
	char					ContractNo2[11];			//�·�2
	char					StrikePrice2[11];			//ִ�м�2
	TOptionType				OptionType2;				//����Ȩ2 TOptionType
};

//�г�״̬
//key=ExchangeNo+CommodityNo
struct TExchangeState : public TExchangeKey
{
	TCommodityType			CommodityType;
	char					CommodityNo[10];
	char					ExchangeDateTime[21];
	TTradeState				TradeState;
};

//�������������
struct TDeepQuote : public TContractKey
{
	TDirect					Direct;						//��������
	double					Price;						//�۸�
	uint					Qty;						//����
	TBool					IsLast;						//�Ƿ����һ��
};

//����������Ϣ��ѯ����
struct TOrderLinkInfoReq
{
	uint					OrderID;
	TLinkType				LinkType;					//������Ϣ����
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
};

//�����޸�
struct TModifyOrder : public TSendOrder
{
	uint					OrderID;					//�����ڿͻ��˵ı�ţ���Ӧ���
	char					ServerFlag;					//��������ʶ
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
	uint					BuyCoverQty;
	uint					BuyCoverDayQty;
	uint					SellOpenQty;
	uint					SellCoverQty;
	uint					SellCoverDayQty;
};

//ѯ��֪ͨ
//key=EnquiryNo+Sign
struct TEnquiry : public TContractKey
{
	char					EnquiryNo[21];				//�������
	TDirect					ReqDirect;					//������ 
	TEnquirerType			ReqBy;						//����   
	char					EnquiryDateTime[21];		//ѯ��ʱ��
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
};
//�ɽ�¼��
struct TInputMatch :public TMatch
{
	TBool					IsCarry;					//�Ƿ�carry��
	char					SystemNo[51];				//ϵͳ��		
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
	double					UnderlyingPrice;			//������¼ۣ�ȡ�������Բ���
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
	const SCommodity *		pCommodity;					//����Ʒ��ָ��
	const TMatch *			pMatch;						//���û�гɽ���Ϣ
};

typedef TPosition TSubPosition;
//typedef TPosition TTotalPosition;
struct TTotalPosition:TPosition{};

//���к�Լ�ֲֵܳĶ�ͷ����ͷ
struct TAllPosition
{
	uint					Long;
	uint					Short;
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
	char					UserEngName[51];
	char					Phone[51];					//�绰
	char					IdentigyNo[21];				//֤����
	char					Address[101];				//��ַ
	char					Remark[182];				//��ע
	char					SettleGroup[11];			//�������
	double					DepositThreshold;			//��֤����ֵ

	bool					QryMoneyed;					//�ʽ��Ƿ��Ƴɹ�
	bool					QryDeliveryed;				//�Ƿ��Ѳ����������

	const TLogin *			pLogin;						//������¼�˺�
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

	double					Balance;					//���ʽ�
	double					Equity;						//��Ȩ��
	double					Available;					//�����
	double					CanCashOut;					//�����	
	double					AccoutMarketValue;			//�˻���ֵ			
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
	double					RiskRate;					//������
	double					RiskRateDown;				//�����ʵ���
	double					SelfRiskRate;				//���з�����
	char					UpdateTime[11];				//����ʱ��
	double					NetProfit;					//��ӯ��
	double					ProfitRate;					//ӯ����
	const TUserInfo *		pUser;
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
	const SCommodity *		pCommodity;					//����Ʒ��ָ��
};

//�˵���ѯ
struct TBillQry
{
	char					UserNo[21];
	TBillType				BillType;
	char					BillDate[11];				//���˵������˵���ʾ�ĳ��Ȳ�һ��
	TBillFormatType			FormatType;					//�˵���ʽ
};

//�ʵ��ر�
//key=UserNo+BillDate+Sign
struct TBill :public TBillQry
{
	char					Sign[21];
	int						ContentSize;				//���Ĵ�С
	char					Content[1];					//�ʵ�����
};

//key=LoginUser+Sign
struct TAgreementInfo
{
	char					Sign[21];
	char					LoginUser[21];
	int						ContentSize;				//���Ĵ�С
	char 					Content[1];					//�ʵ�����
};

//key=UserNo+BankNo+BankAccount+Currency+Sign
struct TBankBase
{
	char					Sign[21];
	char					UserNo[21];					//�ڻ��ʽ��˺�
	char					BankNo[11];					//���б�־
	char					BankAccount[51];			//���п���
	char					Currency[11];				//���ֱ��
};

//�˻�ת����Ϣ
struct TUserBank : public TBankBase
{
	char					BankName[51];				//��������
	TBool					SwapState;      			//ת�˻���״̬
	TBool					TransState;  				//ת��״̬
};

//��������ԃ
struct TBankBalanceQry : public TBankBase
{
	char					TransAuth[21];				//����ת����֤����
	char					BankPsw[21];				//��������
};

//��������ԃ����
struct TBankBalance : public TBankBase
{
	double					Balance;                    //��ת�ʽ�
	uint					ErrorCode;					//������
	char	 				ErrorText[51];				//������Ϣ
};

//�ڻ���ת�ʽ��ѯ
struct TFutureBalanceQry
{
	char					Sign[21];
	char					UserNo[21];
	char					Currency[11];
};

//�ڻ���ת�ʽ�Ӧ��
struct TFutureBalance : public TFutureBalanceQry
{
	double					Balance;
};

//�y���D�~Ո��
struct  TTransferReq : public TBankBalanceQry
{
	TTransDirect			TransDirect;				//ת�˷���
	double					TransFund;					//ת�˽��
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
};

//���µ�����
struct TTradableQty : public TSendOrder
{
	uint					OpenQty;					//�ɿ�
	uint					CoverQty;					//��ƽ

	int						MaxOrdID;					//ԭʼ������	�ж��Ƿ������µ�
	double					Available;					//ԭʼ�����ʽ�	�жϿ����ʽ��Ƿ����仯
};

struct TTradableQtySell : public TTradableQty
{
};

//�µ������ʽ�
struct TPreFreeze : public TSendOrder
{
	double					Deposit;					//���ұ�֤��

	int						MaxOrdID;					//ԭʼ������	�ж��Ƿ������µ�
	double					Available;					//ԭʼ�����ʽ�	�жϿ����ʽ��Ƿ����仯
	//add	
	const TCurrency *		pCurrency;
};

struct TPreFreezeSell : public TPreFreeze
{
};

//����
//key=UpperNo+Sign
struct TUpper
{
	char					Sign[21];

	char					UpperNo[11];
	char					UpperName[21];
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
};

//3M������,key=Sign
struct T3MEndDate
{
	char					Sign[21];
	char					ContractNo[11];
};

//��ʷ���ײ�ѯ: ��ʷί�С���ʷ�ɽ�����ʷ�������ʷ�ʽ����
struct THisTradeQry
{
	char					UserNo[21];
	char					BeginDate[11];
	char					EndDate[11];
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
};

//ί��ƫ�뷶Χ
struct TOrderPriceRange: public TCommodityKey
{	
	TRangeType				RangeType;	
	double					Range;
	TActionType				ActionType;
};

#pragma pack(pop)


#endif