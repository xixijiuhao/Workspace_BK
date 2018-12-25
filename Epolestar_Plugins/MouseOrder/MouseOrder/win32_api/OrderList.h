#ifndef _LIST_ORDER_WIN_
#define _LIST_ORDER_WIN_

#define	N_Buy	0
#define	N_Sell	1
#define	N_Size	2
#define N_Scale	20

#define N_WIDTH_LIST		297
#define N_HEIGHT_LIST_MIN	252
#define N_HEIGHT_LIST		1080
#define N_HEIGHT_ITEM_LIST	36
#define N_AVERAGE			5


//��ɫϵ
//#define RGB_Red				RGB(255, 60, 40)
//#define RGB_Green			RGB(0, 220, 0)
#define RGB_Yellow			RGB(220, 220, 0)
#define RGB_Red				RGB(255, 0, 0)
#define RGB_Green			RGB(0, 255, 0)

#define RGB_Black20			RGB(20,20,20)
#define RGB_Black38			RGB(38, 38, 38)
#define RGB_Black70			RGB(70, 70, 70)
//��ɫϵ
//#define RGB_Red_W			RGB(255,0,0)
//#define RGB_Green_W		RGB(20, 240, 20)

#define RGB_Red_W			RGB(157,36,12)
#define RGB_Green_W			RGB(54, 134, 50)

#define RGB_Red_B			RGB(246, 226, 226)
#define RGB_Green_B			RGB(224, 242, 223)

#define RGB_WhiteWholeBk	RGB(242,243,247)
#define RGB_WhiteSpecialBk	RGB(204, 206, 219)
#define RGB_Yellow_W		RGB(211,163,91)


//defalut
#define Default_Red                     RGB(157,36,12)          //��ɫ 
#define Default_Green                   RGB(54, 134, 50)        //��ɫ
#define Default_AskBid_Red              RGB(255, 180, 180)      //��ɫ ��1��1����
#define Default_AskBid_Green            RGB(180, 240, 180)      //��ɫ ��1��1����

//Light Red Green
#define Light_RG_Red                    RGB(157,36,12)          //Red
#define Light_RG_Green                  RGB(54, 134, 50)        //Green
#define Light_RG_AskBid_Red             RGB(246, 226, 226)      //��ɫ ��1��1����
#define Light_RG_AskBid_Greed           RGB(224, 242, 223)      //��ɫ ��1��1����

//Dark
#define Dark_Red                        RGB(200, 100, 200)      //Red
#define Dark_Green                      RGB(85, 166, 85)        //Green
#define Dark_Black                      RGB(0, 0, 0)            //Black
#define Dark_Bg_Red                     RGB(35, 15, 15)         //Red background ����б��ɫ����
#define Dark_Bg_Green                   RGB(20, 35, 20)         //Green background ����б���ɫ����
#define Dark_AskBid_Red                 RGB(150, 0, 0)          //��ɫ ��1��1����
#define Dark_AskBid_Green               RGB(0, 150, 0)          //��ɫ ��1��1����

//Light Yellow
#define Light_Yellow                    RGB(231,227,207)

//Light Blue
#define Light_Blue                      RGB(192,209,220)

//Light Green
#define Light_Green                     RGB(199,237,204)

//Select Color
#define Select_Color                    RGB(200, 200, 20)

//�ǵ���ʶ
#define LastPrice_Keep		0
#define LastPrice_Up		1
#define LastPrice_Down		-1
enum ListItemId
{
	LII_Tick_Edit_Win,
	LII_Tick_Edit,
};
enum DataPriceType
{
	DPT_Last=0,	//���¼�
	DPT_Bid1,	//��1
	DPT_Ask1,	//��1
	DPT_VLong,	//��ͷ�ֲ�
	DPT_VShort,	//��ͷ�ֲ�
	DPT_High,	//��߼�
	DPT_Low,	//��ͼ�
	DPT_Up,		//��ͣ��
	DPT_Down,	//��ͣ��
	DPT_VStart,	//�ɼ���ʼ
	DPT_VBase,	//��׼
	DPT_VEnd,	//�ɼ���ֹ
	DPT_Size,
};
enum DataSpecialItem
{
	DSI_MarketBuy,	//����
	DSI_MarketSell,	//����
	DSI_Mid_1,		//����
	DSI_Mid_2,		//��һ
	DSI_Mid_3,		//��һ
	DSI_Mid_4,		//�ֶ�
	DSI_CancelB,	//����
	DSI_CancelS,	//����
	DSI_CancelA,	//������
	DSI_Size,	
};
enum ColorLevel
{
	CL_HighLight,	//����
	CL_LowLight,	//����
	CL_Size,	
};
enum ColorStyle
{
    CS_Black,       //��ɫ
	CS_White,		//��ɫ
	CS_BkMid,		//�в�����ɫ
	CS_BKLine,		//���߱���ɫ
	CS_BKSpecial,	//���ⰴť����ɫ
	CS_FGSpecial,	//����ɫ
	CS_LastPrice,	//���¼���ɫ
	CS_PrcFG,		//��ͨ�۸�ǰ��ɫ
	CS_Size,
};
enum DataGridCol
{
	DGC_LEFT,		//��
	DGC_MID,		//��
	DGC_RIGHT,		//��
	DGC_Size,
};
enum DataSpecial
{
	DS_LastHover,	//��󼤻�
	DS_Hover,		//����
    DS_Select,      //ѡ������
	DS_Size,
};
enum ClickType
{
	CT_None,		//�޵��
	CT_Special,		//���ⰴť
	CT_BS,			//������ť
};
enum BaseMidType
{
	BMT_Last,		//���¼۾���
	BMT_Bid1,		//��һ�۾���
	BMT_Ask1,		//��һ�۾���
	BMT_Hand,		//�ֶ�����
	BMT_Size,		//
};
enum BuySellSubItem
{
	BSI_WHOLE,		//����
	BSI_LEFT,		//��
	BSI_RIGHT,		//��
};
enum TStopType
{
	Tst_STP,
	Tst_STL,
	Tst_LMT,
	Tst_Size
};
enum PopWinType
{
	PWT_Mid,
	PWT_Stop,
	PWT_Size,
};

