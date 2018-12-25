#include "PreInclude.h"

#define cCAPTION_H  30
#define cFRAME_H    1280
#define cFRAME_V    618


#define  ccloseWidth 13

extern G_COLOREF			g_ColorRefData;				//ɫ��
extern G_UIFONT				g_FontData;					//����
extern ISpotSystemApi		*g_pSpotSystem;				//�ױ�ƽ̨
StatementWnd::StatementWnd()
	:wtitle(L"���˵�"),  m_bInCloseRect(false), m_subSystem(nullptr)
{
	m_rcClose.right = cFRAME_H - 8;
	m_rcClose.left = m_rcClose.right - ccloseWidth;
	m_rcClose.top = 10;
	m_rcClose.bottom = m_rcClose.top + ccloseWidth;
	m_bMouseTrack = false;
}

//StatementWnd::~StatementWnd()
//{
//	
//}

void StatementWnd::ShowFrm(HWND hParent)
{
	CreateFrm(L"TStatementWnd", hParent, WS_VISIBLE | WS_POPUP | WS_CLIPCHILDREN | WS_SYSMENU, WS_EX_TOOLWINDOW | WS_EX_CONTROLPARENT);
	POINT pt;
	GetCursorPos(&pt);
	SetWindowPos(m_Hwnd, HWND_TOP, pt.x, pt.y, cFRAME_H, cFRAME_V, SWP_SHOWWINDOW);
}

void StatementWnd::Register(SystemFrame * pdata)
{
	if (!pdata)return;
	m_subSystem = pdata;
}

