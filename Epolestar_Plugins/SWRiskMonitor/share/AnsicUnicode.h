#pragma once
#include <Windows.h>
#include <string>

int DrawTextUnicode(HDC hdc, std::string lpchText, LPRECT lprc, UINT format);

BOOL GetWindowTextAnsic(HWND hWnd, std::string &lpString);//Êý×Ö ×ÖÄ¸ --¡·¼ÆËã·½±ã 

BOOL GetWindowTextUnicode(HWND hWnd, std::wstring &lpString);

BOOL SetWindowTextAnsic(HWND hWnd, std::string &lpString);

LRESULT ComBoxStringMsgOperAnsic(HWND hWnd, UINT Msg, int nStart, std::string &lpText);

void SetWindowInt(HWND hWnd, int nInt);

void SetWindowDouble(HWND hWnd, int nPrecision, double dValue);

std::wstring LoadResString(int strID);