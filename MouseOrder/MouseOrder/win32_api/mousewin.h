#ifndef _MOUSE_WIN_
#define _MOUSE_WIN_

//////////////////////////////////////////////////////////////////////////ģ��
#define  _MouseOrder				"MouseOrder"
#define  _CommonModule				"CommonModule"
//////////////////////////////////////////////////////////////////////////��Ϣ��������  
#define  W_CSTYLE_MODIFY			"ORDER_MODIFY"			//�޸Ķ���
#define  W_CSTYLE_MODIFY_BATCH		"ORDER_MODIFY_BATCH"	//�����޸�
#define  W_BN_CLICK					"Bn_Click"				//����
#define  W_BN_DBCLICK				"Bn_DbClick"			//˫��
#define  W_HOLD_OPTION				"Hold_Option"			//�ֲ���Ȩ
#define  W_ON_MENUITEM              "On_MenuItem"

#define  W_SETCONTRACT				"SetContract"	//�����Լ�ı�
#define	 W_LButtonDown				"LButtonDown"	//�������
#define	 W_LButtonDbClk				"LButtonDbClk"	//δʹ��
#define	 W_VK_UP					"VK_UP"			//����������
#define	 W_VK_DOWN					"VK_DOWN"		//����������

#define  W_SRC_GRID					"grid"			//���鱨��
#define  W_SRC_PANEL				"panel"			//�̿�
#define  W_SRC_KLINE				"kline"			//K��
//////////////////////////////////////////////////////////////////////////������Ϣ
#define  W_USER_CHANGED				"UserNo_Update"	//�˺�����
#define  W_USER_LOGIN				"TradeLogin"	//�˺ŵ�¼
//////////////////////////////////////////////////////////////////////////ͨ����Ϣ
#define	 W_MODIFY_RECOVER			"MODIFYRECOVER"	//�ĵ�����ָ�
//////////////////////////////////////////////////////////////////////////
#define	 _FastCheck					"FastCheck"		//���ٹ�ѡ
#define	 _AutoCover					"AutoCover"		//�Զ���ƽ
#define	 _CoverMode					"CoverMode"		//ƽ�ַ�ʽ
#define	 _AddOne					"AddOne"		//�۽���T+1
#define	 _ChaseOrder				"ChaseOrder"	//׷��
#define	 _ExchangeSupport			"ExchangeSupport"	//������֧��
#define	 _MouseHead					"MouseHead"		//��ͷ
#define	 _ChildHead					"ChildHead"		//���� ����-��� | �޼�ֹ��-ֹ��-��ɽ-Ӱ�� | �Զ���-������-��-����-Ӧ��-ѯ��-��Ȩ-��Ȩ | �ͻ���ֹ��-ͬ������-�Ⱥ����� 
#define	 _Advanced					"Advanced"		//�����߼�
#define	 _HeadItems					"HeadItems"		//��ͷ������
#define	 _RightCancel				"RightCancel"   //������ �Ҽ�����
#define  _CoverOpen                 "CoverOpen"     //ƽ�� ����
#define  _BatchCondition            "Condition"     //���� ����

//��Ȩ
#define  _OptionSync				"OptionSync"	//��Ȩ���Բ�����������Ϣ��Action��
//��������
#define  _BackDirect				"BackDirect"	//���� "BackDirect&��Ȩ����"
#define  _Reset						"Reset"			//���� "Reset"
#define  _AddTarget					"AddTarget"		//���ӱ�� "AddTarget&��Ȩ����&��������"
#define  _DeleteTarget				"DeleteTarget"	//ɾ����� "AddTarget&��Ȩ����"

		
//�༭����������
#define MQty_Limit			6			//ί������ �������� �����С��
#define MQty_Max			1000000
#define MQty_Min			1

#define MPrice_Limit		10			//ί�м� ������ ֹ��ֹӯ��
#define MPrice_Max			1000000000
#define MPrice_Min			-100000000

#define MPoint_Limit		3			//����
#define MPoint_Max			999
#define MPoint_Min			-99

#define NNoteLen			50			//��ע��������

//��������ǩ
enum MouseHeadInfo
{
	MHI_Normal,		//��ͨ
	MHI_Advanced,	//�߼�
	MHI_Strategys,	//����
	MHI_BatchOrder,	//����
};
//��ͨ��ǩ
enum ChildHeadNormal
{
	CHN_Vert,		//����
	CHN_Ladder,		//���
	CHN_Option,		//��Ȩ
};
//�߼���ǩ 
enum ChildHeadAdvanced
{
	CHA_Condition,	//����		������ CTP		
	CHA_PreOrder,	//��		������
	CHA_AutoOrder,	//�Զ���	������
	CHA_Swap,		//����		����
	CHA_LimtStop,	//�޼�ֹ��	������ 
	CHA_Stop,		//ֹ��
	CHA_Iceberg,	//��ɽ
	CHA_Shadow,		//Ӱ��
	CHA_Enquiry,	//ѯ��
	CHA_Offer,		//Ӧ��
	CHA_ExecAban,	//����Ȩ
	CHA_SpreadApply,//��������
	CHA_HedgeApply,	//�ױ�����
    CHA_AutoClose,  //�ԶԳ�����(��Ȩǰ��Ȩ����Ȩ���ڻ�)
	CHA_END,
};
//���Ա�ǩ
enum ChildHeadStrategys
{
	CHS_PCStop,		//�ͻ���ֹ��
	CHS_SyncSpread,	//��������
	CHS_SequSpread,	//�Ⱥ�����
};
//�����̱�ʶ
enum InnerForeign
{
	IF_Inner,		//����
	IF_Foreign		//����
};
//�����ֶν��� ����������� "contractid=ZCE|F|CF|901;field=113;src=grid" 
enum ID_QUOTE_INFO
{
	ID_CODE,		//��Լ
	ID_FIELD,		//�����
    ID_SRC,			//��Դ
    ID_LEVEL		//���(�̿���Ч src=panel)
};

typedef char MPriceType;
#define mptOrder	'O'		//ί�м�
#define mptTrig		'T'		//������
#define mptLoss		'L'		//ֹ���
#define mptProfit	'P'		//ֹӯ��

typedef char MErrorType;	//�۸�������
#define metMinTick	'M'		//��С�䶯��
#define metEmpty	'E'		//��
#define metNoNegtive 'N'	//��֧�ָ���
#define metZero		'Z'		//0

#define WM_BATCHORDERCHANGE		6000	//���������޸�
#define WM_ONQUOTE				6001	//����ص� ת��Ϣ����
#define WM_ONPOSITION			6002	//�ֲֻص� 
#define WM_ONCODELISTRETURN		6003	//��Լ�б���س���
#define WM_ONRESETUSERNO		6004	//��ʼ����ɡ����� �����˻���Ϣ
#define WM_PRICE_UPDOWN			6005	//�۸� �����л�
#define WM_QTY_UPDOWN			6006	//���� �����л�
#define WM_DEALQUOTECOMMON		6010	//��������������Ϣ
#define WM_DEALPOSCOMMON		6011	//����ֲ�������Ϣ
#define WM_DEALORDERCOMMON		6012	//����ί��������Ϣ
#define WM_DEALMODIFYRECOVER	6013	//����ĵ�����ָ���Ϣ
#define WM_DEALTRADABLEQTY		6014	//����Գ彨����ϢF
#define WM_DEALPREFREEZE		6015	//����Ԥ������ϢF
#define WM_DEALKLINECONTRACT	6016	//K���л���Ϣ
#define WM_DEALOPTIONSTATEGY	6017	//��Ȩ���
#define WM_ONQUOTETARGET		6018	//��Ȩ�������ص� ת��Ϣ����
#define WM_ONEXEOPTION			6019	//��Ȩ ����Ȩ
#define WM_DEALSPREADCOMMON		6020	//��������������Ϣ

