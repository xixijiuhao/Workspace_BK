//功  能：超时控制
//创建人：刘明祥
#pragma once

#include "_base.h"
#include "crc.h"
#include <map> 


class TimeOutCtrl
{
private:	
	typedef map<uint, uint>	mpTimeOut;
	mpTimeOut	m_timeouts;
	TCRC		m_crc;
	ScopeLock;
public:
	//超时判断, set_time超时后是否用当前时间去重置请求时间
	bool can_req(const uint key, const uint outtime, const bool set_time = true)
	{
		uint start = StlFun::MapFindOnly(m_timeouts, key, 0);
		uint curr  = GetTickCount();	
		bool ret = curr - start > outtime || start == 0;
		if (set_time && ret)
			set_req_time(key);
		return ret;
	}
	//设置超时时间
	void set_req_time(const uint key)
	{
		LockScope();
		uint curr  = GetTickCount();	
		StlFun::CiteToMap(m_timeouts, curr, key);
	}
	//清除超时记录
	void clear_time_out(const uint key)
	{		
		LockScope();
		uint curr = 0;
		StlFun::CiteToMap(m_timeouts, curr, key);
	}

	bool can_req(const char * key, const uint key_len, const uint outtime, const bool set_time = true)
	{
		uint k = m_crc.Crc32Buf(key, key_len);
		return can_req(k, outtime, set_time);
	}
	void set_req_time(const char * key, const uint key_len)
	{
		uint k = m_crc.Crc32Buf(key, key_len);
		set_req_time(k);
	}
	void clear_time_out(const char * key, const uint key_len)
	{
		uint k = m_crc.Crc32Buf(key, key_len);
		return clear_time_out(k);
	}
};