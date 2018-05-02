#ifndef _DUI_BASIC_
#define _DUI_BASIC_

//������
class TDuiControl;
class _STAR_DUI_CLASS_ TDuiWindow
{
public:
	TDuiWindow();
	virtual ~TDuiWindow();
	HWND GetHwnd()  { return m_Hwnd; }

	void Show(HWND parent, DWORD style, DWORD exstyle, HBRUSH bgbrush, int x, int y, int cx, int cy);	//������ˢ���ⲿ�����������಻����
	void ShowModal(DWORD style, DWORD exstyle, HBRUSH bgbrush, int x, int y, int cx, int cy);
	void Hide();

	void Redraw(TDuiControl* control);																	//ͬ���ػ�	�ؼ��ػ棬��NULL�ػ�����
	void RedrawLater(TDuiControl* control);																//�첽�ػ�	�ؼ��ػ�

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
	static void Quit();																					//����ShowModalʹ��
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
	void DrawControl(TDuiControl* control, HDC hdc, HDC mdc, bool follow);					//all��ʾ�ػ���������, follow��ʾ�ؼ����洰��ˢ��
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

	void TriggerMouseLeave(POINTS* pts);													//pts��Ϊ�գ��ж�pts�����򴥷�����

	HDC CreateMemDC(HDC hdc);
	void ReleaseMemDC();

private:
	int m_Cx, m_Cy;																			//�ڸ����ڻ���Ļλ�ã�����Ⱥ͸߶�
	HWND m_Hwnd;																			//��ǰ���ھ��
	HBRUSH m_BgBrush;																		//���ڱ�����ˢ
	bool m_MouseLeaveTrack;																	//����뿪����
	TDuiControl* m_FocusControl;															//����ؼ������ܼ�����Ϣ������Ϊ��
	TDuiControl* m_CaptureControl;															//��꽹��ؼ�

	typedef std::set<TDuiControl*> TDuiControlSetType;
	TDuiControlSetType	m_Controls;															//���ڰ������пؼ����ػ洰��ʱʹ�ã��˱���ʱֻ�����Ӳ���ɾ�����ؼ�����ʱ�����˱����Ч�ʲ��ߣ�������Ż���ÿ���ؼ��ĸ��������������ֹ������
	TDuiControlSetType	m_LeaveControls;													//��עMouseLeave�Ŀؼ�

	HDC m_Mdc;																				//�����ڴ�DC
	HBITMAP m_Bmp;																			//�����ڴ�BMP
	int m_Bmp_Cx;																			//����BMP���
	int m_Bmp_Cy;																			//����BMP�߶�

	HCURSOR m_Cursor;																		//���ڵ�ǰ���ָ��
};

//�ؼ���
class _STAR_DUI_CLASS_ TDuiControl
{
public:
	static void Draw_Arraw(HDC mdc, RECT& r, TDuiDirection dt);									//�������ͷ

public:
	TDuiControl(TDuiWindow& window);
	virtual ~TDuiControl();

	TDuiWindow* GetWindow() { return &m_Window; }
	bool IsVisible() { return m_Visible; };													//��ȡ��ʾλ��
	bool SetVisible(bool visible);															//������ʾ״̬								//����OnVisible�ص�
	void GetRect(RECT& r);																	//��ȡ�ڸ������е�����λ��
	void MoveAndSize(const POINT* p, const SIZE* s);										//���ÿؼ�λ�úʹ�С						//����OnMoveAndSize�ص�

	bool HasFocus();
	bool SetFocus();
	
	void TrackMouseLeave();
	void TriggerMouseLeave();

	bool GetUpdateRect(RECT& r);															//��ȡ��������(������Ϊ��)������ֵ��ʾ�Ƿ���Ч����
	bool SetUpdateRect(const RECT& r, bool& exists);										//���ø������򣬷���ֵ��ʾ�Ƿ����óɹ�������ǰ�Ƿ���ڸ�������
	void Redraw(const RECT* r);																//ͬ���ػ棬rΪNULL�ػ������ؼ�
	void RedrawLater(const RECT* r);														//�첽�ػ棬rΪNULL�ػ������ؼ�

	void ScreenToClient(POINT& p);
	void ClientToScreen(POINT& p);

	bool HasMouse();
	bool CatchMouse();
	void ReleaseMouse();

	void SetCurrCursor(HCURSOR cursor);

public:
	virtual void OnVisible() {	}
	virtual void OnMoveAndSize(bool move, bool size) {  }
	virtual bool OnDraw(HDC hdc, HDC mdc, RECT& cr, RECT& ur, bool follow) { return false; }				//����true��ϵͳ��mdc��ur������Ƶ�hdc��follow��ʾ���洰��ˢ��
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
	int m_X, m_Y, m_Cx, m_Cy;																//�ؼ��ڴ�����λ�úʹ�С
	TDuiWindow& m_Window;																	//������
	bool m_Visible;																			//�Ƿ���ʾ
	bool m_MouseLeaveTrack;																	//����뿪����

	RECT m_UpdateRect;																		//���ػ�����
	CRITICAL_SECTION m_UpdateRectCrit;														//���ػ�����������ֹ������
};


#endif