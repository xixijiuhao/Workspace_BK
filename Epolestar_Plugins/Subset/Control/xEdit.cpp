// sEdit.cpp : 实现文件
//
#include "stdafx.h"
#include "xEdit.h"

#define PasswordSafe	255

// CxEdit
IMPLEMENT_DYNAMIC(CxEdit, CEdit)

CxEdit::CxEdit()
	: m_bNoPasteCut(false)
	, m_bNoPopupMenu(false)
	, m_bNoUndo(false)
	, m_bNoDelAndBack(false)
	, m_bTimerEnabled(false)
	, m_bMouseClicked(false) 
	, m_colText(GetSysColor(COLOR_WINDOWTEXT))
	, m_colBack(GetSysColor(COLOR_WINDOW))
{
	m_brBackGnd.CreateSolidBrush(m_colBack);
}

CxEdit::~CxEdit()
{
	if (m_bTimerEnabled)
		KillTimer(1);	

	if (m_brBackGnd.GetSafeHandle())
		m_brBackGnd.DeleteObject();
}

BEGIN_MESSAGE_MAP(CxEdit, CEdit)
	ON_WM_TIMER()
	ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()

LRESULT CxEdit::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_PASTE: 
		case WM_CUT:		if (m_bNoPasteCut)	message=0; break;
		case WM_UNDO:		if (m_bNoUndo)		message=0; break;
		case WM_CONTEXTMENU:if (m_bNoPopupMenu) message=0; break;
		case WM_KEYDOWN:	
			if (m_bNoDelAndBack && (wParam==VK_DELETE))	
				message=0; 
			else if ((KeyDownProc.Data!=nullptr)&&(KeyDownProc.Code!=nullptr)&&(KeyDownProc.Data->IsChild(this)))//响应KeyDown的函数指针回调
				(KeyDownProc.Data->*KeyDownProc.Code)(this, message, wParam, lParam); 
			break;
		case WM_CHAR:		
			if (m_bNoDelAndBack && (wParam==VK_BACK))	
				message=0; 
			break;	
	    //区分单击与双击
		case WM_LBUTTONDOWN: 
			SetIsClick(true);								//单击判断激活
			break;
		case WM_LBUTTONUP:		
			{
				CRect Rect;
				GetClientRect(Rect);
				CPoint point;

				point.x=lParam & 0xFFFF;//<<16>>16;
				point.y=lParam>>16;

				//鼠标按键弹起时的位置不在本控件之内，则取消单击事件
				if (Rect.PtInRect(point) && m_bTimerEnabled)
				{
					m_bMouseClicked=true;
					MouseClk.wParam=wParam;
					MouseClk.lParam=lParam;
				}
				else
					SetIsClick(false);
			}
			break;
		case WM_LBUTTONDBLCLK:		
			SetIsClick(false);								//触发双击事件时关闭时钟，使单击事件失效
			break;		

		case WM_GETTEXT:
			if (IsPassword())		//密码框保护
			{
				if (HIWORD(wParam)==PasswordSafe)
					wParam = LOWORD(wParam);
				else 
				{
					lParam=0;
					wParam=0;
				}
			}
			break;
	}

	return CEdit::WindowProc(message, wParam, lParam);
}

int CxEdit::GetWindowText(LPTSTR lpszStringBuf, int nMaxCount) const
{
	WPARAM wParam = nMaxCount;
	if (IsPassword())	//密码框取值参数
		wParam = MAKELPARAM(nMaxCount, PasswordSafe);
	SendMessage(WM_GETTEXT, wParam, (LPARAM)lpszStringBuf);
	return strlen(lpszStringBuf);
}
void CxEdit::GetWindowText(CString& rString) const
{
	int Count = GetWindowTextLength();
	char pTxt[MAXWORD];
	pTxt[0]=0;
	this->GetWindowText(pTxt, Count+1);
	rString = pTxt;
}

void CxEdit::OnTimer(UINT_PTR nIDEvent)
{	
	//在一个鼠标双击最大时间间隔之后再触发单击事件
	if (nIDEvent==1 && m_bMouseClicked)
		DoLBtnClick(MouseClk.wParam, MouseClk.lParam);

	CEdit::OnTimer(nIDEvent);
}

void CxEdit::DoLBtnClick(UINT nFlags, CPoint point)
{
	SetIsClick(false);
	SendMessage(WM_LBUTTONCLK, nFlags, point.y<<16 || point.x);	//发送单击消息
}

void CxEdit::SetIsClick(bool Value)
{
	m_bTimerEnabled=Value;

	if (m_bTimerEnabled)
		SetTimer(1, ::GetDoubleClickTime(), nullptr);				//打开时钟，并设置时钟回调的间隔为鼠标双击最大时间间隔
	else
	{
		KillTimer(1);											//关闭时钟
		m_bMouseClicked=false;
	}
}

void CxEdit::PreSubclassWindow()
{
	// TODO: 在此添加专用代码和/或调用基类
	KeyDownProc.Data=nullptr;										//响应KeyDown的函数指针
	SetIsClick(false);

	CEdit::PreSubclassWindow();
}
//设置响应KeyDown的函数指针
void CxEdit::SetKeyDownProc(CWnd * Owner, MsgProc Value)
{
	if (Owner->IsChild(this))
	{
		KeyDownProc.Data=Owner;
		KeyDownProc.Code=Value;
	}
}

bool CxEdit::IsNoPasteCut()
{
	return m_bNoPasteCut;
}

void CxEdit::NoPasteCut(bool Value)
{
	m_bNoPasteCut=Value;
}

bool CxEdit::IsNoPopupMenu()
{
	return m_bNoPopupMenu;
}

void CxEdit::NoPopupMenu(bool Value)
{
	m_bNoPopupMenu=Value;
}

bool CxEdit::IsNoUndo()
{
	return m_bNoUndo;
}

void CxEdit::NoUndo(bool Value)
{
	m_bNoUndo=Value;
}

bool CxEdit::IsNoDelAndBack()
{
	return m_bNoDelAndBack;
}

void CxEdit::NoDelAndBack(bool Value)
{
	m_bNoDelAndBack=Value;
}

void CxEdit::OnDestroy()
{
	CEdit::OnDestroy();
}

HBRUSH CxEdit::CtlColor(CDC* pDC, UINT nCtlColor)
{
	pDC->SetTextColor(m_colText);
	pDC->SetBkColor(m_colBack); 

	return m_brBackGnd;
}

void CxEdit::SetBackColor(COLORREF rgb)
{
	m_colBack = rgb;

	if (m_brBackGnd.GetSafeHandle())
		m_brBackGnd.DeleteObject();
	//set brush to new color
	m_brBackGnd.CreateSolidBrush(m_colBack);
	//redraw
	Invalidate(TRUE);
}

void CxEdit::SetTextColor(COLORREF rgb)
{
	m_colText = rgb;
	//redraw
	Invalidate(TRUE);
}

bool CxEdit::IsPassword() const
{
	LONG lStyle = ::GetWindowLong(m_hWnd,   GWL_STYLE); 
	return (lStyle & ES_PASSWORD) || GetPasswordChar()!=0;
}