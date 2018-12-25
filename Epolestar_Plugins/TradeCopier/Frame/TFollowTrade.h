#ifndef _T_FOLLOW_TRADE_H_
#define _T_FOLLOW_TRADE_H_


enum{HasLogin,Logout};
class TFollowTrade :public TIxFrm,public TradeSpiEx
{
public:
	TFollowTrade();
	~TFollowTrade();
	void ShowFrm();
	void AddLog(UINT Action, const char * pszText, const TOrder* pOrder);
	static void OnMenuClick(const unsigned int MenuIndex, const HWND PastLife);
	static void OnHotKeyDown(const bool Alt, const bool Ctrl, const bool Shift, const unsigned char VKey, const bool Exclusive, HWND source);
	static void __cdecl OnReceiveNotice(HWND From, HWND To, const char* Sender, const char* Action, const char* Content) {};
protected:
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
	void Init();
	void ShowMenu(HMENU hMenu);
	void ShowLogin();
	void UpdateFollowLV();
	void UpdateMainLV();
	void UpdateConfig(int index);    //配置更新显示

	bool IsOrderIDExist(uint orderID);
	void CreateGUID(char *pszRemark,int length);
	void ReadConfigData(TFollowUser * pData);
	void ShowConfigFrm(int index);
protected:
	void OnCreate();
	void OnPaint();
	void OnDestroy();
	void OnTimer(WPARAM wParam, LPARAM lParam);
	void OnLButtonDown(WPARAM wParam, LPARAM lParam);
	void OnLButtonUp(WPARAM wParam, LPARAM lParam);
	void OnCommand(WPARAM wParam, LPARAM lParam);
	void OnNotify(WPARAM wParam, LPARAM lParam);
	void OnDrawItem(WPARAM wParam, LPARAM lParam);
	void OnMeasureItem(WPARAM wParam, LPARAM lParam);
	LRESULT OnCtlColor(WPARAM wParam, LPARAM lParam);
	void OnDealFollowLV(LPARAM lParam);
	void OnDealMainLV(LPARAM lParam);
	void OnUserLogin(LPARAM lParam);
	void OnClickBtn(LPARAM lParam);
	void DrawClose(TMemDC *pMemDC);

	//实现鼠标改变窗口大小
	UINT OnNcHitTest(WPARAM wParam, LPARAM lParam);
	void OnSetCursor(WPARAM wParam, LPARAM lParam);
	void OnNcLButtonDown(WPARAM wParam, LPARAM lParam);

	void OnSize(int cy);
	void OnSizing(WPARAM wParam,LPARAM lParam);
private:
	void OnOrder(WPARAM wParam, LPARAM lParam);
	void OnMainOrder(TOrder *pOrder);
	void OnMainOpen(const TOrder* pOrder, TSendOrder &stOrder, TOrderKey &orderKey);
	void OnFollowOrder(TOrder *pOrder);
	void DealFollowCover(TOrder *pOrder, TSendOrder &stOrder, TOrderKey &orderKey);
	void DealFollowCancel(const TOrder*pOrder, vector<TCancelQty>& vecQty);
	//void DealParOrderCancel(TOrder*pOrder); //处理挂单撤单，主账号撤单，跟随账号撤，

	void OnMainCancel(const TOrder* pOrder);
private:

	void FollowCover(TDataContainer &data, TFollowUser & stFollowUser); //平仓挂单追价

	double GetPrice(const TOrder* pOrder, int priceType, TDirect direct);  //获取价格
	void FollowMain(const TOrder* pOrder);
	void DealMainFilled(const TOrder* pOrder,bool bDoAction);
	void GetTime(char *pszText, int legnth = 12);

	bool CheckFollowParOrder(const TOrder* pOrder);
	TDirect ReverseDirect(TDirect direct);

	static bool IsAuthent();

	void CheckPositionBalance();
	void GetData(vector<const TPosition*> & mainPos, vector<const TPosition*> & followPos);
	void CheckFollowUserPos(TFollowUser& stUser, vector<const TPosition*>&mainPos, vector<const TPosition*>&followPos);
	void DealFollowPos(const TPosition*pPosition, int diffQty, TFollowUser*pFollowUser = nullptr);
	void DealFollowMore(vector<const TPosition*>&followPos, TFollowUser & stUser);