void StatementWnd::CalculateData(string begin, string end)
{
	int beginDay = DateStrToInt(begin);
	int endDay = DateStrToInt(end);
	m_listAll.DeleteAllItems();
	m_vecOpenAgree.clear();				//���ֺ�ͬ
	m_vecCloseAgree.clear();			//ƽ�ֺ�ͬ
	m_vecAgreeRel.clear();				//��ͬ��ϵ
	m_vecCashInout.clear();				//�����
	m_vecFund.clear();					//�ʽ�
	m_vecOpenMatch.clear();					//�ɽ�
	m_vecOpenOrder.clear();					//ί��
	m_vecCloseMatch.clear();					//�ɽ�
	m_vecCloseOrder.clear();					//ί��
	m_vecSpec.clear();					//���
	double		tax;					//˰��

	string key = m_subSystem->get_curkey();
	//��ͬ
	for (auto it_agree : m_subSystem->m_map_mapAgreement[key])
	{
		if (DateStrToInt(it_agree.second.SignDate) >= beginDay && DateStrToInt(it_agree.second.SignDate) <= endDay)
		{
			if(it_agree.second.AgreementType == SMS_AGREEMENT_PURCHASE)
				m_vecOpenAgree.push_back(it_agree.second);
			else
				m_vecCloseAgree.push_back(it_agree.second);
		}
	}
	//��ͬ��ϵ
	for (auto it_agreerel : m_subSystem->m_mapAgreementRel[key])
	{
		m_vecAgreeRel.push_back(it_agreerel);
	}
	//�����
	for (auto it_cash : m_subSystem->m_vecCashInout)
	{
		if (DateStrToInt(it_cash.SettleDate) >= beginDay && DateStrToInt(it_cash.SettleDate) <= endDay)
			m_vecCashInout.push_back(it_cash);
	}
	//�ʽ�
	for (auto it_fund : m_subSystem->m_vecFund)
	{
		if (DateStrToInt(it_fund.SettleDate) >= beginDay && DateStrToInt(it_fund.SettleDate) <= endDay)
			m_vecFund.push_back(it_fund);
	}
	//ί��
	for (auto it_order : m_subSystem->m_vecOrder)
	{
		if (DateStrToInt(it_order.SettleDate) >= beginDay && DateStrToInt(it_order.SettleDate) <= endDay)
		{
			if (it_order.Offset == SMS_O_OPEN)
				m_vecOpenOrder.push_back(it_order);
			else
				m_vecCloseOrder.push_back(it_order);
		}
	}
	//�ɽ�
	//for (auto it_match : m_subSystem->m_mapMatch)
	//{
	//	if (DateStrToInt(it_match.SettleDate) >= beginDay && DateStrToInt(it_match.SettleDate) <= endDay)
	//	{
	//		if (it_match.Offset == SMS_O_OPEN)
	//			m_vecOpenMatch.push_back(it_match);
	//		else
	//			m_vecCloseMatch.push_back(it_match);
	//	}
	//}
	//���
	for (auto it_spec : m_subSystem->m_mapSpec)
	{
		m_vecSpec.push_back(it_spec.second);
	}
	//˰��
	if (!m_subSystem->m_setTax.empty())
	{
		auto it = m_subSystem->m_setTax.rbegin();
		tax = (*it).Tax;
	}

	//////////////////////////////////////����///////////////////////////////////////////////
	int row = 0;
	m_listAll.AddItem("", row, 0, DT_CENTER, g_ColorRefData.GetColorBlack());
	m_listAll.AddItem(m_vecFund.front().Equity, row, 28, 2, DT_CENTER, g_ColorRefData.GetColorBlack());			//�ڳ�Ȩ�棨��棿��
	m_listAll.AddItem(m_vecFund.front().Equity, row, 32, 2, DT_CENTER, g_ColorRefData.GetColorBlack());			//��ĩȨ�棨��棿��
	m_listAll.AddItem(m_vecFund.front().Margin, row, 33, 2, DT_CENTER, g_ColorRefData.GetColorBlack());			//��֤��
	m_listAll.AddItem(m_vecFund.front().Available, row, 34, 2, DT_CENTER, g_ColorRefData.GetColorBlack());			//�ڳ�����
	row++;
	//�ҵ�һ��ƽ�ֺ�ͬ��Ӧ�Ŀ��ֺ�ͬ��Ȼ����ݿ��ֺ�ͬ�����ж�Ӧ��ƽ�ֺ�ͬ��Ȼ��������ƽ�ֺ�ͬ��Ӧ�ĳɽ�������Ȼ��ɾ���ɽ�
	while (!m_vecCloseAgree.empty())
	{
		string  openAgreementNo;
		for (auto it_rel : m_vecAgreeRel)
		{
			if (strcmp(m_vecCloseAgree.front().AgreementNo, it_rel.SellAgreementNo) == 0)
			{
				openAgreementNo = it_rel.BuyAgreementNo;
			}
		}
		//���ݿ��ֺ�ͬ
		SmsAgreementDataRsp tempOpenAgree;
		memset(&tempOpenAgree, 0, sizeof(SmsAgreementDataRsp));
		//bool foundOpen = false;
	
		for (auto it : m_subSystem->m_map_mapAgreement[key])
		{
			if (strcmp(it.second.AgreementNo, openAgreementNo.c_str()) == 0)
			{
				addOpenAgree(it.second, row);
				//foundOpen = true;
				break;
			}
		}
		//if (!foundOpen)
		
		//���ݿ��ֺ�ͬ��������ƽ�ֺ�ͬ��
		vector<string>		tempClose;
		tempClose.clear();
		for (auto it : m_vecAgreeRel)
		{
			if (strcmp(openAgreementNo.c_str(), it.BuyAgreementNo) == 0)
			{
				for (auto it_close : m_vecCloseAgree)
				{
					if (strcmp(it.SellAgreementNo, it_close.AgreementNo) == 0)
					{
						tempClose.push_back(it.SellAgreementNo);
					}
				}
				
			}
		}
		int beginrow = row;
		//���ݿ��ֺ�ͬ���ҵ����еĿ��ֳɽ�
		vector<SmsMatchDataRsp>	tempOpenMatch;
		tempOpenMatch.clear();
		for (auto it_order : m_vecOpenOrder)
		{
			if (strcmp(openAgreementNo.c_str(), it_order.AgreementNo) == 0)
			{
				for (auto it_match : m_vecOpenMatch)
				{
					if (strcmp(it_match.OrderNo, it_order.OrderNo) == 0)
					{
						tempOpenMatch.push_back(it_match);
					}
				}
			}
		}
		//����ƽ�ֺ�ͬ���ҵ����е�ƽ�ֳɽ�
		vector<SmsMatchDataRsp>	tempCloseMatch;
		for (auto it : tempClose)
		{
			for (auto it_close : m_vecCloseAgree)
			{
				if (strcmp(it.c_str(), it_close.AgreementNo) == 0)
				{
					for (auto it_order : m_vecCloseOrder)
					{
						if (strcmp(it_order.AgreementNo, it_close.AgreementNo) == 0)
						{
							for (auto it_match : m_vecCloseMatch)
							{
								if (strcmp(it_match.OrderNo, it_order.OrderNo) == 0)
								{
									addCloseMatch(it_match, row);
									row++;
								}
							}
						}
					}
				}
			}
		}
		int close_match_count = row - beginrow;


		break;

	}
}

