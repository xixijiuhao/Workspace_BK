#pragma once

class CCodeNameStatic :public CxSubClassWnd
{
public:
	CCodeNameStatic();
	~CCodeNameStatic();
protected:
	LRESULT CALLBACK WndProc(UINT message, WPARAM wParam, LPARAM lParam);
	void OnPaint();
private:
	//�ı�
	wstring m_strText;
	//����
	HFONT m_fontText;
	//���� DT_LEFT  DT_RIGHT DT_CENTER
	UINT m_AlignText;
public:
	void InitStatic(HWND hwnd);
	void SetTextFont(LOGFONT &font, UINT AlignText = DT_LEFT);
	void SetStaticText(const wchar_t * szText);
	void Invalidate(){ if (::IsWindow(m_hWnd)) InvalidateRect(m_hWnd, NULL, TRUE); }
	void SetWindowText(const wchar_t *sz)	{ if (::IsWindow(m_hWnd)) ::SetWindowText(m_hWnd, sz); }
};

