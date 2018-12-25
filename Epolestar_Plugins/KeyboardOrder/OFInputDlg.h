#pragma once

#define BTNBORDERHEIGHT				8
#define BTNBORDERWIDTH				10
//�༭�����ַ������������ȣ���Ҫ��䵽�༭���л���Ҫ���������϶
#define	EDITBORDERWIDTH				8	
#define EDITBORDERHEIGHT			8
#define WM_QUOTCHG					WM_USER+100
#define WM_OFHOTKEY					WM_USER+101
#define WM_BATCHORDER				WM_USER+102
#define WM_POSITIONSHOW				WM_USER+103
#define WM_CONDITION				WM_USER+104
#define WM_DEALTRADABLEQTY			WM_USER+105
#define WM_DEALPREFREEZE			WM_USER+106
#define WM_DEALQUOTECOMMON			WM_USER+107
#define WM_DEALPOSCOMMON			WM_USER+108
#define WM_DEALORDERCOMMON			WM_USER+109
#define	WM_DEALKLINECONTRACT		WM_USER+110

//�����µ��ȼ�����
#define Vk_Condition	'T'
#define Vk_CloseOpen	'L'
#define Vk_Fast			'K'
#define Vk_BatchTimes	'B'
#define Vk_BatchQty		'U'
#define Vk_OK			'Y'
#define Vk_PreOrder		'P'
//����
#define KeyDirectBuy	'1'
#define KeyDirectSell	'3'
//��ƽ
#define KeyOffsetOpen	'1'
#define KeyOffsetCover	'2'
#define KeyOffsetCoverT	'3'
//��������
#define KeyLimit		'1'
#define KeyMarket		'2'
#define KeyLimitStop	'3'
#define KeyStop			'4'
#define KeyIceberg		'5'
#define KeyGhost		'6'
//����
#define KeyGreater		'1'
#define KeyLesser		'2'
//������ʽ
#define KeyTrigLast		'1'
#define KeyTrigBuy		'2'
#define KeyTrigSell		'3'
//��ť״̬
#define SwitchLevel1	0
#define SwitchLevel2	1

typedef char FPriceType;
#define fptOrder	'O'
#define fptTrig		'T'

#define  _KeyBoardOrder				"KeyBoardOrder"
//////////////////////////////////////////////////////////////////////////ͨ����Ϣ
#define  W_CSTYLE_MODIFY			"ORDER_MODIFY"			//�����޸�
#define  W_CSTYLE_MODIFY_BATCH		"ORDER_MODIFY_BATCH"	//�����޸�
#define  W_CSTYLE0					"LBUTTON_CLICK"    
#define  W_CSTYLE1					"LBUTTON_DBCLICK"  
#define  W_BN_CLICK					"Bn_Click"//"BN_CLICK"
#define  W_BN_DBCLICK				"Bn_DbClick"//"BN_DBCLICK"
#define	 W_USER_CHANGED				"UserNo_Update"	//�˺�����
#define  W_USER_LOGIN				"TradeLogin"	//�˺ŵ�¼
#define  W_KLINE_CHANGED			"KLINE_SETCONTRACT"	//k���л�

#define	 WM_MODIFY_RECOVER			"MODIFYRECOVER"	//�ĵ�����ָ�

#define  W_SETCONTRACT				"SetContract"
#define	 W_LButtonDown				"LButtonDown"
#define	 W_LButtonDbClk				"LButtonDbClk"
#define	 W_VK_UP					"VK_UP"
#define	 W_VK_DOWN					"VK_DOWN"

#define  W_SRC_GRID					"grid"
#define  W_SRC_PANEL				"panel"
#define  W_SRC_KLINE				"kline"

#define	 ConditionCheck				"ConditionCheck"
#define	 BackHandCheck				"BackHandCheck"
#define	 FastCheck					"FastCheck"
#define	 BatchCountCheck			"BatchCountCheck"
#define  AddOne						"AddOne"

struct WinProperty
{
	bool bConditionCheck;
	bool bBackHandCheck;
	bool bFastCheck;
	bool bBatchCountCheck;
	WinProperty()
	{
		memset(this, 0, sizeof(WinProperty));
	}
};
enum ID_QUOTE_INFO
{
	ID_CODE,
	ID_FIELD,
	ID_SRC,
	ID_LEVEL,
};

#define KPRICELENGTH_LIMIT	10
//�۸����� ���ݴ��ж���ȡ������Ŷ���� �����Ŷ�����
enum ePriceType
{
	BUYUSEBUY = 1,//�������
	BUYUSESELL, //��������
	SELLUSEBUY,	//�������
	SELLUSESELL	//��������
};
//�ؼ�����
enum eCtrlType
{
	EDIT_TYPE,
	CHECK_TYPE,
	COMBO_TYPE,
	BTN_TYPE,
	STATIC1_TYPE,
	STATIC2_TYPE
};

class COFInputDlg :public IxDlg, public TradeSpiEx, public ISQuoteDataSpi, public IOperator, public ICommonInteract,public IKeyOrderCfg, public IStrategySpi,public ICommonInteractFast
{
public:
	COFInputDlg(HWND hPrent,WinProperty wProperty);
	~COFInputDlg();
	//������ģ̬�Ի���
	HWND ShowDlg(HINSTANCE hInstance, int idDlg, HWND hWndParent);
	//��ʼ��
	void Init();
public:
	//IQuoteSpi
	virtual void __stdcall OnQuote(const SContract* cont, const SQuoteUpdate* quote);
	virtual void __stdcall OnSpreadQuote(const SSpreadContract* scont){};

