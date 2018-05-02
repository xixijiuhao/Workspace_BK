#ifndef _TRADEAPI_TYPE_
#define _TRADEAPI_TYPE_

typedef signed long long		int64;
typedef unsigned char			uchar;
typedef unsigned short			ushort;
typedef unsigned long int		uint;
typedef unsigned long long		uint64;


#pragma pack(push, 1)


//���׻ص����� CurrMaxValue = 58
enum TradeNotifyType
{
	tntNone					= 0,

	tntConnect				= 2,		//����
	tntDisconnect			= 3,		//�Ͽ�
	tntAuthenticate			= 56,		//��֤
	tntLogin				= 5,		//��¼��Ϣ 
	tntLoginInfo			= 6,		//��¼��Ϣ���� 
	tntLogout				= 7,		//�ǳ�
	tntIniting				= 8,		//��ʼ����

	tntAgreementInfo 		= 9,		//Э������
	tntConfirmState			= 10,		//Э��ȷ��״̬

	tntUser					= 11,		//�ͻ���Ϣ
	tntRight				= 12,		//Ȩ����Ϣ

	tntBank					= 13,		//�û�������Ϣ
	tntBankBalance			= 14,		//�������
	tntFutureBalance		= 55,
	tntTransfer				= 15,		//ת����ˮ

	tntCfmmcToken			= 16,		//��֤������������

	tntUpper				= 17,		//������Ϣ
	tntCurrency				= 18,		//������Ϣ
	tntExchange				= 19,		//�г���Ϣ
	tntCommodity			= 20,		//Ʒ����Ϣ
	tntContract				= 21,		//��Լ��Ϣ
	tntCommodityOrderType	= 22,		//Ʒ�ֶ���������Ϣ
	tntCommodityValidType	= 23,		//Ʒ����Ч������Ϣ
	tnt3MExpDate			= 24,		//3M������

	tntDeepQuote			= 57,		//�������

	tntBill					= 25,		//�ʵ���Ϣ
	tntBillConfirm			= 26,		//�˵�ȷ��
	tntUserProxy			= 27,		//��ϵ��
	
	tntOrder				= 28,		//������Ϣ
	tntMatch				= 29,		//�ɽ���Ϣ
	tntPosition				= 31,		//�ֲ���ϸ
	tntPositionSub			= 32,		//�ֲ�С��
	tntPositionTotal		= 33,		//�ֲֺϼ�
	tntLiquidate			= 34,		//ƽ����Ϣ
	tntMoney				= 35,		//�ʽ���Ϣ
	tntMsg					= 36,		//��̨��Ϣ	
	tntEnquiry				= 37,		//ѯ��֪ͨ
	tntPositionDeposit		= 38,		//�ֱֲ�֤��

	tntTradableQty			= 39,		//���µ�����
	tntPreFreeze			= 40,		//�µ�Ԥ�����ʽ�
	tntChangePsw			= 41,		//�޸�����
	tntCheckUserProxy		= 42,		//��ϵ����֤

	tntPswValidDay			= 43,		//������Ч����
	tntPswDate				= 44,		//������Ч��

	tntHisOrder				= 45,		//��ʷί��
	tntHisOrderProc			= 46,		//ί������
	tntHisMatch				= 47,		//��ʷ�ɽ�
	tntHisPosition			= 48,		//��ʷ�ֲ�
	tntHisDelivery			= 49,		//��ʷ����
	tntHisCash				= 50,		//��ʷ����� ��ʷ�ʽ����

	tntOrderCheck 			= 51,		//���������Ϣ
	tntErrorInfo			= 52,		//������Ϣ�ر�

	tntRefAllData			= 53,		//ˢ����������, ��ʷ����(45~50)��Ҫ���¼��أ�ʵʱ����ֻ��ˢ�½���

	tntInventory			= 54,		//�ֻ����
	tntExchangeState		= 58,		//������״̬
	tntCalcParam			= 59,		//�������
	tntOrderPriceRange		= 60,		//ί��ƫ�뷶Χ
	
	tntPositionCommodity	= 61,		//Ʒ�ֲֳ�
	tntMatchSub				= 62,		//�ɽ���ϢС��
	tntMatchTotal			= 63		//�ɽ���Ϣ�ϼ�
};

