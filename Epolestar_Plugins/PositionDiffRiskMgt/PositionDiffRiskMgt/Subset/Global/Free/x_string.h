//功能:	ASCII字符串操作类，功能类似std::string，比std::string结构简单,便于扩展，增加comparei()、trim()、format等函数
//创建:	2013-01-05 刘明祥
//修改:	2013-01-14 刘明祥 增加 _str()
//		2013-01-15 刘明祥 修正 replace(), rfind(); 增加 operator(+,+=,>,>=,<,<=), String(const char *)
//在VS2012上实验，证实两个同类型的结构体之间相互赋值时，其成员的赋值会自动调用成员所属类的等号操作符（所以成员所属类必须支持等号操作符），否则可能导致不可预期的结果

#pragma once

#include "stdarg.h"
#include <string>

#define max(a, b) ((a)>(b) ? (a) : (b))
#define min(a, b) ((a)<(b) ? (a) : (b))

struct string_buffer
{
	size_t	size;			//长度
	size_t	capacity;		//容量
	char	buffer[1];		//正文
};

class String
{
private:
	string_buffer * pbuf;

	//地址空间内
	char * _str(const size_t & i)
	{
		return i <= max_size() ? &(pbuf->buffer[i]) : NULL;
	}
	//字符串长度内
	char * c_str(const size_t & i)
	{
		return i<length() ? &(pbuf->buffer[i]) : NULL;
	}
	String& copy_of(const char * sour_str, size_t dest_begin = 0, size_t sour_begin = 0, size_t count = SIZE_MAX)
	{
		if (sour_str==NULL)
			return *this;

		size_t len = strlen(sour_str);
		if (sour_begin >= len)
			return *this;
		count = min(count, len - sour_begin);
		
		int sour_ind = sour_str - c_str(0);
		bool bthis = sour_ind>=0 && sour_str <= c_str(length() - 1);						//判断 sour_str 是否指向本字符串空间
		resize(count + dest_begin);
		if (bthis)
			sour_str = c_str(sour_ind);
		memmove_s(c_str(dest_begin), length() - dest_begin, &(sour_str[sour_begin]), count);//strncpy_s

		_ASSERT(c_str(length())==0);

		return *this;
	}

