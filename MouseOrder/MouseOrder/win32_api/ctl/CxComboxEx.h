#pragma once

class CxComboxEx :public CxSubClassWnd, public ICxStatic,public IOperator
{
public:
	CxComboxEx();
	~CxComboxEx();
	virtual void SetValidType(const wchar_t* sPType) {};
	virtual void SetMaxMinQty(const char* sQty) {};
	virtual bool SendOrder(MsgOrderInfo *msg) { return false; };
	virtual void SetContractNo(const char* sCode, CodeSetType csType = cstChange) {};
	virtual void SetPriceData(const char* sdata) {};
	virtual bool SetHeadData(const wchar_t* sPType, bool bHead = true) { return false; };
	virtual bool SetSelect(const wchar_t* sStrategy, int nRef);
	virtual bool TableButtonToEdit() { return false; };
	virtual bool CxGetWindowText(MH_TYPE m_type, int& nSize, wchar_t* wstr, int nMaxLen = LEN_RET) { return false; };
protected:
	LRESULT CALLBACK WndProc(UINT message, WPARAM wParam, LPARAM lParam);

	void OnPaint();

	void DrawTriangle(HDC &hdc, int nStartx, int nStarty, int nWidth);//�������
private:
	
	bool	m_bHover;

	bool	m_bTracking;
	//�ı�
	wchar_t	m_strText[LEN_RET];

	CPopWinList	m_hlist;

	TMouPriceTypeStrW m_stype[10];

	int		m_nSize;

	bool	m_bHasArrow;

public:
	void Init(HWND hwnd, TMouPriceTypeStrW m_stype[10],int nSize, bool bArrow = true);
	
	void CxSetWindowText(const wchar_t* wstr);//���ⲿ����
	
	void CxGetWindowText(wchar_t* wstr, int nMaxLen = LEN_RET);//���ⲿ����	

	void HidePopWin();
};

