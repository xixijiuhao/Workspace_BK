#pragma once

typedef wchar_t ColName[51]; 

struct ContractInfoHeaderData
{
	int			index;
	ColName		dataName;
};
struct ContractInfoViewItemData
{
	int			index;
	I32			resultuid;
	STR20		mainPattern;
	SContractNoType	contractCode;
	STR20		exchange;
	STR50		symbol;
	I32			direction;
	STR10		interval;
	STR50		pattern;
	STR10		length;
	I32			quality;
	I32			identified_time;
	I32			completed;
};
class ACAnalysisPresenter;

class CContractInfoListView :public TIxFrm
{
	typedef std::function<void(ContractInfoViewItemData)> OnClickContractListItem;
	typedef std::vector<ContractInfoHeaderData> headerData;				//Header����
	typedef std::vector<ContractInfoViewItemData> ItemData;							//Item����
	typedef std::vector<RECT> vtRect;
	typedef std::map<std::string, wstring> MapPatternData;
public:
	CContractInfoListView();
	~CContractInfoListView();
	void RegistFunc(OnClickContractListItem func) 
	{
		if (func != NULL) 
		{
			m_ClickItem = func;
		}
	}
	void CreateView(HWND hParent, RECT r);
	void MoveWindow(RECT rect)
	{
		SetWindowPos(m_Hwnd, 0, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, 0);
	}

	void SetPresenter(ACAnalysisPresenter* presenter) { m_clPresenter = presenter; }

	void SetTotalData(ACDataMap& acDataMap);
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
	void PrepareItemTotalData(ACDataMap& data);											//׼����ǰ��ʾ��ȫ������
	void OnSize(WPARAM wParam, LPARAM lParam);
	void OnPaint();
	void OnMouseMove(WPARAM wParam, LPARAM lParam);
	void OnMouseLeave();
	void OnLButtonDown(WPARAM wParam, LPARAM lParam);
	void OnLButtonUp(WPARAM wParam, LPARAM lParam);
	void OnMouseWheel(WPARAM wParam, LPARAM lParam);
	void TrackMouse();
	bool GetHoverIndex(POINT point);
	void OnDealSetFocus();
	void DrawItemText(TMemDC &dc, int &index, HBRUSH &frameBrush);

	void InsertSymbolContractMap(string symbol);
	string GetSymbolName(string symbol);

public:
	ACAnalysisPresenter* m_clPresenter;

private:
	int					m_iItemHeight;		//����Item�߶�
	int					m_iHeaderHeight;	//Header�߶�
	OnClickContractListItem		m_ClickItem;
	
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
	ItemData			m_vtShowItemData;		//ItemData(��ʾ����)
	ItemData			m_itemData;				//��ǰ��ʾlistItemȫ������
	headerData			m_headerData;			//��ǰ��ʾlistHeaderȫ������
private:
	int					m_itemShowBegin;		//��ʾ��BeginIndex
	int					m_iheaderSelectIndex;	//Header��ǰѡ��Index
	int					m_itemSelectIndex;		//Item��ǰѡ��Index
	int					m_itemHoverIndex;		//item��ǰ����Index
	int					m_iheaderHoverIndex;	//header��ǰ����Index
	bool				m_bMousetrack;
	bool				m_bIsMoveItem;

	MapPatternData		m_PatternDataMap;
};