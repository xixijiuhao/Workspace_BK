#pragma once

#include <afx.h>
#include <vector>
#include <map>
#include <algorithm>
#include "ACCallback.h"

using namespace std;

enum TSortValueType
{
	svtNonne,
	svtNumber,
	svtString
};

//TNode里保存老的排序字段值, 新数据到来时按老的排序值查找所在位置，数据处理完成后更新排序字段值为新值，通过PGetSortValueFun函数获得新值，前台需要传过来PCommpareFun和PGetSortValueFun两个函数
//排序字段
struct TSortValue
{
	TSortValueType type;		//数据类型
	union 
	{
		double dValue;			//数字
		const string * sValue;	//字符串
	};
public:
	TSortValue(){ memset(this, 0, sizeof(*this)); }	
};

template<typename _Type>
class TNode
{
private:	
	typedef TSortValue (CALLBACK * PGetSortValueFun)(_Type*);
	typedef int (CALLBACK * PCommpareFun)(_Type*, _Type*);
	typedef vector<TNode<_Type> *>	vct_nodes;	
private:
	TNode<_Type> * m_parent;		//父节点

	_Type *		m_pdata;
	TSortValue	m_sort_value;

	bool		m_show_childs;		//子节点是否显示到了表格上 m_show_childs为true时m_expanded必定为true
	bool		m_expanded;			//是否展开, m_expanded为true时m_show_childs不一定为true
	int			m_nlevel;
	int			m_nimage;
	vct_nodes	m_childs; 

	
	void _sort(vct_nodes & items, int ilo, int ihi, PCommpareFun sort_commpare)
	{
		int lo = ilo;
		int hi = ihi;
		TNode<_Type> * mid_item = items[lo + (hi - lo) / 2];

		do 
		{
			while (sort_commpare(items[lo]->get_data(), mid_item->get_data()) < 0) lo++;
			while (sort_commpare(items[hi]->get_data(), mid_item->get_data()) > 0) hi--;
			if (lo <= hi)
			{
				if (sort_commpare(items[lo]->get_data(), items[hi]->get_data()) != 0)				//不交换相等的项
					swap(items[lo], items[hi]);
				lo++;
				hi--;
			}
		} while (lo <= hi);

		if (hi > ilo) _sort(items, ilo, hi, sort_commpare);
		if (lo < ihi) _sort(items, lo, ihi, sort_commpare);
	}
	inline void _sort(vct_nodes & items, PCommpareFun sort_commpare)
	{
		if (items.size()>1)
			_sort(items, 0, items.size() - 1, sort_commpare);
	}
public:
	TNode(): m_pdata(nullptr), m_expanded(false), m_nlevel(0), m_nimage(-1), m_show_childs(false), m_parent(nullptr){}
	inline void init()
	{
		m_parent		= nullptr;
		m_pdata			= nullptr;
		m_nlevel		= 0;
		m_nimage		= -1;
		m_expanded		= false;
		m_show_childs	= false;

		m_childs.clear();
	}
	void sort(PCommpareFun sort_commpare)
	{
		for (size_t i = 0; i < m_childs.size(); i++)
			m_childs[i]->sort(sort_commpare);
		_sort(m_childs, sort_commpare);
	}
	//在所有下层节点中查找
	int find_all(_Type * pdata, PCommpareFun sort_commpare, __out TNode<_Type> * parent, __inout int & view_index)
	{		
		int ind = binary_find(pdata, sort_commpare);
		if (ind >= 0)
		{
			parent = this;
			if (is_expanded())
				view_index += ind + 1;
			else
				view_index = -1;
			return ind;
		}
		
		for (size_t i = 0; i < m_childs.size(); i++)
		{
			if (is_expanded())
				view_index++;
			ind = m_childs[i].find_all(pdata, sort_commpare, parent, view_index);	
			if (ind >= 0)
				break;
		}

		return ind;
	}
	//顺序查找非顶层节点
	inline int order_find(_Type * pdata, PCommpareFun sort_commpare, size_t index, bool down)
	{
		int dx = -1;
		if (down)
			dx = 1;
		for (size_t i = index; i>=0 && i< m_childs.size(); i += dx)
		{
			if (sort_commpare(m_childs[i]->get_data(), pdata)!=0)
				break;
			if (m_childs[i]->get_data()==pdata)
				return i;
		}
		return -1;
	}
	//仅在子节点中查找，only_binary二分法查找后就返回，否则如果没有找到还需要顺序查找
	inline int binary_find(_Type * data, PCommpareFun sort_commpare, bool only_binary = false)          
	{		
		int high , mid , low , is_end;
		is_end = -1;
		low = 0;
		high = m_childs.size() - 1;
		while (high >= low)
		{
			mid = low + (high - low) / 2;

			is_end = sort_commpare(data, m_childs[mid]->get_data());
			if (is_end > 0)
				low = mid + 1;	
			else if (is_end < 0)
				high = mid - 1;
			else
				break;
		}

		//在二分的结果区间内顺序查找
		if (0==is_end)		
		{
			if (!only_binary)
			{
				int ind = order_find(data, sort_commpare, mid, false);
				if (ind < 0)
					ind = order_find(data, sort_commpare, mid+1, true);
				if (ind >= 0)
					return ind;
			}
			return mid;
		}
		else
			return low;
	}
	inline int insert(size_t index, TNode<_Type> * node)
	{
		if (index<0)
			index = 0;
		else if (index > m_childs.size())
			index = m_childs.size();

		node->m_nlevel = m_nlevel + 1;

		vct_nodes::iterator iter = m_childs.begin() + index;
		m_childs.insert(iter, node);
		node->m_parent = this;

		return index;
	}
	inline bool remove(size_t ind, vct_nodes & pool)
	{
		if (ind < 0 || ind >= m_childs.size())
			return false;

		m_childs[ind]->_clear(pool);
		vct_nodes::iterator iter = m_childs.begin();
		m_childs.erase(iter + ind);
		return true;
	}
	inline bool moveto(size_t old_index, size_t new_index)
	{		
		if (old_index == new_index)
			return false;
		if (old_index < 0 || old_index >= m_childs.size())
			return false;
		if (new_index < 0 || new_index > m_childs.size())
			return false;

		TNode<_Type>* node = m_childs[old_index];
		vct_nodes::iterator iter = m_childs.begin();
		m_childs.insert(iter + new_index, node);
		iter = m_childs.begin();						//插入之后iter可能变化，需要重新赋值
		if (old_index < new_index)
			m_childs.erase(iter + old_index);
		else
			m_childs.erase(iter + old_index + 1);

		return true;
	}
	void _clear(vct_nodes & pool)
	{
		for (size_t i=0; i<m_childs.size(); i++)
			m_childs[i]->_clear(pool);	

		init();
		pool.push_back(this);
	}
public:
	inline void set_params(bool bexpanded, int nimage = -1)
	{
		m_expanded		= bexpanded;
		m_nimage		= nimage;
	}
	template<typename _DataType>
	inline void set_data_value(_DataType * pdata)
	{
		m_pdata = (_Type*)pdata;
	}
	inline TNode<_Type> * get_parent()
	{
		return m_parent;		
	}
	inline _Type * get_data()
	{
		return m_pdata;
	}
	inline int get_child_count()
	{
		return m_childs.size();
	}
	inline int get_image()
	{
		return m_nimage;
	}
	inline int get_level()
	{
		return m_nlevel;
	}
	inline bool is_expanded()
	{
		return m_expanded && m_show_childs;		//展开又显示出来才算是真的展开了
	}
	//展开view_index：父节点在owner中的索引，level共展开几级子节点
	inline void expand(vct_nodes & owner, size_t parent_view_index, int level = 1)
	{
		if (level-- <= 0 && !m_expanded)		//没展开层级又不够就不显示，展开了的层级不够也要显示
			return;
		if (m_show_childs)						//已经显示则肯定展开了，不需要重新显示
			return;

		int ind = parent_view_index;
		for (size_t i = 0; i < m_childs.size(); i++, ++ind)
		{
			vct_nodes::iterator iter = owner.begin();
			owner.insert(iter + ind + 1, m_childs[i]);
			m_childs[i]->expand(owner, ind, level);
		}

		m_show_childs = m_expanded = true;		
	}
	//合并
	inline void collapse(vct_nodes & owner, size_t parent_view_index, int level = 1)
	{
		level--;

		if (m_show_childs)
		{
			for (size_t i = 0; i < m_childs.size(); i++)
			{
				m_childs[i]->collapse(owner, parent_view_index, level);
				vct_nodes::iterator iter = owner.begin();
				owner.erase(iter + parent_view_index + 1);
			}
		}

		m_show_childs = false;
		if (level >= 0)
			m_expanded = false;
	}
	inline TNode<_Type>* operator[](size_t index)
	{
		return m_childs[index];
	}
	virtual ~TNode()
	{
		m_pdata = nullptr;
		for (int i = m_childs.size() - 1; i >= 0; i--)
			delete m_childs[i];
		m_childs.clear();
	}
};


