// sComboBox.cpp : 实现文件
//
#include "stdafx.h"
#include "xComboBox.h"
#include "string_ex.h"
#include "_base.h"
#include <string>

using namespace std;

/*************************************** CxComboBox ********************************************/
bool CxComboBox::IsReadOnly()
{
	return m_bReadOnly;
}

void CxComboBox::ReadOnly(bool Value)
{
	if (m_bReadOnly==Value)
		return;

	m_bReadOnly=Value;
	//if (m_EditHandle > 0)
	//	::SendMessage(m_EditHandle, EM_SETREADONLY, (int)m_bReadOnly, 0);//这样设置之后控件背景变成了灰色
}

bool CxComboBox::IsNoPasteCut()
{
	return m_bNoPasteCut;
}

void CxComboBox::NoPasteCut(bool Value)
{
	m_bNoPasteCut=Value;
}

bool CxComboBox::IsNoPopupMenu()
{
	return m_bNoPopupMenu;
}

void CxComboBox::NoPopupMenu(bool Value)
{
	m_bNoPopupMenu=Value;
}

bool CxComboBox::IsNoUndo()
{
	return m_bNoUndo;
}

void CxComboBox::NoUndo(bool Value)
{
	m_bNoUndo=Value;
}

bool CxComboBox::IsNoDelAndBack()
{
	return m_bNoDelAndBack;
}

void CxComboBox::NoDelAndBack(bool Value)
{
	m_bNoDelAndBack=Value;
}

bool CxComboBox::IsTextShowValue()
{
	return m_bTextShowValue;
}
void CxComboBox::TextShowValue(bool Value)
{
	if (m_bTextShowValue == Value)
		return;

	m_bTextShowValue = Value;


	int ind = GetCurSel();
	if (ind == CB_ERR)
		return;

	CString ItemText;
	GetLBText(ind, ItemText);
	if (m_bTextShowValue)
		CComboBox::SetWindowText(ItemText);
	else
	{
		int pos = ItemText.Find(m_SplitChar);
		if (pos != CB_ERR)
			CComboBox::SetWindowText(ItemText.Left(pos));
	}
}
int CxComboBox::GetMinVisible()
{
	return SendMessage(CB_GETMINVISIBLE, 0, 0);
}

BOOL CxComboBox::SetMinVisible(int Value)
{
	return SendMessage(CB_SETMINVISIBLE, WPARAM(Value), 0);
}
int CxComboBox::AddString(LPCTSTR lpszString)
{
	return CComboBox::AddString(lpszString);
}

int CxComboBox::InsertString(int nIndex, LPCTSTR lpszString)
{
	return CComboBox::InsertString(nIndex, lpszString);
}
int CxComboBox::AddString(CString Name, CString Value)
{
	CString ItemText;
	ItemText.Format("%s%c%s", Name, m_SplitChar, Value);
	return CComboBox::AddString(ItemText);
}
int CxComboBox::InsertString(int Index, CString Name, CString Value)
{
	CString ItemText;
	ItemText.Format("%s%c%s", Name, m_SplitChar, Value);
	return CComboBox::InsertString(Index, ItemText);
}
int CxComboBox::AddString(char * Name, char * Value)
{
	CString ItemText;
	ItemText.Format("%s%c%s", Name, m_SplitChar, Value);
	return CComboBox::AddString(ItemText);
}
int CxComboBox::InsertString(int Index, char * Name, char * Value)
{
	CString ItemText;
	ItemText.Format("%s%c%s", Name, m_SplitChar, Value);
	return CComboBox::InsertString(Index, ItemText);
}

int CxComboBox::GetWindowText(LPTSTR lpszStringBuf, int nMaxCount) const
{
	if (m_bTextShowValue)
	{
		CString sText = GetCurrName();
		strncpy_s(lpszStringBuf, nMaxCount, sText.GetString());
		return sText.GetLength();
	}
	else
		return CComboBox::GetWindowText(lpszStringBuf, nMaxCount);
}
void CxComboBox::GetWindowText(CString& rString) const
{
	if (m_bTextShowValue)
		rString = GetCurrName();
	else
		CComboBox::GetWindowText(rString);
}

