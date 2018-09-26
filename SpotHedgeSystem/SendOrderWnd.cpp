#include "PreInclude.h"
extern G_COLOREF			g_ColorRefData;				//ɫ��
extern G_UIFONT				g_FontData;					//����
extern ITradeApi			*g_pTradeApi;
extern IStarApi            *g_pStarApi;					//���� 
extern ISpotSystemApi		*g_pSpotSystem;				//�ױ�ƽ̨

//�о�
#define CtrlInterval		5	//�о� 

LRESULT CALLBACK FillOrderSetEditProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int id = GetWindowLong(hwnd, GWL_ID);
	SendOrderWnd *pSendOrderWnd = (SendOrderWnd*)GetWindowLong(hwnd, GWL_USERDATA);
	if (!pSendOrderWnd)
		return FALSE;
	switch (message)
	{
	case WM_CHAR:
		switch (id)
		{
		case ID_Edit_Contract:
			if ((wParam >= '0'&&wParam <= '9') || wParam == '\b')
				break;
			else if ((wParam >= 'a'&&wParam <= 'z') && ID_Edit_Contract == id)
			{
				wParam = wParam - 32;
				break;
			}
			else
				return FALSE;
		case ID_Edit_Dot:
		case ID_Edit_Qty:
			if ((wParam >= '0'&&wParam <= '9') || wParam == '\b')
				break;
			else if (wParam == '-')
			{
				wchar_t sPrice[16] = { 0 };
				GetWindowText(hwnd, sPrice, sizeof(sPrice) / sizeof(wchar_t));
				if (wcsstr(sPrice, L"-") == NULL)
				{
					break;
				}
				else
				{
					DWORD pos = Edit_GetSel(hwnd);
					int start = LOWORD(pos);
					int end = HIWORD(pos);
					if (start == 0 && end == wcslen(sPrice))
						break;
					return FALSE;
				}
			}
			else
				return FALSE;
		}
		break;
	case WM_KEYDOWN://tab�� Enter�� ���¼�����
	{
		switch (wParam)
		{
		case VK_RETURN:
			if (id == ID_Edit_Contract)
			{
				SetFocus(pSendOrderWnd->GetQtyHwnd());
			}
			break;
		default:
			break;
		}
	}
		break;
	default:
		break;
	}
	map<int, WNDPROC>::iterator it = pSendOrderWnd->m_SetEdit.find(id);
	if (it == pSendOrderWnd->m_SetEdit.end())
		return FALSE;
	else
		return CallWindowProc(it->second, hwnd, message, wParam, lParam);
}

SendOrderWnd::SendOrderWnd()
	:m_tradeDots(0.0), m_finishedMatchQty(0), m_canSendLot(0), m_curTcode(""), m_curScode(""),m_curOrderID(-5)
{
	m_hQryMatch = CreateEvent(NULL, TRUE, FALSE, NULL);
}

SendOrderWnd::~SendOrderWnd()
{
	if (g_pStarApi)
	{
		g_pStarApi->UnsubAllQuote(this);
	}
}

void SendOrderWnd::Create(LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle)
{
	CreateFrm(classname, parent, style, exstyle);
}

void SendOrderWnd::Register(SystemFrame * pdata)
{
	if (!pdata)
		return;
	m_System = pdata;
}

HWND SendOrderWnd::GetQtyHwnd()
{
	return m_spineditQty.GetEditHwnd();
}

void SendOrderWnd::JumpOrderWnd(string agreement, string contractno, int qty)
{
	int count = m_cbAgree.GetCount();
	string agree("");
	wchar_t	temp[128] = { 0 };
	vector<string> v_str;
	for (int i = 0; i < count; i++)
	{
		m_cbAgree.SetSelect(i);
		memset(&temp, 0, sizeof(temp));
		m_cbAgree.GetText(temp);
		agree = LoadRC::unicode_to_ansi(temp);
		v_str = m_System->SplitString(agree,' ');
		if (v_str.size() > 0)
		{
			if (strcmp(agreement.c_str(), v_str.at(0).c_str()) == 0)
			{
				m_cbAgree.SetSelect(i);
				break;
			}
		}
	}

	m_editContract.SetText(contractno.c_str());
	SetContractNo(contractno);

	m_spineditQty.SetWindowNumber(qty);
	m_System->switch_index(0, 0);
}

void SendOrderWnd::OnDisconnect(const TSocketDisconnectReason ErrorCode, const TLogin * pLogin)
{
}

void SendOrderWnd::OnInitProgress(const int ErrorCode, const TradeInitProgress CurrProgress, const TLoginBase * pLoginBase)
{
	if (ErrorCode == 0 && CurrProgress == tipReady)
	{

	}
}

void SendOrderWnd::OnMatch(const TMatch * pMatch)
{

}

void SendOrderWnd::OnOrder(const TOrder * pOrder)
{
	if (!pOrder)
		return;
	if (m_curOrderID == -5)
		return;
	if (m_curOrderID != pOrder->OrderID)
		return;

	char order_tip[256] = { 0 };
	string orderState("״̬:��");
	switch (pOrder->OrderState)
	{
	case osSended:			orderState = "�ѷ���";	break;
	case osQueued:			orderState = "���Ŷ�";	break;
	case osPartFilled:		orderState = "���ֳɽ�";	break;
	case osFilled:			orderState = "��ȫ�ɽ�";	break;
	case osCanceled:		orderState = "�ѳ���";	break;
	case osPartCanceled:	orderState = "�ѳ��൥";	break;
	case osFail:			orderState = "ָ��ʧ��";	break;
	}
	sprintf_s(order_tip, "%s%s ί��%d��(%s)���ɽ�%d��", pOrder->CommodityNo,pOrder->ContractNo, pOrder->OrderQty, orderState.c_str(), pOrder->MatchQty);
	m_labMatchTips.SetText(order_tip);
}

