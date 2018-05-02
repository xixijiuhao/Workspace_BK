#ifndef _T_LISTVIEW_H_
#define _T_LISTVIEW_H_

class TListView
{
public:
    TListView();
    ~TListView();
    void CreateList(HWND hParent, UINT style, int itemHeight = 20, int headerHeight = 22, UINT exStyle = 0, int index = 0);
    void MoveWindow(int x, int y, int cx, int cy);
    void AddColumn(const TCHAR *pszText, int width = 100, int index = -1);
    void AddItem(const char *pszText, int iRow, int iCol, UINT text_format = 0, COLORREF textColor = -1);
    void AddItem(double data, int iRow, int iCol, UINT text_format = 0, COLORREF textColor = -1);
    void AddItem(int data, int iRow, int iCol, UINT text_format = 0, COLORREF textColor = -1);
	void AddItem(TCHAR *pszText, int iRow, int iCol, UINT text_format = 0, COLORREF textColor = -1);
    void ModifyItem(const char* pszText, int iRow, int iCol, UINT textFormat = 0, COLORREF textColor = -1);

    void DeleteItem(int iItem);
    void DeleteAllItems();
    void AdjustScroll();//��ȡListView ��Ϣ
public:
    int GetItemCount();
    HWND GetListHwnd(){ return m_hwndList; }
    int GetSelectItem(char* ptext, int nlength, int iSubitem);
    int GetSelectItem();
    //����ListView ����
public:
    void SetColor(COLORREF selectColor, COLORREF unSelectColor, COLORREF gridColor);
    void SetTxtColor(COLORREF textColor);
    void SetItemState(int iRow, int iCol, UINT textFormat, COLORREF textColor);
    void SelectItem(int index);
    //ListView �Ի�
public:
    void OnDrawItem(LPARAM lParam);
    void OnMeasureItem(UINT &height);
    //Header �ػ洦��
private:
    void OnHeaderPaint();
    void OnHeaderLayout(WPARAM wParam, LPARAM lParam);
    void FindBeginEnd(int left, int right, int &iBegin, int &iEnd);
	void OnButtonDown(WPARAM wParam, LPARAM lParam);
	void OnNotify(WPARAM wParam, LPARAM lParam);
    BOOL OnEraseBK(WPARAM wParam, LPARAM lParam);
private:
    static LRESULT CALLBACK NewHeaderProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR id, DWORD_PTR data); //�ػ�Header
    static LRESULT CALLBACK NewListProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR id, DWORD_PTR data);
private:
    HWND m_hwndList;
    HWND m_hwndHeader;
    int  m_iHeaderHeight;
    int  m_iItemHeight;
    int m_iVShift;  //��¼��һ�����ʱ��ֱ�����ϵ�ƫ����	HBRUSH m_hbr[3];   //Selected,unSelected,Grid
	int m_iPreSelectIndex;
    HBRUSH m_hbr[3];   //Selected,unSelected,Grid	vector<COLORREF> m_vItemColor; //������һ��Ԫ�أ�Ĭ��item������ɫ
    vector<COLORREF> m_vItemColor; //������һ��Ԫ�أ�Ĭ��item������ɫ	vector<vector<unsigned short>> m_vItemState;
    vector<vector<unsigned short>> m_vItemState;
	int m_iListId = 0;
	HWND m_hwndParent;
};

#endif