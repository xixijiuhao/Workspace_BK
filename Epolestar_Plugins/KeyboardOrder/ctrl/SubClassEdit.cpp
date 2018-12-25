#include "..\PreInclude.h"

extern CPlugLoad g_PlugLoad;
extern G_COLOREF g_ColorRefData;

CSubClassEdit::CSubClassEdit(unsigned int &pos, int editId, SOFCommodityInf&inf, COFInputDlg &ref) :m_nCursorPosRef(pos)
, m_EditID(editId)
, m_CommodityInf(inf)
, m_hLastFocusWnd(NULL)
, m_RefDlg(ref)
{
	m_bTracking = m_bHover = m_bFocus = FALSE;
	m_ClientBrush = ::CreateSolidBrush(g_ColorRefData.GetColorWhite()/*GetSysColor(COLOR_BTNFACE)*/);
	m_hOrderBrush = ::CreateSolidBrush(/*GetSysColor(COLOR_WINDOWFRAME)*/g_ColorRefData.GetColorRGB150());
	m_HotOrderBrush1 = ::CreateSolidBrush(RGB(96, 200, 253));
	m_HotOrderBrush2 = ::CreateSolidBrush(g_ColorRefData.GetColorLightBlue());
}
CSubClassEdit::~CSubClassEdit()
{
	if (m_ClientBrush)
		DeleteObject(m_ClientBrush);
	if (m_hOrderBrush)
		DeleteObject(m_hOrderBrush);
	if (m_HotOrderBrush1)
		DeleteObject(m_HotOrderBrush1);
	if (m_HotOrderBrush2)
		DeleteObject(m_HotOrderBrush2);
}
void CSubClassEdit::InitEdit(HWND hwnd)
{
	Init(hwnd);
}
LRESULT CALLBACK CSubClassEdit::WndProc(UINT message, WPARAM w, LPARAM l)
{
	//不希望再做处理返回1
	static bool bNeedControl = false;
	static bool bNeedPoint = false;
	static int  iPointForward = 0;
	static int  iPointBackward = 0;
	static HWND LastFocusWnd = NULL;

	static bool	bClick = false;		//单击
	static bool	bSetFocus = false;		//单击
	static bool	bDbClick = false;	//双击
	static bool	bTimer = false;		//定时器触发
	static bool	bdown = false;		//单击

	switch (message)
	{
		case WM_SETFOCUS:
		{
			m_bFocus = TRUE;
			DrawHelper::DrawFrame(GetHwnd(), m_bHover, m_bFocus, m_HotOrderBrush1, m_HotOrderBrush2, m_ClientBrush, m_hOrderBrush);

			//last
			LastFocusWnd = (HWND)w;

			//Reset
			bNeedControl = false;
			bNeedPoint = false;
			iPointForward = 0;
			iPointBackward = 0;
			LastFocusWnd = NULL;

			bClick = false;
			bDbClick = false;
			bTimer = false;

			//获取行情合约id
			SQuoteSnapShot tmpQuotSnap;
			memset(&tmpQuotSnap, 0, sizeof(SQuoteSnapShot));

			SContractNoType contract_no;
			GetRawContractID(contract_no);

			bNeedControl = IsNeedControl(contract_no, tmpQuotSnap);
			if (bNeedControl)
				GetPriceShowInf(tmpQuotSnap, bNeedPoint, iPointForward, iPointBackward);
			//此处不处理得到焦点选中后几位 在父窗口的焦点函数中处理
			m_hLastFocusWnd = LastFocusWnd;
		
			if (!bdown)
			{
				SetTimer(GetHwnd(), 1, 25, NULL);
				bTimer = true;
				bSetFocus = true;
			}
			
		}
		break;
		case WM_LBUTTONDOWN:
		{
			bdown = true;
			SetFocus(GetHwnd());//这句很重要
			SetTimer(GetHwnd(), 1, ::GetDoubleClickTime() / 4, NULL);
			bTimer = true;
			//CEdit::OnLButtonDown(nFlags, point);
		}
		break;
		case WM_LBUTTONUP:
		{
			bdown = false;
			if (bTimer)
				bClick = true;
			else
			{
				bTimer = false;
				KillTimer(GetHwnd(), 1);	//关闭时钟
				bClick = false;
			}
			//CEdit::OnLButtonUp(nFlags, point);
		}
		break;
		case WM_LBUTTONDBLCLK:
		{
			bClick = false;
			if (bTimer)
			{
				KillTimer(GetHwnd(), 1);
				bTimer = false;
			}
			Edit_SetSel(GetHwnd(), 0, -1);
		}
		break;
		case WM_TIMER:
		{
			if (bClick || bSetFocus)
			{
				bool bSet = bClick;
				bClick = false;
				bSetFocus = false;
				string strText;
				strText = GetWindowTextAnsic(GetHwnd());
				KillTimer(GetHwnd(), 1);
				int nStart = 0, nEnd = 0;
				::SendMessage(GetHwnd(), EM_GETSEL, (WPARAM)&nStart, (LPARAM)&nEnd);//获取选定文本的开始与结束位置，并存在nStart,nEnd
				if (bSet&&(((nEnd - nStart) > 0) || ((nEnd - nStart) > (int)m_nCursorPosRef)))
					return false;
				if (m_nCursorPosRef < 0 || (m_nCursorPosRef == 0 && bSet))
					return false;

				if ((UINT)strText.length() < m_nCursorPosRef)
					Edit_SetSel(GetHwnd(), 0, -1);
				else
					Edit_SetSel(GetHwnd(), (int)(strText.length() - m_nCursorPosRef), (int)strText.length());
			}
			//CEdit::OnTimer(nIDEvent);
		}
		break;
		case WM_CHAR:
		{
			if (::IsWindow(m_RefDlg.GetHwnd()))
			{
				//快速选中与否
				if (m_RefDlg.IsKS())
					return 1;
						
			}
			return true;
		}
		break;
		case WM_KEYDOWN:
		{	
			if (::IsWindow(m_RefDlg.GetHwnd()) && m_RefDlg.DoLeftRightUpDownKey(w))	
				return 1;
			if (VK_RETURN == w&&::IsWindow(m_RefDlg.GetHwnd()) && m_RefDlg.DealEnterSendOrder(w))
				return 1;
			if (VK_TAB == w || VK_RETURN == w)
			{
				bool bShiftDown = (0x8000 & GetKeyState(VK_SHIFT))!=0;
				if (::IsWindow(m_RefDlg.GetHwnd()))
				{
					if (bShiftDown)
						m_RefDlg.SetIndexFocus(m_RefDlg.GetLastFocusIndex(GetDlgCtrlID(GetHwnd())), true, false);
					else
						m_RefDlg.SetIndexFocus(m_RefDlg.GetNextFocusIndex(GetDlgCtrlID(GetHwnd())), true);
					return 1;
				}
			}
			else if (VK_UP == w || VK_DOWN == w)
			{
				if (m_RefDlg.DoWndKeyDown(::GetFocus(), w))
					return 0;
			}
			//处理删除键  有选中时这两个键功能一致 无选中时delete删后面 back删前面的
			if (VK_BACK == w || VK_DELETE == w)
			{
				string str;
				str = GetWindowTextAnsic(GetHwnd());
				//如果以前带小数点 合约切换后 选中小数点可以更改数值
				int nStart = -1, nEnd = -1;
				int nCurCursor = -1;
				::SendMessage(GetHwnd(), EM_GETSEL, (WPARAM)&nStart, (LPARAM)&nEnd);
				if (nStart != -1 && nStart < nEnd)
				{
					str = str.substr(0, nStart) + str.substr(nEnd, string::npos);
					SetWindowTextW(GetHwnd(), LoadRC::ansi_to_unicode(str).c_str());
					nCurCursor = nStart;
				}
				else if (nStart == nEnd&&nStart != -1)
				{
					if (VK_BACK == w)
					{
						if (nStart - 1 >= 0)
						{
							str.erase(str.begin() + nStart - 1);
							SetWindowTextW(GetHwnd(), LoadRC::ansi_to_unicode(str).c_str());
							nCurCursor = nStart - 1;
						}
					}
					else if (VK_DELETE == w)
					{
						if (nEnd < (int)str.length())
						{
							str.erase(str.begin() + nEnd);
							SetWindowTextW(GetHwnd(), LoadRC::ansi_to_unicode(str).c_str());
							nCurCursor = nStart;
						}
					}
				}
				Edit_SetSel(GetHwnd(), nCurCursor, nCurCursor);
				return true;
			}
			if (VK_LEFT == w || VK_RIGHT == w)
			{
				string str;
				str = GetWindowTextAnsic(GetHwnd());
				//如果以前带小数点 合约切换后 选中小数点可以更改数值
				int nStart = -1, nEnd = -1;
				int nCurCursor = -1;
				::SendMessage(GetHwnd(), EM_GETSEL, (WPARAM)&nStart, (LPARAM)&nEnd);
				if (nStart != -1 && nStart < nEnd)
					nCurCursor = nStart;
				else if (nStart == nEnd&&nStart != -1)
				{
					if (VK_LEFT == w)
					{
						if (nStart - 1 >= 0)
							nCurCursor = nStart - 1;
					}
					else if (VK_RIGHT == w)
					{
						if (nStart+1 <= (int)str.length())
							nCurCursor = nStart+1;
					}
				}
				Edit_SetSel(GetHwnd(), nCurCursor, nCurCursor);
				return true;
			}
			UINT nChar = MapVirtualKey(w, MAPVK_VK_TO_CHAR);
			if (!isprint(nChar))
				return false;
			//处理可见字符
			if (!IsValidPriceKeyDown(nChar))
				return true;

			string str;
			str = GetWindowTextAnsic(m_hWnd);//保存前次字符串

			if (!IsValidPriceInput(GetHwnd(), nChar, bNeedControl, bNeedPoint, iPointForward, iPointBackward))
				return true;

			string str2;
			str2 = GetWindowTextAnsic(m_hWnd);//检测本次字符串
			if (str2.size() > KPRICELENGTH_LIMIT)
				SetWindowTextA(m_hWnd, str.c_str());
		}
		break;
		case WM_NCPAINT:
		{
			CallWindowProc((WNDPROC)m_OldProc, m_hWnd, message, w, l); //默认回调过程
			DrawHelper::DrawFrame(GetHwnd(), m_bHover, m_bFocus, m_HotOrderBrush1, m_HotOrderBrush2, m_ClientBrush, m_hOrderBrush);
			return 1;
		}
		break;
		case WM_MOUSEMOVE :
		{
			if (!m_bTracking)
			{
				TRACKMOUSEEVENT tme;
				tme.cbSize = sizeof(tme);
				tme.hwndTrack = m_hWnd;
				tme.dwFlags = TME_LEAVE | TME_HOVER;
				tme.dwHoverTime = 50;
				m_bTracking =_TrackMouseEvent(&tme)==TRUE;
			}
		}
		break;
		case WM_MOUSELEAVE:
		{
			m_bTracking = FALSE;
			m_bHover = FALSE;
			DrawHelper::DrawFrame(GetHwnd(), m_bHover, m_bFocus, m_HotOrderBrush1, m_HotOrderBrush2, m_ClientBrush, m_hOrderBrush);
		}
		break;
		case WM_MOUSEHOVER:
		{
			m_bHover = TRUE;
			DrawHelper::DrawFrame(GetHwnd(), m_bHover, m_bFocus, m_HotOrderBrush1, m_HotOrderBrush2, m_ClientBrush, m_hOrderBrush);
		}
		break;
		case WM_KILLFOCUS:
		{
			m_bFocus = FALSE;
			DrawHelper::DrawFrame(GetHwnd(), m_bHover, m_bFocus, m_HotOrderBrush1, m_HotOrderBrush2, m_ClientBrush, m_hOrderBrush);
		}
		break;
		default:
			break;
	}
	return CallWindowProc((WNDPROC)m_OldProc, m_hWnd, message, w, l); //默认回调过程
}
//获取行情ID
void CSubClassEdit::GetRawContractID(SContractNoType & id)
{
	string strCode;
	strCode = GetWindowTextAnsic(GetDlgItem(GetParent(GetHwnd()), m_EditID));
	m_RefDlg.GetContractStr(strCode);
	id[0] = '\0';
	CConvert::StrToQContractNoType(strCode.c_str(), id);
}
//是否需要控制
bool CSubClassEdit::IsNeedControl(SContractNoType & contract_no, SQuoteSnapShot & tmpQuotSnap)
{
	bool bNeedControl = false;
	//表示获取成功并且没有订阅
	if (g_PlugLoad.GetStarApi() && g_PlugLoad.GetStarApi()->GetSnapshot(contract_no, &tmpQuotSnap))
	{
		if (tmpQuotSnap.Data[S_FID_LIMITUPPRICE].FidAttr != S_FIDATTR_NONE && tmpQuotSnap.Data[S_FID_LIMITDOWNPRICE].FidAttr != S_FIDATTR_NONE)
		{
			if (0 != tmpQuotSnap.Data[S_FID_LIMITUPPRICE].Price)
				bNeedControl = true;
		}	
	}
	return bNeedControl;
}
//获取价格小数点前后位数
void CSubClassEdit::GetPriceShowInf(const SQuoteSnapShot&tmpQuotSnap, bool &bNeedPoint, int &iPointForward, int &iPointBackward)
{
	if (!m_CommodityInf.bIfFractionPrice)
	{
		int iForward = -1, iBackward = -1;
		GetIntByPrice(iForward, iBackward, m_CommodityInf.iPrecision, tmpQuotSnap.Data[S_FID_LIMITUPPRICE].FidAttr != S_FIDATTR_NONE ? tmpQuotSnap.Data[S_FID_LIMITUPPRICE].Price:0,
			tmpQuotSnap.Data[S_FID_LIMITDOWNPRICE].FidAttr != S_FIDATTR_NONE ?tmpQuotSnap.Data[S_FID_LIMITDOWNPRICE].Price:0);
		iPointForward = iForward;
		if (m_CommodityInf.iPrecision > 0)
		{
			bNeedPoint = true;
			iPointBackward = iBackward;
		}
		else
		{
			bNeedPoint = false;
			iPointBackward = 0;
		}
	}
}
//获取价格小数点前后位数
bool CSubClassEdit::GetIntByPrice(int &iForward, int &iBackward, int iPrecision, double LimitUpPrice, double LimitDownPrice)
{
	char cc[16] = { 0 };
	sprintf_s(cc,"%0.*lf", iPrecision, LimitUpPrice);
	string strPrice(cc);
	int index = strPrice.find('.');
	if (-1 == index)
	{
		iForward = strPrice.length();
		iBackward = 0;
		return true;
	}
	iForward = index;
	iBackward = strPrice.length() - index - 1;
	return true;
}
//是否是有效的字符输入
bool CSubClassEdit::IsValidPriceKeyDown(char key)
{
	return ((key >= VK_0&& key <= VK_9) || key == '.' || key == VK_OEM_PERIOD || key == VK_DECIMAL ||
		key == '-' || key == VK_OEM_MINUS || key == VK_SUBTRACT);
}
//是否是有效的键盘输入
bool CSubClassEdit::IsValidPriceInput(HWND hwnd, char cCurInput, bool bNeedControl, bool bNeedPoint, int iPointForward, int iPointBackward)
{
	//使用正则表达式判断这个串是否正在录入数值型
	BOOL bIsValidNum = false;
	string str;
	str = GetWindowTextAnsic(hwnd);
	int ind = str.length();
	//如果以前带小数点 合约切换后 选中小数点可以更改数值
	int nStart = -1, nEnd = -1;
	int nCurCursor = 0;
	::SendMessage(hwnd, EM_GETSEL, (WPARAM)&nStart, (LPARAM)&nEnd);
	if (nStart != -1 && nStart < nEnd)
	{
		str = str.substr(0, nStart) + cCurInput + str.substr(nEnd, string::npos);
		nCurCursor = nStart + 1;
	}
	else if (nStart == nEnd)
	{
		str.insert(str.begin() + nEnd,cCurInput);
		nCurCursor = nStart + 1;
	}

	if (m_CommodityInf.bIfFractionPrice)//说明是分数
		bIsValidNum = true;
	else
	{
		if (m_CommodityInf.bCanNegative)//说明是价差
			bIsValidNum = CGeneralGlobal::IsValidNumber(str.c_str(), false, true);
		else//说明是小数
		{
			if (m_CommodityInf.iPrecision == 0)//整数
				bIsValidNum = CGeneralGlobal::IsValidNumber(str.c_str(), true, false);
			else//可以有小数点
				bIsValidNum = CGeneralGlobal::IsValidNumber(str.c_str(), false, false);
		}
	}
	//判断值超出范围没
	if (bIsValidNum&&bNeedControl)
	{
		if (bNeedPoint)
		{
			int index = str.find('.');
			if (-1 == index)
			{
				//自动补上'.'
				if ((int)str.length() > iPointForward)
				{
					str.insert(str.begin() + iPointForward, '.');
					bIsValidNum = false;
					SetWindowTextW(hwnd, LoadRC::ansi_to_unicode(str).c_str());
					Edit_SetSel(hwnd, str.length(), str.length());
				}
			}
			else
			{
				if ((int)str.substr(0, index).length() > iPointForward || (int)str.substr(index + 1, string::npos).length() > iPointBackward)
					bIsValidNum = false;
			}
		}
		else
		{
			int index = str.find('.');
			//本来不能输入小数点 却输入了
			if (-1 != index)
			{

			}
			else
			{
				if ((int)str.length() > iPointForward)
					bIsValidNum = false;
			}
		}
	}
	if (bIsValidNum)
	{
		SetWindowTextW(hwnd, LoadRC::ansi_to_unicode(str).c_str());
		Edit_SetSel(hwnd, nCurCursor, nCurCursor);
	}
	return bIsValidNum ? 1 : 0;
}