//��  �ܣ����Զ�������ѭ������ʵ�ֵı䳤�����ڴ��
//�����ˣ�������
#pragma once

#include <windows.h> 
#include <crtdbg.h> 
#include <malloc.h> 


//�ڴ�أ����Զ�������ѭ������ʵ��
//1.��ѭ�����е���ʽ�����С���ȵ��ڴ�飬ʡȥ���ڴ��Ƶ���ƶ���������ͷ�
//�ڶ�βд��(push)���ݣ��ڶ�ͷ����(front)��ɾ��(pop)����
//ÿ��������ǰΪһ����������ʾ�ñ����ݶεĳ���leng(��������������ĳ���)�����leng�ֽ�Ϊ�ñ����ݵ�����
//2.��˳������������Ҫ����ָ��ʱ���ڵ��ظ����ݰ����������һ��Map,���ظ������ݰ��ĵ�ַ���ؼ��ִ�Ž�ȥ��
//��������ظ��򸲸�ԭ��������,��������ͱ���˿�������������ݿ�����ݳأ���Ҳ����ʹ���ݰ�ʧȥʱ����
class MemQueue
{
private:
	enum TAction
	{
		aNone	= 0,
		aPop	= 1,
		aPush	= 2
	};
	TAction	m_last_action;													//���һ�β�������
private:
	char * 	m_pmem;															//���л���
	int 	m_size;															//�����С
	int 	m_front;														//ͷ
	int 	m_rear;															//β

	int 	m_min_size;														//��С�ռ�
	int 	m_diff_space;													//�ӳ��ͷŵļ���
	int 	m_diff_check_count;												//�ӳٶ��ٸ������ͷŶ���ռ�
	UINT 	m_diff_time;													//�ռ���С��ʱ
	UINT 	m_diff_tick;													//�ϴοռ���С�ж�ʱ��
	bool 	m_deflate_active;												//�Ƿ������ڴ���С����,��д��״̬�л�������״̬ʱִ�У������Ķ���ʱ��ִ�У�����������Ϊ˫����ķ���

	int *	p_head;															//����һ�����ݰ������ݳ��ȵ�ָ�룬ͬһ�����ݶ��д��ʱʹ��

	//���Ӵ洢�ռ�
	void inflate(int new_size)
	{
		int _count = count();
		new_size = max(new_size, m_min_size);
		if (new_size <= m_size)
			return;

		void * pmem = nullptr;
		if (m_front < m_rear || _count == 0)								//����β��ʣ�� �� �ջ���
		{
			if (m_size - _count >= new_size - m_rear)						//new_size-m_rear=item_size��β�����㣬���ܿ��ÿռ��㹻���򲻸ı��С���������Ƶ���ǰ�棬��ʹ���ÿռ伯�е�β��
			{
				memmove_s(m_pmem, _count, m_pmem + m_front, _count);		//�ƶ����ݵ���ǰ��
				m_front = 0;												//�ƶ�ͷָ�뵽��λ��
				m_rear  = _count;											//�ƶ�βָ�뵽��λ��
			}
			else
			{
				pmem = realloc(m_pmem, new_size);
				_ASSERT(pmem > 0);
			}
		}
		else if (m_front >= m_rear)											//�����м�ʣ��
		{
			int offset = new_size - m_size;
			int move_count = m_size - m_front;
			pmem = realloc(m_pmem, new_size);								//���ӿռ䣬��β��ԭ�����ݺ���
			_ASSERT(pmem > 0);
			memmove_s((char *)pmem + m_front + offset, move_count, (char *)pmem + m_front, move_count);

			m_front += offset;												//�ƶ�ͷָ�뵽��λ��
		}

		if (pmem)
		{
			m_size = new_size;												//���»����ܳ���
			m_pmem = (char *)pmem;											//���»�������ֵ
		}
	}
	//�����洢�ռ�
	void deflate(int new_size)
	{
		int _count = count();
		if (new_size >= m_size || new_size <= _count || new_size <= m_min_size)
			return;
		int offset = new_size - m_size;
		void * pmem = nullptr;
		if (m_front < m_rear || _count == 0)								//����β��ʣ�� �� �ջ���
		{
			if (-offset > m_size - m_rear)
			{
				memmove_s(m_pmem, _count, m_pmem + m_front, _count);		//�ƶ����ݵ���ǰ��
				m_front = 0;												//�ƶ�ͷָ�뵽��λ��
				m_rear = _count;											//�ƶ�βָ�뵽��λ��
			}
			pmem = realloc(m_pmem, new_size);
			_ASSERT(pmem > 0);
		}
		else if (m_front > m_rear)											//�����м�ʣ��
		{
			int move_count = m_size - m_front;
			memmove_s(m_pmem + m_front + offset, move_count, m_pmem + m_front, move_count);
			pmem = realloc(m_pmem, new_size);								//��β��ԭ���������ƣ���С�ռ�
			_ASSERT(pmem > 0);

			m_front += offset;												//�ƶ�ͷָ�뵽��λ��
		}

		if (pmem)
		{
			m_size = new_size;												//���»����ܳ���
			m_pmem = (char *)pmem;											//���»�������ֵ
		}
	}
	//����Ƿ���Ҫ���ӻ���С�ڴ�ռ䣬��ǿ�����������ж�һ���Ƿ���Ҫ��С�ڴ�ռ�
	void check_grow(int item_size, bool is_push)
	{
		int _count = count();
		int leng = m_size;
		//�������ռ䲻��ʱ���ӿռ䣬��ָ������
		if (is_push)
		{
			int dx = 0;
			if (m_size == 0)
				dx = 4 * item_size;											//��ʼ����
			else if ((m_front < m_rear || _count == 0) &&
				m_size - m_rear < item_size)								//����β��ʣ�࣬��ʣ��ռ䲻����ʹ������β������
				dx = item_size - (m_size - m_rear);
			else if (m_size - _count < item_size)							//���������򻺴��в�ʣ�࣬��ʣ��ռ䲻�������±������ӻ��泤�� _count >= m_size || 
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
				m_diff_space = m_diff_check_count;							//�����ͷż�����ʼֵ(�����жϵĴ���)
				m_diff_tick = GetTickCount();
			}
		}
		//��������ռ����ʱ���ٿռ䣬��ʱ����
		else if (m_deflate_active && leng > m_min_size)
		{			
			//�������±�������Ҫ��С�����ݴ�С
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
				m_diff_space = m_diff_check_count;							//�����ͷż�����ʼֵ(�����жϵĴ���)
				return;
			}

