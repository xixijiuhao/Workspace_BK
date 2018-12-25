#ifndef _T_EDIT_H_
#define _T_EDIT_H_

#define EditText_MaxLength				50
class MiniEdit
{
public:
	MiniEdit();
	virtual ~MiniEdit();
	enum{Type_Common = 0,Type_Int};
public:
	bool					Create(HWND hwnd);
	void					UseOwnerProc(SUBCLASSPROC wndproc);
	void					SetFont(HFONT hfont){ m_font = hfont; }
	void					SetRect(int nleft,int ntop,int nwidth,int nheight);
	void					SetText(const wchar_t* ptext);
	void					SetText(const char* ptext);
	void					GetText(wchar_t* ptext,int nlength);
	void					SetEditType(int ntype) { m_ntype = Type_Int; }
	void					SetColor(COLORREF clrbk, COLORREF clrframe = 0xFFFFFF) { m_clrbk = clrbk; m_clrframe = clrframe; }
public:
	bool					GetFocus() { return m_bfocus; }
	RECT					GetRect() { return m_rect; }
private:
	RECT											m_rect;
	HWND											m_hwnd;
	bool											m_bfocus;
	COLORREF										m_clrbk;
	COLORREF										m_clrframe;
public:
	void											OnDraw(TMemDC* pmemdc);
	void											OnKeyDown(WPARAM wparam, LPARAM lparam);
	void											OnChar(WPARAM wparam,LPARAM lparam);
	void											OnLButtonDown(WPARAM wparam, LPARAM lparam);
	void											OnLButtonUp(WPARAM wparam, LPARAM lparam);
	void											OnMouseMove(WPARAM wparam, LPARAM lparam);
	void											OnMouseLeave();
	void											OnSetFocus();
	void											OnKillFocus();
private:
	wchar_t					m_wsztext[EditText_MaxLength + 1];
private:
	int						m_ncaretpos;
	int						m_nscrollbegin;
private:
	void					UpdateData();
	void					_SetCaretPos(int ncaretpos);
	int						GetClickCaretPos(POINT pt);
	void					CaretPos2Position(int ncaretpos, POINT& pt);
	bool					DeleteSelectText();
private:
	int						m_nwidth;
	int						m_nheight;
	int						m_ncarettop;
	HFONT					m_font;
private:
	bool					m_blbtndown;
	int						m_nselbegin;
	int						m_nselend;
	bool					m_btrack;
	int						m_ntype;
public:
	WNDPROC					m_oldProc;
protected:
	int						m_nid;
	SUBCLASSPROC			m_subproc;
};
#endif