//Value对齐, AuotListWidth:List自适应文本宽度
void CxComboBox::AlighValue(bool AuotListWidth)
{
	if (!m_bAutoComplete)
		return;

	string max_name, max_value;
	//获得Name最大长度,和所有字符串和关联指针
	CArray<string> Names;
	CArray<string> Values;
	CArray<void *>	Datas;
	string Name, Value;
	void * Data;
	uint pos;
	CString ItemText;
	for (int i=0; i<GetCount(); i++)
	{
		Name.clear(); Value.clear(); 

		GetLBText(i, ItemText);
		pos = ItemText.Find(m_SplitChar);
		if (CB_ERR == pos)
		{
			pos = ItemText.GetLength();
			Name = ItemText.GetString();
		}
		else
		{
			Name = ItemText.Left(pos).GetString();
			Value  = ItemText.Right(ItemText.GetLength() - pos).TrimLeft().GetString();
			if (max_value.length() < Value.length())
				max_value = Value;
		}
		Data = GetItemDataPtr(i);
		
		if (max_name.length() < Name.length())
			max_name = Name;

		Names.Add(Name);
		Values.Add(Value);
		Datas.Add(Data);
	}
	
	//对齐Value，获得最大文本长度
	ResetContent();
	int ind;
	string Text;
	for (int i=0; i<Names.GetCount(); i++)
	{
		Text = Names[i];
		if (!Values[i].empty())
		{	
			Text.append(max_name.length() - Text.length() + 1, m_SplitChar);
			Text.append(Values[i]);
		}

		ind = AddString(Text.c_str());
		SetItemDataPtr(ind, Datas[i]);		
	}
	
	if (!AuotListWidth)
		return;

	//设置下拉框宽度
	CClientDC dc(this);
	CFont * pFont = GetFont();
	CFont * pOldFont = dc.SelectObject(pFont);

	Text = max_name + max_value + "WW";
	int ItemMaxWidth = max(dc.GetTextExtent(Text.c_str()).cx, 0);

	dc.SelectObject(pOldFont);

	ItemMaxWidth += ::GetSystemMetrics(SM_CYBORDER)*2;
	if (GetMinVisible() < GetCount())
		ItemMaxWidth += ::GetSystemMetrics(SM_CXVSCROLL);
	SetDroppedWidth(ItemMaxWidth+2);
}

//回调Edit子控件的消息处理方法WNDPROC
void CxComboBox::PreSubclassWindow()
{
	//重定向子控件Edit的WndProc到CxComboBox的成员函数EditWndProc
	m_EditHandle=::FindWindowEx(m_hWnd, 0, "Edit", 0);
	if (m_EditHandle != 0)
	{	
		//保存Edit的原始处理函数
		m_OldEditWndProc = (WNDPROC) ::GetWindowLong(m_EditHandle, GWL_WNDPROC);

		//转成员函数EditWndProc为回调函数
		m_dEditWndProc.Assign(this, &CxComboBox::EditWndProc);

		//设置Edit的回调函数为EditWndProc
		::SetWindowLong(m_EditHandle, GWL_WNDPROC, (LONG)(WNDPROC)m_dEditWndProc);
	}
	
	CComboBox::PreSubclassWindow();
}
//Combobox的Edit子控件的消息处理方法WNDPROC
LRESULT CxComboBox::EditWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	bool bChange = false;
	switch (message)
	{
		case WM_PASTE: 
		case WM_CUT:		
			if (m_bNoDelAndBack || m_bReadOnly)	
				message=0; 
			break;
		case WM_UNDO:		
			if (m_bNoUndo)			
				message=0; 
			break;
		case WM_CONTEXTMENU:
			if (m_bNoPopupMenu)		
				message=0; 
			break;
		case WM_KEYDOWN:	
			if (m_bNoDelAndBack && (wParam==VK_DELETE))	
				message=0; 
			break;	
		case WM_CHAR:		
			if (m_bReadOnly || m_bNoDelAndBack && (wParam==VK_BACK))	
				message=0; 
			break;
		case WM_LBUTTONDOWN:			//只读时鼠标按下即显示下拉框
			if (m_bReadOnly)
			{
				SendMessage(WM_SETFOCUS);
				ShowDropDown();
				return 0;
			}
			break;
		case WM_SETTEXT:
			if (m_bAutoComplete && !m_bTextShowValue)
			{
				TCHAR * p = strchr((TCHAR*)lParam, m_SplitChar);
				if (p!=nullptr)
				{
					*p=0;
					bChange = true;
				}
			}
			break;
		case EM_SETSEL:
			if (m_bReadOnly || GetParent()==nullptr || GetParent()->GetFocus()==nullptr || GetParent()->GetFocus()->m_hWnd!=hWnd)
				wParam = lParam;
			break;
	}


	//调用原始的处理函数
	LRESULT Result = CallWindowProc(m_OldEditWndProc, hWnd, message, wParam, lParam);


	switch (message)
	{
	case WM_GETTEXT:
		if (m_bTextShowValue)//获取有效文字 Name
		{
			TCHAR * p = strchr((TCHAR*)lParam, m_SplitChar);
			if (p!=nullptr)
				*p = 0;
			Result = strlen((TCHAR*)lParam);
		}
		break;
	case WM_SETTEXT:
		if (bChange)
		{
			SetEditSel(strlen((TCHAR*)lParam)+1, strlen((TCHAR*)lParam)+1);
			GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(this->GetDlgCtrlID(), CBN_EDITCHANGE), (LONG)this->m_hWnd);
		}
		break;
	}

	return Result;
}