void SendOrderWnd::OnQuote(const SContract * cont, const SQuoteUpdate * quote)
{
	if (m_curScode == cont->ContractNo && IsWindowVisible(GetHwnd()))
		PostMessage(GetHwnd(), WM_ONQUOTE, 0, 0);
}

void SendOrderWnd::SetContractNo(string sCode)
{
	if (!sCode.empty())//�ǿպ�Լ
	{
		//��ͬ��Լ���
		if (sCode == m_curTcode)
		{
			m_editContract.SetText(sCode.c_str());
			return;
		}
		m_curTcode = sCode;

		//����Ʒ����Ϣ
		TContractKey key;
		memset(&key, 0, sizeof(key));
		memcpy(key.Sign, m_CommodityInfo.Sign, sizeof(m_CommodityInfo.Sign));
		if (CConvert::TContractNoToTContractKey(sCode.c_str(), key) && CConvert::IsValidTradeCommodity(key))//��Ч���׺�Լ
		{
			//���½���Ʒ����Ϣ
			if (!CConvert::GetTradeCommodityInf(key, m_CommodityInfo))
				return;
		}
		else
		{
			//��������Ʒ����Ϣ
			if (!CConvert::GetQuoteCommodityInf(m_curTcode.c_str(), m_CommodityInfo))
				return;
		}

		//���鶩�� ��ʽ�淶��
		if (g_pStarApi)
		{
			SContractNoType sQuote = { 0 };
			CConvert::TContractNoToSContractNo(sCode.c_str(), sQuote);

			//ȡ������
			if (!m_curScode.empty() && m_curScode != sQuote)
			{
				SContractNoType qCode = { 0 };
				memcpy(qCode, m_curScode.c_str(), sizeof(SContractNoType));
				g_pStarApi->UnsubQuote(&qCode, 1, this);
				m_curScode = "";
			}

			//����
			SContractNoType qQuote = { 0 };
			memcpy(qQuote, sQuote, sizeof(SContractNoType));
			g_pStarApi->SubQuote(&qQuote, 1, this);
			m_curScode = sQuote;

		}
		PostMessage(m_Hwnd, WM_ONQUOTE, 0, 0);
		
	}
	else//��պ�Լ
	{
		m_curTcode = sCode;
		if (g_pStarApi)//���鶩�� ��ʽ�淶��
		{
			if (!m_curScode.empty())
			{
				SContractNoType qCode = { 0 };
				memcpy(qCode, m_curScode.c_str(), sizeof(SContractNoType));
				g_pStarApi->UnsubQuote(&qCode, 1, this);
				m_curScode = "";
			}
		}
	}

	m_editContract.SetText(sCode.c_str());

	m_labSellPrice.SetText("");
	m_labBuyPrice.SetText("");
	

}

