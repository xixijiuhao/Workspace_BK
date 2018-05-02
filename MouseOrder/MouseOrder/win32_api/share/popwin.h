#ifndef _POP_WIN_
#define _POP_WIN_

#define N_UNIT_WIDTH	80
#define N_UNIT_HEIGHT	40
#define WM_PRICETYPE_CHANGED  5001
class CPopWin :public TIxFrm
{
public:
	CPopWin();
	~CPopWin();

	void CreatePopWin(IOperator*opr, LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle = 0, int nleft = 0, int ntop = 0, UINT ncol = 3, UINT nrow = 3);	//������������

	void UpdateData(bool bBase = false);
private:
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	vector<wstring> m_vstring;
	HDC		m_HdcMem;		//����DC
	HDC		m_HdcUnit;		//��ԪDC
	
	HBITMAP m_hBKBitmap;	//����λͼ
	HBITMAP m_hFGBitmap;	//ǰ��λͼ ��Ԫ
	
	UINT	m_nPos;			//���
	
	int		m_nUintx;		//��Ԫ���
	int		m_nUinty;		//��Ԫ�߶�
	RECT	m_reUnit;		//��Ԫ���ο�

	RECT	 m_reClient;	//���ھ��ο�

	int      m_nLeft;
	int      m_nTop;
	UINT     m_nCol;
	UINT     m_nRow;
	int		 m_nWidth;		//���ڿ��
	int		 m_nHeight;		//���ڸ߶�

	IOperator* m_opr;

	sMousewinCfg m_MouseCfg;

	bool	m_bBase;
};
#endif