	void DeleteOrderKey();  //delete m_vOrderKey data
	void AfterUserLogin();
	void PushUser(TFollowUser &stUser, int type);
private:
	void OnLogout(TListView * listView);   //登出主账号还是跟随账号
private:
	bool CanTrade(const TPosition* pPosition);
private:
	int GetParOrderQty(TSendOrder & stOrder);
	int GetRealDiffQty(TSendOrder &stOrder, int diffQty);
	int GetMainParOrderQty(const TPosition* pFollowPos, TFollowUser & stFollowUser); //利用跟随账号的持仓来查找主账号的挂单
	bool IsMainUserOrder(const TOrder* pOrder);
	bool IsSameContract(const TOrder* pOrder, const TPosition* pPosition); //cmp TContractSingleKey

	void FollowMainCover(const TPosition* pPosition,const TOrder* pOrder, TSendOrder & stOrder, TOrderKey & orderKey);
private://交易回调
	virtual void __cdecl OnOrder(const TOrder * pOrder);
	virtual void __cdecl OnPositionTotal(const TTotalPosition * pPosition);	
	void __cdecl OnInitProgress(const int ErrorCode, const TradeInitProgress CurrProgress, const TLoginBase * pLoginBase);
	virtual void __cdecl OnDisconnect(const TSocketDisconnectReason ErrorCode, const TLogin * pLogin);
	virtual void __cdecl OnExchangeState(const TExchangeState * pExchangeState);
private:
	TStaticButton  m_btnStart;  //start follow trade
	TListView      m_lvMainAccount;   //主账户，被跟
	TListView      m_lvFoloowAccount; //跟单账户
	TStatic        m_labelFollow;     //显示跟随账户
	TStatic        m_labelMain;       //显示被跟账户
	HMENU          m_hFollowMenu;     //跟随账户菜单
	HMENU          m_hMainMenu;       //主账户菜单
	TRichEdit      m_editLog;         //交易日志
	TStatic		   m_labLogInfo;

	TradeInfo      m_frmTradeInfo;
private:
	char     m_cAddUser;
	vector<TFollowUser> m_followUser; //跟单账号
	vector<TUser> m_mainUser;		  //主账号
	vector<TOrderKey> m_vOrderKey;    //存储
	vector<TParOrderKey>m_coverOrderWait;//需要等主账号成交后再追的单子
	vector<TCancelOrderKey>m_followCancel; //需要等主账号成交后撤单的OrderId
	vector<TChaseInfo>m_coverOrderNow; //需要立即追的挂单，对应的是主张号撤单
	vector<TUser>    m_vecAllUser;
	bool     m_bHasStartFollow;       //是否已经启动了跟单

	RECT		   m_rcClose;
	int			   m_iEditHeight;
	int			   m_iEditTop;
	int			   m_iTradeInfoHeight;
	bool           m_bCloseClick;
	int			   m_iAllDisconnect; //标记用户是否都全部断开
	bool		   m_bUserLogin;  //标记用户登录，用户登录后十秒将 m_cAddUser = addNone;

	uint           m_iLogFileID;
	char		   m_cExchangeState[4];   //CFFEX DCE SHFE ZCE，保存该账号的各个交易所状态
	char		   m_szDisconnectReason[100];
	map<string, TTradeState>m_mapExchangeState; //key =EXNO+COMMODITYNO

	TradeModel    m_tradeModel;
private:
	enum{id_btnStart = 0,id_lvMainAccount,id_lvFollowAccount, id_labelMain,id_labelFollow};
	enum{idm_loginFollow = 1,idm_addFollow,idm_logoutFollow,idm_deleteFollow,idm_setFollow,
	     idm_loginMain,idm_addMain,idm_logoutMain,idm_deleteMain};
	enum{addNone,addFollowUser,addMainUser};
	enum{logoutMain,logoutFollow};

	enum{
		notUse,
		userAllDisconnect,
		userReConnect,
	};
public:
		enum{
			openAction,
			coverAction,
			cancelAction,
			cancelCover,
			chaseCoverAction,
			stopFollow,
			startFollow,
			welcomeUse,
			chaseEnd,
			followUserLogout,
			mainUserLogout,
			coverByUser,
			posNotBalance,
			mainUserRelogin,
			followUserRelogin,
			logMainUserLogin,
			logFollowUserLogin,
			logMainUserLogout,
			logFollowUserLogout,
		};
};

#endif