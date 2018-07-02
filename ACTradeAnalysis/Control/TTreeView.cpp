#include "BaseInclude.h"

TreeView::TreeView()
{

}
TreeView::~TreeView()
{

}
void TreeView::CreateTree(HWND hParent, int left, int top, int width, int height)
{
	m_hTree = CreateWindowEx(0, WC_TREEVIEW, 0, WS_BORDER | WS_VISIBLE | WS_CHILD | TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT | TVS_SHOWSELALWAYS | TVS_TRACKSELECT
		| WS_TABSTOP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
		left, top, width, height, hParent, 0, GetModuleHandle(NULL), 0);

}
void TreeView::MoveWindow(int x, int y, int cx, int cy)
{
	SetWindowPos(m_hTree, 0, x, y, cx, cy, SWP_SHOWWINDOW | SWP_NOZORDER);
}
//level: zero-based,第几层
//at:第几个节点的子节点
void TreeView::AddNode(const wchar_t* pText, int level, int at)
{
	TVINSERTSTRUCT tvInst;
	tvInst.item.mask = TVIF_TEXT;
	tvInst.hInsertAfter = TVI_LAST;
	tvInst.hParent = NULL;
	tvInst.item.pszText = const_cast<wchar_t*>(pText);
	if (level == 0)  //father node
	{
		HTREEITEM hNode = (HTREEITEM)SendMessage(m_hTree, TVM_INSERTITEM, 0, (LPARAM)&tvInst);
		m_TreeNode.push_back(hNode);
	}
	else if (level == 1) //son node
	{
		if (at >= m_TreeNode.size())
			return;
		tvInst.hParent = m_TreeNode[at];
		HTREEITEM hChild = (HTREEITEM)SendMessage(m_hTree, TVM_INSERTITEM, 0, (LPARAM)&tvInst);
		m_ChildNode.push_back(hChild);
	}
	else if (level == 2) //grand son node
	{
		if (at >= m_ChildNode.size())
			return;
		tvInst.hParent = m_ChildNode[at];
		HTREEITEM hGrandSonNode = (HTREEITEM)SendMessage(m_hTree, TVM_INSERTITEM, 0, (LPARAM)&tvInst);
		m_GrandSonNode.push_back(hGrandSonNode);
	}
}
void TreeView::Expand(UINT which, UINT child)
{
	if (child == 0 && which < m_TreeNode.size())
		TreeView_Expand(m_hTree, m_TreeNode[which], TVE_EXPAND);
	else if (child != 0 && child < m_ChildNode.size())
		TreeView_Expand(m_hTree, m_ChildNode[child], TVE_EXPAND);
}