bool SendOrderWnd::SendOrder(MsgOrderInfo * msg)
{
	TSendOrder sendOrder;
	CConvert::DefaultSendOrder(sendOrder);
	if (!msg || !m_System)
		return false;

	sendOrder.Direct = msg->Direct;
	sendOrder.Offset = msg->Offset;
	//��Լ
	char	contractNo[MAX_PATH] = { 0 };
	m_editContract.GetText(contractNo,sizeof(contractNo));
	if (!CConvert::TContractNoToTContractKey(contractNo, sendOrder) || !CConvert::IsValidTradeCommodity(sendOrder))
	{
		_TMessageBox_Domodal(m_Hwnd, "��ʾ", L"��Լ�������", TMB_OK);
		return false;
	}
	CConvert::TContractNoToTContractKey(contractNo, sendOrder);
	strcpy_s(sendOrder.Sign, m_System->m_curLogin.sSign);
	//�۸�
	char	temp[MAX_PATH] = { 0 };
	if (m_cbPrice.GetSelect() == 0)
	{
		memset(&temp, 0, sizeof(temp));
		m_editPrice.GetText(temp, sizeof(temp));
		sendOrder.OrderPrice = atof(temp);
		if (sendOrder.OrderPrice == 0.0)
		{
			_TMessageBox_Domodal(m_Hwnd, "��ʾ", L"�۸����", TMB_OK);
			return false;
		}
	}
	else if(m_cbPrice.GetSelect() == 1)
	{
		memset(&temp, 0, sizeof(temp));
		m_editDot.GetText(temp, sizeof(temp));
		int dot = atoi(temp);

		//����ת����
		char sCode[101] = { 0 };
		if (!CConvert::TContractNoToSContractNo(contractNo, sCode))
		{
			_TMessageBox_Domodal(m_Hwnd, "��ʾ", L"�����Լ�������", TMB_OK);
			return false;
		}

		SContract *pQuote = NULL;
		if (g_pStarApi && g_pStarApi->GetContractData("", sCode, &pQuote, 1, false))
		{
			double dPrice = 0.0;
			double dPriceTick = 0.0;
			if (pQuote->Commodity)
				dPriceTick = pQuote->Commodity->PriceTick;
			else
			{
				_TMessageBox_Domodal(m_Hwnd, "��ʾ", L"��ȡ��С�䶯��λ����", TMB_OK);
				return false;
			}
			if (pQuote->SnapShot)
				dPrice = CConvert::GetPriceBySnapShot(pQuote->SnapShot, S_FID_LASTPRICE);
			else{
				_TMessageBox_Domodal(m_Hwnd, "��ʾ", L"��ȡ���¼۴���", TMB_OK);
				return false;
			}
			if (sendOrder.Direct == dSell)
			{
				dPrice -= (dPriceTick * dot );
			}
			else if (sendOrder.Direct == dBuy)
			{
				dPrice += (dPriceTick * dot);
			}
			sendOrder.OrderPrice = dPrice;
		}
		else {
			_TMessageBox_Domodal(m_Hwnd, "��ʾ", L"��ȡ��Լ��Ϣ����", TMB_OK);
			return false;
		}
	}
	else if (m_cbPrice.GetSelect() == 2)
	{
		memset(&temp, 0, sizeof(temp));
		m_editDot.GetText(temp, sizeof(temp));
		int dot = atoi(temp);

		//����ת����
		char sCode[101] = { 0 };
		if (!CConvert::TContractNoToSContractNo(contractNo, sCode))
		{
			_TMessageBox_Domodal(m_Hwnd, "��ʾ", L"�����Լ�������", TMB_OK);
			return false;
		}

		SContract *pQuote = NULL;
		if (g_pStarApi && g_pStarApi->GetContractData("", sCode, &pQuote, 1, false))
		{
			double dPrice = 0.0;
			double dPriceTick = 0.0;
			if (pQuote->Commodity)
				dPriceTick = pQuote->Commodity->PriceTick;
			else
			{
				_TMessageBox_Domodal(m_Hwnd, "��ʾ", L"��ȡ��С�䶯��λ����", TMB_OK);
				return false;
			}
			if (sendOrder.Direct == dSell)
			{
				if (pQuote->SnapShot)
					dPrice = CConvert::GetPriceBySnapShot(pQuote->SnapShot, S_FID_BESTBIDPRICE);
				else {
					_TMessageBox_Domodal(m_Hwnd, "��ʾ", L"��ȡ���̼۴���", TMB_OK);
					return false;
				}
				dPrice -= (dPriceTick * dot);
			}
			else if (sendOrder.Direct == dBuy)
			{
				if (pQuote->SnapShot)
					dPrice = CConvert::GetPriceBySnapShot(pQuote->SnapShot, S_FID_BESTASKPRICE);
				else {
					_TMessageBox_Domodal(m_Hwnd, "��ʾ", L"��ȡ���̼۴���", TMB_OK);
					return false;
				}
				dPrice += (dPriceTick * dot);
			}
			sendOrder.OrderPrice = dPrice;
		}
		else {
			_TMessageBox_Domodal(m_Hwnd, "��ʾ", L"��ȡ��Լ��Ϣ����", TMB_OK);
			return false;
		}
	}

	//�˺�
	strcpy_s(sendOrder.UserNo, m_System->m_curLogin.UserNo);
	//�۸�����
	sendOrder.OrderType = otLimit;
	//����
	memset(&temp, 0, sizeof(temp));
	m_spineditQty.GetText(temp, sizeof(temp));
	sendOrder.OrderQty = atoi(temp);
	if (sendOrder.OrderQty == 0)
	{
		_TMessageBox_Domodal(m_Hwnd, "��ʾ", L"��������", TMB_OK);
		return false;
	}
	int index = m_cbAgree.GetSelect();
	if (index != 0 && index != -1 && sendOrder.Offset == oOpen)
	{
		if (sendOrder.OrderQty > m_canSendLot)
		{
			_TMessageBox_Domodal(m_Hwnd, "��ʾ", L"�����˺�ͬ����������", TMB_OK);
			return false;
		}
	}

	sendOrder.OrderWay = owETrade;
	sendOrder.ValidType = vtGFD;
	sendOrder.Hedge = hSpeculate;
	
	//���ͬ
	wchar_t	cb_txt_agree[MAX_PATH] = { 0 };
	m_cbAgree.GetText(cb_txt_agree);
	string agreeTxt = LoadRC::unicode_to_ansi(cb_txt_agree);
	vector<string> v_agree = m_System->SplitString(agreeTxt, ' ');
	if (!v_agree.empty())
	{
		strncpy_s(sendOrder.Remark, v_agree.at(0).c_str());
		m_curOrderID = g_pTradeApi->InsertOrder(sendOrder);
		m_spineditQty.SetWindowNumber(0);
		return true;
	}
	else
	{
		m_curOrderID = g_pTradeApi->InsertOrder(sendOrder);
		m_spineditQty.SetWindowNumber(0);
		return true;
	}
}

bool SendOrderWnd::TableButtonToEdit()
{
	return false;
}

LRESULT SendOrderWnd::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		OnCreate();
		break;
	case WM_PAINT:
		OnPaint();
		break;
	case WM_SIZE:
		OnSize();
		break;
	case WM_COMMAND:
		OnCommand(wParam, lParam);
		return PROCESSED;
	case SSWM_STATIC_BUTTON_CLICKDOWN:
		OnBtnClickDown(wParam, lParam);
		return PROCESSED;
	case WM_LBUTTONDOWN:
		OnLButtonDown(LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_LBUTTONUP:
		OnLButtonUp(LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_CTLCOLORSTATIC:
		return OnCtlColor(wParam, lParam);
	case SSWM_COMBOX_SELECT:
		OnDealSwitch(wParam, lParam);
		break;
	case WM_MOUSEWHEEL:	
		OnMouseWheel(wParam, lParam);
		break;
	case WM_ONQUOTE:				OnDealQuote();
		break;
	//case WM_NCDESTROY:				OnNcDestory();//����
	//	break;
		//case WM_DESTROY:				OnDestroy();
		//	break;
	default:
		return NOT_PROCESSED;
	}
	return PROCESSED;
}

