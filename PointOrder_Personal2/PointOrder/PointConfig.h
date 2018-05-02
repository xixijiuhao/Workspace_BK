#pragma once
#include "CxSpin.h"
#include "CxEdit.h"
#include "CxCombox.h"
class TPointConfig :public TIxFrm
{
public:
	void Create(HWND hParent);
	void MoveWindow(int x, int y);
	void RegistConfig(PointOrderConfig* stConfig);
private:
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	void OnPaint();
	void OnCreate();
	void OnSize(WPARAM wParam, LPARAM lParam);
	LRESULT OnCommand(WPARAM wParam, LPARAM lParam);
	LRESULT OnCtrlColor(WPARAM wParam, LPARAM lParam);
private:
	void OnLButtonDown(WPARAM wParam, LPARAM lParam);
	void OnDealBuySellDirection();
	void OnDealPriceType();
	void OnDealMessageBox();
	void OnDealComparePrice();
private:
	void CreateWidget();
	void InitWidget();
	void SetWidgetPos();
	void HideSomeConfig();
	void InitRcClose();
	void DrawClose(TMemDC *pMemDC);
private:
	//�ؼ���Ϣ
	TStatic  m_scCoverType;
	TStatic  m_scQueueTime;
	TStatic  m_scMessageBox;
	TStatic	 m_scComparePrice;
	TStatic  m_scBuySellDirection;

	CxCombox m_cbCoverType;
	CxCombox m_cbMessageBox;
	CxCombox m_cbComparePrice;
	CxCombox m_cbBuySellDirection;

	CxEdit  m_editTime;
	CxSpin  m_spinTime;
	TStatic m_scSecond;

	TStatic m_scExQueueTime;
private:
	HWND m_hParent;
	RECT m_rcClose;
	bool m_bQueueTime;//��־ƹ�����õ�ʱ���Ƿ����
private:
	const int CAPTION_H = 30;

private:
	PointOrderConfig* m_pstConfig;
	enum{
		ID_COMPRICETYPE = 0, ID_EDITSECOND, ID_SPINSECOND,
		ID_COMMESSAGEBOX, ID_COMCOMPAREPRE,
		ID_COMBUYSELLDIRECTION
	};
};