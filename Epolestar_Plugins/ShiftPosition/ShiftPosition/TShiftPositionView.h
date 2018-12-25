#pragma once
struct LogCount  
{
	int iCount;
	int Orderid;
	void reSet(){
		iCount = 0;
		Orderid = -1;
	}
};
class Presenter;
void OnLinkageAction(HWND From, HWND To, const char* Sender, const char* Action, const char* Content);
static bool m_bNotLogin;
static bool m_bFirstStart;
class TShiftPosition :public TIxFrm, public TraderSpi, public IOperator, public ISQuoteDataSpi
{
public:
	TShiftPosition();
	~TShiftPosition();
	void ShowFrm();
	static void TOnHotKeyDown(const bool Alt, const bool Ctrl, const bool Shift, const unsigned char VKey, const bool Exclusive, HWND source);
	static void OnMenuClick(const unsigned int MenuIndex, const HWND PastLife);
	void UpdateShiftList();
	void ShowAllCondition(bool IsTrigger = false);
	void ShowOneShiftList(ShiftListInfo shift);
	int GetShiftSize(){ return m_mapShiftListInfo.size(); }
	double GetMiniPrice(char* ShiftContracNo, int& iPresicion, char* UserNo, char* UserNo2);
protected:
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
	//普通行情数据
	virtual void __stdcall OnQuote(const SContract* cont, const SQuoteUpdate* quote);
	//套利行情数据
	virtual void __stdcall OnSpreadQuote(const SSpreadContract* scont) {};
private:
	void OnCommand(WPARAM wParam, LPARAM lParam);
	void OnNotify(WPARAM wParam, LPARAM lParam);
	void OnBtnClickDown(WPARAM wParam, LPARAM lParam);	
	void OnBtnOk();
	void OnBtnCondition();
	void OnNcDestroy();
	void OnDestroy();
	void OnTapSeleted(WPARAM wParam, LPARAM lParam);
	void ShowTabList(int index);
	void CALLBACK LoginTimeProc(HWND, UINT, UINT_PTR, DWORD);
	void CALLBACK FirstStartWarning(HWND, UINT, UINT_PTR, DWORD);
private:
	void DealCbChange(WPARAM wParam);
	void DealChangeUserNo(int id);
	void WriteShiftContract();
	void ReadShiftContract();
	void GetShiftContent(char* ShiftContent, ShiftListInfo stShift);
	void InitShiftMap(char* buffer);
	char m_listShiftSelectNo[51];
private:
	//右键操作函数
	void CloseOrder(int OrderType);
	void CanCelOrder();
	void ModiftyOrder(int PriceType);
	void DeleteShiftContract();
	void DealAddCondition();
	void DealConditionSend();
	void DealConditionPause();
	void DealConditionStart();
	void DealConditionModify();
	void DealConditionDelete();
	void DealConditionAllPause();
	void DealConditionAllStart();
	void DealConditionAllDelete();
	void DealShowNoTrigger();
	void DealShowTrigger();
private:
	int m_indexShift;
private:
	void DealPositionList(LPARAM lParam);
	void DealShiftList(LPARAM lParam);
	void DealParOrderList(LPARAM lParam);
	void DealCondition(LPARAM lParam);
	void OnDealLog(TOrder* pst);
private:
	bool m_bIsFirstOrder;
	LogCount m_logCount;//记录日志的刷新次数；
private:
	void OnSearchCode(LPARAM lParam);
	bool m_bFirstContract;//记录是否点击第一腿合约选择按钮;
private:
	void SetUserInfo();
	void InitUserNo();
	void InitTradeData();
	int GetMaxOrderId();
private:
	void ShowAllParOrder();
	void ShowAllPosition();
	void ShowAllShift();


	void UpdateOneCondition(int i, ShiftCondition Condition);
	void UpdateParOrderList(WPARAM wParam, LPARAM lParam);
	void UpdatePositionList(WPARAM wParam, LPARAM lParam);
	void SubQuoteFromPos(const TPosition* pst);
	void SubQuoteFromShift();
	void UpdateOnePosition(int i,TPosition* pst);
	void UpdateOneParOrder(int i, TOrder* pst);
	void UpdateOneShiftList(int i, ShiftListInfo shift);
	void GetShowShiftName(ShiftListInfo shift, string& name, int size);
	int GetMiniPricePresicion(char* code1);
	
private:
	void OnPaint();
	void DrawClose(TMemDC *pMemDC);
	void DrawMini(TMemDC *pMemDC);
	void OnCreate();
	LRESULT OnCtlColor(WPARAM wParam, LPARAM lParam);
private:
	void OnSize(WPARAM wParam, LPARAM lParam);
	void OnSizing(WPARAM wParam, LPARAM lParam);
private:
	void CreateWidget();
	void InitWidget();
	void InitMenu();
	void ShowMenu(HMENU hmenu);
	void EnableMenu(uint id, bool bEnable);
	void InitListView();
	void InitListData();
	void InitOrderWidget();
	void SetWidgetPos();
	int	 m_iListShiftBottom;
	bool m_bMouseTrack;
	bool m_bIsTrigger;
	
private:
	TAddContractView m_AddContract;
	TShiftCondition  m_ShiftCondition;

