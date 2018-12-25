#pragma once
//#include <afxtempl.h>

struct SColumn{			//��¼��ÿ�е���Ϣ��
	BOOL bHide;
	int nItemWidth;		//������ǰ�Ŀ�ȡ���Ϊ���ص�ʱ����Ҫ�������Ϊ0
	SColumn(){
		bHide = FALSE;
		nItemWidth = -1;
	}
};

class THeaderCtrl : public CHeaderCtrl
{
public:
	THeaderCtrl();
public:
	CArray<SColumn, SColumn&> m_aColum;
	public:
	virtual BOOL OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult);
public:
	virtual ~THeaderCtrl();

protected:
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

	DECLARE_MESSAGE_MAP()
private:
	BOOL m_bUseHideCursor;		//����������أ���ô���ָ�벻��
};

