#include "AnsicUnicode.h"
#include "load_rc.h"
#include "LanguageApi_API.h"

extern ILanguageApi* g_pLanguage;
int DrawTextUnicode(HDC hdc, std::string lpchText, LPRECT lprc, UINT format)
{
    std::wstring wstr = LoadRC::ansi_to_unicode(lpchText);//简体中文
	return DrawText(hdc, wstr.c_str(), wstr.size(), lprc, format);
}

char g_sUnicode[1024] = { 0 };
BOOL GetWindowTextAnsic(HWND hWnd, std::string &lpString)
{
	GetWindowTextA(hWnd, g_sUnicode, 1024);
	lpString = g_sUnicode;
	return TRUE;
}

wchar_t g_sAnsic[1024] = { 0 };
BOOL GetWindowTextUnicode(HWND hWnd, std::wstring &lpString)
{
	GetWindowText(hWnd, g_sAnsic, 1024);
	lpString = g_sAnsic;
	return TRUE;
}
BOOL SetWindowTextAnsic(HWND hWnd, std::string& lpString)
{
	return SetWindowText(hWnd, LoadRC::ansi_to_unicode(lpString).c_str());
}

LRESULT ComBoxStringMsgOperAnsic(HWND hWnd, UINT Msg, int nStart, std::string& lpText)
{
	return SendMessage(hWnd, Msg, (WPARAM)nStart, (LPARAM)(LPWSTR)LoadRC::ansi_to_unicode(lpText).c_str());
}

void SetWindowInt(HWND hWnd, int nInt)
{
	wchar_t cQty[20] = {0};
	swprintf_s(cQty, L"%d", nInt);
	SetWindowTextW(hWnd, cQty);
}

void SetWindowDouble(HWND hWnd, int nPrecision, double dValue)
{
	wchar_t sText[20] = { 0 };
	wchar_t format[20] = { 0 };
	swprintf_s(format, L"%%0.%df", nPrecision);
	swprintf_s(sText, format, dValue);
	SetWindowTextW(hWnd, sText);
}

//加载字符串
std::wstring LoadResString(int strID)
{
	if (g_pLanguage)
		return   g_pLanguage->LangText(strID);
	else
		return L"";
}