LRESULT StatementWnd::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
		OnCommand(wParam, lParam);
		return PROCESSED;
	case WM_NOTIFY:
		OnNotify(wParam, lParam);
		return PROCESSED;
	//case SSWM_STATIC_BUTTON_CLICKDOWN:
	//	OnBtnClickDown(wParam, lParam);
	//	return PROCESSED;
	case WM_CREATE:
		OnCreate();
		return PROCESSED;
	case WM_PAINT:
		OnPaint();
		return PROCESSED;
	case WM_SIZE:
		OnSize(wParam, lParam);
		return PROCESSED;
	case WM_CTLCOLORSTATIC:
		return OnCtlColor(wParam, lParam);
	case WM_MOUSEMOVE:
		OnMouseMove(wParam, lParam);
		return PROCESSED;
	case WM_MOUSELEAVE:
		OnMouseLeave();
		return PROCESSED;
	case WM_LBUTTONDOWN:
		OnLButtonDown(wParam, lParam);
		return PROCESSED;
	case WM_NCLBUTTONDOWN:
		OnNcLButtonDown(wParam, lParam);
		return NOT_PROCESSED;
	case WM_LBUTTONUP:
		OnLButtonUp(wParam, lParam);
		return PROCESSED;
	case WM_MOUSEWHEEL:
		OnMouseWheel(wParam, lParam);
		return PROCESSED;
	case WM_DRAWITEM:
		OnDrawItem(wParam, lParam);
		return PROCESSED;
	case WM_MEASUREITEM://==7
		OnMeasureItem(wParam, lParam);
		return PROCESSED;
	case WM_KEYDOWN:
		KeyDown(wParam, lParam);
		break;
	case WM_NCDESTROY:
		OnNcDestroy();
		return NOT_PROCESSED;
	case WM_DESTROY:
		return NOT_PROCESSED;
	default:
		return NOT_PROCESSED;
	}
	return PROCESSED;
}



void StatementWnd::OnCreate()
{
	InitCommonControls();
	m_hInstance = GetModuleHandle(NULL);
	//�б�
	m_listAll.CreateList(m_Hwnd, LVS_REPORT | LVS_OWNERDRAWFIXED | WS_BORDER, 25, 25, LVS_EX_FULLROWSELECT, ID_ListAll);
	m_listAll.MoveWindow(0, cCAPTION_H, cFRAME_H, cFRAME_V - cCAPTION_H);

	//�ֻ���ͬ��Ϣ
	m_listAll.AddColumn(L"�ֻ���ͬ��", 80, -1);	//0
	m_listAll.AddColumn(L"����", 80, -1);	//1
	m_listAll.AddColumn(L"���", 80, -1);//2
	m_listAll.AddColumn(L"�ֻ���ͬ����", 80, -1);//3
	m_listAll.AddColumn(L"�ֻ���ͬ�۸�", 80, -1);//4
	m_listAll.AddColumn(L"��Ӧ�ڻ���Լ", 80, -1);//5
	m_listAll.AddColumn(L"��ͬ����", 80, -1);//6
	//������ϸ
	m_listAll.AddColumn(L"��������", 80, -1);//7
	m_listAll.AddColumn(L"���ֵ���", 80, -1);//8
	m_listAll.AddColumn(L"���ֽ��", 80, -1);//9
	m_listAll.AddColumn(L"����������", 80, -1);//10
	m_listAll.AddColumn(L"��������", 80, -1);//11
	//������ϸ
	m_listAll.AddColumn(L"�����۸�", 80, -1);//12
	m_listAll.AddColumn(L"��������", 80, -1);//13
	m_listAll.AddColumn(L"��������", 80, -1);//14
	//ƽ����ϸ
	m_listAll.AddColumn(L"ƽ������", 80, -1);//15
	m_listAll.AddColumn(L"ƽ�ּ۸�", 80, -1);//16
	m_listAll.AddColumn(L"ƽ������", 80, -1);//17
	m_listAll.AddColumn(L"ƽ��ӯ��", 80, -1);//18
	m_listAll.AddColumn(L"ƽ��������", 80, -1);//19
	//�ڻ�����
	m_listAll.AddColumn(L"�ڻ�����۸�", 80, -1);//20
	m_listAll.AddColumn(L"�ڻ���������", 80, -1);//21
	m_listAll.AddColumn(L"�ڻ���������", 80, -1);//22
	m_listAll.AddColumn(L"�ڻ�����������", 80, -1);//23
	//�ֻ�����
	m_listAll.AddColumn(L"�ֻ�����۸�", 80, -1);//24
	m_listAll.AddColumn(L"�ֻ���������", 80, -1);//25
	m_listAll.AddColumn(L"�ֻ���������", 80, -1);//26
	//�ۺ�����
	m_listAll.AddColumn(L"�ۺ�����", 80, -1);//27
	//��ˮ��ϸ
	m_listAll.AddColumn(L"�ڳ��˻����", 80, -1);//28
	m_listAll.AddColumn(L"���", 80, -1);//29
	m_listAll.AddColumn(L"����", 80, -1);//30
	m_listAll.AddColumn(L"��ĩȨ��", 80, -1);//31
	m_listAll.AddColumn(L"��ĩ�˻����", 80, -1);//32
	m_listAll.AddColumn(L"��֤��", 80, -1);//33
	m_listAll.AddColumn(L"�����ʽ�", 80, -1);//34
	m_listAll.AddColumn(L"��ע", 80, -1);//35

	//�˵�
	m_menuExport = CreatePopupMenu();
	AppendMenu(m_menuExport, MF_STRING, IDM_Export, L"��������");
}

