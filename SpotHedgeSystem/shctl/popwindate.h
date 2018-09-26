#ifndef _POP_WIN_DATE_
#define _POP_WIN_DATE_

#include "IxFrm.h"
#include <string>
#include <vector>
#include "IOperator.h"
#define ND_UNIT_WIDTH	80
#define ND_UNIT_HEIGHT	40
#define WM_VALIDTYPE_CHANGED  5002
#define IDC_MONCTRL     1000
using namespace std;
class CPopWinDate :public TIxFrm
{
public:
	CPopWinDate();
	~CPopWinDate();

	void CreatePopWin(IOperator*opr,LPCTSTR classname, HWND parent, DWORD style, vector<wstring>& vString, DWORD exstyle = 0, int nleft = 0, int ntop = 0, UINT ncol = 4, UINT nrow = 2);	//创建弹出窗口

	bool UpdateData(vector<wstring>& vString);

	bool GetCurOption(wstring & str);

	string CPopWinDate::GetTime();
private:
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	vector<wstring> m_vstring;
	HDC		m_HdcMem;		//缓存DC
	HDC		m_HdcUnit;		//单元DC
	HBRUSH	m_hBKBrush;		//背景画刷
	HBRUSH  m_hFGBrush;		//前景画刷 单元
	HBITMAP m_hBKBitmap;	//背景位图
	HBITMAP m_hFGBitmap;	//前景位图 单元
	HFONT   m_hFont;		//字体格式
	UINT	m_nPos;			//鼠标
	UINT    m_nRectSelect;  //
	
	int		m_nUintx;		//单元宽度
	int		m_nUinty;		//单元高度
	RECT	m_reUnit;		//单元矩形框

	COLORREF m_ColorFG;		//窗口前景色
	COLORREF m_ColorBK;		//窗口背景色
	RECT	 m_reClient;	//窗口矩形框

	int      m_nLeft;		//窗口坐标
	int      m_nTop;
	UINT     m_nCol;		//窗口行列数
	UINT     m_nRow;
	int		 m_nWidth;		//窗口宽度
	int		 m_nHeight;		//窗口高度

	HWND     m_hMonthCtrl;  //日期控件句柄

	wstring   m_sTime;

	IOperator* m_opr;
};
#endif