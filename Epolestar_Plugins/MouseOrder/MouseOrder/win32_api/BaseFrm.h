#ifndef _BASE_FRM_
#define _BASE_FRM_

//基础窗口---------------------------------------------------------------------------------------------------------

//OnMessage返回值
enum TFrmResult
{
	NOT_PROCESSED = -1,
	PROCESSED = 0
};

//窗体状态
enum TFrmState
{
	FRMSTATE_NORMAL = 0,
	FRMSTATE_HOVER,
	FRMSTATE_CLICKED
};


class TBaseFrm
{
public:
	inline HWND GetHwnd() const { return m_Hwnd; }
	void Redraw();

protected:
	void CreateFrm(HWND parent, DWORD style, DWORD exstyle = 0);
	HWND m_Hwnd;

	virtual LRESULT OnMessage(UINT message, WPARAM wParam, LPARAM lParam) { return NOT_PROCESSED;  }
	virtual void OnPaint(const HDC hdc, const RECT& cr, const RECT& ur) {}
	virtual void OnMouseMove(WPARAM wParam, LPARAM lParam) {}
	virtual void OnLButtonDown(WPARAM wParam, LPARAM lParam) {}
	virtual void OnLButtonUp(WPARAM wParam, LPARAM lParam) {}
	virtual void OnLButtonDbClk(WPARAM wParam, LPARAM lParam) {}
	virtual void OnMouseLeave(WPARAM wParam, LPARAM lParam) {}
	virtual void OnKeyDown(WPARAM wParam, LPARAM lParam) {}
	virtual void OnMouseWheel(WPARAM wParam, LPARAM lParam) {}
	virtual void OnKillFocus(WPARAM wParam, LPARAM lParam) {}
	virtual void OnCaptureChanged(WPARAM wParam, LPARAM lParam) {}
	virtual void OnActive(WPARAM wParam, LPARAM lParam) {}
	virtual void OnShowWindow(WPARAM wParam, LPARAM lParam) {}
	virtual void OnSize(WPARAM wParam, LPARAM lParam) {}
	virtual void OnSetCursor(WPARAM wParam, LPARAM lParam);
	virtual void OnDestroy() {}
	virtual void OnNcDestroy() {}
	
private:
	static LRESULT CALLBACK BaseFrmProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	bool m_MouseLeaveTrack;

};

#endif