//���ݳ�ʼ�����ȱ���
enum TradeInitProgress
{
	tipNone 					= 0,	
	
	tipAgreementSign			= 1,			//ȷ��״̬
	tipAgreementInfoQry 		= 2,			//ȷ����Ϣ��ѯ
	tipAgreementInfoReady		= 3,			//ȷ����Ϣ��ѯӦ�����
	tipSignAgreement			= 4,			//ʼ����Ϣ
	tipPswValidDay				= 5,			//������Ч����
	tipPswDate					= 6,			//�����޸�����
	tipUser						= 7,			//�ͻ���Ϣ
	tipRight					= 8,			//Ȩ����Ϣ
	tipBank						= 9,			//�û�������Ϣ
	tipCurrency					= 10,			//������Ϣ
	tipBillConfirm				= 11,			//���㵥ȷ��
	tipUpper					= 12,			//������Ϣ
	tipExchange					= 13,			//�г���Ϣ
	tipCommodity				= 14,			//Ʒ����Ϣ
	tipContract					= 15,			//��Լ��Ϣ
	tipCommodityOrderType		= 16,			//Ʒ�ֶ���������Ϣ
	tipCommodityValidType		= 17,			//Ʒ����Ч������Ϣ
	tip3MExpDate				= 18,			//3M������
	tipOrder					= 19,			//ί����Ϣ
	tipMatch					= 20,			//�ɽ���Ϣ
	tipPosition					= 21,			//�ֲ���Ϣ
	tipLiquidate				= 22,			//ƽ����Ϣ
	tipMoney					= 23,			//�ʽ���Ϣ
	tipMsg						= 24,			//��Ϣ��ѯ
	tipLocalData				= 25,			//��������
	tipReady					= 26,			//�������ݳ�ʼ�����
	tipDepositParam				= 27,			//��֤�����
	tipFeeParam					= 28,			//�����Ѳ���
	tipOrderPriceRange			= 29			//ί��ƫ�뷶Χ
};	

//�ͻ�Ȩ��			  		
enum TradeUserRight								
{						
	turNone						= 0,
	turUnDefine					= 1,			//δ�����Ȩ��
	turShfeDeepQuote			= 2,			//�������������

	//���ͻ�Ȩ��			
	turQuickOrder				= 60001,		//�����µ�
	turIcebergOrder				= 60002,		//��ɽ��
	turGhostOrder				= 60003,		//���鵥

	//����ԱȨ��			
	turEOrderUpdate				= 30001,		//���ӵ��޸ġ�ɾ��
	turOrderMove				= 30002,		//����ת�ơ��ɽ�ת��
	turSystemRemoveOrder		= 30003,		//ϵͳɾ��
	turOrderInput				= 30004,		//����¼��
	turOrderCheck				= 30005			//�������
};

//�ڲ�������붨��
enum TradeErrorCode
{
	tecUnHoldAction				= 0xffff		//��֧�ָò���
};	
	
//���ݹ�������	
enum TFilterType	
{	
	ftNone						= 0,			//������
	ftValid						= 1,			//��Ч���ݣ���������ɾ����ת�Ƶ�����= , Ҳ��������ʷ������
	ftCanShow					= 2				//��Ҫ��ʾ�����ݣ��ɽ�����GTC��ʷ�ɽ�
};

//�սṹ��
struct TNullStruct{};	

#pragma pack(pop)



//����������
class IDataContainer
{
public:
	virtual uint __cdecl size() = 0;
	virtual const void * __cdecl at(uint i) = 0;
	virtual void __cdecl clear() = 0;
	virtual void __cdecl push_back(const void * data) = 0;			
};


#define EMPTY_STR				""								//���ַ���
#define Currency_Base			"Base"							//���ҵı��ִ��룬�ö����������ˣ����ܺ��������б����ظ�
#define Currency_RMB			"CNY"							//����ұ���
#define Currency_Pledge			"Pledge"						//��Ѻ����
#define _3M						"3M"							//3M��ԼĬ�ϴ���
#define INVAID_FLOAT			(-1e+100)						//��Ч�ĸ�����
#define FLOAT_SAME_ERRORDIFF	(1e-10)							//������������
#define KEY_LENGTH				(100)							//�ؼ��ֿռ䳤��
#define KEY_SPLIT				"^"								//�ؼ����е��ֶμ������
#define LocalVersion			(1)								//�������ݰ汾��
#define IsValidFloat(v)			((v) > INVAID_FLOAT && (v) < -INVAID_FLOAT)			//�Ƿ���Ч������
#define SpreadCommodityFormat	"%s&%s"							//��Ʒ����������Ʒ��ʽ


