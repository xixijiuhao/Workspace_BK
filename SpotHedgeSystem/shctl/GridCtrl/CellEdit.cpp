#include"PreInclude.h"

CellEdit::CellEdit()
{
	m_nTuples = 0;
	m_lpfnFormatTextProc = DefFormatTextProc;

	m_hWnd = NULL;
	m_hBackgroundBrush = NULL;
	m_hFont = NULL;
	m_hBoldFont = NULL;
	m_hItalicFont = NULL;
	m_hUnderlineFont = NULL;

	m_bLButtonDown = FALSE;

	m_Selection.start.nTupleIndex = 0;
	m_Selection.start.nPosition = 0;
	m_Selection.end = m_Selection.start;
}

CellEdit::~CellEdit()
{
	ClearData();
	RemoveWindowSubclass(m_hWnd, m_subproc, m_nid);
}

void CellEdit::Create(HWND hwnd)
{
	m_hWnd = hwnd;
	FormatText(m_FormattedText);
	PreCreate();
	UpdateTextOffset();
}

void CellEdit::UseOwnerProc(SUBCLASSPROC wndproc)
{
	m_subproc = wndproc;
	m_oldProc = (WNDPROC)GetWindowLong(m_hWnd, GWL_WNDPROC);
	SetWindowSubclass(m_hWnd, wndproc/*TGridEditCell::EditProc*/, m_nid, (DWORD_PTR)this);
}

void CellEdit::SetRect(int nleft, int ntop, int nwidth, int nheight)
{
	m_rect.left = nleft; m_rect.right = nleft + nwidth;
	m_rect.top = ntop; m_rect.bottom = ntop + nheight;
	m_nWidth = nwidth;
	m_nHeight = nheight;
	m_ptTextOffset.x = nleft;
	m_ptTextOffset.y = ntop;
	Refresh();
}

int CellEdit::GetText(int nindex, TCHAR *ptext)
{
	// Verify that the index is valid
	if ((nindex < 0) || (nindex >(m_nTuples - 1)))
	{
		*ptext = TEXT('\0');
		return -1;
	}
	_tcscpy(ptext, BUFFER(nindex));
	return 0;
}

int CellEdit::SetText(int nindex, const TCHAR *ptext)
{
	// Verify that the index is valid
	if (((nindex < 0) || (nindex >(m_nTuples - 1))) && (nindex != -1)) return -1;

	// -1 : assign the text to all tuples
	if (nindex == -1)
	{
		for (int a = 0; a < m_nTuples; a++)
		{
			_tcscpy(BUFFER(a), ptext);
			m_TupleData[a].nCursorPos = 0;
		}
		m_nCurrentTuple = 0;
	}
	else
	{
		_tcscpy(BUFFER(nindex), ptext);
		m_TupleData[nindex].nCursorPos = _tcslen(BUFFER(nindex));
	}
	Refresh();
	return 0;
}

int CellEdit::SetText(int nindex, const char* ptext)
{
	wchar_t sztext[MAX_PATH];
	MByteToWChar(ptext, sztext, MAX_PATH);
	// Verify that the index is valid
	if (((nindex < 0) || (nindex >(m_nTuples - 1))) && (nindex != -1)) return -1;

	// -1 : assign the text to all tuples
	if (nindex == -1)
	{
		for (int a = 0; a < m_nTuples; a++)
		{
			_tcscpy(BUFFER(a), sztext);
			m_TupleData[a].nCursorPos = 0;
		}
		m_nCurrentTuple = 0;
	}
	else
	{
		_tcscpy(BUFFER(nindex), sztext);
		if(!m_bfocus)//zxw
			m_TupleData[nindex].nCursorPos = _tcslen(BUFFER(nindex));
	}
	Refresh();
	return 0;
}

int CellEdit::SetTextLimit(int nIndex, int nLimit)
{
	// Limit can't be larger than MAX_FE_BUFFER
	if (nLimit > MAX_FE_BUFFER) return -1;

	// -1 : All tuples
	if (nIndex == -1)
	{
		TCHAR *szTemp = new TCHAR[nLimit + 1];
		for (int a = 0; a < m_nTuples; a++)
		{
			// save the current buffer contents
			_tcsncpy(szTemp, BUFFER(a), nLimit);

			// reallocate the buffer with the new limit
			delete BUFFER(a);
			BUFFER(a) = new TCHAR[nLimit + 1];

			// restore the buffer contents
			_tcsncpy(BUFFER(a), szTemp, nLimit);
			BUFFER(a)[nLimit] = TEXT('\0');

			m_TupleData[a].nTextLimit = nLimit;
			m_TupleData[a].nCursorPos = 0;
		}

		delete szTemp;
	}
	else
	{
		// save the current buffer contents
		TCHAR *szTemp = new TCHAR[nLimit + 1];
		_tcsncpy(szTemp, BUFFER(nIndex), nLimit);

		// reallocate the buffer with the new limit
		delete BUFFER(nIndex);
		BUFFER(nIndex) = new TCHAR[nLimit + 1];

		// restore the buffer contents
		_tcsncpy(BUFFER(nIndex), szTemp, nIndex);
		BUFFER(nIndex)[nLimit] = TEXT('\0');

		m_TupleData[nIndex].nTextLimit = nLimit;
		m_TupleData[nIndex].nCursorPos = 0;
	}

	Refresh();
	return 0;
}

