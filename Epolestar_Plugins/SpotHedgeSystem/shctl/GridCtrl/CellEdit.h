#ifndef _MINI_EDIT_H_
#define _MINI_EDIT_H_

class  CellEdit
{
public:
	CellEdit();
	~CellEdit();
	enum { Type_Common = 0, Type_Int,Type_Real };
public:
	void					Create(HWND hwnd);
	void					UseOwnerProc(SUBCLASSPROC wndproc);
	void					SetRect(int nleft, int ntop, int nwidth, int nheight);
	int						GetText(int nindex, TCHAR* ptext);
	int						SetText(int nindex, const TCHAR *ptext);
	int						SetText(int nindex,const char* ptext);
	int						SetTextLimit(int nindex, int nlimit);
	int						SetFont(const TCHAR *szFontFace, int nFontHeight, DWORD dwCharSet = ANSI_CHARSET);
	int						SetBackgroundColor(COLORREF color);
	void					SetStyle(DWORD dwstyle);
	void					SetEditType(int ntype) { m_ntype = ntype; }
	int						GetEditType() { return m_ntype; }
	SELECTION				GetSelection() { return m_Selection; }
public:
	bool					GetFocus() { return m_bfocus; }
	RECT					GetRect() { return m_rect; }
public:
	void					OnDraw(TMemDC* pmemdc);
	BOOL					OnEraseBkgnd(WPARAM wparam, LPARAM lparam);
	LRESULT					OnKillFocus(WPARAM, LPARAM);
	LRESULT					OnMouseActivate(WPARAM, LPARAM);
	LRESULT					OnSetFocus(WPARAM, LPARAM);
	LRESULT					OnChar(WPARAM, LPARAM);
	LRESULT					OnKeyDown(WPARAM wParam, LPARAM lParam);
	LRESULT					OnLButtonDown(WPARAM wParam, LPARAM lParam);
	LRESULT					OnMouseMove(WPARAM wParam, LPARAM lParam);
	LRESULT					OnLButtonUp(WPARAM wParam, LPARAM lParam);
	LRESULT					OnKeyUp(WPARAM wParam, LPARAM lParam);
	LRESULT					OnRButtonUp(WPARAM wParam, LPARAM lParam);
	LRESULT					OnCommand(WPARAM wParam, LPARAM lParam);
	LRESULT					OnDblClick(WPARAM wParam, LPARAM lParam);
private:
	void					PreCreate();
	void					ClearData();
	POINT					GetCaretPos(const FormattedText&) const;
	BOOL					PutChar(TCHAR);
	SIZE					GetTextSize(const FormattedText &ft) const;
	BOOL					UpdateTextOffset();
	int						AdvanceNextTuple();
	FormatTextProc			SetFormatTextProc(FormatTextProc lpfnNewFormatTextProc);
	int						FormatText(FormattedText& ft);
	int						UpdateCaretPos();
	SIZE					GetFormattedTextSize(const FormattedText&, int nPosition) const;
	void					Refresh();
	int						SetCaretPos(TUPLEPOS tuplepos);
	int						GetBufferPosFromIndex(TUPLEPOS tuplepos) const;
	int						GetIndexFromBufferPos(int nPosition, LPTUPLEPOS lptp) const;
	int						GetBufferPosFromPoint(LPPOINT lppt) const;
	BOOL					IsSelected() const;
	POINT					GetPointFromBufferPos(int nBufferPosition) const;
	int						SetSelection(TUPLEPOS start, TUPLEPOS end);
	int						ClearSelection();
	int						GetSelection(TCHAR *szBuffer);
	int						Cut();
	int						Copy();
	int						Paste();
	int						ReorderSelectionPositions(LPTUPLEPOS pos1, LPTUPLEPOS pos2);
////////////////////////////////////////////////////////////////////////////////////////////////
protected:
	static LRESULT CALLBACK DefFormatTextProc(int nIndex, const TCHAR *szText, FormattedText&);
private:// Local Data
	int					m_nWidth;
	int					m_nHeight;
	POINT				m_ptTextOffset;
	int					m_nTuples;
	DWORD				m_dwStyle;

	int				   m_nCurrentTuple;
	TCHAR			   m_szFontFace[32];
	int				   m_nFontHeight;
	TupleData          m_TupleData;
	FormatTextProc     m_lpfnFormatTextProc;
	FormattedText      m_FormattedText;
private:
	BOOL               m_bLButtonDown;
	BOOL               m_bShiftDown;
	SELECTION          m_Selection;
	RECT			   m_rect;
private:
	HMENU              m_hMenu;
	HWND			   m_hWnd;
	HFONT			   m_hFont, m_hBoldFont, m_hItalicFont, m_hUnderlineFont;
	HBRUSH			   m_hBackgroundBrush;
public:
	WNDPROC			   m_oldProc;
protected:
	bool			   m_bfocus;
	int				   m_nid;
	SUBCLASSPROC	   m_subproc;
	int				   m_ntype;
};

LRESULT CALLBACK MyFormatTextProc(int nIndex, const TCHAR *szText, FormattedText &ft);

#endif
