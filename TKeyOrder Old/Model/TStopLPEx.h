#ifndef T_STOP_LOSS_PROFIT_EXPORT_H
#define T_STOP_LOSS_PROFIT_EXPORT_H



//#define�ṹ��
#pragma pack(push,1)
typedef char OrderLPType;
#define otLoss		'0'	//�޼�ֹ��
#define otProfit	'1'	//�޼�ֹӯ
#define otFloat		'2' //����ֹ��
#define otBreakEven	'3' //����ֹӯ
#define otVoid		'v' //��
typedef char OrderState;
#define osGeneral	'0'	//δ����
#define osTrig		'1'	//�Ѵ���
#define osSuspend	'2'	//�ѹ���
#define osBreak		'3'	//��������
#define osDeleted	'4' //��ɾ��
typedef char TriggerModeBase;
#define tmbLatest	'0'//���¼۴���
#define tmbOppo		'1'//���̼۴���

typedef std::multimap<std::string, unsigned int> DATAMAP;
typedef DATAMAP::const_iterator CIT;
typedef pair<CIT, CIT> Range;
typedef pair<std::multimap<std::string, unsigned int>::const_iterator, std::multimap<std::string, unsigned int>::const_iterator> PositionKeyRange;
struct TStopLPInsertItem
{
	char			sign[21];
	char			userNo[21];
	char			code[51];
	TDirect			direct;
	OrderLPType		orderType;			//���ͣ�ֹ����ֹӯ��
	int				orderPriceType;		//ί�м۸�����, 0 �ŶӼ� 1���ּ� 2 ����  3�м� 4���¼�
	double			triggerPrice;		//�����۸�
	unsigned int	Qty;				//����
	unsigned int	MaxCount;			//�����������
	TriggerModeBase trigBase;			//����ģʽ��ֹ��ֹӯ��ָ���۸����ͱ仯����
	double			floatPoint;			//����ֹ�� �۲�
	double			breakEvenPoint;		//�����۲�
	int				point;				//���۵���
	char			descri[51];			//������Ϣ
	double			openPrice;			//���־���
	TStopLPInsertItem()
	{
		triggerPrice = 0.0;
		floatPoint = 0.0;
		breakEvenPoint = 0.0;
		openPrice = 0.0;
	}
};
struct TStopLPitem: public TStopLPInsertItem
{
	unsigned int	keyID;				//�ؼ�ID ���� �������¼���
	OrderState		state;				//״̬

	unsigned int	TriggerCount;		//������������
	bool			LastTirgger;		//��һ���Ƿ񴥷�
	char			trigTime[31];		//����ʱ��
};

#define AutoStopType 0
#define StopOpenType 1
struct AutoStopInfo
{
	int    iOrderType; // AutoStopType�����Զ�ֹ��  StopOpenֹ�𿪲�  ����

	//ֹ�𿪲�����ֶΣ�
	bool   bLoss;        //�Ƿ���ֹ�� ����
	bool   bProfit;      //�Ƿ���ֹӯ�� ����
	bool   bFloat;       //�Ƿ��͸���ֹ�� ����

	double dstopLoss;    //ֹ�� ����
	double dstopProfit;  //ֹӯ ����
	double dFloatPoint;  //����ֹ�� ����
	int    iOrderQty;    //ί������ ����

	//�����Զ�ֹ�𱣴��ֶ�(�����ĸ�����������)
	double dCounterPrice; //ί�з����ĶԼ� ����
	double dMatchPrice;   //��һ���ɽ���
	AutoStopInfo()
	{
		iOrderType = AutoStopType;
		bLoss = false;
		bProfit = false;
		bFloat = false;
		dCounterPrice = 0.0;
		dMatchPrice = 0.0;
		dstopProfit = 0.0;
		dstopLoss = 0.0;
		dFloatPoint = 0.0;
		iOrderQty = 0;
	}
};
#pragma pack(pop)
enum DataType
{
	Del_Type,	//ɾ��ȫ��
	Add_Type,	//����һ��
	Chg_Type,	//�޸�һ��
	Trg_Type,	//���������仯
	ChO_Type	//�޸�Ϊ�������͵���
};
class IStopLPListener
{
public:
	virtual void __cdecl Update(DataType type, const TStopLPitem*pdata) = 0;
};

class IStopLP
{
public://�����ӿ�
	//����ֹ��ֹӯ����
	virtual void __cdecl Attach(IStopLPListener *pOb) = 0;
	//ȡ������
	virtual void __cdecl Detach(IStopLPListener *pOb) = 0;
	//��ֹ��
	virtual unsigned int __cdecl InsertLPOrder(const TStopLPInsertItem *item) = 0;
	//ɾ��ֹ��
	virtual void __cdecl DeleteLPOrder(const unsigned int KeyID) = 0;
	//�޸�ֹ��
	virtual unsigned int __cdecl ModifyLPOrder(const TStopLPitem *item) = 0;
	//����ֹ��
	//����������1(����TriggerΪtrue�������ߴ�����������
	virtual void __cdecl TriggerLPOrder(const unsigned int KeyID, bool Trigger) = 0;
	//����ֹ��
	virtual unsigned int __cdecl OpenLPOrder(const AutoStopInfo *item, const TSendOrder * stSendOrder) = 0;
	//ɾ������ֹ��
	virtual void __cdecl DeleteAllLPOrder() = 0;

	//���ָ���ͻ�����ֹ��
	//���������
	// Datas��ֹ�𵥵�������Ϣ��������Ԫ������Ϊ const TStopLPitem*
	//���������
	// UserNo���ͻ���ţ�Ϊ""��nullptrʱ�����ֿͻ�
	// SCode: ��Լ����,Ϊ""��nullptrʱ�����ֺ�Լ
	// Direct: ����ΪdBothʱ��������������dBuyΪ��dSellΪ��
	// NoTrig��δ������Ϊ����ʱ����δ������Ϊ����ʱ�����Ѵ���,Ϊ0ʱ������
	virtual void __cdecl GetALLStopLPOrder(IDataContainer &Datas, const char * UserNo = "", const char* SCode = "", TDirect direct = dBoth, int NoTrig = 0) = 0;

	//���ָ��keyID��ֹ��
	//���������
	//pItem��ֹ��ָ�룬��û�ҵ���Ӧֹ���򷵻�Ϊ��ָ��nullptr
	//���������
	//KeyID���ؼ���
	virtual void __cdecl GetSingleStopLPOrder(const TStopLPitem* pItem, const unsigned int KeyID) = 0;
};


#endif