#include "BaseInclude.h"
#define FRM_CONTRACTLIST_HEIGHT 150

TMainFrm::TMainFrm() 
{
	m_frmLeft = 2;
	m_bFirstShowSubView = true;
}
TMainFrm::~TMainFrm()
{
	
}

void TMainFrm::OnCreate()
{
	
}
void TMainFrm::OnPaint()
{
	__super::OnPaint();

}
LRESULT TMainFrm::OnDealMsg(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_SIZE:
		OnSize(wParam, lParam);
		return NOT_PROCESSED;
	default:
		return NOT_PROCESSED;
	}
}

LRESULT TMainFrm::OnNcDestory(WPARAM wParam, LPARAM lParam)
{
	return NOT_PROCESSED;
}

void TMainFrm::OnSize(WPARAM wParam, LPARAM lParam)
{
	int width = GET_X_LPARAM(lParam) - m_frmLeft*2;
	int height = GET_Y_LPARAM(lParam);

	//根据Mainframe提供的大小，动态改变尺寸；
	if (width >= KListView_Width) {
		ChangeFrameRect(width, height);
	}
}
void TMainFrm::CreateSubView()
{
	
}

void TMainFrm::CreateView()
{
	//需要嵌入到Mainframe框架中
	ShowFrm(FindWindow(EMainFrmName, NULL), L"arbitrage_frm", g_pLanguageApi->LangText(TitleStrID));
	g_pMainFrm->PutInner(this->GetHwnd());
}
void TMainFrm::setPresenter(KLinePresenter* presenter)
{
	if (presenter) {
		m_Presenter = presenter;;
	}
}
void TMainFrm::ChangeFrameRect(int width, int height)
{
	m_MainFrmRect.IntroductRect = { KLineView_LeftWidth, height - KLineIntroduct_Height , width - KListView_Width - KLineView_LeftWidth, KLineIntroduct_Height };
	m_MainFrmRect.CommodityListRect = { width - KListView_Width, CAPTION_HEIGHT + KLineView_TopHeight, KListView_Width , height - CAPTION_HEIGHT - KLineView_TopHeight };
	m_MainFrmRect.TopRect = { 0 , 0, width, CAPTION_HEIGHT };
	m_MainFrmRect.KLineChartRect = { 0, CAPTION_HEIGHT, width - KListView_Width, height - CAPTION_HEIGHT - KLineIntroduct_Height};
	if (m_bFirstShowSubView) 
	{
		m_Presenter->ShowSubView(m_MainFrmRect);
		m_bFirstShowSubView = false;
	}
		
	m_Presenter->ChangeSubViewFrame(m_MainFrmRect);
}