interface IGridRef
{
	virtual void set_draw(bool draw) = 0;
	virtual void redraw() = 0;
	virtual void redraw(size_t first_row, size_t last_row, size_t first_col, size_t last_col) = 0;
	virtual void redraw_row(size_t first, size_t last) = 0;
	virtual void redraw_col(size_t first, size_t last) = 0;
	virtual void set_row_count(size_t count) = 0;
	virtual size_t get_row_count() = 0;
	virtual void set_col_count(size_t count) = 0;
	virtual size_t get_col_count() = 0;
};


//表格里面显示m_nodes的节点，节点展开时将子节点插入到m_nodes里面以供显示，节点合并时从m_nodes删除该节点的子节点
//排序时先合并所有顶层节点，排序后再展开
//表格清空后所有节点缓存在m_pool里，下次加入数据时可以重复使用
//所有用户加入的数据都保存在m_datas里，用户更新数据时先找到m_datas里找到上次更新的数据，并根据此数据快速找到原来的保存位置，再根据新数据快速确定现在应该保存的位置
//如果不保存上次数据，而只是保存用户数据指针，则新数据到来且新数据排序列的值发生改变时，只有通过顺序查找才可以确定原来的数据位置，数据量大时效率会很低(数据量太大时是否可以改为数据存在则刷新表格)
template<typename _Type, typename _Key>
class TNodes
{
private:	
	typedef int (CALLBACK * PCommpareFun)(_Type*, _Type*);
	typedef vector<TNode<_Type> *>			vct_nodes;		//按排序字段找
	typedef map<_Key, TNode<_Type> *>		map_all_nodes;	//按Key找
private:		
	vct_nodes		m_pool;					//节点缓存池
	vct_nodes		m_nodes;				//显示节点集
	map_all_nodes	m_datas;				//原始数据节点集
	IGridRef *		m_grid;					//关联表格
	bool			m_had_child;			//任何节点是否存在子节点

	
	inline void clear_pool()
	{
		for (int i = m_pool.size() - 1; i >= 0; i--)
			delete m_pool[i];
		m_pool.clear();
	}
	inline void clear_nodes()
	{
		collapse_all();						//需要先合并顶层节点
		for (int i = m_nodes.size() - 1; i >= 0; i--)
			if (m_nodes[i]->get_level()<=0)
				delete m_nodes[i];
		m_nodes.clear();
	}