void SendOrderWnd::OnCreate()
{
	InitCommonControls();
	m_hInstance = GetModuleHandle(NULL);
	//��ͬ
	m_labAgree.Create(m_Hwnd, L"�� ͬ", SS_CENTER, ID_Static_Agree);
	m_cbAgree.Create(m_Hwnd, ID_Combox_Agree);
	//��Լ
	m_labContract.Create(m_Hwnd, L"�� Լ", SS_CENTER, ID_Static_Contract);
	m_editContract.Create(m_Hwnd, 0, ES_UPPERCASE | ES_AUTOHSCROLL, ID_Edit_Contract);
	m_btnContract.Create(m_Hwnd, "...", ID_Button_Contract);
	//m_popAgree.Create(m_Hwnd);
	//m_popAgree.SetAutoHide(true);
	//vector<string> temp;
	//temp.clear();
	//temp.push_back("��ͬ");
	//temp.push_back("��Լ");
	//m_popAgree.SetData(temp);

	//����
	m_labQty.Create(m_Hwnd, L"�� ��", SS_CENTER, ID_Static_Qty);
	m_spineditQty.Create(m_Hwnd, 0, ID_Edit_Qty, ID_Spin_Qty);
	m_labLot.Create(m_Hwnd, L"��", SS_LEFT, ID_Static_Lot);
	//��һ��
	m_labSellPrice.Create(m_Hwnd, L"����:", SS_LEFT, ID_Static_SellPrice);

	//�۸�
	m_labPrice.Create(m_Hwnd, L"�� ��", SS_CENTER, ID_Static_Price);
	m_cbPrice.Create(m_Hwnd, ID_Combox_Price);
	m_labPlus.Create(m_Hwnd, L"+", SS_CENTER, ID_Static_Plus);
	m_editPrice.Create(m_Hwnd, 1, ES_UPPERCASE, ID_Edit_Price);
	m_editDot.Create(m_Hwnd, 0, ES_UPPERCASE, ID_Edit_Dot);
	m_labDot.Create(m_Hwnd, L"��", SS_LEFT, ID_Static_Dot);
	//��һ��
	m_labBuyPrice.Create(m_Hwnd, L"���:", SS_LEFT, ID_Static_BuyPrice);

	//����
	//m_labDirect.Create(m_Hwnd, L"����", SS_LEFT, ID_Static_Direct);
	//m_cbDirect.Create(m_Hwnd, ID_Combox_Direct);
	//��ƽ
	//m_labOffset.Create(m_Hwnd, L"��ƽ", SS_LEFT, ID_Static_OffSet);
	//m_cbOffset.Create(m_Hwnd, ID_Combox_Offset);
	//ȷ��
	//m_btnConfirm.Create(m_Hwnd, "�µ�", ID_Button_Confirm);
	//�µ�
	m_cOrderBtn.CreateWin(this, L"sell", m_Hwnd, WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS, 0, 0, 132);
	//��ʾ
	m_labTips.Create(m_Hwnd, L"", SS_LEFT, ID_Static_Tips);
	//�ɽ���ʾ
	m_labMatchTips.Create(m_Hwnd, L"", SS_LEFT, ID_Static_MatchTips);

	/////////////////////////////////////////////λ��/////////////////////////////////////////////////////
	const int nLeftStart = 0;
	const int nHeightStart = 12;
	const int nRowHeight = 30;			//�и�
	const int nItemGap = 10;			//�ؼ�ˮƽ���

	const int nHeight = 25;				//ͳһ�߶�
	const int nStaticWidth = 70;		//��ǩͳһ���

	const int nEditWidthMini = 40;
	const int nEditWidth = 115;			//�༭��ͳһ���
	const int nEditWidthPlus = 207;

	const int nComboxWidth = 100;		//ͳһ��combox���
	const int nComboxWidthPlus = 225;

	const int nButtonWidth = 80;		//��ťͳһ���

	POINT pt;
	pt.x = nLeftStart;
	pt.y = nHeightStart;
	//��ͬ
	m_labAgree.MoveWindow(pt.x, pt.y, nStaticWidth, nHeight);
	pt.x += nItemGap + nStaticWidth;
	m_cbAgree.MoveWindow(pt.x, pt.y, nComboxWidthPlus + 1, 8 * nHeight);
	//��ʾ
	pt.x  += nItemGap + nComboxWidthPlus;
	m_labTips.MoveWindow(pt.x, pt.y, 400, nHeight);
	//��Լ
	pt.x = nLeftStart;
	pt.y += nRowHeight;
	m_labContract.MoveWindow(pt.x, pt.y, nStaticWidth, nHeight);
	pt.x += nItemGap + nStaticWidth;
	m_editContract.MoveWindow(pt.x, pt.y, nEditWidthPlus, nHeight);
	pt.x += nEditWidthPlus + 1;
	m_btnContract.MoveWindow(pt.x, pt.y, 17, nHeight);
	//�ɽ��ر�
	pt.x += nItemGap + 17;;
	m_labMatchTips.MoveWindow(pt.x, pt.y, 400, nHeight);
	//����
	pt.x = nLeftStart;
	pt.y += nRowHeight;
	m_labQty.MoveWindow(pt.x, pt.y, nStaticWidth, nHeight);
	pt.x += nItemGap + nStaticWidth;
	m_spineditQty.MoveWindow(pt.x, pt.y, nComboxWidthPlus - 20, nHeight);
	pt.x += nComboxWidthPlus -20 +5;
	m_labLot.MoveWindow(pt.x, pt.y, 15, nHeight);
	//��һ��
	pt.x += 15 + nItemGap;
	m_labSellPrice.MoveWindow(pt.x, pt.y, 400, nHeight);

	//�۸�
	pt.x = nLeftStart;
	pt.y += nRowHeight;
	m_labPrice.MoveWindow(pt.x, pt.y, nStaticWidth, nHeight);
	pt.x += nItemGap + nStaticWidth;
	m_cbPrice.MoveWindow(pt.x, pt.y, nComboxWidth, 4 * nHeight);
	pt.x += nComboxWidth;
	m_labPlus.MoveWindow(pt.x, pt.y, nItemGap, nHeight);
	pt.x += nItemGap;
	m_editPrice.MoveWindow(pt.x, pt.y, nEditWidth, nHeight);
	m_editDot.MoveWindow(pt.x, pt.y, nEditWidth-20, nHeight);
	pt.x += nEditWidth - 20 + 5;
	m_labDot.MoveWindow(pt.x, pt.y, 15, nHeight);
	//��һ��
	pt.x += 15 + nItemGap;
	m_labBuyPrice.MoveWindow(pt.x, pt.y, 400, nHeight);
	//����
	//pt.x = nLeftStart;
	//pt.y += nRowHeight;
	//m_labDirect.MoveWindow(pt.x, pt.y, nStaticWidth, nHeight);
	//pt.x += nItemGap + nStaticWidth;
	//m_cbDirect.MoveWindow(pt.x, pt.y, nComboxWidth, 3 * nHeight);
	//��ƽ
	//pt.x = nLeftStart;
	//pt.y += nRowHeight;
	//m_labOffset.MoveWindow(pt.x, pt.y, nStaticWidth, nHeight);
	//pt.x += nItemGap + nStaticWidth;
	//m_cbOffset.MoveWindow(pt.x, pt.y, nComboxWidth, 4 * nHeight);
	//ȷ��
	//pt.x = nLeftStart + nStaticWidth + nItemGap;
	//pt.y += nRowHeight;
	//m_btnConfirm.MoveWindow(pt.x, pt.y, nButtonWidth, nHeight);
	//�µ�
	pt.x = nLeftStart;
	pt.y += nRowHeight;
	SetWindowPos(m_cOrderBtn.GetHwnd(), 0, pt.x, pt.y, 310, 136, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
	m_cOrderBtn.SetBShitPosition(false);
	m_cOrderBtn.UpdatePrice(L"", L"");
	m_cOrderBtn.UpdateCanOpen(0, 0);
	ShowWindow(m_cOrderBtn.GetHwnd(), TRUE);

	/////////////////////////////////////////////��ʼ��/////////////////////////////////////////////////////
	m_cbAgree.Clear();
	m_cbAgree.AddString(L"");

	m_cbAgree.SetSelect(0);

	m_cbPrice.Clear();
	m_cbPrice.AddString(L"�޼�");
	m_cbPrice.AddString(L"���¼�");
	m_cbPrice.AddString(L"���̼�");
	m_cbPrice.SetSelect(2);
	ShowWindow(m_editPrice.GetHwnd(), SW_HIDE);
	//ShowWindow(m_labPlus.GetHwnd(), SW_HIDE);
	//ShowWindow(m_labDot.GetHwnd(), SW_HIDE);

	m_labAgree.SetFont(g_FontData.GetFontWord13());
	m_labContract.SetFont(g_FontData.GetFontWord13());
	m_labQty.SetFont(g_FontData.GetFontWord13());
	m_labPrice.SetFont(g_FontData.GetFontWord13());
	m_labPlus.SetFont(g_FontData.GetFontWord13());
	m_labDot.SetFont(g_FontData.GetFontWord13());

	m_labLot.SetFont(g_FontData.GetFontWord13());
	m_labTips.SetFont(g_FontData.GetFontWord15());
	m_labMatchTips.SetFont(g_FontData.GetFontWord15());
	m_labSellPrice.SetFont(g_FontData.GetFontNumEx15());
	m_labBuyPrice.SetFont(g_FontData.GetFontNumEx15());

	m_cbAgree.SetFont(g_FontData.GetFontNum15());
	m_cbPrice.SetFont(g_FontData.GetFontNum15());

	m_btnContract.SetFont(g_FontData.GetFontNum13());

	m_spineditQty.SetFont(g_FontData.GetFontNumEx15());
	m_spineditQty.SetLimitText(3);
	m_spineditQty.SetRange(0, 999);
	m_spineditQty.SetWindowNumber(0);
	m_editContract.SetFont(g_FontData.GetFontNumEx15());
	m_editDot.SetFont(g_FontData.GetFontNumEx15());
	m_editDot.SetLimitText(9);
	m_editDot.SetWindowNumber(0);
	m_editPrice.SetFont(g_FontData.GetFontNumEx15());


	//m_cOrderBtn.UpdateValidBtn(1);
	//m_labDot.SetFont(g_FontData.GetFontWord15());
	//m_labDirect.SetFont(g_FontData.GetFontWord15());
	//m_cbDirect.SetFont(g_FontData.GetFontWordEx13());

	//m_labOffset.SetFont(g_FontData.GetFontWord15());
	//m_cbOffset.SetFont(g_FontData.GetFontWordEx13());

	//m_btnConfirm.SetFont(g_FontData.GetFontWord15());

	//�༭��ؼ� ���໯�������� 
	m_SetEdit.clear();
	vector<int> vID;
	vID.push_back(ID_Edit_Contract);
	vID.push_back(ID_Edit_Qty);
	vID.push_back(ID_Edit_Dot);

	for (vector<int>::iterator iter = vID.begin(); iter != vID.end(); iter++)
	{
		WNDPROC w = (WNDPROC)SetWindowLong(GetDlgItem(GetHwnd(), *iter), GWL_WNDPROC, (LONG)FillOrderSetEditProc);	//Ĭ�ϼ۸�
		m_SetEdit.insert(pair<int, WNDPROC>(*iter, w));
		SetWindowLong(GetDlgItem(GetHwnd(), *iter), GWL_USERDATA, LONG(this));//�û�����
	}
}

void SendOrderWnd::OnPaint()
{
	TMemDC mdc(m_Hwnd);
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	FillRect(mdc.GetHdc(), &rect, g_ColorRefData.GetHbrBackground());
}

void SendOrderWnd::OnSize()
{
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
}

void SendOrderWnd::OnCommand(WPARAM wParam, LPARAM lParam)
{
	int id = LOWORD(wParam);
	switch (id)
	{
	case ID_Static_Contract:
		OnDealContractNo(wParam, lParam);
		break;
	case ID_Edit_Qty:
		OnDealQty(wParam, lParam);
		break;
	case ID_Edit_Dot:
		break;
	case ID_Combox_Price:
		if(HIWORD(wParam) == CBN_SELCHANGE)
			OnDealPrice();
		break;
	case ID_Combox_Agree:
		if (HIWORD(wParam) == CBN_SELCHANGE)
			OnDealAgree();
		break;
	case ID_Static_SellPrice:
		if (HIWORD(wParam) == BN_CLICKED)
			OnBtnPrice(ID_Static_SellPrice);
		break;
	case ID_Static_BuyPrice:
		if (HIWORD(wParam) == BN_CLICKED)
			OnBtnPrice(ID_Static_BuyPrice);
		break;
	case ID_Edit_Contract:
		if (HIWORD(wParam) == EN_KILLFOCUS)
			OnKillFocusCode();
		break;
	default:
		break;
	}
}

void SendOrderWnd::OnLButtonDown(const int x, const int y)
{

}

void SendOrderWnd::OnLButtonUp(const int x, const int y)
{

}

void SendOrderWnd::OnBtnClickDown(WPARAM wParam, LPARAM lParam)
{
	switch (lParam)
	{
	//case ID_Button_Confirm:
	//	OnBtnConfirm();
	//	break;
	case ID_Button_Contract:
		OnSearchCode();
		break;

	default:
		break;
	}
}

LRESULT SendOrderWnd::OnCtlColor(WPARAM wParam, LPARAM lParam)
{
	SetBkMode((HDC)wParam, TRANSPARENT);
	int i = GetWindowLong((HWND)lParam, GWL_ID);
	if (i == ID_Static_SellPrice)
		SetTextColor((HDC)wParam, g_ColorRefData.GetColorTextGreen());
	else if (i == ID_Static_BuyPrice)
		SetTextColor((HDC)wParam, g_ColorRefData.GetColorBuyRed());
	return (LRESULT)g_ColorRefData.GetHbrBackground();
}

void SendOrderWnd::OnMouseWheel(WPARAM wParam, LPARAM lParam)
{
	UINT id = GetWindowLong(GetFocus(), GWL_ID);
	HWND hwnd = GetDlgItem(m_Hwnd, id);

	switch (id)
	{
	case ID_Edit_Qty:
	{
		int nDelta = GET_WHEEL_DELTA_WPARAM(wParam);
		int nMax = 999;
		int nMin = 0;
		if (nDelta > 0)
		{
			wchar_t sQty[16] = { 0 };
			GetWindowTextW(hwnd, sQty, sizeof(sQty) / sizeof(wchar_t));
			int a = _wtoi(sQty);
			a += 1;

			a = a > nMax ? nMax : a;
			wchar_t addOnePrice[16] = { 0 };
			swprintf_s(addOnePrice, TEXT("%d"), a);
			SetWindowText(hwnd, addOnePrice);
		}
		else
		{
			wchar_t sQty[16] = { 0 };
			GetWindowTextW(hwnd, sQty, sizeof(sQty) / sizeof(wchar_t));
			int a = _wtoi(sQty);
			a -= 1;
			a = a < nMin ? nMin : a;
			wchar_t subOnePrice[16] = { 0 };
			swprintf_s(subOnePrice, TEXT("%d"), a);
			SetWindowText(hwnd, subOnePrice);
		}
	}
	break;

	case ID_Edit_Dot:
	{
		int nDelta = GET_WHEEL_DELTA_WPARAM(wParam);
		int nMax = 999;
		int nMin = -99;
		if (nDelta > 0)
		{
			wchar_t sQty[16] = { 0 };
			GetWindowTextW(hwnd, sQty, sizeof(sQty) / sizeof(wchar_t));
			int a = _wtoi(sQty);
			a += 1;

			a = a > nMax ? nMax : a;
			wchar_t addOnePrice[16] = { 0 };
			swprintf_s(addOnePrice, TEXT("%d"), a);
			SetWindowText(hwnd, addOnePrice);
		}
		else
		{
			wchar_t sQty[16] = { 0 };
			GetWindowTextW(hwnd, sQty, sizeof(sQty) / sizeof(wchar_t));
			int a = _wtoi(sQty);
			a -= 1;
			a = a < nMin ? nMin : a;
			wchar_t subOnePrice[16] = { 0 };
			swprintf_s(subOnePrice, TEXT("%d"), a);
			SetWindowText(hwnd, subOnePrice);
		}
	}
	break;
	default:
		break;
	}
}

void SendOrderWnd::OnSearchCode()
{
	//if (!IsWindow(m_popContractCode.GetHwnd()))
	//	m_popContractCode.CreatePopWin(this, L"popupCode", m_Hwnd, WS_CLIPSIBLINGS, 0);

	//POINT pt = { 0, 0 };

	//m_popContractCode.UpdateData(m_System->m_curLogin.sSign);
	//ClientToScreen(m_editContract.GetHwnd(), &pt);

	//pt.y += 25;
	//SetWindowPos(m_popContractCode.GetHwnd(), 0, pt.x, pt.y, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);

	POINT pt = { 0, 0 };
	ClientToScreen(m_editContract.GetHwnd(), &pt);
	pt.y += 25;

	if (!IsWindow(m_popCode.GetHwnd()))
		m_popCode.CreatePopWin(this, L"popupCode", m_Hwnd, WS_CLIPSIBLINGS, 0, pt.x, pt.y);

	m_popCode.UpdateData(m_System->m_curLogin.sSign);

	SetWindowPos(m_popCode.GetHwnd(), 0, pt.x, pt.y, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
}



void SendOrderWnd::OnDealContractNo(WPARAM wParam, LPARAM lParam)
{
	//m_popAgree.ShowWindow(SW_SHOW);
	RECT rt;
	GetWindowRect(m_labContract.GetHwnd(), &rt);
	//m_popAgree.MoveWindow(rt.left, rt.bottom, 60, 0);
}

void SendOrderWnd::OnDealQty(WPARAM wParam, LPARAM lParam)
{

}

void SendOrderWnd::OnDealSwitch(WPARAM wParam, LPARAM lParam)
{
	HWND hwnd = (HWND)wParam;
	int pos = (int)lParam;
	if (pos == 0)
	{
		m_labContract.SetText("��ͬ��");
		ShowWindow(m_editContract.GetHwnd(), SW_HIDE);
		ShowWindow(m_btnContract.GetHwnd(), SW_HIDE);
		ShowWindow(m_cbAgree.GetHwnd(), SW_SHOW);
	}
	else if (pos == 1)
	{
		m_labContract.SetText("��Լ��");
		ShowWindow(m_editContract.GetHwnd(), SW_SHOW);
		ShowWindow(m_btnContract.GetHwnd(), SW_SHOW);
		ShowWindow(m_cbAgree.GetHwnd(), SW_HIDE);

	}
}

void SendOrderWnd::OnDealPrice()
{
	int index = m_cbPrice.GetSelect();
	if (index == 0)
	{
		ShowWindow(m_editPrice.GetHwnd(), SW_SHOW);

		ShowWindow(m_editDot.GetHwnd(), SW_HIDE);
		ShowWindow(m_labPlus.GetHwnd(), SW_HIDE);
		ShowWindow(m_labDot.GetHwnd(), SW_HIDE);
	}
	else
	{
		ShowWindow(m_editPrice.GetHwnd(), SW_HIDE);

		ShowWindow(m_editDot.GetHwnd(), SW_SHOW);
		ShowWindow(m_labPlus.GetHwnd(), SW_SHOW);
		ShowWindow(m_labDot.GetHwnd(), SW_SHOW);
	}
}

void SendOrderWnd::OnDealAgree()
{
	if (!m_System)return;

	m_finishedMatchQty = 0;
	m_tradeDots = 0;
	m_canSendLot = 0;

	wchar_t	wtext[51] = { 0 };
	m_cbAgree.GetText(wtext);
	string atext = LoadRC::unicode_to_ansi(wtext);
	if (atext.length() == 0)//�޺�ͬ
	{
		m_cOrderBtn.UpdateValidBtn(-1);
		m_popCode.SetTargetCommodity("");
		m_popCode.UpdateData(m_System->m_curLogin.sSign);

		memset(&m_curAgree, 0, sizeof(SmsAgreementDataRsp));
		return;
	}

	vector<string> agree;
	agree.clear();
	agree = m_System->SplitString(atext, ' ');
	string agreeno;
	if (agree.size() > 1)
	{
		agreeno = agree.at(0);
	}
	else
		return;

	//�޶���ť�͹��
	string specNo;
	string key = m_System->get_curkey();
	memcpy(&m_curAgree, &(m_System->m_map_mapAgreement[key][agreeno]), sizeof(SmsAgreementDataRsp));
	specNo = m_curAgree.SpecificationNo;
	//���޸�  �ɹ�--������ƽ      ����--����ƽ
	if (m_curAgree.AgreementType == SMS_AGREEMENT_PURCHASE /*&& it.PositionOffset == SMS_O_OPEN*/)
		m_cOrderBtn.UpdateValidBtn(3);//����
	else if (m_curAgree.AgreementType == SMS_AGREEMENT_SALE/* && it.PositionOffset == SMS_O_OPEN*/)
		m_cOrderBtn.UpdateValidBtn(0);//��
	else if (m_curAgree.AgreementType == SMS_AGREEMENT_PURCHASE /*&& it.PositionOffset == SMS_O_COVER*/)
		m_cOrderBtn.UpdateValidBtn(2);//��ƽ
	else if (m_curAgree.AgreementType == SMS_AGREEMENT_SALE /*&& it.PositionOffset == SMS_O_COVER*/)
		m_cOrderBtn.UpdateValidBtn(1);//��ƽ
	m_cOrderBtn.UpdateValidBtn(-1);
	//for (auto it : m_System->m_mapAgreement[key])
	//{
	//	if (strcmp(it.AgreementNo, agreeno.c_str()) == 0)
	//	{
	//		memcpy(&m_curAgree, &(it), sizeof(SmsAgreementDataRsp));

	//		specNo = it.SpecificationNo;
	//		//���޸�  �ɹ�--������ƽ      ����--����ƽ
	//		if (it.AgreementType == SMS_AGREEMENT_PURCHASE /*&& it.PositionOffset == SMS_O_OPEN*/)
	//			m_cOrderBtn.UpdateValidBtn(3);//����
	//		else if (it.AgreementType == SMS_AGREEMENT_SALE/* && it.PositionOffset == SMS_O_OPEN*/)
	//			m_cOrderBtn.UpdateValidBtn(0);//��
	//		else if (it.AgreementType == SMS_AGREEMENT_PURCHASE /*&& it.PositionOffset == SMS_O_COVER*/)
	//			m_cOrderBtn.UpdateValidBtn(2);//��ƽ
	//		else if (it.AgreementType == SMS_AGREEMENT_SALE /*&& it.PositionOffset == SMS_O_COVER*/)
	//			m_cOrderBtn.UpdateValidBtn(1);//��ƽ
	//		break;
	//	}
	//}

	SpecMapType::iterator it_spec = m_System->m_mapSpec.find(specNo);
	if (it_spec != m_System->m_mapSpec.end())
	{
		char target[24] = { 0 };
		sprintf_s(target, sizeof(target), "%s", it_spec->second.CommodityNo);
		m_popCode.SetTargetCommodity(target);

		//ÿ�ֳ���
		SCommodity* commod[1];
		g_pStarApi->GetCommodityData("", target, commod, 1, false);
		if (commod[0] )
			m_tradeDots = commod[0]->TradeDot;
	}
	else
		m_popCode.SetTargetCommodity("");
	m_popCode.UpdateData(m_System->m_curLogin.sSign);


	//===================================================��ѯ��Լ��Ӧ�ĳɽ�Ȼ������ѳɽ�����===================================================
	SmsMatchQryReq matchReq = { 0 };
	strncpy_s(matchReq.CompanyNo, m_System->m_curLogin.CompanyNo);
	strncpy_s(matchReq.UserNo, m_System->m_curLogin.UserNo);
	strncpy_s(matchReq.AgreementNo, agreeno.c_str());

	ResetEvent(m_hQryMatch);

	m_System->m_matchWnd->m_listMatch.DeleteAllItems();
	m_System->m_mapMatch.clear();

	g_pSpotSystem->QryMatch(&matchReq);
	WaitForSingleObject(m_hQryMatch, 1000);

	//�ѳɽ������ϼ�
	if (!m_System->m_mapMatch.empty())
	{
		for (auto it : m_System->m_mapMatch)
		{
			m_finishedMatchQty += it.second.MatchQty;
		}	
	}

	//��ǰ��ͬ�����������
	if (strlen(m_curAgree.AgreementNo) == 0)
	{
		m_labTips.SetText("");
	}
	else
	{
		m_canSendLot = int((m_curAgree.SpotQty - m_tradeDots * m_finishedMatchQty) / m_tradeDots);

		char	tips[1024] = { 0 };
		sprintf(tips, "%sʣ��%d��,ÿ�ֳ���:%d", m_curAgree.AgreementNo, m_canSendLot, (int)m_tradeDots);
		m_labTips.SetText(tips);
	}

	//��Լ����Ϊ��
	m_editContract.SetText("");
}

void SendOrderWnd::OnBtnPrice(int nID)
{
	string sPrice("");
	if(nID == ID_Static_SellPrice)
		sPrice = GetWindowTextAnsic(m_labSellPrice.GetHwnd());
	else if (nID == ID_Static_BuyPrice)
		sPrice = GetWindowTextAnsic(m_labBuyPrice.GetHwnd());

	int n = sPrice.find(":", 0);
	int nend = sPrice.find(" ", 0);
	if (n != -1 && nend != -1 && n < (int)sPrice.size() && nend < (int)sPrice.size() && (n + 1)<nend)
	{
		if (m_cbPrice.GetSelect() != 0)
			return;
		sPrice = sPrice.substr(n + 1, nend - (n + 1));
		SetFocus(m_editPrice.GetHwnd());
		m_editPrice.SetText(sPrice.c_str());
	}
	else
		return;

}

void SendOrderWnd::OnDealQuote()
{
	SContract *pQuote = NULL;
	if (g_pStarApi && g_pStarApi->GetContractData("", m_curScode.c_str(), &pQuote, 1, false))
	{
		double dSellPrice = 0, dBuyPrice = 0, dLastPrice = 0;
		SQtyType nSellQty = 0, nBuyQty = 0;
		dLastPrice = CConvert::GetPriceBySnapShot(pQuote->SnapShot, S_FID_LASTPRICE);
		dBuyPrice = CConvert::GetPriceByContract(pQuote, S_FID_BESTBIDPRICE);
		dSellPrice = CConvert::GetPriceByContract(pQuote, S_FID_BESTASKPRICE);
		nBuyQty = CConvert::GetQtyByContract(pQuote, S_FID_BESTBIDQTY);
		nSellQty = CConvert::GetQtyByContract(pQuote, S_FID_BESTASKQTY);

		wchar_t wText[51] = { 0 };
		if (nSellQty)
			swprintf_s(wText, L"%s%s  %lld", L"��:", LoadRC::ansi_to_unicode(CConvert::FormatPriceToStr(dSellPrice, m_CommodityInfo)).c_str(), nSellQty);
		else
			swprintf_s(wText, L"%s", L"��:");
		m_labSellPrice.SetText(wText);

		if (nBuyQty)
			swprintf_s(wText, L"%s%s  %lld", L"��:", LoadRC::ansi_to_unicode(CConvert::FormatPriceToStr(dBuyPrice, m_CommodityInfo)).c_str(), nBuyQty);
		else
			swprintf_s(wText, L"%s", L"��:");
		m_labBuyPrice.SetText(wText);
	}
	else
	{
		wchar_t wText[51] = { 0 };
		swprintf_s(wText, L"%s", L"��:");
		m_labSellPrice.SetText(wText);
		swprintf_s(wText, L"%s", L"��:");
		m_labBuyPrice.SetText(wText);
	}
}

void SendOrderWnd::OnKillFocusCode()
{
	char	temp[101] = { 0 };
	m_editContract.GetText(temp, sizeof(temp));
	string sCode = temp;
	SetContractNo(sCode);
}
