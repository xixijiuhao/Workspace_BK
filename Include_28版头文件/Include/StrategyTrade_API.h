#ifndef _STRATEGYTRADE_API_
#define _STRATEGYTRADE_API_

#include "RawTradeApi_Struct.h"

#define PLUG_STRATEGYTRADE_NAME 			L"StrategyTrade"
#define PLUG_STRATEGYTRADE_VERSION 			L"9.3.0.14"

typedef char	TStrategyContract[101];			//��Լ
typedef char	TStrategyUserNo[21];			//�ʽ��˺�
typedef char	TStrategySign[21];				//��������ʶ
typedef char	TStrategyDateTime[21];			//ʱ��
typedef wchar_t	TStrategyErrorText[51];			//������Ϣ

typedef int StrategyWorkState;				//����״̬
#define SWS_NONE			0x00000000		//���� �޹���ģ��
#define SWS_DrawLine		0x00000001		//���߹���
#define SWS_StopTrade		0x00000010		//ֹ����
#define SWS_SpreadTrade		0x00000100		//��������
#define SWS_Assist			0x00001000		//׷�۹���

typedef char TTStrategyType;
#define sgtDrawLine 'd' //����������
#define sgtPrice	'p'	//�۸�������
#define sgtTime		't' //ʱ��������
#define sgtOpen		'o'	//���̴���������
#define sgtPreOrder	'r'	//Ԥ��

typedef char TTypeFilter;
#define tfAll		'a'	//ָ����Լ���е�������
#define tfActive	'c' //������|�ѹ��� ���޸ĵ�������
#define tfEnd		'u'	//�Ѵ���|ָ��ʧ��|��ʧЧ �����޸ĵ�������

typedef char TDrawType;
#define tdtBuy		'b'	//��
#define tdtSell		's'	//��
#define tdtCover	'c'	//ƽ
#define tdtBackHand	'h'	//��
#define tdtErase	'e'	//���

typedef int TDLPriceMode;
#define tpm_Last		0	//���¼�
#define tpm_Match		1	//���̼�
#define tpm_Work		2	//�ҵ���
#define tpm_Market		3	//�м�
#define tpm_DrawLine	4	//���߼�
#define tpm_Unlimit		5	//����ͣ���

typedef int DrawLineType;
#define DLT_Condition	0		//����������
#define DLT_Stop		1		//ֹ�𵥻���
#define DLT_Spread		2		//����������

//�������ṹ
struct TStrategyInsert 
{
	TStrategyContract		sContractId;		//��Լ����*
	TTStrategyType			StrategyType;		//��������*
	TStrategyUserNo			sUserNo;			//�ʽ��˺�*
	TStrategySign			sSign;				//��������ʶ*
	TDirect					tDirect;			//��������
	TOffset					tOffset;			//��ƽ
	double					TriggerPrice;		//�����۸�
	uint					OrderQty;			//����

	TTriggerCondition		TriggerCondition;	//��������
	TDLPriceMode			OrderType;			//ί�м۸�����
	double					dOverPrice;			//����
	bool					bBackHand;			//����

	TTriggerMode			TriggerMode;		//����ģʽ
	double					OrderPrice;			//�۸�
	TStrategyDateTime		sTrigTime;			//ʱ������ ����ʱ��
};

//�������޸� (��Լ �ʽ��˺� �����޸�)
struct TStrategyModify :public TStrategyInsert
{
	uint					StrategyOrderId;	//���������*  //ע�� *�ֶ�ֵ�����޸�
};

//������֪ͨ�ṹ
struct TStrategyOrder :public TStrategyModify
{
	TOrderState				OrderState;			//������״̬     ������|�ѹ���  �Ѵ���|ָ��ʧ��|��Ч��
	TStrategyDateTime		sLocalTime;			//�����µ�ʱ��
	TStrategyDateTime		sUpdateTime;		//���ظ���ʱ��	
	TStrategyErrorText		ErrorText;			//����������Ϣ
	const SCommodity*		pCommodity;			//Ʒ����Ϣ
};

//////////////////////////////////////////////////////////////////////////ֹ��ֹӯ
typedef int  StopType;
#define stLoss			0x00000001	//ֹ��
#define stProfit		0x00000010	//ֹӯ
#define stProtectInvest	0x00000100	//����
#define stFloatLoss		0x00001000	//���� 

struct HWND__; typedef HWND__* HWND;
//ֹ��ؼ���
struct TStopKeyinfo
{
	TStrategyUserNo		UserNo;		//�ʽ��˺�
	TStrategySign		Sign;		//��������ʶ
	TStrategyContract	Code;		//��Լ����
	char				Direct;		//��������
};
//��������Ӯ��
struct  LossProfitSet :public TStopKeyinfo
{
	StopType	stype;					//Ӯ������  ֹ�� ֹӯ ���� ����

