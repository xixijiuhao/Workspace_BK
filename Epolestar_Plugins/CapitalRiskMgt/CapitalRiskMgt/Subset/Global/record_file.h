#pragma once

#include <io.h>
#include <iostream>
#include <unordered_map>
#include <windef.h>
#include "murmur_hash3.h"

struct RecordHead
{
	size_t		HeadSize;
	size_t		ItemSize;	
};

class RWFile  
{
protected:
	FILE *	m_file;
private:
	char	m_file_name[MAX_PATH];
public:
	enum SeekType
	{
		seek_SET = 0,
		seek_CUR = 1,
		seek_END = 2
	};
	RWFile() : m_file(nullptr){m_file_name[0] = 0;}
	virtual ~RWFile()
	{
		if (nullptr != m_file) fclose(m_file);
	}
	inline bool is_opened()
	{
		return m_file != nullptr;
	}
	//文件初始化
	inline bool open(const char * file_name)
	{
		if (file_name==nullptr || file_name[0]==0)
			return false;
		strncpy_s(m_file_name, file_name, strlen(file_name));
		
		m_file = _fsopen(m_file_name, "a+bS", _SH_DENYWR);
		if (m_file != nullptr)
			fseek(m_file, 0, SEEK_END);
				
		return m_file != nullptr;
	}
	inline void close()
	{
		if (nullptr != m_file)
			fclose(m_file);
	}
	inline bool clear()
	{
		close();
		m_file = _fsopen(m_file_name, "w+bS", _SH_DENYWR);		
		return m_file != nullptr;
	}
	//写入二进制数据
	inline size_t write_data(const void * pdata, size_t data_size, bool flush = true)		
	{
		if (nullptr == m_file)
			return 0;
		
		size_t ret = fwrite(pdata, data_size, 1, m_file);
		if (flush)
			fflush(m_file);
		return ret;
	}
	inline size_t read_data(void * pdata, size_t data_size)
	{
		if (nullptr == m_file)
			return 0;
		
		return fread_s(pdata, data_size, data_size, 1, m_file);
	}
	inline int seek(long offset, SeekType type)
	{
		if (nullptr == m_file)
			return -1;
		return fseek(m_file, offset, type);
	}
	inline size_t size()
	{
		return _filelength(_fileno(m_file));
	}
};

class RecordFile
{
protected:
	RWFile		m_file;
	RecordHead m_head;
private:
	uint32_t	m_read_code;
	uint32_t	m_write_code;
public:
	RecordFile() : m_head({ 0 }), m_read_code(0x69884982), m_write_code(0x69884982){}
	~RecordFile() { close(); }
	inline void open(const char * file_name)
	{
		m_file.open(file_name);
	}
	inline void close()
	{
		m_file.close();
	}
	inline size_t write_head(const RecordHead & head)
	{
		m_head = head;

		size_t ret = m_file.write_data(&head, head.HeadSize);
		MurmurHash3_x86_32(&head, head.ItemSize, m_write_code, &m_write_code);
		ret += m_file.write_data(&m_write_code, sizeof(m_write_code));
		return ret;
	}
	inline size_t write_item(void * pdata)
	{
		size_t ret = m_file.write_data(pdata, m_head.ItemSize);
		MurmurHash3_x86_32(pdata, m_head.ItemSize, m_write_code, &m_write_code);
		ret += m_file.write_data(&m_write_code, sizeof(m_write_code));
		return ret;
	}

	inline size_t read_head(RecordHead & head)
	{
		uint32_t code;
		size_t ret = m_file.read_data(&head.HeadSize, sizeof(head.HeadSize));
		ret += m_file.read_data(&head.ItemSize, head.HeadSize - sizeof(head.HeadSize));
		ret += m_file.read_data(&code, sizeof(code));
		
		MurmurHash3_x86_32(&head, head.HeadSize, m_read_code, &m_read_code);
		if (code != m_read_code)//校验失败
			return 0;
		else			
			return ret;
	}
	inline size_t read_item(void * pdata)
	{
		uint32_t code;
		size_t ret = m_file.read_data(&pdata, m_head.ItemSize);
		ret += m_file.read_data(&code, sizeof(code));

		MurmurHash3_x86_32(pdata, m_head.ItemSize, m_read_code, &m_read_code);
		if (code != m_read_code)//校验失败
			return 0;
		else			
			return ret;
	}
	inline int seek(long offset, RWFile::SeekType type)
	{
		return m_file.seek(offset, type);
	}
	inline size_t size()
	{
		return m_file.size();
	}
};

//索引文件的读写
class IndexRecordFile: public RecordFile
{
private:
	typedef std::unordered_map<size_t, size_t> indMp;
	indMp		m_indexs;
	bool		m_cove_data;
	uint32_t	m_check_code;
public:
	//cove_data：是否覆盖相同索引上已存在的数据
	IndexRecordFile(bool cove_data = true) : m_cove_data(cove_data), m_check_code(0x69819a25){}
	inline size_t write_item(void * pdata, size_t index)
	{
		indMp::iterator it = m_indexs.find(index);
		if (it != m_indexs.end())
		{
			if (!m_cove_data)
				return 0;
			m_file.seek(it->second, RWFile::seek_SET);
		}
		else
		{
			m_file.seek(it->second, RWFile::seek_END);
			m_indexs.insert(indMp::value_type(index, m_file.size()));
		}

		size_t ret = m_file.write_data(pdata, m_head.ItemSize);
		uint32_t code;
		MurmurHash3_x86_32(pdata, m_head.ItemSize, m_check_code, &code);
		ret += m_file.write_data(&code, sizeof(code));
		return ret;
	}
	inline size_t read_item(void * pdata)
	{
		uint32_t code, code2;
		size_t ret = m_file.read_data(&pdata, m_head.ItemSize);
		ret += m_file.read_data(&code, sizeof(code));

		MurmurHash3_x86_32(pdata, m_head.ItemSize, m_check_code, &code2);
		if (code != code2)//校验失败
			return 0;
		else			
			return ret;
	}
};