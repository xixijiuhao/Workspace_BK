#ifndef _MOUSE_HEAD_WIN_
#define _MOUSE_HEAD_WIN_

#define N_MOUSEHEAD_WIDTH	(N_NORMAL_WIDYH-24)
#define N_MOUSEHEAD_HEIGHT	25

class CMouseHeadWin :public TIxFrm
{
public:
	CMouseHeadWin();
	~CMouseHeadWin();

	int CreateHeadWin(CMousewin*opr, LPCTSTR classname,int nLeft, HWND parent, DWORD style);	//������������

	int UpdateData(int nSize,TMouPriceTypeStrW str[10],bool bClearSpecial = false);

	bool SetShowData(const wchar_t* wText);

	void DrawTriangle(HDC &hdc, int nStartx, int nStarty, int nWidth);

	void DrawItem(int i);
private:
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	vector<wstring> m_vstring;

	wstring		m_sSpecial;

	HDC			m_HdcMem;		//����DC
	
	HBITMAP		m_hBKBitmap;	//����λͼ
	
	UINT		m_nPos;			//���
	UINT		m_nSelect;		//ѡ��
	int			m_nPosDown;

	int			m_nUintx;		//��Ԫ���

	RECT		m_reClient;		//���ھ��ο�

	int			m_nLeft;
	int			m_nTop;
	int			m_nWidth;		//���ڿ��
	int			m_nHeight;		//���ڸ߶�

	CMousewin*	m_opr;

	bool		m_bBatch;		//����
	bool		m_bTrader;		//����Ա
	bool		m_bStopTrade;	//ֹ��
	int			m_nInner;		//����
};
#endif