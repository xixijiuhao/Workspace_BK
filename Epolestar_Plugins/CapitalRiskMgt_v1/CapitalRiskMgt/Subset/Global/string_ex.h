//��  �ܣ���ȫ�򻯵��ַ��������͸�ʽ������
//�����ˣ�������

#pragma once

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

//#include <concrt.h>
//(concurrency::Context::Yield)();

//������strncpy_s������ʵ�֣������滻strcpy_s������Ҫ�ı�ԭ�����������,�����_source������\0��β
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


//������_snprintf_s������ʵ��
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

//������strncpy_s������ʵ�֣������滻strcpy_s������Ҫ�ı�ԭ�����������,�����_source������\0��β
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

//������_snprintf_s������ʵ��
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