#include "PreInclude.h"

//
MiniEdit::MiniEdit()
{
	memset(m_wsztext,0,sizeof(m_wsztext)/sizeof(wchar_t));
	m_ncaretpos = wcslen(m_wsztext);
// 	m_font = CreateFont(18/*18*/, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
// 		CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, L"宋体"/*TEXT("Microsoft Yahei")*/);

	m_blbtndown = false;
	m_nselbegin = 0;
	m_nselend = 0;
	m_btrack = false;
	m_nscrollbegin = 0;
	m_clrbk = RGB(231, 231, 231);
	m_clrframe = RGB(96, 96, 96);
	m_bfocus = false;
	m_ntype = Type_Common;
}

MiniEdit::~MiniEdit()
{
//	DeleteObject(m_font);
	RemoveWindowSubclass(m_hwnd, m_subproc, m_nid);
}
//Property
bool MiniEdit::Create(HWND hwnd)
{
	m_hwnd = hwnd;
	return true;
}

void MiniEdit::UseOwnerProc(SUBCLASSPROC wndproc)
{
	m_subproc = wndproc;
	m_oldProc = (WNDPROC)GetWindowLong(m_hwnd, GWL_WNDPROC);
	SetWindowSubclass(m_hwnd, wndproc/*TGridEditCell::EditProc*/, m_nid, (DWORD_PTR)this);
}

void MiniEdit::SetRect(int nleft, int ntop, int nwidth, int nheight)
{
	m_rect.left = nleft; m_rect.right = nleft + nwidth;
	m_rect.top = ntop; m_rect.bottom = ntop + nheight;
	m_nwidth = nwidth;
	m_nheight = nheight;
}

void MiniEdit::SetText(const wchar_t* ptext)
{
	wcsncpy_s(m_wsztext, sizeof(m_wsztext) / 2, ptext, _TRUNCATE);
	m_ncaretpos = wcslen(m_wsztext);
}

void MiniEdit::SetText(const char* ptext)
{
	MByteToWChar(ptext, m_wsztext, sizeof(m_wsztext) / 2);
	m_ncaretpos = wcslen(m_wsztext);
}

void MiniEdit::GetText(wchar_t* ptext, int nlength)
{
	wcsncpy_s(ptext, nlength, m_wsztext, _TRUNCATE);
}

