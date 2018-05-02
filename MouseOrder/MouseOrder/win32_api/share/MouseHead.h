#ifndef _MOUSE_HEAD_WIN_
#define _MOUSE_HEAD_WIN_

#define N_MOUSEHEAD_WIDTH	(N_NORMAL_WIDYH-24)
#define N_MOUSEHEAD_HEIGHT	25

class CMouseHeadWin :public TIxFrm
{
public:
	CMouseHeadWin();
	~CMouseHeadWin();

	int CreateHeadWin(CMousewin*opr, LPCTSTR classname,int nLeft, HWND parent, DWORD style);	//创建弹出窗口

	int UpdateData(int nSize,TMouPriceTypeStrW str[10],bool bClearSpecial = false);

	bool SetShowData(const wchar_t* wText);

	void DrawTriangle(HDC &hdc, int nStartx, int nStarty, int nWidth);

	void DrawItem(int i);
private:
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	vector<wstring> m_vstring;

	wstring		m_sSpecial;

	HDC			m_HdcMem;		//缓存DC
	
	HBITMAP		m_hBKBitmap;	//背景位图
	
	UINT		m_nPos;			//鼠标
	UINT		m_nSelect;		//选中
	int			m_nPosDown;

	int			m_nUintx;		//单元宽度

	RECT		m_reClient;		//窗口矩形框

	int			m_nLeft;
	int			m_nTop;
	int			m_nWidth;		//窗口宽度
	int			m_nHeight;		//窗口高度

	CMousewin*	m_opr;

	bool		m_bBatch;		//批量
	bool		m_bTrader;		//交易员
	bool		m_bStopTrade;	//止损
	int			m_nInner;		//内盘
};
#endif