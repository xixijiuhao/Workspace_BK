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
	typedef std::map<int, std::vector<KContract>> ListTotalData;		//��������
	typedef ListTotalData::iterator listIterator;						
	typedef std::vector<KListViewHeaderData> headerData;				//Header����
	typedef std::vector<KContract> itemData;							//Item����
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
	void PrepareItemTotalData();											//׼����ǰ��ʾ��ȫ������
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
	int					m_iItemHeight;		//����Item�߶�
	int					m_iHeaderHeight;	//Header�߶�
	ListTotalData		m_totalData;
	OnClickListItem		m_ClickItem;
private:
	int					m_iHeight;
	int					m_iWidth;
	int					m_iShowCount;		//Item�ܹ���ʾ����
	int					m_iItemTotalHeight;	//Item�ܸ߶�			
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
	RECT				m_itemRect;				//��ItemRect
	RECT				m_headerRect;			//��HeaderRect
	vtRect				m_vtHeaderRect;			//HeaderRect
	vtRect				m_vtItemRect;			//ItemRect(��ʾ����)
	itemData			m_vtShowItemData;		//ItemData(��ʾ����)
	itemData			m_itemData;				//��ǰ��ʾlistItemȫ������
	headerData			m_headerData;			//��ǰ��ʾlistHeaderȫ������
private:
	int					m_itemShowBegin;		//��ʾ��BeginIndex
	int					m_iheaderSelectIndex;	//Header��ǰѡ��Index
	int					m_itemSelectIndex;		//Item��ǰѡ��Index
	int					m_itemHoverIndex;		//item��ǰ����Index
	int					m_iheaderHoverIndex;	//header��ǰ����Index
	bool				m_bMousetrack;		
	bool				m_bIsMoveItem;

};
class KListViewItem
{
};
class KListHeader
{

};