#ifndef _POP_WINQUOTE_CODE_
#define _POP_WINQUOTE_CODE_

//套利合约选择窗
class CPopWinCodeQuote :public TIxFrm
{

public:
	CPopWinCodeQuote();
	~CPopWinCodeQuote();

	void	CreatePopWin(IOperator*opr, LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle = 0, int nleft = 0, int ntop = 0, UINT nWidth = 330, UINT nrow = 10);	//创建弹出窗口
	
	bool	UpdateData();		//更新合约显示

	void	SelectCode();		//选择合约
	
	int		m_sel_row;			//列表框内 鼠标移动行列信息
	
	WNDPROC m_ListViewProc;		//列表子类化过程
private:
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	
	RECT	 m_reClient;		//窗口矩形框

	UINT     m_nRow;			//行数
	int      m_nLeft;			//左起点x坐标
	int      m_nTop;			//上起点y坐标
	int		 m_nWidth;			//窗口宽度
	int		 m_nHeight;			//窗口高度

	HWND     m_hListView;		//列表控件句柄
	
	SContractNoType m_sCode;	//套利合约

	IOperator* m_opr;			//操作指针	
};
#endif