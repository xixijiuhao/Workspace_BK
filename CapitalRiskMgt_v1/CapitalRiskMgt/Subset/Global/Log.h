#pragma once

#include <iostream>

class TLog  
{
private:
	FILE *	m_file;

	char	m_line[4096];
	bool	m_modify;							//更改标志

public:
	TLog() : m_modify(false), m_file(nullptr){}
	virtual ~TLog()
	{
		close();
	}
	bool is_opened()
	{
		return m_file != nullptr;
	}
	//文件初始化
	bool open(const char * file_name, bool clear = true)
	{
		if (file_name==nullptr || file_name[0]==0)
			return false;

		if (clear)
			m_file = _fsopen(file_name, "wbS", _SH_DENYWR);
		else
			m_file = _fsopen(file_name, "abS", _SH_DENYWR);

		if (m_file==nullptr)
			return false;

		fseek(m_file, 0, SEEK_END);

		return true;
	}
	//写入格式化字符串, flush:立即提交到磁盘文件,写入时将字符串结束符一并写入
	void add_log(bool flush, const char * format, ...)						
	{
		va_list lst;
		va_start(lst, format);

		int leng = vsnprintf_s(m_line, sizeof(m_line), sizeof(m_line) - 4, format, lst);
		add_log(m_line, leng + 1, flush);												

		va_end(lst);
	}	
	//写入二进制数据
	void add_log(const void * pdata, uint data_size, bool flush = true)		
	{
		if (nullptr != m_file)
		{
			fwrite(pdata, 1, data_size, m_file);
			if (flush)
				fflush(m_file);
			m_modify = ! flush;
		}
	}
	void update()
	{
		if (!m_modify || nullptr == m_file)
			return;
		
		fflush(m_file);
		m_modify = false;
	}
	void close()
	{
		if (nullptr != m_file)
			fclose(m_file);
		m_file = nullptr;
	}
	//因为只在文件尾写入，所以不用fseek(m_File, 0, SEEK_END), 直接ftell就是文件大小
	uint file_size()
	{		
		if (nullptr != m_file)
			return (uint)ftell(m_file);						
		else
			return 0;
	}
};
