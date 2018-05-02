#ifndef _POP_WIN_PRICE
#define _POP_WIN_PRICE

#define N_PRICE_WIDTH	80
#define N_PRICE_HEIGHT	20
#define WM_PRICEDATA_CHANGED  5006

class CPopWinPrice :public TIxFrm
{
public:
	CPopWinPrice();
	~CPopWinPrice();

	void CreatePopWin(IOperator*opr,LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle = 0, int nleft = 0, int ncenter = 0, UINT ncol = 1, UINT nrow = 9);	//������������

	bool UpdateData(SOFCommodityInf CommkodityInfo, double dMidprice, double dMaxprice, double dMinprice);

	void GetWinSize(int &nwidth,int &nheight);
private:
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	vector<string> m_vstring;
	HDC		m_HdcMem;		//����DC
	HDC		m_HdcUnit;		//��ԪDC
	
	HBITMAP m_hBKBitmap;	//����λͼ
	HBITMAP m_hFGBitmap;	//ǰ��λͼ ��Ԫ
	UINT	m_nPos;			//���
	
	int		m_nUintx;		//��Ԫ���
	int		m_nUinty;		//��Ԫ�߶�
	RECT	m_reUnit;		//��Ԫ���ο�
	RECT	 m_reClient;	//���ھ��ο�

	int      m_nLeft;		//��ʼ����
	int      m_nCenter;
	UINT     m_nCol;		//������
	UINT     m_nRow;
	int		 m_nWidth;		//���ڿ��
	int		 m_nHeight;		//���ڸ߶�

	bool	IsValidPrice(double dPrice);
	void	GetStrPriceByLineNo(int row, string &strPrice);
	double	GetPriceByLineNo(int row);
	bool	GetFractionPriceByLineNo(int row, SFractionPrice &tempPrice);
	void	DrawLineText(int nLine);
	SOFCommodityInf m_CommkodityInfo;
	double	m_MidPrice;	//���¼�
	double	m_MaxPrice;	//��ͣ��
	double	m_MinPrice;	//��ͣ��
	int		m_iIndex;	//����
	string	m_sPrice;	//�۸��ַ���

	IOperator* m_opr;
};
#endif