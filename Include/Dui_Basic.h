#ifndef _DUI_BASIC_
#define _DUI_BASIC_

//窗口类
class TDuiControl;
class _STAR_DUI_CLASS_ TDuiWindow
{
public:
	TDuiWindow();
	virtual ~TDuiWindow();
	HWND GetHwnd()  { return m_Hwnd; }

	void Show(HWND parent, DWORD style, DWORD exstyle, HBRUSH bgbrush, int x, int y, int cx, int cy);	//背景画刷在外部创建，窗口类不管理
	void ShowModal(DWORD style, DWORD exstyle, HBRUSH bgbrush, int x, int y, int cx, int cy);
	void Hide();

	void Redraw(TDuiControl* control);																	//同步重绘	控件重绘，传NULL重绘所有
	void RedrawLater(TDuiControl* control);																//异步重绘	控件重绘

	void GetRect(RECT& r);
	void SetPos(int x, int y, int cx, int cy, UINT flag);
	bool TrackMouseLeave(TDuiControl* control);

	bool HasFocus(TDuiControl* control);
	bool SetFocus(TDuiControl* control);

	bool AddControl(TDuiControl* control);
	bool HasControl(TDuiControl* control);
	TDuiControl* FindControl(POINTS& pts);	

	bool HasCapture(TDuiControl* control);
	bool SetCapture(TDuiControl* control);
	void ReleaseCapture();
	void Close();
	void Destroy();
	static void Quit();																					//搭配ShowModal使用
	static void GetCurrScreenRect(RECT& r);

	bool PostUserMsg(UINT Msg, WPARAM wParam, LPARAM lParam);

	void SetCurrCursor(HCURSOR cursor);
	
protected:
	virtual void OnShow(WPARAM wParam, LPARAM lParam) {}
	virtual void OnSize(WPARAM wParam, LPARAM lParam) {}
	virtual LRESULT OnClose();
	virtual void OnDestroy() {}
	virtual void OnNcDestroy() {}
	virtual void OnRButtonDownOut(WPARAM wParam, LPARAM lParam) {}
	virtual void OnCaptureChanged(WPARAM wParam, LPARAM lParam) {}
	virtual void OnKillFocus(WPARAM wParam, LPARAM lParam) {}
	virtual LRESULT OnMessage(UINT message, WPARAM wParam, LPARAM lParam);
	
private:
	static LRESULT CALLBACK DuiWindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

	void OnDraw(WPARAM wParam, LPARAM lParam);
	void DrawControl(TDuiControl* control, HDC hdc, HDC mdc, bool follow);					//all表示重绘所有区域, follow表示控件跟随窗口刷新
	void OnMouseMove(WPARAM wParam, LPARAM lParam);
	void OnSetCursor(WPARAM wParam, LPARAM lParam);
	void OnLButtonDown(WPARAM wParam, LPARAM lParam);
	void OnRButtonDown(WPARAM wParam, LPARAM lParam);
	void OnLButtonUp(WPARAM wParam, LPARAM lParam);
	void OnLButtonDbClk(WPARAM wParam, LPARAM lParam);
	void OnMouseLeave();
	void OnKeyDown(WPARAM wParam, LPARAM lParam);
	void OnSysKeyDown(WPARAM wParam, LPARAM lParam) {}
	void OnMouseWheel(WPARAM wParam, LPARAM lParam);
	void OnWindowShow(WPARAM wParam, LPARAM lParam);
	void OnWindowSize(WPARAM wParam, LPARAM lParam);

	void TriggerMouseLeave(POINTS* pts);													//pts不为空，判断pts，否则触发所有

	HDC CreateMemDC(HDC hdc);
	void ReleaseMemDC();

private:
	int m_Cx, m_Cy;																			//在父窗口或屏幕位置，及宽度和高度
	HWND m_Hwnd;																			//当前窗口句柄
	HBRUSH m_BgBrush;																		//窗口背景画刷
	bool m_MouseLeaveTrack;																	//鼠标离开跟踪
	TDuiControl* m_FocusControl;															//焦点控件，接受键盘消息，可以为空
	TDuiControl* m_CaptureControl;															//鼠标焦点控件

