//功  能：用自动伸缩的循环队列实现的变长数据内存池
//创建人：刘明祥
#pragma once

#include <windows.h> 
#include <crtdbg.h> 
#include <malloc.h> 


//内存池：用自动伸缩的循环队列实现
//1.以循环队列的形式保存大小不等的内存块，省去了内存的频繁移动、申请和释放
//在队尾写入(push)数据，在队头读出(front)或删除(pop)数据
//每个数据域前为一个整数，表示该笔数据段的长度leng(不包括这个整数的长度)，其后leng字节为该笔数据的内容
//2.按顺序逐包处理；如果要丢弃指定时间内的重复数据包则可再引入一个Map,将重复的数据包的地址按关键字存放进去，
//如果出现重复则覆盖原来的数据,这样该类就变成了可以随机访问数据块的数据池，但也会致使数据包失去时序性
class MemQueue
{
private:
	enum TAction
	{
		aNone	= 0,
		aPop	= 1,
		aPush	= 2
	};
	TAction	m_last_action;													//最后一次操作类型
private:
	char * 	m_pmem;															//队列缓存
	int 	m_size;															//缓存大小
	int 	m_front;														//头
	int 	m_rear;															//尾

	int 	m_min_size;														//最小空间
	int 	m_diff_space;													//延迟释放的计数
	int 	m_diff_check_count;												//延迟多少个包再释放多余空间
	UINT 	m_diff_time;													//空间缩小延时
	UINT 	m_diff_tick;													//上次空间缩小判断时间
	bool 	m_deflate_active;												//是否启用内存缩小策略,从写入状态切换到读出状态时执行，连续的读出时不执行，兼容用其作为双缓冲的方法

	int *	p_head;															//最新一个数据包的数据长度的指针，同一包数据多次写入时使用

	//增加存储空间
	void inflate(int new_size)
	{
		int _count = count();
		new_size = max(new_size, m_min_size);
		if (new_size <= m_size)
			return;

		void * pmem = nullptr;
		if (m_front < m_rear || _count == 0)								//缓存尾部剩余 或 空缓存
		{
			if (m_size - _count >= new_size - m_rear)						//new_size-m_rear=item_size。尾部不足，但总可用空间足够，则不改变大小，将数据移到最前面，以使可用空间集中到尾部
			{
				memmove_s(m_pmem, _count, m_pmem + m_front, _count);		//移动数据到最前面
				m_front = 0;												//移动头指针到新位置
				m_rear  = _count;											//移动尾指针到新位置
			}
			else
			{
				pmem = realloc(m_pmem, new_size);
				_ASSERT(pmem > 0);
			}
		}
		else if (m_front >= m_rear)											//缓存中间剩余
		{
			int offset = new_size - m_size;
			int move_count = m_size - m_front;
			pmem = realloc(m_pmem, new_size);								//增加空间，将尾部原有数据后移
			_ASSERT(pmem > 0);
			memmove_s((char *)pmem + m_front + offset, move_count, (char *)pmem + m_front, move_count);

			m_front += offset;												//移动头指针到新位置
		}

		if (pmem)
		{
			m_size = new_size;												//更新缓存总长度
			m_pmem = (char *)pmem;											//更新缓存区地值
		}
	}
	//缩减存储空间
	void deflate(int new_size)
	{
		int _count = count();
		if (new_size >= m_size || new_size <= _count || new_size <= m_min_size)
			return;
		int offset = new_size - m_size;
		void * pmem = nullptr;
		if (m_front < m_rear || _count == 0)								//缓存尾部剩余 或 空缓存
		{
			if (-offset > m_size - m_rear)
			{
				memmove_s(m_pmem, _count, m_pmem + m_front, _count);		//移动数据到最前面
				m_front = 0;												//移动头指针到新位置
				m_rear = _count;											//移动尾指针到新位置
			}
			pmem = realloc(m_pmem, new_size);
			_ASSERT(pmem > 0);
		}
		else if (m_front > m_rear)											//缓存中间剩余
		{
			int move_count = m_size - m_front;
			memmove_s(m_pmem + m_front + offset, move_count, m_pmem + m_front, move_count);
			pmem = realloc(m_pmem, new_size);								//将尾部原有数据上移，缩小空间
			_ASSERT(pmem > 0);

			m_front += offset;												//移动头指针到新位置
		}

		if (pmem)
		{
			m_size = new_size;												//更新缓存总长度
			m_pmem = (char *)pmem;											//更新缓存区地值
		}
	}
	//检查是否需要增加或缩小内存空间，增强：间隔半分钟判断一次是否需要缩小内存空间
	void check_grow(int item_size, bool is_push)
	{
		int _count = count();
		int leng = m_size;
		//入队则检查空间不足时增加空间，按指数增加
		if (is_push)
		{
			int dx = 0;
			if (m_size == 0)
				dx = 4 * item_size;											//初始长度
			else if ((m_front < m_rear || _count == 0) &&
				m_size - m_rear < item_size)								//缓存尾部剩余，但剩余空间不足则使缓冲区尾部补齐
				dx = item_size - (m_size - m_rear);
			else if (m_size - _count < item_size)							//队列已满或缓存中部剩余，但剩余空间不足则按以下比例增加缓存长度 _count >= m_size || 
			{
				int num = leng / item_size;
				if (num > 64)
					dx = leng / 4;
				else if (num > 8)
					dx = 16 * item_size;
				else
					dx = 4 * item_size;
			}

			if (dx > 0)
			{
				inflate(leng + dx);
				m_diff_space = m_diff_check_count;							//设置释放计数初始值(连续判断的次数)
				m_diff_tick = GetTickCount();
			}
		}
		//出队则检查空间过多时减少空间，延时减少
		else if (m_deflate_active && leng > m_min_size)
		{			
			//根据以下比例计算要缩小的数据大小
			int null_num = (leng - _count) / item_size;
			int full_num = (_count / item_size);
			if (0 == full_num || 0 == null_num)
				return;
			int dx_num = 0;
			if (full_num > 64)
				dx_num = full_num / 4;
			else if (full_num > 8)
				dx_num = 16;
			else
				dx_num = 4;

			if (null_num / full_num > 2)
				dx_num = full_num;

			if (dx_num >= null_num)
			{
				m_diff_space = m_diff_check_count;							//设置释放计数初始值(连续判断的次数)
				return;
			}

			m_diff_space--;													//连续判断的次数

			if (dx_num < null_num && m_diff_space == 0 && GetTickCount() - m_diff_tick > m_diff_time)
			{
				deflate(_count + dx_num * item_size);						//释放多余空间
				m_diff_space = m_diff_check_count;							//设置释放计数初始值(连续判断的次数)
				m_diff_tick = GetTickCount();
			}
		}
				
		m_deflate_active = is_push;
	}
public:
	MemQueue(int min_size = 1024 * 16, int diff_check_count = 5, uint diff_time = 30 * 1000) 
		: m_front(0)
		, m_rear(0)
		, m_size(0)
		, m_pmem(nullptr)
		, m_min_size(min_size)
		, m_deflate_active(false)
		, m_diff_time(diff_time)
		, m_diff_tick(GetTickCount())
		, m_diff_space(diff_check_count)
		, m_diff_check_count(diff_check_count)
		, p_head(nullptr)
		, m_last_action(aNone)
	{}
	virtual ~MemQueue()
	{
		if (m_pmem)
			free(m_pmem);
	}

