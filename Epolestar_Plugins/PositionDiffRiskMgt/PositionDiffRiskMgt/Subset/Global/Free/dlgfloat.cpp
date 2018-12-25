////根据VC同名文件修改而来

#include "stdafx.h"
#include <float.h>              // floating point precision
#include "dlgddx.h"


/////////////////////////////////////////////////////////////////////////////
// Extra data validation procs for float/double support
//  see "dlgdata.cpp" for non-floating point support
/////////////////////////////////////////////////////////////////////////////

/*
AFX_STATIC BOOL AFXAPI _AfxSimpleFloatParse(LPCTSTR lpszText, double& d)
{
	ASSERT(lpszText != NULL);
	while (*lpszText == ' ' || *lpszText == '\t')
		lpszText++;

	TCHAR chFirst = lpszText[0];
	d = _tcstod(lpszText, (LPTSTR*)&lpszText);
	if (d == 0.0 && chFirst != '0')
		return FALSE;   // could not convert
	while (*lpszText == ' ' || *lpszText == '\t')
		lpszText++;

	if (*lpszText != '\0')
		return FALSE;   // not terminated properly

	return TRUE;
}
*/

void AFXAPI xAfxTextFloatFormat(CDataExchange* pDX, int nIDC,
	void* pData, double value, int nSizeGcvt)
{
	ASSERT(pData != NULL);

	pDX->PrepareEditCtrl(nIDC);
	HWND hWndCtrl;
	pDX->m_pDlgWnd->GetDlgItem(nIDC, &hWndCtrl);
	
	const int TEXT_BUFFER_SIZE = 400;
	TCHAR szBuffer[TEXT_BUFFER_SIZE];
	if (pDX->m_bSaveAndValidate)
	{
		::GetWindowText(hWndCtrl, szBuffer, _countof(szBuffer));
		double d;
		if (_sntscanf_s(szBuffer, _countof(szBuffer), _T("%lf"), &d) != 1)
		{
			//lmx deldte
			//AfxMessageBox(AFX_IDP_PARSE_REAL);
			//pDX->Fail();            // throws exception

			//lmx add
			AfxSetWindowText(hWndCtrl, _T(""));
			d=0;
		}
		if (nSizeGcvt == FLT_DIG)
			*((float*)pData) = (float)d;
		else
			*((double*)pData) = d;
	}
	else
	{
		ATL_CRT_ERRORCHECK_SPRINTF(_sntprintf_s(szBuffer, _countof(szBuffer), _countof(szBuffer) -1, _T("%.*g"), nSizeGcvt, value));
		AfxSetWindowText(hWndCtrl, szBuffer);
	}
}

void AFXAPI xDDX_Text(CDataExchange* pDX, int nIDC, float& value)
{
	xAfxTextFloatFormat(pDX, nIDC, &value, value, FLT_DIG);
}

void AFXAPI xDDX_Text(CDataExchange* pDX, int nIDC, double& value)
{
	xAfxTextFloatFormat(pDX, nIDC, &value, value, DBL_DIG);
}