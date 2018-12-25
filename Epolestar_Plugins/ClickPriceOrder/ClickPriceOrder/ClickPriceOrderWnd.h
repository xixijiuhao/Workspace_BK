#pragma once
//乒乓配置
#include "CxCheck.h"
#include "CxSpin.h"
#include "CxEdit.h"
#include "CxButton.h"

class TCridHeader
{
public:
	TCridHeader();
	~TCridHeader();
public:
	void GetHeaderInfo(GridHeaderInfo& stInfo);
	void SetHeaderInfo(GridHeaderInfo stInfo);
	void DrawHeaderBase(TMemDC* pmemdc, const RECT& rect);

private:
	void DrawFrame(TMemDC* pmemdc,RECT rect);
	void DrawRow(TMemDC* pmemdc, RECT rect);
	void DrawRect(TMemDC* pmemdc, RECT rect, int iRow, int iCol);
	void DrawHeaderText(TMemDC* pmemdc, RECT rect,  int iRow, int iCol);
	void DrawFirstRowText(TMemDC* pmemdc, RECT rect, int iCol);
	void DrawSecondRowText(TMemDC* pmemdc, RECT rect, int iCol);
	void NumToChar(char* szText, int iSize, const int Num);

private:
	GridHeaderInfo m_stGridHeaderInfo;

private:
	const int m_iRow = 2;
	const int m_iCol = 9;
	const int m_iHeaderHeight = 20;
	const int m_iHeaderWidth = 50;
	COLORREF m_clBkGray;//顶头背景和边框的颜色;
	COLORREF m_clBkWhite;//白色
};


class TClickPriceOrderWnd :public TIxFrm, public IOperator , public TradeSpiEx
{
public:
	TClickPriceOrderWnd();
	void ShowFrm();
	void SetHeaderInfo(GridHeaderInfo stHeaderInfo);
	void OnCancelNotice(bool bsuccess);
    static void TOnHotKeyDown(const bool Alt, const bool Ctrl, const bool Shift, const unsigned char VKey, const bool Exclusive, HWND source);

	static void OnMenuClick(const unsigned int MenuIndex, const HWND PastLife);
	static void __cdecl TOnLinkageAction(HWND From, HWND To, const char* Sender, const char* Action, const char* Content);
	void ReadConfigFile();
	void WriteConfigFile();//增加记忆界面大小
private:
	void InitStConfig(char* buf);
	RECT m_rc;
private:
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	virtual void __cdecl OnInitProgress(const int ErrorCode, const TradeInitProgress CurrProgress, const TLoginBase * pLoginBase);
	void __cdecl OnDisconnect(const TSocketDisconnectReason ErrorCode, const TLogin * pLogin);
	void OnDealInitProcess();
	void OnDealHeaderChange(WPARAM wParam, LPARAM lparam);
	void OnMoney(WPARAM wParam, LPARAM lParam);
    LRESULT OnCtlColor(WPARAM wParam, LPARAM lParam);
	void OnDestroy(WPARAM wParam, LPARAM lParam);
private:
	virtual void SetPriceType(wstring str){}
	virtual void SetContractNo(string sCode);
private:
	void OnPaint();	
	void OnCreate();
	void OnSize(WPARAM wParam, LPARAM lParam);
	void OnSizing(WPARAM wParam, LPARAM lParam);
	LRESULT OnCommand(WPARAM wParam, LPARAM lParam);
	void OnDealCheck(WPARAM wParam);
	void OnDealEditChange(WPARAM wParam);
	void OnBtnClick(WPARAM wParam, LPARAM lParam);
	void OnComboxSel(WPARAM wParam, LPARAM lParam);
	LRESULT OnKeyDown(UINT message,WPARAM wParam, LPARAM lParam);
	void OnDealComContract();
	void OnDealHotKeyDirect();
	void OnSearchContract();
	void OnCancelAll();
	void OnCoverAll();
	void InsertCloseOrder(TSendOrder& stOrder);
	void InsertOrder(TSendOrder& stOrder);
private:
	bool m_bCoverAll;
	
private:
	TCanCelAllOrder m_clCanCel;
private:
	void OnLButtonDown(WPARAM wParam, LPARAM lParam);
	UINT OnNcHitTest(WPARAM wParam, LPARAM lParam);
	void OnSetCursor(WPARAM wParam, LPARAM lParam);
	void OnNcLButtonDown(WPARAM wParam, LPARAM lParam);
private:
	void CreateWidget();
	//void CreateTipFroRect();
	void InitWidget();
	void SetWidgetPos();
	void HidePPCtr();
	void SetBaseInfoPos();
	void SetHotKeyAndPPPos();
	void SetOrderFrmPos(int cy);	
	void SetGridHeaderPos();
	void SetComCtrPos();
	void InitRcContractInfo(POINT pt);
	void CreateSpinHwnd(HWND& hedit, HWND& hspin, int idEdit, int idSpin);
	void EnablePP(bool Enable);
private:
	void DrawClose(TMemDC *pMemDC);
	void DrawContractInfo(TMemDC *pMemDC);
	void DrawLine(TMemDC *pMemDC);
	void DrawExplain(TMemDC *pMemDC);
	void DrawExplainChinese(TMemDC* pMemDC);
	//void DrawBottomExplain(TMemDC* pMemDC);
	void GetContractInfo(char* Info, int iSize);

private:
	enum{ From_Quote = 0, From_TradeD, From_TradeDDB, From_SHTradeD };

