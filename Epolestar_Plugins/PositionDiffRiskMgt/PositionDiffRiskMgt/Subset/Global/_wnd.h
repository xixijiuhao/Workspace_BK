#pragma once

#include "global.h"

class Wnd
{
public:
	HWND m_hwnd;

	Wnd();	
	virtual ~Wnd();

	HWND Create(const UINT ex_style, const UINT style, const LPCSTR class_name, const HWND parent, const TRect & rect);
	void DestroyWindow();
private:		
	typedef LRESULT (Wnd::*p_wnd_proc)(HWND, UINT, WPARAM, LPARAM);
	ACCallback<WNDPROC, Wnd, p_wnd_proc> _wnd_proc;
	LRESULT self_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);
protected:
	virtual LRESULT wnd_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);
public:
	void SetText(const char * text);
	void GetText(string & text);
};

/////////////////////////////////////////////////////////////////////////////////////////////////
Wnd::Wnd() : m_hwnd(0)
{		
}

Wnd::~Wnd()
{
	DestroyWindow();
}

HWND Wnd::Create(const UINT ex_style, const UINT style, const LPCSTR class_name, const HWND parent, const TRect & rect)
{
	WNDCLASSEXA wnd_class = {
		sizeof(wnd_class),
		CS_HREDRAW | CS_VREDRAW | style,
		DefWindowProcA,
		0,
		0,
		FileFun::GetSelfHandle(),
		nullptr,
		LoadCursor(nullptr, IDC_ARROW),
		(HBRUSH)GetStockObject(WHITE_BRUSH),
		nullptr,
		class_name,
		nullptr
	};


	WNDCLASSEXA tmp_class;
	BOOL reged = GetClassInfoExA(wnd_class.hInstance, class_name, &tmp_class);
	if (!reged || tmp_class.lpfnWndProc != wnd_class.lpfnWndProc)
	{
		if (reged && !UnregisterClassA(class_name, wnd_class.hInstance))
			StrFun::trace("Warning: Window unregister failed: GetLastError returns %d\n", GetLastError());
		else if (RegisterClassExA(&wnd_class) == 0)
			StrFun::trace("Warning: Window register failed: GetLastError returns %d\n", GetLastError());	
	}

	m_hwnd = CreateWindowExA(ex_style, wnd_class.lpszClassName, nullptr, style, rect.left, rect.top, rect.Width(), rect.Height(), parent, 0, wnd_class.hInstance, nullptr);
	if (m_hwnd != 0)
	{
		_wnd_proc.Assign(this, &Wnd::self_proc);
		SetWindowLongA(m_hwnd, GWL_WNDPROC, (LONG)(WNDPROC)_wnd_proc);
	}
	else
		StrFun::trace("Warning: Window creation failed: GetLastError returns %d\n", GetLastError());
	 
	return m_hwnd;
}

void Wnd::DestroyWindow()
{
	if (::IsWindow(m_hwnd))
		::DestroyWindow(m_hwnd);
	m_hwnd = 0;
}

LRESULT Wnd::self_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	LRESULT ret = wnd_proc(hwnd, message, wparam, lparam);
	if (ret == 0)
		ret = DefWindowProc(hwnd, message, wparam, lparam);
	return ret;
}
	
LRESULT Wnd::wnd_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	return 0;
}

void Wnd::SetText(const char * text)
{
	::SetWindowTextA(m_hwnd, text);
}

void Wnd::GetText(string & text)
{
	int len = ::GetWindowTextLengthA(m_hwnd);
	if (len == 0)
		text = "";
	else
	{
		char * txt = (char*)malloc(len + 1);
		::GetWindowTextA(m_hwnd, txt, len);
		text = txt;
		delete txt;
	}
}