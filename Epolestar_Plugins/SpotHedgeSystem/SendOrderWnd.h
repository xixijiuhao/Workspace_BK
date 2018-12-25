#pragma once
enum ControlID
{
	ID_Begin,
	//��Լ
	ID_Static_Contract,
	ID_Edit_Contract,
	ID_Button_Contract,
	//��ͬ
	ID_Static_Agree,
	ID_Combox_Agree,
	//����
	ID_Static_Qty,
	ID_Edit_Qty,
	ID_Spin_Qty,
	ID_Static_Lot,
	//�۸�
	ID_Static_Price,
	ID_Combox_Price,
	ID_Static_Plus,
	ID_Edit_Price,
	ID_Edit_Dot,
	ID_Static_Dot,
	//��ʾ
	ID_Static_Tips,
	//����һ��
	ID_Static_SellPrice,
	ID_Static_BuyPrice,
	//�ɽ�ί�лر�
	ID_Static_MatchTips,

	ID_End
};

class SendOrderWnd : public TIxFrm, public IOperator, public ISQuoteDataSpi, public TradeSpiEx
{
public:
	SendOrderWnd();
	~SendOrderWnd();
	HANDLE										m_hQryMatch;
public:
	void Create(LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle = 0);
	void Register(SystemFrame* pdata);
	HWND GetQtyHwnd();
	void JumpOrderWnd(string agreement, string contractno, int qty);
	CxCombox		m_cbAgree;
public:
public:
	//////////////////////////////////////////////////////////////////////////ITradeSpi ���׻ص�
	virtual void __cdecl OnDisconnect(const TSocketDisconnectReason ErrorCode, const TLogin * pLogin);//����
	virtual void __cdecl OnInitProgress(const int ErrorCode, const TradeInitProgress CurrProgress, const TLoginBase * pLoginBase);	//���׳�ʼ�����
	virtual void __cdecl OnMatch(const TMatch * pMatch);											//�ɽ��ر� ˢ�ֲ֣����˴� �ֲ�ȫˢ ���ܼ�سֲ֣�
	virtual void __cdecl OnOrder(const TOrder * pOrder);											//ί�лر�

	//////////////////////////////////////////////////////////////////////////����ص� 	
	virtual void __stdcall OnQuote(const SContract* cont, const SQuoteUpdate* quote);				//��ͨ��������	
	virtual void __stdcall OnSpreadQuote(const SSpreadContract* scont) {};								//������������
protected:
	virtual void	SetContractNo(string sCode);
	virtual bool	SendOrder(MsgOrderInfo *msg);										//���ö�����Ϣ ����
	virtual bool	TableButtonToEdit();												//Tab�л�
private:
	//////////////////////////////////////////////////////////////////////////////
	HINSTANCE			m_hInstance;

	//////////////////////////////////////////////////////////////////////////////
	virtual LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);	//���ڴ������
	void OnCreate();
	void OnPaint();
	void OnSize();
	void OnCommand(WPARAM wParam, LPARAM lParam);
	void OnLButtonDown(const int x, const int y);
	void OnLButtonUp(const int x, const int y);
	void OnBtnClickDown(WPARAM wParam, LPARAM lParam);
	LRESULT OnCtlColor(WPARAM wParam, LPARAM lParam);
	void OnMouseWheel(WPARAM wParam, LPARAM lParam);
private:
	void OnSearchCode();
	void OnDealContractNo(WPARAM wParam, LPARAM lParam);
	void OnDealQty(WPARAM wParam, LPARAM lParam);
	void OnDealSwitch(WPARAM wParam, LPARAM lParam);
	void OnDealPrice();
	void OnDealAgree();
	void OnBtnPrice(int nID);
	void OnDealQuote();
	void OnKillFocusCode();								//��Լ����ʧȥ����
public:
	map<int, WNDPROC>		m_SetEdit;					//���໯���ڹ���
private:
	SystemFrame * m_System;
	//��Լ
	TStatic			m_labContract;
	CxEdit			m_editContract;
	TStaticButton   m_btnContract;
	//��ͬ
	TStatic			m_labAgree;

	//����
	TStatic			m_labQty;
	CxSpinEdit		m_spineditQty;
	TStatic			m_labLot;

	//�۸�
	TStatic		m_labPrice;
	CxCombox	m_cbPrice;
	TStatic		m_labPlus;
	CxEdit		m_editPrice;
	CxEdit		m_editDot;
	TStatic		m_labDot;
	//��ʾ
	TStatic		m_labTips;
	//����һ��
	TStatic		m_labSellPrice;
	TStatic		m_labBuyPrice;
	//�ɽ�ί�лر�
	TStatic		m_labMatchTips;

	CPopWinCode     m_popCode;
	//TAgreementPopWnd m_popAgree;

	//�µ���ť
	COrderButton		m_cOrderBtn;	


	string				m_curTcode;		//��ǰ���׺�Լ
	string				m_curScode;
	SOFCommodityInf		m_CommodityInfo;	//Ʒ����Ϣ

	//�ѳɽ������ϼ�
	int					m_finishedMatchQty;
	//��ǰ��ͬ
	SmsAgreementDataRsp	m_curAgree;
	//ÿ�ֳ���
	double				m_tradeDots;
	//��ǰ��ͬ�����������
	int					m_canSendLot;
	//��ǰOrderID
	int					m_curOrderID;
};