	double		dBasePrice;				//��׼�۸�//��ʼ�Ƚϼ۸�  //����ʱ���������¼ۣ��ɽ��ۣ��仯
	double		dStopLossPrice;			//ֹ�𴥷���   			�� dBasePrice-LossPoint*MinChangePrice			�� dBasePrice+LossPoint*MinChangePrice
	double		dStopProfitPrice;		//ֹӯ������			�� dBasePrice+ProfitPoint*MinChangePrice		�� dBasePrice-ProfitPoint*MinChangePrice
	double		dFloatPrice;			//�����۸�				dLastPrice-dBasePrice>=dFloatPrice				//dStopLossTrigPrice+=dFloatPrice;
	double		dStopKeepPrice;			//�����۸�
};
//ί��ֹ����Ϣ
struct TOrderStopInfo
{
	LossProfitSet	item;
	uint			uCount;				//����
	int				OrderID[100];		//������
};
//�ֲ�����ʾ����
struct TPositionShow
{
	uint		 uStopQty;				//ֹ��ֹӯ��
	double       dStopLossPrice;		//ֹ���
	double       dStopFloatPrice;		//������
	double       dStopProfitPrice;		//ֹӯ��
	double       dStopKeepPrice;		//������
};
//ί����ʾ����
struct TOrderShow :public TPositionShow
{
	bool		bTriggered;				//������
};
//���黭�� �ֲ����� (��֧�� չʾ �޸� ֹ��ֹӯ��)
struct TPositionDraw 
{
	uint		 uLocalId;				//���ر��
	StopType	 nType;					//ֹ��ֹӯ����
	uint		 uStopQty;				//ֹ��ֹӯ��
	double       dStopLossPrice;		//ֹ���
	double       dStopProfitPrice;		//ֹӯ��
};
//���� ����ֹ��ֹӯ��
struct TAddStopDraw 
{
	const TPosition*	pPosition;		//�ֲ�ָ��
	double				dPrice;			//�����۸�
};
//���� �޸�ֹ��ֹӯ��
struct TModifyStopDraw
{
	const TPosition*	pPosition;		//�ֲ�ָ��
	uint				uLocalId;		//���ر��
	StopType			nType;			//ֹ��ֹӯ����
	double				dPrice;			//�޸ĵĴ����۸�
	uint				uQty;			//�޸�����
};
//���� ɾ��ֹ��ֹӯ��
struct TDeleteStopDraw
{
	const TPosition*	pPosition;		//�ֲ�ָ��
	uint				uLocalId;		//���ر��
};
//////////////////////////////////////////////////////////////////////////
//��������
typedef char TSpreadType;
#define stSpreadSyn		'S' //ͬ������(��������)
#define stSpreadSequ	'E' //�Ⱥ�����

//�����������״̬
typedef char TPairState;
#define psNone			'N'//��
#define psPaired		'P'//�����
#define psPairing		'I'//�����
#define psUnpaired		'U'//��ȳ��

//�����������״̬
typedef char TActiveState;
#define tvsNone			'N'//δ��Ч
#define tvsActive		'A'//��Ч
#define tvsEnd			'E'//����

//���� �µ�����ṹ
struct TSpreadSendOrder 
{
	SContractNoType			ContractNo;					//�����Լ
	TContractSingleKey		Contract[4];				//���׺�Լ�ؼ���
	TStrategyUserNo			UserNo[4];					//�������� �û��˺�2
	TStrategySign			Sign[4];					//�������� ��������ʶ
	TOffset					Offset[4];					//�������� �ڶ��ȵĿ�ƽ���Ʋ�ʱʹ��
	TBool					IsRecove[4];				//�������� �Ƿ�׷��

	TSpreadType				SpreadType;					//��������
	TDirect					Direct[4];					//�������� 
	double					OrderPrice;					//�۲�
	uint					OrderQty;					//����
	SSpreadRateModeType		RateMode[3];				//�۲� �۱�
	TTriggerCondition		TrigCondition;				//��������	���ڵ��� С�ڵ���
	TDLPriceMode			TrigPriceMode;				//���� �ҵ�
	bool					bTimeWaitForTrig;			//ʱ����Ч��
	TStrategyDateTime		BeginTime;					//��ʼʱ��
	TStrategyDateTime		EndTime;					//����ʱ��
};
//���� ����Ӧ��ṹ
struct TSpreadOrder
{
	TSpreadSendOrder		sendorder;					//�µ���Ϣ