struct DataPriceID
{
	double	dPrice;	//�۸�
	int		nQty;
	int		nId;	//���
};
struct DataQuoteQty
{
	double	dPrice;	//�۸�
	int		nQty;	//����
};
struct DataLocalWorkQty
{
	int		nQtyNormal[N_Sell+1];		//�ҵ�����
	int		nQtyCondition[N_Sell + 1];	//ֹ������

};
struct ItemRowCol
{
	int		nRow;		//��
	int		nCol;		//��
	int		nSubItem;	//����
};
struct ItemSpecial
{
	RECT    rect;		//��ť
	wchar_t wtext[20];	//��ʾ����
};
struct ItemClick
{
	int	nSpecial;		//���ⰴť���
	int	nCol;			//������ť ��
	int nRow;			//������ť ��
	ClickType nType;	//���ⰴť��������ť
};
struct ListOrderInfo
{
	TDirect			Direct;		//����
	TOffset			Offset;		//��ƽ
	double			OrderPrice;	//ί�м۸�
	TOrderType		OrderType;	//�м� �޼�
	double			TriggerPrice;//ֹ��
};

enum ColorSytle
{
    Style_Defalut = 0,      //Ĭ�Ϸ�����ǳɫ
    Style_Light_RG,         //ǳɫ������������������
    Style_Dark,             //��ɫ����
    Style_Light_Yellow,     //ǳ��ɫ����
    Style_Light_Blue,       //ǳ��ɫ����
    Style_Light_Green,      //ǳ��ɫ����
};

class CMousewin;

class COrderList :public TIxFrm,public IOperator
{
public:
	COrderList();
	~COrderList();
	
	void CreateWin(CMousewin* opr, LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle = 0, int nleft = 0, int ntop = 0, bool bUseFor = false);					//������������
	
	void UpdateCode(const SContractNoType sCode, SOFCommodityInf& sinfo);	//���º�Լ��Ϣ

	void UpdateUser(const char* UserNo, const char* Sign, char cCoverMode);	//�����û�������Ϣ

	void UpdatePrice(bool bBaseChange = false);								//���¼۸�

	void UpdateQty(int nQty) { m_OrderQty = nQty; };						//��������

	void UpdateTradeOrder(const TOrder* data);								//ί�лر�����

	void UpdateParOrderQty(bool bUpdateUI = true);							//�ҵ�������

	void UpdatePosition(bool bUpdateUI = true);								//�ֲ��߸���

	void OnSpinDeltaUpDownNormal(int nDelta, int nID, int nMax = 999, int nMin = -99);

	virtual void SetPriceType(const wchar_t* sPType){};
	virtual void SetValidType(const wchar_t* sPType){};
	virtual void SetMaxMinQty(const char* sQty){};
	virtual bool SendOrder(MsgOrderInfo *msg) { return false; };
	virtual void SetContractNo(const char* sCode, CodeSetType csType = cstChange){};
	virtual void SetPriceData(const char* sdata){};
	virtual bool GetMouseWinCfg(sMousewinCfg & cfg){ return false; };
	virtual bool SetHeadData(const wchar_t* sPType, bool bHead = true){ return false; };
	virtual bool SetSelect(const wchar_t* sStrategy, int nRef);
	virtual bool TableButtonToEdit(){ return false; };
	virtual bool CxGetWindowText(MH_TYPE m_type,int& nSize, wchar_t* wstr, int nMaxLen = LEN_RET) { return false; };