	void _sort(vct_nodes & items, int ilo, int ihi, PCommpareFun sort_commpare)
	{
		int lo = ilo;
		int hi = ihi;
		TNode<_Type> * mid_item = items[lo + (hi - lo) / 2];

		do 
		{
			while (sort_commpare(items[lo]->get_data(), mid_item->get_data()) < 0) lo++;
			while (sort_commpare(items[hi]->get_data(), mid_item->get_data()) > 0) hi--;
			if (lo <= hi)
			{
				if (sort_commpare(items[lo]->get_data(), items[hi]->get_data()) != 0)				//不交换相等的项
					swap(items[lo], items[hi]);
				lo++;
				hi--;
			}
		} while (lo <= hi);

		if (hi > ilo) _sort(items, ilo, hi, sort_commpare);
		if (lo < ihi) _sort(items, lo, ihi, sort_commpare);
	}
	inline void _sort(vct_nodes & items, PCommpareFun sort_commpare)
	{
		if (items.size()>1)
			_sort(items, 0, items.size() - 1, sort_commpare);
	}
public:
	TNodes(IGridRef * grid = nullptr):m_grid(grid), m_had_child(false){}
	inline TNode<_Type> * at(size_t index)
	{
		return m_nodes.at(index);
	}
	inline TNode<_Type> * operator[](size_t index)
	{
		return m_nodes[index];
	}
	inline int index_of(TNode<_Type> * pNode)
	{
		return (&pNode - &(m_nodes[0])) / sizeof(pNode)
	}
	inline void expand(size_t ind, int level = 1, bool draw=true)
	{
		if (m_had_child)
		{
			m_nodes[ind]->expand(m_nodes, ind, level);
			if (m_grid && draw)
				m_grid->set_row_count(m_nodes.size());
		}
	}
	inline void collapse(size_t ind, int level = 1, bool draw=true)
	{		
		if (m_had_child)
		{
			m_nodes[ind]->collapse(m_nodes, ind, level);
			if (m_grid && draw)
				m_grid->set_row_count(m_nodes.size());
		}
	}
	//level=1,展开顶层节点，level=0显示状态为展开的顶层节点的子节点，用以便恢复虚合并
	inline void expand_all(int level = 1, bool draw=true)
	{
		if (m_had_child)
			for (size_t i = 0; i < m_nodes.size(); i++)
				expand(i, level, draw);
	}
	//level=1,合并顶层节点，level=0虚合并顶层节点（不显示顶层节点的子节点，但顶层节点的展开状态不会改变），以便恢复展开状态
	inline void collapse_all(int level = 1, bool draw=true)
	{
		if (m_had_child)
			for (size_t i = 0; i < m_nodes.size(); i++)
				collapse(i, level, draw);
	}
protected:
	inline TNode<_Type> * alloca_node()
	{
		if (m_pool.empty())
			return new TNode<_Type>();
		else
		{
			TNode<_Type> * node = m_pool.back();
			m_pool.pop_back();

			ASSERT(node->get_level()==0 && node->get_child_count()==0 && node->is_expanded()==false);
			return node;
		}
	}
	template<typename _DataType>
	inline TNode<_Type> * new_data(_DataType * data, _Key key)
	{
		TNode<_Type> * node = alloca_node();
		node->set_data_value(data);		
		save_by_key(key, node);

		return node;
	}
protected:
	//临时用
	inline int order_find_ex(_Type * pdata, size_t index, bool down)
	{
		int dx = down ? 1 : -1;
		for (size_t i = index; i>=0 && i < m_nodes.size(); i += dx)
		{
			if (m_nodes[i]->get_level() > 0)
				continue;
			if (m_nodes[i]->get_data()==pdata)
				return i;
		}
		return -1;
	}
	//在排序值相等的条件下顺序查找顶层节点
	inline int order_find(_Type * pdata, PCommpareFun sort_commpare, size_t index, bool down)
	{
		int dx = -1;
		if (down)
			dx = 1;
		for (size_t i = index; i>=0 && i < m_nodes.size(); i += dx)
		{
			if (m_nodes[i]->get_level() > 0)
				continue;
			if (sort_commpare(m_nodes[i]->get_data(), pdata)!=0)//关键字是否相等
				break;
			if (m_nodes[i]->get_data()==pdata)
				return i;
		}
		return -1;
	}
	//二分查找顶层节点, 需要绕过非顶层节点，only_binary二分法查找后就返回，否则如果没有找到还需要顺序查找
	inline int binary_find(_Type * data, PCommpareFun sort_commpare, bool only_binary = false)          
	{		
		int high , mid , low , is_end;
		is_end = -1;
		low = 0;
		high = m_nodes.size() - 1;
		while (high >= low)
		{
			if (m_nodes[high]->get_level()>0)
			{
				high--;
				continue;
			}
			if (m_nodes[low]->get_level()>0)
			{
				low++;
				continue;
			}

			mid = low + (high - low) / 2;
			while (m_nodes[mid]->get_level()>0)
				mid--;

			is_end = sort_commpare(data, m_nodes[mid]->get_data());
			if (is_end > 0)
				low = mid + 1;	
			else if (is_end < 0)
				high = mid - 1;
			else
				break;
		}
		
		//在二分的结果区间内顺序查找
		if (0==is_end)		
		{
			if (!only_binary)
			{
				int ind = order_find(data, sort_commpare, mid, false);
				if (ind < 0)
					ind = order_find(data, sort_commpare, mid+1, true);
				if (ind >= 0)
					return ind;
			}
			return mid;
		}
		else
		{
			while ((size_t)low < m_nodes.size() && m_nodes[low]->get_level()>0)
				low++;

			return low;
		}
	}
	//在所有下层节点中查找并定位插入位置, parent返回父节点，函数返回值返回在父节点下的位置，view_index返回查找节点的显示位置,
	inline int find_all(_Type * pdata, PCommpareFun sort_commpare, __out TNode<_Type> * parent, __out int & view_index)
	{		
		parent = nullptr;
		int ind = view_index = binary_search(data, sort_commpare);
		if (ind >= 0)
			return ind;

		for (size_t i = 0; i < m_nodes.size(); i++)
		{
			view_index++;
			ind = m_nodes[i].find_all(pdata, sort_commpare, parent, view_index);
			if (ind >= 0)
				return ind;
		}
			
		return -1;
	}
	inline bool moveto(size_t old_index, size_t new_index, TNode<_Type> * parent = nullptr, size_t parent_view_index = 0)
	{
		if (old_index == new_index)
			return false;

		if (parent)
			parent->moveto(old_index, new_index);


		int old_v_ind = get_view_index(old_index, parent, parent_view_index); 
		int new_v_ind = get_view_index(new_index, parent, parent_view_index); 

		if (old_v_ind < 0 || (size_t)old_v_ind >= m_nodes.size())
			return false;
		if (new_v_ind < 0 ||  (size_t)new_v_ind > m_nodes.size())
			return false;	
		
		TNode<_Type> * node = m_nodes[old_v_ind];
		vct_nodes::iterator iter = m_nodes.begin();
		m_nodes.insert(iter + new_v_ind, node);
		iter = m_nodes.begin();						//插入之后iter可能变化，需要重新赋值
		if (old_v_ind < new_v_ind)
			m_nodes.erase(iter + old_v_ind);
		else
			m_nodes.erase(iter + old_v_ind + 1);
		
		if (m_grid)
		{
			if (old_v_ind < new_v_ind)
				m_grid->redraw_row(old_v_ind, new_v_ind-1);
			else
				m_grid->redraw_row(new_v_ind, old_v_ind);
		}

		return true;
	}
protected:
	//按关键字保存数据
	inline void save_by_key(_Key key, TNode<_Type> * node)
	{
		pair<map_all_nodes::iterator, bool> ret = m_datas.insert(map_all_nodes::value_type(key, node));
		if (!ret.second && ret.first->second != node)
			ret.first->second = node;
	}
	//按关键字查找数据节点，如果是无效数据则直接删除之,IsPop为true则在查找之后删除数据
	inline TNode<_Type> * find_by_key(_Key key, bool IsPop = false)
	{		
		map_all_nodes::iterator iter = m_datas.find(key);
		if (iter != m_datas.end())
		{
			if (iter->second->get_data() != nullptr)
			{
				TNode<_Type> * node = iter->second;
				if (IsPop)
					m_datas.erase(iter);
				return node;
			}
			else
				m_datas.erase(iter);
		}

		return nullptr;
	}
	//在子节点中查找并定位应插入的位置
	inline int find(_Type * data, PCommpareFun sort_commpare, TNode<_Type> * parent = nullptr, bool only_binary = false)
	{
		if (parent)
			return parent->binary_find(data, sort_commpare, only_binary);
		else
			return binary_find(data, sort_commpare, only_binary);
	}
	template<typename _DataType>
	inline int insert(size_t index, _DataType * data, _Key key, TNode<_Type> * parent = nullptr, size_t parent_view_index = 0, bool bexpanded = false, int nimage = -1)
	{
		TNode<_Type> * node = new_data(data, key);
		node->set_params(bexpanded, nimage);

		int ind = index;
		if (parent)
		{
			ind = parent->insert(index, node);
			m_had_child = ind >= 0 || m_had_child;
		}

		int v_ind = get_view_index(ind, parent, parent_view_index);
		if (v_ind >= 0 && (size_t)v_ind <= m_nodes.size())
		{
			vct_nodes::iterator iter = m_nodes.begin() + v_ind;
			m_nodes.insert(iter, node);
			if (bexpanded)
			{
				size_t _v_ind = v_ind;
				node->expand(m_nodes, _v_ind);
			}

			if (m_grid)
				m_grid->set_row_count(m_nodes.size());
		}

		return ind;
	}
public:
	//设置关联表格
	inline void set_grid(IGridRef * pgrid)
	{
		m_grid = pgrid;
		if (m_grid)
			m_grid->set_row_count(m_nodes.size());
	}

