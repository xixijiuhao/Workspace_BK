#pragma once

//�µ�����

typedef int		CM_TYPE;
#define	CM_NORMAL		0x00000000	//��ͨ�µ�
#define	CM_BATCHQTY		0x00000001	//�����µ�
#define	CM_BATCHCOUNT	0x00000002	//�����µ�
#define	CM_DRAWORDER	0x00000004	//�����µ�
#define	CM_STOPORDER	0x00000008	//ֹ���µ�
#define	CM_ENQUIRY		0x00000010	//ѯ���µ�

typedef int		CT_SUBPARAM;
#define	CT_NONE			0x00000000	//��
#define	CT_COVEROPEN	0x00000001	//ƽ��
#define	CT_RECOVE		0x00000002	//׷��

//ģ��ID
typedef int		CM_SUBID;
#define	AssistFillID	0			//�����
#define	DealOrderID		1			//�µ�����
#define	ExpandInfoID	2			//��չ����
#define	FastInfoID		3			//�����µ�
#define	BatchInfoID		4			//����

//����� �۸�ʽ
typedef int		CM_FillOrderPriceMode;
#define fopm_Last				0	//���³���
#define fopm_Follow				1	//���̳���
#define fopm_Work				2	//�ҵ�����
#define fopm_Nodefault			3	//��Ĭ��

//����� ��������
typedef int		CM_FillOrderTrigCondition;
#define foc_Loss				0	//ֹ��ģʽ
#define foc_Profit				1	//ֹӯģʽ

//����� ������ʽ
typedef int		CM_FillOrderTrigMode;
#define fot_Last				0	//���¼�
#define fot_Buy					1	//���
#define fot_Sell				2	//����

//�ֲ���ʾ ��ʾ��ʽ
typedef int    CM_FillOrderShowPosType;
#define fosp_Single				0	//����
#define fosp_All				1	//ȫ��

//�ֲ���ʾ ��ʾ��ʽ
typedef int    CM_ChaseType;
#define ct_Match				0	//�Լ۸�
#define ct_Work					1	//�ŶӸ�
#define ct_Chase				2	//����׷

//������Ϣ
struct BatchCountsInfo
{
	unsigned long int	uiCounts;	//�µ�����
	double				dPriceInc;	//�۸�����
	int					nQtyInc;	//��������
};

//�������
struct CM_PARAM
{
	CM_TYPE			cmParam;		//����
	CT_SUBPARAM		ctParam;		//����
	BatchCountsInfo bcinfo;			//����
};

//����������
class IVecInt
{
public:
	virtual unsigned int __cdecl size() = 0;
	virtual int	 __cdecl at(unsigned int) = 0;
	virtual void __cdecl clear() = 0;
	virtual void __cdecl push_back(int data) = 0;
};

enum ErrorLevelCommon
{
	EL_None,		//��ȷ
	EL_Error,		//����
	EL_Warn			//����
};

struct ErrorReflectInfo
{
	ErrorLevelCommon		nErrorLevel;	//���󼶱� ��ȷ0 ����1 ����2
	wchar_t					error_text[101];//������Ϣ
};

struct sFastInfo	//�����µ���Ϣ
{
	bool bcondition;			//�Ƿ�ѡ������
	bool bFastOrder;			//�Ƿ�ѡ�п���
	bool bBatchQuantities;		//�Ƿ�ѡ������
	bool bBackHand;				//�Ƿ�ѡ��ƽ��
	bool bBatchTimes;			//�Ƿ�ѡ������
	bool bAddOne;				//�Ƿ�ѡ��T+1
	bool bRecove;				//�Ƿ�ѡ��׷��

	char sFastUserNo[21];		//�����ʽ��˺� ���������˺�
	char sFastCode[51];			//���ٽ��׺�Լ
	char sFastSign[21];			//���ٷ�������ʶ
	double  dQtyRatio;			//��������

	//����
	char cTrigMode;				//������ʽ
	char cTrigCondition;		//��������
	double dTrigPrice;			//�����۸�

	//����
	int		uiQtyInc;			//��������
	double  dPriceInc;			//�۸�����
	unsigned long int  uiOrderTimes;	//�µ�����

	TValidType	ValidType;		//��Ч����
	char		ValidTime[21];	//��Ч����ʱ��(GTD�����ʹ��)
};

//�������Ϣ
struct C_FillOrderAssistInfo
{
	unsigned int				uSize;						//�ṹ��С

	//��������Ϣ
	CM_FillOrderTrigMode		uiTrigMode;					//����� ������ʽ 0���� 1��� 2����
	CM_FillOrderTrigCondition	uiTrigCondition;			//��������	0ֹ��ģʽ	1ֹӯģʽ
	CM_FillOrderPriceMode		uiTrigPriceMode;			//�����۸�	0���³��� 1���̳��� 2�ҵ����� 3��Ĭ��
	int							uiTrigOverPoint;			//�����۸񳬼۵���
	bool						bTrigSynQuote;				//�����۸�����ͬ��ˢ��

	bool						bBackMatchUserNo;			//�ʽ��˺ŷ���ƥ��
	unsigned int				uiPricePos;					//�۸���λ��
	bool						bShowPosition;				//��ʾ�������
	CM_FillOrderShowPosType		uiShowPosType;				//�ֲ���ʾ����

	bool						bShowAccountNo;				//״̬���ʽ��˺���Ϣ��ʾ
};
//�µ�������Ϣ
struct C_OrderDealInfo
{
	unsigned int				uSize;							//�ṹ��С

	bool						bOpenAutoCancel;				//�����Զ�����
	bool						bCoverAutoCancel;				//ƽ���Զ����� ��ʶ

	bool						bShowTipSpreadTrig;				//˫���Ŷӵ������ñ�ί�� �޸�Ϊ �������µ��ﵽ��������������ʾ0418
	bool						bShowTipCancelOrder;			//��������ȷ����Ϣ

	bool						bPositionCoverTip;				//�ֲ��б�ƽ�ֲ�����ȷ��
};
//�µ�������Ϣ
struct C_OrderReturnInfo
{
	unsigned int				uSize;							//�ṹ��С

	bool						bClearDirect;					//�µ����������
	bool						bValidTypeGFD;					//�µ�����Ч������Ϊ������Ч
	bool						bOrderTypeLimit;				//�µ��󶨵�������Ϊ�޼�
	bool						bCancelForceCover;				//�µ���ȡ��ǿƽ
	bool						bCancelAddOne;					//�µ���ȡ���۽���T+1
};
//������Ϣ
struct C_FastOrderInfo
{
	unsigned int				uSize;							//�ṹ��С
	double						dChangePoint;					//���ʱ䶯����
};
//������Ϣ
struct BatchGroupInfo
{
	unsigned int				uSize;							//�ṹ��С

	char						BatchGroupNo[11];				//���������	
	char						BatchGroupName[21];				//����������
	char						BaseUser[21];					//������׼�˺� / �׸����ʺ� 
};
struct C_BatchGroupInfo
{
	unsigned int				uMember;						//�������

	BatchGroupInfo*				pBatchGroupInfo;				//������Ϣ
};
//���Ϣ
struct C_FillOrderInfo
{
	char	sUserNo[21];
	char	sCode[101];
	int		nQtyType;	//0 ʹ��nQty   1�����ϴ�ί������  2Ĭ���µ���
	int		nQty;
	double	dPrice;
	char	cDirect;
	char	cOffset;
	char	cStrategy;
	int		nOpenQty;
	int		nCoverQty;
	int		nClickType;//0 ����		1 ˫��
};
//�������Ϣ
struct C_ExcInfo 
{
	CM_FillOrderPriceMode	PriceMode;	//���۷�ʽ 
	int						nOverPoint;	//���۵���
};