int CellEdit::SetFont(const TCHAR *szFontFace, int nFontHeight, DWORD dwCharSet)
{
	LOGFONT lf;

	// Fonts can be recreated at runtime
	// Therefore delete existing handles before creating new ones
	if (m_hFont != NULL)			 DeleteObject(m_hFont);
	if (m_hBoldFont != NULL)		 DeleteObject(m_hBoldFont);
	if (m_hItalicFont != NULL)	 DeleteObject(m_hItalicFont);
	if (m_hUnderlineFont != NULL) DeleteObject(m_hUnderlineFont);


	// Store the values in member variables
	_tcscpy(m_szFontFace, szFontFace);
	m_nFontHeight = nFontHeight;

	// Standard Font
	memset(&lf, 0, sizeof(LOGFONT));
	lf.lfHeight = nFontHeight;
	lf.lfWeight = 400;
	lf.lfItalic = FALSE;
	lf.lfUnderline = FALSE;
	lf.lfCharSet = dwCharSet;
	_tcscpy(lf.lfFaceName, szFontFace);
	m_hFont = CreateFontIndirect(&lf);

	// Bold Font
	memset(&lf, 0, sizeof(LOGFONT));
	lf.lfHeight = nFontHeight;
	lf.lfWeight = 700;
	lf.lfItalic = FALSE;
	lf.lfUnderline = FALSE;
	lf.lfCharSet = dwCharSet;
	_tcscpy(lf.lfFaceName, szFontFace);
	m_hBoldFont = CreateFontIndirect(&lf);

	// Italic Font
	memset(&lf, 0, sizeof(LOGFONT));
	lf.lfHeight = nFontHeight;
	lf.lfWeight = 400;
	lf.lfItalic = TRUE;
	lf.lfUnderline = FALSE;
	lf.lfCharSet = dwCharSet;
	_tcscpy(lf.lfFaceName, szFontFace);
	m_hItalicFont = CreateFontIndirect(&lf);

	// Underline Font
	memset(&lf, 0, sizeof(LOGFONT));
	lf.lfHeight = nFontHeight;
	lf.lfWeight = 400;
	lf.lfItalic = FALSE;
	lf.lfUnderline = TRUE;
	lf.lfCharSet = dwCharSet;
	_tcscpy(lf.lfFaceName, szFontFace);
	m_hUnderlineFont = CreateFontIndirect(&lf);

	Refresh();

	return 0;
}

int CellEdit::SetBackgroundColor(COLORREF color)
{
	if (m_hBackgroundBrush != NULL) DeleteObject(m_hBackgroundBrush);
	m_hBackgroundBrush = CreateSolidBrush(color);
//	InvalidateRect(m_hWnd, NULL, TRUE);
	return 0;
}

void CellEdit::SetStyle(DWORD dwStyle)
{
	m_dwStyle = dwStyle;
	Refresh();
}

/////////////////////////////////////////////////////////////////////////////
void CellEdit::OnDraw(TMemDC* pmemdc)
{
//	PAINTSTRUCT ps;
	HBITMAP hbmp;
	HDC hdc,hsec;
	FormattedText::iterator it;
//	hdc = BeginPaint(m_hWnd, &ps);
	hdc = pmemdc->GetHdc();
	
	hsec = CreateCompatibleDC(pmemdc->GetDVdc());
	hbmp = CreateCompatibleBitmap(pmemdc->GetDVdc(), m_rect.right, m_rect.bottom);

	SelectObject(hsec, hbmp);
	// Clear the background
	FillRect(hdc, &m_rect, m_hBackgroundBrush);
	FillRect(hsec, &m_rect, m_hBackgroundBrush);


	// Write the text
	SetBkMode(hdc, TRANSPARENT);
	SetBkMode(hsec, TRANSPARENT);

	// Paint the Selection
	if (IsSelected())
	{
		OutputDebugStringA("OnPaint\r\n");
		RECT rcsel = m_rect;
		POINT ptStart, ptEnd;
		int nBufferPos;

		nBufferPos = GetBufferPosFromIndex(m_Selection.start);
		ptStart = GetPointFromBufferPos(nBufferPos);

		nBufferPos = GetBufferPosFromIndex(m_Selection.end);
		ptEnd = GetPointFromBufferPos(nBufferPos);
		HBRUSH hbrsel = CreateSolidBrush(RGB(142, 153, 182));
		rcsel.left = ptStart.x; rcsel.right = ptEnd.x;
		OffsetRect(&rcsel, m_rect.left, 0);
//		FillRect(hdc, &rcsel, hbrsel);
		FillRect(hsec, &rcsel, hbrsel);
		DeleteObject(hbrsel);
	}

	it = m_FormattedText.begin();

	POINT raster; // current textout position, updated after each textout.
	SIZE  size;
	raster.x = m_ptTextOffset.x + m_rect.left;
	raster.y = m_ptTextOffset.y + m_rect.top;

	// Render each text block
	while (it != m_FormattedText.end())
	{
		LPFORMATTEDTEXTBLOCK lpft = (LPFORMATTEDTEXTBLOCK)&(*it);

		SetTextColor(hdc, lpft->clColor);

		switch (lpft->dwStyle)
		{
		case TEXTFORMAT_BOLD:
			SelectObject(hdc, m_hBoldFont);
			SelectObject(hsec, m_hBoldFont);
			break;
		case TEXTFORMAT_ITALIC:
			SelectObject(hdc, m_hItalicFont);
			SelectObject(hsec, m_hItalicFont);
			break;
		case TEXTFORMAT_UNDERLINE:
			SelectObject(hdc, m_hUnderlineFont);
			SelectObject(hsec, m_hUnderlineFont);
			break;
		default:
			SelectObject(hdc, m_hFont);
			SelectObject(hsec, m_hFont);
			break;
		}

		TextOut(hsec/*hdc*/, raster.x, raster.y, lpft->strBlock.c_str(), _tcslen(lpft->strBlock.c_str()));
		StretchBlt(hdc, m_rect.left, m_rect.top + EditBorder, _Width(m_rect), _Height(m_rect),
			hsec, m_rect.left, m_rect.top, _Width(m_rect), _Height(m_rect), SRCCOPY);
		GetTextExtentPoint32(hdc, lpft->strBlock.c_str(), _tcslen(lpft->strBlock.c_str()), &size);
		raster.x += size.cx;

		it++;
	}

	DeleteObject(hsec);
	DeleteObject(hbmp);
//	EndPaint(m_hWnd, &ps);
}

BOOL CellEdit::OnEraseBkgnd(WPARAM wparam, LPARAM lparam)
{
	return TRUE;
	HDC hdc = (HDC)wparam;

	SelectObject(hdc, m_hBackgroundBrush);
	PatBlt(hdc, 0, 0, m_nWidth, m_nHeight, PATCOPY);

	return TRUE;
}