	TListView	m_listShift;
	TListView   m_listCondition;
	TListView   m_listPosition;
	TListView   m_listParOrder;


	TStatic		m_labUserNo1;
	TStatic		m_labUserNo2;
	CxCombox	m_cbUserNo1;
	CxCombox	m_cbUserNo2;
	TStatic     m_labOrderContract;
	TStatic     m_labOrderQty;
	TStatic     m_labOrderType;
	TStatic     m_labMarketValue;

	TStatic     m_labFirstContract;
	TStatic     m_labSecondContract;

	CxEdit      m_editFirstContract;
	CxEdit      m_editSecondContract;

	CxEdit      m_editFirstOrderQty;
	CxEdit      m_editSecondOrderQty;
	CxSpin      m_spinFirstOrderQty;
	CxSpin      m_spinSecondOrderQty;
	TStatic     m_labFirstMarketValue;

	CxSpinEdit  m_sEditFirstOrderQty;
	CxSpinEdit  m_sEditSecondOrderQty;
	CxCombox    m_cbFirstOrderType;
	CxCombox    m_cbSecondOrderType;
	TStatic     m_labSecondMarketValue;

	TStaticButton   m_btnFirstContract;
	TStaticButton   m_btnSecondContract;
	CPopWinCode     m_popContractCode;
	CPopWinCodeList m_popContractList;
	bool            m_bShowCodeList;
	
	TStatic			m_labTotalQty;
	CxSpinEdit		m_sEditTotalQty;

	//超价设置
	TStatic			m_labFirstSuperPot;
	CxSpinEdit		m_sEditFirstSuperPot;
	TStatic			m_labSecondSuperPot;
	CxSpinEdit		m_sEditSecondSuperPot;
	//追价
	CxCheck			m_ckChasePrice;

	TStaticButton m_btnInsertOrder;
	TStaticButton m_btnConditionOrder;
	TStatic       m_labDirect;
	CxCombox      m_cbDirect;

	HMENU         m_menuPosition;
	HMENU         m_menuParOrder;
	HMENU         m_menuShift;
	HMENU         m_menuConditon;
	

	CxRichEdit    m_RichEdit;
	TTabs         m_tabShiftPosition;
	enum
	{
		//列表ID
		ID_LISTSHIFT = 1, ID_LISTPOSITION, ID_LISTPARORDER, ID_LISTCONDITION,
		//下单板
		ID_LAB_USERNO1, ID_CB_USERNO1, ID_LAB_USERNO2, ID_CB_USERNO2, ID_LAB_CONTRACT, ID_LAB_ORDERQTY, ID_LAB_ORDERTYPE, ID_LAB_FIRSTORDER, ID_LAB_SECONDORDER,
		ID_EDIT_FIRSTCONTRACT, ID_EDIT_FIRSTORDERQTY, ID_CB_FIRSTORDERTYPE, ID_EDIT_SECONDCONTRACT, ID_EDIT_SECONDORDERQTY,
		ID_CB_SECONDORDERTYPE, ID_BTN_CONTRACT, ID_BTN_OK, ID_SPIN_FIRSTORDERQTY, ID_SPIN_SECONDORDERQTY, ID_BTN_FIRSTCONTRACT, ID_BTN_SECONDCONTRACT,
		ID_LAB_DIRECT, ID_CB_DIRECT, ID_BTN_CONDITIONORDER, ID_SEDITFIRSTORDERQTY, ID_SEDITSECONDORDERQTY,ID_LAB_FIRSTSUPERPOT, ID_EDIT_FIRSTSUPERPOT,ID_SPIN_FIRSTSUPERPOT,
		ID_LAB_SECONDSUPERPOT,ID_EDIT_SECONDSUPERPOT,ID_SPIN_SECONDSUPERPOT,ID_CK_CHASEPRICE,ID_LAB_TOTALQTY,ID_EDIT_TOTALQTY,ID_SPIN_TOTALQTY,ID_STATIC_TOTALQTY,
		//右键MENU
		IDM_SHIFTADDNEWSHIFT, IDM_SHIFTDELETESHIFT, IDM_SHIFTLINE, IDM_SHIFTADDCONDITION,
		IDM_POSITTIONCOUNTERCLOSE, IDM_POSITIONMARKETCLOSE, IDM_POSITIONALLCLOSE, IDM_POSITIONFRESH,
		IDM_PARORDERCANCEL, IDM_PARORDERMARKETFOLLOW, IDM_PARORDERCOUNTERFOLLOW, IDM_PARORDERALLCANCEL, IDM_PARORDERFRESH,

