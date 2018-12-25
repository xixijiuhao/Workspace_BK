#ifndef _BTN_ORDER_WIN_
#define _BTN_ORDER_WIN_

#define N_TEXT_LEN 21
#define N_LONGTEXT_LEN	51

typedef int OrderButtonType;
#define	obInner		0
#define obForeign	1
#define	obInnerSwap	2
#define	obInnerOffer	3

typedef int OrderButtonCount;
#define	obc_2		2
#define obc_3		3
#define obc_4		4

enum OrderButtonDirectP //�� ƽ�� ƽ�� ��
{
	OBP_Buy,
	OBP_CoverS,
	OBP_CoverL,
	OBP_Sell
};
enum OrderButtonDirect //�� ƽ ��
{
	OBD_Buy,
	OBD_Cover,
	OBD_Sell
};
enum OrderButtonDirectF	//�� ��
{
	OBD_BuyF,
	OBD_SellF
};
enum OrderButtonItem	//�۸� ����
{
	OB_Price,
	OB_Direct
};
enum OrderShowInfo //���� ��֤�� ����
{
	OSI_Qty,
	OSI_Money,
	OSI_Currency
};
enum ItemText			//
{
	IT_NoOffset,
	IT_NoPosition,
	IT_HasPosition,
	IT_HasMatchPosition,
	IT_Offer,
	IT_Offer2,
	IT_Price,
    IT_ShiftPosition,
    IT_ShiftPosition2,
};
enum CanOpenCover	//�ɿ� ��ƽ
{
	COC_OpenB,		
	COC_CoverB,
	COC_OpenS,
	COC_CoverS,
};
enum CanOpenCoverF	//���� �Գ彨�� ���� 
{
	COC_QtyB,
	COC_MoneyB,
	COC_CurrencyB,
	COC_QtyS,
	COC_MoneyS,
	COC_CurrencyS,
};
struct ItemInfo
{
	RECT	rItem;
	RECT	rTextUp[OB_Direct+1];
	RECT	rTextDown[OB_Direct+1];
	bool	bInRect;
	int		nIt;
	wchar_t wContent[IT_ShiftPosition2 + 1][N_LONGTEXT_LEN];
	ItemInfo()
	{
		memset(this, 0, sizeof(ItemInfo));
	}
};
struct ItemQty 
{
	RECT	rItem;
	wchar_t wContent[N_LONGTEXT_LEN];
	ItemQty()
	{
		memset(this, 0, sizeof(ItemQty));
	}
};
struct PositionInfo 
{
	OrderButtonType		type;
	OrderButtonCount	nButton;
	int					nLong;
	int					nShort;
	int					nEditQty;
};
class COrderButton :public TIxFrm
{
public:
	COrderButton();
	~COrderButton();

	void CreateWin(IOperator* opr, LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle = 0, int nleft = 0, int ntop = 0, COLORREF rbuy = RGB(201, 51, 39), COLORREF rsell = RGB(26, 129, 237));					//������������
	
	void UpdateData(const PositionInfo& info);

	void UpdateCanOpen(int nBuy, int nSell);

	void UpdatePrice(wchar_t *wBuy, wchar_t *wSell);

	void UpdateEditQty(int nQty);

	void UpdateForeignQty(wchar_t* wcon,TDirect direct);

	void UpdateForeignPre(wchar_t* wMoney, wchar_t* wCurrency, TDirect direct);

	void OnSetFocus(int nID = 0);

	void OnSetFocus(char cDirect, char cOffset);
	
	void ButtonSendOrder(MsgOrderInfo &Msg);

    void SetBShitPosition(bool bShiftPosition = false);
private:
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);

	void OnCreate(WPARAM wParam, LPARAM lParam);

	void OnCommond(WPARAM wParam, LPARAM lParam);

	void OnDestory(WPARAM wParam, LPARAM lParam);

	void OnPaint(WPARAM wParam, LPARAM lParam);

	LONG OnCtlColorStatic(WPARAM wParam, LPARAM lParam);

	void OnLButtonDown(WPARAM wParam, LPARAM lParam);

	void OnLButtonUp(WPARAM wParam, LPARAM lParam);

	void OnMouseMove(WPARAM wParam, LPARAM lParam);

	void OnMouseLeave(WPARAM wParam, LPARAM lParam);

	void OnKillFocus(WPARAM wParam, LPARAM lParam);

	void OnKeyDown(WPARAM wParam, LPARAM lParam);

	void paint_obc_2();		//�����ػ�

	void paint_obc_3();		//�����ػ�

	void paint_obc_4();		//�ļ��ػ�

	bool IsInnerType();		//����

	bool IsInnerSwap() { return m_PosInfo.type == obInnerSwap; };		//����

	bool IsInnerOffer() { return m_PosInfo.type == obInnerOffer; };		//Ӧ��
private:
	IOperator		*m_opr;								//����
	COLORREF		m_ColorBuy;							//����ɫ
	COLORREF		m_ColorSell;						//����ɫ
	ItemInfo		m_InnerItemP[OBP_Sell + 1];			//4������

	ItemInfo		m_InnerItem[OBD_Sell + 1];			//3������
	ItemQty			m_CanOpenItem[COC_CoverS + 1];		//���̿ɿ���ƽ

	ItemInfo		m_ForeignItem[OBD_SellF + 1];		//2������
	ItemQty			m_CanOpenItemF[COC_CurrencyS + 1];	//���̶Գ彨��

	HDC				m_HdcMem;						//����DC
	HBITMAP			m_hBitmap;						//ͼ��
	HBRUSH			m_BKBrush;						//������ˢ
	HBRUSH			m_BuyBrush;						//���뻭ˢ
	HBRUSH			m_SellBrush;					//������ˢ

	bool			m_bTracking;					//������
	
	PositionInfo	m_PosInfo;						//�ֲ���Ϣ
private:
	void			UpdateLayout();					//����������Ϣ

	void			OnClickItemInner(int item);		//��������
	void			OnClickItemInnerP(int item);	//�����ļ�
	void			OnClickItemForeign(int item);	//��������

	//��ʱ����
	int				m_nInnerClickIndex;				//���̵������
	int				m_nForeignClickIndex;			//���̵������
	bool			m_bClickDown;					//�������
	int				m_nFocusId;						//����ID

    bool            m_bShiftPosition;               //�Ƿ��Ʋ�
};


#endif // !1