//���̵ļ��ҽ�����
#define exNone					"NONE"							//��
#define exZCE					"ZCE"							//֣����
#define exSHFE					"SHFE"							//������
#define exCFFEX					"CFFEX"							//�н���
#define exDCE					"DCE"							//������
#define exSZSE					"SZSE"							//���
#define exSSE					"SSE"							//�Ͻ���
#define exSGE					"SGE"							//����
#define exINE					"INE"							//�ܽ���



/**************************************************************************/
typedef char TBool;
#define	bYes					'Y'	//��
#define	bNo						'N'	//��

typedef char TServerType;
#define svtNone					'N' 
#define svtTradeServer			't' //���׷�����
#define svtFrontServer			'f' //ǰ�û�����
#define svtNameServer			'n' //ǰ����������

//��Լ����
typedef char TCommodityType;
#define ctUnDefine				'U' //δ����
#define ctNone				    'N'	//��Ч

#define	ctGoods					'G'	//�ֻ�
#define ctDefer					'Y' //�ֻ�����

#define	ctFutures				'F'	//�ڻ�
#define ctSpreadMonth			'm'	//��������
#define ctSpreadCommodity		'c'	//Ʒ������
#define ctSpreadLocal		    'L'	//��������
#define ctIndexNumber			'Z'	//ָ��
#define	ctOption				'O'	//��Ȩ
#define	ctBUL					'u'	//��ֱ��������
#define	ctBER					'e'	//��ֱ��������
#define	ctBLT					'l'	//ˮƽ��������	
#define ctBRT					'r'	//ˮƽ��������	
#define ctSTD					'd'	//��ʽ����
#define ctSTG					'g'	//���ʽ����
#define ctPRT					'P' //�������

#define ctDirect				'D'	//���ֱ�ӻ���
#define ctInDirect				'I' //����ӻ���
#define ctCross					'C' //��㽻�����

#define ctStocks				'T'	//��Ʊ
#define ctStatics				'0'	//ͳ��


//��Ȩ����
typedef char TOptionType;
#define otNone					'N' //��
#define otCall					'C' //����
#define otPut					'P'	//����

//���򻯲�������
typedef char TProgramActionType;
#define atBuy					(0)	//����
#define atBuyToCover			(1)	//����ƽ��
#define atSellShort				(2)	//��������
#define atSell					(3)	//����ƽ��

//��Լ����
typedef char TContractType;
#define ctTradeQuote			'1' //���������Լ
#define ctQuote					'2' //�����Լ
//��Լ����
#define ctTrade					'1' //���׺�Լ
#define ctQuote					'2' //�����Լ
#define ctLocal                 '3' //���غ�Լ

//������״̬
typedef char TTradeState;
#define tsUnknown				'N' //δ֪״̬
#define tsIniting				'I' //����ʼ��
#define tsReady					'R' //׼������
#define tsSwitchDay             '0' //�������л�
#define tsBiding                '1' //�����걨
#define tsMakeMatch             '2' //���۴��
#define tsTradeing				'3' //��������
#define tsPause					'4' //������ͣ
#define tsClosed				'5' //���ױ���
#define tsBidPause				'6' //������ͣ
#define tsGatewayDisconnect		'7' //����δ��
#define tsTradeDisconnect		'8' //����δ��
#define tsCloseDeal				'9' //���д���


//�۲������ؼ���Լ 
typedef char TSpreadDirect;
#define dfNone					'0' //��
#define dfFirst					'1' //�Ե�һ��Ϊ׼
#define dfSecond				'2' //�Եڶ���Ϊ׼

//������������
#define PRD_DYNA				'0' //��ʱ
#define PRD_TICK				'1' //��ϸ
#define PRD_SECOND				'2' //��
#define PRD_MIN					'3' //����
#define PRD_DAY					'5' //��
#define PRD_WEEK				'6' //��
#define PRD_MONTH				'7' //��
#define PRD_YEAR				'8' //��

//��½����
typedef char TLoginType;
#define ltPtl					'P' //Э���½
#define ltApi					'A' //API��½
#define ltMob					'M'	//�ֻ���½

//��������
typedef char TOrderType;
#define otUnDefine				'U' //δ����
#define	otMarket				'1'	//�м۵�
#define	otLimit					'2'	//�޼۵�
#define	otMarketStop			'3'	//�м�ֹ��
#define	otLimitStop				'4'	//�޼�ֹ��
#define otExecute				'5' //��Ȩ
#define otAbandon				'6' //��Ȩ
#define otEnquiry				'7' //ѯ��
#define otOffer					'8' //Ӧ��
#define otIceberg				'9' //��ɽ��
#define otGhost					'A' //Ӱ�ӵ�
#define otSwap					'B' //����
#define otSpreadApply			'C' //��������
#define otHedgApply				'D' //�ױ�����
#define otOptionAutoClose		'F' //��Ȩǰ��Ȩ�ԶԳ�����
#define otFutureAutoClose		'G' //��Լ�ڻ��ԶԳ�����
#define otMarketOptionKeep		'H' //����������

//��������
typedef char TStrategyType;
#define stNone					'N' //��
#define stPreOrder				'P' //Ԥ����(��)
#define stAutoOrder				'A' //�Զ���
#define stCondition				'C' //������
//�����ֶ������ʹ�ÿ��Դﵽ2.0ԤԼ����Ч��
#define stAttach				'a' //���Ӷ���
#define stOCO					'o' //����һ

//��Ч����
typedef char TValidType;
#define vtNone					'N' //��
#define	vtFOK					'4'	//��ʱȫ��
#define	vtIOC					'3'	//��ʱ����
#define vtGFS					'5' //������Ч
#define	vtGFD					'0'	//������Ч
#define	vtGTC					'1'	//������Ч
#define	vtGTD					'2'	//������Ч
//#define	vtNDF					'3'	//�����׽���Ч(HK)

//ί����Դ
typedef char TOrderWay;
#define owUndefine				'U' //δ����
#define	owAll					'A'	//����
#define	owETrade				'E'	//E-Trade
#define	owProxyETrade			'P'	//����
#define	owJTrade				'J'	//J-Trade
#define	owManual				'M'	//�˹���
#define	owCarry					'C'	//Carry��
#define	owDelivery				'D'	//������Ȩ
#define	owProgram				'S' //��ʽ��	
#define owExecute				'e' //��Ȩ
#define owAbandon				'a' //��Ȩ
#define owChannel				'c' //ͨ����
#define owRTS					'R' //RTS
#define owAPI					'I' //API

#define owSpreadSyn				'Y' //ͬ������
#define owSpreadSequ			'u' //�Ⱥ�����
#define owSpreadButterfly		'T' //��ʽ����
#define owLocalPrepare			'O'	//������
#define	owHorz					'H'	//�����µ�
#define	owVert					'V'	//�����µ�
#define	owClickPrice			'L'	//����µ�
#define	owCard					'r' //��Ƭ�µ�	
#define owBrtch					'B' //�����µ�
#define owFast					'F' //�����µ�
#define owPingPong				'p' //ƹ��
#define	owStopLoss				's' //ֹ��
#define	owStopProfit			'o' //ֹӯ
#define owFollow				'f' //����
#define owCatch					't' //׷��
#define owClear					'l' //һ�����
#define owDrawLine				'd' //�����µ�
#define owMouseCove				'm' //���ƽ��
#define owQuoteOrder			'q' //�����µ�

//���
typedef char TOrderMark;
#define omNone					'N'
#define omAttention				'A' //��ע
#define omIgnore				'I' //����
#define omPass					'Y' //���ͨ��
#define omNopass				'F' //���δͨ��

//����
typedef char TDirect;
#define dNone					'N'
#define	dBuy					'B'	//����
#define	dSell					'S'	//����
#define	dBoth					'A'	//˫��

//��ƽ
typedef char TOffset;
#define	oNone					'N'	//��
#define	oOpen					'O'	//����
#define	oCover					'C'	//ƽ��
#define	oCoverT					'T'	//ƽ��
#define oOpenCover				'1' //��ƽ��Ӧ��ʱ��Ч, ��������Ҳ����
#define oCoverOpen				'2'	//ƽ����Ӧ��ʱ��Ч, ��������Ҳ����

//Ͷ�����
typedef char THedge;
#define	hNone					'N'	//��
#define	hSpeculate				'T'	//Ͷ��
#define	hHedge					'B'	//�ױ�
#define hSpread					'S' //����
#define hMarket					'M' //����

//����ʱ��
typedef char TTradeSection;
#define tsDay					'D' //���콻��ʱ��
#define tsNight					'N' //���ϣ�T+1������ʱ��
#define tsAll					'A' //ȫ����ʱ��

//����״̬
typedef char TOrderState;
#define osNone					'N' //��
#define	osSended				'0'	//�ѷ���
#define	osAccept				'1'	//������
#define osTriggering			'2'	//������
#define	osActive				'3'	//����Ч
#define	osQueued				'4'	//���Ŷ�
#define	osPartFilled			'5'	//���ֳɽ�
#define	osFilled				'6'	//��ȫ�ɽ�
#define	osCanceling				'7'	//����
#define	osModifying				'8'	//����
#define	osCanceled				'9'	//�ѳ���
#define	osPartCanceled			'A'	//�ѳ��൥
#define	osFail					'B'	//ָ��ʧ��
#define osChecking				'C' //�����
#define	osSuspended				'D'	//�ѹ���
#define	osApply					'E'	//������
#define osInvalid				'F' //��Ч��
#define osPartTriggered			'G' //���ִ���
#define osFillTriggered			'H' //��ȫ����
#define osPartFailed			'I' //�൥ʧ��


//������������״̬
#define osPaired				'J'//�����
#define osPairing				'K'//�����
#define osUnpaired				'L'//��ȳ��

//�ʵ�����
typedef char TBillType;
#define btDay					'D'	//�սᵥ
#define btMonth					'M'	//�½ᵥ

//�˵���ʽ����
typedef char TBillFormatType;
#define bftText					'T' //���ı�
#define bftHtml					'H'	//��ҳ
#define bftPdf					'P'	//Pdf
#define bftExcel				'E'	//Excel
#define bftRtf					'R'	//rtf
#define bftLink					'L'	//����
#define bftDoc					'D'	//doc
#define bftCsv					'C'	//Csv
#define bftDsv					'd'	//Dsv
#define bftPng					'p'	//Png

//�û����
typedef char TUserIdentity;
#define uiNone					'n'
#define uiUnDefine				'u' //δ����
#define	uiUser					'c'	//���ͻ�
#define uiProxy					'd' //�µ���
#define	uiBroker				'b'	//������
#define	uiTrader				't'	//����Ա
#define	uiQUser					'q'	//����ͻ�

//ƽ�ַ�ʽ
typedef char TCoverMode;
#define	cmNone					'N'	//�����ֿ�ƽ
#define	cmUnfinish				'U'	//ƽ��δ�˽�
#define	cmCover					'C'	//���֡�ƽ��
#define	cmCoverToday			'T'	//���֡�ƽ�֡�ƽ��

//��Ȩ����
typedef char TExcuteType;
#define etAMERICAN				'1' //��ʽ��Ȩ
#define etEUROPEAN				'2' //ŷʽ��Ȩ

//���ʽ
typedef char TDeliveryMode;
#define	dmGoods					'G'	//ʵ�ｻ��
#define	dmCash					'C'	//�ֽ𽻸�
#define	dmExecute				'E'	//��Ȩ��Ȩ
#define	dmAbandon				'A'	//��Ȩ��Ȩ

//�ֱֲ�֤����㷽ʽ
typedef char TDepositMode;
#define	dmNormal				'N'	//����
#define	dmClean					'C'	//��Լ���ֲ�
#define	dmLock					'L'	//Ʒ������

//��������
typedef char TPswType;
#define	ptTrade					'T'	//��������
#define	ptQuote					'Q'	//��������
#define	ptAuth					'A'	//�����µ�����
#define	ptMoney					'M'	//�ʽ�����

//�ɽ���ʽ
typedef char TMatchMode;
#define	mmNormal				'N'	//����
#define	mmUpdate				'U'	//����ί��
#define	mmOther					'O'	//����

//�����˺�����
typedef char TTradeNoTyep;
#define	ttNormal				'N'	//����
#define	ttSpeculation			'T'	//Ͷ��
#define	ttHedging				'B'	//��ֵ
#define	ttSpread				'S'	//����

//��֤����㷽ʽ
typedef char TDepositMode;
#define	dmRatio					'1'	//����
#define	dmQuota					'2'	//����
#define	dmDiffRatio				'3'	//��ֵ����
#define	dmDiffQuota				'4'	//��ֵ����
#define	dmDisCount				'5'	//�ۿ�

//����״̬
typedef char TUpperType;
#define usUnInit				(0)	//δ��ʼ��
#define usInited				'0'	//�Ѿ���
#define usLogined				'1'	//�ѵ�¼
#define usConnected				'2'	//������
#define usDisconnected			'3'	//�ѶϿ�

//���������
typedef char TCashType;
#define ctOut					'O' //����
#define ctIn					'I' //���
#define ctAdjust				'A' //�ʽ����

//�����ʽ
typedef char TCashMode;
#define cmNone					'N'
#define cmAmount				'A' //ת��
#define cmCheck					'B' //֧Ʊ
#define cmCash					'C' //�ֽ�
#define cmExchange				'E' //����
#define cmTransfer				'T' //����

#define cmFeeAdjust				'F' //Ӷ�����
#define cmProfitAdjust			'P' //ӯ������
#define cmPledge				'L' //��Ѻ�ʽ�
#define cmInterestre			'I' //��Ϣ����

//���״̬
typedef char TCheckState;
#define csNone					'N'
#define csWaiting				'W' //�����
#define csPass					'Y' //��ͨ��
#define csFail					'F' //δͨ��
#define csTransing				'T' //����;

//�������ģʽ
typedef char TCheckMode;
#define cmNone					'N' //�����
#define cmAutoCheck				'A' //�Զ����
#define cmManualCheck			'M' //�˹����

//�ɽ�ɾ�������ر��
typedef char TDeleteType;
#define	dtYes					'Y'	//��
#define	dtNo					'N'	//��
#define dtHide					'D'	//���ڵĳɽ���

//��Ϣ��ѯ����
typedef char TMsgQryType;
#define mqtAll					'A'	//ȫ��
#define mqtOnlyValid			'E' //����Ч

//��Ϣ����
typedef char TMessageType;
#define mtQuote					'Q'	//������Ϣ
#define mtTrade					'T' //������Ϣ

//��Ϣ����
typedef char TMessageLevel;
#define mlInfo					'I'	//��ʾ
#define mlWarning				'W' //����
#define mlError					'E' //����
#define mlVital					'V' //��Ҫ
#define mlUrgent				'U' //����

//�۸񾫶�ȡ����
typedef char TPrecisionType;
#define ptDefault				'D'	//Ĭ��
#define ptSpread				'S'	//�۲�
#define ptCarry					'C'	//Carry
#define ptAverage				'A'	//����
#define ptOptionRiskParam		'O' //��Ȩ���ղ���

//ѯ����
typedef char TEnquirerType;
#define eExchange				'M' //����������
#define eUser					'P' //��Ա�ͻ�����

//�ͻ�����
typedef char TUserType;
#define utPersonal				'P' //����
#define utOrgan					'O' //����
#define utMarketMaker			'M' //������

//�û���ϵ������
typedef char TUserProxyType;
#define uptNone					'N'
#define uptTrade				'T' //ָ���µ���
#define uptCash					'C' //�ʽ������
#define uptBillVerify			'S' //���㵥ȷ����
#define uptOpenAccount			'O' //������Ȩ��
#define uptAgencyTrade			'A' //ί�н�����
#define uptAssetsManagement		'M' //�ʲ�������
#define uptUnionAccount			'U' //�����˻�
#define uptBeneficiary			'B' //����������
#define uptUSAuthperson			'P' //������Ȩ��
#define uptUSBeneficiary		'E' //����������
#define uptAuthSign				'I' //��Ȩǩ����

//�ڻ��ʽ��㷨
typedef char TFuturesAlg;
#define faTradeByTrade			'1'	//���
#define faMarketByMarket  		'2'	//����

//��Ȩ�ʽ��㷨	
typedef char TOptionAlg;
#define oaFutures				'1'	//�ڻ���ʽ
#define oaOption 				'2'	//��Ȩ��ʽ

