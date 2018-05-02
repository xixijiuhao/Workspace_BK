#ifndef  _T_TRADEINFO_H_
#define  _T_TRADEINFO_H_

class TTradeInfo :public IColleague, TIxFrm
{
public:
	TTradeInfo(IMediator *pMediator, HWND hParent);
	~TTradeInfo();
	//设置显示大小
	void SetShowRect(RECT rect);
	//设置左边位置
	void SetViewLeft(int left){ m_iFrmLeft = left; }
	//显示窗口
	void ShowFrm(int formerIndex, int nowIndex);
	//绑定数据
    void BindingData();
	//初始化，刷一遍数据，持仓、委托、挂单、成交、止损止盈单等
    void InitListData();
	//更新挂单，选中order id， 是否刷新，如果当前挂单界面没有显示，就不刷新，等界面切过来时刷新
	void UpdateParOrder(uint iParOrderId, bool bUpdate =true);
	//更新持仓
	void UpdatePosition(bool bUpdate =true);
	//更新委托
    void UpdateOrder(TOrder* pOrder);
	//更新成交
    void UpdateMatch(TMatch* pMatch);
	//更新一条持仓数据
	void UpdateOnePosition(int i);
	//更新资金
	void UpdateMoney(const TMoney* pMoney, bool bUpdate =true);
	//设置所有列表都需要刷新
	void SetNeedUpdate();
	//更新止损止盈列表
	void UpdateStopLP(const TStopLPitem* pitem);
	//更新一条止损止盈数据
	void UpdateStopLPOneItem(int atIndex,int SelKeyID);
	//修改一条止损止盈数据
	void ModStopLPOneItem(int i, const TStopLPitem* pitem);
	//更新一条条件单
	bool UpdateOneConOrder(TConditionOrder *pOrder, bool bSetCount =true, int iRow = -1);
	//更新所有条件单
	void UpdateConOrder();
	//更新持仓浮盈
	void UpdateProfit(bool bUpdateRow =true); //是否更新每一行的数据
	//设置当前需选中的持仓，需在刷新持仓列表前调用
	void SetSelectedPosition(const TPosition * pPosition);
private:
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	void OnCreate();
	void OnPaint();
	void OnSize();
	void OnTabSelChange();
    void OnNotify(WPARAM wParam, LPARAM lParam);
	void OnCommand(WPARAM wParam, LPARAM lParam);
	void OnDrawItem(WPARAM wParam, LPARAM lParam);
	void OnMeasureItem(WPARAM wParam, LPARAM lParam);
private:
	void CreateWidget();
	void InitWidget();
    void InitMenu();
	void SetWidgetPos();
	void InitListView();
	void SetFirstPagePos(int cx, int cy);
	void FirstTabPageShow(UINT show);
    void EnableMenu(UINT which, bool bEnale);
    void ShowMenu(HMENU menu);

    void ShowOrderAllItem();
    void ShowOrderOneItem(TOrder* pOrder, int i);
    void UpdateOrderOneItem(TOrder* pOrder, int i);

    void ShowMatchOneItem(TMatch* pOrder, int i);
    void UpdateMatchOneItem(TMatch* pOrder, int i);
    void ShowMatchAllItem();

    void DealConditionOrder(LPARAM lParam);
    void DealStoplossOrder(LPARAM lParam);
    void DealMoneyList(LPARAM lParam);
    void DealOrderList(LPARAM lParam);
    void DealOrderState(LPARAM lParam);
    void DealMatchList(LPARAM lParam);
    void DealPosition(LPARAM lParam);
private:
	void AddProfit(UINT lvID, int iRow, int iCol, double data);

	void ChangePrice(uint type);

	void ShowTabFrm();

	void UpdateAvaQty(const TOrder* pOrder);

	void EnableConMenu(int index);
	void GetStopMethodText(int iMethod,char*pText, int length);
	void GetStopLossProfitText(TConditionOrder*pOrder, char* pLossText, char* pProfitText, int length);
private:
	TListView		m_listTradeInfo[6];
	TStaticButton	m_btnStrategy[6];
	HMENU			m_menuTradeInfo[6];
	TListView		m_listOrderState;
	HMENU			m_menuOrderState;
	TTabs			m_tabTradeInfo;
private:
	HWND			m_hParent;
	int				m_iFrmLeft;
	int				m_iFrmTop;
	int				m_iFrmWidth;
	int				m_iFrmHeight;
	bool			m_bShow;
	int             m_iTabSelect;
    TDataContainer* m_pConTotalPosition;
	TDataContainer* m_pConOrder;
	TDataContainer* m_pConParOrder;
	TDataContainer* m_pConMatch;
	TDataContainer* m_pConMoney;
	TDataContainer* m_pConLPOrder;

