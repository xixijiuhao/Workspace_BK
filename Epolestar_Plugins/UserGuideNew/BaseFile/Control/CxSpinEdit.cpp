#include "BaseInclude.h"


void CxSpinEdit::Create(HWND hParent, UINT editStyle, UINT editID, UINT spinID, UINT style, bool bSetBuddy)
{
	m_edit.Create(hParent, style, editStyle, editID);
	m_spin.Create(hParent, spinID);

	if (bSetBuddy)
		m_spin.SetBuddy(m_edit.GetHwnd());
	m_spin.SetRange(-10000, 1000000);

	m_edit.SetFont(g_FontData.g_FontNum15);
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
int CxSpinEdit::GetWindowNumber()
{
	return m_edit.GetWindowNumber();
}

bool CxSpinEdit::IsText()
{
	TCHAR szText[100] = { 0 };
	m_edit.GetText(szText, sizeof(szText)/sizeof(szText[0]));
	
	double dValue = _wtof(szText);
	if (dValue != 0) //˵��������
		return false;
	else if (dValue == 0)
	{
		if (szText[1] == L'\0' &&szText[0] == L'0') //˵������ľ���0
			return false;
		return true;
	}
	return false;
}