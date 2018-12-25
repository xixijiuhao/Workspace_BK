#pragma once



class TPointOrderFrm : public TIxFrm
{
public:
    TPointOrderFrm();
    ~TPointOrderFrm();
public:
    bool Create(HWND hParent);
    void MoveWindow(const int &x, const int &y, const int &cx, const int &cy);
    void SetContractNo(string sCode);
    void RegistOrderInfo(OrderInfo stOrderInfo);
    void RegistConfig(PointOrderConfig *stConfig);
protected:
    void Init();
    void OnPaint();
    void OnMouseWheel(WPARAM wParam, LPARAM lParam);
    void OnSize(WPARAM wParam, LPARAM lParam);
    void OnLButtonDown(WPARAM wParam, LPARAM lParam);
    void OnRButtonDown(WPARAM wParam, LPARAM lParam);
    void OnLButtonUp(WPARAM wParam, LPARAM lParam);
    void OnLButtonDbClick(WPARAM wParam, LPARAM lParam);
    void OnMouseMove(WPARAM wParam, LPARAM lParam);
    void OnMouseLeave(WPARAM wParam, LPARAM lParam);
    void OnEnableWindow(WPARAM wParam, LPARAM lParam);
    void OnKeyUP(WPARAM wParam, LPARAM lParam);
    void OnKeyDown(WPARAM wParam, LPARAM lParam);  
    void OnVKeyDown(WPARAM wParam, LPARAM lParam);
    void OnHKeyDown(WPARAM wParam, LPARAM lParam);
    void OnSetFocus();
    void OnKillFocus();
    void OnClickOrder(WPARAM wParam, LPARAM lParam);
    void InsertConditionOrder(WPARAM wParam, LPARAM lParam);
    void DrawPosition(int iCol);

    void CancelParOrder(WPARAM wParam, LPARAM lParam);
    void CancelConditionOrder(WPARAM wParam, LPARAM lParam);
	int GetCol(int x);

    void DrawMainData(TMemDC &memdc);
    void OnQuote();
    void OnPosition();
    void OnContractDataInit();
    void OnOrder();
    bool FillOrder(TSendOrder &stSendOrder);
	SCommodity* pstCommodity;

protected:
    void SetQuoteHeaderInfo();
    void SetPositionHeaderInfo();
    void SetOrderHeaderIndo();
    void GetPrice(int iOffset, double &price);
protected:
    virtual LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
    bool	m_bMouseTrack;
    bool	m_bHover;
    bool	m_bHasFocus;
    int		m_KBFocusOffset;
    int     m_iMouseWheelOffset;
    WPARAM	m_MouseMoveOffset;
    //bool    m_bIsCtrlPressed;
    //bool    m_bIsAltPressed;
    int       m_iVisiabalRow;
    double             m_dLastPrice;
    double             m_dPreSettlePrice;
	bool m_bIsForeign = false;

private:
    //const int iWight_Width;
    const int iWight_Height;
private:
    TPointOrderFrmData m_clData;
	PingPangOrder      m_clPingPang;
    enum
    {
        ID_COL_OFFSET = 0, ID_COL_CONDITIONORDER, ID_COL_DIRECT, ID_COL_ORDERQTY
    };
	enum BuyLeft
    {
        ID_COL_LONGPOSITION = 0, ID_COL_LONGCONDITIONORDER, ID_COL_LONGPARORDER, ID_COL_LONGORDERQTY, ID_COL_ORDERPRICE,
        ID_COL_SHORTORDERQTY, ID_COL_SHORTPARORDER, ID_COL_SHORTCONDITIONORDER, ID_COL_SHORTPOSITION
    };
    TContractKey        m_stConKey;
    OrderInfo           m_stOrderInfo;
    string              m_sCode;
    GridHeaderInfo      m_stGridHeaderInfo;
    PointOrderConfig   *m_stConfig;
};