	typedef std::set<TDuiControl*> TDuiControlSetType;
	TDuiControlSetType	m_Controls;															//窗口包含所有控件，重绘窗口时使用，此表暂时只会增加不会删除，控件过多时遍历此表可能效率不高，随后考虑优化，每个控件的更新区域加锁，防止脏数据
	TDuiControlSetType	m_LeaveControls;													//关注MouseLeave的控件

	HDC m_Mdc;																				//缓存内存DC
	HBITMAP m_Bmp;																			//缓存内存BMP
	int m_Bmp_Cx;																			//缓存BMP宽度
	int m_Bmp_Cy;																			//缓存BMP高度

	HCURSOR m_Cursor;																		//窗口当前鼠标指针
};

//控件类
class _STAR_DUI_CLASS_ TDuiControl
{
public:
	static void Draw_Arraw(HDC mdc, RECT& r, TDuiDirection dt);									//画方向箭头

public:
	TDuiControl(TDuiWindow& window);
	virtual ~TDuiControl();

	TDuiWindow* GetWindow() { return &m_Window; }
	bool IsVisible() { return m_Visible; };													//获取显示位置
	bool SetVisible(bool visible);															//设置显示状态								//触发OnVisible回调
	void GetRect(RECT& r);																	//获取在父窗口中的坐标位置
	void MoveAndSize(const POINT* p, const SIZE* s);										//设置控件位置和大小						//触发OnMoveAndSize回调

	bool HasFocus();
	bool SetFocus();
	
	void TrackMouseLeave();
	void TriggerMouseLeave();

	bool GetUpdateRect(RECT& r);															//获取更新区域(并重置为空)，返回值表示是否有效区域
	bool SetUpdateRect(const RECT& r, bool& exists);										//设置更新区域，返回值表示是否设置成功，更新前是否存在更新区域
	void Redraw(const RECT* r);																//同步重绘，r为NULL重绘整个控件
	void RedrawLater(const RECT* r);														//异步重绘，r为NULL重绘整个控件

	void ScreenToClient(POINT& p);
	void ClientToScreen(POINT& p);

	bool HasMouse();
	bool CatchMouse();
	void ReleaseMouse();

	void SetCurrCursor(HCURSOR cursor);

public:
	virtual void OnVisible() {	}
	virtual void OnMoveAndSize(bool move, bool size) {  }
	virtual bool OnDraw(HDC hdc, HDC mdc, RECT& cr, RECT& ur, bool follow) { return false; }				//返回true，系统将mdc的ur区域绘制到hdc，follow表示跟随窗口刷新
	virtual void OnMouseMove(WORD btn, POINTS& pts) { }
	virtual void OnLButtonDown(WORD btn, POINTS& pts) { }
	virtual void OnRButtonDown(WORD btn, POINTS& pts) { }
	virtual void OnLButtonUp(WORD btn, POINTS& pts) { }
	virtual void OnLButtonDbClk(WORD btn, POINTS& pts) { }
	virtual void OnMouseLeave() { }
	virtual void OnKeyDown(WPARAM vk, LPARAM lParam) { }
	virtual void OnMouseWheel(short rot, WORD btn, POINTS& pts) { }

private:
	TDuiControl(const TDuiControl& r);
	TDuiControl& operator=(const TDuiControl& r);

private:
	int m_X, m_Y, m_Cx, m_Cy;																//控件在窗口中位置和大小
	TDuiWindow& m_Window;																	//父窗口
	bool m_Visible;																			//是否显示
	bool m_MouseLeaveTrack;																	//鼠标离开跟踪

	RECT m_UpdateRect;																		//待重绘区域
	CRITICAL_SECTION m_UpdateRectCrit;														//待重绘区域锁，防止脏数据
};


#endif