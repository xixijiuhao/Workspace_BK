#ifndef _POP_WIN_TIP_
#define _POP_WIN_TIP_

class CPopWinTip :public TIxFrm
{
public:
	CPopWinTip();
	~CPopWinTip();

	void CreatePopWin(LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle = 0);	//������������
private:
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	wstring m_string;
	HDC		m_HdcMem;		//����DC
	
	HBITMAP m_hBKBitmap;	//����λͼ
	RECT	 m_reClient;	//���ھ��ο�

	int		 m_nWidth;		//���ڿ��
	int		 m_nHeight;		//���ڸ߶�

	BOOL    m_bMouseTrack;	//������
	bool    m_bHover;
};
#endif