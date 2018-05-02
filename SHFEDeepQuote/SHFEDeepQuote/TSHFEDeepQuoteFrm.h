#pragma once
void OnLinkageAction(HWND From, HWND To, const char* Sender, const char* Action, const char* Content);

class SHFEDeepQuoteFrm :public VirtualWnd, public IOperator,public IQuoteUpdate
{
public:
    SHFEDeepQuoteFrm();
	~SHFEDeepQuoteFrm();
    static void TOnHotKeyDown(const bool Alt, const bool Ctrl, const bool Shift, const unsigned char VKey, const bool Exclusive, HWND source);
    static void OnMenuClick(const unsigned int MenuIndex, const HWND PastLife);
	void Update();
	ColumnSequence GetColumnSequence();
	string GetContractType() { return m_sContractType;}
	void SetContractType(string str) { m_sContractType = str; }
protected:
	virtual LRESULT OnDealMsg(UINT message, WPARAM wParam, LPARAM lParam);
    virtual void OnCreate();
	virtual LRESULT OnNcDestory(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnCommand(WPARAM wParam, LPARAM lParam);
	virtual void OnBtnClick(WPARAM wParam, LPARAM lParam);

    void OnDrawItem(WPARAM wParam, LPARAM lParam);
    void OnMeasureItem(WPARAM wParam, LPARAM lParam);
    LRESULT OnCtlColor(WPARAM wParam, LPARAM lParam);
	void OnComboxSel(WPARAM wParam, LPARAM lParam);
    void OnSearchContract();
    void OnSize(WPARAM wParam, LPARAM lParam);
private:
    void OnDealComContract();
    void OnTimer();
	void CreateWidget();
    void InitWidget();   //对空间进行初始化，设置字体，颜色，格式等
	void InitMenu();	 //初始化右键菜单
    void SetWidgetPos(); //设置控件的位置
    void AddContractToComCtr(string Code);
	void OnColumnSequence(WPARAM wParam);
	void RefreshData(ColumnSequence* clSequence);
	void RefreshColumn(ColumnSequence* clSequence);
	void SetColumn(int num);
	void SetItem(int num, int line,int column, TDeepQuote* pBuyQuote, TDeepQuote* pSellQuote,char* szBuyPrice,char * szSellPrice);

	void SetTotalQty(int buyQty, int sellQty);
	void UpdateQty();
private:
    virtual void SetPriceType(wstring str){}
    virtual void SetContractNo(string sCode);
private:
	void			OnContent(WPARAM wParam, LPARAM lParam);
	LRESULT			OnNotify(WPARAM wParam, LPARAM lParam);
	void			DealColumnHeader(LPARAM lParam);
	void			ShowMenu(HMENU hmenu);
	vector<string>	GetContract(string content, vector<string> vecstr);
	void			ShowContract(vector<string> vecstr);
	void			OnDestory();
	void			WriteColumnSequence();
	void			ReadColumnSequence();
	void			InitColumn(char* buffer, int column);
	void			GetColumnSequence(char* Columnname, int i);
	
private:    //控件
    TCombox				m_comContract;
    TStaticButton		m_btnContract;
    CPopWinCode			m_popupContractCode;
    TListView			m_listDeepQuotel;     
    TStatic				m_stCount[2];
    TCheckBox			m_ckthreshold[2];
    TPinEdit			m_threshold[2];
    TPinEdit			m_RefreshFrequence;
    TStatic				m_stRefreshFrequenceTips;
    OrderInfo			m_stOrderInfo;
    bool				m_bIsDataFinished;
    int					m_iRefreshFrequence;
    int					m_iTimerCount;
	HMENU				m_menuColumn;

	CMBLSpiHandler *	m_QuoteData;
	//TDeepQuoteData		m_QuoteData;
	ColumnSequence		*m_clSequence;
	string m_sContractType;

	
	enum {
		ID_LONGBUY = 0, ID_SHORTSELL, ID_COMCONTRACT, ID_BTNSEARCH, ID_PINQUOTE, ID_LISTDEEPQUOTEL, ID_MENUCOLUMN,
	};

	//new XZC 2017.8.2 11:12
	//增加档位
	POINT m_ptPopup;
	TStatic m_stQuote; //档位
	TPinEdit m_pinQuote;

	//
public:
	static bool bQuoteLoginSuccess;
};