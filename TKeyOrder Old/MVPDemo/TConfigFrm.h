#ifndef _T_CONFIG_FRM_H_
#define _T_CONFIG_FRM_H_


class TConfigFrm:public IColleague
{
public:
	TConfigFrm(IMediator *pMediator, HWND hParent);
	~TConfigFrm();
	void Init(HWND hParent){ m_hParent = hParent; }
	void ShowFrm(int formerIndex, int nowIndex);
	void SetShowRect(RECT rect);
private:
	void Show(int index, UINT show);
private:
	ChasePriceConfig   m_frmChasePrice;
	DefaultQtyConfig   m_frmDefaultQty;
	OptionConfigFrm    m_frmOption;
	SuperPriceConfig   m_frmSuperPrice;
	TStopLossParamFrm  m_frmStopLoss;
private:
	HWND    m_hParent;
	RECT    m_rcShow;
	int     m_iNowSel;

};


#endif
