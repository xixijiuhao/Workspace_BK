#pragma once


// CTabBeam

#define WM_GX_CANTABSWITCH	WM_USER + 200
#define WM_GX_TABSWITCH		WM_USER + 201
#define WM_PAGEPOPUP		WM_USER + 202

#if _MFC_VER <= 0x0300
typedef struct tagSCROLLINFO
{
	UINT    cbSize;
	UINT    fMask;
	int     nMin;
	int     nMax;
	UINT    nPage;
	int     nPos;
	int     nTrackPos;
}   SCROLLINFO, FAR *LPSCROLLINFO;
#endif

class CGXTabInfo: public CObject
{
public:
	int         nWidth;
	CString     sLabel;
	BOOL        bSel;
	CObject*    pExtra;
	BOOL        bDeleteExtra;
	HMENU       hMenu;
	BOOL        bMul;

	// Scrollbar Info
	SCROLLINFO  sbInfoVert,
				sbInfoHorz;

	// Color
	COLORREF rgbFaceNormal;
	COLORREF rgbFaceSel;
	CBrush   brFaceNormal;
	CBrush   brFaceSel;

	// bit-field flag to decide whether a tab should use its own color or
	// the default tab beam color.
	unsigned int flagUseOwnColorFaceNormal:1;  //1 bit flag
	unsigned int flagUseOwnColorFaceSel:1;

	CGXTabInfo();
	virtual ~CGXTabInfo();

};

enum TabStyle
{
	tsTrapezoid,	//����
	tsRectangle		//����
};
enum TBtnStyle
{
	bsNone,
	bsAuto,
	bsShortBtn,		//ǰ������������ͷ
	bsAllBtn		//ǰ������+��ǰ����ĸ���ͷ
};
enum TBtnAlient
{
	baLeft,
	baRight
};
class CTabBeam : public CWnd
{
	DECLARE_DYNCREATE(CTabBeam)
	//DECLARE_REGISTER()    // ���������Ǳ���ô�����ȥ�ģ�

// Construction, initialization
public:
	CTabBeam();
	BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID,TabStyle tabStyle= tsTrapezoid, TBtnStyle btnStyle = bsAuto, TBtnAlient btnAlient = baRight);
	void CreateFonts();
	void UpdateColors();
	void DisplayScrollBtns(TBtnStyle btnStyle = bsAuto, TBtnAlient btnAlient = baLeft);
	void ScrollTabInView(int nTab);

	void SetNotifyWnd(CWnd* pNotify);
	void SetTabBkColor(int nTab, COLORREF cr);
	void SetTabSelBkColor(int nTab, COLORREF cr);
	void SetTabMultiOption(int nTab, BOOL bMul);
	BOOL GetTabMultiOption(int nTab);

	void CaclTabsRect(CRect &rect);//�������б�ǩ������εĴ�С

	virtual ~CTabBeam();

// Attributes
public:
	LOGFONT		m_lfontCurrent;			// current tab font
	LOGFONT		m_lfontNormal;          // normal tab font
							            
	COLORREF    m_rgbTextCurrent;       // current tab text color
	COLORREF    m_rgbTextNormal;        // normal tab text color
	COLORREF    m_rgbFaceNormal;        // normal tab back color
	COLORREF    m_rgbFaceSel;           // selected tab back color
	COLORREF    m_rgbBackground;        // ������ɫ
	COLORREF	m_rgbTabBorder;			// ��ǩ������ɫ
	COLORREF	m_rgbDisableBtn;		// ��Ч��ť��ͼ����ɫ

	//��������Ŀǰ�����ھ��α�ǩ
	int			m_nDockLineWidth;		// ͣ���ָ��ߵĿ��
	BOOL		m_rgbDockLine;			// ͣ���ָ��ߵ���ɫ
	int			m_nSpace;				// ��ͼȡ�����ұ߽�ľ���

	CObArray    m_TabInfo;              // Tab Infos

	int         m_nTabs;                // no of tabs
	int         m_nCurrent;             // current tab
	int         m_nFirstTab;            // First Visible Tab

	int         m_nWidthEx;             // ��ǩ�������� Extra space at right and left side (triangles)
	int         m_nMargin;              // x coordinate of first tab
	int         m_nHeight;              // height of the beam
	int         m_nButtonWidth;         // width of scroll button (= scroll's thumb button)
	
	TabStyle	m_dwStyle;

	TBtnStyle	m_BtnStyle;
	TBtnAlient	m_BtnAlient;

	int			m_nHightBarHeight;		//height of bottom border of button
	COLORREF	m_rgbHightBar;			//color of bottom border of button

	BOOL        m_bLastVisible;         // Is Last Button Visible

	HCURSOR     m_hcurArrow;

	// Mouse Actions
	int         m_nBtnDown;
	BOOL        m_bBtnDown;
	BOOL        m_bMouseCapture;

	// Wnd to receive messages
	CWnd*       m_pNotifyWnd;
private:
	CFont		m_fontCurrent;
	CFont		m_fontNormal; 

	CBrush      m_brFaceNormal;
	CBrush      m_brFaceSel;
	CBrush      m_brBlack;

	CPen        m_penShadow;

	int GetBtnCount();
	bool IsShowBtns();
	int GetBtnOffset();
	int GetDrawTabLeft();//tab��ͼ���������
	void GetBtnRect(CRect &rect, int Btn=-1);
	void GetDefaultFont();

// Operations
public:
	virtual void    DrawTab(CDC* pDC, int nTab);
	virtual void	DrawTrapezoidTab(CDC* pDC, int nTab);//������tab
	virtual void	DrawRectangleTab(CDC* pDC, int nTab);//������tab
	virtual void    DrawButton(CDC* pDC, int i, BOOL bPressed);
	virtual int     CalcTabOffset(int nTab);
	virtual int     CalcTabFromOffset(int nOffset);
	virtual int     CalcLastVisibleTab();

	virtual void    AutoMove(UINT nRefMessage = WM_LBUTTONUP);
	virtual void    SwitchToTab(CPoint point);
	virtual int     ScrollBtnHit(CPoint point, int nButton = -1);
	virtual void    Scroll(int nDirection);

	// TabInfo
	CGXTabInfo&		GetTab(int nTab);
	virtual void    InsertTab(BOOL bInsert, LPCTSTR szLabel, CObject* pExtra = nullptr, BOOL bDelExtra = FALSE, int nWidth = -1, HMENU hMenu = 0, BOOL bMul=FALSE);
	virtual void    DeleteTab(int nTab);
	virtual void    AdjustTabSize(int nTab);
	virtual int     FindTab(const CObject *pExtra);

	// Current Selection, TabCount
	int     GetCurSel();
	void	SetCurSel(int nCurrent);
	int     GetCount();

// Implementation
public:
	static void AFXAPI PatB(CDC* pDC,int x,int y,int dx,int dy, COLORREF rgb);

protected:
	// Generated message map functions
	//{{AFX_MSG(CTabBeam)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnSysColorChange();
	afx_msg void OnWinIniChange(LPCTSTR lpszSection);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


