#ifndef _BTN_DIRECT_WIN_
#define _BTN_DIRECT_WIN_

//��Ϣ
#define WM_ORDER		5004	

class CBtnWin :public TIxFrm
{
public:
	CBtnWin();
	~CBtnWin();

	void	CreatePopWin(IOperator *opr, LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle = 0, int nleft = 0, int ntop = 0, int nwidth = 0, int nheight = 0, char cDirect = oNone, char cOffset = dNone, char cSellOffset = dNone,COLORREF rbuy=RGB(201,51,39),COLORREF rsell = RGB(26,129,237));	//������������

	void	SetLongData(int nlong);// { m_nLong = nlong; };		//���ö�ͷ����
	int     GetLongData(){ return m_nLong; };
	void	SetPriceData(wchar_t* dprice);// { m_dPrice = dprice; };		//���ö�ͷ����
	wstring  GetPriceData(){ return m_dPrice; };
	void	SetShortData(int nshort);// { m_nShort = nshort; };	//���ÿ�ͷ����
	int     GetShortData(){ return m_nShort; };

	void    UpdateWindow(RECT &rect);									//���´���

	void	SetOffset(TCoverMode cm);						//���ÿ�ƽ

	void    SetPosShowFlag(bool bShow = false);// { m_bShowPos = bShow; }	//������ʾ�ֲ�

	void	SetCheck(bool bGetF);										//�����Ƿ��ȡ������
	bool	GetCheck();													//��ȡ�Ƿ�ӵ�н�����

	void    SetOffer(bool bOffer){ m_bOffer = bOffer; }					//����Ӧ�۱�ʶ

	void    SetTextQty(wchar_t *cQty);						//���ÿ�����Ϣ�ı�		����
	void    SetTextPre(wchar_t *cPre, wchar_t *cPreBase);	//����Ԥ������������Ϣ	����

private:
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:

	HDC		m_hdctemp;		//����DC ͼƬ������
	HDC		m_HdcMem;		//����DC ͼƬ����
	
	RECT	m_reClient;		//���ھ��ο�
	RECT    m_rePos;		//������ʾ��������

	int		m_nLeft;		//������� ������
	int     m_nTop;			//������� ������
	int		m_nWidth;		//���ڿ��
	int		m_nHeight;		//���ڸ߶�

	TDirect m_tDirect;		//��������
	TOffset m_tOffset;		//��ƽ����
	TOffset m_tSellOffset;	//������ƽ����
	HBITMAP m_hBitmap;		//��ɫλͼ
	HBITMAP m_hbmptemp;		//Ŀ�Ĵ�Сλͼ
	BITMAP  m_bBitmapInfo;	//λͼ��С
	int     m_nState;			//״̬
	BOOL    m_bMouseTrack;		//������

	void    DrawItemText(HDC &hdc, RECT &rect);

	bool    m_bShowPos;			//��ʾ�ֱֲ�ʶ
	wchar_t    m_cPosShow[20];		//�ֲ��ı�
	//�ֲ�
	int     m_nLong;			//��ͷ 
	int     m_nShort;			//��ͷ
	//�۸�
	wstring  m_dPrice;			//�µ��۸�
	RECT    m_rePrice;			//�۸�

	IOperator *m_opr;	//����
	bool	m_bFocus;	//��ǰ �Ƿ�õ�����
	bool	m_bInFlag;	//�Ƿ��ڴ������������

	bool    m_bOffer;	//Ӧ�۱�ʶ

	wstring    m_cCanQtyShow;		//��������
	wstring    m_cPreFreeShow;		//Ԥ�����ʽ�
	wstring    m_cPreFreeBaseShow;	//����
	RECT    m_reCanQty;				//������ʾ��������
	RECT    m_rePreFree;			//������ʾ��������
	RECT    m_rePreFreeBase;		//������ʾ��������

	COLORREF m_BuyColor;
	COLORREF m_SellColor;
};

class CBtnDirectWin :public TIxFrm
{
public:
	CBtnDirectWin();
	~CBtnDirectWin();

	void CreatePopWin(IOperator *opr, LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle = 0, int nleft = 0, int ntop = 0, int nwidth = 0, int nheight = 0, char cDirect = dNone, COLORREF rbuy = RGB(201, 51, 39), COLORREF rsell = RGB(26, 129, 237));	//������������

	void	SetLongData(int nlong);							//���ö�ͷ�ֲ�
	int     GetLongData(){ return m_nLong; };				//��ȡ��ͷ�ֲ�
	void	SetPriceData(wchar_t* dprice);					//���ö�ͷ����
	wstring  GetPriceData(){ return m_dPrice; };
	void	SetShortData(int nshort);						//���ÿ�ͷ�ֲ�
	int     GetShortData(){ return m_nShort; };				//��ȡ��ͷ�ֲ�
	void	SetCanOpenCover(wchar_t* wCon);

	void    UpdateWindow(RECT &rect);						//ˢ�´��� �ػ�	

	void	SetOffset(TCoverMode cm);

	void    SetPosShowFlag(bool bShow = false);// { m_bShowPos = bShow; }

	HWND    GetHwndOpen();
	HWND    GetHwndClose();

private:
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	vector<string> m_vstring;
	HDC		m_HdcMem;		//����DC
	

	UINT	m_nPos;			//���
	
	RECT	m_reUnit;		//��Ԫ���ο�
	RECT	m_reClient;		//���ھ��ο�

	RECT    m_reText;		//����
	RECT    m_rePos;		//�ֲ�
	int		m_nLeft;
	int     m_nTop;
	int		m_nWidth;		//���ڿ��
	int		m_nHeight;		//���ڸ߶�

	CBtnWin m_Btnwinleft;		//���ְ�ť
	CBtnWin m_Btnwinright;		//ƽ�ְ�ť

    TDirect m_tDirect;			//����
	
	HBITMAP m_hBitmap;			//��ɫλͼ
	BITMAP  m_bitmap;			//λͼ��С

	wstring m_sDirect;			//�����ı�
	wstring m_sDirectOffer;		//Ӧ�۷���
	bool	m_bShowPos;			//�ֲ���ʾ���
	wchar_t m_cPosShow[20];		//�ֲ�����
	
	int     m_nLong;			//��ͷ //�ֲ�
	int     m_nShort;			//��ͷ

	//�۸�
	wstring m_dPrice;			//�µ��۸�
	RECT	m_rePrice;			//�۸�

	wchar_t m_sCanOpenCover[40];//�ɿ���ƽ
	RECT	m_reCanOpenCover;	//�ɿ���ƽ

	IOperator *m_opr;

	RECT    m_reLeftBtn;		//
	RECT    m_reRightBtn;		//

	COLORREF m_BuyColor;
	COLORREF m_SellColor;
};
#endif