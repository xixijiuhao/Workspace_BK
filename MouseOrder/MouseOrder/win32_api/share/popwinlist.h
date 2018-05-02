#ifndef _POP_WIN_LIST_
#define _POP_WIN_LIST_

class CPopWinList :public TIxFrm
{
public:
	CPopWinList();
	~CPopWinList();

	void CreatePopWin(IOperator *p, HWND parent,int nwidth, int nleft, int ntop, int nRef, TMouPriceTypeStrW sMem[10], int nSize,int nRow, int nCol,bool bCombox = false);	//������������

	void UpdateData(TMouPriceTypeStrW sMem[10], int nSize);

	void UpdateUnit(int nWidth);
private:
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);

	void DrawList();
	void DrawCombox();
private:
	vector<wstring> m_vstring;

	HDC		m_HdcMem;		//����DC
	
	HBITMAP m_hBKBitmap;	//����λͼ
	
	UINT	m_nPos;			//���
	
	RECT	m_reClient;	//���ھ��ο�

	int     m_nLeft;
	int     m_nTop;
	int		m_nWidth;		//���ڿ��
	int		m_nHeight;		//���ڸ߶�

	int		m_nUnit;		//���ڿ��
	int		m_nRow;
	int		m_nCol;
private:
	bool	m_bHover;		//����
	bool	m_bTracking;	//����
	bool	m_bCombox;		//�߷�����
private:
	IOperator *m_pOper;			//�ص��ӿ�

	int    m_nRef;
};
#endif