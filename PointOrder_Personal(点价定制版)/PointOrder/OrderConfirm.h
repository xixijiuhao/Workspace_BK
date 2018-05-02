#pragma once


#include "CxCheck.h"
#include "CxSpin.h"
#include "CxEdit.h"
#include "CxButton.h"

class TOrderConfirm : public TModalWnd
{
public:
	TOrderConfirm(HWND hparent, UINT ntype);
	~TOrderConfirm();
public:
	void setContractInfo(char* ContractInfo) { memcpy(m_szContractInfo, ContractInfo, sizeof(m_szContractInfo)); };
	void setOrderQty(uint* OrderQty) {
		m_iOrderQty = OrderQty;
	};
	void setOrderPrice(char* OrderPrice) { memcpy(m_szOrderPrice, OrderPrice, sizeof(m_szOrderPrice)); };
protected:
	void OnSize();
	void OnButtonDown(WPARAM wParam, LPARAM lParam);
	void OnKeyDown(WPARAM wParam, LPARAM lParam);
	void OnMouseMove();
	void OnLbuttonDown(WPARAM wParam, LPARAM lParam);

protected:
	virtual	void	DrawMain(TMemDC* pmemdc);
	virtual	void	SetModalRect();
	virtual	void	CreateSubCtrl();
	virtual	LRESULT	WndProc(UINT message, WPARAM wParam, LPARAM lParam);

private:
	void SetBtnFocus();
	void SetBtnFocus(int id);
	void DrawFocusBtn(TMemDC* pmemdc, int id);
	void Result(int id);
	void CreateSpinHwnd(HWND& hedit, HWND& hspin, int idEdit, int idSpin);

protected:
	RECT m_rcOk;
	RECT m_rcNo;
	UINT m_ntype;

private:
	TStatic m_scContract;
	TStatic m_scContractInfo;
	TStatic m_scOrderQty;
	CxEdit  m_editOrderQty;
	CxSpin  m_spinOrderQty;
	TStatic m_scOrderPrice;
	TStatic m_scOrderPriceInfo;

	char m_szContractInfo[250];
	char m_szOrderPrice[250];
	uint*  m_iOrderQty;

private:
	bool  m_bFocusOk;
	bool  m_bFocusNo;
	enum {
		ID_OK = 0, ID_NO, ID_NULL,
		id_scContract, id_scContractInfo, id_scOrderQty, id_editOrderQty, id_spinOrderQty, id_scOrderPrice, id_scOrderPriceInfo
	};
};
//bool OrderConfirm_Demodal(HWND hwnd, const char* sztitle, const wchar_t* sztext, UINT ntype = TMB_OK);
bool OrderConfirm_Domodal(HWND hwnd, char* ContractInfo, uint* OrderQty, char* OrderPriceInfo, UINT ntype = TMB_OK);