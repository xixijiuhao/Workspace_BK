#ifndef _MOUSEORDER_POP_WIN_
#define _MOUSEORDER_POP_WIN_

class CMouseOrderPopWin : public TBaseFrame, public ISQuoteEventSpi, public TradeSpiEx
{
public:
	CMouseOrderPopWin();
	~CMouseOrderPopWin();

	void Init();

	void Deinit();

	void ShowPopWin();
	
    bool GetRestorePos(char *property, RECT *rect);
    void RestorePos(RECT *rect);

	virtual void AdjustChildrenPos();

	bool IsGlobalMousePop(HWND hwnd);

    inline bool IsLock() const { return m_bLock; }
    inline void SetLock(bool b) { m_bLock = b; }

public:
    //连接成功
    virtual void __stdcall OnConnect() {}
    //连接断开
    virtual void __stdcall OnDisconnect() {}
    //登录应答
    virtual void __stdcall OnLogin(const SUserNoType user, const SErrCodeType err) {}
    //初始化完成（第二次登录不再初始化）
    virtual void __stdcall OnInitComplete();
    //交易初始化完成
    virtual void __cdecl OnInitProgress(const int ErrorCode, const TradeInitProgress CurrProgress
        , const TLoginBase * pLoginBase);			

public:
    void OnHotKeyDown(const bool Alt, const bool Ctrl, const bool Shift, const unsigned char VKey, const bool Exclusive, HWND source);

protected:
    virtual LRESULT OnMessage(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void OnPaint(const HDC hdc, const RECT& cr, const RECT& ur);
	
	virtual void OnMouseMove(WPARAM wParam, LPARAM lParam);
	virtual void OnMouseLeave(WPARAM wParam, LPARAM lParam);
	virtual void OnLButtonDown(WPARAM wParam, LPARAM lParam);
private:
	void LoadProperty(char(&cProperty)[MAX_PATH]);
	void SaveProperty(char(&cProperty)[MAX_PATH]);

    void UpdateMenuItem();
private:
	CMousewin* m_gmouseopr;

	RECT m_rtClose;

	bool m_bInCloseRect;

	char m_strPopwinProperty[MAX_PATH];			//参数保存文件路径	

    bool m_bLock;       /// 是否界面锁定

    std::vector<wstring>    m_vecMenuItemVert;
    std::vector<wstring>    m_vecMenuItemInner;
    std::vector<wstring>    m_vecMenuItemForeign;
    std::vector<wstring>    m_vecMenuItemStrategy;
    int                     m_nLoginType;   //内外盘
    bool                    m_bCompleted;
};
#endif