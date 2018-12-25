#ifndef _ORDER_OPTION_WIN_
#define _ORDER_OPTION_WIN_

#define Nref_Pricetype 0
#define Nref_Validtype 1
#define Nref_OCtype	   2

#define WM_DELOPTIONQUOTE	7000

#define N_OPTION_NONE	 0
#define N_OPTION_STD	 1
#define N_OPTION_STG	 2

enum StrategyGroup
{
	T_Bullish,			//����
	T_NotRise,			//����
	T_DullRiseCall,		//����Call
	T_DullRisePut,		//����Put
	T_Bearish,			//���
	T_NotFall,			//����
	T_DullFallCall,		//�̵�Call
	T_DullFallPut,		//�̵�Put
	T_Dull,				//����
	T_DullStrangle,		//��������
	T_Break,			//ͻ��	
	T_BreakStrangle,	//ͻ�ƣ���	
	T_AutoGroup,		//����
	T_GroupType,		//�龳
	T_END,
};
//�� ����
enum ERowIndex
{
	N_Head_row,
	N_1_row,
	N_2_row,
	N_3_row,
	N_4_row,
	N_5_row,
	N_Target_row,//���
	N_Max_Row	
}; 

//������
enum EColItem
{
	COO_Code,
	COO_Direct,	
	COO_Qty,
	COO_PriceType,
	COO_Price,
	COO_Delta,
	COO_Gamma,
	COO_END
};

//����ؼ�ID
enum OptinControl
{
	OC_PriceSync_Btn,
	OC_QtySync_Btn,
	OC_Price_Edit,
	OC_Price_Spin,
	OC_Qty_Edit,
	OC_Qty_Spin,
	OC_Support_Check,
	OC_AddOne_Check,
	QC_END,
};

//�Ի�ؼ�ID
enum ESpecItem
{
	SPI_Type,
	SPI_Option,
	SPI_BackDirect,
	SPI_AddTarget,
	SPI_Reset,
	SPI_OC,
	SPI_Valid,
	SPI_Order,
    SPI_Info,   /// PreFrozen or Error message
	SPI_END
};

//����
enum EDirectColor
{
	EDC_Buy,
	EDC_Sell,
	EDC_END
};

//���� ����
enum EQuoteTrade
{
	EQT_Quote,
	EQT_Trade,
	EQT_End,
};

//��Ԫ����
struct OCClickItem
{
	int nRow;
	int nCol;
};

//��Ȩ�����Ϣ
struct TOptionInfo 
{
	SContractNoType	Contract;
	TDirect			Direct;
};
//�����Ϣ
struct TOptionTarget
{
	bool			bAdd;					//�Ƿ����
	TDirect			Direct;					//��������
	SContractNoType	Code[EQT_End];		//�������//���״���
	TOptionTarget()
	{
		memset(this, 0, sizeof(TOptionTarget));
		Direct = dBuy;
	}
};

//��Ȩ������
class COrderOption :public TIxFrm, public IOperator, public ISQuoteDataSpi
{
public:
	COrderOption();
	~COrderOption();

	void CreateWin(CMousewin* opr, HWND parent, int nleft = 0, int ntop = 0, bool bUseFor = false);

	void UpdateOptionInfo(int nSize, TOptionInfo info[N_5_row], int nType = 0);

	void UpdatePrice(int nIndex = 0);				//���¼۸�

	void SetExchangeSupport(bool bCheck);
private:
	void SendOptionStrategy();

	bool IsActive();
public:
	//////////////////////////////////////////////////////////////////////////����ص� 	
	virtual void __stdcall OnQuote(const SContract* cont, const SQuoteUpdate* quote) ;				//��ͨ��������	
	virtual void __stdcall OnSpreadQuote(const SSpreadContract* scont) {};							//������������
public:
	//////////////////////////////////////////////////////////////////////////
	virtual void SetPriceType(const wchar_t* sPType) {};
	virtual void SetValidType(const wchar_t* sPType) {};
	virtual void SetMaxMinQty(const char* sQty) {};
	virtual bool SendOrder(MsgOrderInfo *msg) { return false; };
	virtual void SetContractNo(const char* sCode, CodeSetType csType = cstChange) {};
	virtual void SetPriceData(const char* sdata) {};
	virtual bool GetMouseWinCfg(sMousewinCfg & cfg) { return false; };
	virtual bool SetHeadData(const wchar_t* sPType, bool bHead = true) { return false; };
	virtual bool SetSelect(const wchar_t* sStrategy, int nRef);
	virtual bool TableButtonToEdit() { return false; };
	virtual bool CxGetWindowText(MH_TYPE m_type, int& nSize, wchar_t* wstr, int nMaxLen = LEN_RET) { return false; };
public:
	WNDPROC				m_EditProc;		//�۸������
	WNDPROC				m_EditProc2;	//�����������
	SOFCommodityInf		m_OptionCommodityInfo;	//Ʒ����Ϣ
	SOFCommodityInf		m_TargetCommodityInfo;	//Ʒ����Ϣ
private:
	CMousewin		*m_opr;				//�������

	CPopWinList		m_PopListPriceType;	//�۸�ѡ��
	CPopWinList		m_PopListValidType;	//��Ч����
	CPopWinList		m_PopListOCType;	//��ƽ

	CLockBtn		m_SyncPriceTypeBtn;	//�۸�����
	CLockBtn		m_SyncQtyBtn;		//��������
	
	bool			m_bSyncPrice;		//�۸���������ѡ��
	bool			m_bSyncQty;			//��������ѡ��
	