	//TraderSpi
	virtual void __cdecl OnOrder(const TOrder * pOrder);
	virtual void __cdecl OnDisconnect(const int ErrorCode, const TLogin * pLogin);
	virtual void __cdecl OnInitProgress(const int ErrorCode, const TradeInitProgress CurrProgress, const TLoginBase * pLoginBase);
	virtual void __cdecl OnPosition(const TPosition * pPosition);						//�ֱֲ仯֪ͨ
	virtual void __cdecl OnLogin(const int ErrorCode, const TLogin * pLogin);
	virtual void __cdecl OnPreFrozen(const TPreFreeze * pPreFreeze);
	virtual void __cdecl OnTradableQty(const TTradableQty * pTradableQty);

	//ICommonCfg
	virtual void __cdecl OnCommonCfgChanged(CM_SUBID PartID, char* cdata);
	virtual bool __cdecl GetFastInfo(sFastInfo& sInfo);
	virtual void __cdecl OnFastError(const ErrorReflectInfo* eInfo);
	virtual void __cdecl OnFastOrderInfo(const TSendOrder* TSOrder, double dRatio);
	virtual void __cdecl OnFastStateInfo(int nstate);
	//Ioperator
	virtual void SetPriceType(wstring sPType){};
	virtual void SetValidType(wstring sVData){};
	virtual void SetMaxMinQty(string sQty){};
	virtual void SendOrder(MsgOrderInfo *msg){};
	virtual void SetContractNo(string sCode, bool brepaint = true, bool bnutton = false, bool bgetfreeze = false);
	virtual void SetPriceData(string sdata){};
	//IKeyBoardCfg
	virtual void __cdecl OnKeyBoardCfgChange(const C_KeyBoardOrderCfg* kbcfg);
	virtual void __cdecl OnKeyBoardCfgFontChange(const C_KeyBoardFontInfoCfg* kbcfg);
	virtual void __cdecl OnKeyBoardSeqChange(const C_KeyBoardSeqCfg* kbcfg);
	//IStrategySpi
	virtual void __cdecl OnStrategyNotice(const TStrategyOrder* pdata) {};
	virtual void __cdecl OnPositionNotice(const TPosition* pdata) {};
	virtual void __cdecl OnStrategyWorkState(const StrategyWorkState state) {};
	virtual void __cdecl OnStopInfoToPosition(const TPosition* p) {};
	virtual void __cdecl OnStopInfoToOrder() {};
	virtual void __cdecl OnStopInfoUpdate(bool bShwoUpdate) {};
	virtual void __cdecl OnSpreadInfoNotice(const TSpreadOrder* pspread){};
	virtual void __cdecl OnAssistInfoRef(uint OrderIdOld, uint OrderIdNew);
	//���ø��»ص�
	virtual void __cdecl OnStrategyParamModify(ST_PARAM type) {};
	//
	void SetHotKeyActive(bool bActive);
	bool GetHotKeyActive(){ return  m_bHotKeyAltActive; };

	//
	virtual void __cdecl SetCodeFromQuote(const char * sKey, const char* cType);	//˫�� ���� �����
	virtual void __cdecl SetCodeFromPosTotal(const char * sKey, const char* cType);						//����ֲֺϼ�
	virtual void __cdecl SetCodeFromPosSub(const char * sKey, const char* cType);						//����ֲ���ϸ
	virtual void __cdecl SetCodeFromOrder(const char *sOrderID, const char* cType);						//˫�� ���� ί���	

	void ModifyOrderOper(const TOrder *pOrder, bool bBatch = false);//�ĵ�
	void OnDealModifyRecover();										//�ĵ��ָ�
private:
	//������Ϣ
	INT_PTR DlgProc(UINT message, WPARAM wParam, LPARAM lParam);

	void OFSendOrder(TSendOrder* order);
private:
	//�Ƿ��ʼ�����
	bool m_nInitReady;
	//�Ƿ񼤻�����ݼ�
	bool m_bHotKeyAltActive;
	//�����ϴε�ֵ
	string m_LastBLStr;	
	//�����ϴε�ֵ
	string m_LastVolStr;						
	//ˢ��
	HBRUSH m_ctlBrush;
	//���ٱ���ˢ��
	HBRUSH m_ctlBrushKS;
	//����ˢ��
	HBRUSH m_BkBrushDlg;
	//��ʼ����̬�ռ��ı���ֱ����
	void IniStaticTextVer();
	//����spin
	void SetNumSpin(int ID, int IDSpin, bool bCanNat);
	//��ʼ���Ի���
	BOOL OnInitDialog();
	//����������Ϣ
	BOOL OnCommandCOFInputDlg(WPARAM wParam, LPARAM lParam);
	//����ͨ����Ϣ
	BOOL OnNotifyCOFInputDlg(WPARAM wParam, LPARAM lParam);
	//��������check
	void OnBnClickedCheckTJ();
	//����ƽ��check
	void OnBnClickedCheckPK();
	//�������check
	void OnBnClickedCheckKS();
	//��������check
	void OnBnClickedCheckBatchCount();
	//��������check
	void OnBnClickedCheckBatchOrder();
	//����T1check
	void OnBnClickedCheckT1();
	//����ǿƽcheck
	void OnBnClickedCheckForCover();

	//zyp ��������Ͷ��check
	void OnBnClickedCheckSP();
	//zyp ��������ֵcheck
	void OnBnClickedCheckHG();

