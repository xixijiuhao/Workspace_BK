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

	void CreatePopWin(IOperator*opr,LPCTSTR classname, HWND parent, DWORD style, vector<wstring>& vString, DWORD exstyle = 0, int nleft = 0, int ntop = 0, UINT ncol = 4, UINT nrow = 2);	//������������

	bool UpdateData(vector<wstring>& vString);

	bool GetCurOption(wstring & str);

	string CPopWinDate::GetTime();
private:
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	vector<wstring> m_vstring;
	HDC		m_HdcMem;		//����DC
	HDC		m_HdcUnit;		//��ԪDC
	HBRUSH	m_hBKBrush;		//������ˢ
	HBRUSH  m_hFGBrush;		//ǰ����ˢ ��Ԫ
	HBITMAP m_hBKBitmap;	//����λͼ
	HBITMAP m_hFGBitmap;	//ǰ��λͼ ��Ԫ
	HFONT   m_hFont;		//�����ʽ
	UINT	m_nPos;			//���
	UINT    m_nRectSelect;  //
	
	int		m_nUintx;		//��Ԫ���
	int		m_nUinty;		//��Ԫ�߶�
	RECT	m_reUnit;		//��Ԫ���ο�

	COLORREF m_ColorFG;		//����ǰ��ɫ
	COLORREF m_ColorBK;		//���ڱ���ɫ
	RECT	 m_reClient;	//���ھ��ο�

	int      m_nLeft;		//��������
	int      m_nTop;
	UINT     m_nCol;		//����������
	UINT     m_nRow;
	int		 m_nWidth;		//���ڿ��
	int		 m_nHeight;		//���ڸ߶�

	HWND     m_hMonthCtrl;  //���ڿؼ����

	wstring   m_sTime;

	IOperator* m_opr;
};
#endif