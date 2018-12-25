#ifndef _POP_WIN_DATE_
#define _POP_WIN_DATE_

#define ND_UNIT_WIDTH	98
#define ND_UNIT_HEIGHT	29
#define IDC_MONCTRL     1000
#define ND_BORDER		2
class CPopWinDate :public TIxFrm
{
public:
	CPopWinDate();
	~CPopWinDate();

	void CreatePopWin(IOperator *opr, HWND parent, TMouValidStrW vType[10], int nSize, int nleft = 0, int ntop = 0);	//创建弹出窗口
private:
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	vector<wstring> m_vstring;
	HDC		m_HdcMem;		//缓存DC

	HBITMAP m_hBKBitmap;	//背景位图
	
	UINT	m_nPos;			//鼠标
	UINT    m_nRectSelect;  //

	RECT	 m_reClient;	//窗口矩形框

	int      m_nLeft;		//窗口坐标
	int      m_nTop;
	int		 m_nWidth;		//窗口宽度
	int		 m_nHeight;		//窗口高度

	HWND     m_hMonthCtrl;  //日期控件句柄
	RECT	 m_rcMonth;
	IOperator* m_opr;
};
#endif