IMPLEMENT_DYNAMIC(CxComboBox, CComboBox)

CxComboBox::CxComboBox()
	: m_bTimerEnabled(false)
	, m_bMouseClicked(false)
	, m_bReadOnly(false)
	, m_bNoPasteCut(false)
	, m_bNoPopupMenu(false)
	, m_bNoUndo(false)
	, m_bNoDelAndBack(false)
	, m_bAutoComplete(true)
	, m_bCanAutoComplete(true)
	, m_SplitChar(_T(' '))
	, m_bTextShowValue(false)
{ 
}

BEGIN_MESSAGE_MAP(CxComboBox, CComboBox)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_CONTROL_REFLECT(CBN_EDITUPDATE, OnEditupdate)
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()

BOOL CxComboBox::PreTranslateMessage(MSG* pMsg)
{
	//鼠标单双击判断
	switch (pMsg->message)
	{
		case WM_LBUTTONDOWN: 
			SetIsClick(true);					//单击判断激活
			break;
		case WM_LBUTTONUP:		
			{
				CRect Rect;
				GetClientRect(Rect);
				CPoint point;

				point.x=pMsg->lParam & 0xFFFF;//<<16>>16;
				point.y=pMsg->lParam>>16;

				//鼠标按键弹起时的位置不在本控件之内，则取消单击事件
				if (Rect.PtInRect(point) && m_bTimerEnabled)
				{
					m_bMouseClicked=true;
					MouseClk.wParam=pMsg->wParam;
					MouseClk.lParam=pMsg->lParam;
				}
				else
					SetIsClick(false);
			}
			break;
		case WM_MOUSELEAVE: 
		case WM_LBUTTONDBLCLK:		
			SetIsClick(false);			          //触发双击事件时关闭时钟，使单击事件失效
			break;
		case WM_KEYDOWN:
			m_bCanAutoComplete = pMsg->wParam != VK_DELETE && pMsg->wParam != VK_BACK;
			break;
	}

	return CComboBox::PreTranslateMessage(pMsg);
}

void CxComboBox::OnTimer(UINT_PTR nIDEvent)
{	
	//在一个鼠标双击最大时间间隔之后才触发单击事件
	if (m_bMouseClicked)
		DoLBtnClick(MouseClk.wParam, MouseClk.lParam);

	CComboBox::OnTimer(nIDEvent);
}

void CxComboBox::DoLBtnClick(UINT nFlags, CPoint point)
{
	SetIsClick(false);
	SendMessage(WM_LBUTTONCLK, nFlags, point.y<<16 || point.x);//发送单击消息
}

void CxComboBox::SetIsClick(bool Value)
{
	m_bTimerEnabled=Value;

	if (m_bTimerEnabled)
		SetTimer(1, ::GetDoubleClickTime(), nullptr);	//打开时钟，并设置时钟回调的间隔为鼠标双击最大时间间隔
	else
	{
		KillTimer(1);								//关闭时钟
		m_bMouseClicked=false;
	}
}

LRESULT CxComboBox::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT Result = CComboBox::WindowProc(message, wParam, lParam);

	//设置SETCURSEL之后自动触发CBN_SELCHANGE消息
	switch (message)
	{
		case CB_SETCURSEL:
		case CB_SELECTSTRING:
		{
			CWnd *pWnd = this->GetParent();
			while  (pWnd && (! pWnd->IsKindOf(RUNTIME_CLASS(CDialog))))
				pWnd = pWnd->GetParent();
			if (pWnd)
				Result = pWnd->SendMessage(WM_COMMAND, MAKEWPARAM(this->GetDlgCtrlID(), CBN_SELCHANGE), (LONG)this->m_hWnd);
		}
		break;
	}
				
	return Result;
}

void CxComboBox::OnDestroy()
{
	SetIsClick(false);

	CComboBox::OnDestroy();
}

void CxComboBox::AutoComplete(bool Value)
{
	m_bAutoComplete=m_bCanAutoComplete=Value;
}

void CxComboBox::SetSplitChar(char Value)
{
	m_SplitChar=Value;
}

