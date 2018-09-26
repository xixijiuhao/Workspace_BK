#pragma once

class COFInputDlg;
class CSubClassEdit :public CxSubClassWnd
{
public:
	CSubClassEdit(unsigned int &, int editId, SOFCommodityInf&, COFInputDlg &);
	~CSubClassEdit();
protected:
	LRESULT CALLBACK WndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	//价格光标位于第几项 引用
	unsigned int &m_nCursorPosRef;	
	//合约窗口句柄
	int m_EditID;
	//inf
	SOFCommodityInf &m_CommodityInf;
	//上次焦点
	HWND m_hLastFocusWnd;
	//引用
	COFInputDlg &m_RefDlg;

	bool m_bHover;
	bool m_bFocus;
	bool m_bTracking;
	HBRUSH m_ClientBrush;
	HBRUSH m_hOrderBrush;
	HBRUSH m_HotOrderBrush1;
	HBRUSH m_HotOrderBrush2;
public:
	void InitEdit(HWND hwnd);
	//获取上次焦点
	HWND GetLastFocusWnd() { return m_hLastFocusWnd; }
private:
	//是否需要控制
	bool IsNeedControl(SContractNoType & contract_no, SQuoteSnapShot & tmpQuotSnap);
	//获取行情ID
	void GetRawContractID(SContractNoType & id);
	//获取价格小数点前后位数
	void GetPriceShowInf(const SQuoteSnapShot&tmpQuotSnap, bool &bNeedPoint, int &iPointForward, int &iPointBackward);
	//获取价格小数点前后位数
	bool GetIntByPrice(int &iForward, int &iBackward, int iPrecision, double LimitUpPrice, double LimitDownPrice);
	//是否是有效的字符输入
	bool IsValidPriceKeyDown(char key);
	//是否是有效的键盘输入
	bool IsValidPriceInput(HWND hwnd, char cCurInput, bool bNeedControl, bool bNeedPoint, int iPointForward, int iPointBackward);
};

