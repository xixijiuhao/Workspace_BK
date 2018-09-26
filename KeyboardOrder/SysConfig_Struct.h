#pragma once 
#include <WinDef.h>
#include <WinGDI.h>

#pragma	pack(push, 1)

#define  ZCE_NO		"ZCE"
#define  DCE_NO		"DCE"
#define  SHFE_NO	"SHFE"
#define  CFFEX_NO	"CFFEX"


#define LOAD_XML_CFG							0x00000000

#define	KEYORDER_ORDEROPTION_HOTKEY				0x00000002		//���� ->�µ���ݼ�
#define	KEYORDER_ORDEROPTION_HOTKEYOTHER		0x00000004		//����->������ݼ�
#define	KEYORDER_ORDEROPTION_ABNORMAL			0x00000008		//����->�쳣����
#define	KEYORDER_ORDEROPTION_DEFAULTNUM			0x00000010		//����->�µ�Ĭ����
#define KEYORDER_ORDEROPTION_CONTRACTHOTKEY		0x00000020		//����->��Լ��ݼ�
#define KEYORDER_ORDEROPTION_FACADESET			0x00000800		//����->�������� ������ģ�鴦����Ʋ�һ�� "�߼�"�е����塢�����µ��е�˳��Ҫ����
#define SYSGENERAL_ORDER_SETTING				0x00002000		//���� ->�µ�����
#define SYSFRAME_SOUND_SETTING					0x00008000		//����->��������
#define	BATCH_ORDER_SETTING						0x00080000		//����->����->�����µ�����
#define FAST_QTY_MULTI							0x00100000		//����->�����µ�����������
#define GENERAL_RISK_CONTROL					0x00200000		//����->���տ���
#define CONDITION_ORDER_SET						0x00400000		//����->������
#define KEYORDER_CHANGECODE						0x01000000		//����->��Լ�л�����
#define USERNO_FILTER							0x02000000		//���� ->�ɽ��ر� �µ��˹���
#define TRADE_ADVANCE							0x04000000		//����->�߼�

//������ID���壬һ���������ID������������
//szFieldΪ������Ŀǰ�������׺�����������
//szSectionΪ�������ڵĽڣ�����ʵ���������
//szItemIDΪ������������ID��������ĳ������

struct OrderHotKey//�µ���ݼ������ýṹ
{
	unsigned long	kBuyBySellPrice4;			//�������4
	unsigned long	kSellByBuyPrice4;			//�������4
	int				iOverPoint14;				//���۵���4

	unsigned long	kBuyBySellPrice3;			//�������3
	unsigned long	kSellByBuyPrice3;			//�������3
	int				iOverPoint13;				//���۵���3

	unsigned long	kBuyBySellPrice2;			//�������2
	unsigned long	kSellByBuyPrice2;			//�������2
	int				iOverPoint12;				//���۵���2

	unsigned long	kBuyBySellPrice1;			//�������1
	unsigned long	kSellByBuyPrice1;			//�������1
	int				iOverPoint11;				//���۵���1

	unsigned long	kBuyByBuyPrice1;			//�������1
	unsigned long	kSellBySellPrice1;			//��������1
	int				iOverPoint21;				//���۵���1

	unsigned long	kBuyByBuyPrice2;			//�������2
	unsigned long	kSellBySellPrice2;			//��������2
	int				iOverPoint22;				//���۵���2

	unsigned long	kBuyByBuyPrice3;			//�������3
	unsigned long	kSellBySellPrice3;			//��������3
	int				iOverPoint23;				//���۵���3

	unsigned long	kBuyByBuyPrice4;			//�������4
	unsigned long	kSellBySellPrice4;			//��������4
	int				iOverPoint24;				//���۵���4

	int				iPCType;					//ƽ�ַ�ʽ
	bool			bPCCtrlInvert;				//��Ctrl������ƽ�ַ�ʽ��ʱȡ��
	bool			bKCBySpace;					//���ո����������
	unsigned int	iBySpaceOpenQty;			//���ո���������ֵĿ�������ȡֵ 0Ĭ�Ͽ�����  1����ֲ���

	//�������ʵ���  �������ʿ�ݼ������е�
	double			fQtyMulPoint;

	unsigned long	kCancelKC;					//�������ݼ�
	unsigned long	kCancelPC;					//��������ݼ�
	int				iCancelType;				//����ѡ�� 0==��ȫ�� 1 ��ѡ��

	unsigned long	kCancelOpen;				//�����ֿ�ݼ�
	unsigned long	kCancelCover;				//��ƽ�ֿ�ݼ�
	unsigned long	kCancelLast;				//�����һ��
	unsigned long	kCancelOld;					//������һ��
};

struct OtherHotKey//�µ�������ݼ�
{
	unsigned long	kBuyOfNormal;				//��ͨ�µ�-���
	unsigned long	kSellOfNormal;				//��ͨ�µ�-����
	unsigned long	kKCOfNormal;				//��ͨ�µ�-����
	unsigned long	kPCOfNormal;				//��ͨ�µ�-ƽ��
	unsigned long	kPJOfNormal;				//��ͨ�µ�-ƽ��

	unsigned long	kZXJOfCondition;			//�����µ�-���¼�
	unsigned long	kBuyJOfCondition;			//�����µ�-���
	unsigned long	kSellJOfCondition;			//�����µ�-����
	unsigned long	kDYOfCondition;				//�����µ�->=
	unsigned long	kXYOfCondition;				//�����µ�-<=

	unsigned long	kCancelBySysNO;				//��ϵͳ�ų���

	unsigned long	kYJQC;						//һ�����
	int				iQCPriceType;				//�۸�����
	int				iQCOverPoint;				//���۵���
	bool			bQCCancelOrder;				//���ʱ�Զ�����
	int				iQCType;					//������� ƽ�� ��ƽ�򡢻���ȫ��

	//2015/1/4
	unsigned long	kOTLimit;					//�������� �޼�
	unsigned long	kOTMarket;					//�������� �м�
	unsigned long	kOTLimitStop;				//�������� �޼�ֹ��
	unsigned long	kOTMarketStop;				//�������� ֹ��
	unsigned long	kOTIce;						//�������� ��ɽ
	unsigned long	kOTGhost;					//�������� Ӱ��

	//2015/6/11
	unsigned long   kLockPos;					//һ������
	int				iLockPosBuyPriceType;		//����۸�����  '0'lpOppo   '1'lpQuene  '2'lpLast
	int             iLockPosBuyOverPoint;		//���볬�۵���
	int				iLockPosSellPriceType;		//�����۸�����
	int             iLockPosSellOverPoint;		//�������۵���
	char			code[71];					//��Լ
	int				iOpenQty;					//��������
};

//�����µ���ԼĬ����
struct CommodityVol								
{
	char			commodity[70];				//��ʽ��֮��� �̶���ʽ�ĺ�Լ�ַ��� ZCE SR 305 ; ZCE SR 305 P4400  ;ZCE SR 305/307; ZCE SR 305 P4400/307 C4400
	unsigned int	vol;						//����Ĭ����
	unsigned int	Covervol;					//ƽ��Ĭ����
};

//��������
struct FastQtyMultip								
{
	unsigned long	kFastKey;			//��ݼ�
	double	fMultip;			//��������
};

//�����µ���Լ��ݼ�
struct ContractHotKey
{
	char			szKey[11];					//��Լ��ݼ�
	char			commodity[70];				//��ʽ��֮��� �̶���ʽ�ĺ�Լ�ַ��� ZCE SR 305 ; ZCE SR 305 P4400  ;ZCE SR 305/307; ZCE SR 305 P4400/307 C4400
};

struct AbnormalTradeLimit//�쳣����������Ϣ
{
	char			ZCEExchangeNo[11];
	//char			ZCEsign[21];
	char			DCEExchangeNo[11];
	//char			DCEsign[21];
	char			SHFEExchangeNo[11];
	//char			SHFEsign[21];
	char			CFFEXExchangeNo[11];
	//char			CFFEXsign[21];

	bool			bVerifySelfMatchZCE;		//��֤֣�����Գɽ�
	int				nSelfMatchTimesZCE;			//֣�����Գɽ�������Ŀ
	bool			bVerifyCancelZCE;			//��֤֣������������
	int				nCancelTimesZCE;			//֣�����������ƴ���
	int				nBigCancelNumZCE;			//֣������������
	bool			bVerifyBigCancelZCE;		//��֤֣�����󵥳�������
	int				nBigCancelTimesZCE;			//֣���������������

	bool			bVerifySelfMatchDCE;		//��֤�������Գɽ�
	int				nSelfMatchTimesDCE;			//�������Գɽ�������Ŀ
	bool			bVerifyCancelDCE;			//��֤��������������
	int				nCancelTimesDCE;			//�������������ƴ���
	int				nBigCancelNumDCE;			//��������������
	bool			bVerifyBigCancelDCE;		//��֤�������󵥳�������
	int				nBigCancelTimesDCE;			//�����������������

	bool			bVerifySelfMatchSHFE;		//��֤�������Գɽ�
	int				nSelfMatchTimesSHFE;		//�������Գɽ�������Ŀ
	bool			bVerifyCancelSHFE;			//��֤��������������
	int				nCancelTimesSHFE;			//�������������ƴ���
	int				nBigCancelNumSHFE;			//��������������
	bool			bVerifyBigCancelSHFE;		//��֤�������󵥳�������
	int				nBigCancelTimesSHFE;		//�����������������

	bool			bVerifySelfMatchCFFEX;		//��֤�н����Գɽ�
	int				nSelfMatchTimesCFFEX;		//�н����Գɽ�������Ŀ
	bool			bVerifyCancelCFFEX;			//��֤�н�����������
	int				nCancelTimesCFFEX;			//�н����������ƴ���
	bool			bVerifyOpenCFFEX;			//��֤�н������ֳɽ���
	int				nOpenMatchNumCFFEX;			//�н������ֳɽ���������

	bool			bInsertOrderCheckSelfMatch;	//�µ�ʱ�ж��Գɽ����������Գɽ�����ʱ��ʾ
	bool			bIgnoreOrder;				//�ж��Գɽ�ʱ�������������ɽ����м۵�
	bool			bMaxWarnCount;				//��󾯽����
	int				nMaxWarnCount;				//��󾯽����
	bool			bForbidOper;				//�ﵽ����ֵʱǿ�ƽ�ֹͬ�����

	bool            bShowExchangeCancelCount;	//��ʾ��������ͳ�Ƶĳ��������ܺ�
};
//�����µ� ��������

//�����ֶ�����
enum TradeFieldType
{
	TFT_STRING,
	TFT_NUMERIC,
	TFT_DATE
};

//�ڻ��µ������ֶ� //������ֻ������� ���ܴ��м���� ����ᵼ�����Ͽͻ��汾������ �м���뵼��IAF_ACCOUNT�������ļ������ֵ����Ӧ
enum FutureInputArea
{
	IAF_TJCHECK,			//����ѡ���� ���ܲ�������
	IAF_ACCOUNT,			//�ͻ��˺�
	IAF_COMMODITY,			//��Լ����
	IAF_DIRECT,				//����
	IAF_OFFSET,				//��ƽ
	IAF_PRICE,				//ί�м۸�  ������� + ���� /��ĸ �ĸ� ���ĸ�Ҫ��ʾ�������ί�м۸�֮��
	IAF_PRICE_ADD,
	IAF_PRICE_FENZI,
	IAF_PRICE_CHUHAO,
	IAF_PRICE_FENMU,
	IAF_VOL,				//ί������
	IAF_TRIGGERMODE,		//������ʽ
	IAF_TRIGGERCONDITION,	//��������
	IAF_TRIGGERPRICE,		//�����۸�
	IAF_VOLBL,				//��������
	//IAF_STOPPRICE,			//ֹ��۸�
	//IAF_MATCHCONDITION,		//�ɽ�����
	IAF_VALIDTIME,			//��Ч����

	IAF_SELECODE,			//ѡ���Լ ʼ����ʾ
	IAF_TRIGGERPRICE_ADD,
	IAF_TRIGGERPRICE_FENZI,
	IAF_TRIGGERPRICE_CHUHAO,
	IAF_TRIGGERPRICE_FENMU,
	//IAF_ORDERPRICE_

	IAF_VOLCOUNT,		//�µ�����  ����
	IAF_INCREVOL,		//��������

	IAF_INCREPRICE,			//�۸����� 
	IAF_INCRERPRICE_CHUHAO,	//�۸���������  ��ʾ������ڼ۸�����֮��
	IAF_INCREPRICE_FENMU,	//�۸�������ĸ  ��ʾ������ڼ۸���������֮��

	IAF_ORDERTYPE,			//��������
	IAF_VALIDTYPE,			//��Ч����

	IAF_MAXQTY,				//����� ��ɽ
	IAF_MINQTY,				//��С�� ��ɽ

	//IAF_BUTTON,				//���ð�ť��������
	//IAF_SUM 				//����������ӵ��ֶ��������ǰ��
};
struct  FacadeSetting
{
	LOGFONT			lgBtnFont;			//��ť����
	LOGFONT			lgCheckFont;		//CheckBox����
	LOGFONT			lgStaticFont1;		//��̬�ı�����
	LOGFONT			lgStaticFont2;		//״̬��ʾ����
	LOGFONT			lgComboFont;		//�ɱ༭����
	LOGFONT			lgPositionFont;		//�ֲ�����

	int				iLeftMargin;		//�ؼ���߾�
	int				iTopMargin;			//�ؼ����߾�

	int				iHorCtrlInterval;	//ˮƽ�ؼ����
	int				iVerCtrlInterval1;	//��ֱ�ؼ����1 ��̬�ı���ˮƽ�ؼ��ļ��
	int				iVerCtrlInterval2;	//��ֱ�ؼ����2 ˮƽ�ؼ���״̬��ʾ�ļ��
	int				iVerCtrlInverval3;	//��ֱ�ؼ����2 ��ť֮��Ĵ�ֱ���

	COLORREF		rgbBkPosition;		//�ֲֵı���ɫ
	COLORREF		rgbBuy;				//��ͷ�ĳֲ���ɫ
	COLORREF		rgbSell;			//��ͷ�ĳֲ���ɫ

	// �ֶνṹ����
	struct OrderField 
	{
		int		id;						// �ֶα�ţ���Ӧ��TradeOrderField������� FutureInputArea
		int		width;					// ��ʾ���(������) �Զ���� �������޹�
		int		height;					// ��ʾ���(������) �Զ���� �������޹�
		bool	bShow;					//�Ƿ���ʾ
	};

	int				iOrderFieldCount;	//�ֶνṹ����	
	OrderField		*pOrderField;		//�ֶνṹָ��

	unsigned int	iDefaultKbCfg;		//�����µ����ý���  //0==С���� 1==������ 2==������
};


//����->�µ����ýṹ
struct GeneralOrderSetting
{
	bool			bAutoSelectPJPZ;			//ƽ��ƽ������Ӧ

	bool			bLiquidateAutoCancel;		//ƽ���Զ�����
	unsigned int	nPCCDType;					//ƽ�ֳ�������

	bool			bPKAutoKC;					//ƽ��ʱ����ƽ�ּ�����λ�Զ�����
	int				nOverPCStep;				//����ƽ�ּ�����λ

	unsigned int	nCursorPos;					//�۸���λ��(����)
	bool			bReturnWT;					//�µ��󷵻�ί�н���
	bool			bDbClickOrderCancel;		//˫���Ŷӵ������ñ�ί��
	bool			bUpDownEqualTab;			//���¼�=Tab��
	bool			bLeftRightEqualTab;			//���Ҽ�=Tab��
	unsigned int	nWTOrderType;				//ί���µ�����
	bool			bClickQuoteOrCCOrder;		//���������ֲ��Զ��
	bool			bCommodityChangeAutoSelect;	//��Լ����ı��Զ�ѡ������

	bool			bDbClickQuoteGetVol;		//˫�������ȡ������
	unsigned int	nDefaultPriceType;			//Ĭ�ϼ۸�
	int				nDefaultPriceOverStep;		//Ĭ�ϼ۸񳬼۵���

	//2014/4/9
	bool			bOpenMaxAutoCancel;			//�����Զ�����

	//2014/9/9
	bool			bT1;//T+1
	//2014/10/21
	bool			bMatchTip;//�յ��³ɽ�ʱ����������ʾ

	bool			bPriceDynaChange;			//ί�м۸�������ͬ��ˢ��
	bool			bCommodityChangeGetPrice;	//��Լ����ı��ȡ�۸�  �����ε�����8.0��Լ�л��������ʹ��
	bool			bSetWTFocus;				//����ί�д���Ĭ�Ͻ���
	unsigned int	nWTDefaultFocusCol;			//ί�д���Ĭ�Ͻ���λ�ڵڼ���
	bool			bDbClickQuoteOpen;			//˫�������Ƿ��ƽ  iClickQuotOpen=0Ĭ�Ͽ��� iClickQuotOpen=1�Զ���ƽ
	unsigned int	iClickQuotOpen;				//���bDbClickQuoteOpenʹ��
	bool			bAutoMarketPrice;			//ί�м۸�Ϊ���Զ����м۵�
	bool			bOnlyFillActiveOrder;		//�����Զ����󼤻��µ����� ?
	bool			bKuaiSu;					//�Ƿ�����µ�

	bool			bMsgTip;//�յ���Ϣ������������ʾ
	bool			bNewMsgTip;//������������Ϣ
	bool			bOrderErrorTip;				//ί��ʧ�ܵ�����ʾ