enum EN_AREATYPE
{
	Area_Head = 0,
	Area_UserNo,					//�ʽ��˺�			�˺� �˺��� ���
	Area_Commodity,					//Ʒ�ֺ�Լ			��Լ ��Լ��
    Area_Spread_Code,               //������Լ
	Area_OrderQty,					//ί������			���� ��Ч���� �������� ��۳ֲ�
	Area_ValidType,					//��Ч����			//����
	Area_OrderQtyRate,				//��������			//����
	Area_TrigPrice,					//�����۸�			//���� ֹ�� �޼�ֹ��
	Area_OrderPrice,				//ί�м۸�			����һ������
	Area_TimePick,					//����ʱ��			������Чʱ��
	Area_SellPrice,					//�����۸�			
	Area_EnquiryNo,					//ѯ�ۺ�			
	Area_LocalStop,					//����ֹ��			//���ͻ�
	Area_MaxQty,					//�������			//��ɽ
	Area_MinQty,					//��С����
	Area_Note,						//��ע				//����Ա+����
	Area_FastOrder,					//����				//���ͻ�
	Area_AutoDirect,				//�Զ���ƽ			//����
	Area_Force,						//ǿƽ				//����Ա
	Area_AddOne,					//T+1				//�۽���
	Area_CloseDay,					//ƽ��				//������
	Area_OpenContinue,				//��������			//����
	Area_CmbFlag,					//���������ʶ		//����
	Area_AssistOrder,				//׷��				//����
	Area_Advanced,					//�߼�				//����
	Area_TimerCheck,				//ʱ������			//����
    Area_RightCancel,               //�Ҽ�����           //���
    Area_CoverOpen,                 //ƽ�� ����          //����
    Area_Condition,                 //���� ����
    Area_ExeAutoClose,              //��Ȩ ��Ȩ���ԶԳ�
    Area_MovePosition,              //�Ʋ�
	Area_Picture,					//������ť			�ļ�-����-����
	Area_List,						//��۰�
	Area_Option,					//��Ȩ��
	Area_Ok,						//ȷ��				//
	Area_Modify,					//�޸�				//�޸Ķ��� �����޸�
	Area_Cancel,					//ȡ��
	Area_End						//
};

enum ControlID
{
	//ѡ��ͷ
	CI_Head_Button = 0,
	CI_Head_Search,

	//�ʽ��˺�
	CI_UserNoBatch_Static,
	CI_UserNo_Static,
	CI_UserNo_Static2,
	CI_UserNo_Edit,
	CI_UserNo_Combox,
	CI_UserNo_Lock,
	CI_UserName_Static,
	CI_UserNo_StaticA,
	//�����˺�
	
	CI_UserNo_Edit2, 
	CI_UserNo_Edit3,
	CI_UserNo_Edit4,

	CI_UserNo_Combox2,
	CI_UserNo_Sync,	
    CI_UserNo_Combox3,
    CI_UserNo_Combox4,

	//��Լ
	CI_ContractNo_Static,
	CI_ContractNo_Edit,
	CI_ContractNo_Lock,
	CI_ContractNo_Edit_Win,

    //������Լ 
	CI_Spread_Static,
	CI_Spread_Edit,

	//ί������
	CI_OrderQty_Static,
	CI_OrderQty_Edit,
	CI_OrderQty_Spin,
    //�������� ����
    CI_OrderRatio_Static,

	//��ͷ��ͷ
	CI_OrderLongShort_Static,
    //��Ч����
	CI_ValidStyle_Button,

	//��������
	CI_OrderQtyRate_Static,
	CI_OrderQtyRate_Edit,
	CI_OrderQtyRate_Spin,

	//ί�м۸�
	CI_OrderPrice_Static,
	CI_OrderPrice_Edit,
	CI_OrderPrice_Spin,
	CI_OrderPrice_Static_Cal,
	CI_OrderPrice_Edit_Mole,
	CI_OrderPrice_Static_Div,
	CI_OrderPrice_Edit_Dem,
	//���۵���
	CI_OverPoint_Static,
	CI_OverPoint_Edit,
	CI_OverPoint_Spin,
	CI_OverPoint_Edit_Win,

	CI_SpreadSwitch_Static,
	//��������
	CI_SpreadTrigMode_Combox,
	CI_SpreadTrigMode_Combox2,
	//��������ʱ��
	CI_SpreadBeignTime_Static,
	CI_SpreadBeginTime_Picker,
    CI_SpreadEndTime_Static,
    CI_SpreadEndTime_Picker,

	//�����۸�
	CI_BuyPrice_Static,
	CI_SellPrice_Static,
	CI_SellPrice_Edit,
	CI_SellPrice_Spin,
	CI_SellPrice_Static_Cal,
	CI_SellPrice_Edit_Mole,
	CI_SellPrice_Static_Div,
	CI_SellPrice_Edit_Dem,
	
	//ѯ�ۺ�
	CI_EnquriyNo_Static,
	CI_EnquriyNo_Edit,

	//����ֹ��
	CI_StopLoss_Static,
	CI_StopLoss_Edit,
	CI_StopLoss_Spin,
	CI_StopLoss_Static_Cal,
	CI_StopLoss_Edit_Mole,
	CI_StopLoss_Static_Div,
	CI_StopLoss_Edit_Dem,

	CI_StopProfit_Static,
	CI_StopProfit_Edit,
	CI_StopProfit_Spin,
	CI_StopProfit_Static_Cal,
	CI_StopProfit_Edit_Mole,
	CI_StopProfit_Static_Div,
	CI_StopProfit_Edit_Dem,

	//����µ���
	CI_MaxQty_Static,
	CI_MaxQty_Edit,
	CI_MaxQty_Spin,

	//��С�µ���
	CI_MinQty_Static,
	CI_MinQty_Edit,
	CI_MinQty_Spin,
	
	//��������
	CI_OrderSellPrice_Static,
	CI_OrderBuyPrice_Static,

	//�����۸�
	CI_TriggerPrice_Static,
	CI_TriggerPrice_Edit,
	CI_TriggerPrice_Spin,
	CI_TriggerPrice_Static_Cal,
	CI_TriggerPrice_Edit_Mole,
	CI_TriggerPrice_Static_Div,
	CI_TriggerPrice_Edit_Dem,

	//������ ������ʽ
	CI_TrigMode_Combox,
	CI_TrigMode_Combox2,
	CI_TrigMode_Static2,

	//��ע��Ϣ
	CI_NoteInfo_Static,
	CI_NoteInfo_Edit,

	//�Զ���ƽ ǿƽ T+1 ƽ��
	CI_AutoDirect_Check,
	CI_ForceClose_Check,
	CI_AddOne_Check,
	CI_CloseDay_Check,
	CI_FastOrder_Check,
	CI_AssistOrder_Check,
	CI_Advanced_Check,
	CI_TimeValid_Check,
	CI_OpenContinue_Check,
    CI_Right_Cancel_Check,
    CI_CoverOpen_Check,
    CI_Condition_Check,
    CI_ExeAutoClose_Check,      //��Ȩ���ԶԳ�
    CI_MovePosition_Check,

