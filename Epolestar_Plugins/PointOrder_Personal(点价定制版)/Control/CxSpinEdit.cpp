#include "BaseInclude.h"


void CxSpinEdit::Create(HWND hParent,UINT editStyle,  UINT editID, UINT spinID, bool IsBubby)
{
	m_edit.Create(hParent, CxEdit::intStyle, editStyle, editID);

	m_spin.Create(hParent, spinID);

	if (IsBubby)
		m_spin.SetBuddy(m_edit.GetHwnd());

	m_spin.SetRange(-1400, 1000000);
}
void CxSpinEdit::SetRange(int lower, int upper)
{
	m_spin.SetRange(lower, upper);
}
void CxSpinEdit::SetLimitText(int length)
{
	m_edit.SetLimitText(length);
}
void CxSpinEdit::SetFont(HFONT font)
{
	m_edit.SetFont(font);
}
void CxSpinEdit::MoveWindow(int x, int y, int cx, int cy)
{
	int spinWidth = 15;
	m_edit.MoveWindow(x, y, cx - spinWidth -1, cy);
	m_spin.MoveWindow(x + cx - spinWidth, y, spinWidth, cy);
}
void CxSpinEdit::SetWindowNumber(int number)
{
	m_edit.SetWindowNumber(number);
}
void CxSpinEdit::SetWindowText(char* pszText)
{
	m_edit.SetText(pszText);
}
int CxSpinEdit::GetWindowNumber()
{
	return m_edit.GetWindowNumber();
}
void CxSpinEdit::GetText(char* pszText, int iSize)
{
	m_edit.GetText(pszText, iSize);
}
void CxSpinEdit::Enable(bool bEnable)
{
	EnableWindow(m_edit.GetHwnd(), bEnable);
	EnableWindow(m_spin.GetHwnd(), bEnable);
}