void StatementWnd::OnPaint()
{
	TMemDC memdc(m_Hwnd);
	int oldDc = SaveDC(memdc.GetHdc());
	SetBkMode(memdc.GetHdc(), TRANSPARENT);

	RECT rcClient;
	GetClientRect(m_Hwnd, &rcClient);

	HBRUSH hbrBorder = CreateSolidBrush(RGB(100, 100, 100));
	InflateRect(&rcClient, -1, -1);
	FrameRect(memdc.GetHdc(), &rcClient, hbrBorder);
	InflateRect(&rcClient, -1, -1);
	FrameRect(memdc.GetHdc(), &rcClient, hbrBorder);

	RECT rcCaption = { rcClient.left, rcClient.top, rcClient.right, cCAPTION_H };

	HBRUSH hbrCaption = CreateSolidBrush(RGB(100, 100, 100));
	FillRect(memdc.GetHdc(), &rcCaption, hbrCaption);

	rcClient.top = cCAPTION_H;
	FillRect(memdc.GetHdc(), &rcClient, g_ColorRefData.GetHbrWhite());

	rcCaption.left = 8;
	SelectObject(memdc.GetHdc(), g_FontData.GetFontWord13());
	SetTextColor(memdc.GetHdc(), RGB(238, 238, 238));
	DrawText(memdc.GetHdc(), wtitle.c_str(), -1, &rcCaption, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

	DrawClose(&memdc);
	DeleteObject(hbrBorder);
	DeleteObject(hbrCaption);
	RestoreDC(memdc.GetHdc(), oldDc);
}

void StatementWnd::OnSize(WPARAM wParam, LPARAM lParam)
{
	int cx = LOWORD(lParam);
	int cy = HIWORD(lParam);

	m_rcClose.right = cx - 8;
	m_rcClose.left = m_rcClose.right - ccloseWidth;
	m_rcClose.top = 10;
	m_rcClose.bottom = m_rcClose.top + ccloseWidth;
}

void StatementWnd::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
	if (!m_bMouseTrack)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = m_Hwnd;
		tme.dwHoverTime = 0;
		::TrackMouseEvent(&tme);
		m_bMouseTrack = true;
	}
	InvalidateRect(m_Hwnd, 0, false);


	POINT pointM;
	pointM.x = LOWORD(lParam);
	pointM.y = HIWORD(lParam);
	//�رհ�Ť
	if (PtInRect(&m_rcClose, pointM))
	{
		if (!m_bInCloseRect)
			InvalidateRect(m_Hwnd, &m_rcClose, TRUE);
		m_bInCloseRect = true;
	}
	else
	{
		if (m_bInCloseRect)
			InvalidateRect(m_Hwnd, &m_rcClose, TRUE);
		m_bInCloseRect = false;
	}
}