	CI_CmbFlag_Static,
	//���� ����
	CI_OrderBtn,

	//���� ���
	CI_OrderList,

	//�޸� ȡ��
	CI_Ok_Button,
	CI_Modify_Button,
	CI_Cancel_Button,

	//��Ȩ ��ʵֵ ������ ��ʾ
	CI_ShowTip_Option,

	//����Ȩ��
	CI_CanExcute_Option,

	//��ʾ��Ϣ
	CI_ShowTip_Static,

	//����������
	CI_Mouse_End,
};
//������Ϣ
struct sAreaField
{
	int		nID;		//������ EN_AREATYPE
	bool	bShow;		//��ʾ�� ���
	bool    bSubShow;	//������
	sAreaField(){
		nID = 0;
		bShow = false;
		bSubShow = false;
	}
};
//�û���Ϣ
struct sUserInfor
{
	char sUserNo[21];	//�˺�
	char sUserName[21];	//���
	char sSign[21];		//��������ʶ
	char cUserType;		//�������
	sUserInfor()
	{
		memset(this, 0, sizeof(sUserInfor));
		cUserType = uiUser;
	}
};
//��Ȩ��ʵֵ
struct TOptionRealValue
{
	TOptionType		cOptionType;
	double			dStrikePrice;
	wchar_t			cRealValue[21];
	TOptionRealValue()
	{
		memset(this, 0, sizeof(TOptionRealValue));
		cOptionType = otNone;
	}
};
//��ͷ��Ϣ
struct  TClickHeadInfo
{
	int nCurHeadType;
	int nCurChildType;
	TClickHeadInfo()
	{
		memset(this, 0, sizeof(TClickHeadInfo));
	}
};
//�洢��Ϣ
struct THeadCurInfo
{
	int				nSize;
	TClickHeadInfo	tHeadInfo[N_Search_Size];
	THeadCurInfo()
	{
		memset(this, 0, sizeof(THeadCurInfo));
	}
};
//�µ�������Ա����ֶ� �����ڿ���� 
struct WinProperty
{
	bool bCheckFast;		//���ٹ�ѡ
	bool bCheckAutoCOver;	//�Զ���ƽ��ѡ
	char cCoverMode;		//ƽ�ַ�ʽ��������ʼ���������̲��� �������̲��֣�
	bool bAddOne;			//�۽���T+1
	bool bChaseOrder;		//׷��
	bool bAdvanced;			//�����߼�
	bool bExchangeSup;		//������֧�� ��ʽ ���ʽ
	int	 nMouseHead;		//MouseHeadInfo
	int	 nChildHead;		//ChildHeadInfo
	char HeadItemsInfo[21];	//��ͷ����
    bool bRightCancel;      //�Ҽ�����
    bool bCoverOpen;        //ƽ�� ����
    bool bCondition;        //���� ����
    int nOrderListStyle;    //�����ʽ

	WinProperty()
	{
		memset(this, 0, sizeof(WinProperty));
		cCoverMode = oCover;//Ĭ�����ֿ�ƽ ����
		bExchangeSup = true;
        bRightCancel = true;
        bCoverOpen = false;
        bCondition = false;
	}
};
//�����µ���
class CMousewin : public TIxFrm, public TradeSpiEx, public ISQuoteDataSpi, public IOperator, 
                  public ICommonInteract, public ICommonInteractFast, public IStrategySpi, public IOrderConfig
{
public:
	CMousewin(WinProperty wProprety);
	virtual ~CMousewin();
	void Create(LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle = 0);
	//////////////////////////////////////////////////////////////////////////ϵͳ֪ͨ��Ϣ������
	virtual void __cdecl ModifyOrder(const uint OrderID, bool bBatch = false);						//�ĵ� 
	virtual void __cdecl SetModifyRecover();														//�ĵ��ָ�������ĵ�����ͬ���ָ�ʱ���ã�

	virtual void __cdecl SetCodeFromQuote(const char * sKey, const char* cType);					//˫�� ���� �����
	virtual void __cdecl SetCodeFromPosTotal(const char * sKey, const char* cType);					//����ֲֺϼ�
	virtual void __cdecl SetCodeFromPosSub(const char * sKey, const char* cType);					//����ֲ���ϸ
	virtual void __cdecl SetCodeFromOrder(const char *sOrderID, const char* cType);					//˫�� ���� ί���	
	virtual void __cdecl SetCodeFromSpread(const char *sOrderID, const char* cType);				//˫�� ���� �����	
	virtual void __cdecl SetCodeFromQuoteKline(const char *sContent);								//����K���л�
	virtual void __cdecl SetCodeFromOption(const char *sContent);									//��Ȩ���
	virtual void __cdecl SetExecFromPosTotal(const char * sKey);									//����Ȩ
public:
	//////////////////////////////////////////////////////////////////////////ITradeSpi ���׻ص�
	virtual void __cdecl OnDisconnect(const TSocketDisconnectReason ErrorCode, const TLogin * pLogin);//����
	virtual void __cdecl OnInitProgress(const int ErrorCode, const TradeInitProgress CurrProgress
		, const TLoginBase * pLoginBase);															//���׳�ʼ�����
	virtual void __cdecl OnMatch(const TMatch * pMatch);											//�ɽ��ر� ˢ�ֲ֣����˴� �ֲ�ȫˢ ���ܼ�سֲ֣�
	virtual void __cdecl OnOrder(const TOrder * pOrder);											//ί�лر�
	virtual void __cdecl OnPreFrozen(const TPreFreeze * pPreFreeze);								//Ԥ����ر�
	virtual void __cdecl OnTradableQty(const TTradableQty * pTradableQty);							//�Գ彨�ֻر�
public:
	//////////////////////////////////////////////////////////////////////////����ص� 	
	virtual void __stdcall OnQuote(const SContract* cont, const SQuoteUpdate* quote);				//��ͨ��������	
	virtual void __stdcall OnSpreadQuote(const SSpreadContract* scont);								//������������
public:
	//////////////////////////////////////////////////////////////////////////ICommonInteract ͨ�ûص�
	virtual void __cdecl OnCommonCfgChanged(CM_SUBID PartID, char* pdata);
	virtual bool __cdecl GetFastInfo(sFastInfo& sInfo);												//�����µ���Ϣ��ȡ
	virtual void __cdecl OnFastError(const ErrorReflectInfo* eInfo);								//�����µ�������Ϣ����
	virtual void __cdecl OnFastOrderInfo(const TSendOrder* TSOrder, double dRatio);					//�����µ��������Ϣ
	virtual void __cdecl OnFastStateInfo(int nstate);												//���ٹ�ѡ״̬	1��ѡ 0δ��ѡ
public:
	//////////////////////////////////////////////////////////////////////////IStrategyTrade ���Իص�
	virtual void __cdecl OnStopInfoToPosition(const TPosition *p){};								//Ӯ����Ϣ���µ��ֲֺϼ��� ��ʾ
	virtual void __cdecl OnStopInfoToOrder(){};														//Ӯ����Ϣ���µ�ί���� ��ʾ
	virtual void __cdecl OnStopInfoUpdate(bool bShwoUpdate);										//���ؼ������� �д�ȷ����Ϣ
	virtual void __cdecl OnStrategyNotice(const TStrategyOrder *pdata){};							//������״̬�仯֪ͨ NULLʱ��Ҫ���»�ȡ
	virtual void __cdecl OnPositionNotice(const TPosition *pdata){};								//�ֱֲ仯֪ͨ NULLʱ��Ҫ���»�ȡ
	virtual void __cdecl OnStrategyWorkState(const StrategyWorkState stste){};						//������ ����״̬�仯֪ͨ
	virtual void __cdecl OnSpreadInfoNotice(const TSpreadOrder *pspread){};							//�������ݸ���
	virtual void __cdecl OnAssistInfoRef(uint OrderIdOld, uint OrderIdNew) {};
public:
	//////////////////////////////////////////////////////////////////////////IOperator
	virtual void	SetValidType(const wchar_t* sVData);								//������Ч����
	virtual void	SetMaxMinQty(const char* sQty);										//���������С�µ���
	virtual bool	SendOrder(MsgOrderInfo *msg);										//���ö�����Ϣ ����
	virtual void	SetContractNo(const char* sCode, CodeSetType csType = cstChange);	//���ú�Լ
	virtual void	SetPriceData(const char* sdata);									//���ü۸� ί�� ���� ����
	virtual bool	SetHeadData(const wchar_t* sPType,bool bHead=true);					//����������ͷ��Ϣ
	virtual bool	SetSelect(const wchar_t* sSelect, int nRef);						//���ò��ԣ����� �Զ� �񵥣�
	virtual bool	TableButtonToEdit();												//Tab�л�
	virtual bool	CxGetWindowText(MH_TYPE m_type, int& nSize, wchar_t* wstr, int nMaxLen = LEN_RET);
public:
    /////IOrderConfig 
    virtual void OnLadderStyleChanged();			//����µ����øı�
public:
	//////////////////////////////////////////////////////////////////////////Tab Enter
	BOOL DealTabKey(HWND hwnd, int nID);			//Tab������
	BOOL DealReturnKey(HWND hwnd, int nID);			//Enter������
public:
	bool TablesEditToButton();						//tab�л����µ���ť

	bool IsLadderLayOut();							//�Ƿ���
	bool IsCheckOpenContinues();					//�Ƿ�ѡ��������
	bool IsOptionStrategyLayOut();					//�Ƿ���Ȩ����
    bool IsAutoCloseLayOut();                       //�Ƿ��ԶԳ�����

	bool LadderSendOrder(ListOrderInfo &OrderInfo);	//��۷���

	bool OptionSendOrder(TSendOrder &OrderInfo);	//��Ȩ���Է���

	bool TradeLoginLinkage();						//������Ϣ ������¼����

	void GetSign(char(&sign)[21]) { memcpy(sign, m_sSign[0].c_str(),sizeof(sign)); };			//��ȡ��������ʶ

	void UpdateSelectInfo(TMouPriceTypeStrW str[N_Search_Size], int nSize, bool bClearSpecial);	//������ͷ

	void OnSizeHeadWidth();							//�̶��п�ȱ仯����

	void GetPopTopLeftAxis(const int nWidth,int &left, int &nTop);//��ȡ�������Ͻ�����

	void UpdateExchangeSupport(bool bCheck);		//���½�����֧�ֹ�ѡ��

	void GetProperty(char(&cProperty)[MAX_PATH]);

    bool GetOrderUser(TMouUserNoStrA(&UserNo), bool bShowMsg = true);   //��ȡ�ʽ��˻�  ��Ȩ���㱣֤����Ҫ��ȡuserno
public:
	map<int, WNDPROC> m_gTabs;			//Table�л����ڴ������

	SOFCommodityInf m_CommodityInfo;	//Ʒ����Ϣ
	
	C_FillOrderInfo m_FillOrderInfo;	//���Ϣ ͨ�÷���

	string			m_sOrderInfo;		//���Ϣ ί��ʹ��

	string			m_sSpreadInfo;		//���Ϣ ����ʹ��
private:
	virtual LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);	//���ڴ������

	wstring		GetModulePathEx();						//����·��
private:
	//////////////////////////////////////////////////////////////////////////������Ϣ����
	void OnCreateWindow();								//���ڴ�����Ϣ��Ӧ

	void OnNotify(WPARAM wParam, LPARAM lParam);		//֪ͨ��Ϣ���� spin���� 

	void OnCommond(WPARAM wParam, LPARAM lParam);		//�¼�����

	void OnSize(WPARAM wParam, LPARAM lParam);			//��ק����

	void OnPaint(WPARAM wParam, LPARAM lParam);			//�Ի�

	void OnDestroy();									//�������� �������

	void OnNcDestory();									//�ͷ�

	void OnDrawItem(WPARAM wParam, LPARAM lParam);		//Combox�ػ�

	void OnMeasureItem(WPARAM wParam, LPARAM lParam);	//Combox�ػ�

	void OnMouseWheel(WPARAM wParam, LPARAM lParam);	//������Ϣ �༭�򣨼۸� ���� ��Լ�����б�

	void OnMouseMove();									//����ƶ�

	void OnMouseLeave();								//����Ƴ�

	void OnMouseHover();								//����

	void OnLButtonDown(WPARAM wParam, LPARAM lParam);	//�������

private:
	//////////////////////////////////////////////////////////////////////////�ĵ� ��Ȩ ��Ȩ ѯ��
	void OnOkSendOrder();								//�ĵ�����
	
	void OnModifySendOrder();							//�ĵ�����

	void OnCancel();									//ȡ��

	void OnOK();										//ȷ��
	//////////////////////////////////////////////////////////////////////////�ӿؼ������¼�
	void OnButtonSearch();								//����
	void OnButtonUserLock();							//�˺���
	void OnButtonUserSync();							//�˺�����
	void OnButtonValidStyle();							//��Ч����
	void UpdatePriceStyleControl();						//�۸����� �ؼ�״̬����
	void OnButtonContractLock();						//��Լ��
	void OnButtonCalFlag(int nID);						//�����

	void OnButtonAutoDirect();							//�Զ���ƽ����
	void OnButtonFastOrder();							//����
	void OnButtonSpreadAdvanced();						//�߼� ����
	void OnButtonSpreadTimer();							//�߼� ���� ʱ��
	void OnButtonQuotePrice(int nID);					//������� ��Լ������Ϣ ��дί�м۸�
    void OnCheckRightCancel();                          //�Ҽ�����

	void OnSpinDeltaUpDownEx(int nDelta, int nID, int nMoleID);		//�۸���������
	void OnSpinDeltaUpDownNormal(int nDelta, int nID, int nMax = MQty_Max, int nMin = MQty_Min);				//������������
	void OnSpinDeltaUpDownRate(int nDelta, int nID);				//����������������

	void OnWheelCodeList(int nID, int nDelta);			//��Լ�б���Ӧ������Ӧ
	void OnCodeReturn(int nID);							//��Լ�б���Ӧ�س���ť

	void OnClickUserStatic();							//�˺�ͬ����Ϣ

	void OnKillFocusUserNo(int nID);					//�ʽ��˻��༭��ʧȥ���㴦��
	void OnSelChangeUserNo(int nID);					//�ʽ��˻��༭����Ϣ�ı䴦��
	void OnEditChangeUserNo(int nID,bool bSync = false);//�ʽ��˻��༭����Ϣ�ı䴦�� bSync�Ƿ���������仯

	void OnKillFocusCode(int nID = CI_ContractNo_Edit);	//��Լ����ʧȥ����
	void OnSelChangeCode(int nID = CI_ContractNo_Edit);	//��Լ���뷢���仯
	void UpdateCodeEdit(int nID = CI_ContractNo_Edit);	//��Լ��ʾ�仯
    void UpdateSpreadCodeEdit(SSpreadContract &sQuote); //������Լ�仯
	
	void OnSelChangeEditPrice(int nID);					//�۸�䶯����
	void OnSetFocusEditPrice(int nID);					//�۸�༭��õ����㴦��

	void OnSelChangeEditPoint();						//���۵����䶯����

	void OnKillFocusOrderQty();							//ί������ʧȥ����
	void OnSetFocusOrderQty();							//ί�������õ�����
	void OnUpdateOrderQty();							//ί����������

	void OnEditChangeNoteInfo();						//��ע��Ϣ
	//////////////////////////////////////////////////////////////////////////
	void CreateControlWnd();							//���ڿؼ�����

	void SubClassWindow();								//���ڿؼ����໯

	inline void SubClass(int nID);						//���໯����

	void EnableWndStaticOwnerDraw(int nID, BOOL bEnable);//�ؼ�״̬ �仯 ˢ��

	void CreateStaticControl(int nID, const wchar_t* text, DWORD type, int left, int top, int nwidth, int nheight);
	void CreateEditControl(int nID, const wchar_t* text, DWORD type, int left, int top, int nwidth, int nheight);
	void CreateComboxControl(int nID, const wchar_t* text, DWORD type, int left, int top, int nwidth, int nheight);
	void CreateButtonControl(int nID, const wchar_t* text, DWORD type, int left, int top, int nwidth, int nheight);
	void CreateSpinControl(int nID, const wchar_t* text, DWORD type, int left, int top, int nwidth, int nheight);

	void SetIdWithWnd();								//�� Id ����
	void SetSpinSize();									//���� spin�ؼ���С
	void SetEditLen();									//���ñ༭�����ݳ���
	void SetControlFont();								//���ÿؼ�����
	void SetRedrawWnd();								//���ÿؼ��ػ�
	void SetPopListContent();							//���õ�����������
public:
	//////////////////////////////////////////////////////////////////////////����ѡ���
	void OnPopCodeSelectWnd(int nID = CI_ContractNo_Edit);								//��Լ����õ�����

	void OnPopPriceSelectWnd(int nID);						//�۸�

	void OnPopQtySelectWnd(int nID);						//ί������ �����С����
private:
	//////////////////////////////////////////////////////////////////////////����
	void LayOut();															//���²���

	void UpdateAreaInfo();													//����������Ϣ

	void SetAreaVisible(int nID, bool bshow,bool bshowsub = false);			//����������ʾ��Ϣ
	
	void InitOrderTypeData();												//��ʼ��������Ϣ
	
	void MoveArea_Head(int &nCurHeight, bool bshow);
	void MoveArea_UserNo(int &nCurHeight, bool bshow, bool bshowsub = false);
	void MoveArea_Commodity(int &nCurHeight, bool bshow, bool bshowsub = false);
    void MoveArea_SpreadCode(int &nCurHeight, bool bshow);
	void MoveArea_OrderQty(int &nCurHeight, bool bshow);
	void MoveArea_ValidType(bool bshow);
	void MoveArea_CmbFlag(int &nCurHeight,bool bshow, bool &bdiv);
	void MoveArea_OrderQtyRate(int &nCurHeight, bool bshow);
	void MoveArea_OrderPrice(int &nCurHeight, bool bshow, bool bshowsub = false);
	void MoveArea_TimePicker(int &nCurHeight, bool bshow);
	void MoveArea_SellPrice(int &nCurHeight, bool bshow);
	void MoveArea_Enquriy(int &nCurHeight, bool bshow);
	void MoveArea_LocalStop(int &nCurHeight, bool bshow);
	void MoveArea_MaxQty(int &nCurHeight, bool bshow);
	void MoveArea_MinQty(int &nCurHeight, bool bshow);
	void MoveArea_TrigPrice(int &nCurHeight, bool bshow, bool bshowsub = false);
	void MoveArea_Note(int &nCurHeight, bool bshow);
	void MoveArea_Check(int nID,int &nCurHeight, int &nCurWidth, bool bshow, bool &bdiv);
	void MoveArea_Picture(int &nCurHeight, bool bshow, bool bdiv);
	void MoveArea_List(int &nCurHeight, bool bshow, bool bdiv);
	void MoveArea_Option(int &nCurHeight, bool bshow, bool bdiv);
	void MoveArea_Ok(int &nCurHeight, bool bshow);
	void MoveArea_Modify(int &nCurHeight,int &nCurWidth, bool bshow);
	void MoveArea_Cancel(int &nCurHeight, bool bshow);
	
	void UpdateAreaInfoNormal();		//������ͨ��
	void UpdateAreaInfoAdvanced();		//���¸߼���
	void UpdateAreaInfoStartegy();		//���²�����

	void InitTotalInfo();				//��ʼ�� ��ͷ����
	void UpdateSearchLayOut();			//��ʾ����£���½���� ����״̬��
	void UpdatePropertyToHeadItem();	//���±������� �̶���
	void UpdatePropertyCurHead();		//���±������� ѡ����
	void UpdateCurHeadStr();			//����ѡ����

	bool GetShowItemByStr(TClickHeadInfo& item, const TMouPriceTypeStrW str);
	bool GetShowStrByItem(const TClickHeadInfo& item, TMouPriceTypeStrW str);
	bool UpdateShowItemByOrderType(char cOrderType, char cStrategyType = stNone);
	
	bool ResizeCtrlWindowWidth(HWND hwnd, int nWidth);						//����仯����

	RECT MoveCtrlWindow(HWND hwnd, int &nHeightStart);						//�����ƶ���׼�ؼ�����
		
	RECT MoveCtrlWindow(HWND hwnd, int &nHeightStart, int &nHeightCenter);	//�����ƶ���׼�ؼ�����

	RECT MoveCtrlRowWindow(HWND hwnd, int nHeightCenter);					//�����ƶ���ͬ�䶯�ؼ�����

	RECT MoveCtrlColWindow(HWND hwnd, int &nWidthStart);					//�����ƶ���ͬ�䶯�ؼ�����

	RECT MoveCtrlColWindowEx(HWND hwnd, const int nWidthMove);				//�����ƶ���ͬ�䶯�ؼ�����

	void SetWindowCtrlEnableOrDisable(bool bEnable = true);					//�ؼ���Ч�Կ���

	void UpdateCheckState();												//���¿��ٹ�ѡ

	void UpdateModifyRecover();												//���¸ĵ��ָ�

	void UpdateFocus(char cDirect, char cOffset);							//���½���		

	void UpdatePropertyControl();											//���±������� �ؼ�״̬

	void UpdatePropertyCfg();												//���� �Զ���ѡ���Ա���

	void SetFastControlState();												//���ÿ���״̬

	void SetPriceControlState();											//���ü۸�ؼ�״̬

	void ResetFastCheck();													//���ٹ�ѡ�ָ�

	void ResetPriceLevel();													//ί�м۸�ָ�

	void ClearPosition();													//��ճֲ���ʾ

	OrderButtonType GetModeType();											//��ȡ�����̷��

	bool IsCodeOfDCE();														//�Ƿ���������

	bool IsSpreadLayOut();													//�Ƿ���������

	bool IsSwapLayOut();													//�Ƿ񻥻�����

	bool IsOfferLayOut();													//�Ƿ�Ӧ�۲���

	bool IsStopLayOut();													//�Ƿ�ֹ�𲼾�

	bool IsOptionLayOut();													//��Ȩ����
	
	bool IsOptionExAbnLayOut();												//��Ȩ ��Ȩ��Ȩ
	
	bool IsBatchLayOut();													//��������

	bool IsConditionLayout();												//��������

	bool IsSpreadApplyLayOut();												//�������벼��
	
	bool IsHedgeApplyLayOut();												//�ױ����벼��

	bool IsInnerCommodity();												//�Ƿ����̺�Լ

	bool IsUserNoSync();													//�˺�����

	bool IsUserNoLock();													//�˺�����

	bool IsSpreadCodeAdd(const char* str);									//������Լ����

	bool IsSpreadTriggered();												//ͬ������ ���мۣ���������

	bool CheckSameCode(const char* sCode,bool bSpread = false);				//������ú�Լ�Ƿ���ͬ				

	void SetCreateFlag(bool bWinCreated){ m_bCreateComplete = bWinCreated; };//����������ɱ�ʶ

	bool GetCreateFlag(){ return m_bCreateComplete; };						//��ȡ���ڴ�����ʶ

	//////////////////////////////////////////////////////////////////////////���ø���	
	void OnDealAssistFill(const C_FillOrderAssistInfo* pdata);		//�����

	void OnDealOrderDeal(const C_OrderDealInfo* pdata);				//��������

	void OnDealOrderReturn(const C_OrderReturnInfo* pdata);			//�µ���չ

	void OnDealFastOrderInfo(const C_FastOrderInfo* pdata);			//�����µ�

	void OnDealBatchGroupInfo(const C_BatchGroupInfo* pdata);		//�����µ�

	void OnDealBatchOrderChange();									//���������޸�

	////////////////////////////////////////////////////////////////////////////���� ����ˢ�»ص����� ||���� �ֲָ��»ص�����	
	void OnDealQuote();												//�����������

	void OnDealQuoteOptionTarget();									//������Ȩ���������� ��Ȩ��ʵֵ

	void OnDealPosition();											//����ֲָ���

	void OnDealPreFreeze(int nDirect);								//����Ԥ����

	void OnDealTradableQty(int nDirect);							//����ɿ��ɳ�

	//////////////////////////////////////////////////////////////////////////��������
	void OnDealQuoteCommon();										//������������

	void OnDealPositionCommon();									//����ί�� �ֲ� ����

	void OnDealOrderCommon();										//����ί�� ί�� ����

	void OnDealSpreadCommon();										//�������� ί�� ����

	void OnDealModifyRecover();										//����ĵ�����ָ�

	void OnDealKLineContract();										//K���л�����

	void OnDealOptionStartegy();									//��Ȩ��ϴ���

	void OnDealOptionExe();											//��Ȩ����Ȩ

	void FillBaseDataToDlg(const TSendOrder * pSendOrder);			//�ĵ������

	//////////////////////////////////////////////////////////////////////////���� ���� ���ݸ���
	bool UpdateUserInfo(string sUser, int index = 0);				//�����û������Ϣ
	//bool UpdateUserInfo2(string sUser);								//�����û������Ϣ

	void CodeUpdateCommodityInfo();									//���º�ԼƷ����Ϣ �����л�
	//void UpdateSpreadCommodityInfo(SSpreadRateModeType RateMode, SOFCommodityInf &sinf,	SOFCommodityInf &sinf2);//��������Ʒ����Ϣ

	void UpdateQuoteContract(const SContractNoType sQuote,CodeSetType csType);					//���������Լ
	
	bool UpdateOptionTarget();										//������Ȩ��ĺ�Լ

	void UpdatePositionQty();										//���³ֲ���ʾ
	void UpdatePositionQtySpread();									//���������ֲ���ʾ
	void UpdatePositionQtySwap();									//���»����ֲ���ʾ
	void UpdatePositionQtyNormal();									//������ͨ�ֲ���ʾ

	void UpdateBtnOpenCover(const TContractKey& key, uint nLong, uint nShort);
	void UpdateCanExecuteQty(char cCommodityType,uint nPos);		//���¿���Ȩ��	����Ա����ʾ�������ҵ�Ч�ʵͣ�
	void UpdateStaticPosInfo(const TContractKey &key,uint nLong,uint nShort);	//����//��Ȩ ��Ȩ Ӧ�� ��� //�ֲ����� �۸�

	void UpdateBtnCanOpen(const TContractKey* key = NULL);			//��ť �ɿ���ƽ��ʾ����

	void UpdateConditionData(char cDirect);							//��������

	void UpdatePriceRange();										//���¼۸�ѡ��Χ �ǵ�ͣ ����
	
	void UpdateShowText(int nEditID,bool setfocus);										//���¼۸�ѡ��Χ �ǵ�ͣ ����
	
	bool GetTradeCommodityInf(TContractKey &key, SOFCommodityInf &CommodityInfo);							//�ؼ��ָ���Ʒ����Ϣ

	void GetPositionVol(const char * UserNo, const TContractKey * ContractKey
		, uint &LongCount, uint &ShortCount, uint &DayLongCount, uint &DayShortCount);						//��ȡ�ֲ�

	uint GetParOrderExcute();																				//��ȡ��Ȩ�ҵ���

	bool GetConditionInfo(char &cMode, char &cCondition, double &dPrice, bool bShowMsg = true);				//��ȡ��������Ϣ

	bool GetSpreadPriceInfo(TDLPriceMode &cMode, char &cCondition, double &dPrice, bool bShowMsg = true);	//��ȡ�����۸���Ϣ

	bool GetDefaultTrigPrice(const SContractNoType ContractId, TDirect tdirect, double& dPrice);			//��ȡ���������۸�

	//////////////////////////////////////////////////////////////////////////�Գ彨�� �ɿ���ƽ ��Ҫ��Ϣ
	void ShowCanOrderVol(const TTradableQty * pTradableQty);		//��ʾ����

	void ShowOrderFrozen(const TPreFreeze * pPreFreeze);			//��ʾԤ���ᱣ֤��

	bool GetAvailableMoney(double &dAvailabe);						//�õ������ʽ�

	bool IsVaild(const TSendOrder *pOrder, double dPrice);			//��ȡ�õ������Ƿ���Ч

	void FormatDisplayQtyAndFreeze(bool bQty, bool bFreeze);		//��ʽ���ɿ���ƽ �� Ԥ���ᱣ֤�� //����Լ1��ʾ��Ч

	wstring FormatUserSummary(const TUserInfo* tInfo);				//��ʽ���û���Ҫ��Ϣ

	////////////////////////////////////////////////////////////////////// ���浽����
	void FillAndSendOrder(TSendOrder& sendorder, bool bGetPrice = true,bool bGetCode = true);			//������Ϣ��д���� ����  ����۲���ȡ�۸�  ��Ȩ����ȡ�۸� ����ȡ��Լ��
	
	bool FillOrder(TSendOrder& sOrder, bool bGetPrice = true, bool bGetCode = true);					//������Ϣ��д����
	bool FillSpreadOrder(TSpreadSendOrder& sOrder);														//������Ϣ��д������

	bool ReSetUserNos();															//������ʾ���ʽ��˻���Ϣ �����µ����� ���׶Ͽ� ���׳�ʼ���ɹ�

	//bool GetOrderUser(TMouUserNoStrA(&UserNo), bool bShowMsg = true);														//��ȡ					�ʽ��˻�
	bool GetOrderUser2(TMouUserNoStrA(&UserNo), int index, bool bShowMsg = true);														//��ȡ					�ʽ��˻�

	bool GetOrderCode(TContractKey& key, string &sCode, int nID, bool bShowMsg = true);										//��ȡ					���׺�Լ
	bool GetSpreadOrderCode(SSpreadContract &sQuote, TSpreadSendOrder& sOrder, bool bShowMsg = true);					        //��ȡ					������Լ

	bool GetDoublePrice(int nID, int nMoleID, double &dPrice, bool bShowMsg = false, MPriceType type = mptOrder);			//��ȡ�۸�
	bool GetSpreadTrigPrice(double &dPrice, bool bShowMsg = false);			//��ȡ�۸�
	
	bool SetOrderPrice(int nID, int nMoleID, double dPrice);																//�۸���ʾ������
	bool GetOrderPrice(int nID, int nMoleID, double &dPrice, TDirect td, bool bShowMsg = true, MPriceType type = mptOrder);	//��ȡ					ί�м۸�
	bool GetDoubleOrderPrice(double& dPriceBuy, double& dPriceSell);														//��ȡ					����ί�м۸�

	bool CheckOrderPrice(double dPrice, bool bShowMsg = true, MPriceType type = mptOrder);									//�жϼ۸���Ч��
	void PopPriceCheckMsg(MPriceType ptype = mptOrder, MErrorType etype = metMinTick);										//�۸���Ϣ��ʾ����
	void GetErrorText(TMouPriceExceed &ErrorText, int nID, const SOFCommodityInf &sInfo, double dp1, double dp2);			//��ʽ��������Ϣ
	void GetSpeExchangeText(TMouPriceExceed &ErrorText, int nID, const SOFCommodityInf &sInfo, const wchar_t *wExchange, double dp);			//��ʽ��������Ϣ

	void SetOrderQty(int nID, int nQty);																					//������ʾ������
	bool GetOrderQty(int nID, uint &nQty, bool bShowMsg = true);															//��ȡ					ί������	

	bool GetOrderValidStyle(TSendOrder &sOrder);																			//��ȡ					��Ч����

	bool GetOrderType(TSendOrder &sOrder);																					//��ȡ					��������

	bool CheckOrderTypeSupport(TSendOrder &sOrder);																			//��� ����֧��

	bool CheckIsOption(char ctype, bool bShowMsg = true);																	//�ж��Ƿ���Ȩ��Լ

	bool CorrectOrderPrice(TSendOrder &sOrder);																				//У���۸�

	bool CheckSpreadOrderPriceTrigState(TSpreadSendOrder &Sspread);															//�����۸� �������

	void SetShowTips(const wchar_t* wtips);																					//������ʾ��Ϣ

	void SetStopPreHangOrder(const TSendOrder &TOrderInfo, TVecInt& vOrderID);												//����ί��ֹ��

	bool DoubleSameOrExceed(double dFirst, double dSecond, bool bFirst = true);												//���ڵ���
	
	bool DoubleExceed(double dFirst, double dSecond, bool bFirst = true);													//���� С��

	bool GetStopData(TDirect direct, double dprice);																		//��ȡֹ������

	void ResetStopContent();																								//���ֹ��۸�	

	bool GetActiveTime(TSpreadSendOrder &Sspread);																			//��ȡ��Чʱ��

    bool UpdateOffsetOfMovePosition(TSpreadSendOrder& sOrder);                                                              //�Ʋָ��¿�ƽ

    bool IsOneOfComboCode(std::string strCode, std::string strCombCode);            //�Ƿ�����Ϻ�Լ����ɺ�Լ
	//////////////////////////////////////////////////////////////////////////�Զ��崰��
	HINSTANCE			m_hInstance;

	vector<sAreaField>	m_AreaInfo;			//������Ϣ����

	HWND				m_hControl[CI_Mouse_End - CI_Head_Button + 1];				//�ؼ��������

	CMouseHeadWin		m_cHeadWin;			//����ͷ
	CPopWinSearch		m_cPopSearch;

	CxStaticEx			m_StaticPriceSwitch;//�۸�-�޼� �м� ���� �ҵ� ����
	CxStaticEx			m_StaticSpCtSwitch;//�۸�-������ ��������

	CxMousePos			m_StaticPosInfo;	//�ֲ���Ϣ

	CxStatic			m_StaticUserNo;		//�˺� ����
    //CxStatic			m_StaticUserNo2;	//�˺� ����
	CMouseComboBox		m_ComboxUserNo;		//�˺� ���
	CMouseComboBox		m_ComboxUserNo2;	//�˺�
    CMouseComboBox		m_ComboxUserNo3;	//�˺�
    CMouseComboBox		m_ComboxUserNo4;	//�˺�
	CMouseStatic		m_StaticUserName;	//����
	CLockBtn			m_LockUserNoBtn;	//�˻�����
	CLockBtn			m_SyncUserNoBtn;	//�˻�����
	CLockBtn			m_SearchBtn;		//����

	CxStatic			m_StaticCode;		//��Լ
	CxStatic			m_StaticCode2;		//��Լ
	CxEditEx			m_EditExContract;	//��Լ�༭��
	CxEditEx			m_EditExOverPoint;	//���۱༭��
	CPopWinCode			m_cCodeWin;			//����Լ���� 30k
	CPopWinCodeList		m_CodeList;			//����Լ�б� 16k child
	//CPopWinCodeQuote	m_cCodeSpreadWin;	//������Լ����
	CLockBtn			m_LockCodeBtn;		//��Լ����

	CxDrawButton		m_BtnValidType;		//��Ч����
	CPopWinDate			m_cValidTypeWin;	//��Ч���� 4k

	CPopWinQty			m_cOrderQtyWin;		//ί������ 5k child

	CPopWinPrice		m_cOrderPriceWin;	//�۸� 5k

	COrderButton		m_cOrderBtn;		//�µ���ť

	COrderList			m_OrderList;		//����б�

	COrderOption		m_OptionView;		//��Ȩ

	CPopWinTip			m_cStopTip;			//ֹ��ֹӯȷ����Ϣ

	CxComboxEx			m_ComboxMode;		//������ʽ
	CxComboxEx			m_ComboxCompare;	//��������

	CxComboxEx			m_ComboxSpreadMode;	//������ʽ
	CxComboxEx			m_ComboxSpreadCompare;	//��������

	CxComboxTime		m_CombBeginTime;	//��ʼʱ��
    CxComboxTime        m_CombEndTime;      //����ʱ��

	CxDrawButton		m_BtnOk;			//ȷ����ť
	CxDrawButton		m_BtnModify;		//�޸İ�ť
	CxDrawButton		m_BtnCancel;		//ȡ����ť

	//////////////////////////////////////////////////////////////////////////�ؼ��ػ�
	CxSpin m_SpinQty;	//3k
	CxSpin m_SpinQtyRate;
	CxSpin m_SpinPrice;
	CxSpin m_SpinSellPrice;
	CxSpin m_LossPrice;
	CxSpin m_ProfitPrice;
	CxSpin m_SpinMaxQty;
	CxSpin m_SpinMinQty;
	CxSpin m_SpinTrigPrice;
	CxSpin m_SpinOverPoint;
	
	CxEdit m_EditContractNo2;
	CxEdit m_EditOrderQty;
	CxEdit m_EditOrderQtyRate;
	CxEdit m_EditMaxQty;
	CxEdit m_EditMinQty;
	CxEdit m_EditEnquiry;
	CxEdit m_EditNote;
	CxEdit m_EditOrderPriceMole;
	CxEdit m_EditSellPriceMole;
	CxEdit m_EditTrigPriceMole;
	CxEdit m_EditOverPoint;
	CxEdit m_EditLossMole;
	CxEdit m_EditProfitMole;

	CxCheck m_CheckAutoDirect;
	CxCheck m_CheckForceClose;
	CxCheck m_CheckAddOne;
	CxCheck m_CheckCloseDay;
	CxCheck m_CheckFastOrder;
	CxCheck m_CheckAssistOrder;
	CxCheck m_CheckAdvanced;
	CxCheck m_CheckTimePicker;
	CxCheck m_CheckOpenContinue;
    CxCheck m_CheckRightCancel;
    CxCheck m_CheckCoverOpen;                       //ƽ�� ����
    CxCheck m_CheckCondition;                       //���� ����
    CxCheck m_CheckAutoClose;                       //��Ȩ ��Ȩ���ԶԳ�
    CxCheck m_CheckMovePosition;                    //�Ʋ�

	CMousePriceEdit m_EditOrderPrice;
	CMousePriceEdit m_EditSellPrice;
	CMousePriceEdit m_EditTrigPrice;
	CMousePriceEdit m_EditLoss;
	CMousePriceEdit m_EditProfit;

	//////////////////////////////////////////////////////////////////////////����
	wstring					m_strPath;				//·��

	sMousewinCfg			m_MouseCfg;				//������Ϣ

	C_FillOrderAssistInfo	m_FillOrderAssist;		//����� ����������

	C_OrderDealInfo			m_OrderDealInfo;		//�µ�����

	C_OrderReturnInfo		m_OrderReturn;			//�µ����� 	

	C_FastOrderInfo			m_fastOrderInfo;		//����

	WinProperty				m_wProperty;			//��������

	//SSpreadContract			m_SpreadParam;			//��������		

	//////////////////////////////////////////////////////////////////////////����
	TClickHeadInfo		m_nCurClickItem;		//��ǰѡ����

	//��ˢ
	HBRUSH		m_hStaticBKBrush;				//��̬�ı�������ˢ
	//����
	wstring		m_sValidtype;					//��Ч�����ַ���

	TMouPriceTypeStrW	m_sPricetype;			//�۸���Ϣ
	TMouPriceTypeStrW	m_sPriceSpreadtype;		//�۸���Ϣ

	string		m_sUserNo[4];				    //�ʽ��˺�
	//string		m_sUserNo2;						//�ʽ��˺�
	string		m_sUserName;					//����
	string		m_sSign[4];						//��������ʶ
	//string		m_sSign2;						//��������ʶ
	char		m_cUserType;					//��½���� ����Ա�����ͻ��� �������˻�ʱ�л��˻��任������ʾǿƽ��Ϣ

	string		m_sQuoteContractId;				//��Լ���� �����飩
	string		m_sOptionTarget;				//��Ȩ��� �����飩
	string      m_sCode;						//��Լ���� �����ף�
    string      m_sSpreadCode;                  //������Լ���� (����)
	string		m_sQuoteKline;					//K�ߺ�Լ  �����飩
	string		m_sQuoteOption;					//��Ȩ  �����飩
	string		m_sExeOption;					//��Ȩ  ������Ȩ��

	TOptionRealValue m_OptionInfo;				//��Ȩ��Ϣ

	double		m_sPriceLast;					//���¼�
	double		m_sPriceMax;					//��ͣ��
	double		m_sPriceMin;					//��ͣ��
	double		m_sPriceBid;					//���
	double		m_sPriceAsk;					//����

	//����
	int         m_nQtySelectID;					//�����仯 ����
	bool		m_bCanShowPop;					//�õ������Ƿ���ʾ����

	int			m_nPriceSelectID;				//�۸�仯����
	//�����Ϣ
	bool		m_bAutoCover;					//�Զ���ƽ

	bool		m_bModifyOrderOper;				//�����޸ı��

	bool		m_bBatchModifyOrderOper;		//�����޸ı��

	bool		m_bBatchInfoModify;				//���������޸� �е�����ѡ� ��Ҫ����

	bool		m_bTradeLogined;				//�Ƿ���Ա��¼

	bool		m_bCreateComplete;				//���ڴ�����ɱ�ʶ

	bool        m_bShowFuzzyQuery;				//��Լ�仯ˢ��
	void		SetFuzzyQuery(bool bShow){ m_bShowFuzzyQuery = bShow; }

	bool		m_bCanStrategyTrade;				//�Ƿ����ֹ��

	bool		m_bCanBatchOrder;				//�Ƿ��������

	bool		m_bStopPreHangOrder;			//����ֹ����

	bool		m_bShowStopSubmit;				//ֹ����Ϣȷ�� ���׳�ʼ����� ��Ҫ��������ȷ��

	int			m_LoginType;
	//����
	bool		m_bTracking;					//�����ٱ�ʶ

	bool		m_bExecute;
    bool        m_bOptionAutoClose;              //��Ȩ��Ȩǰ�ԶԳ�
