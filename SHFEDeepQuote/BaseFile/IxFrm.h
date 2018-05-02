#ifndef _IX_FRM_
#define _IX_FRM_

//#define WIN32_LEAN_AND_MEAN
//#include <windows.h>

//��������---------------------------------------------------------------------------------------------------------
class TIxFrm
{
public:
	static const LRESULT PROCESSED = 0;
	static const LRESULT NOT_PROCESSED = -1;
	//��ȡ�Ӵ����ڸ������е����λ��
	static bool GetInParentPoint(HWND hwnd, POINT& point);

public:
	inline HWND GetHwnd() const { return m_Hwnd; }
	HWND GetParent();
	bool GetInParentPoint(POINT& point);
protected:
	//�Լ��������Ϣ���� PROCESSED �����������Ϣ���� NOT_PROCESSED ��
	virtual LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam) = 0;
	void CreateFrm(LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle = 0);
	HWND m_Hwnd;
private:
	static LRESULT CALLBACK IxFrmProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

};

//����״̬
enum TFrmState
{
	FRMSTATE_NORMAL = 0,
	FRMSTATE_HOVER,
	FRMSTATE_CLICKED
};

#endif