#pragma once

class TPointOrderTipsFrm:public TIxFrm
{
private:
    explicit TPointOrderTipsFrm();
public:
    static TPointOrderTipsFrm& GetInstance();
    void Create();
	void SetText(const char *pszText);

    void SetText(const TCHAR *pszText);
    void SetText(const int iNum);
private:
    void OnPaint();
    void OnTimer();
protected:
    virtual LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
    int m_iSecondCount;
    wstring m_wstrText;
    static TPointOrderTipsFrm* __instance;
};