	//find比较函数为类成员函数
	template<typename _DataType, class _Class, typename _pMemberFunc> 
	inline int find(_DataType * data, _Key key, _Class * pClass, _pMemberFunc pMemberFunc, TNode<_Type> * parent = nullptr)			
	{
		ACCallback<PCommpareFun, _Class, _pMemberFunc> sort_commpare(pClass, pMemberFunc);
		return find(data, key, (PCommpareFun)sort_commpare, parent);
	}
	template<typename _DataType>
	inline int find(_DataType * data, _Key key, PCommpareFun sort_commpare, TNode<_Type> * parent = nullptr)
	{
		TNode<_Type> * node = find_by_key(key);

		if (node==nullptr)
			return -1;
		else
			return find(node->get_data(), sort_commpare, parent);		
	}

	//add比较函数为类成员函数
	template<typename _DataType, class _Class, typename _pMemberFunc> 
	inline int add(_DataType * data, _Key key, _Class * pClass, _pMemberFunc pMemberFunc, TNode<_Type> * parent = nullptr, size_t parent_view_index = 0, bool bexpanded = false, int nimage = -1)			
	{
		ACCallback<PCommpareFun, _Class, _pMemberFunc> sort_commpare(pClass, pMemberFunc);
		return add(data, key, (PCommpareFun)sort_commpare, parent, parent_view_index, bexpanded, nimage);
	}
	//新增并维持原有顺序, 不检查重复项
	template<typename _DataType>
	inline int add(_DataType * data, _Key key, PCommpareFun sort_commpare, TNode<_Type> * parent = nullptr, size_t parent_view_index = 0, bool bexpanded = false, int nimage = -1)
	{
		int ind = find((_Type *)data, sort_commpare, parent, true);
		return insert(ind, data, key, parent, parent_view_index, bexpanded, nimage);
	}

