#include "PreInclude.h"
#define LEFT 10
#define TOP  5
#define RIGHT 130
#define BOTTOM 20
#define WIDGETH 20
#define WIDGETV 120
#define X_VAP  10
#define Y_VAP  8

//@String Define
static char szCHS[][120] = {
	"����׷�۲���: ", "�Զ�����׷�۴�������", "׷�ۼ۸�", "׷����ֹ", "��û�гɽ�",
	"����׷��", "�м�", "����ֹ", "1��׷��,��ָ�ڵ���û�м�ʱ�ɽ�������£�����ί�У��������ڳɽ��ļ۸�Ϊί�мۣ����·���ί�С�", "2��׷�ۼ۸�����Ϊ������׷�ۡ�ʱ�����״��µ��۸�����ϣ���һ�γ�1�����ڶ��γ�2�����Ժ�ÿ�ζ���3����λ��",
	"3��׷����ֹ�е����ֱ�ʾ׷���µ�ƫ���״��µ��۸񳬹����ٸ���λ��ֹͣ׷�ۡ���׷�ۼ۸�Ϊ�м�ʱ���ܴ�ѡ����ƣ�",
};
static char szCHT[][120] = {
	"�B�m׷�r����: ", "�Ԅ��B�m׷�r�|�l�l��", "׷�r�r��", "׷�r�Kֹ", "��]�гɽ�",
	"�A��׷�r", "�Ѓr", "���Kֹ", "1��׷�r,��ָ�چ��ӛ]�м��r�ɽ�����r�£�����ί�У�������춳ɽ��ăr���ί�Ѓr�����°l��ί�С�", "2��׷�r�r���O�Þ顰�A��׷�r���r�����״��΃r����A�ϣ���Ҽ�γ�1�����ڶ��γ�2��������ÿ�ζ���3���rλ��",
	"3��׷�r�Kֹ�еĔ��ֱ�ʾ׷�r��ƫ�x�״��΃r���^���ق��rλ��ֹͣ׷�r����׷�r�r����Ѓr�r���ܴ��x헿��ƣ�",
};
static char szENU[][120] = {
	"�B�m׷�r����: ", "�Ԅ��B�m׷�r�|�l�l��", "׷�r�r��", "׷�r�Kֹ", "��]�гɽ�",
	"�A��׷�r", "�Ѓr", "���Kֹ", "1��׷�r,��ָ�چ��ӛ]�м��r�ɽ�����r�£�����ί�У�������춳ɽ��ăr���ί�Ѓr�����°l��ί�С�", "2��׷�r�r���O�Þ顰�A��׷�r���r�����״��΃r����A�ϣ���Ҽ�γ�1�����ڶ��γ�2��������ÿ�ζ���3���rλ��",
	"3��׷�r�Kֹ�еĔ��ֱ�ʾ׷�r��ƫ�x�״��΃r���^���ق��rλ��ֹͣ׷�r����׷�r�r����Ѓr�r���ܴ��x헿��ƣ�",
};

static char* _GetStringById(int i)
{
	if (g_language == CHT)
	{
		if (i + 1 > (sizeof(szCHT) / sizeof(szCHT[0])))
			return 0;
		return szCHT[i];
	}
	else if (g_language == ENU)
	{
		if (i + 1 > (sizeof(szENU) / sizeof(szENU[0])))
			return 0;
		return szENU[i];
	}
	else
	{
		if (i + 1 > (sizeof(szCHS) / sizeof(szCHS[0])))
			return 0;
		return szCHS[i];
	}
}

ChasePriceConfig::ChasePriceConfig()
{

}
ChasePriceConfig::~ChasePriceConfig()
{

}

void ChasePriceConfig::ShowFrm(HWND hParent, RECT rect, UINT show)
{
	if (!m_Hwnd)
		CreateFrm(_T("ChasePriceConfig"), hParent, WS_CHILD | WS_CLIPCHILDREN | WS_VISIBLE);
	SetWindowPos(m_Hwnd, 0, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, show);
}

