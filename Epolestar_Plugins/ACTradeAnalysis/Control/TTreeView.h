#ifndef T_TREE_VIEW_H
#define T_TREE_VIEW_H
class TreeView
{
public:
	TreeView();
	~TreeView();
public:
	void CreateTree(HWND hParent, int left, int top, int width, int height);
	void AddNode(const wchar_t* pText, int level = 0, int at = 0);
	HWND GetHwnd(){ return m_hTree; }
	void MoveWindow(int x, int y, int cx, int cy);
	void Expand(UINT which, UINT child = 0);
private:
	HWND            m_hTree;
	std::vector<HTREEITEM>m_TreeNode;
	std::vector<HTREEITEM>m_ChildNode;
	std::vector<HTREEITEM>m_GrandSonNode;
};
#endif