	//update比较函数为类成员函数
	template<typename _DataType, class _Class, typename _pMemberFunc> 
	inline int update(_DataType * data, _Key key, _Class * pClass, _pMemberFunc pMemberFunc, TNode<_Type> * parent = nullptr, size_t parent_view_index = 0, bool bexpanded = false, int nimage = -1)			
	{
		ACCallback<PCommpareFun, _Class, _pMemberFunc> sort_commpare(pClass, pMemberFunc);
		return update(data, key, (PCommpareFun)sort_commpare, parent, parent_view_index, bexpanded, nimage);
	}
	//原数据不存在则增加，存在则更新，并调整新数据的排序位置
	//key:data的唯一关键字，sort_commpare排序比较函数
	template<typename _DataType>
	inline int update(_DataType * data, _Key key, PCommpareFun sort_commpare, TNode<_Type> * parent = nullptr, size_t parent_view_index = 0, bool bexpanded = false, int nimage = -1)
	{
		TNode<_Type> * node = find_by_key(key);

		if (node==nullptr)
			return add(data, key, sort_commpare, parent, parent_view_index, bexpanded, nimage);
		else
		{
			if (m_grid)
				m_grid->redraw();				//临时，数据更新后暂时不重新排序 当前没有老数据，没法找老的索引
			return -2;

			int ind = -2;
			bool order_chage = sort_commpare(node->get_data(), (_Type *)data)!=0; 

			if (order_chage || parent==nullptr || parent->is_expanded())			//顺序未变，也没显示，只需更新数据，不用刷新表格,否则
			{
				ind = find(node->get_data(), sort_commpare, parent);				//查找老数据位置， 是为了刷新当前行而查找一下，还是直接刷新所有行(可见行)，哪个更高效呢
				
				if (order_chage)				//删老的，插新的
				{				
					int old_ind = ind;
					ind = find((_Type*)data, sort_commpare, parent);				//定位新位置					
					node->set_data_value(data);										//更新数据
					moveto(old_ind, ind, parent, parent_view_index);
				}
				else							//如显示则刷新
				{
					node->set_data_value(data);										//更新数据
					int v_ind = get_view_index(ind, parent, parent_view_index);				
					if (v_ind >=0 && m_grid)
						m_grid->redraw_row(v_ind, v_ind);
				}		
				
				if (bexpanded && !node->is_expanded())
				{
					size_t v_ind = get_view_index(ind, parent, parent_view_index);	
					expand(v_ind);
				}
			}
			else				
				node->set_data_value(data);	
			
			return ind;
		}
	}
	