	void DealNotice(const char * strContent, UINT iNoticeType);
    void DealHotKeySyn(WPARAM wParam, LPARAM lParam);
	void OnDealNotice(char* strContent);
	bool GetContractID(string &strContractId);
	void SetContractCode(string Code);
	void AddContractToComCtr(string Code);

private:
	static bool IsAuthent();
private:
	//设置提示信息
	HWND m_hCloseTips;
	HWND m_hSetTips;

private:
	RECT        m_rcGirdHeader;
	RECT        m_rcExplain;
	TCridHeader m_clHeader;

	//TPointOrderFrm  m_clOrderFrm;
	//控件
	TCombox       m_comContract;
	TStaticButton m_btnContract;
	CPopWinCode   m_popupContractCode;

	TStatic  m_scPositionQtyInfo;
	TStatic  m_scProfitInfo;
	TStatic  m_scFundInfo;

	TStatic  m_scPositionQty;
	TStatic  m_scProfit;
	TStatic  m_scFund;

	CxCheck   m_ckHotKey;
	TCombox   m_comHotKeyDirect;
	
	CxCheck   m_ckPP;
	TStatic   m_scPPBuy;
	TStatic   m_scPPBuyD;

	CxEdit    m_editBuy;
	CxSpin    m_spinBuy;
	TStatic   m_scPPSell;
	TStatic   m_scPPSellD;

	CxEdit    m_editSell;
	CxSpin    m_spinSell;


	TStatic   m_scOrderQty;
	CxEdit    m_editOrderQty;
	CxSpin    m_spinOrderQty;
	CxCheck   m_ckDeepQuote;
	CxCheck   m_ckLastPriceCenter;

	TStaticButton m_btnCancleOrder;
	TStaticButton m_btnCloseOrder;
private:
	POINT  m_ptSearchCode;
	RECT   m_rcContractInfo;
private:
	bool      m_bUserLogin;
	OrderInfo m_stOrderInfo;
	string    m_strNoticeContent;
	int       m_iNoticeType;
private:
	//PointOrderConfig m_stOrderConfig;
	//TPointConfig     m_clPointConfig;
private:
	HICON m_hSet;
	RECT m_rcClose;
	RECT m_rcSet;
	RECT m_rcOrderFrm;

	enum{ ID_COMCONTRACT = 0, ID_BTNSEARCH, ID_CKHOTKEY, ID_COMHOTKEYDIRECT, ID_CKPP,ID_EDITBUY, ID_SPINPPBUY,
		ID_EDITSELL, ID_SPINPPSELL, ID_EDITORDDERQTY, ID_SPINORDERQTY, ID_CKDEEPQUOTE, ID_CKLASTPRICECENTER, ID_BTNCANCEL,
		ID_BTNCLOSE
	};
};