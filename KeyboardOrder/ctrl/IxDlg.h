#pragma once

class IxDlg
{
public:
	IxDlg();
	~IxDlg();
public:
	inline HWND GetHwnd() const { return m_hDlg; }
	inline HWND GetParent() const { return m_hParent; }
protected:
	HWND m_hDlg;
	HWND m_hParent;
	//�Լ��������Ϣ����1 �����������Ϣ����false ��
	virtual INT_PTR DlgProc(UINT message, WPARAM wParam, LPARAM lParam) = 0;
	//������ģ̬�Ի���
	HWND CreateDlg(HINSTANCE hInstance, int idDlg, HWND hWndParent);
	//����ģ̬�Ի���
	INT_PTR DlgBox(HINSTANCE hInstance, int idDlg, HWND hWndParent);
private:
	//�Ի��������
	static INT_PTR CALLBACK IxDlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
};

