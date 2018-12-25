#ifndef TLOCKCHECK_H
#define TLOCKCHECK_H

class TLockCheck :public TIxFrm
{
public:
	TLockCheck();
	~TLockCheck();

	void MoveWindow(const int& x, const int& y, const int& cx, const int& cy);
	void SetValue(bool value) { m_bValue = value; InvalidateRect(m_Hwnd, 0, true); }
	bool GetValue(){ return m_bValue; }
	void EnableLockCheck(bool bEnable){ EnableWindow(m_Hwnd, bEnable); }
	bool Create(HWND hParent, int LockId, int unLockId);
protected:
	virtual LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
	void OnPaint();
	void OnLButtonDown(WPARAM wParam, LPARAM lParam);
private:
	bool	m_bValue;//锁上是true,没锁上是flase;
	HICON	m_hLock;
	HICON	m_hUnLock;

	COLORREF m_frameclr;
	COLORREF m_hoverclr;
};
#endif