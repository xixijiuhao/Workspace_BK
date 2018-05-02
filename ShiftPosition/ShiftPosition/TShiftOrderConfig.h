#pragma once
#include "CxSpin.h"
#include "CxEdit.h"
#include "CxCombox.h"
class TShiftConfig :public TIxFrm
{
public:
	void Create(HWND hParent);
	void MoveWindow(int x, int y);
	void RegistConfig(ShiftOrderConfig* stConfig);
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
private:
	void CreateWidget();
	void InitWidget();
	void SetWidgetPos();
	void InitRcClose();
	void DrawClose(TMemDC *pMemDC);
	void SingleOrder(WPARAM wParam);
private:
	//控件信息
	TStatic			m_labConfigName;

	TStatic			m_labTimeFore;
	CxSpinEdit		m_sEditTime;
	TStatic			m_labTimeLater;

	TStatic			m_labPrice;
	CxCombox		m_cbPrice;

	TStatic			m_labStop;
	CxCombox		m_cbStop;

	TStatic			m_labInsureType;
	CxCombox		m_cbInsureType;

	//策略设置
	CxCheck			m_ckSingleOrder;
	//策略注释
	TStatic			m_labStrategy;
	TStatic			m_labStrategyA;
	//条件单撤回设置
	CxCheck			m_ckCondition;
	CxSpinEdit		m_sEditConditionTime;
	TStatic			m_labConditionTimeLater;
private:
	HWND m_hParent;
	RECT m_rcClose;
private:
	const int CAPTION_H = 30;

private:
	ShiftOrderConfig* m_pstConfig;
	enum {
		id_labconfigname = 0,
		id_labtimefore, id_edittime,id_spintime, id_labtimelater,
		id_labprice, id_cbprice,
		id_labstop, id_cbstop,
		id_cksingle,
		id_ckcondition, id_editconditiontime, id_spinconditiontime,
		id_labinsuretype, id_cbinsuretype,
	};
};