LRESULT CellEdit::OnKillFocus(WPARAM wParam, LPARAM lParam)
{
	m_bfocus = false;
	DestroyCaret();
	return 0;
}

LRESULT CellEdit::OnMouseActivate(WPARAM wParam, LPARAM lParam)
{
	SetFocus(m_hWnd);
	return 0;
}

LRESULT CellEdit::OnSetFocus(WPARAM wParam, LPARAM lParam)
{
	ClearSelection();
	m_bfocus = true;
	if (!(m_dwStyle & FES_READONLY))
	{
		CreateCaret(m_hWnd, NULL, 1, m_nFontHeight);
		UpdateCaretPos();
	}
	return 0;
}

LRESULT CellEdit::OnChar(WPARAM wParam, LPARAM lParam)
{
	int   nKey = wParam;
	TCHAR szTemp[MAX_FE_BUFFER];
	BOOL  bBufferChanged = FALSE;
	BOOL  bTextOffsetChanged;

	if (m_ntype == Type_Int && nKey == '.')
		return 0;
	switch (wParam)
	{
	case VK_SPACE:
		if (m_dwStyle & FES_DISABLESPACE)
		{
			return OnKeyDown(VK_RIGHT, NULL);
		}
		else
		{
			if (!(m_dwStyle & FES_READONLY))
			{
				if (IsSelected()) Cut();

				bBufferChanged = PutChar(TEXT(' '));
			}
		}
		break;

		// Do nothing
	case VK_ESCAPE:
		if (IsSelected()) ClearSelection();
		return 0;

	case VK_RETURN:
	case VK_TAB:
		AdvanceNextTuple();
		break;

	case 1:
		// Ctrl+A, Select All
		m_Selection.start.nTupleIndex = 0;
		m_Selection.start.nPosition = 0;
		m_Selection.end.nTupleIndex = m_nTuples - 1;
		m_Selection.end.nPosition = _tcslen(BUFFER(m_nTuples - 1));
//		InvalidateRect(m_hWnd, NULL, TRUE);
		break;
	case 24:
		// Ctrl+X
		if (!(m_dwStyle & FES_READONLY))
		{
			Copy();
			Cut();
		}
		break;

	case 3:
		// Ctrl+C
		Copy();
		break;

	case 22:
		// Ctrl+V
		if (!(m_dwStyle & FES_READONLY))
		{
			Paste();
		}
		break;

	case 127:
		if (IsSelected()) ClearSelection();
		// Ctrl+Backspace, delete the current tuple
		if (!(m_dwStyle & FES_READONLY))
		{
			CURRENT_BUFFER[0] = TEXT('\0');
			CURSOR_POS = 0;
			bBufferChanged = TRUE;
		}
		break;

	case VK_BACK:

		if (!(m_dwStyle & FES_READONLY))
		{
			if (IsSelected())
			{
				Cut();
			}
			else
			{
				if (CURSOR_POS != 0)
				{
					_tcscpy(szTemp, CURRENT_BUFFER + CURSOR_POS);
					CURSOR_POS--;
					_tcscpy(CURRENT_BUFFER + CURSOR_POS, szTemp);
					bBufferChanged = TRUE;
				}
				else
				{
					return OnKeyDown(VK_LEFT, NULL);
				}
			}
		}

		break;

	default:
		// A printable character
		if (!(m_dwStyle & FES_READONLY))
		{
			if (IsSelected()) Cut();

			bBufferChanged = PutChar(wParam);
		}
	}


	if (bBufferChanged)	FormatText(m_FormattedText);

	bTextOffsetChanged = UpdateTextOffset();

	if (bTextOffsetChanged || bBufferChanged)
	{
		InvalidateRect(m_hWnd, NULL, TRUE);
	}

	UpdateCaretPos();

	return 0;
}


LRESULT CellEdit::OnKeyDown(WPARAM wParam, LPARAM lParam)
{
	int   nKey = wParam;
	TCHAR szTemp[MAX_FE_BUFFER];
	BOOL  bPaint = FALSE;
	BOOL  bBufferChanged = FALSE;
	BOOL  bTextOffsetChanged;


	switch (wParam)
	{
	case VK_LEFT:

		if (CURSOR_POS != 0)
		{
			CURSOR_POS--;
		}
		else
		{
			if (m_nCurrentTuple != 0) m_nCurrentTuple--;
		}

		if (m_bShiftDown)
		{
			m_Selection.end.nTupleIndex = m_nCurrentTuple;
			m_Selection.end.nPosition = CURSOR_POS;
			InvalidateRect(m_hWnd, NULL, TRUE);
		}
		else ClearSelection();

		break;

	case VK_RIGHT:


		if (CURSOR_POS != _tcslen(CURRENT_BUFFER))
		{
			CURSOR_POS++;
		}
		else
		{
			if (m_nCurrentTuple != (m_nTuples - 1)) m_nCurrentTuple++;
		}

		if (m_bShiftDown)
		{
			m_Selection.end.nTupleIndex = m_nCurrentTuple;
			m_Selection.end.nPosition = CURSOR_POS;
			InvalidateRect(m_hWnd, NULL, TRUE);
		}
		else ClearSelection();

		break;

	case VK_HOME:
		CURSOR_POS = 0;
		if (m_bShiftDown)
		{
			m_Selection.end.nTupleIndex = m_nCurrentTuple;
			m_Selection.end.nPosition = CURSOR_POS;
			InvalidateRect(m_hWnd, NULL, TRUE);
		}
		else ClearSelection();
		break;

	case VK_END:
		CURSOR_POS = _tcslen(CURRENT_BUFFER);
		if (m_bShiftDown)
		{
			m_Selection.end.nTupleIndex = m_nCurrentTuple;
			m_Selection.end.nPosition = CURSOR_POS;
			InvalidateRect(m_hWnd, NULL, TRUE);
		}
		else ClearSelection();
		break;

	case VK_DELETE:
		if (!(m_dwStyle & FES_READONLY))
		{
			if (!IsSelected())
			{
				if (CURSOR_POS != _tcslen(CURRENT_BUFFER))
				{
					_tcscpy(szTemp, CURRENT_BUFFER + CURSOR_POS + 1);
					_tcscpy(CURRENT_BUFFER + CURSOR_POS, szTemp);
					bBufferChanged = TRUE;
				}
			}
			else Cut();
		}
		break;

	case VK_SHIFT:
		m_bShiftDown = TRUE;
		if (!IsSelected())
		{
			m_Selection.start.nTupleIndex = m_nCurrentTuple;
			m_Selection.start.nPosition = CURSOR_POS;
			m_Selection.end = m_Selection.start;
		}
		break;

	}

	if (bBufferChanged) FormatText(m_FormattedText);

	bTextOffsetChanged = UpdateTextOffset();

	if (bTextOffsetChanged || bBufferChanged)
	{
		InvalidateRect(m_hWnd, NULL, TRUE);
	}

	UpdateCaretPos();


	return 0;
}