	uint					OrderID;					//ί�к�
	uint					SendQty[4];					//�������� ��������
	TBool					LoginState[4];
	uint					MatchQty[4];				//�ɽ���

	TOrderState				OrderState;					//ί��״̬ 
	TPairState				PairState;					//���״̬
	uint					PairQty;					//�������

	int						ErrorCode;					//������Ϣ��				
	TStrategyErrorText		ErrorText;					//���´�����Ϣ
	TStrategyDateTime		UpdateDateTime;

	SSpreadPriorityLeg		PriorityLeg;				//����������
	TActiveState			ActiveState;				//��Ч״̬

	const SCommodity *		pCommodity[4];				//�������� ����Ʒ��
};

class IStrategySpi;
//׷�����ò���
struct TAssistInfo 
{
	uint	OrderID;			//׷�� ������	
	const	IStrategySpi* p;	//׷��ע�����
};

//״̬���֪ͨ
class IStrategySpi
{
public:
	//////////////////////////////////////////////////////////////////////////
	//������״̬�仯֪ͨ NULLʱ��Ҫ���»�ȡ
	virtual void __cdecl OnStrategyNotice(const TStrategyOrder* pdata) = 0;
	//�ֱֲ仯֪ͨ NULLʱ��Ҫ���»�ȡ
	virtual void __cdecl OnPositionNotice(const TPosition* pdata) = 0;
	//������ ����״̬�仯֪ͨ
	virtual void __cdecl OnStrategyWorkState(const StrategyWorkState state) = 0;
	//////////////////////////////////////////////////////////////////////////
	//Ӯ����Ϣ���µ��ֲֺϼ��� ��ʾ
	virtual void __cdecl OnStopInfoToPosition(const TPosition* p) = 0;	
	//Ӯ����Ϣ���µ�ί���� ��ʾ
	virtual void __cdecl OnStopInfoToOrder() = 0;						
	//���ؼ������� �д�ȷ����Ϣ
	virtual void __cdecl OnStopInfoUpdate(bool bShwoUpdate) = 0;	
	//////////////////////////////////////////////////////////////////////////
	//�����������ݸ���
	virtual void __cdecl OnSpreadInfoNotice(const TSpreadOrder* pspread) = 0;
	//////////////////////////////////////////////////////////////////////////
	//׷�ۻص�
	virtual void __cdecl OnAssistInfoRef(uint OrderIdOld,uint OrderIdNew) = 0;
};

//�������ӿ�
class IStrategyTradeAPI
{
public:
	//ע��/ע���ص�
	virtual	bool __cdecl RegistStrategyInfo(IStrategySpi* p) = 0;
	virtual	bool __cdecl UnRegistStrategyInfo(IStrategySpi* p) = 0;

	//��ȡ����״̬������Ա��¼���˳���
	virtual StrategyWorkState __cdecl GetStartegyWorkState() = 0;

	//���߲���
	virtual void __cdecl PopupDrawlineWnd() = 0;																		//�򿪻����µ�����
	virtual void __cdecl CancelDrawLineOperation() = 0;																	//ȡ�������µ�����
	virtual int  __cdecl AddDrawLineStrategyOrder(const TStrategyContract sContractId,double TriggerPrice, DrawLineType nDrawLineType) = 0;	//������������������ 0�ɹ� 
	virtual void __cdecl ModifyDrawLinePice(uint StrategyOrderId, double TriggerPrice, DrawLineType nDrawLineType) = 0;						//�޸����������������۸�
	virtual void __cdecl SyncModifyWnd(uint StrategyOrderId, double TriggerPrice, DrawLineType nDrawLineType, bool bDrag = false) = 0;		//ͬ���򿪻��ߴ���
	
	virtual int  __cdecl AddDrawStopRecord(const TAddStopDraw& tadd) = 0;												//����ֹ��ֹӯ��(����)
	virtual bool __cdecl DeleteDrawStopRecord(const TDeleteStopDraw& tdelete) = 0;										//ɾ��ֹ��ֹӯ��(����)
	virtual int  __cdecl ModifyDrawStopRecord(const TModifyStopDraw& tmodify) = 0;										//�޸�ֹ��ֹӯ��(����)
	virtual void __cdecl SyncStopModifyWnd(const TModifyStopDraw& tmodify, bool bDrag = false) = 0;						//�޸�ֹ�𴰿�
	