	inline bool empty(){	return m_rear == m_front && m_last_action != aPush;	}	

	//已使用的空间大小
	inline int count()
	{
		if (m_rear > m_front)			
			return m_rear - m_front;
		else if (m_rear < m_front)		
			return m_size - (m_front - m_rear);
		else 	
			return m_last_action != aPush ? 0 : m_size;
	}
	
	//加入新数据(入队),以后可以将获取待写入空间和提交写好数据的空间分开，以支持更好的并行操作
	void push(const void * buf, int buf_size)
	{
		check_grow(buf_size + sizeof(buf_size), true);

		//int leng = m_size - max(count(), m_rear);
		//_ASSERT(leng >= buf_size + (int)sizeof(buf_size));

		p_head = (int *)(m_pmem + m_rear);
		*p_head = buf_size;													//写入数据域长度
		m_rear += sizeof(buf_size);
		memcpy_s(m_pmem + m_rear, buf_size, buf, buf_size);					//写入数据域内容

		m_rear = (m_rear + buf_size) % m_size;								//移动尾指针到新的写入位置
		m_last_action = aPush;

		_ASSERT(count() <= m_size);
	}

	//读取下一个数据域：成功则返回数据指针，buf_size为获取的数据长度；否则返回空指针且buf_size=0
	char * front(int & buf_size)
	{
		if (empty())
		{
			buf_size = 0;
			return nullptr;
		}

		buf_size = *(int *)(m_pmem + m_front);								//读取数据域长度
		return m_pmem + m_front + sizeof(buf_size);							//返回数据头指针
	}

	//删除第一个数据域，头指针指向下一数据域
	void pop()
	{
		if (empty())
			return;

		//因为内存字节顺序的原因，不能写成buf_size=*(m_pmem+m_front)
		int buf_size = *(int *)(m_pmem + m_front);							//读取数据域长度，
		m_front = (m_front + buf_size + sizeof(buf_size)) % m_size;			//移动头指针到下一数据域
		m_last_action = aPop;

		_ASSERT(count() >= 0);

		check_grow(buf_size + sizeof(buf_size), false);
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////	
	///调用push函数时需要外部先将小数据拼成一个完整的内存块，然后再通过push函数将这个内存块拷贝到这里保存 ///
	///为了提高数据拼包的处理效率，增下了面两个函数，将外部的小数据直接拼到这里，避免了一次大内存的拷贝   ///
	///每次数据拼包前先调用一次write_head，之后可调用多次write_data来完成一包数据的写入                   ///
	///多线程调用时write_head和write_data必须位于同一个锁的作用范围之内                                   ///
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	//开始写入数据，可以预申请一块size大小的内存
	void write_head(const int size)
	{		
		check_grow(size + sizeof(size), true);								//可以预先申请一块大点的内存

		p_head = (int *)(m_pmem + m_rear);									//记录包头地址
		*p_head = 0;														//初始数据域长度

		m_rear += sizeof(size);												//移动尾指针到新的写入位置
		m_last_action = aPush;
		
		_ASSERT(count() <= m_size);
	}
	//追加数据
	void write_data(const void * buf, int buf_size)
	{						
		int leng = m_size - max(count(), m_rear);
		if (leng < buf_size)												//申请的内存不够用了，需要追加内存
			check_grow(buf_size + sizeof(buf_size), true);

		*p_head = *p_head + buf_size;										//更新数据域长度
		memcpy_s(m_pmem + m_rear, buf_size, buf, buf_size);					//写入数据域内容

		m_rear = (m_rear + buf_size) % m_size;								//移动尾指针到新的写入位置
		m_last_action = aPush;

		_ASSERT(count() <= m_size);
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void * malloc_buf(const size_t size)
	{
		check_grow(size + sizeof(size), true);								//可以预先申请一块大点的内存

		p_head = (int *)(m_pmem + m_rear);									//记录包头地址
		*p_head = 0;														//初始数据域长度

		m_rear += sizeof(size);												//移动尾指针到新的写入位置
		m_last_action = aPush;
		
		_ASSERT(count() <= m_size);
	}
};