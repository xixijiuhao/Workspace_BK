#ifndef _POP_WIN_SEARCH_
#define _POP_WIN_SEARCH_

#define N_WIN_HEIGHT	28
#define N_Search_Size	3
enum CPS_TYPE
{
	CPS_Normal,
	CPS_Advanced_Inner,
	CPS_Advanced_For,
	CPS_Strategy,
	CSP_END
};

#define TYPE_Inner		1
#define TYPE_For		2
#define TYPE_Both		3

struct RECTITEM
{
	int nx;
	int ny;
	bool bInCheck;
};
class CMousewin;
class CPopWinSearch :public TIxFrm
{
public:
	CPopWinSearch();
	~CPopWinSearch();

	void CreatePopWin(CMousewin *p, HWND parent, TMouPriceTypeStrW sMem[CSP_END][N_MAX_CHILD], int nSize[CSP_END], bool bShowLimitStopInner = false);	//创建弹出窗口

	void UpdateLayout(int nType = 0,bool bShowStrategy = true);

	void UpdateData(int nSize, TMouPriceTypeStrW str[N_MAX_CHILD]);

private:
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	void DrawFix(RECT rect, bool bCheck = false, bool bHover = false);

	void UpdateSelectItems(int nOldLoginType, bool bOldShowStrategy);
	void UpdateSelectItem(const wchar_t* str);
	void UpdateSelectItemToUI(bool bClearSpecial = false);

	bool IsInCheck(const RECT& rect,const POINT& point);
private:	
	TMouPriceTypeStrW m_string[CSP_END][N_MAX_CHILD];
	int		m_nSize[CSP_END];
	RECT	m_ItemRect[CSP_END][N_MAX_CHILD];
	int		m_nHead[CSP_END];
	HDC		m_HdcMem;		//缓存DC
	
	HBITMAP m_hBKBitmap;	//背景位图
	
	RECT	m_reClient;	//窗口矩形框

	int		m_nWidth;		//窗口宽度
	int		m_nHeight;		//窗口高度

	int		m_nUnit;		//窗口宽度
	
	int		m_nCol;
	int		m_nRow;
	RECTITEM	m_CurItem;

	int		m_nChecked;
	
	const int m_nCheckWidth = 13;

	set<wstring> m_SelectedItem;
private:
	bool	m_bHover;		//激活
	bool	m_bTracking;	//跟踪
	int		m_LoginType;	//登录类型
	bool	m_bShowStrategy;
	bool	m_bShowLimitStopInner;	//内盘止损配置
	bool    m_bFirst;
private:
	CMousewin *m_pOper;			//回调接口
};
#endif