	//����ctrlcolor
	HBRUSH OnCtlColor(WPARAM wParam, LPARAM lParam);
	//����ȷ����ť
	void OnBnClickedButtonDoOrder();
	//�����񵥰�ť
	void OnBnClickedButtonPayOrder();
	//�����Զ�����ť
	void OnBnClickedButtonAutoOrder();
	//����˳��ť
	void OnBnClickedButtonSeq();
	//�������밴ť
	void OnBnClickedBtnBuy();
	//����������ť
	void OnBnClickedBtnSell();
	//��������̬�ؼ�����
	void OnStnClickedStaticDirect();
	//��������̬�ؼ���ƽ
	void OnStnClickedStaticOffset();
	//�����򽹵�
	void OnEnSetfocusEditDirect();
	//��ƽ����
	void OnEnSetfocusEditOffset();
	//�����������
	void OnCbnSetfocusComboTirgMode();
	//ί�м۸񽹵�
	void OnEnSetfocusEditOrderPrice();
	//ί����������
	void OnEnSetfocusEditOrderQty();
	//�����۸񽹵�
	void OnEnSetfocusEditTrigPrice();
	//������������
	void OnEnSetfocusEditTrigType();
	//�������ʽ���
	void OnEnSetfocusEditQtyMulti();
	//�˻����ѡ��
	void OnCbnSelChangeUserNo();
	//�����������ѡ��
	void OnCbnSelchangeComboOrdertype();
	//���ö����������ѡ��
	void OnCbnSetfocusComboOrderType();
	//��Ч�������ѡ��
	void OnCbnSelchangeComboValidtype();
	//��Լ�������ݸı�
	void OnEnChangeEditCode();
	//��Լ�����������ʧȥ����
	void OnEnKillFocusEditCode();
	//�����������ݸı�
	void OnEnChangeEditDirect();
	//ί�м۸�spin
	void OnDeltaposSpinOrderPrice(int nDelta);
	//ί������spin
	void OnDeltaposSpinOrderQty(int nDelta);
	//��������spin
	void OnDeltaposSpinQtyMulti(int nDelta);
	//�����۸�spin
	void OnDeltaposSpinTrigPrice(int nDelta);
	//�µ�����spin
	void OnDeltaposSpinQtycount(int nDelta);
	//��������spin
	void OnDeltaposSpinIncreqty(int nDelta);
	//�۸�����spin
	void OnDeltaposSpinIncreprice(int nDelta);
	//�����spin
	void OnDeltaposSpinMaxqty(int nDelta);
	//��С��spin
	void OnDeltaposSpinMinqty(int nDelta);
	//�˻�edit����
	void OnCbnEditchangeComuserno();
	//���ʸ���
	void OnEnUpdateEditQtyMulti();
	//��������
	void OnEnUpdateEditOrderQty();
	//�۸���������
	void OnEnUpdateEditIncreprice();
	//�µ���������
	void OnEnUpdateEditIncreqty();
	//ѡ��Լ��ť
	void OnBnClickedBtnSelcode();
	//���̼۸�
	void OnBnClickedCheckAutoprice();
	//������Ϣ �༭�򣨼۸� ���� ��Լ�����б�
	void OnMouseWheel(WPARAM wParam, LPARAM lParam);
	//�ȼ���Ӧ
	void OnDealHotKey(WPARAM wParam, LPARAM lParam);
	//���������޸�
	void OnBatchOrderInfoCHG();
private:
	//�������Լ��ص���Ϣ
	SOFCommodityInf	m_CommodityInf;	
	//���Ϣ
	C_FillOrderInfo m_FillOrderInfo;
private:
	//�˻�����
	vector<CStringsVec> m_vecUserNo;
	//�˻��ؼ�
	CMultiColComboBox m_UserNO;
	//��ť����
	HFONT m_fontBtn;
	//check����
	HFONT m_fontCheck;
	//��̬�ؼ�1����
	HFONT m_fontStatic1;
	//��̬�ؼ�2����
	HFONT m_fontStatic2;
	//�������
	HFONT m_fontCombo;
	//edit����
	HFONT m_fontEdit;
	//�ֲ���ʾ����
	HFONT m_fontPosition;
private:
	typedef vector<FacadeSetting::OrderField> vecOrderField;
	//�������������
	vector<C_OrderField> m_InputCfgArray;
	//�Ƿ��ܹ�layout
	bool m_bCanLayout;
	//Btnԭʼ���
	int	m_iOrginBtnWidth;	
	//Btn��ԭʼ����Ŀ��
	int	m_iOrginBtnFontWidth;
	//��Լ����༭�򳤶��ַ���
	string	m_CodeLengthStr;					
	//��ȡ�µ���ťԭʼ��С
	void SetOrginBtnWidth();
	//���ÿɼ�
	void SetItemVisible(int id, BOOL bShow);
	//��ȡ�����б��������Ϣ
	void ReadFieldConfig();		
	//�����������ؼ���С
	bool UseFontSetCtrlSize(eCtrlType eType, HFONT &font, SIZE & sz, wstring& str);
	//���ñ༭�����
	bool SetEditCtrlParam(FutureInputArea eInputArea, wstring str, int iEditID, SIZE &sz, bool bExtraWidth = true);		
	//�����������
	bool SetComboCtrlParam(FutureInputArea eInputArea, wstring str, int iComboID, SIZE &sz);		
	//��Ծ�̬�ı�
	bool SetStaticCtrlParam(HFONT &font, wstring str, int iStaticID, SIZE &sz, int *pDefaultMaxWidht = NULL, bool bExtraWidth = true);
	//��԰�ť ��ť��С��Ҫ��ͬ,����0��ȷ�� 1������
	bool SetBtnCtrlParam(wstring str, int iBtnID, SIZE &sz, int *pwidth = NULL);							
	//����check�����
	bool SetCheckCtrlParam(FutureInputArea eInputArea, wstring str, int iCheckID, SIZE &sz);
	bool SetCheckCtrlParam2(wstring str, int iCheckID, SIZE &sz);
	//�������ڿؼ�����
	bool SetDateTimeCtrlParam();
	//�޸Ŀؼ����
	void ModifyCtrlWidth(int id, int iNewWidth);
	//��������
	bool SetCtrlFont();
	//���ÿ��ƴ���layout
	inline void SetCanLayout(bool bCan) { m_bCanLayout = bCan; }
	//��ȡ���ƴ���layout
	inline bool GetCanLayout()			 { return m_bCanLayout; }
	//check����
	void CheckLayout(int id, bool bShow, int height, RECT &ctrlRect);
	//����������
	void TJCheck(C_OrderField &of, RECT &ctrlRect);
	//����״̬
	void UpdateItemStates();
	//�ƶ���̬�ı�
	void MoveStaticCtrl(bool bShow, int idStatic, RECT &ctrlRect);
	//�ƶ��ؼ�
	void MoveCtrl(bool bShow, int id, int iVerInterval1, int height, RECT &ctrlRect, HWND &LastWnd);
	//�ƶ�spin
	void MoveSpinCtrl(bool bShow, int idspin,int idStatic, const RECT &InputRect, RECT &ctrlRect, int &iExtraSpinWidht);
	//�ƶ��������۲���
	void MoveFractionCtrl(bool bShow, int idStatic, int idFraction, int interval, int iVerInterval1, int height, RECT &InputRect, RECT &ctrlRect, HWND &LastWnd);
	//�ƶ���ť���ֲ� ����
	void MoveBtnCtrl(RECT &rectBtn, RECT &InputRect, int interval, HWND&LastWnd, int iStateBarLeft,int &iSeqBtnLeft);
	//�ƶ�״̬��ʾ
	void MoveStatusTip(int iStateBarLeft, int iTempHeight, int iVerInterval2, int iDepoRight);
	//�ƶ��ֲ�
	void MovePosCtrl(int interval, RECT &ctrlRect);
	//����
	int  Layout();
	//��ȡ�ؼ��߶�
	int  FindCtrlHeight(int id);
	//��ȡ�Ի����С
	SIZE GetKbDlgSize();
	//ת����Ļ����
	void ScreenToClient(RECT&rt);
	//�ػ��Ӵ���
	void ReInvalidateCtrl();
	//�޸İ�ť����
	void ModifyBtnName();
	//�޸ľ�̬�ı�����
	void ModifyStaticUserName();
	//�ĵ�����ctrl����
	void EnableModifyOrder(bool bModify);
	//T1
	void EnableT1(const char *szCode);
	//��ʼ�����̼�check
	void IniAutoPriceCheck();
private:
	//�汾���� 0����  1���� 2������
	int	m_iVersionType;
	//�ж�������������
	void LoadVersionType();
private:
	//��ȡcheck�Ƿ�ѡ��
	bool IsCheck(int check);
	//�Ƿ񵥿ͻ�
	inline bool IsOnlyOneUser()	{ return m_vecUserNo.size() <= 1; }
	//�Ƿ���ʾ������ť
	inline bool IsShowBuySellBtn() { return m_KeyBoardSet.uiDoOrderStyle == 1; }
	//�Ƿ���ʾ ����
	inline bool IsShowKS() { return !CConvert::IsTraderLogined(); }
	//�Ƿ���ʾ ����
	inline bool IsShowTJ() { return  (0 == VersionType() || 2 == VersionType()); }
	//�Ƿ���ʾ ƽ��
	inline bool IsShowPK(){ return  (0 == VersionType() || 2 == VersionType()); }
	//�Ƿ���ʾ ����
	inline bool IsShowPC(){ return !CConvert::IsTraderLogined(); }
	//�Ƿ���ʾ ����
	inline bool IsShowPL(){ return (!CConvert::IsTraderLogined() && m_vecUserNo.size() > 1); }
	//�Ƿ���ʾ T+1
	inline bool IsShowT1(){ return /*GetTraderLoginCount() > 0 &&*/ (1 == VersionType() || 2 == VersionType()); }
	//�Ƿ���ʾ ǿƽ
	inline bool IsShowForceCover(){ return CConvert::IsTraderLogined(); }
	//0��ʾ����ʾ 1��ʾ��ʾ
	inline int  IsShowComboType() { return (VersionType() == 1 || VersionType() == 2) || m_KeyBoardSet.bShowOrderValidType; }
	//�Ƿ���ʾ �����۸�
	bool IsShowTrigPrice();
	//�Ƿ���ʾ˳�� �� �Զ�����ť �������ú�xml�ļ� ��ordercfg.ini �������ļ� ���  ���ȼ�ordercfg.ini�ϸ߼�
	inline bool IsShowBtn(){ return (VersionType() == 0 || VersionType() == 2) && 2==m_KeyBoardSet.uiDoOrderStyle; }