void ChasePriceConfig::MoveWindow(int x, int y, int cx, int cy)
{
	SetWindowPos(m_Hwnd, 0, x, y, cx, cy, SWP_NOZORDER);
	InvalidateRect(m_Hwnd, NULL, true);
}
void ChasePriceConfig::RegistSt(stChasePrice* pstChasePrice)
{
	if (pstChasePrice == NULL)return;
	m_pstChasePrice = pstChasePrice;
}
void ChasePriceConfig::UpdateConfig()
{
	m_spinSecond.SetWindowNumber(m_pstChasePrice->iSecond);
	m_comChasePrice.SetSelect(m_pstChasePrice->iChasePriceCom);
	m_comChaseOver.SetSelect(m_pstChasePrice->iChaseOver);
}
LRESULT ChasePriceConfig::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		OnCreate();
		return PROCESSED;
	case WM_PAINT:
		OnPaint();
		return PROCESSED;
	//case WM_COMMAND:
		OnComboxSel(wParam, lParam);
	//	return PROCESSED;
	case WM_COMMAND:
		OnCommand(wParam, lParam);
		return PROCESSED;
	case WM_NOTIFY:
		//OnSpinChange();
		return PROCESSED;
	case WM_CTLCOLORSTATIC:
		return OnCtrlColor(wParam, lParam);
	default:
		return NOT_PROCESSED;
	}
}
void ChasePriceConfig::OnCommand(WPARAM wParam, LPARAM lParam)
{
	WORD ctlId = LOWORD(wParam);
	if (ctlId == id_editSecond && HIWORD(wParam) == EN_CHANGE)
	{
		OnSpinChange();
	}
	else if (ctlId == id_cbChasePrice && HIWORD(wParam) == CBN_SELCHANGE)
	{
		int index = m_comChasePrice.GetSelect();
		if (index == 0)
			m_pstChasePrice->iChasePriceCom = ChaseLadderPrice;
		else if (index == 1)
			m_pstChasePrice->iChasePriceCom = ChaseMarketPrice;
	}
	else if (ctlId == id_cbChaseOver && HIWORD(wParam) == CBN_SELCHANGE)
	{
		m_pstChasePrice->iChaseOver = m_comChaseOver.GetSelect();
	}
}
void ChasePriceConfig::OnCreate()
{

	RECT rect = { LEFT, TOP, RIGHT, BOTTOM };
	m_labChaseConfig.Create(m_Hwnd, ANSIToUnicode(_GetStringById(0)).c_str());
	m_labChaseConfig.MoveWindow(rect.left, rect.top, WIDGETV, WIDGETH);

	rect.bottom = rect.bottom + Y_VAP;
	m_labChaseTrigger.Create(m_Hwnd, ANSIToUnicode(_GetStringById(1)).c_str());
 	m_labChaseTrigger.MoveWindow(rect.left, rect.bottom, WIDGETV + 10, WIDGETH);


	rect.bottom = rect.bottom + WIDGETH + Y_VAP;
	m_labChasePrice.Create(m_Hwnd, ANSIToUnicode(_GetStringById(2)).c_str());
 	m_labChasePrice.MoveWindow(rect.left, rect.bottom, WIDGETV - 30, WIDGETH);
 
	rect.bottom = rect.bottom + WIDGETH + Y_VAP;
	m_labchaseOver.Create(m_Hwnd, ANSIToUnicode(_GetStringById(3)).c_str());
	m_labchaseOver.MoveWindow(rect.left, rect.bottom, WIDGETV - 30, WIDGETH);
 
	rect.left = rect.left + WIDGETV - 30 + X_VAP; rect.top = rect.bottom; rect.right = rect.left + WIDGETV;
 	m_comChaseOver.Create(m_Hwnd,id_cbChaseOver);
 	m_comChaseOver.MoveWindow(rect.left, rect.top, WIDGETV, WIDGETH*5);
 
	rect.top = rect.top - WIDGETH - Y_VAP; rect.bottom = rect.top + WIDGETH;
  	m_comChasePrice.Create(m_Hwnd, id_cbChasePrice);
  	m_comChasePrice.MoveWindow(rect.left, rect.top, WIDGETV, WIDGETH*5);
  
  	rect.left = LEFT + WIDGETV + 3*X_VAP; rect.right = rect.left + WIDGETV/2  + X_VAP;
  	rect.top = TOP + WIDGETH  + 2; rect.bottom = rect.top + WIDGETH + 2;
  	
	m_spinSecond.Create(m_Hwnd, ES_NUMBER, id_editSecond);
	m_spinSecond.MoveWindow(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
  	m_spinSecond.SetFont(g_FontData.g_FontNum15);
	m_spinSecond.SetRange(1, 3600);
  
	rect.left = rect.right + X_VAP/2; rect.right = rect.left + WIDGETV;
	m_labChaseTriggerL.Create(m_Hwnd, ANSIToUnicode(_GetStringById(4)).c_str());
	m_labChaseTriggerL.MoveWindow(rect.left, rect.top, WIDGETV,  WIDGETH);

	rect.left = LEFT; rect.right = rect.left + WIDGETV; rect.top = 5 * WIDGETH + 5 * Y_VAP;
	rect.bottom = rect.top + WIDGETH;
	InitWidget();
}
void ChasePriceConfig::InitWidget()
{
	m_spinSecond.SetWindowNumber((int)m_pstChasePrice->iSecond);


	m_comChasePrice.AddString(ANSIToUnicode(_GetStringById(5)).c_str());
	m_comChasePrice.AddString(ANSIToUnicode(_GetStringById(6)).c_str());

	m_comChaseOver.AddString(ANSIToUnicode(_GetStringById(7)).c_str());
	m_comChaseOver.AddString(L"1");
	m_comChaseOver.AddString(L"2");
	m_comChaseOver.AddString(L"3");
	m_comChaseOver.AddString(L"4");
	m_comChaseOver.AddString(L"5");
	m_comChaseOver.AddString(L"6");
	m_comChaseOver.AddString(L"7");
	m_comChaseOver.AddString(L"8");
	m_comChaseOver.AddString(L"9");
	m_comChaseOver.AddString(L"10");


	m_comChasePrice.SetSelect(m_pstChasePrice->iChasePriceCom);
	m_comChaseOver.SetSelect(m_pstChasePrice->iChaseOver);
}
void ChasePriceConfig::OnPaint()
{
	TMemDC memdc(m_Hwnd);
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	FillRect(memdc.GetHdc(), &rect, g_ColorRefData.g_brush_bk);
	SelectFont(memdc.GetHdc(), g_FontData.g_FontWord12);
	SetBkMode(memdc.GetHdc(), TRANSPARENT);
	rect.left = LEFT; rect.right = rect.left + 3 * WIDGETV; rect.top = 5 * WIDGETH + 6 * Y_VAP;
	rect.bottom = rect.top + 1.5*WIDGETH;
	DrawText(memdc.GetHdc(), ANSIToUnicode(_GetStringById(8)).c_str(), -1, &rect, DT_WORDBREAK);
	rect.top = rect.bottom + Y_VAP; rect.bottom = rect.top + 1.5 * WIDGETH;
	DrawText(memdc.GetHdc(), ANSIToUnicode(_GetStringById(9)).c_str(), -1, &rect, DT_WORDBREAK);
	rect.top = rect.bottom + Y_VAP; rect.bottom = rect.top + 1.5 * WIDGETH;
	DrawText(memdc.GetHdc(), ANSIToUnicode(_GetStringById(10)).c_str(), -1, &rect, DT_WORDBREAK);
}
void ChasePriceConfig::OnComboxSel(WPARAM wParam, LPARAM lParam)
{
	HWND hwnd = (HWND)lParam;
	if (hwnd == m_comChasePrice.GetHwnd())
	{
		if (wParam == 0)
			m_pstChasePrice->iChasePriceCom = ChaseLadderPrice;
		else if (wParam == 1)
			m_pstChasePrice->iChasePriceCom = ChaseMarketPrice;

	}
	else if (hwnd = m_comChaseOver.GetHwnd())
	{
		m_pstChasePrice->iChaseOver = wParam;		
	}
}
void ChasePriceConfig::OnSpinChange()
{
	char cTemp[10] = { 0 };
	m_spinSecond.GetText(cTemp, sizeof(cTemp));
	if (atoi(cTemp) == 0)
	{
		m_spinSecond.SetWindowNumber(1);
		return;
	}
	m_pstChasePrice->iSecond = atoi(cTemp);
}