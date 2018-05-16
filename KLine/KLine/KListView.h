#pragma once
struct KListViewHeaderData
{
	int					index;
	KPlateName			plateName;
};
struct KListViewItemData
{
	int					index;
	SContractCodeType	contractCode;
};
class KListView :public TIxFrm
{
	typedef std::function<void(KContract)> OnClickListItem;
	typedef std::map<int, std::vector<KContract>> ListTotalData;		//整体数据
	typedef ListTotalData::iterator listIterator;						
	typedef std::vector<KListViewHeaderData> headerData;				//Header数据
	typedef std::vector<KContract> itemData;							//Item数据
	typedef std::vector<RECT> vtRect;
public:
	KListView();
	~KListView();
	void RegistFunc(OnClickListItem func) {
		if (func != NULL){
			m_ClickItem = func;
		}
	}
	void CreateView(HWND hParent, RECT r);
	void MoveWindow(RECT rect)
	{
		SetWindowPos(m_Hwnd, 0, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, 0);
	}
	void SetTotalData(ListTotalData& data);
	void SetItemHeight(int i) { m_iItemHeight = i; };
	void SetHeaderHeight(int i) { m_iHeaderHeight = i; };
protected:
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	void PrepareRect(RECT r);
	void PrepareItemRect();
	void PrepareHeaderRect();
	bool GetShowIndex(int change);
	void PrepareSubData();
	void PrepareHeaderTotalData();
	void PrepareItemTotalData();											//准备当前显示的全部数据
	void OnSize(WPARAM wParam, LPARAM lParam);
	void OnPaint();
	void OnMouseMove(WPARAM wParam, LPARAM lParam);
	void OnMouseLeave();
	void OnLButtonDown(WPARAM wParam, LPARAM lParam);
	void OnLButtonUp(WPARAM wParam, LPARAM lParam);
	void OnMouseWheel(WPARAM wParam, LPARAM lParam);
	void TrackMouse();
	bool GetHoverIndex(POINT point);
private:
	int					m_iItemHeight;		//单个Item高度
	int					m_iHeaderHeight;	//Header高度
	ListTotalData		m_totalData;
	OnClickListItem		m_ClickItem;
private:
	int					m_iHeight;
	int					m_iWidth;
	int					m_iShowCount;		//Item能够显示数量
	int					m_iItemTotalHeight;	//Item总高度			
private:
	COLORREF			m_bkColor;
	COLORREF			m_frameColor;
	COLORREF			m_headerColor;
	COLORREF			m_textColor;
	COLORREF			m_scrollColor;
	COLORREF			m_selectColor;
	COLORREF			m_selectTextColor;
private:
	HWND				m_hParent;
	RECT				m_frameRect;			//Frame
	RECT				m_scrollRect;			//Scroll
	RECT				m_itemRect;				//总ItemRect
	RECT				m_headerRect;			//总HeaderRect
	vtRect				m_vtHeaderRect;			//HeaderRect
	vtRect				m_vtItemRect;			//ItemRect(显示部分)
	itemData			m_vtShowItemData;		//ItemData(显示部分)
	itemData			m_itemData;				//当前显示listItem全部数据
	headerData			m_headerData;			//当前显示listHeader全部数据
private:
	int					m_itemShowBegin;		//显示的BeginIndex
	int					m_iheaderSelectIndex;	//Header当前选中Index
	int					m_itemSelectIndex;		//Item当前选中Index
	int					m_itemHoverIndex;		//item当前悬浮Index
	int					m_iheaderHoverIndex;	//header当前悬浮Index
	bool				m_bMousetrack;		
	bool				m_bIsMoveItem;

};
class KListViewItem
{
};
class KListHeader
{

};