	//remove比较函数为类成员函数
	template<typename _DataType, class _Class, typename _pMemberFunc> 
	inline int remove(_DataType * data, _Key key, _Class * pClass, _pMemberFunc pMemberFunc, TNode<_Type> * parent = nullptr, size_t parent_view_index = 0)			
	{
		ACCallback<PCommpareFun, _Class, _pMemberFunc> sort_commpare(pClass, pMemberFunc);
		return remove(data, key, (PCommpareFun)sort_commpare, parent, parent_view_index);
	}
	//删除一条数据
	template<typename _DataType>
	inline int remove(_DataType * data, _Key key, PCommpareFun sort_commpare, TNode<_Type> * parent = nullptr, size_t parent_view_index = 0)
	{
		TNode<_Type> * node = find_by_key(key, true);

		if (node == nullptr)
			return -1;

		//TODO: find时，增加直接使用顺序查找的参数 //order_find临时增加顺序查找
		int ind = order_find_ex(data, 0, true);//find(node->get_data(), sort_commpare, parent);//当前没有老数据，没法找老的索引
		ASSERT(ind>=0);
		if (ind < 0)
			return -1;
		
		//删除表格显示行，先合并本节点，然后删除之,需要更新表格行数
		int v_ind = get_view_index(ind, parent, parent_view_index); 		
		if ((uint)v_ind >= m_nodes.size())
			return -1;
		if (v_ind >= 0)
		{
			collapse(v_ind, false);
			vct_nodes::iterator iter1 = m_nodes.begin() + v_ind;
			m_nodes.erase(iter1);				
			
			if (m_grid)
				m_grid->set_row_count(m_nodes.size());
		}

		//删除存储的数据
		if (parent)
			parent->remove(ind, m_pool);		
		else
			node->_clear(m_pool);

		
		//子节点的关键字索引没有删除，只是清空了子节点中的数据，这样可以在find_by_key中再清理
		
		return ind;
	}
	//获得某个节点的显示位置
	int view_index(TNode<_Type> * node, PCommpareFun sort_commpare)
	{
		TNode<_Type> * parent = node->get_parent();
		if (parent==nullptr)
			return binary_find(node->get_data(), sort_commpare);
		else if (parent->is_expanded())
		{
			int ind = parent->binary_find(node->get_data(), sort_commpare);
			int parent_int = -1;
			if (ind >= 0)
				parent_int = view_index(parent, sort_commpare);

			if (parent_int >=0  && ind >= 0)
				return ind + parent_ind + 1;
			else
				return -1;
		}
		else
			return -1;
	}
	//获得表格显示行号
	inline int get_view_index(size_t child_index, TNode<_Type> * parent = nullptr, size_t parent_view_index = 0)
	{
		if (parent == nullptr)
			return child_index;
		else if (child_index >=0 && parent->is_expanded())	
			return parent_view_index + child_index + 1;

		return -1;
	}