	vector<TConditionOrder*>*m_pVecConOrder;
	vector<TConditionOrder*>*m_pVecTrigConOrder;
	bool                     m_bShowTrig;
	bool					 m_bRunState;

	TSpecialPrice*  m_pSpeifiedCover;
	TShiftPosition* m_pShiftPosition;
	TModifyConView	m_clModifyConOrder;
	TMenuInfo       m_stMenuInfo;
	uint			m_iSelParOrderID;
	bool			m_bNeedUpdateOrder;
	bool			m_bNeedUpdateMatch;
	bool            m_bNeedUpdatePosition;
	bool			m_bNeedUpdateParOrder;
	bool			m_bNeddUpdateMoney;

	const TPosition *	    m_pSelectedPosition;
public:
	enum
	{
		ID_BtnCover33 = 1,
		ID_BtnCover50,
		ID_BtnCover100,
		ID_BtnBackHand,
		ID_BtnStopLoss,
		ID_BtnRisk,

		ID_ListPosition,
		ID_ListOrder,
		ID_ListMatch,
		ID_ListConOrder,
		ID_ListStopLossOrder,
		ID_ListMoney,
		ID_ListOrderState,
	};

    enum{
        IDM_RIVALCOLSE = 0, IDM_BACKHAND, IDM_CLOSE, IDM_STOPLOSS, IDM_STOPWIN, IDM_BREAKEVEN,
        IDM_TIMINGCLOSE, IDM_OVERCLOSE, IDM_MARKETCLOSE, IDM_SPECIFIEDCLOSE, IDM_CLOSESH, IDM_LOCKPOSITION,
        IDM_MOVEPOSITION, IDM_OVERBACKHAND, IDM_MARKETBACKHAND, IDM_SAVE, IDM_REFRESH, IDM_CANCELCLOSE, //持仓右键菜单
        //委托状态菜单右键
        IDM_CANCEL, IDM_EDITPRICE, IDM_RIVALFOLLOW, IDM_LINEUPFOLLOW, IDM_OVERFOLLOW,
        IDM_MARKETFOLLOW, IDM_CANCELALL, IDM_REFRESHORDERSTATE,
        //止损（赢）单右键
        IDM_STOPCANCEL, IDM_STOPCHANGE, IDM_STOPPAUSE, IDM_STOPSTART, IDM_STOPDELETEALL, IDM_TRIG, IDM_NOTTRIG,
        //条件单菜单
		IDM_COND_STOP,IDM_COND_START,
        IDM_COND_SENDORDER, IDM_COND_MODITFY, IDM_COND_DEL, IDM_COND_SENDALLPREORDER, IDM_COND_DELALL,
		IDM_COND_UNTRIG,IDM_COND_TRIG,
        //时间单右键
        IDM_TCOND_DEL, IDM_TCOND_SHOWTRIG, IDM_TCOND_SHOWNOTTRIG,
        //资金右键
        IDM_MONEYFRESH,
        //委托右键
        IDM_ORDER_SAVE, IDM_ORDER_FRESH,
        //成交右键
        IDM_MATCH_SVAE, IDM_MATCH_FRESH
    };
    enum{
		CMD_ViewTrade_BindPositionTotal,
		CMD_ViewTrade_BindParOrder,
		CMD_ViewTrade_BindOrder,
		CMD_ViewTrade_BindMatch,
		CMD_ViewTrade_BindMoney,
		CMD_ViewTrade_BindStopLPOrder,
		CMD_ViewTrade_BindConOrder,
		CMD_ViewTrade_BindTrigConOrder,

		CMD_ViewTrade_SetContract,
		CMD_ViewTrade_ClickOrderState,
		CMD_ViewTrade_ClickBtn,
		CMD_ViewTrade_OnCommand,
		CMD_ViewTrade_DbLClick,
		CMD_ViewTrade_UpdateMoney,
		CMD_ViewTrade_DbLClickOneLine,
		CMD_ViewTrade_ConMenuClick,
		CMD_ViewTrade_UpdateStopLP,

   };


};

#endif