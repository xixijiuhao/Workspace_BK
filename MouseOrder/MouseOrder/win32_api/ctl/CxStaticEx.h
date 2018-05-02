#pragma once

enum PopType
{
	INNER,
	FOREIGN
};
class ICxStatic
{
public:
	virtual void CxGetWindowText(wchar_t* wstr, int nMaxLen = LEN_RET) = 0;//供外部调用	
};
class CxStaticEx :public CxSubClassWnd,public ICxStatic
{
public:
	CxStaticEx();
	~CxStaticEx();
protected:
	LRESULT CALLBACK WndProc(UINT message, WPARAM wParam, LPARAM lParam);

	void OnPaint();

	void DrawTriangle2(HDC &hdc, int nStartx, int nStarty, int nWidth);
private:
	
	bool	m_bHover;

	bool	m_bNeedHover;

	bool	m_bTracking;
	//文本
	wchar_t	m_strText[LEN_RET];

	CPopWinList	m_hlist[FOREIGN+1];

	PopType		m_nType;

	IOperator*	m_oper;

	TMouPriceTypeStrW m_stype[FOREIGN + 1][10];

	int m_nSize[FOREIGN + 1];

	int m_nRow[FOREIGN + 1];

	int m_nCol[FOREIGN + 1];

	int m_nRef;
public:
	void Init(IOperator *p, HWND hwnd,int nRef, TMouPriceTypeStrW stype[FOREIGN + 1][10], int nSize[FOREIGN + 1], int nRow[FOREIGN + 1], int nCol[FOREIGN + 1], PopType ntype = INNER,bool bNeedHover = false);
	
	void UpdatePopType(PopType ntype = INNER);
	
	PopType GetPopType(){ return m_nType; };
	
	void CxSetWindowText(const wchar_t* wstr);//供外部调用
	
	void CxGetWindowText(wchar_t* wstr, int nMaxLen = LEN_RET);//供外部调用	

	int CxCurrentSize() { return m_nSize[m_nType]; };//供外部调用	

	void HidePopWin();
};

