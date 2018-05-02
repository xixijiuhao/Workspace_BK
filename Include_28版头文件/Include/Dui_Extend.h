#ifndef _DUI_EXTEND_
#define _DUI_EXTEND_

//标签类
class _STAR_DUI_CLASS_ TDuiLabelControl : public TDuiControl
{
public:
	TDuiLabelControl(TDuiWindow& window);

public:
	void SetBackground(HBRUSH brush) { m_BackgroundBrush = brush; }
	void SetFontColor(COLORREF color) { m_FontColor = color; }
	void SetText(const wchar_t* text);
	void DisableMove();

public:
	virtual bool OnDraw(HDC hdc, HDC mdc, RECT& cr, RECT& ur, bool follow);			//返回true，系统将mdc的ur区域绘制到hdc，follow表示跟随窗口刷新
	virtual void OnMouseMove(WORD btn, POINTS& pts);
	virtual void OnLButtonDown(WORD btn, POINTS& pts);
	virtual void OnLButtonUp(WORD btn, POINTS& pts);

private:
	wchar_t m_Text[51];
	POINT m_BeginPoint;
	HBRUSH m_BackgroundBrush;
	COLORREF m_FontColor;
	bool m_MouseMove;
};

//按钮类
class _STAR_DUI_CLASS_ TDuiButtonControl : public TDuiControl
{
public:
	TDuiButtonControl(TDuiWindow& window);

public:
	void SetText(const wchar_t* text);
	void SetSpi(TDuiButtonControlSpi* spi) { m_Spi = spi; }

public:
	virtual bool OnDraw(HDC hdc, HDC mdc, RECT& cr, RECT& ur, bool follow);
	virtual void OnMouseMove(WORD btn, POINTS& pts);
	virtual void OnLButtonDown(WORD btn, POINTS& pts);
	virtual void OnLButtonUp(WORD btn, POINTS& pts);
	virtual void OnMouseLeave();

private:
	wchar_t m_Text[21];
	TDuiButtonState m_State;

	TDuiButtonControlSpi* m_Spi;
};

//请使用 TDuiPopMenuWindow !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//PopMenu控件类-------------------------------------------------------------------------------------------------
class TDuiPopMenuWindow;
class _STAR_DUI_CLASS_ TDuiPopMenuControl : public TDuiControl
{
public:
	static const int POPMENU_WIDTH = 120;
	static const int POPMENU_RIGHT_WIDTH = 60;
	static const int POPMENU_CELL_HEIGHT = 26;
	static const int POPMENU_PADDING = 2;

public:
	TDuiPopMenuControl(TDuiWindow& window);
	virtual ~TDuiPopMenuControl();

public:
	bool AddItem(TDuiPopMenuItem& item);
	bool AddSubItem(TDuiPopMenuItem& item);
	void SetSpi(TDuiPopMenuControlSpi* spi) { m_Spi = spi; }
	size_t GetItemCount(){ return m_Items.size(); }
	void ReleaseSub();
	void ResetHot();

public:
	virtual bool OnDraw(HDC hdc, HDC mdc, RECT& cr, RECT& ur, bool follow);				//返回true，系统将mdc的ur区域绘制到hdc
	virtual void OnMouseMove(WORD btn, POINTS& pts);
	virtual void OnLButtonDown(WORD btn, POINTS& pts);
	virtual void OnLButtonUp(WORD btn, POINTS& pts);
	virtual void OnRButtonDown(WORD btn, POINTS& pts);

private:
	bool HasSubItem(int i);

private:
	TDuiPopMenuItemVectorType m_Items;
	TDuiPopMenuSubItemMapType m_SubItems;
	TDuiPopMenuControlSpi* m_Spi;
	int m_Hot_Index;
	bool m_LButtonDown;

	TDuiPopMenuWindow* m_SubPopMenu;

};

//PopMenu窗口类-------------------------------------------------------------------------------------------------
class _STAR_DUI_CLASS_ TDuiPopMenuWindow : public TDuiWindow
{
public:
	TDuiPopMenuWindow();

public:
	void ShowPop(int x, int y, bool canmove, HWND parent);
	void ShowSubPop(int x, int y, TDuiPopMenuWindow* parent);
	bool AddItem(TDuiPopMenuItem& item) { return m_Control.AddItem(item); }
	bool AddSubItem(TDuiPopMenuItem& item) { return m_Control.AddSubItem(item); }
	void SetSpi(TDuiPopMenuControlSpi* spi) { m_Control.SetSpi(spi); }
	void SetWidth(int w) { m_Width = w; }
	void ReleaseParent();
	bool IsSub() { return m_IsSub; }

protected:
	virtual void OnShow(WPARAM wParam, LPARAM lParam);
	virtual void OnSize(WPARAM wParam, LPARAM lParam);
	virtual void OnRButtonDownOut(WPARAM wParam, LPARAM lParam);
	virtual void OnCaptureChanged(WPARAM wParam, LPARAM lParam);
	virtual void OnKillFocus(WPARAM wParam, LPARAM lParam);

private:
	void OnRBtnOut(POINT& p);

