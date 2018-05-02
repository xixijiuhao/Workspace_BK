#pragma once

class TClickPriceOrderTipsFrm:public TIxFrm
{
private:
    explicit TClickPriceOrderTipsFrm();
public:
    static TClickPriceOrderTipsFrm& GetInstance();
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
    static TClickPriceOrderTipsFrm* __instance;
};