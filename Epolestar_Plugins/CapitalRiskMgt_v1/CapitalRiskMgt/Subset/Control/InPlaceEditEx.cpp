/*******************************************************************************
	Author						: Aravindan Premkumar
	Unregistered Copyright 2003	: Aravindan Premkumar
	All Rights Reserved
	
	This piece of code does not have any registered copyright and is free to be 
	used as necessary. The user is free to modify as per the requirements. As a
	fellow developer, all that I expect and request for is to be given the 
	credit for intially developing this reusable code by not removing my name as 
	the author.
*******************************************************************************/

#include "stdafx.h"
#include "InPlaceEditEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define CTRL_C	0x3
#define CTRL_V	0x16
#define CTRL_X	0x18

/////////////////////////////////////////////////////////////////////////////
// CInPlaceEditEx

CInPlaceEditEx* CInPlaceEditEx::m_pInPlaceEditEx = nullptr;  

CInPlaceEditEx::CInPlaceEditEx()
{
	m_iRowIndex= -1;
	m_iColumnIndex = -1;
	m_bESC = FALSE;
	m_strValidChars.Empty();
}

CInPlaceEditEx::~CInPlaceEditEx()
{
}

BEGIN_MESSAGE_MAP(CInPlaceEditEx, CEdit)
	//{{AFX_MSG_MAP(CInPlaceEditEx)
	ON_WM_KILLFOCUS()
	ON_WM_CHAR()	
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
//	ON_MESSAGE(WM_PASTE, OnPaste)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInPlaceEditEx message handlers
/*
void CInPlaceEditEx::OnPaste(WPARAM , LPARAM )
{
	if (m_strValidChars.IsEmpty())
	{
		return;	
	}

    CString strFromClipboard;

	// get the text from clipboard
	if(OpenClipboard()) {
		HANDLE l_hData = GetClipboardData(CF_TEXT);
		if(nullptr == l_hData) {
			return;
		}
		
		char *l_pBuffer = (char*)GlobalLock(l_hData);
		if(nullptr != l_pBuffer) {
			strFromClipboard = l_pBuffer;
		}

		GlobalUnlock(l_hData);
		CloseClipboard();
	}

	// Validate the characters before pasting 
	for(int iCounter_ = 0; iCounter_ < strFromClipboard.GetLength(); iCounter_++)
	{
		if (-1 == m_strValidChars.Find(strFromClipboard.GetAt(iCounter_)))
		{
			return;
		}
	}
		
	//let the individual control handle other processing
	CEdit::Default();	
}
*/

void CInPlaceEditEx::OnKillFocus(CWnd* pNewWnd) 
{
	CEdit::OnKillFocus(pNewWnd);
	
	// TODO: Add your message handler code here

	// Get the text in the edit ctrl
	CString strEdit;
	GetWindowText(strEdit);

	// Send Notification to parent of edit ctrl
	LV_DISPINFO dispinfo;
	dispinfo.hdr.hwndFrom = GetParent()->m_hWnd;
	dispinfo.hdr.idFrom = GetDlgCtrlID();
	dispinfo.hdr.code = LVN_ENDLABELEDIT;

	dispinfo.item.mask = LVIF_TEXT;
	dispinfo.item.iItem = m_iRowIndex;
	dispinfo.item.iSubItem = m_iColumnIndex;
	dispinfo.item.pszText = m_bESC ? LPTSTR((LPCTSTR)m_strWindowText) : LPTSTR((LPCTSTR)strEdit);
	dispinfo.item.cchTextMax = m_bESC ? m_strWindowText.GetLength() : strEdit.GetLength();
	
	GetParent()->SendMessage(WM_NOTIFY, GetParent()->GetDlgCtrlID(), (LPARAM)&dispinfo);

	PostMessage(WM_CLOSE);
}

void CInPlaceEditEx::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default

 	if ((m_strValidChars.IsEmpty()) || ((-1 != m_strValidChars.Find(static_cast<TCHAR> (nChar))) || 
		(nChar == VK_BACK) || (nChar == CTRL_C) || (nChar == CTRL_V) || (nChar == CTRL_X)))
	{
		CEdit::OnChar(nChar, nRepCnt, nFlags);
	}
	else
	{
		MessageBeep(MB_ICONEXCLAMATION);
		return;
	}
}

BOOL CInPlaceEditEx::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if (WM_KEYDOWN == pMsg->message && (VK_ESCAPE == pMsg->wParam || VK_RETURN == pMsg->wParam))
	{
		if (VK_ESCAPE == pMsg->wParam)
		{
			m_bESC = TRUE;
		}

		GetParent()->SetFocus();
		return TRUE;
	}

	return CEdit::PreTranslateMessage(pMsg);
}

int CInPlaceEditEx::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CEdit::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	// Set the proper font
	CFont* pFont = GetParent()->GetFont();
	SetFont(pFont);

	ShowWindow(SW_SHOW);
	SetWindowText(m_strWindowText);
	SetSel(0, -1);
	SetFocus();
	
	  
	return 0;
}

CInPlaceEditEx* CInPlaceEditEx::GetInstance()
{
	if(m_pInPlaceEditEx == nullptr)
	{
		m_pInPlaceEditEx = new CInPlaceEditEx;
	}
	return m_pInPlaceEditEx;
}

void CInPlaceEditEx::DeleteInstance()
{
	delete m_pInPlaceEditEx;
	m_pInPlaceEditEx = nullptr;
}

BOOL CInPlaceEditEx::ShowEditCtrl(DWORD dwStyle, const RECT &rCellRect, CWnd* pParentWnd, 
								UINT uiResourceID, int iRowIndex, int iColumnIndex,
								CString& strValidChars, CString& rstrCurSelection)
{
	m_iRowIndex = iRowIndex;
	m_iColumnIndex = iColumnIndex;
	m_strValidChars = strValidChars;
	m_strWindowText = rstrCurSelection;
	m_bESC = FALSE;

	if (nullptr == m_pInPlaceEditEx->m_hWnd) 
	{
		return m_pInPlaceEditEx->Create(dwStyle, rCellRect, pParentWnd, uiResourceID); 
	}	

	return TRUE;
}