	enum tirm_type
	{
		tm_left,
		tm_right,
		tm_both
	};
	String& trim(const tirm_type & type) 
	{
		size_t i = 0, len = length();

		if (type != tm_right)
			while(i < len && at(i) <= 32) i++;

		if (i>=len) 
			return clear();
		else if (type != tm_left)
			while(at(len-1) <= 32) len--;

		if (i==0 && len == length())
			return *this;
		else
		{
			memmove_s(c_str(0), length(), c_str(i), len -i);
			return resize(len-i);
		}
	}
	int __strncmp(const char * str1, const char * str2, size_t n1, size_t n2, bool _case)//_case:是否区分大小写
	{
		if (str1==NULL || n1==0)
			return -1;
		else if (str2==NULL  || n2==0)
			return 1;
		
		size_t n = min(n1, n2);
		int _res = 0;
		if (_case)
			_res = _strnicmp(str1, str2, n);
		else
			_res = strncmp(str1, str2, n);

		if (_res == 0)
		{
			if (str1[n] == str2[n] && str1[n]==0)
				return 0;
			else if (str1[n]==0)
				return -1;
			else if (str2[n]==0)
				return 1;
		}
		else 
			return _res;
	}
public:
	size_t length()
	{
		if (pbuf==NULL)
			return 0;
		else
			return pbuf->size;
	};
	const char * c_str()
	{
		return _str(0);
	}
	const char at(const size_t & i)
	{
		return (i<length()) ? pbuf->buffer[i] : 0;
	}
	String& append(const char * str, size_t index = 0, size_t len = SIZE_MAX)
	{
		return copy_of(str, length(), index, len);		 
	};
	String& append(String& str, size_t index = 0, size_t len = SIZE_MAX)
	{
		return append(str.c_str(), index, len);		 
	};
	String& append(size_t num, char ch)
	{
		return resize(length()+num, ch);		 
	};
	String& append_format(const char *fmt, ...)
	{
		va_list ap;
		va_start(ap, fmt);

		int size = _vscprintf(fmt, ap);
		if (size > 0)
		{
			char * buffer = (char *)malloc(size + 1);
			buffer[0] = 0;
			if (_vsnprintf_s(buffer, size + 1, size, fmt, ap) > 0)
				append(buffer);
			free(buffer);
		}		

		va_end(ap);

		return *this;
	}
	String& format(const char *fmt, ...)
	{
		va_list ap;
		va_start(ap, fmt);

		int size = _vscprintf(fmt, ap);
		if (size > 0)
		{
			char * buffer = (char *)malloc(size + 1);
			buffer[0] = 0;
			if (_vsnprintf_s(buffer, size + 1, size, fmt, ap) > 0)
				copy_of(buffer);
			free(buffer);
		}

		va_end(ap);

		return *this;
	}
	String& assign(String& str)
	{
		return assign(str.c_str());
	}
	String& assign(const char * str, size_t index = 0, size_t len = SIZE_MAX)
	{
		return copy_of(str, 0, index, len);
	}
	String& assign(size_t num, char ch)
	{
		resize(num);
		if (num)
			memset(c_str(0), ch, num);
		return *this;
	}
	String& clear()
	{
		return resize(0);
	}
	int find(const char * sub, size_t index=0, size_t num=SIZE_MAX)
	{
		if (!c_str(index))
			return -1;

		char *p = strstr(c_str(index), sub);
		if (p)
			return (size_t)(p - c_str(0)) > index + num ? -1 : p - c_str(0);
		else
			return -1;
	}
	int rfind(const char * sub, size_t index=SIZE_MAX, size_t num=SIZE_MAX)
	{
		if (!c_str(index))
			return -1;

		size_t len = strlen(sub);
		index = index + len -1;
		index = min(index, length() - 1);	//开始查找位置

		num = min(num, index+1);			//在index前num个字符数中查找

		const char * p = c_str(index);		
		const char * p1;

		const char * q;						//字串

		size_t m;							//m == 0 是否找到
		size_t n;
		while(num-- && *p )					//反向查找
		{		
			q = sub + len - 1;
			if( *p-- == *q-- )
			{
				p1 = p;
				for (p1 = p, m = len - 1, n = num; m && n && *p1-- == *q--; m--, n--);
				_ASSERT(m >= 0); 
				if( m == 0 )											//m == 0 是否找到
					return ++p1 - c_str(0);
			}
		}

		return -1;
	}
	int find_first_of(const char * sub, size_t index=0, size_t num=SIZE_MAX)
	{
		const char * p = c_str(index);
		if (!p)
			return -1;
		
		for (num = min(length() - index, num); num && !strchr(sub, *p); num--, p++);
		if (num == 0)
			return -1;
		else
			return p - c_str(0);
	}
	int find_first_not_of(const char * sub, size_t index=0, size_t num=SIZE_MAX)
	{		
		const char * p = c_str(index);
		if (!p)
			return -1;

		for (num = min(length() - index, num); num && strchr(sub, *p); num--, p++);
		if (num == 0)
			return -1;
		else
			return p - c_str(0);
	}
	int find_last_of(const char * sub, size_t index=SIZE_MAX, size_t num=SIZE_MAX)
	{
		const char * p = c_str(index);
		if (!p)
			return -1;

		for (num = min(index + 1, num); num && !strrchr(sub, *p); num--, p--);
		if (num == 0)
			return -1;
		else
			return p - c_str(0);
	}
	int find_last_not_of(const char * sub, size_t index=SIZE_MAX, size_t num=SIZE_MAX)
	{
		const char * p = c_str(index);
		if (!p)
			return -1;

		for (num = min(index + 1, num); num && strrchr(sub, *p); num--, p--);
		if (num <= 0)
			return -1;
		else
			return p - c_str(0);
	}
	String& trim()
	{
		return trim(tm_both);
	}
	String& trim_left()
	{		
		return trim(tm_left);
	}
	String& trim_right()
	{
		return trim(tm_right);
	}
	size_t copy_to(char * dest_str, size_t dest_len, size_t dest_begin=0, size_t sour_begin=0, size_t count=SIZE_MAX)
	{
		if (dest_len <= dest_begin)
			return 0;
		else
			return strncpy_s(dest_str + dest_begin, dest_len - dest_begin, c_str(sour_begin), count);
	}
	String copy_left(size_t count)
	{
		return String(*this, 0, count);
	}
	String copy_right(size_t count)
	{
		size_t index = 0;
		if (length() > count)
			index = length() - count;
		return String(*this, index, count);
	}	
	//str	:比较字符串
	//index	:从自己的index位置开始比较
	//leng	:自己的字符串长度
	//index2:从str的index2位置开始比较
	//leng2	:比较的字符串长度
	int compare(const char * str, size_t index=0, size_t leng = SIZE_MAX, size_t index2=0, size_t leng2 = SIZE_MAX)
	{
		return __strncmp(c_str(index), str+index2, leng, leng2, false);
	}
	//不区分大小写的字符串比较
	int comparei(const char * str, size_t index=0, size_t leng = SIZE_MAX, size_t index2=0, size_t leng2 = SIZE_MAX)
	{
		return __strncmp(c_str(index), str+index2, leng, leng2, true);
	}
	bool empty()
	{
		return length()==0;
	}
	String& erase(size_t index, size_t count)
	{
		if (length() > index)
		{
			if (length() <= index + count)
				resize(index);
			else
			{
				memmove_s(c_str(index), length() - index, c_str(index + count), length() - index - count);
				resize(length() - count);
			}
		}
		return *this;
	}
	String& insert(const char * str, size_t index, size_t index2=0, size_t count=SIZE_MAX)
	{
		size_t old_len = length();
		size_t len = strlen(str);
		if (index2 >= len)
			return *this;

		count = min(len - index2, count);

		resize(count + old_len);
		memcpy_s(c_str(index + count), old_len - index, c_str(index), old_len - index);
		memcpy_s(c_str(index), count, str + index2, count);

		_ASSERT(c_str(length())==0);
		return *this;
	}
	String& replace(size_t index, size_t leng, const char * str, size_t index2, size_t leng2)
	{
		size_t old_len = length();
		size_t len = strlen(str);
		if (index2 >= len || index >= old_len)
			return *this;

		leng  = min(old_len - index, leng);
		leng2 = min(len - index2, leng2);
		if (leng2 > leng)
			resize(leng2 - leng + old_len);
		memcpy_s(c_str(index + leng2), length() - index - leng, c_str(index), leng);
		memcpy_s(c_str(index), leng2, str + index2, leng2);
		
		_ASSERT(c_str(length())==0);
		return *this;
	}
	String& replace(char * target_str, char * replace_str)
	{
		if (c_str(0)==NULL || target_str==NULL || replace_str==NULL)
			return *this;
		if (strcmp(target_str, replace_str)==0)
			return *this;

		size_t len_target	= strlen(target_str); 
		size_t len_replace	= strlen(replace_str);
		if(length() < len_target || len_target <= 0)
			return *this;

		size_t MAX_REPLACE_COUNT = 30;	//每次扩充容量时增加的替换个数

		size_t curr_ind;				//当前写入位置
		char *p_write;					//写入指针
		char *p1;						//匹配指针1
		char *p2;						//匹配指针2
		char *p_replace;				//替换串

		size_t len_read = length();
		
		char * p_tmp	= NULL;			//读取指针
		char * p_read	= c_str(0);		//读取指针
		if (len_replace > len_target)
		{
			p_tmp = (char*)malloc(length()+1);							
			copy_to(p_tmp, length()+1);
			p_read = p_tmp;
		}

		size_t n = 0;
		p_write = c_str(0);
		while( *p_read )
		{		
			p2 = target_str;
			if( *p_read == *p2++ )
			{
				p1  = p_read+1;
				while( (*p1++ == *p2++) && *p2 );									//查找

				if( *p2 == '\0' )													//*p2 == '\0' 是否找到
				{
					if (len_replace > len_target && n <= 0)							//容量是否足够
					{
						n = MAX_REPLACE_COUNT;

						curr_ind = p_write - c_str(0);					
						resize(length() + (len_replace - len_target) * n);
						p_write = c_str(curr_ind);
					}

					for( p_replace = replace_str; *p_replace; p_replace++ )			//替换
						*p_write++ = *p_replace;
					p_read += len_target;

					n--;

					continue;
				}
			}
			if (len_replace != len_target)
				*p_write = *p_read;	
			p_write++; p_read++;
		}	

		if (p_tmp)
			free(p_tmp);

		return resize(p_write - c_str(0));
	}
	String& swap(String& str)
	{
		String tmp(str);
		str = *this;
		*this = tmp;
		return *this;
	}
	String& resize(const size_t & len)
	{
		return resize(len, ' ');
	}
	String& resize(const size_t & len, const char & ch)
	{
		size_t old_len = length();
		if (len > max_size())
		{
			reserve(len);									//reserve(len + len / 2);空间适当冗余
			if (max_size()>len)
				memset(_str(old_len), ch, len - old_len);	//填充扩充字符
		}

		*(_str(len)) = 0;									//置结束符
		pbuf->size = len;

		return *this;
	}
	String& reserve(size_t capacity)
	{
		if (capacity > max_size())
		{
			void * p = realloc(pbuf, sizeof(string_buffer) + capacity);
			if (p!=NULL)
			{
				pbuf = (string_buffer *)p;
				pbuf->capacity	= capacity;
			}
		}

		return *this;
	}
	String substr(size_t index, size_t count)
	{
		return String(*this, index, count);	
	}
	size_t size()
	{
		return length();
	}
	size_t max_size()
	{
		if (pbuf==NULL)
			return 0;
		else
			return pbuf->capacity;
	}
public:
	String():pbuf(NULL)
	{
		reserve(1); resize(0);
	};
	String(size_t capacity, char ch):pbuf(NULL)
	{ 
		resize(capacity, ch);	
	};	
	String(const char * str):pbuf(NULL)	
	{ 
		copy_of(str); 
	};
	String(String & str):pbuf(NULL)
	{ 
		copy_of(str.c_str()); 
	};
	String(const char * str, size_t index, size_t count):pbuf(NULL)
	{ 
		copy_of(str, 0, index, count);
	};	
	String(String & str, size_t index, size_t count):pbuf(NULL)
	{ 
		copy_of(str.c_str(), 0, index, count); 
	};
	~String()
	{
		if (pbuf!=NULL) 
			free(pbuf);
	};

	String& operator=(String & str)
	{
		return copy_of(str.c_str());
	}
	String& operator=(const char * str)
	{
		return copy_of(str);
	}
	char operator[](const size_t & ind)//外部以此赋值的时候怎么写
	{
		return at(ind);
	}
	bool operator==(String & str)
	{
		return strcmp(c_str(), str.c_str()) == 0;
	}	
	bool operator!=(String & str)
	{
		return strcmp(c_str(), str.c_str()) != 0;
	}
	bool operator>(String & str)
	{
		return strcmp(c_str(), str.c_str()) > 0;
	}
	bool operator>=(String & str)
	{		
		return strcmp(c_str(), str.c_str()) >= 0;
	}
	bool operator<(String & str)
	{
		return strcmp(c_str(), str.c_str()) < 0;
	}
	bool operator<=(String & str)
	{
		return strcmp(c_str(), str.c_str()) <= 0;
	}	
	String operator+(const char *str)
	{
		String s = *this;
		return s.append(str);
	}
	String operator+(String & str)
	{
		String s = *this;
		return s.append(str);
	}	
	String & operator+=(const char *str)
	{
		return append(str);
	}
	String & operator+=(String & str)
	{
		return append(str);
	}
};