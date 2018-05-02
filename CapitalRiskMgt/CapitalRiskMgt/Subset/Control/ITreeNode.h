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

//TNode�ﱣ���ϵ������ֶ�ֵ, �����ݵ���ʱ���ϵ�����ֵ��������λ�ã����ݴ�����ɺ���������ֶ�ֵΪ��ֵ��ͨ��PGetSortValueFun���������ֵ��ǰ̨��Ҫ������PCommpareFun��PGetSortValueFun��������
//�����ֶ�
struct TSortValue
{
	TSortValueType type;		//��������
	union 
	{
		double dValue;			//����
		const string * sValue;	//�ַ���
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
	TNode<_Type> * m_parent;		//���ڵ�

	_Type *		m_pdata;
	TSortValue	m_sort_value;

	bool		m_show_childs;		//�ӽڵ��Ƿ���ʾ���˱���� m_show_childsΪtrueʱm_expanded�ض�Ϊtrue
	bool		m_expanded;			//�Ƿ�չ��, m_expandedΪtrueʱm_show_childs��һ��Ϊtrue
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
				if (sort_commpare(items[lo]->get_data(), items[hi]->get_data()) != 0)				//��������ȵ���
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
	//�������²�ڵ��в���
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
	//˳����ҷǶ���ڵ�
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
	//�����ӽڵ��в��ң�only_binary���ַ����Һ�ͷ��أ��������û���ҵ�����Ҫ˳�����
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