	CxSpin			m_SpinPrice;		//�۸� ΢��
	CxSpin			m_SpinQty;			//���� ΢��

	CxCheck			m_ExchangeSupport;
	CxCheck			m_AddOne;

	HWND			m_hControl[QC_END];	//�ؼ��������

	RECT			m_rcItem[N_Max_Row][COO_END];		//list��������
	RECT			m_rcSpecialItem[SPI_END];			//button��������
			
	TMouPriceTypeStrW	m_strItem[N_Max_Row][COO_END];	//list�ı�
	TMouPriceTypeStrW m_strSpecialItem[SPI_END];		//button�ı�
	int				m_nCurHoverID;						//������ʾ����
	int				m_CurType;							//��ǰ����
	int				m_nSTDSTG;							//�Ƿ�STD STG

	OCClickItem		m_LClickItem;		//�����������
	OCClickItem		m_CurShowItem;		//��ǰ��������

	HDC				m_HdcMem;			//����DC
	HBITMAP			m_hBitmap;			//ͼ��

	COLORREF		m_ExColor[EDC_END];	//��ɫ	
	HBRUSH			m_ExBrush[EDC_END];	//��ˢ

	int				m_nleft;			//������
	int				m_ntop;				//������
	int				m_nWidth;			//���ڿ��
	int				m_nHeight;			//���ڸ߶�
	int				m_nOptionRows;	    //��ǰ��Ч��Լ��
	bool			m_bTracking;		//������
    bool            m_bInfoError;       /// ��ǰ��Ϣ�Ǵ�����Ϣ���Ǳ�֤��
    wchar_t         m_strInfo[MAX_PATH]; /// ״̬��Ϣ����ʾ��֤��������Ϣ
private:
	char			m_Sign[21];			//�˻���������ʶ

	SContractNoType m_QuoteTradeCode[EQT_End][N_5_row];//�����Լ
	
	TOptionTarget	m_OptionTarget;

	SOptionSeriesNoType	m_SeriesNo;
private:
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
	
	void LayOut();						//���򲼾ָ���
	
	void OnCreate(WPARAM wParam, LPARAM lParam);	
	void OnNotify(WPARAM wParam, LPARAM lParam);//֪ͨ��Ϣ���� spin���� 		
	void OnCommond(WPARAM wParam, LPARAM lParam);
	void OnPaint(WPARAM wParam, LPARAM lParam);
	void OnSize(WPARAM wParam, LPARAM lParam);
	LONG OnCtlColorStatic(WPARAM wParam, LPARAM lParam);
	void OnDestory(WPARAM wParam, LPARAM lParam);
	
	void OnLButtonDown(WPARAM wParam, LPARAM lParam);
	void OnLButtonUp(WPARAM wParam, LPARAM lParam);

	void OnMouseMove(WPARAM wParam, LPARAM lParam);
	void OnMouseLeave(WPARAM wParam, LPARAM lParam);
	void OnMouseWheel(WPARAM wParam, LPARAM lParam);

	void OnKillFocusPriceEdit();		//�۸�༭��ʧȥ����
	void OnKillFocusQtyEdit();			//�����༭��ʧȥ����
	
	void MoveEdit(int x, int y, int width, int height, bool bPrice = true);	//�ƶ�Edit����ʾ
	
	void OnButtonPriceSync();			//�����۸�����
	void OnButtonQtySync();				//��������
	void ResetSyncState();				//��������

	void OnBackDirect();
	void OnAddOptionTarget();
	void OnReset();

	void OnCheckSupport();				//������֧�����
	
	void OnSpinDeltaUpDownEx(int nDelta, int nID);										//�۸���������
	void OnSpinDeltaUpDownNormal(int nDelta, int nID, int nMax = 1000, int nMin = 1);	//������������

	//////////////////////////////////////////////////////////////////////////�����������
	void DrawBackColor();		//����
	void DrawGridLine();		//����	
	void DrawListContent();		//�б�
	void DrawRow(int i);		//��
	void DrawSpecial();			//��������
	void DrawTriangle(HDC &hdc, int nStartx, int nStarty, int nWidth, COLORREF color = RGB(200, 200, 200));//�������

	void Redraw(RECT* rc = NULL);
	//////////////////////////////////////////////////////////////////////////��Ϣ����
	void UpdateSyncQty();							//������������	
	void UpdateSyncType(int nIndex = 0);			//���¼۸���������	
	void UpdateCommodityInfo(const char* Code,bool bOption = true);		//����Ʒ����Ϣ
	void UpdateDelta(int i);
	void UpdateTargetDelta();
	void UpdateTotal();
	int GetTypeStrId(int nType);
	bool GetSeriNo(const char* sQuote);
	//////////////////////////////////////////////////////////////////////////
	bool GetOrderInfo(int i, TSendOrder &sOrder, bool bShowMessage = true);    //��ȡ������Ϣ
	bool GetOrderPrice(int nRow, double &dprice, bool bShowMessage = true);	//��ȡ�۸�	
	bool GetOrderQty(int nRow, uint &uQty, bool bShowMessage = true);			//��ȡ����	
	bool GetOffset(int nRow, TOffset &Offset);		//��ȡ��ƽ	
	void GetValidType(TValidType &type);			//��ȡ��Ч����
    void UpdatePreErrorMsg(int nErrCode, std::string strContractCode);           /// ���±�֤�������Ϣ

    void UpdatePreFreeze(); /// ����Ԥ�����ʽ�

};

#endif
