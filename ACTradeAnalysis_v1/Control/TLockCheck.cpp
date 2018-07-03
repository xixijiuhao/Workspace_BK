#include "BaseInclude.h"

TLockCheck::TLockCheck()
{
	bool m_bValue = true;
	
}

TLockCheck::~TLockCheck()
{

}

bool TLockCheck::Create(HWND hParent, int LockId, int unLockId)
{
	CreateFrm(_T("TLockCheckbox"), hParent, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN);
	m_hLock = LoadIcon(GetModuleHandle(L"TKeyOrder.dll"), MAKEINTRESOURCE(LockId));
	m_hUnLock = LoadIcon(GetModuleHandle(L"TKeyOrder.dll"), MAKEINTRESOURCE(unLockId));
	return true;
}


void TLockCheck::MoveWindow(const int& x, const int& y, const int& cx, const int& cy)
{
	SetWindowPos(m_Hwnd, 0, x, y, cx, cy, SWP_NOZORDER);
	InvalidateRect(m_Hwnd, 0, true);
}

LRESULT TLockCheck::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:
		OnPaint();
		break;
	case WM_LBUTTONDOWN:
		OnLButtonDown(wParam, lParam);
		break;
	default:
		return NOT_PROCESSED;
	}

	return NOT_PROCESSED;
}

void TLockCheck::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
	m_bValue = !m_bValue;
	WPARAM param; param = MAKEWORD(0, BN_CLICKED);
	PostMessage(GetParent(), WM_COMMAND, param, (LPARAM)m_Hwnd);
	InvalidateRect(m_Hwnd, 0, false);
}


void TLockCheck::OnPaint()
{
	TMemDC memdc(m_Hwnd);
	RECT rect;
	SetBkMode(memdc.GetHdc(), TRANSPARENT);
	GetClientRect(m_Hwnd, &rect);
	FillRect(memdc.GetHdc(), &rect, g_ColorRefData.g_brush_bk);
	HICON hIcon;
	if (m_bValue)
	{
		hIcon = m_hLock;
	}
	else
	{
		hIcon = m_hUnLock;
	}
	DrawIconEx(memdc.GetHdc(), rect.left, rect.top, hIcon, rect.right - rect.left, rect.bottom - rect.top, 0, 0, DI_NORMAL);
}