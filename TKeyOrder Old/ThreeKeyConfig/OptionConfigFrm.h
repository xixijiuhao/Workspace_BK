#ifndef OPTION_CONFIG_FRM_H
#define OPTION_CONFIG_FRM_H
class OptionConfigFrm:public TIxFrm
{
public:
	OptionConfigFrm();
	~OptionConfigFrm();
	void ShowFrm(HWND hParent, RECT rect, UINT show);
	void MoveWindow(const int &x, const int &y, const int &cx, const int &cy);
	void RegistSt(OptionConfig* pstConfig);
protected:
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	void OnCreate();
	void OnPaint();
	void DrawTextMessage(TMemDC* pmemdc);
	void OnShowWindow(WPARAM wParam, LPARAM lParam);
	void OnClickCheckBox(WPARAM wParam, LPARAM lParam);
	void OnCommand(WPARAM wParam, LPARAM lParam);
private:
	void GetData();
	void UpdateConfig();
private:
	CxCheck  m_ckWndPosition;
	CxCheck	 m_ckDoublePositionConfirm;
	CxCheck  m_ckThreeKeyConfirm;
	CxCheck  m_ckDoubleClickPositionClose;
	CxCheck	 m_ckSplitBigOrder;//´óµ¥²ð·Ö


	CxCombox	m_cbPositionQty;
	CxCombox	m_cbBackFillOrderMod;
	CxCombox	m_cbProportionFillOrderMod;
	CxCombox	m_cbCloseAllFillOrderMod;

	CxCombox    m_cbHedge;

	OptionConfig* m_stConfig;

	const int m_iThreeKeyXoffset = -20;
	const int m_iThreeKeyYoffset = 10;
	const int CHECKBOXYOFFSET = 70;
};

#endif