BOOL CxComboBox::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style &= ~CBS_SORT;
	return CComboBox::PreCreateWindow(cs);
}

CString CxComboBox::GetName(int Index) const
{
	if (Index<0)
		return "";
	CString ItemText;
	GetLBText(Index, ItemText);
	if (!m_bAutoComplete)
		return ItemText;

	int pos = ItemText.Find(m_SplitChar);
	if (pos != CB_ERR)
		return ItemText.Left(pos);
	else
		return ItemText;
}
CString CxComboBox::GetValue(int Index) const
{
	CString ItemText;
	GetLBText(Index, ItemText);
	if (ItemText.IsEmpty() || !m_bAutoComplete)
		return "";

	int pos = ItemText.Find(m_SplitChar);
	if (pos == CB_ERR)
		return "";
	else
		return ItemText.Right(ItemText.GetLength() - pos).TrimLeft();
}

int CxComboBox::GetCurrIndex() const
{
	CString str;
	CComboBox::GetWindowText(str);
	return GetIndexByName(str.GetString());
}

int CxComboBox::GetIndexByName(const char * Name) const
{
	int Index = FindString(-1, Name);
	if(Index == CB_ERR)
		return CB_ERR;

	CString ItemText=GetName(Index);
	if (ItemText.Compare(Name)==0)
		return Index;
	else
		return CB_ERR;
}

CString CxComboBox::GetCurrValue() const
{
	int ind = GetCurSel();
	if (ind>CB_ERR)
		return this->GetValue(ind);

	CString Text;
	CComboBox::GetWindowText(Text);
	int pos = Text.Find(m_SplitChar);
	if (pos == CB_ERR)
	{
		int ind=FindString(-1, Text);
		if(ind != CB_ERR)
		{
			GetLBText(ind, Text);
			pos = Text.Find(m_SplitChar);
		}
	}

	if (pos != CB_ERR)
		return Text.Right(Text.GetLength() - pos).TrimLeft();	
	else
		return "";
}

CString CxComboBox::GetCurrName() const
{
	int ind = GetCurSel();
	if (ind>CB_ERR)
		return this->GetName(ind);
	
	CString Text;
	CComboBox::GetWindowText(Text);
	int pos = Text.Find(m_SplitChar);
	if (pos != CB_ERR)
		return Text.Left(pos);
	else
		return Text;
}

//设置自动完成的选中行
void CxComboBox::OnEditupdate() 
{
	CString str;
	CComboBox::GetWindowText(str);
	if (str.IsEmpty())							//空值时选中-1
		SetCurSel(-1);


	if (!(m_bAutoComplete && m_bCanAutoComplete)) 
		return;

	int nLength = str.GetLength();

	DWORD dwCurSel = GetEditSel();				//选择位置
	WORD dStart = LOWORD(dwCurSel);
	WORD dEnd   = HIWORD(dwCurSel);
	
	int ind=FindString(-1, str);
	if(ind != CB_ERR)
	{
		GetLBText(ind, str);

		int pos = str.Find(m_SplitChar);
		if (pos != CB_ERR)
			SetWindowText(str.Left(pos));		//有分隔符时只显示分隔符前面的子串
		else 
			SetWindowText(str);					//无分割符时显示整条信息

		if (dwCurSel != CB_ERR)
			SetEditSel(dStart, dEnd);
	}

	if (dEnd < nLength && dwCurSel != CB_ERR)
		SetEditSel(dStart, dEnd);
	else
		SetEditSel(nLength, -1);
}

//只读时将光标设为ARROW
BOOL CxComboBox::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (nHitTest == HTCLIENT && m_bReadOnly)
	{
		SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
		return true;
	}
	else
		return CComboBox::OnSetCursor(pWnd, nHitTest, message);
}

////////////////////////////////////////////////////////////////////////////////
//void AFXAPI DDX_TextX(CDataExchange* pDX, int nIDC, CString& value)
//{
//	HWND hWndCtrl = pDX->PrepareEditCtrl(nIDC);
//	if (pDX->m_bSaveAndValidate)
//	{
//		CWnd * pWnd = CWnd::FromHandlePermanent(hWndCtrl);
//		if (pWnd!=nullptr && pWnd->IsKindOf(RUNTIME_CLASS(CxComboBox)))
//			((CxComboBox*)pWnd)->GetWindowText(value);
//		else
//		{
//			int nLen = ::GetWindowTextLength(hWndCtrl);
//			::GetWindowText(hWndCtrl, value.GetBufferSetLength(nLen), nLen+1);
//			value.ReleaseBuffer();
//		}
//	}
//	else
//	{
//		AfxSetWindowText(hWndCtrl, value);
//	}
//}