private:
	map<wstring, TClickHeadInfo> m_TotalItemInfo;

	map<string, sUserInfor>		m_UserInfo;			//�ʽ��˻���Ϣ����

	map<string, char>			m_UserCoverModeMap;	//��½�˻���ƽ�ַ�ʽ �����̲���ʹ��

	map<string, BatchGroupInfo> m_batchGroupVec;	//������Ϣ

	TOrder			m_OrderModifyParam;				//�ĵ�����

	PositionInfo	m_posinfo;						//�ֲ���Ϣ

	double			m_dLossPrice;					//ֹ���

	double			m_dProfitPrice;					//ֹӯ��

	StopType		m_StopType;						//ֹ������

	TMouPriceTypeStrW m_SearchString[CSP_END][N_MAX_CHILD];

	int				m_nSearchSize[CSP_END];
		
	THeadCurInfo	m_HeadCurInfo;					//��ǰ��ͷ��
	//////////////////////////////////////////////////////////////////////////�Գ� ������Ϣ
	CMyLock					m_lock;
	TTradableQty			m_BuyTradableQtytemp;
	TTradableQty			m_SellTradableQtytemp;
	TPreFreeze				m_BuyPreFreezetemp;
	TPreFreeze				m_SellPreFreezetemp;

	TTradableQty			m_LastBuyQty;		//�����ϴεĿɿ���ƽ ��ֹ�ظ���ѯ
	TTradableQty			m_LastSellQty;
	TPreFreeze				m_LastBuyFreeze;	//�����ϴε�ռ�ñ�֤��
	TPreFreeze				m_LastSellFreeze;
	int						m_MaxOrdID;			//ԭʼ������	
    int                     m_ValidTypeCfgValue;//������������Ч����
};

#endif // _MOUSE_WIN_