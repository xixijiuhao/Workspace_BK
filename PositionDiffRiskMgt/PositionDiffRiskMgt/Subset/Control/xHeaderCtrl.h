#pragma once
//#include <afxtempl.h>

struct SColumn{			//记录了每列的信息。
	BOOL bHide;
	int nItemWidth;		//在隐藏前的宽度。因为隐藏的时候需要将宽度设为0
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
	BOOL m_bUseHideCursor;		//如果该列隐藏，那么鼠标指针不变
};