			m_diff_space--;													//�����жϵĴ���

			if (dx_num < null_num && m_diff_space == 0 && GetTickCount() - m_diff_tick > m_diff_time)
			{
				deflate(_count + dx_num * item_size);						//�ͷŶ���ռ�
				m_diff_space = m_diff_check_count;							//�����ͷż�����ʼֵ(�����жϵĴ���)
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

	//��ʹ�õĿռ��С
	inline int count()
	{
		if (m_rear > m_front)			
			return m_rear - m_front;
		else if (m_rear < m_front)		
			return m_size - (m_front - m_rear);
		else 	
			return m_last_action != aPush ? 0 : m_size;
	}
	
	//����������(���),�Ժ���Խ���ȡ��д��ռ���ύд�����ݵĿռ�ֿ�����֧�ָ��õĲ��в���
	void push(const void * buf, int buf_size)
	{
		check_grow(buf_size + sizeof(buf_size), true);

		//int leng = m_size - max(count(), m_rear);
		//_ASSERT(leng >= buf_size + (int)sizeof(buf_size));

		p_head = (int *)(m_pmem + m_rear);
		*p_head = buf_size;													//д�������򳤶�
		m_rear += sizeof(buf_size);
		memcpy_s(m_pmem + m_rear, buf_size, buf, buf_size);					//д������������

		m_rear = (m_rear + buf_size) % m_size;								//�ƶ�βָ�뵽�µ�д��λ��
		m_last_action = aPush;

		_ASSERT(count() <= m_size);
	}

	//��ȡ��һ�������򣺳ɹ��򷵻�����ָ�룬buf_sizeΪ��ȡ�����ݳ��ȣ����򷵻ؿ�ָ����buf_size=0
	char * front(int & buf_size)
	{
		if (empty())
		{
			buf_size = 0;
			return nullptr;
		}

		buf_size = *(int *)(m_pmem + m_front);								//��ȡ�����򳤶�
		return m_pmem + m_front + sizeof(buf_size);							//��������ͷָ��
	}

	//ɾ����һ��������ͷָ��ָ����һ������
	void pop()
	{
		if (empty())
			return;

		//��Ϊ�ڴ��ֽ�˳���ԭ�򣬲���д��buf_size=*(m_pmem+m_front)
		int buf_size = *(int *)(m_pmem + m_front);							//��ȡ�����򳤶ȣ�
		m_front = (m_front + buf_size + sizeof(buf_size)) % m_size;			//�ƶ�ͷָ�뵽��һ������
		m_last_action = aPop;

		_ASSERT(count() >= 0);

		check_grow(buf_size + sizeof(buf_size), false);
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////	
	///����push����ʱ��Ҫ�ⲿ�Ƚ�С����ƴ��һ���������ڴ�飬Ȼ����ͨ��push����������ڴ�鿽�������ﱣ�� ///
	///Ϊ���������ƴ���Ĵ���Ч�ʣ����������������������ⲿ��С����ֱ��ƴ�����������һ�δ��ڴ�Ŀ���   ///
	///ÿ������ƴ��ǰ�ȵ���һ��write_head��֮��ɵ��ö��write_data�����һ�����ݵ�д��                   ///
	///���̵߳���ʱwrite_head��write_data����λ��ͬһ���������÷�Χ֮��                                   ///
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	//��ʼд�����ݣ�����Ԥ����һ��size��С���ڴ�
	void write_head(const int size)
	{		
		check_grow(size + sizeof(size), true);								//����Ԥ������һ������ڴ�

		p_head = (int *)(m_pmem + m_rear);									//��¼��ͷ��ַ
		*p_head = 0;														//��ʼ�����򳤶�

		m_rear += sizeof(size);												//�ƶ�βָ�뵽�µ�д��λ��
		m_last_action = aPush;
		
		_ASSERT(count() <= m_size);
	}
	//׷������
	void write_data(const void * buf, int buf_size)
	{						
		int leng = m_size - max(count(), m_rear);
		if (leng < buf_size)												//������ڴ治�����ˣ���Ҫ׷���ڴ�
			check_grow(buf_size + sizeof(buf_size), true);

		*p_head = *p_head + buf_size;										//���������򳤶�
		memcpy_s(m_pmem + m_rear, buf_size, buf, buf_size);					//д������������

		m_rear = (m_rear + buf_size) % m_size;								//�ƶ�βָ�뵽�µ�д��λ��
		m_last_action = aPush;

		_ASSERT(count() <= m_size);
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void * malloc_buf(const size_t size)
	{
		check_grow(size + sizeof(size), true);								//����Ԥ������һ������ڴ�

		p_head = (int *)(m_pmem + m_rear);									//��¼��ͷ��ַ
		*p_head = 0;														//��ʼ�����򳤶�

		m_rear += sizeof(size);												//�ƶ�βָ�뵽�µ�д��λ��
		m_last_action = aPush;
		
		_ASSERT(count() <= m_size);
	}
};