	//remove比较函数为类成员函数
	template<class _Class, typename _pMemberFunc> 
	inline void sort(_Class * pClass, _pMemberFunc pMemberFunc)			
	{
		ACCallback<PCommpareFun, _Class, _pMemberFunc> sort_commpare(pClass, pMemberFunc);
		sort((PCommpareFun)sort_commpare);
	}

	//排序,排序时可以不排合并了的项目，等到这个项目展开时再排序，合并项比较多的时候可以加快速度，但是需要保存上次排序的函数
	//排序函数又是个外部函数，等有节点需要展开时排序函数所在的类有可能已经不存在了,所以这样做不是很合适
	inline void sort(PCommpareFun sort_commpare)
	{
		if (m_grid)
			m_grid->set_draw(false);

		collapse_all(0, false);

		if (m_had_child)
			for (size_t i = 0; i < m_nodes.size(); i++)
				m_nodes[i]->sort(sort_commpare);
		_sort(m_nodes, sort_commpare);

		expand_all(0, false);
		
		if (m_grid)
		{
			m_grid->set_draw(true);
			m_grid->redraw();
		}
	}
	inline void clear()
	{
		if (m_grid)
			m_grid->set_row_count(0);

		m_datas.clear();					
		for (size_t i = m_nodes.size(); i-- > 0; )
		{
			if (m_nodes[i]->get_level()==0)			
				m_nodes[i]->_clear(m_pool);
		}
		m_nodes.clear();

		m_had_child = false;
	}
	inline void reserve(size_t count)
	{
		m_nodes.reserve(count);
	}
	inline size_t view_size()
	{
		return m_nodes.size();
	}
	virtual ~TNodes()
	{
		if (m_grid)
		{
			m_grid->set_row_count(0);
			m_grid = nullptr;
		}
		m_datas.clear();
		clear_nodes();
		clear_pool();
	}
};