void StatementWnd::OnMouseLeave()
{
	m_bMouseTrack = false;
	m_bInCloseRect = false;
}

void StatementWnd::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	rect.bottom = rect.top + cCAPTION_H;
	POINT pt = { LOWORD(lParam), HIWORD(lParam) };
	if (PtInRect(&m_rcClose, pt))
	{
		//ShowWindow(m_Hwnd, SW_HIDE);
	}
	else if (PtInRect(&rect, pt))
	{
		PostMessage(m_Hwnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
	}
}

void StatementWnd::OnNcLButtonDown(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case HTTOP:
		SendMessage(m_Hwnd, WM_SYSCOMMAND, SC_SIZE | WMSZ_TOP, lParam);
		break;
	case HTBOTTOM:
		SendMessage(m_Hwnd, WM_SYSCOMMAND, SC_SIZE | WMSZ_BOTTOM, lParam);
		break;
	case HTLEFT:
		SendMessage(m_Hwnd, WM_SYSCOMMAND, SC_SIZE | WMSZ_LEFT, lParam);
		break;
	case HTRIGHT:
		SendMessage(m_Hwnd, WM_SYSCOMMAND, SC_SIZE | WMSZ_RIGHT, lParam);
		break;
	default:
		break;
	}
}

void StatementWnd::OnLButtonUp(WPARAM wParam, LPARAM lParam)
{
	POINT pt = { LOWORD(lParam), HIWORD(lParam) };
	if (PtInRect(&m_rcClose, pt))
	{
		ShowWindow(m_Hwnd, SW_HIDE);
	}
}

void StatementWnd::OnDrawItem(WPARAM wParam, LPARAM lParam)
{
	LPDRAWITEMSTRUCT p = (LPDRAWITEMSTRUCT)lParam;
	switch (p->CtlID)
	{
	case ID_ListAll:
		m_listAll.OnDrawItem(lParam);
		break;
	default:
		break;
	}
}

void StatementWnd::OnMeasureItem(WPARAM wParam, LPARAM lParam)
{
	LPMEASUREITEMSTRUCT p = (LPMEASUREITEMSTRUCT)lParam;
	m_listAll.OnMeasureItem(p->itemHeight);
}

void StatementWnd::OnMouseWheel(WPARAM wParam, LPARAM lParam)
{
}

void StatementWnd::KeyDown(WPARAM wParam, LPARAM lParam)
{
}

void StatementWnd::OnNcDestroy()
{
	//delete this;
}

LRESULT StatementWnd::OnCtlColor(WPARAM wParam, LPARAM lParam)
{
	SetBkMode((HDC)wParam, TRANSPARENT);
	return (LRESULT)g_ColorRefData.GetHbrWhite();
}

void StatementWnd::OnCommand(WPARAM wParam, LPARAM lParam)
{

}

void StatementWnd::OnNotify(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case ID_ListAll:
		DealAllList(lParam);
		break;
	default:
		break;
	}
}

void StatementWnd::DealAllList(LPARAM lParam)
{
	LPNMITEMACTIVATE p = (LPNMITEMACTIVATE)lParam;
	//char code[21] = { 0 };
	//sprintf(code,"code:%d\n",p->hdr.code);
	//OutputDebugStringA(code);

	if (p->hdr.code == NM_RCLICK)
	{
		EnableMenuItem(m_menuExport, IDM_Export, MF_ENABLED);
		ShowMenu(m_menuExport);
	}
}

void StatementWnd::ShowMenu(HMENU hmenu)
{
	POINT pt;
	GetCursorPos(&pt);
	TrackPopupMenu(hmenu, TPM_LEFTALIGN | TPM_TOPALIGN, pt.x, pt.y, 0, m_Hwnd, NULL);
}

