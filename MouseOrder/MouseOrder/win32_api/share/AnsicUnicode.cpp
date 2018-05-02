#include "..\PreInclude.h"
extern ILanguageApi	*g_pLanguage;
int DrawTextUnicode(HDC hdc, const char* lpchText, LPRECT lprc, UINT format)
{
	wstring wstr = LoadRC::ansi_to_unicode(lpchText);//简体中文
	return DrawText(hdc, wstr.c_str(), wstr.size(), lprc, format);
}

char g_sAnsic[1024] = { 0 };
string GetWindowTextAnsic(HWND hWnd)
{
	GetWindowTextA(hWnd, g_sAnsic, 1024);
	return g_sAnsic;
}

BOOL SetWindowTextAnsic(HWND hWnd, const char* lpString)
{
	return SetWindowText(hWnd, LoadRC::ansi_to_unicode(lpString).c_str());
}

LRESULT ComBoxStringMsgOperAnsic(HWND hWnd, UINT Msg, int nStart, const char* lpText)
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
const wchar_t* LoadResWchar_t(int strID)
{
	if (g_pLanguage)
		return g_pLanguage->LangText(strID);
	else
		return L"";
}