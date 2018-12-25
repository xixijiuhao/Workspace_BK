#ifndef _T_ORDER_BUTTON_H
#define _T_ORDER_BUTTON_H

class TOrderButton :public TIxFrm
{
public:
	TOrderButton();
	void Create(HWND hParent, int id = 0);
	void MoveWindow(int x, int y, int cx, int cy);
	void SetColor(COLORREF bkColor, COLORREF textColor, COLORREF borderColor, COLORREF focusBorderColor);
	void SetPrice(const char* pszPrice);
	void SetActionText(const char* pszText);

	char* GetPrice(){ return m_strPrice; }
	char* GetAction(){ return m_strAction; }
protected:
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	void OnPaint();
	void OnMouseMove(WPARAM wParam, LPARAM lParam);
	void OnMouseLeave();
	void OnLButtonDown(WPARAM wParam, LPARAM lParam);
	void OnLButtonUp();
	void OnKillFocus();
	void OnSetFocus();
private:
	bool      m_bMouseTrack;
	bool      m_bMouseClicked;
	bool      m_bHasFocus;
	char      m_strPrice[50];
	char      m_strAction[20];//¿ª²Ö£¬Æ½²Ö£¬Ëø²ÖµÈ
private:
	COLORREF  m_bkColor;
	COLORREF  m_borderColor;
	COLORREF  m_focusBorderColor;
	COLORREF  m_textColor;
};

#endif