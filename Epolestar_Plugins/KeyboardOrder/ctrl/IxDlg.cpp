#include "PreInclude.h"

IxDlg::IxDlg() :m_hDlg(NULL)
, m_hParent(NULL)
{

}
IxDlg::~IxDlg()
{
	
}
//�Ի��������
INT_PTR CALLBACK IxDlg::IxDlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	INT_PTR ret(FALSE);
	IxDlg* dlg(NULL);
	switch (message)
	{
		case WM_INITDIALOG:
			dlg = (IxDlg*)lParam;
			dlg->m_hDlg = hwnd;
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG)dlg);
			break;
		default:
			dlg = (IxDlg*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
			break;
	}

	if (NULL != dlg&&dlg->m_hDlg != 0)
		ret = dlg->DlgProc(message, wParam, lParam);

	/*if (FALSE == ret)
		ret = DefDlgProc(hwnd, message, wParam, lParam);*/
	return ret;
}
//������ģ̬�Ի���
HWND IxDlg::CreateDlg(HINSTANCE hInstance, int idDlg, HWND hWndParent)
{
	m_hParent = hWndParent;
	return CreateDialogIndirectParam(hInstance, (LPCDLGTEMPLATEW)LoadRC::load_dlg_template(idDlg), hWndParent, IxDlgProc, (LPARAM)this);
}
//����ģ̬�Ի���
INT_PTR IxDlg::DlgBox(HINSTANCE hInstance, int idDlg, HWND hWndParent)
{
	m_hParent = hWndParent;
	return DialogBoxIndirectParam(hInstance, (LPCDLGTEMPLATEW)LoadRC::load_dlg_template(idDlg), hWndParent, IxDlgProc, (LPARAM)this);
}