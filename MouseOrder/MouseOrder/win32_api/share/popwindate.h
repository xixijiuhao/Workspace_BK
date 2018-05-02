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

	void CreatePopWin(IOperator *opr, HWND parent, TMouValidStrW vType[10], int nSize, int nleft = 0, int ntop = 0);	//������������
private:
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	vector<wstring> m_vstring;
	HDC		m_HdcMem;		//����DC

	HBITMAP m_hBKBitmap;	//����λͼ
	
	UINT	m_nPos;			//���
	UINT    m_nRectSelect;  //

	RECT	 m_reClient;	//���ھ��ο�

	int      m_nLeft;		//��������
	int      m_nTop;
	int		 m_nWidth;		//���ڿ��
	int		 m_nHeight;		//���ڸ߶�

	HWND     m_hMonthCtrl;  //���ڿؼ����
	RECT	 m_rcMonth;
	IOperator* m_opr;
};
#endif