LRESULT CellEdit::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
	if (!m_bfocus)
		return 0;
	POINT pt;
	int nBufferPos;
	pt.x = LOWORD(lParam);
	pt.y = HIWORD(lParam);

	nBufferPos = GetBufferPosFromPoint(&pt);
	char sztemp[100];
	sprintf_s(sztemp, "%d\r\n", nBufferPos);
	OutputDebugStringA(sztemp);
	if (nBufferPos != -1)
	{
		TUPLEPOS tuplepos;

		if (GetIndexFromBufferPos(nBufferPos, &tuplepos) != -1)
		{
			SetCaretPos(tuplepos);

			m_Selection.start = tuplepos;
			sprintf_s(sztemp, "OnLButtonDown%d\r\n", tuplepos.nPosition);
			OutputDebugStringA(sztemp);
			ClearSelection();
		}
	}


	m_bLButtonDown = TRUE;

	return 0;
}

LRESULT CellEdit::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
	int xPos = LOWORD(lParam);
	int yPos = HIWORD(lParam);


	TUPLEPOS tuplepos;
	POINT pt;
	pt.x = xPos;
	pt.y = yPos;

	int nBufferPos = GetBufferPosFromPoint(&pt);

	if (GetIndexFromBufferPos(nBufferPos, &tuplepos) != -1)
	{
		if (m_bLButtonDown)
		{
			char sztemp[MAX_PATH];
			sprintf_s(sztemp, "OnMouseMove%d\r\n", tuplepos.nPosition);
			OutputDebugStringA(sztemp);
			m_Selection.end = tuplepos;
			InvalidateRect(m_hWnd, NULL, TRUE);
			SetCaretPos(tuplepos);
		}

	}

	return 0;
}

LRESULT CellEdit::OnLButtonUp(WPARAM wParam, LPARAM lParam)
{
	m_bLButtonDown = FALSE;

	return 0;
}

LRESULT CellEdit::OnKeyUp(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case VK_SHIFT:
		m_bShiftDown = FALSE;
		break;
	}

	return 0;
}

LRESULT CellEdit::OnRButtonUp(WPARAM wParam, LPARAM lParam)
{
	int xPos = LOWORD(lParam);
	int yPos = HIWORD(lParam);

	POINT ptClick;
	ptClick.x = xPos;
	ptClick.y = yPos;

	//	if(GetBufferPosFromPoint(&ptClick) == -1) return -1;
	ClientToScreen(m_hWnd, &ptClick);
	TrackPopupMenu(m_hMenu, TPM_LEFTALIGN | TPM_TOPALIGN, ptClick.x, ptClick.y, NULL, m_hWnd, NULL);

	return 0;
}

LRESULT CellEdit::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{
	case IDM_CUT:
		OnChar(24, NULL);
		break;
	case IDM_COPY:
		OnChar(3, NULL);
		break;
	case IDM_PASTE:
		OnChar(22, NULL);
		break;
	case IDM_SELECTALL:
		OnChar(1, NULL);
		break;
	case IDM_CLEARTUPLE:
		ClearSelection();
		OnChar(127, NULL);
		break;
	case IDM_CLEARALLTUPLES:
		ClearSelection();
		for (int a = 0; a < m_nTuples; a++)
		{
			BUFFER(a)[0] = TEXT('\0');
			m_TupleData[a].nCursorPos = 0;
		}
		m_nCurrentTuple = 0;
		Refresh();
		break;
	}

	return 0;
}