	//zyp ����������ֵ
	inline bool IsShowSPAndHG() { return !CConvert::IsTraderLogined(); }
	inline bool IsShowHG() { return !CConvert::IsTraderLogined(); }

public:
	//��ȡ����״̬
	inline bool IsKS()	{ return IsCheck(IDC_CHECK_KS); }
	//�������¼������Ҽ�����tab��
	bool DoLeftRightUpDownKey(char key,bool Combo=false);
	//����ener
	BOOL DisposeEnterMsg(DWORD key);
	//����enter����
	bool DealEnterSendOrder(DWORD key);
private:
	//0��ʾ���� 1��ʾ���� 2=�����̾���
	inline int  VersionType(){ return m_iVersionType; }
public:
	//���ֹ���
	BOOL DoWndWheel(int nID, BOOL bUP);
	//˫��
	BOOL DoWndDbClick(HWND hWnd, LPARAM lparam);
	//����
	BOOL DoWndKeyDown(HWND hWnd, WPARAM wg);
private:
	//���÷������ۺ���ͨ���۹��ֹ�������
	void SetSpinCtrlBuddy(int IDSpin, int ID, int IDFenZi, int IDFenMu);
	//���ü۸����Զ�ѡ��
	void SetPriceSel();			
	//��ʽ��double
	string FormatDoubleValue(int precision, double value, BOOL bZeroEmpty);
	//��ʽ������
	string FormatIntValue(int value, BOOL bZeroEmpty);
	//bDoubleType�Ƿ��Ǵ�С�� ��ί�������Ͳ��ܴ�С��  �������ʿ��Դ�С��
	void UpdateNumEdit(BOOL bUp, int ID, double iChangePoint, bool bCanNat, bool bDoubleType = false);
	//��Լ۸�
	void SpinUpOrDown(int ID, int IDFenzi, BOOL bUp);
	//��Լ۸�����
	void SpinUpOrDown(int IDIncre, BOOL bUp);

private:
	//ί�м۸��Ƿ�����
	BOOL OrderPriceIsCanAble();
	//�����۸��Ƿ�����
	BOOL TriggerPriceIsCanAble();
	//ί�м۸� �����۸� enable 
	void EnablePrice(int ID, int IDAdd, int IDFenzi, int IDChu, int IDFenmu, int IDSpin,BOOL bEanble);
private:
	//�����µ�
	COrderBtn m_BuyBtn;
	COrderBtn m_SellBtn;
	//���ð�ť��� ��ֻ��ʾ�������� �����м۸�
	void SetBuySellBtnStyle(bool bShowPrice);
public:
	struct ItemPos
	{
		//λ������
		int iPosIndex;
		//�����ؼ�ID
		int hID;
	public:
		ItemPos() :iPosIndex(-1), hID(-1){}
	};
	typedef vector<ItemPos> vecItemPos;
	//��������item
	vecItemPos m_vecItemPos;
	//��ʼ���ؼ�˳���ID
	void InitItemPos();
	//������������
	void SetIndexFocus(int index,bool btabling = false,bool bAdd = true);
	//��ȡ��һ������λ������
	int GetLastFocusIndex(int ID);
	//��ȡ��һ������λ������
	int GetNextFocusIndex(int ID);
public:
	//���໯
	void InitCtrl();
	//ί�м۸����໯
	CSubClassEdit m_OrderPriceEdit;	
	//ί�м۸����໯
	CxKeyBoardEdit m_OrderPriceEditFZ;
	//�����۸����໯
	CSubClassEdit m_TriggerPriceEdit;
	//�����۸����໯
	CxKeyBoardEdit m_TriggerPriceEditFZ;
	//��Լ�������໯
	CxKeyBoardEdit m_CodeEdit;
	//�����������໯
	CxKeyBoardEdit m_DirectEdit;
	//��ƽ�������໯
	CxKeyBoardEdit m_OffsetEdit;
	//ί������
	CxKeyBoardEdit m_OrderQtyEdit;
	//��������
	CxKeyBoardEdit m_QtyMultiEdit;
	//����
	CxKeyBoardEdit m_TrigCondiEdit;
	//�����
	CxKeyBoardEdit m_MaxQtyEdit;
	//��С��
	CxKeyBoardEdit m_MinQtyEdit;
	//�µ�����
	CxKeyBoardEdit m_QtyCountEdit;
	//��������
	CxKeyBoardEdit m_IncreQtyEdit;
	//�۸�����
	CxKeyBoardEdit m_IncrePriceEdit;
	//�µ���ť
	CxKeyBoardButton m_OrderButton;
	//�񵥰�ť
	CxKeyBoardButton m_PayOrderButton;
	//�Զ�����ť
	CxKeyBoardButton m_AutoOrderButton;
	//˳��ť
	CxKeyBoardButton m_SeqButton;
	//ѡ��Լ��ť
	CxKeyBoardButton m_SeleCodeButton;
	//�Ǽ۸�edit��
	bool IsPriceEdit(HWND hwnd);
private:
	//�л���������
	BOOL SwitchDirect();
	//�л���ƽ
	BOOL SwitchOffset();
	//�л�����
	BOOL SwitchTJ();
	//��ȡ�м�λ��
	int  GetMiddlePos(int ID);
private:
	//�޸Ŀؼ�����
	void ModifyStyle(HWND hCtrl, DWORD dwRemove, DWORD dwAdd);
	//�޸Ŀؼ�����
	void ModifyCtrlStyle();
	//��ʾ��ʾ
	void ShowTip(const wstring &msg);
private:
	//������״̬
	void UpdateTJState();
	//�������״̬��������
	void FillTConditionByDirect();
	//�����������۸����
	void UpdateTrigPriceFill(string sCode,char cDirect);
private:
	//��ǰѡ�е��˻�
	string m_CurUserNo;
	//��ǰ��¼�˻�sign ����
	string m_CurLoginNo;
	//�����˻���sign
	void UpdateLoginSign(const char *szUserNo, const char *sign);
	//��ȡsign
	void SetLoginSign(char(&Sign)[21]);
	//��ȡsign
	const char * GetLoginSign();
private:
	//�˻�����
	CLockBtn m_LockUserNoBtn;
	//��Լ����
	CLockBtn m_LockCodeBtn;
	//����������ť
	void OnBnClickedBtnLockuserno();
	//����������ť
	void OnBnClickedBtnLockcode();
private:
	//��������ص�
	LRESULT	OnQuotInfo(WPARAM wparam, LPARAM lparam);
	//bDefaultPriceΪ˫��������顢�����µ�����˫�������ۡ�������֮��������������� bDefaultPrice=trueֱ���� =falseʱ����direct==dNoneʱ���� ��=falseʱ�п�����˫�������ۡ������� ��ʱӦ�ø����Ƿ���direct�������۸�
	//bDbClickQuot��˫�����黹��˫�������б�
	void FillKeyBoardOrderDlg(const TSendOrder *pSendOrder, bool bFastOrder, bool bQuot, bool bTradeData, bool bModifyOrder);
	//�Ƿ�LME
	//bool IsLME(SExchangeKey&ExchangeKey);
	//���ĵ�
	void FillModifyOrder(const TSendOrder &SendOrder);
	//���ü۸񵽽���
	void SetPriceToDlg(int ID, int IDMolecules, int IDDenominator, double dInputPriceParma);
	//���ü۸�������ť��
	void SetPriceToBtn(int IDBtn, double dPrice);
	//��ռ۸�
	void EmptyPrice(int ID, int IDMolecules);
	//��Ч�ļ۸�
	inline bool IsValidPrice(char commodityType, double price);
private:
	//���Ĺ�������
	string m_regQuotId;
	//��ȡ������Ϣ���������� ȡ����ȡ�� ȡ����������
	bool GetQuotInfo(const char *szCode, SQuoteSnapShot&QuotInfo);
	//��ȡ������Ϣ���������� ȡ����ȡ�� ȡ���������� ����true��ʾ��ȡ�ɹ� false ��ʾʧ��
	bool GetQuotInfo(const char *szCode, SQuoteSnapShot &, int iTypePrice, int nDefaultPriceOverStep, char Direct, double &dPrice, double dDefaultPrice = 0);
private:
	//������ʽ
	CxComboBox m_TrigModeCmb;
	//��������
	CxComboBox m_OrderTypeCmb;
	//��Ч����
	CxComboBox m_ValidTypeCmb;
	//���ö����������
	void SetOrderTypeCmbAtrribute();
	//������Ч�����������
	void SetValidTypeCmbAtrribute();
	//�µ���ȡ��ǿƽ
	void CancelForceCover();
	//�µ��󶨵����͸�Ϊ�޼� ǰ���Ƕ���������ʾ����
	void ModifyOrderType(char OrderType);
	//�µ��󶨵����͸�Ϊ�޼� ǰ���Ƕ���������ʾ����
	void ModifyValidType(const char *szTime);
	//�µ�����Ч���͸�Ϊ������Ч ǰ������Ч������ʾ����
	void ModifyValidType(char ValidType);
	//�µ���ȡ��T1
	void CancelT1();
private:
	//��һ����䴥���۸�ĺ�Լ ֻ�е�ǰ�����ֵ��ͬ����Ҫ��䴥���۸�
	string m_strLastTriggerFillCode;
	//�����۸��Ƿ��Ѿ������� �������� �õ�����ʱ�Ͳ���Ҫ�������
	bool m_bValidFillTrigger;
	//�Ƿ���Ҫ���������
	bool IsNeedFillTrigger();
	//������Ч��
	void SetValidFillTrigger(bool b){ m_bValidFillTrigger = b; }
	//��ȡ��Ч��
	bool GetValidFillTrigger(){ return m_bValidFillTrigger; }
	//���ú�Լ
	void SetLastTriggerFillCode(const string & s){ m_strLastTriggerFillCode = s; }
	//��ȡ��Լ
	string &GetLastTriggerFillCode(){ return m_strLastTriggerFillCode; }
	//����
	void UpdateFillTriggerValid(const string &strCur);
private:
	//��������Ƿ���Ч Ҳ���Ե�m_vecUserNo�Ƿ���Ч
	bool m_bValidGroup;		
	//��ȡ�˻�����
	bool GetIndexUserNo(const vector<CStringsVec> &vecStr, const string &str, int &index);
	//����˻����
	void SetUser(bool bValidData = false);	
private:
	//��ʾ����
	CCodeNameStatic	m_StaticCodeName;
	//��ʾ�ɿ���ƽ
	CCodeNameStatic	m_StaticCanOpenCover;
	//��ʾ��֤��ռ��
	CCodeNameStatic	m_StaticDeposit;
	//��ͷ�ֲ� ������ʾ��ʽ �����˺���ʾ���߸��ݺ�Լ��ʾ�ֲ���Ϣ
	CDrawStatic	m_BuyPositionStatic;					
	//��ͷ�ֲ� ������ʾ��ʽ �����˺���ʾ���߸��ݺ�Լ��ʾ�ֲ���Ϣ
	CDrawStatic	m_SellPositionStatic;					
	//����Լ��Ч��
	void CheckCodeInput();
	//iType=0������ʾ��Լ�ֲ֣�1�����˻��ֲ�
	void UpdatePositionInf();
	//ע��
	bool RequestCommodity(const char *szCode);
	//����Ʒ����Ϣ
	void UpdateCommodityTick(const char *szCode);
	//���½������ݲ����²���
	bool UpdateTradeLayOut();
private:
	//������Ϣ
	wstring g_FastError;
	//��ȡ������Ϣ
	bool GetDlgCtrlInput(TSendOrder &SendOrder, bool bShowTip = false, bool bShowMsgBox = false, bool bFastOrder = false, char key = '\0', const char * ContractID = NULL);		
	//�õ��û����ж���Ч�� �Ϸ�����ture  bShowTip�Ƿ���ʾ��ʾ��Ϣ Ϊtureʱ�ǵ�����ʾ�����ڽ�������ʾ
	bool GetCtrlUser(char(&UserNo)[21], bool bShowTip = false, bool bShowMsgBox = false);		
	//SContractKey���
	bool GetKeyCode(TContractKey &key, bool bShowTip = false, bool bShowMsgBox = false);	
	//��ȡ��������
	char GetOrderType(char *pType = NULL);
	//��ȡ�������� ���selchange
	char GetOrderTypeEx();
	//��ȡ��Ч���� 
	char GetValidType();
	//��ȡ��Ч���� ���selchange
	char GetValidTypeEx();
	//��ȡ��Ч����
	string GetValidTime();
	//��ȡ��������
	bool GetDirect(char &Direct, bool bShowMsgBox = false, bool bSetFocus = true);
	//��ȡ��ƽ
	bool GetOffset(char &Offset, bool bShowMsgBox = false, bool bSetFoucus = true);
	//��ȡ����
	bool GetCtrlOrderNum(uint & Qty, bool bShowMsgBox = false, bool bSetFocus = true);
	//��ȡ��ɽ���������
	bool GetCtrlMaxNum(uint & Qty, bool bShowMsgBox = false, bool bSetFocus = true);
	//��ȡ��ɽ����С����
	bool GetCtrlMinNum(uint & Qty, bool bShowMsgBox = false, bool bSetFocus = true);
	//��ȡ����ֵ��Ĭ�ϵ�ֵ�����ַ����� ?
	bool GetCtrlPrice(int id, int idMolecular, double &outPrice, bool bShowMsgBox = false, bool bSetFocus = true, FPriceType type = fptOrder);
	//��ȡ����������
	bool GetTJParam(TSendOrder &SendOrder, bool bShowTip = false, bool bShowMsgBox = false, bool bSetFocus = true);
	//��ȡ���δ���
	bool GetPCQtyCount(uint &QtyCount, bool bShowMsgBox = false, bool bSetFocus = true);
	//��ȡ������������
	bool GetPCQtyIncre(int &QtyIncre, bool bShowMsgBox = false, bool bSetFocus = true);
	//��ȡ���μ۸�����
	bool GetPCPriceIncre(double &PriceIncre, bool bShowMsgBox = false, bool bSetFocus = true);
private:
	//ԭʼ������
	int m_MaxOrdID;	
	//�����ϴεĿɿ���ƽ ��ֹ�ظ���ѯ
	TTradableQty m_LastBuyQty;
	//����
	TTradableQty m_LastSellQty;
	//�����ϴε�ռ�ñ�֤��
	TPreFreeze m_LastBuyFreeze;	
	//����
	TPreFreeze m_LastSellFreeze;
	CMyLock					m_lock;
	TTradableQty			m_BuyTradableQtytemp;
	TTradableQty			m_SellTradableQtytemp;
	TPreFreeze				m_BuyPreFreezetemp;
	TPreFreeze				m_SellPreFreezetemp;
private:
	//����Ԥ����
	void OnDealPreFreeze(char cDirect);		
	//����ɿ��ɳ�
	void OnDealTradableQty(char cDirect);				
	//��ȡ���ID ��ʼΪ0����ί���и���
	inline int  GetMaxOrderID()  {return m_MaxOrdID; }
	//��ʾ���ᱣ֤��
	void ShowOrderFrozen(const TPreFreeze * pPreFreeze);
	//��ʾ�ɿ���ƽ
	void ShowCanOrderVol(const TTradableQty * pTradableQty, bool bAutoCover = false);
	//�����ʾ��
	void ClearTipBar();
	//�õ������ʽ�
	bool GetAvailableMoney(double &dAvailabe);
	//��ʽ����֤��
	void FormatStaticDepositDisplay();
	//��ʽ���ɿ���ƽ
	void FormatStaticNumDisplay();
	//��ʽ����Լ
	void FormatStaticCodeDisplay();
	//�ɿ���ƽ����֤��ռ���Ƿ���Ч���ǲ�����Ҫ�������� ifpOrder->OrderQty��ЧΪ�ɿ���ƽ
	bool IsVaild(TSendOrder * pOrder, int AMaxOrdID, double AAvailable);
public:
	//�ȼ��ص�
	void OnHotKeyDown(const bool Alt, const bool Ctrl, const bool Shift, const unsigned char VKey, const bool Exclusive, HWND source);
	//ת��
	bool TransKey(HWND hwnd, WPARAM wParam, LPARAM lParam);
	//��ȡ����
	double GetMultiQty();
	//ί�м۸�Ϊ0���м۵�
	bool IsOrderTypeToMarket(const TSendOrder &SendOrder);
	//������
	inline bool IsSHFE(const char *szExchange);
	//�޸���������������
	void ModifySHFEOrderTyep(TSendOrder &SendOrder);
	//CommitModeӦ����Ϊ�񵥺��Զ���ʹ�õ�
	void DoOrder(char CommitMode, char Direct = dNone);	
	//�µ�֮����� ���� ȡ��ǿƽ ������Ϊ�޼� ��Ч���͵�
	void RecoverState();
private:
	//�Ƿ�ĵ�
	bool m_bModifyOrderOper;
	//�Ƿ������ĵ�
	bool m_bModifyOrderOperBatch;
	//�Ƿ��Ǹĵ�
	inline bool GetModifyOrderOper() { return m_bModifyOrderOper; }
	//���øĵ�
	void SetModifyOrderOper(bool ModifyOrderOper){ m_bModifyOrderOper = ModifyOrderOper; }
	//�ĵ��ṹ
	TOrder m_ModifyOrder;
	//�ĵ�ʱ����ǰ��������ѡ������
	void ResetTJCheckBox(TBool IsRiskOrder, TTradeSection AddOneIsValid);
private:
	//spin���໯
	CxKeyBoardSpin m_OrderPriceSpin;
	CxKeyBoardSpin m_OrderQtySpin;
	CxKeyBoardSpin m_TrigPriceSpin;
	CxKeyBoardSpin m_MaxQtySpin;
	CxKeyBoardSpin m_MinQtySpin;
	CxKeyBoardSpin m_QtyMultiSpin;
	CxKeyBoardSpin m_QtyCountSpin;
	CxKeyBoardSpin m_IncreQtySpin;
	CxKeyBoardSpin m_IncrePriceSpin;
	CxDataCtrl m_ValidDataCtrl;
private:
	//check
	CxKeyBoardCheck m_CheckTJ;
	CxKeyBoardCheck m_CheckPK;
	CxKeyBoardCheck m_CheckKS;
	CxKeyBoardCheck m_CheckPC;
	CxKeyBoardCheck m_CheckPL;
	CxKeyBoardCheck m_CheckT1;
	CxKeyBoardCheck m_CheckQP;
	//zyp ���� Ͷ��/��ֵ
	CxKeyBoardCheck m_CheckHG;  //��ֵ
	CxKeyBoardCheck m_CheckSP;  //Ͷ��

	CSwitchBtn	m_SwitchFollow;			//����
private:
	CPopWinCode m_cCodeWin;				//����Լ����
private:
	//�߳�
	CThread m_thread;
public:
	//�򻯺�Լת��
	void GetContractStr(string &str);
	//�޸�ͨ��˫������ֲֵȷ�ʽ���code������ �����ZCE SR 501תΪSR1501||SR501
	void ModifyContractStr(string&str);
private:
	//ͨ��
	void OnDealQuoteKLine();
	void OnDealQuoteCommon();
	void OnDealPositionOrderCommon();

	bool GetSignByUserNo(string sUser, int& nIndex);

	C_KeyBoardOrderCfg		m_KeyBoardSet;			//�����µ�����

	C_KeyBoardFontInfoCfg	m_OrderFont;			//��������

	C_KeyBoardSeqCfg		m_OrderFiled;			//����ؼ�˳��

	C_FillOrderAssistInfo	m_FillOrderAssist;		//����� ����������

	C_OrderDealInfo			m_OrderDealInfo;		//�µ�����

	C_OrderReturnInfo		m_OrderReturn;			//�µ����� 	

	C_FastOrderInfo			m_fastOrderInfo;		//�������ʸ�������

	map<string, BatchGroupInfo> m_batchGroupVec;	//������Ϣ

