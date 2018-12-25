//根据VC同名文件修改而来

#include "stdafx.h"
#include "dlgddx.h"

AFX_STATIC void AFX_CDECL x_Afx_DDX_TextWithFormat(CDataExchange* pDX, int nIDC,
												  LPCTSTR lpszFormat, UINT nIDPrompt, ...)
												  // only supports windows output formats - no floating point
{
	va_list pData;
	va_start(pData, nIDPrompt);

	HWND hWndCtrl = pDX->PrepareEditCtrl(nIDC);
	ASSERT( hWndCtrl != NULL );

	const int SZT_SIZE = 64;
	TCHAR szT[SZT_SIZE];
	if (pDX->m_bSaveAndValidate)
	{
		void* pResult;

		pResult = va_arg( pData, void* );
		// the following works for %d, %u, %ld, %lu
		::GetWindowText(hWndCtrl, szT, _countof(szT));
		if (_sntscanf_s(szT, _countof(szT), lpszFormat, pResult) != 1)
		{
			//lmx delete
			//AfxMessageBox(nIDPrompt);
			//pDX->Fail();        // throws exception

			//lmx add
			AfxSetWindowText(hWndCtrl, _T(""));
			if (AFX_IDP_PARSE_BYTE==nIDPrompt)
				*((BYTE*)pResult)=0;
			else if (AFX_IDP_PARSE_INT==nIDPrompt)
			{
				if (strcmp(lpszFormat, _T("%d"))==0)
					*((int*)pResult)=0;
				else if (strcmp(lpszFormat, _T("%hd"))==0)
					*((short*)pResult)=0;
				else if (strcmp(lpszFormat, _T("%ld"))==0)
					*((long*)pResult)=0;
				else if (strcmp(lpszFormat, _T("%I64d"))==0)
					*((LONGLONG*)pResult)=0;
				else 
					*((int*)pResult)=0; 				
			}
			else 
			{
				if (strcmp(lpszFormat, _T("%u"))==0)
					*((UINT*)pResult)=0;
				else if (strcmp(lpszFormat, _T("%lu"))==0)
					*((DWORD*)pResult)=0;
				else if (strcmp(lpszFormat, _T("%I64u"))==0)
					*((ULONGLONG*)pResult)=0;
				else 
					*((UINT*)pResult)=0; 		
			}
		}
	}
	else
	{

		ATL_CRT_ERRORCHECK_SPRINTF(_vsntprintf_s(szT, _countof(szT), _countof(szT) - 1, lpszFormat, pData));
		// does not support floating point numbers - see dlgfloat.cpp
		AfxSetWindowText(hWndCtrl, szT);
	}

	va_end(pData);
}

/////////////////////////////////////////////////////////////////////////////
// Simple formatting to text item

void AFXAPI xDDX_Text(CDataExchange* pDX, int nIDC, BYTE& value)
{
	int n = (int)value;
	if (pDX->m_bSaveAndValidate)
	{
		x_Afx_DDX_TextWithFormat(pDX, nIDC, _T("%u"), AFX_IDP_PARSE_BYTE, &n);
		if (n > 255)
		{
			AfxMessageBox(AFX_IDP_PARSE_BYTE);
			pDX->Fail();        // throws exception
		}
		value = (BYTE)n;
	}
	else
		x_Afx_DDX_TextWithFormat(pDX, nIDC, _T("%u"), AFX_IDP_PARSE_BYTE, n);
}

void AFXAPI xDDX_Text(CDataExchange* pDX, int nIDC, short& value)
{
	if (pDX->m_bSaveAndValidate)
		x_Afx_DDX_TextWithFormat(pDX, nIDC, _T("%hd"), AFX_IDP_PARSE_INT, &value);
	else
		x_Afx_DDX_TextWithFormat(pDX, nIDC, _T("%hd"), AFX_IDP_PARSE_INT, value);
}

void AFXAPI xDDX_Text(CDataExchange* pDX, int nIDC, int& value)
{
	if (pDX->m_bSaveAndValidate)
		x_Afx_DDX_TextWithFormat(pDX, nIDC, _T("%d"), AFX_IDP_PARSE_INT, &value);
	else
		x_Afx_DDX_TextWithFormat(pDX, nIDC, _T("%d"), AFX_IDP_PARSE_INT, value);
}

void AFXAPI xDDX_Text(CDataExchange* pDX, int nIDC, UINT& value)
{
	if (pDX->m_bSaveAndValidate)
		x_Afx_DDX_TextWithFormat(pDX, nIDC, _T("%u"), AFX_IDP_PARSE_UINT, &value);
	else
		x_Afx_DDX_TextWithFormat(pDX, nIDC, _T("%u"), AFX_IDP_PARSE_UINT, value);
}

void AFXAPI xDDX_Text(CDataExchange* pDX, int nIDC, long& value)
{
	if (pDX->m_bSaveAndValidate)
		x_Afx_DDX_TextWithFormat(pDX, nIDC, _T("%ld"), AFX_IDP_PARSE_INT, &value);
	else
		x_Afx_DDX_TextWithFormat(pDX, nIDC, _T("%ld"), AFX_IDP_PARSE_INT, value);
}

void AFXAPI xDDX_Text(CDataExchange* pDX, int nIDC, DWORD& value)
{
	if (pDX->m_bSaveAndValidate)
		x_Afx_DDX_TextWithFormat(pDX, nIDC, _T("%lu"), AFX_IDP_PARSE_UINT, &value);
	else
		x_Afx_DDX_TextWithFormat(pDX, nIDC, _T("%lu"), AFX_IDP_PARSE_UINT, value);
}

void AFXAPI xDDX_Text(CDataExchange* pDX, int nIDC, LONGLONG& value)
{
	if (pDX->m_bSaveAndValidate)
		x_Afx_DDX_TextWithFormat(pDX, nIDC, _T("%I64d"), AFX_IDP_PARSE_INT, &value);
	else
		x_Afx_DDX_TextWithFormat(pDX, nIDC, _T("%I64d"), AFX_IDP_PARSE_INT, value);
}

void AFXAPI xDDX_Text(CDataExchange* pDX, int nIDC, ULONGLONG& value)
{
	if (pDX->m_bSaveAndValidate)
		x_Afx_DDX_TextWithFormat(pDX, nIDC, _T("%I64u"), AFX_IDP_PARSE_UINT, &value);
	else
		x_Afx_DDX_TextWithFormat(pDX, nIDC, _T("%I64u"), AFX_IDP_PARSE_UINT, value);
}

void AFXAPI xDDX_Text(CDataExchange* pDX, int nIDC, CString& value)
{
	HWND hWndCtrl = pDX->PrepareEditCtrl(nIDC);
	if (pDX->m_bSaveAndValidate)
	{
		int nLen = ::GetWindowTextLength(hWndCtrl);
		::GetWindowText(hWndCtrl, value.GetBufferSetLength(nLen), nLen+1);
		value.ReleaseBuffer();
	}
	else
	{
		AfxSetWindowText(hWndCtrl, value);
	}
}