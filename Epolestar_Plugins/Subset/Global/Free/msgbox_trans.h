#pragma once 

#include "load_rc.h"

//MessageBox按钮上的文字替换, 目前只替换系统对话框，稍作扩展即可切换窗口上所有控件的语言
//MessageBox可能用到的按钮ID:[IDOK,IDCANCEL,IDABORT,IDRETRY,IDIGNORE,IDYES,IDNO,IDCLOSE,IDHELP,IDTRYAGAIN,IDCONTINUE,IDTIMEOUT]
//程序资源里需要加入这些ID对应的字符串
class MessageBoxManager
{
public:
	static void reg_hook()
	{
		if (hook != 0)
			unreg_hook();
		hook = SetWindowsHookEx(WH_CALLWNDPROCRET, &MessageBoxManager::messagebox_hook_proc, 0, GetCurrentThreadId());
	}
	static void unreg_hook()
	{
		if (hook == 0)
			return;
		UnhookWindowsHookEx(hook);
		hook = 0;
	}
private:
	static HHOOK hook;
	static int btn_num;

	//判断是对话框资源，且该对话框资源不在本进程资源之中
	static bool __stdcall no_self_dlg(HWND hwnd)
	{
		char class_name[21];
		GetClassName(hwnd, class_name, sizeof(class_name));
		if (0 != strcmp(class_name, "#32770"))
			return false;

		HINSTANCE instance = (HINSTANCE)::GetWindowLong(hwnd, GWL_HINSTANCE);
		return  GetModuleHandle(nullptr) != instance;
	}
	static LRESULT __stdcall messagebox_hook_proc(int ncode, WPARAM wparam, LPARAM lparam)
	{
		if (ncode < 0)
			return CallNextHookEx(hook, ncode, wparam, lparam);
	
		CWPRETSTRUCT msg = *(CWPRETSTRUCT*)lparam;
		if (msg.message == WM_INITDIALOG && no_self_dlg(msg.hwnd))
		{
			btn_num = 0;
			EnumChildWindows(msg.hwnd, &MessageBoxManager::messagebox_enum_proc, 0);
			if (btn_num == 1)
			{
				HWND hbtn = GetDlgItem(msg.hwnd, IDCANCEL);
				if (hbtn != 0)
					SetWindowText(hbtn, LoadRC::load_str(IDOK).c_str());
			}
		}
	
		return CallNextHookEx(hook, ncode, wparam, lparam);
	}
	static BOOL __stdcall messagebox_enum_proc(HWND hwnd, LPARAM lparam)
	{
		char class_name[21];
		GetClassName(hwnd, class_name, sizeof(class_name));
		if (strcmp(class_name, "Button") == 0)
		{
			int btn_id = GetDlgCtrlID(hwnd);
			string btn_txt = LoadRC::load_str(btn_id);
			if (!btn_txt.empty())
				SetWindowText(hwnd, btn_txt.c_str());
			btn_num++;
		}

		return TRUE;
	}
};


HHOOK MessageBoxManager::hook = 0; 
int MessageBoxManager::btn_num = 0;