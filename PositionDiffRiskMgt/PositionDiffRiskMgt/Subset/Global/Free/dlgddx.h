#pragma once

/////////////////////////////////////////////////////////////////////////////
void AFXAPI xDDX_Text(CDataExchange* pDX, int nIDC, BYTE& value);
void AFXAPI xDDX_Text(CDataExchange* pDX, int nIDC, short& value);
void AFXAPI xDDX_Text(CDataExchange* pDX, int nIDC, int& value);
void AFXAPI xDDX_Text(CDataExchange* pDX, int nIDC, UINT& value);
void AFXAPI xDDX_Text(CDataExchange* pDX, int nIDC, long& value);
void AFXAPI xDDX_Text(CDataExchange* pDX, int nIDC, DWORD& value);
void AFXAPI xDDX_Text(CDataExchange* pDX, int nIDC, LONGLONG& value);
void AFXAPI xDDX_Text(CDataExchange* pDX, int nIDC, ULONGLONG& value);
void AFXAPI xDDX_Text(CDataExchange* pDX, int nIDC, CString& value);

//////////////////////////////////////////////////////////////////////////
void AFXAPI xDDX_Text(CDataExchange* pDX, int nIDC, float& value);
void AFXAPI xDDX_Text(CDataExchange* pDX, int nIDC, double& value);