//MSG
 void MiniEdit::OnDraw(TMemDC* pmemdc)
 {
	 RECT rect, rctext;
	 rect = m_rect;
/*	 rctext = rect;*/

	 //可视区域
	 HBRUSH hframe, hbk;
	 hframe = CreateSolidBrush(m_clrframe);
	 hbk = CreateSolidBrush(m_clrbk);
	 FillRect(pmemdc->GetHdc(), &rect, hbk);
	 FrameRect(pmemdc->GetHdc(), &rect, hframe);

	 //创建全文字dc
	 HDC scrolldc = CreateCompatibleDC(pmemdc->GetDVdc());
	 SetBkMode(scrolldc, TRANSPARENT);
	 SelectObject(scrolldc, m_font);

	 SIZE size;
	 ::GetTextExtentPoint(scrolldc, m_wsztext, wcslen(m_wsztext), &size);

	 memset(&rctext, 0, sizeof(RECT));
	 rctext.right = max(size.cx, _Width(rect)); rctext.bottom = _Height(rect);
	 HBITMAP hbmp = CreateCompatibleBitmap(pmemdc->GetDVdc(), _Width(rctext), _Height(rctext));
	 SelectObject(scrolldc, hbmp);
	 FillRect(scrolldc, &rctext, hbk);

	 //选中状态
	 if (m_nselend != m_nselbegin)
	 {
		 RECT rcsel = m_rect;
		 POINT ptbegin, ptend;
		 HBRUSH hbrsel = CreateSolidBrush(RGB(142, 153, 182));
		 rcsel.top = m_ncarettop; rcsel.bottom -= m_ncarettop;
		 CaretPos2Position(m_nselbegin, ptbegin);
		 CaretPos2Position(m_nselend, ptend);
		 rcsel.left = ptbegin.x; rcsel.right = ptend.x;
		 OffsetRect(&rcsel, -1, 0);
		 FillRect(scrolldc, &rcsel, hbrsel);
		 DeleteObject(hbrsel);
	 }

	 //绘制数据
 	 rect.left += EditText_Begin;
 	 rect.right -= EditText_End;

	 m_nscrollbegin = max(size.cx - _Width(rect), 0);
	 DrawText(scrolldc, m_wsztext, wcslen(m_wsztext), &rctext, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	 StretchBlt(pmemdc->GetHdc(), rect.left, rect.top + EditBorder, _Width(rect), _Height(rect) - EditBorder * 2,
		 scrolldc, m_nscrollbegin + rctext.left, EditBorder + rctext.top, _Width(rctext), _Height(rctext) - EditBorder * 2, SRCCOPY);
	 DeleteDC(scrolldc);
	 DeleteObject(hbk);
	 DeleteObject(hbmp);
	 DeleteObject(hframe);
}

void MiniEdit::OnChar(WPARAM wparam, LPARAM lparam)
{
	DeleteSelectText();
	wchar_t wchar = (wchar_t)wparam;
	if (m_ntype == Type_Int && wchar == '.')
		return;

	int nlength = (int)wcslen(m_wsztext);
	if ((wchar < Begin_Code || nlength >= EditText_MaxLength)&& wchar != VK_BACK/* && wchar != VK_RETURN*/)
		return;
	if (wchar != VK_BACK && wchar != VK_RETURN)
	{
		::MoveMemory(m_wsztext + m_ncaretpos + 1, m_wsztext + m_ncaretpos, (nlength - m_ncaretpos + 1) * sizeof(wchar_t));
		m_wsztext[m_ncaretpos++] = wchar;
	}

	UpdateData();
	_SetCaretPos(m_ncaretpos);
	PostMessage(m_hwnd, SSWM_MINIEDIT_CHANGE, wparam, lparam);
}

void MiniEdit::OnLButtonDown(WPARAM wparam, LPARAM lparam)
{
	POINT pt;
	pt.x = (int)(short)LOWORD(lparam);
	pt.y = (int)(short)HIWORD(lparam);
	m_ncaretpos = GetClickCaretPos(pt);
	POINT pt2;
	GetCaretPos(&pt2);
	wchar_t wszlog[100];
	swprintf_s(wszlog, L"OnLButtonDown()--%d\r\n", m_ncaretpos);
	OutputDebugString(wszlog);

	m_nselbegin = m_nselend = m_ncaretpos;
	m_blbtndown = true;
	_SetCaretPos(m_ncaretpos);
	InvalidateRect(m_hwnd, &m_rect, false);
}

void MiniEdit::OnMouseMove(WPARAM wparam, LPARAM lparam)
{
	if (m_blbtndown)
	{
		POINT pt;
		pt.x = (int)(short)LOWORD(lparam);
		pt.y = (int)(short)HIWORD(lparam);

		m_ncaretpos = GetClickCaretPos(pt);
		
		wchar_t wszlog[100];
		swprintf_s(wszlog, L"OnMouseMove()--%d\r\n", m_ncaretpos);
		OutputDebugString(wszlog);

		m_nselend = m_ncaretpos;
		_SetCaretPos(m_ncaretpos);
		InvalidateRect(m_hwnd, &m_rect, false);
	}
}

void MiniEdit::OnLButtonUp(WPARAM wparam, LPARAM lparam)
{
	if (m_blbtndown)
	{
		POINT pt;
		pt.x = (int)(short)LOWORD(lparam);
		pt.y = (int)(short)HIWORD(lparam);

		m_ncaretpos = GetClickCaretPos(pt);

		wchar_t wszlog[100];
		swprintf_s(wszlog, L"OnLButtonUp()--%d\r\n", m_ncaretpos);
		OutputDebugString(wszlog);

		m_nselend = m_ncaretpos;
		m_blbtndown = false;
		_SetCaretPos(m_ncaretpos);
		InvalidateRect(m_hwnd, &m_rect, false);
	}
}

void MiniEdit::OnMouseLeave()
{
	m_btrack = false;
	m_blbtndown = false;
}

void MiniEdit::OnKeyDown(WPARAM wparam, LPARAM lparam)
{
	wchar_t code = (wchar_t)wparam;
	switch (code)
	{
	case VK_LEFT: // 左光标键
		if (m_ncaretpos > 0)
			m_ncaretpos--;
		break;
	case VK_RIGHT:     // 右光标键
		if (m_ncaretpos < (int)::_tcslen(m_wsztext))
			m_ncaretpos++;
		break;
	case VK_BACK: // 退格键
		if (!DeleteSelectText() && m_ncaretpos > 0)
		{
			::MoveMemory(m_wsztext + m_ncaretpos - 1, m_wsztext + m_ncaretpos, (::_tcslen(m_wsztext) - m_ncaretpos + 1) * sizeof(wchar_t));
			m_ncaretpos--;
			UpdateData();
		}
		break;
	case VK_DELETE:
	{  // 删除键
		int len = ::_tcslen(m_wsztext);
		if (!DeleteSelectText()&&m_ncaretpos < len)
		{
			::MoveMemory(m_wsztext + m_ncaretpos, m_wsztext + m_ncaretpos + 1, (::_tcslen(m_wsztext) - m_ncaretpos + 1) * sizeof(wchar_t));
			UpdateData();
		}
	}
	break;
	}//end switch

	if (code < VK_SPACE&&m_nselend != m_nselbegin)
	{
		m_nselbegin = m_nselend = min(m_nselbegin, m_nselend);
		InvalidateRect(m_hwnd, &m_rect, false);
	}
	_SetCaretPos(m_ncaretpos);
}

void MiniEdit::OnSetFocus()
{
	m_bfocus = true;
	TEXTMETRIC metric;
	HDC hdc = ::GetDC(m_hwnd);
	SelectObject(hdc, m_font);
	GetTextMetrics(hdc, &metric);
	
	m_ncarettop = max((m_nheight - metric.tmHeight) / 2,0);
	::CreateCaret(m_hwnd, (HBITMAP)NULL, 1, metric.tmHeight);
	_SetCaretPos(m_ncaretpos);
	::ShowCaret(m_hwnd);
	::ReleaseDC(m_hwnd, hdc);
}

void MiniEdit::OnKillFocus()
{
	m_bfocus = false;
	::HideCaret(m_hwnd);
	::DestroyCaret();
}
//设置
void MiniEdit::_SetCaretPos(int ncaretpos)
{
	SIZE size;
	HDC hDC = ::GetDC(m_hwnd);
	SelectObject(hDC, m_font);
	::GetTextExtentPoint(hDC, m_wsztext, ncaretpos, &size);

	int nleft = min(Caret_XGap + size.cx + EditText_Begin - m_nscrollbegin, m_rect.right - EditText_End) + m_rect.left;
	int ntop = m_ncarettop + m_rect.top;
	::SetCaretPos(nleft, ntop);
	::ReleaseDC(m_hwnd, hDC);
}

void MiniEdit::UpdateData()
{
	InvalidateRect(m_hwnd, &m_rect, false);
	UpdateWindow(m_hwnd);
}

int MiniEdit::GetClickCaretPos(POINT pt)
{
	pt.x = max(pt.x, 0);
	HDC hdc = ::GetDC(m_hwnd);
	SIZE size; int ncaretpos = 0, nprecx = 0;
	int nlength = ::_tcslen(m_wsztext);
	SelectObject(hdc, m_font);

	pt.x += m_nscrollbegin;
	for (ncaretpos = 0; ncaretpos <= nlength; ncaretpos++)
	{
		::GetTextExtentPoint(hdc, m_wsztext, ncaretpos, &size);
		if (pt.x >= nprecx&&pt.x < size.cx)
		{
			ncaretpos--;
			break;
		}
		nprecx = size.cx;
	}

	if (ncaretpos >= nlength)
		int a = 0;
	::ReleaseDC(m_hwnd, hdc);
	int nret = 0;
	nret = min(ncaretpos, nlength);
	return nret;
}

void MiniEdit::CaretPos2Position(int ncaretpos,POINT& pt)
{
	HDC hdc = ::GetDC(m_hwnd);
	SIZE size; SelectObject(hdc, m_font);
	::GetTextExtentPoint(hdc, m_wsztext, ncaretpos, &size);
	pt.x = size.cx + EditText_Begin/* - m_nscrollbegin*/;

	::ReleaseDC(m_hwnd, hdc);
}

bool MiniEdit::DeleteSelectText()
{
	if (m_nselend == m_nselbegin)
		return false;
	if (m_nselend != m_nselbegin)
	{
		int nmax = max(m_nselbegin, m_nselend);
		int nmin = min(m_nselbegin, m_nselend);

		m_nselbegin = m_nselend = nmin;
		::MoveMemory(m_wsztext + nmin, m_wsztext + nmax, (::_tcslen(m_wsztext)/* - m_nselend+m_nselend*/ - nmin/*m_ncaretpos + 1*/) * sizeof(wchar_t));
		UpdateData();
		m_ncaretpos = nmin;
	}
	return true;
}

