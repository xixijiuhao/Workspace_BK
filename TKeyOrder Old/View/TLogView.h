#ifndef _T_LOG_VIEW_H_
#define _T_LOG_VIEW_H_

class TLogView :public IColleague, public TIxFrm
{
public:
	TLogView(IMediator *pMediator, HWND hParent);
	~TLogView();
	void SetShowRect(RECT rect);
	void SetViewTop(int top){ m_iFrmTop = top; }
	void ShowFrm(int formerIndex, int nowIndex);
    void UpdateData();
    void UpdateLog();
private:
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	void OnCreate();
	void OnPaint();
	void OnSize();
    void OnClickBtn(WPARAM wParam, LPARAM lParam);
    LRESULT OnCtrlColor(WPARAM wParam, LPARAM lParam);

private:
	void CreateWidget();
	void InitWidget();
	void SetWidgetPos();
private:
	TStatic        m_labContractInfo;
	TRichEdit      m_editLog;
	TStatic        m_lablogConInfo;
	TStatic        m_lablogPosInfo;
	TStatic        m_lablogSell;
	TStatic        m_lablogSellQty;
	TStatic        m_lablogNewPrice;
	TStatic        m_lablogBuy;
	TStatic        m_lablogBuyQty;
	TStatic        m_lablogNewQty;
	TStaticButton  m_btnlogSell;
	TStaticButton  m_btnlogSellQty;
	TStaticButton  m_btnlogNewPrice;
	TStaticButton  m_btnlogBuy;
	TStaticButton  m_btnlogBuyQty;
	TStaticButton  m_btnlogNewQty;

	TStaticButton  m_btnOpenLoss;
	TStaticButton  m_btnConOrder;
private:
	HWND           m_hParent;
	int            m_iFrmWidth;
	int            m_iFrmHeight;
	int            m_iFrmTop;
	int            m_iFrmLeft;
	bool           m_bShow;
	int            m_iLogTop;
	int            m_iBtnTop;
	int            m_iActualHeight;
    TQuoteData*    m_pQuoteData;
    TLogData*      m_pLogData;
public:
	enum{
		ID_BtnSell = 1, ID_BtnSellQty, ID_BtnNewPrice, ID_BtnBuy, ID_BtnBuyQty,
		ID_BtnNewQty,ID_BtnOpenLoss,ID_BtnConOrder,
	};
    enum
    {
        CMD_TLog_GetQuoteData,
        CMD_TLog_GetLog,
        CMD_TLog_SetQty,
        CMD_TLog_Setprice,
		
		CMD_TLog_ClickBtn,
		CMD_TLog_OpenLoss,
		CMD_TLog_ConOrder,
    };
};


#endif