    void SetRightCancel(bool b);                                             //�Ҽ�����

	WNDPROC			m_SetEdit;

	void ListSendOrder(ListOrderInfo& order);

    void UpdateColorStyle(bool bUseFor);

private:
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);

	bool IsActive();

	void OnCreate(WPARAM wParam, LPARAM lParam);

	void OnCommond(WPARAM wParam, LPARAM lParam);

	void OnDestory(WPARAM wParam, LPARAM lParam);

	void OnPaint(WPARAM wParam, LPARAM lParam);

	void OnSize(WPARAM wParam, LPARAM lParam);

	LONG OnCtlColorStatic(WPARAM wParam, LPARAM lParam);

	void OnLButtonDown(WPARAM wParam, LPARAM lParam);

	void OnLButtonUp(WPARAM wParam, LPARAM lParam);

	void OnRButtonDown(WPARAM wParam, LPARAM lParam);

	void OnRButtonUp(WPARAM wParam, LPARAM lParam);

	void OnMouseMove(WPARAM wParam, LPARAM lParam);

	void OnMouseHover(WPARAM wParam, LPARAM lParam);

	void OnMouseLeave(WPARAM wParam, LPARAM lParam);

	void OnMouseWheel(WPARAM wParam, LPARAM lParam);
		
	void OnKillFocus(WPARAM wParam, LPARAM lParam);

	void OnKeyDown(WPARAM wParam, LPARAM lParam);

	void DrawItem(HDC hdc);							//�����б�Ԫ

	void DrawItemLeft(HDC hdc, int n, int m);						//�����б�Ԫ

	void DrawItemMid(HDC hdc, int n, int m);						//�����б�Ԫ

	void DrawItemRight(HDC hdc, int n, int m);						//�����б�Ԫ

	void DrawItemQty(HDC hdc);										//��������ҵ� �ͻ��ҵ�

	void DrawItemSpecial(HDC hdc);									//�������ⵥԪ

	void ResetHoverState(bool bUpdateWin = false);					//
	
	void DrawTriangle(HDC &hdc, int nStartx, int nStarty, int nWidth);	//��������
	
	void DrawArrow(HDC &hdc, int nStartx, int nStarty, int nWidth, int nHeight);	//���Ƽ�ͷ

	void DrawDiv(HDC &hdc, int nStartx, int nStarty, int nWidth);	//���Ʒָ���

	void DrawHighLow(HDC& hdc, RECT& rect,bool bHigh = true);

	void UpdateRect(ItemRowCol &item);								//��������
private:
	bool GetPriceStrByLineId(TMouFormatPriceStrW &wtext, int nID);

	int GetSTLTick();

	void SetColorStyle(bool bUseFor = false, bool bBlack = true);

	void SetChildRectContent();

	void UpdateMidHead();													//������ͷ�в���ʾ

	void UpdateLastPriceData();												//�������¼���Ϣ
private:
	void OnClickSpecial(int nItem);

	void OnClickBuySell(int nCol,int nRow);

	void OnRCancelOrder(int nCol, int nRow);

	bool  OnCancelOrder(int nOrderId,TDirect direct,bool bShowMsg = false);

private:
	void ClearData();	
    void CreateBrushObject();
    void DeleteBrushObject();
    void ResetColorStyle(bool bUseFor, int style);