//socket�Ͽ�ԭ�� reason
typedef int TSocketDisconnectReason;
#define sdrInitiative			(1) //�����Ͽ�
#define sdrPassive				(2) //�����Ͽ�
#define sdrReadError			(3) //������
#define sdrWriteError			(4) //д����
#define sdrBufFill				(5) //��������
#define sdrAsynError			(6) //�첽��������
#define sdrParseError			(7) //�������ݴ���
#define sdrConnectOuttime		(8) //���ӳ�ʱ

//���ݶ�������
typedef char TDataSubscibeType;
#define dstSubscibe				'S' //����
#define dstCancel				'C' //ȡ��
#define dstQuery				'Q' //��ѯ

//ת�˷���
typedef char TTransDirect;
#define tdNone					'N' //δ����
#define tdToFutures				'I' //����->�ڻ�(ת��)
#define tdToBank				'O' //�ڻ�->����(ת��)

//ת��״̬
typedef char TTransState;
#define tsNone					'N' //δ����
#define tsSend					'R' //�ѷ���
#define tsTransing				'T' //ת����
#define tsTransed				'S' //ת�˳ɹ�
#define tsTransFail				'F' //ת��ʧ��
#define tsReversing				'r' //������
#define tsReversed				's' //�����ɹ�
#define tsReversFail			'f' //����ʧ��,��״̬����

//ת�˷���
typedef char TTransInitiator;
#define tiNone					'N' //δ����
#define tiFutures				'F' //�ڻ�
#define tiBank					'B' //����

//����ģʽ
typedef char TTriggerMode;
#define tmNone					'N' //��
#define tmLatest				'L' //���¼�
#define tmBid					'B' //���
#define tmAsk					'A' //����

//��������
typedef char TTriggerCondition;
#define tcNone					'N' //��
#define tcGreater				'g' //����
#define tcGreaterEqual			'G' //���ڵ���
#define tcLess					'l' //С��
#define tcLessEqual				'L' //С�ڵ���


//���ݺϼ�����
typedef char TTotalType;
#define ttDetail				'D' //��ϸ
#define ttPrice					'P' //���۸�ϼ�
#define ttContract				'C' //����Լ�ϼ�

//�ֲ�����
typedef char TPositionType;
#define ptTotal					'T' //�ֲֺϼ�
#define ptSub					'S'	//�ֲ�С��
#define ptDetail				'D'	//�ֲ���ϸ

//�ֱֲ�֤��汾
typedef char TDepositVertion;
#define dvEsunny3				(3) //3.0
#define dvEsunny9				(9) //9.0
#define dvDaystar				'D' //Daystar
#define dvCTP					(1) //1.0

//����������Ϣ
typedef char TLinkType;
#define ltCheckInfo				'C' //���������Ϣ

//FATCA״̬
typedef char TFATCAState;
#define fsAmerican				'0' //������
#define fsUnAmerican			'1' //��������
#define fsNoIntention 			'2' //�����-�޼���
#define fsWilling				'3' //�����-�м���
#define fsTempUser				'4' //��ʱ��
#define fsNoApplicable			'5' //������
#define fsPFFI					'A' //PFFI	
#define fsDCFFI					'B' //DCFFI	
#define fsODFFI					'C' //ODFFI	
#define fsUSE					'D' //USE	
#define fsUSEBO					'E' //USEBO	
#define fsPNFFE					'F' //PNFFE	
#define fsEE					'G' //EE	
#define fsNPFFI					'H' //NPFFI	

//����ƽ̨
typedef char TTradePlatFrom;
#define tpEsunny				'E' //esunny
#define tpJTrade				'J' //jtrade
#define tpPats					'P' //PATS
#define tpSP					'S' //SP

//˰������
typedef char TTaxType;
#define ttW8					'8' //W-8
#define ttW9					'9' //W-9

//ί��ƫ�뷶Χ����
typedef char TRangeType;
#define rtRate					'R' //�ٷֱ�
#define trValue					'V' //��ֵ

//ί��ƫ�뷶Χ��������
typedef char TActionType;
#define atWarn					'W' //����
#define atReject				'R' //�ܾ�

//�۸�����
typedef char TPriceType;
#define ptNone					'n'
#define ptMatch					'm' //���̳���
#define ptWorking				'w' //�ҵ�����
#define ptLast					'l' //���³���
#define ptUnLimit				'u' //����ͣ��
#define ptMarket				'a' //�м�


#endif