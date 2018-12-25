//功  能：安全简化的字符串拷贝和格式化函数
//创建人：刘明祥

#pragma once

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

//#include <concrt.h>
//(concurrency::Context::Yield)();

//增加了strncpy_s的两种实现，用以替换strcpy_s而不需要改变原来的输入参数,这里的_source必须以\0结尾
inline size_t strncpy_s(char * _dest , size_t _destsize, const char * _source) throw()
{
	return strncpy_s(_dest, _destsize, _source, _destsize-1);
}

template<size_t _size>
inline size_t strncpy_s(char (&_dest)[_size], const char * _source) throw()
{
	return strncpy_s(_dest, _size, _source, _size-1);
}

//strncat_s
inline size_t strncat_s(char * _dest , size_t _destsize, const char * _source) throw()
{
	return strncat_s(_dest, _destsize, _source, _destsize-1);
}

template<size_t _size>
inline size_t strncat_s(char (&_dest)[_size], const char * _source) throw()
{
	return strncat_s(_dest, _size, _source, _size-1);
}


//增加了_snprintf_s的两种实现
inline size_t _snprintf_s(char * _dest , size_t _destsize, const char * _format, ...) throw()
{
	va_list ap;
	va_start(ap, _format);
	size_t leng = _vsnprintf_s(_dest, _destsize, _destsize - 1, _format, ap);
	va_end(ap);

	return leng;
}

template<size_t _size>
inline size_t _snprintf_s(char (&_dest)[_size], const char * _format, ...) throw()
{
	va_list ap;
	va_start(ap, _format);
	size_t leng = _vsnprintf_s(_dest, _size, _size - 1, _format, ap);
	va_end(ap);

	return leng;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

//增加了strncpy_s的两种实现，用以替换strcpy_s而不需要改变原来的输入参数,这里的_source必须以\0结尾
inline size_t wcsncpy_s(wchar_t * _dest , size_t _destsize, const wchar_t * _source) throw()
{
	return wcsncpy_s(_dest, _destsize, _source, _destsize-1);
}

template<size_t _size>
inline size_t wcsncpy_s(wchar_t (&_dest)[_size], const wchar_t * _source) throw()
{
	return wcsncpy_s(_dest, _size, _source, _size-1);
}

//strncat_s
inline size_t wcsncat_s(wchar_t * _dest , size_t _destsize, const wchar_t * _source) throw()
{	
	return wcsncat_s(_dest, _destsize, _source, _destsize-1);
}

template<size_t _size>
inline size_t wcsncat_s(wchar_t (&_dest)[_size], const wchar_t * _source) throw()
{
	return wcsncat_s(_dest, _size, _source, _size-1);
}

//增加了_snprintf_s的两种实现
inline size_t _snwprintf_s(wchar_t * _dest , size_t _destsize, const wchar_t * _format, ...) throw()
{
	va_list ap;
	va_start(ap, _format);
	size_t leng = _vsnwprintf_s(_dest, _destsize, _destsize - 1, _format, ap);
	va_end(ap);

	return leng;
}

template<size_t _size>
inline size_t _snwprintf_s(wchar_t (&_dest)[_size], const wchar_t * _format, ...) throw()
{
	va_list ap;
	va_start(ap, _format);
	size_t leng = _vsnwprintf_s(_dest, _size, _size - 1, _format, ap);
	va_end(ap);

	return leng;
}