	// OrderId==0ʱ ����UserNo��Ӧ������������;   UserNo=="" ���������û��������� 
	virtual	int  __cdecl MoveStrategyOrder(uint StrategyOrderId, DrawLineType nDrawLineType, const char* UserNo = "") = 0;					//ɾ��/ȡ��
	virtual int  __cdecl SuspendStrategyOrder(uint StrategyOrderId, DrawLineType nDrawLineType, const char* UserNo = "") = 0;				//��ͣ 
	virtual int  __cdecl ResumeStrategyOrder(uint StrategyOrderId, DrawLineType nDrawLineType, const char* UserNo = "") = 0;				//���� 
	virtual int  __cdecl TrigStrategyOrder(uint StrategyOrderId, DrawLineType nDrawLineType) = 0;											//��������
				 
	virtual int  __cdecl SuspendDrawStopRecord(const TPosition* pPosition) = 0;											//��ͣ ��Լ��Ӧ������ֹ��ֹӯ��(����) 
	virtual int  __cdecl ResumeDrawStopRecord(const TPosition* pPosition) = 0;											//���� ��Լ��Ӧ������ֹ��ֹӯ��(����)

	//��ȡָ����Լ�ĳֲ�
	virtual int  __cdecl GetPositionTotal(SContractNoType ContractId, TPosition* data[], int count) = 0;
	
	//������
	virtual int  __cdecl GetAllStrategyOrder(uint beginId, TStrategyOrder* data[], int count) = 0;						//��ȡ����������
	virtual int  __cdecl GetStrategyOrdersByContractId(SContractNoType ContractId, uint beginId, TStrategyOrder* data[],
													  int count, TTypeFilter nTypeFilter = tfActive) = 0;				//��ȡָ����Լ��������
	virtual const TStrategyOrder* __cdecl GetStrategyOrder(uint OrderId) = 0;											//��ȡ������������Ϣ

	//ֹ��ֹӯ	
	virtual	void __cdecl AddOrderStopRecord(const TOrderStopInfo& item) = 0;											//ί��ֹ��(�µ����)	
	virtual bool __cdecl GetPositionStopInfo(IDataContainer& Datas, const TPosition* p) = 0;							//��ȡ�ֲ���ʾ��Ϣ(�б�)
	virtual int  __cdecl GetPositionStopInfo(uint beginId, TPositionDraw data[], int count, const TPosition* p) = 0;	//��ȡ�ֲ���ʾ��Ϣ(����)
	virtual bool __cdecl GetOrderStopInfo(TOrderShow& Datas, const TOrder* p) = 0;										//��ȡί����ʾ��Ϣ(�б�)
	virtual void __cdecl StopLossModify(const TPosition* pPosition, HWND hwnd) = 0;										//����ֹ��ֹӯ(�б�)
	virtual void __cdecl StopLossMakeSure(HWND hwnd) = 0;																//ֹ��ȷ��(�µ����)

	//��������
	virtual int  __cdecl AddSpreadOrder(const TSpreadSendOrder& data) = 0;												//����(�µ����)
	virtual int  __cdecl CancelSpreadOrder(uint order_id) = 0;															//���� (�����б�����) order_id==0 ��ȫ��
	virtual int  __cdecl GetAllSpreadOrder(uint beginId, TSpreadOrder* data[], int count) = 0;							//��ȡ����������(�����б�)
	virtual int  __cdecl GetTriggeringSpreadOrders(uint beginId, TSpreadOrder* data[], int count,
												   SContractNoType ContractId) = 0;										//��ȡ������������(����)
	virtual const TSpreadOrder* __cdecl GetSpreadOrderInfo(uint order_id) = 0;											//��ȡ����������Ϣ
	virtual int  __cdecl GetSpreadSubOrderIDCounts(uint order_id) = 0;													//��ȡ�����������������������б�
	virtual int  __cdecl GetSpreadSubOrderIDs(uint order_id,uint beginId, uint data[], 
											  int count,bool bPosIndex = false) = 0;									//��ȡ�������������������б�
	virtual bool __cdecl GetSSpreadContractNo(SContractNoType &ContractId,const TSpreadOrder* data) = 0;				//��ȡ������Լ����

	//׷���ӿ�
	virtual bool __cdecl AddAssistOrder(const TAssistInfo& passist) = 0;												//����׷�۵� ParamΪ�� ʹ��׷������������Ϣ
	virtual bool __cdecl StopAssistOrder(const TAssistInfo& passist) = 0;												//ֹͣ׷�ӵ� order_idΪ0 ֹͣ��Ӧ����׷��

	//���������ӿ� ����0��ʾʧ��
	virtual int  __cdecl GetSpreadIdbyOrderId(uint order_id) = 0;														//ͨ��OrderId�õ�����������ID(�����б�)
};


#endif