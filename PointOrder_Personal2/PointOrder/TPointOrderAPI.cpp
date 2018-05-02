#include "PreInclude.h"
extern vector<TPointOrderWnd*> g_pVecPointWnd;

void TPointOrderAPI::CreatePointOrder(POINT Position)
{
	if (!CConvert::IsUserLogin())
		return;
	if (g_pVecPointWnd.size() == 2)
		return;
	TPointOrderWnd* PointOrder = new TPointOrderWnd();
	PointOrder->ShowFrm();
	PointOrder->ReadConfigFile();
	g_pMainFrame->AddLinkage(PointOrder->GetHwnd(), 0, TPointOrderWnd::TOnLinkageAction);
	g_pVecPointWnd.push_back(PointOrder);
}

void TPointOrderAPI::CloseAll()
{
	while (g_pVecPointWnd.size())
	{
		g_pVecPointWnd[0]->WriteConfigFile();
		g_pMainFrame->DelLinkage(g_pVecPointWnd[0]->GetHwnd());
		DestroyWindow(g_pVecPointWnd[0]->GetHwnd());
	}
}