		//�ڶ��ֵĽ��������˳�����
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
		iter = m_childs.begin();						//����֮��iter���ܱ仯����Ҫ���¸�ֵ
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
		return m_expanded && m_show_childs;		//չ������ʾ�������������չ����
	}
	//չ��view_index�����ڵ���owner�е�������level��չ�������ӽڵ�
	inline void expand(vct_nodes & owner, size_t parent_view_index, int level = 1)
	{
		if (level-- <= 0 && !m_expanded)		//ûչ���㼶�ֲ����Ͳ���ʾ��չ���˵Ĳ㼶����ҲҪ��ʾ
			return;
		if (m_show_childs)						//�Ѿ���ʾ��϶�չ���ˣ�����Ҫ������ʾ
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
	//�ϲ�
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


//���������ʾm_nodes�Ľڵ㣬�ڵ�չ��ʱ���ӽڵ���뵽m_nodes�����Թ���ʾ���ڵ�ϲ�ʱ��m_nodesɾ���ýڵ���ӽڵ�
//����ʱ�Ⱥϲ����ж���ڵ㣬�������չ��
//�����պ����нڵ㻺����m_pool��´μ�������ʱ�����ظ�ʹ��
//�����û���������ݶ�������m_datas��û���������ʱ���ҵ�m_datas���ҵ��ϴθ��µ����ݣ������ݴ����ݿ����ҵ�ԭ���ı���λ�ã��ٸ��������ݿ���ȷ������Ӧ�ñ����λ��
//����������ϴ����ݣ���ֻ�Ǳ����û�����ָ�룬�������ݵ����������������е�ֵ�����ı�ʱ��ֻ��ͨ��˳����Ҳſ���ȷ��ԭ��������λ�ã���������ʱЧ�ʻ�ܵ�(������̫��ʱ�Ƿ���Ը�Ϊ���ݴ�����ˢ�±��)
template<typename _Type, typename _Key>
class TNodes
{
private:	
	typedef int (CALLBACK * PCommpareFun)(_Type*, _Type*);
	typedef vector<TNode<_Type> *>			vct_nodes;		//�������ֶ���
	typedef map<_Key, TNode<_Type> *>		map_all_nodes;	//��Key��
private:		
	vct_nodes		m_pool;					//�ڵ㻺���
	vct_nodes		m_nodes;				//��ʾ�ڵ㼯
	map_all_nodes	m_datas;				//ԭʼ���ݽڵ㼯
	IGridRef *		m_grid;					//�������
	bool			m_had_child;			//�κνڵ��Ƿ�����ӽڵ�

	
	inline void clear_pool()
	{
		for (int i = m_pool.size() - 1; i >= 0; i--)
			delete m_pool[i];
		m_pool.clear();
	}
	inline void clear_nodes()
	{
		collapse_all();						//��Ҫ�Ⱥϲ�����ڵ�
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
				if (sort_commpare(items[lo]->get_data(), items[hi]->get_data()) != 0)				//��������ȵ���
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
	//level=1,չ������ڵ㣬level=0��ʾ״̬Ϊչ���Ķ���ڵ���ӽڵ㣬���Ա�ָ���ϲ�
	inline void expand_all(int level = 1, bool draw=true)
	{
		if (m_had_child)
			for (size_t i = 0; i < m_nodes.size(); i++)
				expand(i, level, draw);
	}
	//level=1,�ϲ�����ڵ㣬level=0��ϲ�����ڵ㣨����ʾ����ڵ���ӽڵ㣬������ڵ��չ��״̬����ı䣩���Ա�ָ�չ��״̬
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
	//��ʱ��
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
	//������ֵ��ȵ�������˳����Ҷ���ڵ�
	inline int order_find(_Type * pdata, PCommpareFun sort_commpare, size_t index, bool down)
	{
		int dx = -1;
		if (down)
			dx = 1;
		for (size_t i = index; i>=0 && i < m_nodes.size(); i += dx)
		{
			if (m_nodes[i]->get_level() > 0)
				continue;
			if (sort_commpare(m_nodes[i]->get_data(), pdata)!=0)//�ؼ����Ƿ����
				break;
			if (m_nodes[i]->get_data()==pdata)
				return i;
		}
		return -1;
	}
	//���ֲ��Ҷ���ڵ�, ��Ҫ�ƹ��Ƕ���ڵ㣬only_binary���ַ����Һ�ͷ��أ��������û���ҵ�����Ҫ˳�����
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
		
		//�ڶ��ֵĽ��������˳�����
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
	//�������²�ڵ��в��Ҳ���λ����λ��, parent���ظ��ڵ㣬��������ֵ�����ڸ��ڵ��µ�λ�ã�view_index���ز��ҽڵ����ʾλ��,
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
		iter = m_nodes.begin();						//����֮��iter���ܱ仯����Ҫ���¸�ֵ
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
	//���ؼ��ֱ�������
	inline void save_by_key(_Key key, TNode<_Type> * node)
	{
		pair<map_all_nodes::iterator, bool> ret = m_datas.insert(map_all_nodes::value_type(key, node));
		if (!ret.second && ret.first->second != node)
			ret.first->second = node;
	}
	//���ؼ��ֲ������ݽڵ㣬�������Ч������ֱ��ɾ��֮,IsPopΪtrue���ڲ���֮��ɾ������
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
	//���ӽڵ��в��Ҳ���λӦ�����λ��
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
	//���ù������
	inline void set_grid(IGridRef * pgrid)
	{
		m_grid = pgrid;
		if (m_grid)
			m_grid->set_row_count(m_nodes.size());
	}

	//find�ȽϺ���Ϊ���Ա����
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

	//add�ȽϺ���Ϊ���Ա����
	template<typename _DataType, class _Class, typename _pMemberFunc> 
	inline int add(_DataType * data, _Key key, _Class * pClass, _pMemberFunc pMemberFunc, TNode<_Type> * parent = nullptr, size_t parent_view_index = 0, bool bexpanded = false, int nimage = -1)			
	{
		ACCallback<PCommpareFun, _Class, _pMemberFunc> sort_commpare(pClass, pMemberFunc);
		return add(data, key, (PCommpareFun)sort_commpare, parent, parent_view_index, bexpanded, nimage);
	}
	//������ά��ԭ��˳��, ������ظ���
	template<typename _DataType>
	inline int add(_DataType * data, _Key key, PCommpareFun sort_commpare, TNode<_Type> * parent = nullptr, size_t parent_view_index = 0, bool bexpanded = false, int nimage = -1)
	{
		int ind = find((_Type *)data, sort_commpare, parent, true);
		return insert(ind, data, key, parent, parent_view_index, bexpanded, nimage);
	}

	//update�ȽϺ���Ϊ���Ա����
	template<typename _DataType, class _Class, typename _pMemberFunc> 
	inline int update(_DataType * data, _Key key, _Class * pClass, _pMemberFunc pMemberFunc, TNode<_Type> * parent = nullptr, size_t parent_view_index = 0, bool bexpanded = false, int nimage = -1)			
	{
		ACCallback<PCommpareFun, _Class, _pMemberFunc> sort_commpare(pClass, pMemberFunc);
		return update(data, key, (PCommpareFun)sort_commpare, parent, parent_view_index, bexpanded, nimage);
	}
	//ԭ���ݲ����������ӣ���������£������������ݵ�����λ��
	//key:data��Ψһ�ؼ��֣�sort_commpare����ȽϺ���
	template<typename _DataType>
	inline int update(_DataType * data, _Key key, PCommpareFun sort_commpare, TNode<_Type> * parent = nullptr, size_t parent_view_index = 0, bool bexpanded = false, int nimage = -1)
	{
		TNode<_Type> * node = find_by_key(key);

		if (node==nullptr)
			return add(data, key, sort_commpare, parent, parent_view_index, bexpanded, nimage);
		else
		{
			if (m_grid)
				m_grid->redraw();				//��ʱ�����ݸ��º���ʱ���������� ��ǰû�������ݣ�û�����ϵ�����
			return -2;

			int ind = -2;
			bool order_chage = sort_commpare(node->get_data(), (_Type *)data)!=0; 

			if (order_chage || parent==nullptr || parent->is_expanded())			//˳��δ�䣬Ҳû��ʾ��ֻ��������ݣ�����ˢ�±��,����
			{
				ind = find(node->get_data(), sort_commpare, parent);				//����������λ�ã� ��Ϊ��ˢ�µ�ǰ�ж�����һ�£�����ֱ��ˢ��������(�ɼ���)���ĸ�����Ч��
				
				if (order_chage)				//ɾ�ϵģ����µ�
				{				
					int old_ind = ind;
					ind = find((_Type*)data, sort_commpare, parent);				//��λ��λ��					
					node->set_data_value(data);										//��������
					moveto(old_ind, ind, parent, parent_view_index);
				}
				else							//����ʾ��ˢ��
				{
					node->set_data_value(data);										//��������
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
	
	//remove�ȽϺ���Ϊ���Ա����
	template<typename _DataType, class _Class, typename _pMemberFunc> 
	inline int remove(_DataType * data, _Key key, _Class * pClass, _pMemberFunc pMemberFunc, TNode<_Type> * parent = nullptr, size_t parent_view_index = 0)			
	{
		ACCallback<PCommpareFun, _Class, _pMemberFunc> sort_commpare(pClass, pMemberFunc);
		return remove(data, key, (PCommpareFun)sort_commpare, parent, parent_view_index);
	}
	//ɾ��һ������
	template<typename _DataType>
	inline int remove(_DataType * data, _Key key, PCommpareFun sort_commpare, TNode<_Type> * parent = nullptr, size_t parent_view_index = 0)
	{
		TNode<_Type> * node = find_by_key(key, true);

		if (node == nullptr)
			return -1;

		//TODO: findʱ������ֱ��ʹ��˳����ҵĲ��� //order_find��ʱ����˳�����
		int ind = order_find_ex(data, 0, true);//find(node->get_data(), sort_commpare, parent);//��ǰû�������ݣ�û�����ϵ�����
		ASSERT(ind>=0);
		if (ind < 0)
			return -1;
		
		//ɾ�������ʾ�У��Ⱥϲ����ڵ㣬Ȼ��ɾ��֮,��Ҫ���±������
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

		//ɾ���洢������
		if (parent)
			parent->remove(ind, m_pool);		
		else
			node->_clear(m_pool);

		
		//�ӽڵ�Ĺؼ�������û��ɾ����ֻ��������ӽڵ��е����ݣ�����������find_by_key��������
		
		return ind;
	}
	//���ĳ���ڵ����ʾλ��
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
	//��ñ����ʾ�к�
	inline int get_view_index(size_t child_index, TNode<_Type> * parent = nullptr, size_t parent_view_index = 0)
	{
		if (parent == nullptr)
			return child_index;
		else if (child_index >=0 && parent->is_expanded())	
			return parent_view_index + child_index + 1;

		return -1;
	}

	//remove�ȽϺ���Ϊ���Ա����
	template<class _Class, typename _pMemberFunc> 
	inline void sort(_Class * pClass, _pMemberFunc pMemberFunc)			
	{
		ACCallback<PCommpareFun, _Class, _pMemberFunc> sort_commpare(pClass, pMemberFunc);
		sort((PCommpareFun)sort_commpare);
	}

	//����,����ʱ���Բ��źϲ��˵���Ŀ���ȵ������Ŀչ��ʱ�����򣬺ϲ���Ƚ϶��ʱ����Լӿ��ٶȣ�������Ҫ�����ϴ�����ĺ���
	//���������Ǹ��ⲿ���������нڵ���Ҫչ��ʱ���������ڵ����п����Ѿ���������,�������������Ǻܺ���
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