	unsigned int	iDBQuotDirect;//����������䷽ʽ 0(˫��������������˫������������) 1(˫������������ ˫������������)
};
// �ɽ��ر� �µ��˹��� �����TMatchSaveInfo ����
struct TMatchTipQty
{
	struct TMatchSaveInfo
	{
		char UserNo[21];
	};

	unsigned int	iQty;
	TMatchSaveInfo	*pMatchSaveInfo;
};
//���Ի�->��������
struct SoundSetting
{
	bool	bQueue;
	bool	bPartFilled;
	bool	bFilled;
	bool	bCanceled;

	bool	bFail;
	bool	bTradeDisConnect;
	bool	bVipDisconnect;
	bool	bCounterMsg;
};

//����->���տ���
struct GeneralRiskControl
{
	bool			bCancelConfir;				//����ʱ����ȷ����Ϣ
	unsigned int	nOpenPriceScope;			//����ʱί�������¼۲Χ����
	unsigned int	nCoverPriceScope;			//ƽ��ʱί�������¼۲Χ����
	unsigned int	nSingleMaxOrderQty;			//��������µ���

	//2014/10/24
	bool			bSaveLastQty;				//�Զ��ʱ�����ϴε�ί������
	bool			bOrderTypeToLimit;			//�µ��󶨵�������Ϊ�޼�
	bool			bValidTypeToDay;			//�µ�����Ч������Ϊ������Ч
	bool			bCancelFCover;				//�µ���ȡ��ǿƽѡ��

	int				iOnlyCoverDay;				//�µ���Լ�ٽ�ֻ��ƽ���յ�����ʾ
	bool			bEmptyDirect;				//�µ����������
	bool			bFastOrderConfir;			//�����µ���ȷ��
	bool			bSuperPriceProtect;			//�ҵ����۱���
	unsigned int	nKSOpenPriceScope;			//���ٿ��������۲Χ����
	unsigned int	nKSCoverPriceScope;			//����ƽ�������۲Χ����
	bool			bKSDBclickSeleCode;			//�����µ�˫���л���Լ

	bool			bOrderConfir;				//��ͨ�µ�����ȷ�ϴ���

	bool			bCancelT1;					//�µ���ȡ��T+1
};

//����->����������
struct ConditionOrderSet
{
	unsigned int	nTriggerMode;			//������ʽ ���¼ۡ���ۡ�����
	unsigned int	nTriggerCondition;		//�������� ֹ��ģʽ��ֹӮģʽ

	unsigned int	nTriggerPrice;			//���³��ۡ����̳��ۡ��ҵ����ۡ���Ĭ��
	int				iTriggerPriceOverStep;	//���۵���
	bool			bTriggerPirctDyn;		//�����۸�������ͬ��ˢ��
};
//�����µ� �ͺ�Լ�л�
struct CommodityInfo
{
	char			commodity[70];				//��ʽ��֮��� �̶���ʽ�ĺ�Լ�ַ��� ZCE SR 305 ; ZCE SR 305 P4400  ;ZCE SR 305/307; ZCE SR 305 P4400/307 C4400
};
struct CommodityChange								
{
	bool			bChangeCode;//���¼��л���Լ

	int				iCommodityInfoCount;	//�ֶνṹ����	
	CommodityInfo	*pCommodityInfo;
};

//���� ->�߼�
struct TradeAdvance
{
	bool			bMoneyShortageAdjustCanOpen;//�ʽ���ʱ�������Զ�����Ϊ�ɿ�����
	bool			bSingleOrderSplit;			//����ί�д��� nSingleOrderSplitNum ʱ�Զ����
	unsigned int	nSingleOrderSplitNum;		

	bool			bCoverAutoNum;				//ƽ�������ڿ�ƽ��ʱ�Զ����Ϊ���
	bool			bIsShowPosition;			//�Ƿ���ʾ�ֲ�
	unsigned int	nPostionShowType;			//�ֲ���ʾ���� ���ʳֲ�0 ȫ���ֲ�1

	bool			bCancelBtn;					//�����µ��������򵥡��Զ�����˳��ť

	bool			bLightedShow;				//ί����������������ƽ��������ʾ
	unsigned int	iCFFEXMarket;				//�н����м۴�����

	//2014/1/14
	bool			bOnlyOneOrderBtn;			//�����µ�ֻ��ʾһ����ť
	//2015/2/10
	bool			bReverFilter;				//IDC_CHECK_ReverFilter

	bool			bShowCmb;					//�Ƿ���ʾ�������ͺ���Ч����

	bool            bShowFilter;				//�����б���ʾ��������  Ĭ��true
};

#pragma	pack(pop)