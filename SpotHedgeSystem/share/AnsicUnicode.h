#pragma once

int DrawTextUnicode(HDC hdc, const char* lpchText, LPRECT lprc, UINT format);

string GetWindowTextAnsic(HWND hWnd);//Êý×Ö ×ÖÄ¸ --¡·¼ÆËã·½±ã 

BOOL SetWindowTextAnsic(HWND hWnd, const char* lpString);

LRESULT ComBoxStringMsgOperAnsic(HWND hWnd, UINT Msg, int nStart, const char*lpText);

void SetWindowInt(HWND hWnd, int nInt);

void SetWindowDouble(HWND hWnd, int nPrecision, double dValue);

const wchar_t* LoadResWchar_t(int strID);