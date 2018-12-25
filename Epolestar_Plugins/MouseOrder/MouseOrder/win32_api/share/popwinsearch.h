#ifndef _POP_WIN_SEARCH_
#define _POP_WIN_SEARCH_

#define N_WIN_HEIGHT	28
#define N_Search_Size	3
enum CPS_TYPE
{
	CPS_Normal,
	CPS_Advanced_Inner,
	CPS_Advanced_For,
	CPS_Strategy,
	CSP_END
};

#define TYPE_Inner		1
#define TYPE_For		2
#define TYPE_Both		3

struct RECTITEM
{
	int nx;
	int ny;
	bool bInCheck;
};
class CMousewin;
class CPopWinSearch :public TIxFrm
{
public:
	CPopWinSearch();
	~CPopWinSearch();

	void CreatePopWin(CMousewin *p, HWND parent, TMouPriceTypeStrW sMem[CSP_END][N_MAX_CHILD], int nSize[CSP_END], bool bShowLimitStopInner = false);	//������������

	void UpdateLayout(int nType = 0,bool bShowStrategy = true);

	void UpdateData(int nSize, TMouPriceTypeStrW str[N_MAX_CHILD]);

private:
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	void DrawFix(RECT rect, bool bCheck = false, bool bHover = false);

	void UpdateSelectItems(int nOldLoginType, bool bOldShowStrategy);
	void UpdateSelectItem(const wchar_t* str);
	void UpdateSelectItemToUI(bool bClearSpecial = false);

	bool IsInCheck(const RECT& rect,const POINT& point);
private:	
	TMouPriceTypeStrW m_string[CSP_END][N_MAX_CHILD];
	int		m_nSize[CSP_END];
	RECT	m_ItemRect[CSP_END][N_MAX_CHILD];
	int		m_nHead[CSP_END];
	HDC		m_HdcMem;		//����DC
	
	HBITMAP m_hBKBitmap;	//����λͼ
	
	RECT	m_reClient;	//���ھ��ο�

	int		m_nWidth;		//���ڿ��
	int		m_nHeight;		//���ڸ߶�

	int		m_nUnit;		//���ڿ��
	
	int		m_nCol;
	int		m_nRow;
	RECTITEM	m_CurItem;

	int		m_nChecked;
	
	const int m_nCheckWidth = 13;

	set<wstring> m_SelectedItem;
private:
	bool	m_bHover;		//����
	bool	m_bTracking;	//����
	int		m_LoginType;	//��¼����
	bool	m_bShowStrategy;
	bool	m_bShowLimitStopInner;	//����ֹ������
	bool    m_bFirst;
private:
	CMousewin *m_pOper;			//�ص��ӿ�
};
#endif