	TDuiPopMenuControl m_Control;
	TDuiPopMenuWindow* m_ParentPopMenu;
	int m_Width;
	bool m_CanMove;
	HWND m_Parent;
	bool m_IsSub;
};

//Combo
class _STAR_DUI_CLASS_ TDuiComboControl : public TDuiControl, public TDuiPopMenuControlSpi
{
public:
	TDuiComboControl(TDuiWindow& window);

	void RegComboSpi(TDuiComboSpi* spi) { m_ComboSpi = spi; }
	void AddItem(wchar_t* item);
	void SetSelect(int index);

public:
	virtual bool OnDraw(HDC hdc, HDC mdc, RECT& cr, RECT& ur, bool follow);
	virtual void OnLButtonUp(WORD btn, POINTS& pts);

public:
	virtual void __cdecl OnPopMenuClick(TDuiPopMenuItem* obj);

private:
	typedef std::vector<wchar_t*> TDuiComboItemVectorType;
	TDuiComboItemVectorType m_Items;	//保存外部常量指针, 无需释放
	int m_SelectIndex;
	TDuiComboSpi* m_ComboSpi;
};

//edit
class _STAR_DUI_CLASS_ TDuiEditControl : public TDuiControl
{
public:
	static const int PADDING = 4;

public:
	TDuiEditControl(TDuiWindow& window);

	void SetText(wchar_t* text);
	bool GetText(wchar_t* text);

public:
	virtual bool OnDraw(HDC hdc, HDC mdc, RECT& cr, RECT& ur, bool follow);		
	virtual void OnLButtonDown(WORD btn, POINTS& pts);
	virtual void OnKeyDown(WPARAM vk, LPARAM lParam);

private:
	wchar_t m_Text[11];
};

//ListView使用时，先添加列，再添加数据
class _STAR_DUI_CLASS_ TDuiListViewControl : public TDuiControl, public TDuiPopMenuControlSpi
{
public:
	static const size_t UNIT_DATA_SIZE = 1024;
	static const int CELL_PADDING = 2;
	static const int HEAD_HEIGHT = 28;
	static const int ROW_HEIGHT = 24;
	static const int SCROLL_WIDTH = 5;

public:
	TDuiListViewControl(TDuiWindow& window);
	virtual ~TDuiListViewControl();

public:
	void SetSpi(TDuiListViewControlSpi* spi) { m_Spi = spi; }
	void AddCol(TDuiListViewCol& col);

	bool GetInt(size_t r, size_t c, int& d);
	bool GetDouble(size_t r, size_t c, double& d);
	bool GetStr(size_t r, size_t c, char* d, size_t l);
	bool GetWStr(size_t r, size_t c, wchar_t* d, size_t l);
	bool GetCheck(size_t r, size_t c, bool& d);
	bool GetCombo(size_t r, size_t c, wchar_t* d, size_t l);
	bool GetVoid(size_t r, size_t c, void*& d);

	bool SetInt(size_t r, size_t c, int d);
	bool SetDouble(size_t r, size_t c, double d);
	bool SetStr(size_t r, size_t c, const char* d);
	bool SetWStr(size_t r, size_t c, const wchar_t* d);
	bool SetCheck(size_t r, size_t c, bool d);
	bool SetCombo(size_t r, size_t c, const wchar_t* d);
	bool SetVoid(size_t r, size_t c, void* d);
	bool SetCurrCombo(wchar_t* d);
	void Clear();
	bool Remove(size_t r);

	bool GetScrollRect(RECT& r);
	size_t GetRowCount() { return m_RowCount;  }

	bool MoveUp();			//此函数未考虑SelectRow不在显示范围的情况，后期修复bug
	bool MoveDown();

	void SetSelectIndex(int index) { m_SelectRow = index; }
	int GetSelectIndex() { return m_SelectRow; }

public:
	virtual bool OnDraw(HDC hdc, HDC mdc, RECT& cr, RECT& ur, bool follow);				//返回true，系统将mdc的ur区域绘制到hdc，follow表示跟随窗口刷新
	virtual void OnMouseMove(WORD btn, POINTS& pts);
	virtual void OnLButtonDown(WORD btn, POINTS& pts);
	virtual void OnLButtonUp(WORD btn, POINTS& pts);
	virtual void OnLButtonDbClk(WORD btn, POINTS& pts);
	virtual void OnMouseLeave();
	virtual void OnKeyDown(WPARAM vk, LPARAM lParam);
	virtual void OnMouseWheel(short rot, WORD btn, POINTS& pts);

public:
	virtual void __cdecl OnPopMenuClick(TDuiPopMenuItem* obj);

private:
	void AllocateData(size_t need);
	void GetHotRowAndCol(POINTS& pts, int& r, int& c);

private:
	TDuiListViewColVectorType m_Cols;
	TDuiListViewSizeType m_RowValueSize;
	
	size_t m_RowCount;
	char* m_Data;
	TDuiListViewSizeType m_DataSize;

	int m_SelectRow;
	int m_HotRow;
	int m_HotCol;
	
	size_t m_ScrollPos;
	POINT m_ScrollBegin;

	TDuiListViewControlSpi* m_Spi;
};

#endif