LRESULT CellEdit::OnDblClick(WPARAM wParam, LPARAM lParam)
{
	TUPLEPOS stbegin, stend;
	stbegin.nPosition = 0;
	stbegin.nTupleIndex = 0;
	stend = stbegin;
	stend.nPosition = _tcslen(BUFFER(0));
	SetSelection(stbegin, stend);
	return 0;
	int xPos = LOWORD(lParam);
	int yPos = HIWORD(lParam);

	POINT ptClick;
	ptClick.x = xPos;
	ptClick.y = yPos;

	int nBufferPos = GetBufferPosFromPoint(&ptClick);

	if (nBufferPos != -1)
	{
		TUPLEPOS tuplepos;

		if (GetIndexFromBufferPos(nBufferPos, &tuplepos) != -1)
		{
			m_Selection.start.nTupleIndex = tuplepos.nTupleIndex;
			m_Selection.start.nPosition = 0;

			m_Selection.end.nTupleIndex = tuplepos.nTupleIndex;
			m_Selection.end.nPosition = _tcslen(BUFFER(tuplepos.nTupleIndex));

			SetCaretPos(m_Selection.end);

			InvalidateRect(m_hWnd, NULL, TRUE);
		}
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
void CellEdit::PreCreate()
{
	m_nTuples = 1;
	m_TupleData.reserve(m_nTuples);
	for (int a = 0; a < m_nTuples; a++)
	{
		TUPLEDATA tupledata;

		tupledata.nIndex = a;
		tupledata.nCursorPos = 0;
		tupledata.nTextLimit = MAX_FE_BUFFER;
		tupledata.szBuffer = new TCHAR[MAX_FE_BUFFER];
		tupledata.szBuffer[0] = TEXT('\0');

		m_TupleData.push_back(tupledata);
	}

	m_nCurrentTuple = 0;
	SetBackgroundColor(RGB(255, 255, 225));
	SetFont(TEXT("Verdana"), 14);
}
void CellEdit::ClearData()
{
	if (m_hFont != NULL) DeleteObject(m_hFont);
	if (m_hBoldFont != NULL) DeleteObject(m_hBoldFont);
	if (m_hItalicFont != NULL) DeleteObject(m_hItalicFont);
	if (m_hUnderlineFont != NULL) DeleteObject(m_hUnderlineFont);
	if (m_hBackgroundBrush != NULL) DeleteObject(m_hBackgroundBrush);

	if (!m_TupleData.empty())
	{
		for (int a = 0; a < m_nTuples; a++) delete BUFFER(a);

		m_TupleData.clear();
	}
}


POINT CellEdit::GetCaretPos(const FormattedText &ft) const
{
	POINT pt;
	TUPLEPOS tuplepos;

	tuplepos.nTupleIndex = m_nCurrentTuple;
	tuplepos.nPosition = CURSOR_POS;

	//	SIZE size = GetFormattedTextSize(m_FormattedText, MAKELONG(m_nCurrentTuple, CURSOR_POS));
	SIZE size = GetFormattedTextSize(m_FormattedText, GetBufferPosFromIndex(tuplepos));

	pt.x = size.cx;
	pt.y = size.cy;

	return pt;
}

// Returns TRUE if the a character is inserted to the buffer, FALSE otherwise.
BOOL CellEdit::PutChar(TCHAR ch)
{
	TCHAR szTemp[MAX_FE_BUFFER];

	// Check if the buffer limit is reached
	if (_tcslen(CURRENT_BUFFER) == CURRENT_TUPLE.nTextLimit)
	{
		MessageBeep(-1);
		return FALSE;
	}

	if (m_dwStyle & FES_NUMERIC)
	{
		if (ch < TEXT('0') || ch > TEXT('9')) return FALSE;
	}

	if (m_dwStyle & FES_LOWERCASE)
	{
		if (isalpha(ch))	ch = tolower(ch);
	}

	if (m_dwStyle & FES_UPPERCASE)
	{
		if (isalpha(ch)) ch = toupper(ch);
	}

	// Insert the character
	_tcscpy(szTemp, CURRENT_BUFFER + CURSOR_POS);
	CURRENT_BUFFER[CURSOR_POS] = (TCHAR)ch;
	CURSOR_POS++;
	_tcscpy(CURRENT_BUFFER + CURSOR_POS, szTemp);

	return TRUE;
}


SIZE CellEdit::GetTextSize(const FormattedText &ft) const
{

	SIZE size, sizetemp;
	LPFORMATTEDTEXTBLOCK lpft;
	FormattedText::const_iterator it;
	HDC hdc;

	size.cx = 0;
	size.cy = 0;

	if (ft.empty()) return size;

	hdc = GetDC(m_hWnd);
	it = ft.begin();
	while (it != ft.end())
	{
		lpft = (LPFORMATTEDTEXTBLOCK)&(*it);
		switch (lpft->dwStyle)
		{
		case TEXTFORMAT_BOLD:
			SelectObject(hdc, m_hBoldFont);
			break;
		case TEXTFORMAT_ITALIC:
			SelectObject(hdc, m_hItalicFont);
			break;
		case TEXTFORMAT_UNDERLINE:
			SelectObject(hdc, m_hUnderlineFont);
			break;
		default:
			SelectObject(hdc, m_hFont);
		}

		GetTextExtentPoint32(hdc, lpft->strBlock.c_str(), _tcslen(lpft->strBlock.c_str()), &sizetemp);

		size.cx += sizetemp.cx;
		if (sizetemp.cy > size.cy) size.cy = sizetemp.cy;

		it++;
	}

	ReleaseDC(m_hWnd, hdc);

	return size;
}

BOOL CellEdit::UpdateTextOffset()
{
	POINT pt;
	SIZE  sizetext;
	BOOL  bRet;
	TUPLEPOS tuplepos;

	tuplepos.nTupleIndex = -1;
	tuplepos.nPosition = NULL;

	pt = m_ptTextOffset;

	//	sizetext = GetTextSize(m_FormattedText);
	//  sizetext = GetFormattedTextSize(m_FormattedText, -1);		
	sizetext = GetFormattedTextSize(m_FormattedText, GetBufferPosFromIndex(tuplepos));

	// Center the text vertically
//	if (m_nHeight > sizetext.cy)	//zxw
//	{
//		pt.y = (m_nHeight - sizetext.cy) / 2;
//	}
//	else
//	{
//		pt.y = -(sizetext.cy - m_nHeight) / 2;
//	}
	pt.y = 5;

	if (sizetext.cx < (m_nWidth - 5 * 2))
	{
		// text size is smaller than window width		

		if (m_dwStyle & FES_CENTERTEXT)
		{
			pt.x = (m_nWidth - sizetext.cx) / 2;
		}
		else
		{
			pt.x = 5;
		}
	}
	else
	{
		// text size is larger than window width

		POINT ptCaret = GetCaretPos(m_FormattedText);

		if ((ptCaret.x > (-m_ptTextOffset.x + 5)) && (ptCaret.x < (-m_ptTextOffset.x + m_nWidth - 5)))
		{
			// caret in the view, m_ptTextOffset needn't change.
			//			return FALSE;
		}
		else
		{
			// caret outside the view
			// set the start offset so that the caret is positioned in the center
			pt.x = -(ptCaret.x - m_nWidth / 2);
		}

	}

	// Check whether m_ptTextOffset has changed.
	// No need to paint the window if m_ptTextOffset has not changed.
	// Return TRUE if the window needs to be repainted.

	bRet = FALSE;
//	pt.x = m_rect.left;pt.y = m_rect.top;//zxw
	if ((pt.x != m_ptTextOffset.x) || (pt.y != m_ptTextOffset.y)) bRet = TRUE;

	wchar_t* ptext = m_TupleData[0].szBuffer;
	m_ptTextOffset = pt;

	return bRet;
}

int CellEdit::AdvanceNextTuple()
{
	m_nCurrentTuple = (m_nCurrentTuple + 1) % m_nTuples;

	return m_nCurrentTuple;
}

FormatTextProc CellEdit::SetFormatTextProc(FormatTextProc lpfnNewFormatTextProc)
{
	FormatTextProc lpfnRet = m_lpfnFormatTextProc;

	if (lpfnNewFormatTextProc == NULL) m_lpfnFormatTextProc = DefFormatTextProc;
	else m_lpfnFormatTextProc = lpfnNewFormatTextProc;

	Refresh();

	return lpfnRet;
}

int CellEdit::FormatText(FormattedText &ft)
{
	FORMATTEDTEXTBLOCK fmtborders;

	ft.clear();
	fmtborders.clColor = RGB(0, 0, 0);
	fmtborders.dwStyle = TEXTFORMAT_BOLD;
	fmtborders.strBlock = TEXT("");
	ft.push_back(fmtborders);

	for (int a = 0; a < m_nTuples; a++)
	{
		// call the formatting function for each tuple
		m_lpfnFormatTextProc(a, BUFFER(a), ft);

		if (a != (m_nTuples - 1))
		{
			fmtborders.strBlock = TEXT(" , ");
			ft.push_back(fmtborders);
		}
	}

	fmtborders.strBlock = TEXT("");
	ft.push_back(fmtborders);

	return 0;
}

int CellEdit::UpdateCaretPos()
{
	TUPLEPOS tuplepos;

	tuplepos.nTupleIndex = m_nCurrentTuple;
	tuplepos.nPosition = CURSOR_POS;

	wchar_t* ptext = m_TupleData[0].szBuffer;
	//	SIZE size = GetFormattedTextSize(m_FormattedText, MAKELONG(m_nCurrentTuple, CURSOR_POS));
	SIZE size = GetFormattedTextSize(m_FormattedText, GetBufferPosFromIndex(tuplepos));

	//zxw
	::SetCaretPos(size.cx + m_ptTextOffset.x + m_rect.left, m_ptTextOffset.y + m_rect.top);
	ShowCaret(m_hWnd);

	return 0;
}

SIZE CellEdit::GetFormattedTextSize(const FormattedText&, int nPosition) const
{
	HDC hdc;
	SIZE  size, sizeret;
	FormattedText::const_iterator it;
	FORMATTEDTEXTBLOCK lpft;
	int a, rasterpos = 0;
	int nCurPos = 0;

	sizeret.cx = 0;
	sizeret.cy = 0;

	if (m_FormattedText.empty()) return sizeret;

	TCHAR *szTemp = new TCHAR[MAX_FE_BUFFER*m_nTuples];

	hdc = GetDC(m_hWnd);

	it = m_FormattedText.begin();

	do
	{
		lpft = *it;

		switch (lpft.dwStyle)
		{
		case TEXTFORMAT_BOLD:
			SelectObject(hdc, m_hBoldFont);
			break;
		case TEXTFORMAT_ITALIC:
			SelectObject(hdc, m_hItalicFont);
			break;
		case TEXTFORMAT_UNDERLINE:
			SelectObject(hdc, m_hUnderlineFont);
			break;

		default:
			SelectObject(hdc, m_hFont);
		}

		GetTextExtentPoint32(hdc, lpft.strBlock.c_str(), _tcslen(lpft.strBlock.c_str()), &size);

		rasterpos += size.cx;

		if (size.cy > sizeret.cy) sizeret.cy = size.cy;

		nCurPos += _tcslen(lpft.strBlock.c_str());

		it++;

	} while (nCurPos < nPosition&&it!= m_FormattedText.end());

	rasterpos -= size.cx;
	nCurPos -= _tcslen(lpft.strBlock.c_str());

	GetTextExtentPoint32(hdc, lpft.strBlock.c_str(), nPosition - nCurPos, &size);//zxw  nPosition - nCurPos

	rasterpos += size.cx;

	sizeret.cx += rasterpos;

	ReleaseDC(m_hWnd, hdc);

	delete szTemp;

	return sizeret;
}

void CellEdit::Refresh()
{
	FormatText(m_FormattedText);
	UpdateTextOffset();
	UpdateCaretPos();
	InvalidateRect(m_hWnd, NULL, TRUE);
}

int CellEdit::SetCaretPos(TUPLEPOS tuplepos)
{
	// check the index
	if ((tuplepos.nTupleIndex < 0) || (tuplepos.nTupleIndex >= m_nTuples)) return -1;

	m_nCurrentTuple = tuplepos.nTupleIndex;

	CURSOR_POS = (tuplepos.nPosition > _tcslen(BUFFER(tuplepos.nTupleIndex))) ? _tcslen(BUFFER(tuplepos.nTupleIndex)) : tuplepos.nPosition;

	BOOL bPaint = UpdateTextOffset();
	UpdateCaretPos();

	if (bPaint) InvalidateRect(m_hWnd, NULL, TRUE);

	return 0;
}

int CellEdit::GetBufferPosFromIndex(TUPLEPOS tuplepos) const
{
	int nBufferPosition = 0;
	FormattedText::const_iterator it;
	int *nLength = new int[m_nTuples];

	for (int a = 0; a < m_nTuples; a++) nLength[a] = _tcslen(BUFFER(a));

	if (tuplepos.nTupleIndex == -1)
	{
		// calculate the size of the whole text
		it = m_FormattedText.begin();
		while (it != m_FormattedText.end())
		{
			nBufferPosition += _tcslen(((LPFORMATTEDTEXTBLOCK)&(*it))->strBlock.c_str());
			it++;
		}
	}
	else
	{
		for (int a = 0; a < tuplepos.nTupleIndex; a++) nBufferPosition += nLength[a];
//		nBufferPosition += (tuplepos.nTupleIndex + 1) * 3;//zxw
		nBufferPosition += tuplepos.nPosition;
	}

	delete nLength;

	return nBufferPosition;
}

int CellEdit::GetIndexFromBufferPos(int nPosition, LPTUPLEPOS lptp) const
{
	TUPLEPOS tuplepos;

	tuplepos.nTupleIndex = -1;
	tuplepos.nPosition = NULL;

	// Get the whole buffer length
	int nBufferLength = GetBufferPosFromIndex(tuplepos);

	if (nPosition <= 0/*3*/)
	{
		lptp->nTupleIndex = 0;
		lptp->nPosition = 0;

		return 0;
	}

	if (nPosition >= nBufferLength/*(nBufferLength - 2)*/)
	{
		lptp->nTupleIndex = m_nTuples - 1;
		lptp->nPosition = _tcslen(BUFFER(m_nTuples - 1));
		return 0;
	}


	int *nLength = new int[m_nTuples];

	for (int a = 0; a < m_nTuples; a++) nLength[a] = _tcslen(BUFFER(a));


	int nCurpos = 0, a;
	for (a = 0; a < m_nTuples; a++)
	{
		nCurpos +=/* 3 + */nLength[a];

		if (nCurpos >= nPosition) break;
	}

	lptp->nTupleIndex = a;
	lptp->nPosition = nPosition - (nCurpos - nLength[a]);

	delete nLength;

	if (lptp->nPosition < 0) return -1;

	return 0;
}

int CellEdit::GetBufferPosFromPoint(LPPOINT lppt) const
{
	BOOL bFound = FALSE;
	SIZE size, sizeprev;
	TUPLEPOS tuplepos;

	//zxw
	POINT pt = *lppt;
	pt.x -= m_rect.left;
	pt.y -= m_rect.top;

	tuplepos.nTupleIndex = -1;
	tuplepos.nPosition = NULL; 

	int nBufferLength = GetBufferPosFromIndex(tuplepos);
	int nPosition = 0;

	sizeprev.cx = 0;
	sizeprev.cy = 0;

	while (nPosition < nBufferLength)
	{
		size = GetFormattedTextSize(m_FormattedText, nPosition);

		if ((m_ptTextOffset.x <= pt.x) && ((m_ptTextOffset.x + size.cx) >= pt.x))
		{
			bFound = TRUE;

			int nDiff1 = pt.x - (m_ptTextOffset.x + sizeprev.cx);
			int nDiff2 = (m_ptTextOffset.x + size.cx) - pt.x;

			if (nDiff1 <= nDiff2) nPosition--;

			break;
		}

		nPosition++;
		sizeprev = size;
	}

	size = GetFormattedTextSize(m_FormattedText, nBufferLength);
	if (bFound)
		return nPosition;
	else if (lppt->x <= m_rect.left)
		return 0;
	else if (lppt->x >= m_rect.left + size.cx)
		return nPosition;
	else if (lppt->x <= m_rect.right && m_ptTextOffset.x + size.cx < lppt->x)//zxw
		return nPosition;
	else return -1;

}

BOOL CellEdit::IsSelected() const
{
	// If selection start end are the same positions than there is no selection
	if (SELECTION_EQUAL(m_Selection.start, m_Selection.end)) return FALSE;

	return TRUE;
}


POINT CellEdit::GetPointFromBufferPos(int nBufferPosition) const
{
	POINT pt;

	SIZE size = GetFormattedTextSize(m_FormattedText, nBufferPosition);

	pt.x = m_ptTextOffset.x + size.cx;
	pt.y = m_ptTextOffset.y;

	return pt;
}

int CellEdit::SetSelection(TUPLEPOS start, TUPLEPOS end)
{
	m_Selection.start = start;
	m_Selection.end = end;
	OutputDebugStringA("SetSelection\r\n");
	InvalidateRect(m_hWnd, NULL, TRUE);

	return 0;
}

int CellEdit::ClearSelection()
{
	m_Selection.end = m_Selection.start;
	OutputDebugStringA("ClearSelection\r\n");
	InvalidateRect(m_hWnd, NULL, TRUE);

	return 0;

}

int CellEdit::GetSelection(TCHAR *szBuffer)
{
	*szBuffer = TEXT('\0');

	// positions are the same, no selection
	if (ReorderSelectionPositions(&m_Selection.start, &m_Selection.end) == 0) return -1;

	TUPLEPOS current = m_Selection.start;

	TCHAR *cp = BUFFER(m_Selection.start.nTupleIndex) + m_Selection.start.nPosition;
	TCHAR *szDest = szBuffer;

	while (!SELECTION_EQUAL(current, m_Selection.end))
	{
		cp = BUFFER(current.nTupleIndex) + current.nPosition;

		*szDest = *cp;
		szDest++;

		current.nPosition++;

		if (current.nPosition >= _tcslen(BUFFER(current.nTupleIndex)))
		{
			if (SELECTION_EQUAL(current, m_Selection.end)) break;

			szDest[0] = TEXT(' ');
			szDest[1] = TEXT(',');
			szDest[2] = TEXT(' ');
			szDest += 3;

			current.nTupleIndex++;
			current.nPosition = 0;
		}
	}

	*szDest = TEXT('\0');
	return 0;
}

int CellEdit::Cut()
{
	TCHAR szBuffer[MAX_FE_BUFFER];
	TCHAR szLeftBuffer[MAX_FE_BUFFER], szRightBuffer[MAX_FE_BUFFER];

	// no selection
	if (ReorderSelectionPositions(&m_Selection.start, &m_Selection.end) == 0) return -1;

	_tcsncpy(szLeftBuffer, BUFFER(m_Selection.start.nTupleIndex), m_Selection.start.nPosition);
	szLeftBuffer[m_Selection.start.nPosition] = TEXT('\0');

	_tcscpy(szRightBuffer, BUFFER(m_Selection.end.nTupleIndex) + m_Selection.end.nPosition);

	for (int a = m_Selection.start.nTupleIndex; a <= m_Selection.end.nTupleIndex; a++)
	{
		BUFFER(a)[0] = TEXT('\0');
	}

	_tcscpy(BUFFER(m_Selection.start.nTupleIndex), szLeftBuffer);
	_tcscat(BUFFER(m_Selection.end.nTupleIndex), szRightBuffer);

	m_nCurrentTuple = m_Selection.start.nTupleIndex;
	CURSOR_POS = m_Selection.start.nPosition;

	ClearSelection();

	Refresh();
	return 0;
}

int CellEdit::Copy()
{
	TCHAR szBuffer[MAX_FE_BUFFER];


	if (GetSelection(szBuffer) != -1)
	{
		// copy selection to clipboard
		if (OpenClipboard(m_hWnd))
		{
			EmptyClipboard();

			HANDLE hMem = GlobalAlloc(GMEM_MOVEABLE, (_tcslen(szBuffer) + 1) * sizeof(TCHAR));

			if (hMem != NULL)
			{
				LPTSTR szData = (LPTSTR)GlobalLock(hMem);

				memcpy(szData, szBuffer, (_tcslen(szBuffer) + 1) * sizeof(TCHAR));

				SetClipboardData(CF_TEXT, hMem);

				GlobalUnlock(hMem);
			}

			CloseClipboard();
		}
	}


	return 0;
}

int CellEdit::Paste()
{
	TCHAR szBuffer[MAX_FE_BUFFER], szTemp[MAX_FE_BUFFER];

	szBuffer[0] = TEXT('\0');

	if (IsSelected()) Cut();

	if (OpenClipboard(m_hWnd))
	{
		if (IsClipboardFormatAvailable(CF_TEXT))
		{
			HANDLE hData = GetClipboardData(CF_TEXT);

			if (hData != NULL)
			{
				LPTSTR szData = (LPTSTR)GlobalLock(hData);

				_tcscpy(szBuffer, szData);

				GlobalUnlock(hData);
			}
		}

		CloseClipboard();
	}

	// Nothing to paste
	if (*szBuffer == TEXT('\0')) return -1;

	int nLeftSpace = m_TupleData[m_nCurrentTuple].nTextLimit - _tcslen(CURRENT_BUFFER);

	if (nLeftSpace != 0)
	{
		int nCopyCount;

		_tcsncpy(szTemp, CURRENT_BUFFER, CURSOR_POS);
		_tcscpy(szTemp + CURSOR_POS, szBuffer);
		_tcscat(szTemp, CURRENT_BUFFER + CURSOR_POS);

		nCopyCount = (_tcslen(szTemp) > nLeftSpace) ? m_TupleData[m_nCurrentTuple].nTextLimit : _tcslen(szTemp);

		_tcsncpy(CURRENT_BUFFER, szTemp, nCopyCount);
		CURRENT_BUFFER[nCopyCount] = TEXT('\0');
		CURSOR_POS += (_tcslen(szBuffer) < nLeftSpace) ? _tcslen(szBuffer) : nLeftSpace;

		Refresh();
	}
	else MessageBeep(-1);


	return 0;
}

int CellEdit::ReorderSelectionPositions(LPTUPLEPOS pos1, LPTUPLEPOS pos2)
{

	TUPLEPOS temp1 = *pos1;
	TUPLEPOS temp2 = *pos2;

	if (SELECTION_EQUAL(temp1, temp2)) return 0;

	if (pos2->nTupleIndex < pos1->nTupleIndex)
	{
		TUPLEPOS temp = *pos1;
		*pos1 = *pos2;
		*pos2 = temp;

	}
	else if ((pos1->nTupleIndex == pos2->nTupleIndex) && (pos2->nPosition < pos1->nPosition))
	{
		int nTemp = pos1->nPosition;
		pos1->nPosition = pos2->nPosition;
		pos2->nPosition = nTemp;
	}
	return 1;
}

//////////////////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK CellEdit::DefFormatTextProc(int nIndex, const TCHAR *szText, FormattedText &ft)
{
	FORMATTEDTEXTBLOCK fmttext;

	if (*szText == TEXT('\0')) return 0;

	// Default formatting : black, normal font
	fmttext.dwStyle = 0;
	fmttext.clColor = RGB(0, 0, 0);
	fmttext.strBlock = szText;
	ft.push_back(fmttext);

	return 0;
}

LRESULT CALLBACK MyFormatTextProc(int nIndex, const TCHAR *szText, FormattedText &ft)
{
	FORMATTEDTEXTBLOCK fmttext;

	if (*szText == TEXT('\0')) return 0;

	TCHAR *szBuffer = new TCHAR[MAX_FE_BUFFER];

	const TCHAR *cpStart = szText, *cpCurrent = szText;

	while (*cpCurrent)
	{
		if (isdigit(*cpCurrent))
		{
			// Numbers

			while (isdigit(*cpCurrent)) cpCurrent++;

			_tcsncpy(szBuffer, cpStart, cpCurrent - cpStart);
			szBuffer[cpCurrent - cpStart] = TEXT('\0');

			fmttext.dwStyle = 0;
			fmttext.clColor = RGB(0, 0, 255);
			fmttext.strBlock = szBuffer;
			ft.push_back(fmttext);

			cpStart = cpCurrent;
		}
		else if (isalpha(*cpCurrent))
		{
			// alphanumeric

			while (isalpha(*cpCurrent)) cpCurrent++;

			_tcsncpy(szBuffer, cpStart, cpCurrent - cpStart);
			szBuffer[cpCurrent - cpStart] = TEXT('\0');

			fmttext.dwStyle = 0;
			fmttext.clColor = RGB(165, 0, 165);
			fmttext.strBlock = szBuffer;
			ft.push_back(fmttext);

			cpStart = cpCurrent;
		}
		else
		{
			// other characters

			while (!isdigit(*cpCurrent) && !isalpha(*cpCurrent) && *cpCurrent != '\0') cpCurrent++;

			_tcsncpy(szBuffer, cpStart, cpCurrent - cpStart);
			szBuffer[cpCurrent - cpStart] = TEXT('\0');

			fmttext.dwStyle = TEXTFORMAT_BOLD;
			fmttext.clColor = RGB(0, 130, 0);
			fmttext.strBlock = szBuffer;
			ft.push_back(fmttext);

			cpStart = cpCurrent;
		}

	}

	delete szBuffer;

	return 0;
}