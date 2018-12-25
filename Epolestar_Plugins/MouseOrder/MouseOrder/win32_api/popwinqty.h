#ifndef _POP_WIN_QTY_
#define _POP_WIN_QTY_

#define NQ_UNIT_WIDTH	37
#define NQ_UNIT_HEIGHT	22
#define WM_ORDERQTY_CHANGED  5003
#define NQ_BORDER		2
class CPopWinQty :public TIxFrm
{
public:
	CPopWinQty();
	~CPopWinQty();

	void CreatePopWin(IOperator*opr, LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle = 0, int nleft = 0, int ntop = 0, UINT ncol = 3, UINT nrow = 3);	//������������

private:
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	vector<string> m_vstring;
	HDC		 m_HdcMem;		//����DC
	
	HBITMAP  m_hBKBitmap;	//����λͼ
	
	UINT	 m_nPos;			//���
	
	RECT	 m_reClient;	//���ھ��ο�

	int      m_nLeft;
	int      m_nTop;
	UINT     m_nCol;
	UINT     m_nRow;
	int		 m_nWidth;		//���ڿ��
	int		 m_nHeight;		//���ڸ߶�

	IOperator* m_opr;
};
#endif