		ID_RICHEDIT,
		//未触发条件单右键
		IDM_CONDITIONNO_SEND,
		IDM_CONDITIONNO_MODIFY, IDM_CONDITIONNO_DELETE, IDM_CONDITIONNO_PAUSE, IDM_CONDITIONNO_START, 
		IDM_CONDITIONNO_ALLPAUSE, IDM_CONDITIONNO_ALLSTART, IDM_CONDITIONNO_ALLDELETE,
		IDM_CONDITIONNO_TRIGGER, IDM_CONDITIONNO_NOTTRIGGER,
		//Timer ID 
		ID_Login_Delay,ID_ShiftPosition_Warning,
	};
protected:
	virtual void __cdecl OnDisconnect(const TSocketDisconnectReason ErrorCode, const TLogin * pLogin);
	virtual void __cdecl OnInitProgress(const int ErrorCode, const TradeInitProgress CurrProgress, const TLoginBase * pLoginBase);	//初始化进度通知
	virtual void __cdecl OnOrder(const TOrder * pOrder);
	virtual void __cdecl OnMatch(const TMatch * pMatch);							//成交消息通知
	virtual void __cdecl OnPositionTotal(const TTotalPosition * pPosition);			//持仓合计通知
private:
	RECT m_rcClose;
	RECT m_rcSet;
	RECT m_rcMini;
	TShiftConfig						m_clOrderConfig;       //移仓配置界面
private:
	TUserInfo                             m_stUserInfo1;            //当前选中的UserNo
	TUserInfo							  m_stUserInfo2;
	std::map<std::string, TUserInfo*>     m_mapUserInfo;
	std::map<std::string, ShiftListInfo>  m_mapShiftListInfo;           //移仓合约列表
	std::map<int, ShiftCondition>         m_mapShiftConditon;			//条件单
	std::vector<int>					  m_vectorOrderId;				//防止日志多次刷新


	TContainer                            m_PositionContainer;     //持仓列表
	TContainer                            m_ParOrderContainer;     //挂单列表

	TContainer                            m_UserPosition;           //当前客户持仓信息
	TContainer                            m_UserParOrder;			//当前客户挂单信息

	void OnDrawItem(WPARAM wParam, LPARAM lParam);
	void OnMeasureItem(WPARAM wParam, LPARAM lParam);

private:
	void OnMouseMove(WPARAM wParam, LPARAM lParam);
	void OnMouseLeave();
	void OnLButtonDown(WPARAM wParam, LPARAM lParam);
	UINT OnNcHitTest(WPARAM wParam, LPARAM lParam);
	void OnSetCursor(WPARAM wParam, LPARAM lParam);
	void OnNcLButtonDown(WPARAM wParam, LPARAM lParam);
	void GetParOrder(char* UserNo, char* sign);
	void GetPosition(char* UserNo, char* sign);
	bool IsUserNoInfo(const char* UserNo, const char* sign);
	bool IsAddOrUpdate(const TPosition* pPosition);
	void UpdateAvaQty(WPARAM wParam);
	void UpdateMarketValue();
	void SetMarketValue();
	bool FindItem(int& item, char* ShiftContractNo);
private:
	int m_iShiftOrderId;

private:
	virtual void SetPriceType(wstring str){}
	virtual void SetContractNo(string sCode);
	void SetContractNo(string Code, int iFirstOrSecond = 1);
	void SubQuote(string Code,int num);
	void OnTimer(WPARAM wParam);
	void OnMouseWheel(WPARAM wParam, LPARAM lParam);
	void KeyDown(WPARAM wParam, LPARAM lParam);
	void OrderStateToChar(int iOrderState, char* OrderState, int iSize);
	void DiffPriceTypeToChar(int iPriceType, char* cPriceType, int iSize);// DiffPrice
	void PriceConditionToChar(int iPriceCondition, char* cPriceCondition, int iSize); //>= <=
	void OrderTypeToChar(int iFirstType, int iSecondType, char* cOrderType, int iSize);

public:
	void TotalQtyWidget(bool single);
	bool QtyCheck(int Qty1, int Qty2, int Total, bool firstcolse);

private:
	
};
