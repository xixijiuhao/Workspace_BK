#ifndef _CX_SPIN_EDIT_H_
#define _CX_SPIN_EDIT_H_

class CxSpinEdit
{
public:
	void Create(HWND hParent,UINT editStyle = 0,  UINT editID = 0, UINT spinID = 0, bool IsBubby = true);
	void SetRange(int lower, int upper);
	void SetLimitText(int length);
	void SetFont(HFONT font);
	void MoveWindow(int x, int y, int cx, int cy);
	void SetWindowNumber(int number);
	void SetWindowText(char* pszText);
	int  GetWindowNumber();
	void GetText(char* pszText, int iSize);
	void Enable(bool bEnable);
private:
	CxSpin     m_spin;
	CxEdit     m_edit;
};

#endif