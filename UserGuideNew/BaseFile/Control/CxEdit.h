#pragma once


class CxEdit :public CxSubClassWnd
{
public:

	enum{ floatStyle = 1, intStyle = ES_NUMBER, };

public:
	CxEdit();
	~CxEdit();
	void Create(HWND hParent, UINT style = 0, UINT specialStyle = 0, UINT id = 0);
	void SetLimitText(UINT length);
	void SetWindowNumber(int number);
	int  GetWindowNumber();
	double GetDoubleData();
	void SetDoubleData(double data);
	void SetText(const TCHAR *pText);
	/* length = sizeof(pText)/sizeof(pText[0]) */
	void GetText(TCHAR *pText, int length);
	void SetText(const char* pText);
	void GetText(char*pText, int length);

	bool IsEmpty();
protected:
	LRESULT CALLBACK WndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	bool m_bHover;
	bool m_bFocus;
	bool m_bTracking;
	HBRUSH m_ClientBrush;
	HBRUSH m_hOrderBrush;
	HBRUSH m_HotOrderBrush1;
	HBRUSH m_HotOrderBrush2;

	int    m_iExStyle;
	bool   m_bReturn;
	void DrawItemFrame(bool bHover, bool bFocus);
	bool DealFloat(WPARAM wParam, LPARAM lParam);
};

