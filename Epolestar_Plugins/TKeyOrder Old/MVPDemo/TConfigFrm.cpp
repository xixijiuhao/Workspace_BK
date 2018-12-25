#include "BaseInclude.h"


TConfigFrm::TConfigFrm(IMediator *pMediator, HWND hParent) :IColleague(pMediator)
{
	m_hParent = hParent;
    m_frmChasePrice.RegistSt(&g_pGlobalModel->m_stChaseConfig);
	m_frmDefaultQty.RegistSt(&g_pGlobalModel->m_mapDefaultQty);
	m_frmOption.RegistSt(&g_pGlobalModel->m_stOptionConfig);
	m_frmSuperPrice.RegistSt(&g_pGlobalModel->m_mapSuperPrice);
	m_frmStopLoss.RegistSt(&g_pGlobalModel->m_stStopLossConfig);

	m_iNowSel = -1;
}

TConfigFrm::~TConfigFrm()
{

}

void TConfigFrm::SetShowRect(RECT rect)
{
	m_rcShow = rect; 
	Show(m_iNowSel, SWP_SHOWWINDOW);
}
void TConfigFrm::ShowFrm(int formerIndex, int nowIndex)
{
	Show(formerIndex, SWP_HIDEWINDOW);
	Show(nowIndex, SWP_SHOWWINDOW);
	m_iNowSel = nowIndex;
}

void TConfigFrm::Show(int index, UINT show)
{
	switch (index)
	{
	case ID_frmOptionConfig:
		m_frmOption.ShowFrm(m_hParent, m_rcShow, show);
		break;
	case ID_frmDefaultQty:
		m_frmDefaultQty.ShowFrm(m_hParent, m_rcShow, show);
		break;
	case ID_frmSuperPrice:
		m_frmSuperPrice.ShowFrm(m_hParent, m_rcShow, show);
		break;
	case ID_frmChasePrice:
		m_frmChasePrice.ShowFrm(m_hParent, m_rcShow, show);
		break;
	case ID_frmStopLoss:
		m_frmStopLoss.ShowFrm(m_hParent, m_rcShow, show);
		break;
	}
}