private:
	CMousewin		*m_opr;	//����

	bool			m_bCreate;
	COLORREF		m_Color[N_Sell + 1][CL_Size];	//������ɫ
	HBRUSH			m_Brush[N_Sell + 1][CL_Size];	//������ˢ

	COLORREF		m_ExColor[CS_Size];
	HBRUSH			m_ExBrush[CS_Size];	

    COLORREF        m_ColorBackground;              //���屳��ɫ
    COLORREF        m_ColorTextHover;               //hover״̬������ɫ
    COLORREF        m_ColorOrderQty;                //ί������������ɫ
    COLORREF        m_ColorTextBuy;                 //����б���������ɫ
    COLORREF        m_ColorTextSell;                //����б�sell������ɫ
    COLORREF        m_ColorTextNormal;              //��ͨ������ɫ,û�йҵ����ļ۸��
    COLORREF        m_ColorTextLmt;                 //LMT��STP����ɫ
    COLORREF        m_ColorBuy;                     //buy
    COLORREF        m_ColorSell;                    //sell
    COLORREF        m_ColorHighLow;                 //��߼ۡ���ͼ���ɫ
    COLORREF        m_ColorLine;                    //����ߵ���ɫ
    COLORREF        m_ColorHoverFrame;              //hover�߿���ɫ
    COLORREF        m_ColorBgSpecial;               //��������ı���ɫ
    COLORREF        m_ColorLastPrice;               //���¼���ɫ
    COLORREF        m_ColorBgMid;                   //��ͷ�м����򱳾�ɫ
    COLORREF        m_ColorBlack;                   //black
    COLORREF        m_ColorWhite;                   //white
    COLORREF        m_ColorBgAsk1;                  //��1����ɫ
    COLORREF        m_ColorBgBid1;                  //��1����ɫ
    COLORREF        m_ColorBgBid;                   //�����䱳��ɫ
    COLORREF        m_ColorBgAsk;                   //�����䱳��ɫ
    COLORREF        m_ColorBgBuy;                   //buy����ɫ
    COLORREF        m_ColorBgSell;                  //sell����ɫ

    HBRUSH          m_BrushBackground;              //���屳��ɫ
    HBRUSH          m_BrushTextHover;               //hover״̬������ɫ
    HBRUSH          m_BrushTextQty;                 //����������ɫ
    HBRUSH          m_BrushTextBuy;                 //����б���������ɫ
    HBRUSH          m_BrushTextSell;                //����б�sell������ɫ
    HBRUSH          m_BrushTextNormal;              //��ͨ���廭ˢ
    HBRUSH          m_BrushBuy;                     //buy
    HBRUSH          m_BrushSell;                    //sell
    HBRUSH          m_BrushBgBuy;                   //buy����ɫ
    HBRUSH          m_BrushBgSell;                  //sell����ɫ
    HBRUSH          m_BrushHighLow;                 //��߼ۡ���ͼ���ɫ
    HBRUSH          m_BrushLine;                    //����ߵ���ɫ
    HBRUSH          m_BrushHoverFrame;              //hover�߿�
    HBRUSH          m_BrushLastPrice;               //���¼���ɫ
    HBRUSH          m_BrushBgMid;                   //��ͷ�м����򱳾�ɫ
    HBRUSH          m_BrushBlack;                   //black
    HBRUSH          m_BrushWhite;                   //white
    HBRUSH          m_BrushBgAsk1;                  //��1����
    HBRUSH          m_BrushBgBid1;                  //��1����
    HBRUSH          m_BrushBgAsk;                   //�����䱳��
    HBRUSH          m_BrushBgBid;                   //�����䱳��
    HBRUSH          m_BrushBgSpecial;               //�������򱳾�
    HBRUSH          m_BrushSelect;                  //��ǰѡ�б���ɫ

	HDC				m_HdcMem;						//����DC
	HBITMAP			m_hBitmap;						//ͼ��

	bool			m_bTracking;					//������

    bool            m_bRightCancel;                 //�Ҽ�����

	CxEditEx		m_EditExPoint;					//��Լ�༭��
private:
	DataPriceID		m_DataAxis[DPT_Size];			//����

	RECT			m_ItemRect[DGC_Size][N_Scale * 2 + 1];	//�б�3���� ��Ԫ����

	ItemSpecial		m_ItemRectSpecial[DSI_Size];	//���ⰴť

	ItemRowCol		m_SpecialItem[DS_Size];		//������ť ���¾ɣ�

	int				m_nLine;						//����

	int				m_nListHeight;					//�߶� 25������

	RECT			m_HeadRect;						//�б�ͷ����
	RECT			m_ListRect;						//�����б�����
	RECT			m_FooterRect;					//�б�������

	int				m_nScrollOffset;				//������ƫ��

	ItemClick		m_ItemClickL;					//��������Ϣ
	ItemClick		m_ItemClickR;					//�Ҽ������Ϣ

	int				m_BaseMidType;					//��������

	int				m_nStopType;					//ֹ������

	bool			m_bInner;						//�����̱��

	CPopWinList		m_PopWinList[PWT_Size];			//��������

	TMouPriceTypeStrW	m_PopWinMidStr[2][BMT_Size];	//�����ַ���

	TMouPriceTypeStrW	m_PopWinStopStr[2][Tst_Size];	//ֹ���ַ���
private:
	map<int, DataQuoteQty> m_WorkData;				//����ҵ�

	map<int, DataLocalWorkQty> m_LocalWorkData;		//�ͻ��ҵ�

	list<DataPriceID>			m_listLastPriceId;	//���¼ۻ���
	double						m_OldLast;			//ǰ�����¼�
	int							m_LastUpdown;		//�ǵ���ʶ

	SOFCommodityInf	m_CommodityInfo;				//Ʒ����Ϣ

	SContractNoType m_sQuoteCode;					//�����Լ

	string			m_UserNo;						//�˺�

	string			m_Sign;							//��������ʶ

	map<int, const TOrder*>	m_ParOrderInfo;			//�ҵ���Ϣ

	int				m_nCanCancelQty[N_Sell + 1];	//�ҵ���

	int				m_OrderQty;						//����
private:
	int  DivResult(const double Value, const double SubValue, const double ErrorDiff = 1e-8);
};


#endif // !1
