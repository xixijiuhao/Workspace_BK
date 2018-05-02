#include "PreInclude.h"
//extern vector<TPointOrderWnd*> g_pVecPointWnd;

void TPointOrderAPI::CreatePointOrder(POINT Position)
{
	if (!CConvert::IsUserLogin())
		return;
	/*if (g_pVecPointWnd.size() == 2)
		return;*/
	if (g_pVecPointWnd.size() == 2)
		return;
	TClickPriceOrderWnd* clickPriceOrder = new TClickPriceOrderWnd();
	clickPriceOrder->ShowFrm();
	clickPriceOrder->ReadConfigFile();
	g_pMainFrame->AddLinkage(clickPriceOrder->GetHwnd(), 0, TClickPriceOrderWnd::TOnLinkageAction);
	//g_pVecPointWnd.push_back(clickPriceOrder);
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