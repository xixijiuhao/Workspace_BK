#include "PreInclude.h"

extern ILanguageApi *g_pLanguage;

void DoubleToChar(int iPrecision, double Price, char* Dest)
{
	char format[20] = { 0 };
	sprintf_s(format, "%%0.%df", iPrecision);
	sprintf_s(Dest, sizeof(format), format, Price);
}
void CharToDouble(int iPrecision, const  char* Price, char* Dest)
{
	char format[20] = { 0 };
	sprintf_s(format, "%%s.%df", iPrecision);
	sprintf_s(Dest, sizeof(format), format, Price);
}
int DrawTextUnicode(HDC hdc, string lpchText, LPRECT lprc, UINT format)
{
	wstring wstr = ANSIToUnicode(lpchText);//简体中文
	return DrawText(hdc, wstr.c_str(), wstr.size(), lprc, format);
}

char g_sUnicode[1024] = { 0 };
BOOL GetWindowTextAnsic(HWND hWnd, string &lpString)
{
	GetWindowTextA(hWnd, g_sUnicode, 1024);
	lpString = g_sUnicode;
	return TRUE;
}

wchar_t g_sAnsic[1024] = { 0 };
BOOL GetWindowTextUnicode(HWND hWnd, wstring &lpString)
{
	GetWindowText(hWnd, g_sAnsic, 1024);
	lpString = g_sAnsic;
	return TRUE;
}
BOOL SetWindowTextAnsic(HWND hWnd, string lpString)
{
	wstring wstr = ANSIToUnicode(lpString);//简体中文
	return SetWindowText(hWnd, wstr.c_str());
}

LRESULT ComBoxStringMsgOperAnsic(HWND hWnd, UINT Msg, int nStart, string lpText)
{
	wstring wstr = ANSIToUnicode(lpText);//简体中文
	return SendMessage(hWnd, Msg, (WPARAM)nStart, (LPARAM)(LPWSTR)wstr.c_str());
}

wstring ANSIToUnicode(const string& str)//简体Ansic转Unicode
{
	UINT nPage = 936;
	size_t  len = 0;
	len = str.length();
	int  unicodeLen = ::MultiByteToWideChar(nPage, 0, str.c_str(), -1, nullptr, 0);

	wchar_t *  pUnicode;
	pUnicode = new  wchar_t[unicodeLen + 1];
	memset(pUnicode, 0, (unicodeLen + 1)*sizeof(wchar_t));

	::MultiByteToWideChar(nPage, 0, str.c_str(), -1, (LPWSTR)pUnicode, unicodeLen);
	wstring  rt = (wchar_t*)pUnicode;
	delete[]  pUnicode;

	return  rt;
}

string UnicodeToANSI(const wstring& str)
{
	UINT nPage = 936;
	size_t  len = 0;
	len = str.length();
	int  unicodeLen = ::WideCharToMultiByte(nPage, 0, str.c_str(), -1, nullptr, 0, 0, 0);

	char*  ansi;
	ansi = new  char[unicodeLen + 1];
	memset(ansi, 0, (unicodeLen + 1)*sizeof(char));

	::WideCharToMultiByte(nPage, 0, str.c_str(), -1, (LPSTR)ansi, unicodeLen ,0, 0);
	string rt = (char*)ansi;
	delete[]  ansi;

	return  rt;
}

void SetWindowInt(HWND hWnd, int nInt)
{
	char cQty[20] = {0};
	sprintf_s(cQty, "%d", nInt);
	SetWindowTextAnsic(hWnd, cQty);
}

void SetWindowDouble(HWND hWnd, int nPrecision, double dValue)
{
	char sText[20] = { 0 };
	char format[20] = { 0 };
	sprintf_s(format, "%%0.%df", nPrecision);
	sprintf_s(sText, format, dValue);
	SetWindowTextAnsic(hWnd, sText);
}

//封装GetWindowText
void GetWndText(HWND hCtrlWnd, string &str)
{
	//修改
	int nLen = ::GetWindowTextLengthA(hCtrlWnd);
	str.resize(nLen);
	char *buffer = new char[nLen + 1];
	buffer[0] = '\0';
	::GetWindowTextA(hCtrlWnd, buffer, nLen + 1);
	str = buffer;
	delete[]buffer;
}
//封装GetWindowText
void GetWndText(HWND hCtrlWnd, wstring &str)
{
	int nLen = ::GetWindowTextLength(hCtrlWnd);
	str.resize(nLen);
	wchar_t *buffer = new wchar_t[nLen + 1];
	memset(buffer, 0, (nLen + 1)*sizeof(wchar_t));
	//buffer[0] = '\0';
	::GetWindowText(hCtrlWnd, buffer, nLen + 1);
	str = (wchar_t *)buffer;
	delete[]buffer;
}
//封装SetWindowText
void SetWndText(HWND hCtrlWnd, const wstring &str)
{
	::SetWindowText(hCtrlWnd, str.c_str());
}
//封装SetWindowText
void SetWndText(HWND hCtrlWnd, const string &str)
{
	::SetWindowTextA(hCtrlWnd, str.c_str());
}

//加载字符串
wstring LoadResString(int strID)
{
	return   LoadRC::w_load_str(strID);
}

//加载字符串
const wchar_t* LoadResWchar_t(int strID)
{
	if (g_pLanguage)
		return g_pLanguage->LangText(strID);
	else
		return L"";
}