void StatementWnd::DrawClose(TMemDC * pMemDC)
{
	int iold = SaveDC(pMemDC->GetHdc());
	HPEN hPen = NULL;
	if (m_bInCloseRect)
		hPen = CreatePen(PS_SOLID, 1, RGB(225, 225, 225));
	else
		hPen = CreatePen(PS_SOLID, 1, RGB(142, 146, 153));
	HPEN hOldPen = (HPEN)SelectObject(pMemDC->GetHdc(), hPen);

	MoveToEx(pMemDC->GetHdc(), m_rcClose.left, m_rcClose.top, 0);
	LineTo(pMemDC->GetHdc(), m_rcClose.right + 1, m_rcClose.bottom + 1);

	MoveToEx(pMemDC->GetHdc(), m_rcClose.left, m_rcClose.bottom, 0);
	LineTo(pMemDC->GetHdc(), m_rcClose.right + 1, m_rcClose.top - 1);

	SelectObject(pMemDC->GetHdc(), hOldPen);
	DeleteObject(hPen);
	RestoreDC(pMemDC->GetHdc(), iold);
}

std::vector<std::string> StatementWnd::SplitString(const std::string & str, char cSplit)
{
	vector<string> vecstr;
	string tempstr(str);
	string::iterator tempit = tempstr.begin();
	while (tempit != tempstr.end())
	{
		tempit = find<string::iterator, char>(tempstr.begin(), tempstr.end(), cSplit);
		if (tempit != tempstr.end())
		{
			iterator_traits<string::iterator>::value_type dis = distance(tempstr.begin(), tempit);
			vecstr.push_back(tempstr.substr(0, dis));
			if (++tempit != tempstr.end())
			{
				tempstr.erase(tempstr.begin(), tempit);
				tempit = tempstr.begin();
			}
			else
				break;
		}
		else//û�ҵ�' '��������  ����abc;������ȻҪ����
		{
			iterator_traits<string::iterator>::value_type dis = distance(tempit, tempstr.end());
			vecstr.push_back(tempstr);
			break;
		}
	}
	return vecstr;
}

int StatementWnd::DateStrToInt(const std::string & str)
{
	vector<string> tempvec = SplitString(str, '-');
	if (tempvec.size() < 3)
		return 0;
	int year =  atoi(tempvec.at(0).c_str());
	int month = atoi(tempvec.at(1).c_str());
	int day = atoi(tempvec.at(2).c_str());
	return year * 10000 + month * 100 + day;
}

void StatementWnd::addOpenAgree(SmsAgreementDataRsp & openagree, int row)
{
	m_listAll.AddItem(openagree.AgreementNo, row, 0, DT_CENTER, g_ColorRefData.GetColorBlack());
	m_listAll.AddItem(openagree.SignDate, row, 1, DT_CENTER, g_ColorRefData.GetColorBlack());
	m_listAll.AddItem(openagree.SpecificationNo, row, 2, DT_CENTER, g_ColorRefData.GetColorBlack());
	m_listAll.AddItem(openagree.SpotQty, row, 3, 0, DT_CENTER, g_ColorRefData.GetColorBlack());
	m_listAll.AddItem(openagree.SpotPrice, row, 4, 2, DT_CENTER, g_ColorRefData.GetColorBlack());
	if(openagree.AgreementType == SMS_AGREEMENT_PURCHASE)
		m_listAll.AddItem("�ɹ���ͬ", row, 6, DT_CENTER, g_ColorRefData.GetColorBlack());
	else
		m_listAll.AddItem("���ۺ�ͬ", row, 6, DT_CENTER, g_ColorRefData.GetColorBlack());
}

void StatementWnd::addCloseMatch(SmsMatchDataRsp & closematch, int row)
{
	int count = m_listAll.GetItemCount();
	if(row >= count)
		m_listAll.AddItem("", row, 0, DT_CENTER, g_ColorRefData.GetColorBlack());
	m_listAll.AddItem(closematch.ContractNo, row, 5, DT_CENTER, g_ColorRefData.GetColorBlack());
	m_listAll.AddItem(closematch.SettleDate, row, 15, DT_CENTER, g_ColorRefData.GetColorBlack());
	m_listAll.AddItem(closematch.MatchPrice, row, 16, 2, DT_CENTER, g_ColorRefData.GetColorBlack());
	m_listAll.AddItem(closematch.MatchQty*5, row, 17, 0, DT_CENTER, g_ColorRefData.GetColorBlack());
	m_listAll.AddItem(closematch.MatchFee, row, 19, 2, DT_CENTER, g_ColorRefData.GetColorBlack());

}
