#pragma once
#include "windows.h"
#include <string>
#include "load_rc.h"
using namespace std;
void DoubleToChar(int iPrecision, double Price, char* Dest);
void CharToDouble(int iPrecision, const char* Price, char* Dest);

int DrawTextUnicode(HDC hdc, string lpchText, LPRECT lprc, UINT format);

BOOL GetWindowTextAnsic(HWND hWnd, string &lpString);//���� ��ĸ --�����㷽�� 

BOOL GetWindowTextUnicode(HWND hWnd, wstring &lpString);

BOOL SetWindowTextAnsic(HWND hWnd, string lpString);

LRESULT ComBoxStringMsgOperAnsic(HWND hWnd, UINT Msg, int nStart, string lpText);

wstring ANSIToUnicode(const string& str);

void SetWindowInt(HWND hWnd, int nInt);

void SetWindowDouble(HWND hWnd, int nPrecision, double dValue);

//��װGetWindowText
void GetWndText(HWND hCtrlWnd, string &str);
//��װGetWindowText
void GetWndText(HWND hCtrlWnd, wstring &str);
//��װSetWindowText
void SetWndText(HWND hCtrlWnd, const wstring &str);
//��װSetWindowText
void SetWndText(HWND hCtrlWnd, const string &str);

wstring LoadResString(int strID);

const wchar_t* LoadResWchar_t(int strID);