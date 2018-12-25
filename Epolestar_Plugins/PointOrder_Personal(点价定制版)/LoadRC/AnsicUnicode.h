#pragma once
#include "windows.h"
#include <string>
#include "load_rc.h"
using namespace std;
void DoubleToChar(int iPrecision, double Price, char* Dest);
void CharToDouble(int iPrecision, const char* Price, char* Dest);

int DrawTextUnicode(HDC hdc, string lpchText, LPRECT lprc, UINT format);

BOOL GetWindowTextAnsic(HWND hWnd, string &lpString);//数字 字母 --》计算方便 

BOOL GetWindowTextUnicode(HWND hWnd, wstring &lpString);

BOOL SetWindowTextAnsic(HWND hWnd, string lpString);

LRESULT ComBoxStringMsgOperAnsic(HWND hWnd, UINT Msg, int nStart, string lpText);

wstring ANSIToUnicode(const string& str);

void SetWindowInt(HWND hWnd, int nInt);

void SetWindowDouble(HWND hWnd, int nPrecision, double dValue);

//封装GetWindowText
void GetWndText(HWND hCtrlWnd, string &str);
//封装GetWindowText
void GetWndText(HWND hCtrlWnd, wstring &str);
//封装SetWindowText
void SetWndText(HWND hCtrlWnd, const wstring &str);
//封装SetWindowText
void SetWndText(HWND hCtrlWnd, const string &str);

wstring LoadResString(int strID);