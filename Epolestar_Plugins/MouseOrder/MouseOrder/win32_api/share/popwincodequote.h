#ifndef _POP_WINQUOTE_CODE_
#define _POP_WINQUOTE_CODE_

//������Լѡ��
class CPopWinCodeQuote :public TIxFrm
{

public:
	CPopWinCodeQuote();
	~CPopWinCodeQuote();

	void	CreatePopWin(IOperator*opr, LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle = 0, int nleft = 0, int ntop = 0, UINT nWidth = 330, UINT nrow = 10);	//������������
	
	bool	UpdateData();		//���º�Լ��ʾ

	void	SelectCode();		//ѡ���Լ
	
	int		m_sel_row;			//�б���� ����ƶ�������Ϣ
	
	WNDPROC m_ListViewProc;		//�б����໯����
private:
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	
	RECT	 m_reClient;		//���ھ��ο�

	UINT     m_nRow;			//����
	int      m_nLeft;			//�����x����
	int      m_nTop;			//�����y����
	int		 m_nWidth;			//���ڿ��
	int		 m_nHeight;			//���ڸ߶�

	HWND     m_hListView;		//�б�ؼ����
	
	SContractNoType m_sCode;	//������Լ

	IOperator* m_opr;			//����ָ��	
};
#endif