#pragma once

int DrawTextUnicode(HDC hdc, const char* lpchText, LPRECT lprc, UINT format);

string GetWindowTextAnsic(HWND hWnd);//���� ��ĸ --�����㷽�� 

wstring GetWindowTextUnicode(HWND hWnd);

BOOL SetWindowTextAnsic(HWND hWnd, const char* lpString);

LRESULT ComBoxStringMsgOperAnsic(HWND hWnd, UINT Msg, int nStart, const char*lpText);

void SetWindowInt(HWND hWnd, int nInt);

void SetWindowDouble(HWND hWnd, int nPrecision, double dValue);

wstring LoadResString(int strID);