	void OnDealAssistFill(const C_FillOrderAssistInfo* pdata);
	void OnDealOrderDeal(const C_OrderDealInfo* pdata);
	void OnDealOrderReturn(const C_OrderReturnInfo* pdata);
	void OnDealFastOrderInfo(const C_FastOrderInfo* pdata);
	void OnDealBatchGroupInfo(const C_BatchGroupInfo* pdata);
private:
	//����
	void SetFastControlState();

	void UpdateAltHotKeyShowState();

	bool TradeLoginLinkage();
private:
	//����K��
	string m_sQuoteKline;												//K�ߺ�Լ
	virtual void __cdecl SetCodeFromQuoteKline(const char *sContent);	//����K���л�
private:
	//���table���뷨����
	bool m_bTabling;

	WinProperty		m_wProperty;

	void UpdatePropertyCfg();

	void UpdatePropertyControl();

	bool m_bLoginTrader;
private:
	//Add by xzc 2018.6.12����
	//���¼��л���Լ
	std::vector<std::string> m_vecUpDownSwitchContract;
	//�Ƿ�����һ��
	std::string GetOneSwitchContract(bool bNext);
	//��Ӧ���¼���������¼���ΪTab�ã���ô�Ͳ��л���Լ
	bool OnKeyUpDown(bool isDown, HWND hFocus);
	//�����û�û�е�¼ʱ���л���GetContractStr����ת�����ɹ���AP810->ZEC AP 810
	void DealUserNotLoginSwitch(std::string & sCode);
	//end
};

 