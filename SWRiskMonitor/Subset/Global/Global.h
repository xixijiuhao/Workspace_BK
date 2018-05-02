//功  能：一些通用函数合集
//创建人：刘明祥

#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif 

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN
#endif

#pragma comment(lib, "Wininet.lib")
#pragma comment(lib, "Version.lib")
#pragma comment(lib, "Shlwapi.lib")

#include <windows.h>
#include "math.h"
#include "time.h"
#include "direct.h"
#include "Str2Time.h"
#include "ACCallback.h"
#include "WinInet.h"
#include <regex> 
#include <string> 
#include <vector> 
#include "ObjBase.h"
#include "string_ex.h"
#include "shlwapi.h"
#include "_base.h"
#include "x_types.h"
#include "uxtheme.h"
#include "io.h"
#include <tlhelp32.h>
#include <list>

using namespace std;

#define BUF_SIZE 2048

//Assert() 返回出错的文件名和行号
inline void _assert(char* str, char* fname, int lineno)
{
	fprintf(stderr, "error: %s in %s at %d\n", str, fname, lineno);
	abort();
};
#ifdef _DEBUG
#define Assert(c) if(c) 0; else _assert(#c, __FILE__, __LINE__)
#else
#define Assert(c) 0
#endif

class SystemFun
{
public:
	static uint GetCoreCount()
	{
		SYSTEM_INFO info;
		GetSystemInfo(&info);
		return info.dwNumberOfProcessors;
	}
	static bool IsLittleMode()					//判断大小端
	{
		int x = 1;
		return *(char *)&x == 1;
	}
};


typedef vector<string> Strings;
class StrFun
{
public:
	static bool MatchPattern(const char *pattern, const char *target)//正则匹配
	{
		tr1::cmatch res;
		tr1::regex rx(pattern);
		tr1::regex_search(target, res, rx);
		return !res.empty();
	}
	static bool IsValidNumber(const char * sValue, bool bOnlyInt = false, bool bCanNegative = false)//是否有效的数字
	{
		string pattern = "^(\\d+)?(\\.)?(\\d+)?$";	//整数、小数
		if (bOnlyInt)
			pattern = "^(\\d+)?$";					//只允许为整数
		if (bCanNegative)
			pattern.insert(1, "(-)?");				//允许为负
		//	^:行开头
		//	\d：数字
		//	+ ：出现至少1次
		//	.：任意字符，除换行和回车之外
		//	？：出现0或1次
		//	(.\d + ) ? ：括号里内出现0或1次
		//	$：行结尾
		return MatchPattern(pattern.c_str(), sValue);
	}
	static void format(string & dest, const char *fmt, const va_list & ap)
	{
		dest = "";
		int size = _vscprintf(fmt, ap); //计算_vsprintf()函数中的buffer字符串要多少字节的空间。
		if (size > 0)
		{
			char * buffer = (char *)malloc(size + 1);
			buffer[0] = 0;
			if (_vsnprintf_s(buffer, size + 1, size, fmt, ap) > 0)
				dest = buffer;
			free(buffer);
		}		
	}
	static void format(string & dest, const char *fmt, ...)
	{	
		va_list ap;					//参数列表的指针
		va_start(ap, fmt);			//限定从fmt开始,限定多少个参数
		format(dest, fmt, ap);
		va_end(ap);
	}
	static string format(const char *fmt, ...)
	{
		string ret;

		va_list ap;
		va_start(ap, fmt);
		format(ret, fmt, ap);
		va_end(ap);

		return ret;
	}
	static void format(wstring & dest, const wchar_t *fmt, const va_list & ap)
	{		
		dest = L"";
		int size = _vscwprintf(fmt, ap);
		if (size > 0)
		{
			wchar_t * buffer = (wchar_t *)malloc((size + 1) * sizeof(wchar_t));
			buffer[0] = 0;
			if (_vsnwprintf_s(buffer, size + 1, size, fmt, ap) > 0)
				dest = buffer;
			free(buffer);
		}	
	}
	static void format(wstring & dest, const wchar_t *fmt, ...)
	{
		va_list ap;
		va_start(ap, fmt);
		format(dest, fmt, ap);
		va_end(ap);
	}
	static wstring format(const wchar_t *fmt, ...)
	{
		wstring ret;

		va_list ap;
		va_start(ap, fmt);
		format(ret, fmt, ap);
		va_end(ap);

		return ret;
	}
	static void trace(const char *fmt, ...)
	{
		va_list ap;
		va_start(ap, fmt);
		
		int size = _vscprintf(fmt, ap);
		if (size > 0)
		{
			char * buffer = (char *)malloc(size + 4);
			buffer[0] = 0;
			if (_vsnprintf_s(buffer, size + 1, size, fmt, ap) >= 0)
			{
				buffer[size++] = '\n';
				buffer[size] = 0;
				OutputDebugStringA(buffer);
			}
			free(buffer);
		}

		va_end(ap);
	}	
	static void trace(const wchar_t *fmt, ...)
	{
		va_list ap;
		va_start(ap, fmt);
		
		int size = _vscwprintf(fmt, ap);
		if (size > 0)
		{
			wchar_t * buffer = (wchar_t *)malloc((size + 4) * sizeof(wchar_t));
			buffer[0] = 0;
			if (_vsnwprintf_s(buffer, size + 1, size, fmt, ap) >= 0)
			{
				buffer[size++] = '\n';
				buffer[size] = 0;
				OutputDebugStringW(buffer);
			}
			free(buffer);
		}

		va_end(ap);
	}
	class trace_dbg
	{
	private:
		const char *	m_file_name;	
		const int		m_line_no;
	public:
		trace_dbg(const char * file_name, int line_no): m_file_name(file_name), m_line_no(line_no){}
		void operator()(const char * fmt, ...) const
		{		
			va_list ap; 
			va_start(ap, fmt);
			string str;
			format(str, fmt, ap);				
			va_end(ap);			

#ifdef _DEBUG
			_CrtDbgReport(_CRT_WARN, m_file_name, m_line_no, nullptr, "%s", str.c_str());
#else
			trace("%s, %n: %s", m_file_name, m_line_no, str.c_str());
#endif
		}
		void operator()(const wchar_t * fmt, ...) const
		{
			wchar_t fileName[_MAX_PATH] = {0};	
			if (swprintf_s(fileName, _MAX_PATH, L"%S", m_file_name) == -1)
				return;

			va_list ap; 
			va_start(ap, fmt);
			wstring str;
			format(str, fmt, ap);
			va_end(ap);		

#ifdef _DEBUG
			_CrtDbgReportW(_CRT_WARN, fileName, m_line_no, nullptr, L"%s", str.c_str());
#else
			trace("%s, %n: %s", m_file_name, m_line_no, str.c_str());
#endif
		}
	};

	//用正则表达式替换
	static string regex_replace(const char * str, const char * souc_regex, const char * dest_regex_fmt)
	{		
		std::regex regex(souc_regex, std::regex::icase);	//忽视大小写
		return std::regex_replace(str, regex, dest_regex_fmt);
	}

	//字符串替换
	static string replace(const char * str, const char * souc_sub_str, const char * dest_sub_str)
	{
		string ret;

		int souc_sub_len = strlen(souc_sub_str);
		const char * p = str;
		const char * q = str;
		while (q)
		{
			p = q;
			q = strstr(p, souc_sub_str);
			if (q)
			{
				ret.append(p, q - p);
				ret.append(dest_sub_str);
				q += souc_sub_len;
			}
		}
		ret.append(p);

		return ret;
	}
	//字符替换
	static string replace(const char * str, const char souc, const char dest)
	{
		string ret = str;

		for (uint i = 0; i < ret.length(); i++)
		{
			if (ret[i] == souc)
				ret[i] = dest;
		}

		return ret;
	}
	//...传进来的可变参数必须都是字符串
	static string MergeString(const char * Delimiter, uint strCount, ...)
	{
		char * p;
		char buffer[1024];
		char * q = &(buffer[0]);
		int leng = sizeof(buffer) / sizeof(char);
		bool isempty = true;

		va_list ap;
		va_start(ap, strCount);
		for (uint i = 0; i < strCount; i++)
		{
			p = va_arg(ap, char *);
			isempty = isempty && p[0] == 0;
			int n = _snprintf_s(q, leng, "%s%s", p, Delimiter);
			leng -= n;
			q += n;
		}
		q -= lstrlenA(Delimiter);
		*q = 0;
		va_end(ap);

		if (isempty)
			return "";
		else
			return buffer;
	}
	//合并字符串
	static string MergeString(Strings & sArray, const char cDelimiter = ',')
	{
		string str;
		for (uint i = 0; i < sArray.size(); i++)
		{
			str.append(sArray[i]);
			str.append(1, cDelimiter);
		}
		if (str.length()>0)
			str.at(str.length() - 1) = 0;

		return str;
	}
	//以sDelimiters中的任一字符分割text//MaxCount:最大分割数量，-1则忽略此参数
	static void SplitStringEx(const string & text, Strings & sArray, const string & sDelimiters, const uint MaxCount = -1)
	{
		sArray.clear();
		string item;
		char chr;
		for (uint i = 0; i < text.length(); i++)
		{
			chr = text.at(i);
			if (sDelimiters.find(chr, 0) != string::npos && (sArray.size() < MaxCount - 1 || MaxCount == -1))
			{
				sArray.push_back(item);
				item = "";
			}
			else
				item += chr;
		}
		if (!item.empty())
			sArray.push_back(item);
	}
	//以单字符cDelimiter分割text//MaxCount:最大分割数量，-1则忽略此参数
	static void SplitString(const string & text, Strings & sArray, const char cDelimiter, const uint MaxCount = -1)
	{
		sArray.clear();
		string item;
		char chr;
		for (uint i = 0; i < text.length(); i++)
		{
			chr = text.at(i);
			if (chr == cDelimiter && (sArray.size() < MaxCount - 1 || MaxCount == -1))
			{
				sArray.push_back(item);
				item = "";
			}
			else
				item += chr;
		}
		if (!item.empty())
			sArray.push_back(item);
	}
	//以字符串cDelimiter分割text//MaxCount:最大分割数量，-1则忽略此参数
	static void SplitString(const string & str, Strings & arrParts, const string & sDelimiter, const uint MaxCount = -1)
	{
		uint    pos = 0;
		uint    index = 0;
		arrParts.clear();

		while (index < str.length())
		{
			pos = str.find(sDelimiter, index);
			if (-1 != pos && (-1 == MaxCount || arrParts.size() < MaxCount - 1))
			{
				arrParts.push_back(str.substr(index, pos - index));
				index = pos + sDelimiter.length();
			}
			else
			{
				arrParts.push_back(str.substr(index, str.length() - index));
				index = str.length();
			}
		}
	}
	static void ToLower(string & str, const uint ind = 0)
	{
		for (uint i = ind; i < str.length(); i++)
			if (str[i] >= 'A' && str[i] <= 'Z')
				str[i] += 32;
	}
	static void ToUpper(string & str, const uint ind = 0)
	{
		for (uint i = ind; i < str.length(); i++)
			if (str[i] >= 'a' && str[i] <= 'z')
				str[i] -= 32;
	}
	static void Left(string & dest, const char * sour, const uint num)
	{
		dest.clear();
		if (sour != nullptr)
			dest.append(sour, num);
	}
	static void Right(string & dest, const char * sour, const uint num)
	{
		if (sour != nullptr)
			dest = sour + max(0, strlen(sour) - num);
		else
			dest.clear();
	}
	static void Left(string & dest, const char * sour, const uint ind, const uint num)
	{
		dest.clear();
		if (sour != nullptr)
		{
			int i = min(ind, strlen(sour)-1);
			const char * s = sour + i;
			Left(dest, s, num);
		}
	}
	static void Right(string & dest, const char * sour, const uint ind, const uint num)
	{
		dest.clear();
		if (sour != nullptr)
		{
			int i = min(ind, strlen(sour)-1);
			const char * s = sour + i - min(num, strlen(sour));
			Left(dest, s, num);
		}
	}
	static void Trim(wstring & str)
	{
		if (str.empty())
			return;
		uint i = 0, j = str.length() - 1;

		while (i < str.length())
			if ((WORD)str[i++] > 32)
				break;
		while (j >= i)
			if ((WORD)str[j--] > 32)
				break;		

		if (i > 0) i--;
		if (j < str.length() - 1) j++;

		if (i >= j)
			str = L"";
		else
			str = str.substr(i, j - i + 1);
	}
	static void Trim(string & str)
	{
		if (str.empty())
			return;
		uint i = 0, j = str.length() - 1;

		while (i < str.length())
			if ((BYTE)str[i++] > 32)
				break;
		while (j >= i)
			if ((BYTE)str[j--] > 32)
				break;		

		if (i > 0) i--;
		if (j < str.length() - 1) j++;

		if (i >= j)
			str = "";
		else
			str = str.substr(i, j - i + 1);
	}
	template<size_t _size>
	static void Trim(char(&str)[_size], const char filter_char = 0)
	{
		if (str[0] == 0)
			return;
		uint i = 0, len = strlen(str), j = len - 1;

		while (i < len)
			if ((BYTE)str[i] > 32 && filter_char != str[i])
				break;
			else
				i++;
		while (j >= i)
			if ((BYTE)str[j] > 32 && filter_char != str[j])
				break;
			else
				j--;

		if (i >= j)
			str[0] = 0;
		else
			memmove_s(str, _size, &str[i], j - i + 1);
			str[j - i + 1] = 0;
	}
	static void StringsTrim(Strings & arrParts)
	{
		for (uint i = 0; i < arrParts.size(); i++)
			Trim(arrParts[i]);
	}
	static void StringsUpper(Strings & arrParts)
	{
		for (uint i = 0; i < arrParts.size(); i++)
			ToUpper(arrParts[i]);
	}
	static string html2txt(const char * html)
	{
		//将所有空格、换行符、tab去掉，因为html中这些是被忽略的
		string ret = replace(html		 , "/r"						, " ");
		ret = replace(ret.c_str()		 , "/n"						, " ");
		ret = replace(ret.c_str()		 , "/t"						, " ");

		//将<head>标记中的所有内容去掉
		ret = regex_replace(ret.c_str(), "(<head>).*(</head>)"		, "");

		//将<script>标记中的所有内容去掉
		ret = regex_replace(ret.c_str(), "<( )*script([^>])*>"		, "<script>");
		ret = regex_replace(ret.c_str(), "(<script>).*(</script>)"	, "");

		//将<style>标记中的所有内容去掉
		ret = regex_replace(ret.c_str(), "<( )*style([^>])*>"		, "<style>"); //清空属性
		ret = regex_replace(ret.c_str(), "(<style>).*(</style>)"	, "");

		//将td标记换成空格, li, br标记换成换行符
		ret = regex_replace(ret.c_str(), "<( )*td([^>])*>"			, " ");
		ret = regex_replace(ret.c_str(), "<( )*br( )*>"				, "\r");
		ret = regex_replace(ret.c_str(), "<( )*li( )*>"				, "\r");

		//将tr, p标记换成双换行符
		ret = regex_replace(ret.c_str(), "<( )*tr([^>])*>"			, "\r\r");
		ret = regex_replace(ret.c_str(), "<( )*p([^>])*>"			, "\r\r");

		//将以"<>"符号为头尾的标记去掉
		ret = regex_replace(ret.c_str(), "<[^>]*>"					, "");

		//将转义字符还原
		ret = regex_replace(ret.c_str(), "&amp;"					, "&");
		ret = regex_replace(ret.c_str(), "&nbsp;"					, " ");
		ret = regex_replace(ret.c_str(), "&lt;"						, "<");
		ret = regex_replace(ret.c_str(), "&gt;"						, ">");
		ret = regex_replace(ret.c_str(), "&(.{2,6});"				, "");

		//将多余的空格和空行去掉
		//ret = regex_replace(ret.c_str(), " ( )+"					, " ");
		//ret = regex_replace(ret.c_str(), "(\r)( )+(\r)"			, "\r\r");
		//ret = regex_replace(ret.c_str(), "(\r\r)+"				, "\r\n");

		Trim(ret);

		return ret;
	}
};

#define traceDbg StrFun::trace_dbg(__FILE__, __LINE__)


class TimeFun
{
public:
	static void GetTime(char * tim)
	{
		time_t now = time(nullptr);
		tm lt;
		localtime_s(&lt, &now);
		strftime(tim, 20, "%H:%M:%S", &lt);
	}
	static void GetDate(char * Date)
	{
		time_t now = time(nullptr);
		tm lt;
		localtime_s(&lt, &now);
		strftime(Date, 20, "%Y-%m-%d", &lt);
	}
	static string GetDate()
	{
		char Date[11];
		time_t now = time(nullptr);
		tm lt;
		localtime_s(&lt, &now);
		strftime(Date, 20, "%Y-%m-%d", &lt);
		return Date;
	}
	static wstring GetDateW()
	{
		wchar_t Date[11];
		time_t now = time(nullptr);
		tm lt;
		localtime_s(&lt, &now);
		wcsftime(Date, 20, L"%Y-%m-%d", &lt);
		return Date;
	}
	static size_t GetDateTime(char * DateTime)
	{
		time_t now = time(nullptr);
		tm lt;
		localtime_s(&lt, &now);
		return strftime(DateTime, 20, "%Y-%m-%d %H:%M:%S", &lt);
	}
	static size_t GetDateTime(char * DateTime, const char * fmt)
	{
		time_t now = time(nullptr);
		tm lt;
		localtime_s(&lt, &now);
		return strftime(DateTime, 20, fmt, &lt);
	}
	template<size_t _size>
	static void spDateTime(char * strFormat, char(&strDest)[_size])
	{
		FormatDateTime(strFormat, strDest, _size);
	}
	static size_t FormatDateTime(char * strFormat, char * strDest, uint iDestSize)
	{
		time_t now = time(nullptr);
		tm lt;
		localtime_s(&lt, &now);
		return strftime(strDest, iDestSize, strFormat, &lt);
	}
	static size_t TimeToStr(char * strFormat, char * strDest, uint iDestSize, time_t value)
	{
		tm lt;
		localtime_s(&lt, &value);
		return strftime(strDest, iDestSize, strFormat, &lt);
	}
	static void systime_to_tm(SYSTEMTIME & tim, tm &lt)
	{
		lt.tm_year	= tim.wYear - TM_YEAR_BASE;
		lt.tm_mon	= tim.wMonth - 1;
		lt.tm_mday	= tim.wDay;
		lt.tm_wday	= tim.wDayOfWeek;
		lt.tm_hour	= tim.wHour;
		lt.tm_min	= tim.wMinute;
		lt.tm_sec	= tim.wSecond;
		lt.tm_yday	= 0;
		lt.tm_isdst = 0;
	}	
	static void tm_to_systime(tm &lt, SYSTEMTIME & time)
	{
		time.wYear		= lt.tm_year + TM_YEAR_BASE;
		time.wMonth		= lt.tm_mon + 1;
		time.wDay		= lt.tm_mday;
		time.wDayOfWeek	= lt.tm_wday;
		time.wHour		= lt.tm_hour;
		time.wMinute	= lt.tm_min;
		time.wSecond	= lt.tm_sec;
		time.wMilliseconds = 0;
	}	
	static void get_now(SYSTEMTIME & tim)
	{
		time_t now = time(nullptr);
		TimeFun::time_to_systime(now, tim);
	}	
	static time_t get_date()
	{
		time_t t = time(nullptr);
		tm lt;
		localtime_s(&lt, &t);
		lt.tm_hour = 0;
		lt.tm_min = 0;
		lt.tm_sec = 0;
		return mktime(&lt);
	}
	static time_t systime_to_time(SYSTEMTIME & tim)
	{
		tm lt;
		systime_to_tm(tim, lt);
		return mktime(&lt);
	}
	static void time_to_systime(time_t t, SYSTEMTIME & tim)
	{	
		tm lt;
		localtime_s(&lt, &t);
		tm_to_systime(lt, tim);
	}
	//%s:毫秒数
	static size_t FormatDateTimeEx(char * strFormat, char * strDest, uint iDestSize)
	{
		SYSTEMTIME   lTime;
		::GetLocalTime(&lTime);

		char MilliSecond[4];
		int subLeng = _snprintf_s(MilliSecond, "%03d", lTime.wMilliseconds);

		//将"%s"替换为毫秒
		string sFormat = strFormat;
		int ind = sFormat.find("%s", 0);
		while (ind >= 0)
		{
			sFormat.replace(ind, 2, MilliSecond);
			ind = sFormat.find("%s", ind + subLeng);
		}

		tm  lt;
		systime_to_tm(lTime, lt);
		return strftime(strDest, iDestSize, sFormat.c_str(), &lt);
	}
	static bool StrToTime(const char * strTime, char * strFormat, tm & lt)
	{
		char sTime[MAX_PATH];
		strncpy_s(sTime, strTime);
		__try
		{
			memset(&lt, 0, sizeof(tm));
			strptime(sTime, strFormat, &lt);
			return true;
		}
		__except (0)//catch (CException* e)
		{
			//e->Delete();
			return false;
		}
	}
	static time_t StrToTime(const char * strTime, char * strFormat)
	{
		tm lt;
		if (StrToTime(strTime, strFormat, lt))
		{			
			if (strcmp(strFormat , "%H:%M:%S") == 0)
			{
				time_t t = time(nullptr);
				tm _lt;
				localtime_s(&_lt, &t);
				lt.tm_year = _lt.tm_year;
				lt.tm_mon  = _lt.tm_mon;
				lt.tm_mday = _lt.tm_mday;
				lt.tm_yday = _lt.tm_yday;
				lt.tm_wday = _lt.tm_wday;
			}
			return mktime(&lt);
		}
		else
			return 0;
	}
	static time_t FileTimeToTime(FILETIME & ft)  
	{  
        //LONGLONG  ll;   
        ULARGE_INTEGER     ui;  
        ui.LowPart         =  ft.dwLowDateTime;  
        ui.HighPart        =  ft.dwHighDateTime;   
        //ll				   =  ft.dwHighDateTime  <<  32  +  ft.dwLowDateTime;   
        return (time_t)((LONGLONG)(ui.QuadPart  -  116444736000000000)  /  10000000);  
	} 
	//日期是否相等，忽略时间
	static bool CompareDay(time_t t1, time_t t2)
	{
		//tm lt1 = *localtime(&t1);		//localtime重复调用，返回的都是同一个地址的指针，后面的结果会覆盖前面的，所以这里要取值而不是指针
		//tm lt2 = *localtime(&t2);

		tm lt1;
		localtime_s(&lt1, &t1);
		tm lt2;
		localtime_s(&lt2, &t2);
		return lt1.tm_year == lt2.tm_year && lt1.tm_yday == lt2.tm_yday;
	}
	//日期差，忽略时间
	static int DayDiff(time_t t1, time_t t2)
	{
		//time_t是格林威治时间，所以本地日期转为time_t能被(60*60*24)整除，差了8小时（28800秒）
		//tm	是本地时间
		//日期比较的时候需要先转为本地时间，再将时分秒置零，然后转为格林威治时间作差

		//tm lt1 = *localtime(&t1);		//localtime重复调用，返回的都是同一个地址的指针，后面的结果会覆盖前面的，所以这里要取值而不是指针
		//tm lt2 = *localtime(&t2);		
		tm lt1;
		localtime_s(&lt1, &t1);
		tm lt2;
		localtime_s(&lt2, &t2);
		lt1.tm_hour = 0;
		lt1.tm_min = 0;
		lt1.tm_sec = 0;
		lt2.tm_hour = 0;
		lt2.tm_min = 0;
		lt2.tm_sec = 0;

		time_t tt1 = mktime(&lt1);
		time_t tt2 = mktime(&lt2);

		double Result = difftime(tt1, tt2) / (60 * 60 * 24);
		//_ASSERT(Result-(int)Result < 1e-10);
		return (int)(Result);
	}
	//CPU上电以来所经过的时钟周期数,支持Pentium以上级别的CPU
	static inline uint64 GetCycleCount()
	{
		//asm RDTSC 
		__asm
		{
			_emit 0x0F;
			_emit 0x31;
		}
	} 
};

#define WM_DOWNLOAD (WM_USER + 300);
class FileFun
{
public:
	//取路径
	static string GetFilePath(const char * FileName)
	{
		char Path[MAX_PATH];
		strncpy_s(Path, FileName);
		char * p = strrchr(Path, '\\');
		if (!p)
			p = strrchr(Path, '/');
		if (p)
		{
			*++p = 0;
			return Path;
		}
		else
			return "";
	};
	//取路径
	static wstring GetFilePath(const wchar_t * FileName)
	{
		wchar_t Path[MAX_PATH];
		wcsncpy_s(Path, FileName);
		wchar_t * p = wcsrchr(Path, L'\\');
		if (!p)
			p = wcsrchr(Path, L'/');
		if (p)
		{
			*++p = 0;
			return Path;
		}
		else
			return L"";
	};
	static string GetFileName(const char * Path)
	{
		const char * p = strrchr(Path, '\\');
		if (!p)
			p = strrchr(Path, '/');

		if (p)
			return ++p;
		else
			return Path;
	}
	static string GetFileExt(const char * Path)
	{
		char* p = strrchr((char*)Path, '.');

		if (p)
			return ++p;
		else
			return Path;
	}
	static bool IsValidFilePath(const char * path)
	{
		if (path == nullptr || path[0] == 0)
			return false;

		while (*path)
		{
			switch (*path++)
			{
				case '\\':
				case '/':
				case ':':
				case '*':
				case '?':
				case '"':
				case '<':
				case '>':
				case '|':
					return false;
			}
		}
		return true;
	}
	//获得绝对路径
	static string GetAbsolutePath(const char * base_path, const char * relative_path)
	{
		char ret[MAX_PATH] = { 0 };
		string base = base_path;
		string relative = relative_path;
		if (base[base.size() - 1] != '\\')
			base.append("\\");
		if (relative.length() > 0 && relative[0] == '\\')
			relative.erase(0, 1);

		PathCombineA(ret, base.c_str(), relative.c_str());
		return ret;
	}
	//获得绝对路径
	static wstring GetAbsolutePath(const wchar_t * base_path, const wchar_t * relative_path)
	{
		wchar_t ret[MAX_PATH] = { 0 };
		wstring base = base_path;
		wstring relative = relative_path;
		if (base[base.size() - 1] !=L'\\')
			base.append(L"\\");
		if (relative.length() > 0 && relative[0] == L'\\')
			relative.erase(0, 1);

		PathCombineW(ret, base.c_str(), relative.c_str());
		return ret;
	}
	//获得相对路径
	static string GetRelativePath(const char * path, const char *  file)
	{
		char ret[MAX_PATH] = { 0 };
		PathRelativePathToA(ret, path, FILE_ATTRIBUTE_NORMAL, file, FILE_ATTRIBUTE_DIRECTORY);
		return ret;
	}
	//将基于主程序目录的相对路径转为绝对路径
	static string GetAbsolutePathForExe(const char * relative_path)
	{
		string path = GetExePath();
		return GetAbsolutePath(path.c_str(), relative_path);
	}
	//将基于主程序目录的相对路径转为绝对路径
	static wstring GetAbsolutePathForExe(const wchar_t * relative_path)
	{
		wstring path = GetExePathW();
		return GetAbsolutePath(path.c_str(), relative_path);
	}
	//将绝对路径转为基于主程序目录的相对路径
	static string GetRelativePathForExe(const char *  file)
	{
		string path = GetExePath();
		return GetRelativePath(path.c_str(), file);
	}
	//文件夹是否存在
	static bool DirectoryExists(string Directory)
	{
		int Code;
		Code = GetFileAttributesA(Directory.c_str());
		return (Code != -1) && (FILE_ATTRIBUTE_DIRECTORY && (Code != 0));
	};
	//文件夹是否存在
	static bool DirectoryExists(wstring Directory)
	{
		int Code;
		Code = GetFileAttributesW(Directory.c_str());
		return (Code != -1) && (FILE_ATTRIBUTE_DIRECTORY && (Code != 0));
	};
	//检查并创建文件夹 暂不支持相对路径
	static bool CreateDirOfDeep(string FileName)
	{
		string sDir;
		sDir = GetFilePath(FileName.c_str());
		if (DirectoryExists(sDir))
			return true;

		string Dir = "";
		for (uint i = 0; i < sDir.size(); i++)
		{
			Dir = Dir + sDir[i];
			if (((FileName[i] == '/') || (FileName[i] == '\\')) && (i > 2))
				if (!DirectoryExists(Dir))
				{
					if (!CreateDirectoryA(Dir.c_str(), nullptr))
						return false;
				}
		}
		return true;
	};
	//检查并创建文件夹 暂不支持相对路径
	static bool CreateDirOfDeep(wstring FileName)
	{
		wstring sDir;
		sDir = GetFilePath(FileName.c_str());
		if (DirectoryExists(sDir))
			return true;

		wstring Dir = L"";
		for (uint i = 0; i < sDir.size(); i++)
		{
			Dir = Dir + sDir[i];
			if (((FileName[i] == L'/') || (FileName[i] == L'\\')) && (i > 2))
				if (!DirectoryExists(Dir))
				{
					if (!CreateDirectoryW(Dir.c_str(), nullptr))
						return false;
				}
		}
		return true;
	};

	static string GetExeFile()
	{
		char Path[MAX_PATH];
		GetModuleFileNameA(GetModuleHandle(nullptr), Path, MAX_PATH);
		return Path;
	}
	//取得当前进程文件路径
	static void GetExePath(char * Path)
	{
		GetModuleFileNameA(GetModuleHandle(nullptr), Path, MAX_PATH);
		char* p = strrchr(Path, '\\');
		*++p = 0;
	}
	static string GetExePath()
	{
		char Path[MAX_PATH];
		GetModuleFileNameA(GetModuleHandle(nullptr), Path, MAX_PATH);
		char* p = strrchr(Path, '\\');
		*++p = 0;
		return Path;
	}
	static wstring GetExePathW()
	{
		wchar_t Path[MAX_PATH];
		GetModuleFileNameW(GetModuleHandle(nullptr), Path, MAX_PATH);
		wchar_t* p = wcsrchr(Path, L'\\');
		*++p = 0;
		return Path;
	}
	static string GetExeName()
	{
		char Path[MAX_PATH];
		GetModuleFileNameA(GetModuleHandle(nullptr), Path, MAX_PATH);
		char* p = strrchr(Path, '\\');
		return ++p;
	}
	static wstring GetExeNameW()
	{
		wchar_t Path[MAX_PATH];
		GetModuleFileNameW(GetModuleHandle(nullptr), Path, MAX_PATH);
		wchar_t* p = wcsrchr(Path, L'\\');
		return ++p;
	}
	static string GetExeExt()
	{
		char Path[MAX_PATH];
		GetModuleFileNameA(GetModuleHandle(nullptr), Path, MAX_PATH);
		char* p = strrchr(Path, '.');
		return ++p;
	}
	//获得当前模块的句柄
	static HMODULE GetSelfHandle()
	{
		HMODULE hMod = nullptr;
		GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT | GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCSTR)&FileFun::GetSelfHandle, &hMod);
		return hMod;
	}
	//获得当前模块的文件全路径
	static string GetSelfFile()
	{
		char Path[MAX_PATH];
		GetModuleFileNameA(GetSelfHandle(), Path, MAX_PATH);
		return Path;
	}
	static wstring GetSelfFileW()
	{
		wchar_t Path[MAX_PATH];
		GetModuleFileNameW(GetSelfHandle(), Path, MAX_PATH);
		return Path;
	}
	//获得当前模块的文件目录
	static string GetSelfPath()
	{
		char Path[MAX_PATH];
		GetModuleFileNameA(GetSelfHandle(), Path, MAX_PATH);
		char* p = strrchr(Path, '\\');
		*++p = 0;
		return Path;
	}
	static string GetSelfName()
	{
		char Path[MAX_PATH];
		GetModuleFileNameA(GetSelfHandle(), Path, MAX_PATH);
		char* p = strrchr(Path, '.');
		*p = 0;
		p = strrchr(Path, '\\');
		return ++p;
	}
	static wstring GetSelfNameW()
	{
		wchar_t Path[MAX_PATH];
		GetModuleFileNameW(GetSelfHandle(), Path, MAX_PATH);
		wchar_t* p = wcsrchr(Path, L'.');
		*p = 0;
		p = wcsrchr(Path, L'\\');
		return ++p;
	}
	static string GetSelfNameExt()
	{
		char Path[MAX_PATH];
		GetModuleFileNameA(GetSelfHandle(), Path, MAX_PATH);
		char* p = strrchr(Path, '\\');
		return ++p;
	}
	static string GetSelfExt()
	{
		char Path[MAX_PATH];
		GetModuleFileNameA(GetSelfHandle(), Path, MAX_PATH);
		char* p = strrchr(Path, '.');
		return ++p;
	}
	//取文件版本号
	static string GetVersion(const char * File)
	{
		string strVersion = "";

		uint handle;
		uint verInfoSize = GetFileVersionInfoSizeA(File, &handle);
		if (verInfoSize <= 0)
			return strVersion;

		char *verInfo = new char[verInfoSize];
		bool ret = GetFileVersionInfoA(File, 0, verInfoSize, verInfo) == TRUE;
		if (ret)
		{
			uint size = sizeof(VS_FIXEDFILEINFO);
			VS_FIXEDFILEINFO *pFixedFileInfo = nullptr;
			ret = VerQueryValueA(verInfo, "\\", (LPVOID*)&pFixedFileInfo, (PUINT)&size) == TRUE;
			if (ret)
			{
				//取版本数据
				ushort b1 = HIWORD(pFixedFileInfo->dwProductVersionMS);
				ushort b2 = LOWORD(pFixedFileInfo->dwProductVersionMS);
				ushort b3 = HIWORD(pFixedFileInfo->dwProductVersionLS);
				ushort b4 = LOWORD(pFixedFileInfo->dwProductVersionLS);
				StrFun::format(strVersion, "%d.%d.%d.%d", b1, b2, b3, b4);
			}
		}
		delete[] verInfo;

		return strVersion;
	}
	static wstring GetVersionW(const wchar_t * File)
	{
		wstring strVersion = L"";

		uint handle;
		uint verInfoSize = GetFileVersionInfoSizeW(File, &handle);
		if (verInfoSize <= 0)
			return strVersion;

		wchar_t *verInfo = new wchar_t[verInfoSize];
		bool ret = GetFileVersionInfoW(File, 0, verInfoSize, verInfo) == TRUE;
		if (ret)
		{
			uint size = sizeof(VS_FIXEDFILEINFO);
			VS_FIXEDFILEINFO *pFixedFileInfo = nullptr;
			ret = VerQueryValueW(verInfo, L"\\", (LPVOID*)&pFixedFileInfo, (PUINT)&size) == TRUE;
			if (ret)
			{
				//取版本数据
				ushort b1 = HIWORD(pFixedFileInfo->dwProductVersionMS);
				ushort b2 = LOWORD(pFixedFileInfo->dwProductVersionMS);
				ushort b3 = HIWORD(pFixedFileInfo->dwProductVersionLS);
				ushort b4 = LOWORD(pFixedFileInfo->dwProductVersionLS);
				StrFun::format(strVersion, L"%d.%d.%d.%d", b1, b2, b3, b4);
			}
		}
		delete[] verInfo;

		return strVersion;
	}
	static string GetAppVersion()
	{
		char path[MAX_PATH];
		::GetModuleFileNameA(nullptr, path, sizeof(path));
		return GetVersion(path);
	}
	static string GetSelfVersion()
	{
		return GetVersion(GetSelfFile().c_str());
	}
	static wstring GetSelfVersionW()
	{
		return GetVersionW(GetSelfFileW().c_str());
	}
	static string VerNumToString(UINT64 dwVersion)
	{
		ushort b1 = *(((ushort *)&dwVersion) + 3);
		ushort b2 = *(((ushort *)&dwVersion) + 2);
		ushort b3 = *(((ushort *)&dwVersion) + 1);
		ushort b4 = *(((ushort *)&dwVersion) + 0);
		return StrFun::format("%d.%d.%d.%d", b1, b2, b3, b4);
	}
	static UINT64 VerStrToNumber(string sVersion)
	{
		uint vNum = 4;										//正常版本号的区间数，形如：x.x.x.x
		UINT64 int64Version = 0;
		Strings sPartArray;
		StrFun::SplitString(sVersion, sPartArray, '.');

		for (uint i = 0; i < vNum; i++)
			if (i < sPartArray.size())
				*(((ushort *)&int64Version) + vNum - 1 - i) = (ushort)atoi(sPartArray[i].c_str());

		return int64Version;
	}
	//下载Http文件
	static bool HttpDownloadFile(const char * url, const char * localFile, HWND hWnd = 0)
	{
		HINTERNET hSession = nullptr;
		HINTERNET hInternet = nullptr;
		char buf[8192] = { 0 };
		uint dwRead = 1;
		FILE *pFile = nullptr;

		errno_t err = fopen_s(&pFile, localFile, "wb");
		if (err != 0)
			return false;
		hSession = InternetOpenA("Updata...", INTERNET_OPEN_TYPE_PRECONFIG, nullptr, nullptr, 0);
		if (!hSession)
		{
			fclose(pFile);
			return false;
		}
		hInternet = InternetOpenUrlA(hSession, url, nullptr, 0, INTERNET_FLAG_DONT_CACHE, 0);
		if (!hInternet)
		{
			fclose(pFile);
			InternetCloseHandle(hSession);
			return false;
		}

		bool bResult = true;
		while (dwRead > 0)
		{
			dwRead = 0;
			if (!InternetReadFile(hInternet, buf, sizeof(buf), &dwRead))
			{
				bResult = false;
				break;
			}
			if (dwRead != fwrite(buf, sizeof(TCHAR), dwRead, pFile))
			{
				bResult = false;
				break;
			}
			//::SendMessage(hWnd, WM_DOWNLOAD, dwRead, 0);
		}

		fclose(pFile);
		InternetCloseHandle(hSession);
		InternetCloseHandle(hInternet);
		return bResult;
	}
	/*
	static bool UnzipFile(string zipFile,string dir)
	{
	CUnzipFile uf(zipFile);
	uf.GoToFirstFile();
	do
	{
	unz_file_info ui;

	// 	getting the information about the file
	// (date, attributes, the size of the filename)
	uf.GetCurrentFileInfo(&ui);

	int iNameSize = ui.size_filename + 1;
	char* pName = new char [iNameSize];

	// get the name of the file
	uf.GetCurrentFileInfo(nullptr, pName, iNameSize);

	CFile f( dir + pName, CFile::modeWrite | CFile::modeCreate);

	delete[] pName;
	uf.OpenCurrentFile();
	char buf[BUF_SIZE];

	int size_read;
	do
	{
	size_read = uf.ReadCurrentFile(buf, BUF_SIZE);
	if (size_read > 0)
	f.Write(buf, size_read);
	}
	while (size_read == BUF_SIZE);

	// set the original date stamp and attributes to the unpacked file
	// (this function closes the file "f" which is needed to apply
	// the attributes)
	uf.UpdateFileStatus(f, ui);
	}while(uf.GoToNextFile());
	// cannot be called by the destructor
	uf.Close();
	return true;
	}
	*/
	//获得文件修改时间
	static time_t GetWriteTime(const char * file_name)
	{
		HANDLE hfile = CreateFileA(file_name, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (hfile == INVALID_HANDLE_VALUE)
			return 0;

		FILETIME ftCreate, ftAccess, ftWrite;
		::GetFileTime(hfile, &ftCreate, &ftAccess, &ftWrite);
		CloseHandle(hfile);
	
		return TimeFun::FileTimeToTime(ftWrite);
	}
	static bool FileLocked(const char * file_name)
	{
		if (file_name==nullptr || *file_name==0)
			return false;

		OFSTRUCT st = {0};
		st.cBytes = sizeof(st);
		st.fFixedDisk = *file_name != 'a' && *file_name != 'A';
		strncpy_s(st.szPathName, file_name);
		HFILE hf = ::OpenFile(file_name, &st, OF_SHARE_EXCLUSIVE);
		if (hf != HFILE_ERROR)
			::CloseHandle((HANDLE)hf);
		return hf != HFILE_ERROR;
	}
	static void DelOldFile(const char * dir, const int out_day = 30)
	{
		string fdir(dir);
		if (!fdir.empty() && fdir[fdir.length()-1] != '\\')
			fdir.append("\\");

		WIN32_FIND_DATAA fdata;
		string fpath = fdir + "*.*";
		HANDLE hfind = ::FindFirstFileA(fpath.c_str(), &fdata);
		if (INVALID_HANDLE_VALUE == hfind)
			return;

		time_t now = time(nullptr);
		string fname;
		while (true)
		{
			fname = fdata.cFileName;
			if (fname.compare(".") && fname.compare(".."))
			{
				string sub = fdir + fname;
				if ((fdata.dwFileAttributes & _A_SUBDIR) == _A_SUBDIR)
					DelOldFile(sub.c_str());
				else if (!FileLocked(sub.c_str()) && ::SetFileAttributesA(sub.c_str(), FILE_ATTRIBUTE_NORMAL))
				{
					time_t ft = TimeFun::FileTimeToTime(fdata.ftLastAccessTime);
					if (TimeFun::DayDiff(now, ft) > out_day)
						::DeleteFileA(sub.c_str());
				}
			}

			if (!::FindNextFileA(hfind, &fdata))
			{
				if (GetLastError() == ERROR_NO_MORE_FILES)
					break;
			}
		}

		FindClose(hfind);
	}
	//结束子进程，不包括主进程的重启进程
	static void ExitChildProccse(DWORD ppid)
	{
		HANDLE procSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if(procSnap == INVALID_HANDLE_VALUE)
			return;

#ifdef UNICODE
		wstring exe_file = FileFun::GetExeNameW();
#else
		string exe_file = FileFun::GetExeName();
#endif

		PROCESSENTRY32 item = {0};
		item.dwSize = sizeof(item);
		BOOL ret = Process32First(procSnap, &item);
		while(ret)
		{
			ret = Process32Next(procSnap, &item);
			if (ppid != item.th32ParentProcessID || exe_file.compare(item.szExeFile)==0)
				continue;

			HANDLE proc_id = ::OpenProcess(PROCESS_TERMINATE | PROCESS_QUERY_INFORMATION, FALSE, item.th32ProcessID);
			TerminateProcess(proc_id, 0);
		}

		CloseHandle(procSnap);
	}
	static int is_unicode_head(uint buf)
	{
		if ((buf << 8) == 0xBFBBEF00)
		{
			return 3;
		}
		if ((buf << 16) == 0xFEFF0000 && SystemFun::IsLittleMode() ||
			(buf << 16) == 0xFFFE0000 && !SystemFun::IsLittleMode())
		{
			return 2;
		}
		if (buf == 0xFEFF0000 && SystemFun::IsLittleMode()  ||
			buf == 0xFFFE0000 && !SystemFun::IsLittleMode())
		{
			return 4;
		}
		return 0;
	}
	//返回值：0:asc; 1:UTF-8; 2:UTF-16; 3:UTF-32	
	static int jmup_unicode_head(FILE * & file)
	{
		char buf[4] = {0};
		fread_s(buf, sizeof(buf), sizeof(buf), 1, file);

		if (buf[0] == 0xEF && buf[1] == 0xBB && buf[2] == 0xBF)
		{
			fseek(file, 2, SEEK_SET);
			return 1;
		}
		if (buf[0]==0xFF && buf[1]==0xFE && SystemFun::IsLittleMode() ||
				 buf[0]==0xFE && buf[1]==0xFF && !SystemFun::IsLittleMode())
		{
			fseek(file, 1, SEEK_SET);
			return 2;
		}
		if (buf[0]==0 && buf[1]==0 && buf[2]==0xFF && buf[3]==0xFE && SystemFun::IsLittleMode()  ||
				 buf[0]==0 && buf[1]==0 && buf[2]==0xFE && buf[3]==0xFF && !SystemFun::IsLittleMode())
		{
			fseek(file, 3, SEEK_SET);
			return 3;
		}
		
		fseek(file, 0, SEEK_SET);
		return 0;
	}
	static void rename(const char * name)
	{
		const char* p = strrchr(name, '.');

		string _name(name, p - name);
		int i = 1;
		string __name;
		do
		{
			__name = StrFun::format("%s%d%s", _name.c_str(), i++, p);

		} while (::PathFileExistsA(__name.c_str()));			

		::rename(name, __name.c_str());
	}
};

class MathFun
{
public:
	static bool A2I(char * sValue, int & iValue)
	{
		iValue = atoi(sValue);
		return !(0 == iValue && '0' != sValue[0]);
	}
	static bool A2F(char * sValue, double & fValue)
	{
		fValue = atof(sValue);
		return !(0 == fValue && '0' != sValue[0]);
	}
	//取整
	static long Trunc(double value)
	{
		if (value > 0)
			return (long)floor(value);
		else
			return (long)ceil(value);
	};
	//四舍五入为整数
	static long Round(double value)
	{
		if (value > 0)
			return (long)(value + 0.5);
		else
			return (long)(value - 0.5);
	};
	//将数字Value规范为最靠近的SubValue的整数倍
	static double GetMod(const double Value, const double SubValue)
	{
		if (SubValue != 0)
			return Round(Value / SubValue)*SubValue;
		else
			return Value;
	}
	static int GetMod(const int Value, const int SubValue)
	{
		if (SubValue != 0)
			return Round(Value / SubValue)*SubValue;
		else
			return Value;
	}
	//是否整除
	static bool IsDiv(const double Value, const double SubValue, const double ErrorDiff = 1e-5)
	{
		double n = 0;
		double m = modf(abs(Value / SubValue), &n);
		return m < ErrorDiff || 1 - m < ErrorDiff; //误差使值变大或误差使值变小两钟情况处理
	}
	//四舍五入到指定精度, Precision介于正负37之间
	static double SimpleRoundTo(const double Value, const int Precision = 2)
	{
		double LFactor = pow(10.0, -Precision);
		if (Value < 0)
			return Trunc((Value / LFactor) - 0.5) * LFactor;
		else
			return Trunc((Value / LFactor) + 0.5) * LFactor;
	}
	//判断两浮点数是否相等
	static bool SameValue(const double A, const double B, double Epsilon = 0)
	{
		if (Epsilon == 0)
			Epsilon = max(min(abs(A), abs(B)) * (1e-15) * 1000, (1e-15) * 1000);
		if (A > B)
			return (A - B) <= Epsilon;
		else
			return (B - A) <= Epsilon;
	}
	static int CmpValue(const double A, const double B, double Epsilon = 0)
	{
		if (SameValue(A, B, Epsilon))
			return 0;
		else if (A > B)
			return 1;
		else
			return -1;
	}
	//求最大公约数
	//Greatest Common Divisor}
	static int GCD(const int x, const int y)
	{
		int a = x;
		int b = y;

		int n = b;
		if (a < y)
		{
			b = a;
			a = n;
		}

		while (n > 0)
		{
			n = a % b;
			a = b;
			if (n > 0)
				b = n;
		}

		return b;
	}
	//求最小公倍数
	//Least Common Multiple}
	static int LCM(const int x, const int y)
	{
		return (x * y) / GCD(x, y);
	}
	static double GCD(const double x, const double y, const int precision = 2)
	{
		double L = pow(10.0, precision);

		int a = Round(x * L);
		int b = Round(y * L);

		double R = GCD(a, b);
		return R / L;
	}
	static double LCM(const double x, const double y)
	{
		return (x * y) / GCD(x, y);
	}

	//PI
	static double PI()	{ return 3.1415926535897932384626433832795; };
	//按精度格式化浮点数
	static string GetFormatValue(const double value, const int Precision = 0)
	{
		char FormatStr[50];
		_snprintf_s(FormatStr, "%%.%df", Precision);

		return StrFun::format(FormatStr, value);
	}
	//十六进制转二进制
	static int HexToBin(const char * Text, char * Buffer, int BufSize)
	{
		const int Convert['f' - '0' + 1] =
		{
			0, 1, 2, 3, 4, 5, 6, 7, 8, 9, -1, -1, -1, -1, -1, -1,
			-1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			-1, 10, 11, 12, 13, 14, 15
		};

		uchar * pText = (uchar*)Text;

		int I = BufSize, Count = 0;
		char CurrChar, NextChar;
		while (I > 0)
		{
			CurrChar = *(pText++);
			NextChar = *(pText++);

			if (!(('0' <= CurrChar && CurrChar <= 'f') && ('0' <= NextChar && NextChar <= 'f')))
				break;
			Buffer[0] = char((Convert[CurrChar - '0'] << 4) + Convert[NextChar - '0']);
			Buffer++;
			Count++;
			I--;
		};
		Buffer -= Count;
		return BufSize - I;
	}
	//二进制转十六进制
	static void BinToHex(const char * Buffer, char * Text, int BufSize)
	{
		const char Convert[17] = "0123456789ABCDEF";
		uchar * pBuf = (uchar *)Buffer;
		char * pText = Text;

		for (int I = 0; I < BufSize; I++)
		{
			pText[0] = Convert[pBuf[I] >> 4];
			pText[1] = Convert[pBuf[I] & 0xF];
			pText += 2;
		};
		pText[0] = 0;
	}
	//十六进制转十进制
	static int HexToDec(const char * value)
	{
		int Int;
		sscanf_s(value, "%x", &Int);
		return Int;
	}
	//十进制数转到x进制，ary表示目标进制，ary在2到255之间，126进制之内按数组指定顺序转换，大于126进制按asc码顺序转换
	static string dec_to_x(uint64 value, byte ary)
	{
		if (value == 0)
			return "0";
		if (ary == 10)
		{
			char str[96];
			_i64toa_s(value, str, sizeof(str), 10);
			return str;
		}

		const byte dx = 126;
		const byte low = 1;
		//x_to_dec函数中convert的原文,与其元素一一对应，修改其中一个则另一个也要作相应修改
		const char convert[dx] =
		{
			//48..57 //10
			'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',

			//65..90 //36
			'A', 'B', 'C', 'D', 'E', 'F', 'G',
			'H', 'I', 'J', 'K', 'L', 'M', 'N',
			'O', 'P', 'Q', 'R', 'S', 'T',
			'U', 'V', 'W', 'X', 'Y', 'Z',

			//97..122 //62
			'a', 'b', 'c', 'd', 'e', 'f', 'g',
			'h', 'i', 'j', 'k', 'l', 'm', 'n',
			'o', 'p', 'q', 'r', 's', 't',
			'u', 'v', 'w', 'x', 'y', 'z',

			//32..47 58..64 91..96 123..126 //95
			' ', '!', '"', '#', '$', '%', '&', '\'', '(', ')', '*',
			'+', ',', '-', '.', '/', ':', ';', '<', '=', '>', '?',
			'@', '[', '\\', ']', '^', '_', '`', '{', '|', '}', '~',

			//1..31 //126
			1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
			11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
			21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31
		};

		byte c;
		char result[sizeof(value) * 8 + 1];
		char * p = result + sizeof(result) - 1;
		*p-- = 0;

		while (value != 0)
		{
			c = value % ary;
			if (c < dx)
				*p-- = convert[c];
			else
				*p-- = c + low;
			value /= ary;
		}

		return ++p;
	}

	//x进制数转到十进制，ary表示原始进制，ary在2到255之间，value不能大约int64的最大值，126进制之内按数组指定顺序转换，大于126进制按asc码顺序转换
	static uint64 x_to_dec(const char * value, byte ary)
	{
		if (value == nullptr || value[0] == 0)
			return 0;
		if (ary == 10)
			return _atoi64(value);

		const byte dx = 126;
		const byte low = 1;
		//dec_to_x函数中convert的倒排索引,与其元素一一对应，修改其中一个则另一个也要作相应修改
		const byte convert[dx] =
		{
			95, 96, 97, 98, 99, 100, 101, 102, 103, 104,
			105, 106, 107, 108, 109, 110, 111, 112, 113, 114,
			115, 116, 117, 118, 119, 120, 121, 122, 123, 124,
			125, 62, 63, 64, 65, 66, 67, 68, 69, 70,
			71, 72, 73, 74, 75, 76, 77, 0, 1, 2,
			3, 4, 5, 6, 7, 8, 9, 78, 79, 80,
			81, 82, 83, 84, 10, 11, 12, 13, 14, 15,
			16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
			26, 27, 28, 29, 30, 31, 32, 33, 34, 35,
			85, 86, 87, 88, 89, 90, 36, 37, 38, 39,
			40, 41, 42, 43, 44, 45, 46, 47, 48, 49,
			50, 51, 52, 53, 54, 55, 56, 57, 58, 59,
			60, 61, 91, 92, 93, 94
		};


		uint64 result = 0;
		uint64 dot = 1;
		byte * p = (byte *)value + strlen(value) - 1;
		while (p >= (byte *)value)
		{
			if (*p <= dx)
				result += (convert[*p - low] * dot);
			else
				result += ((*p - low) * dot);

			dot *= ary;

			p--;
		}

		return result;
	}
	//x进制转到y进制，126进制之内按数组指定顺序转换，大于126进制按asc码顺序转换
	static string x_to_y(const char * sour, byte sour_ary, byte dest_ary)
	{
		if (sour_ary == dest_ary)
			return sour;
		else
		{
			uint64 n = x_to_dec(sour, sour_ary);
			return dec_to_x(n, dest_ary);
		}
	}
	static double _log(double x, double y)
	{
		return log(y) / log(x);
	}
	static void NewGUID(string & Str)
	{
		GUID guid;
		if (S_OK == ::CoCreateGuid(&guid))
		{
			 StrFun::format(Str, "{%08X-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X}"
				, guid.Data1
				, guid.Data2
				, guid.Data3
				, guid.Data4[0], guid.Data4[1]
				, guid.Data4[2], guid.Data4[3]
				, guid.Data4[4], guid.Data4[5]
				, guid.Data4[6], guid.Data4[7]
				);
		}
	}
	static void simple_guid(string & Str)
	{
		GUID guid;
		if (S_OK == ::CoCreateGuid(&guid))
		{
			StrFun::format(Str, "%X%x%X%x%X%x%X%x%X%x%X"
				, guid.Data1
				, guid.Data2
				, guid.Data3
				, guid.Data4[0], guid.Data4[1]
				, guid.Data4[2], guid.Data4[3]
				, guid.Data4[4], guid.Data4[5]
				, guid.Data4[6], guid.Data4[7]
				);
		}
	}
	static string FormatThousand(const char * Fmt, double value)
	{
		char sResult[512];
		FormatThousand(Fmt, value, sResult, sizeof(sResult));
		return sResult;
	}
	//将数字格式化为带千分位的格式
	template<typename NUMBER>
	static int FormatThousand(const char * Fmt, NUMBER value, char * Buf, size_t BufLen)
	{
		char sSrc[512];
		char sDst[512];
		int iLen = _snprintf_s(sSrc, Fmt, value);

		//小数点位置
		int iStart = iLen;
		char * P = strchr(sSrc, '.');
		if (P)
			iStart = P - sSrc;

		int FlagCount = (iStart - 1 - int(!isdigit(sSrc[0]))) / 3;	//千分位个数
		int iDstLen = iLen + FlagCount;								//目标缓冲长度

		int iCount = 0;
		int j = iDstLen;
		for (int i = iLen; i >= 0; i--)
		{
			sDst[j--] = sSrc[i];
			if (i < iStart && j>0 && isdigit(sSrc[i]))
			{
				iCount++;
				if (iCount == 3)
				{
					sDst[j--] = ',';
					iCount = 0;
				}
			}
		}

		strncpy_s(Buf, BufLen, sDst);

		return iDstLen;
	}
	static uint get_precision(const double & value, const uint max_precision = 8)
	{
		uint result = 0;
		double dx = 1.0;
		double min_dx = pow(0.1, max_precision);
		double _value = abs(value) + min_dx / 10; 
		while (fmod(_value, dx) >= min_dx)
		{
			dx /= 10;
			result++;
		}
		return result;
	}
	static double FloatToDouble(const float Value, int Precision = 6)
	{
		Precision = max(7, Precision);
		int64 iValue = (int64)(Value * powf(10.0, (float)Precision));
		return iValue / pow(10.0, Precision);
	}
	static float DoubleToFloat(const double & Value, int Precision = 10)
	{
		int64 iValue = (int64)(Value * pow(10.0, Precision));
		return iValue / powf(10.0, (float)Precision);;
	}
	static bool BOOLTobool(bool value)
	{
		return value == TRUE;
	}
	static bool boolToBOOL(bool value)
	{
		return value == true;
	}
	//字符串转数字，非数字字符转为0
	static uint s_to_i(const char * s)
	{
		if (s == nullptr)
			return 0;

		uint ret = 0;
		while (*s)
		{
			ret = ret * 10;
			if (isdigit(*s))
				ret += *s - '0';

			s++;
		}
		return ret;
	}
};

//一般大数运算可以分解为一个整数数组的运算，最后将结果数组拼接成字符串输出，这里需求简单，没有这样作
//x进制大数加法，用于生成按字符序递增的本地号
class big_num
{
private:
	char m_zero;//进制内1位数最小值，类似十进制的0
	char m_dx;	//进制数，类似十进制的10

	//大数加法，一般的大数运算不这样实现，纯粹为了生成递增的本地号
	string add(const char * a, const char * b)
	{
		if (!a || !b)
			return string(1, m_zero);

		int len_a = (int)strlen(a);
		int len_b = (int)strlen(b);

		string res((len_a > len_b ? len_a : len_b) + 2, m_zero);

		int ex = 0;
		int t = 0;

		const char* pa = a + len_a - 1;
		const char* pb = b + len_b - 1;

		int i = res.length() - 1;
		while (pa >= a || pb >= b || ex)
		{
			if (pa >= a)
				ex += (*pa--) - m_zero;

			if (pb >= b)
				ex += (*pb--) - m_zero;

			t = ex % m_dx;
			ex = ex / m_dx;

			res[i--] = t + m_zero;
			_ASSERT(i >= 0);
		}

		return res.substr(++i);
	}
public:
	//min_define:进制内1位数最小值，类似十进制的0; max_define:进制内1位数最大值，类似十进制的9
	//默认为95进制，每位数字均为可打印字符
	big_num(char min_define = 32, char max_define = 126)
		: m_zero(min_define)
		, m_dx(max_define - min_define + 1)
	{
		//参数错误时恢复到默认值
		if (min_define >= max_define)
		{
			m_zero = 32;
			m_dx = 126;
		}
	}
	string next_id(const char * current_id)
	{
		char b[2] = { m_zero + 1, 0 };
		return add(current_id, b);
	}
};


#include <memory> 
#include <iostream> 
class StlFun
{
public:
	//将Map中的结构体存到文件
	template <class FILENAME, class MAP, class HEAD> 
	static void SaveToFileBase(const FILENAME & FileName, MAP & Map, HEAD & Head)
	{
		string name(FileName);
		FILE * file = _fsopen(name.c_str(), "wbS", _SH_DENYWR);
		if (!file)
			return;	

		fwrite(&Head, sizeof(Head), 1, file);	//保存文件头		
		MAP::key_type Key;
		MAP::mapped_type Item;
		MAP::iterator iter;
		for (iter = Map.begin(); iter != Map.end(); iter++)
		{
			Key = iter->first;
			Item = iter->second;
			fwrite(&Key, sizeof(Key), 1, file);
			fwrite(&Item, sizeof(Item), 1, file);
		}
		fclose(file);
	}
	//将文件中的结构体读到Map，该结构体必须有一个Key()函数返回其加入Map所用的关键字
	template <class FILENAME, class MAP, class HEAD> 
	static bool LoadFormFileBase(const FILENAME & FileName, MAP & Map, HEAD & Head)
	{
		//读取Order
		string name(FileName);
		FILE * file = _fsopen(name.c_str(), "rbS", _SH_DENYWR);
		if (!file)
			return false;	
		
		MAP::key_type Key;
		MAP::mapped_type Item;
		long KeyLeng = sizeof(Key);
		long ItemLeng = sizeof(Item);
		long file_size = _filelength(_fileno(file));
		if ((file_size - sizeof(Head)) % ItemLeng != 0)
		{
			fclose(file);
			return false;
		}
		
		fseek(file, 0, SEEK_SET);
		fread_s(&Head, sizeof(Head), sizeof(Head), 1, file);//读取文件头
		Map.clear();
		for (long i = 0; i < (file_size - (long)sizeof(Head)) / ItemLeng; i++)
		{
			fread_s(&Key, KeyLeng, KeyLeng, 1, file);
			fread_s(&Item, ItemLeng, ItemLeng, 1, file);
			Map.insert(MAP::value_type(Key, Item));
		}
		fclose(file);
		return true;
	}
	//将Map中的结构体存到文件
	template <class FILENAME, class MAP, class HEAD> 
	static void SaveToFile(const FILENAME & FileName, MAP & Map, HEAD & Head)
	{
		string name(FileName);
		FILE * file = _fsopen(name.c_str(), "wbS", _SH_DENYWR);
		if (!file)
			return;	

		fwrite(&Head, sizeof(Head), 1, file);	//保存文件头
		MAP::mapped_type Item;
		MAP::iterator iter;
		for (iter = Map.begin(); iter != Map.end(); iter++)
		{
			Item = iter->second;
			fwrite(Item, sizeof(*Item), 1, file);	//保存文件头
		}
		fclose(file);
	}
	//将文件中的结构体读到Map，该结构体必须有一个Key()函数返回其加入Map所用的关键字
	template <class FILENAME, class MAP, class HEAD> 
	static bool LoadFormFile(const FILENAME & FileName, MAP & Map, HEAD & Head)
	{
		//读取Order
		string name(FileName);
		FILE * file = _fsopen(name.c_str(), "rbS", _SH_DENYWR);
		if (!file)
			return false;	

		MAP::mapped_type Item;
		long ItemLeng = sizeof(*Item);
		long file_size = _filelength(_fileno(file));
		if ((file_size - sizeof(Head)) % ItemLeng != 0)
		{
			fclose(file);
			return false;
		}
		
		fseek(file, 0, SEEK_SET);
		fread_s(&Head, sizeof(Head), sizeof(Head), 1, file);//读取文件头
		Map.clear();
		for (long i = 0; i < (file_size - (long)sizeof(Head)) / ItemLeng; i++)
		{
			Item = (MAP::mapped_type)malloc(ItemLeng);
			fread_s(Item, ItemLeng, ItemLeng, 1, file);
			pair<MAP::iterator, bool> ret = Map.insert(MAP::value_type(MAP::key_type(Item->Key()), Item));
			if (!ret.second && ret.first->second != Item)
			{
				free(ret.first->second);
				ret.first->second = Item;
			}
		}
		fclose(file);
		return true;
	}
	//将文件中的结构体读到
	template <class FILENAME, class VECTOR, class HEAD> 
	static bool LoadFormFileToVector(const FILENAME & FileName, VECTOR & Vector, HEAD & Head)
	{
		//读取Order
		string name(FileName);
		FILE * file = _fsopen(name.c_str(), "rbS", _SH_DENYWR);
		if (!file)
			return false;	
		
		VECTOR::value_type Item;
		long ItemLeng = sizeof(*Item);
		long file_size = _filelength(_fileno(file));
		if ((file_size - sizeof(Head)) % ItemLeng != 0)
		{
			fclose(file);
			return false;
		}
		
		fseek(file, 0, SEEK_SET);
		fread_s(&Head, sizeof(Head), sizeof(Head), 1, file);//读取文件头
		Vector.clear();
		for (long i = 0; i < (file_size - (long)sizeof(Head)) / ItemLeng; i++)
		{
			Item = (VECTOR::value_type)malloc(ItemLeng);
			fread_s(Item, ItemLeng, ItemLeng, 1, file);
			Vector.push_back(Item);
		}
		fclose(file);
		return true;
	}
	
	//将文件中的结构体读到
	template <class FILENAME, class ITEM, class HEAD> 
	static bool LoadFormFileEx(const FILENAME & FileName, bool (CALLBACK *head_fun)(HEAD & head), void (CALLBACK *item_fun)(ITEM & item))
	{
		//读取Order
		string name(FileName);
		FILE * file = _fsopen(name.c_str(), "rbS", _SH_DENYWR);
		if (!file)
			return false;	
		
		long HeadLeng = sizeof(HEAD);
		long ItemLeng = sizeof(ITEM);
		long file_size = _filelength(_fileno(file));
		if ((file_size - HeadLeng) % ItemLeng != 0)
		{
			fclose(file);
			return false;
		}
		
		fseek(file, 0, SEEK_SET);

		HEAD Head;
		fread_s(&Head, HeadLeng, HeadLeng, 1, file);//读取文件头
		if (!head_fun(Head))
		{
			fclose(file);
			return false;
		}

		ITEM Item;
		for (long i = 0; i < (file_size - HeadLeng) / ItemLeng; i++)
		{
			fread_s(&Item, ItemLeng, ItemLeng, 1, file);
			item_fun(Item);
		}
		fclose(file);
		return true;

	}		
	template <class FILENAME, class DATAS, class HEAD> static void SaveToFileAt(const FILENAME & FileName, DATAS & datas, HEAD & Head)
	{
		string name(FileName);
		FILE * file = _fsopen(name.c_str(), "wbS", _SH_DENYWR);
		if (!file)
			return;	

		fwrite(&Head, sizeof(Head), 1, file);	//保存文件头
		for (size_t i = 0; i < datas.size(); i++)
			fwrite(datas.at(i), sizeof(*datas.at(i)), 1, file);	
		fclose(file);
	}
	//将Map中的数据全部拷贝到数组里，返回数组首指针//需要在外部用free释放返回的指针 
	template <class MAP> static void * MapToArray(MAP & Map, uint & Count, bool (CALLBACK *pFilterFunc)(void *) = nullptr)
	{
		Count = Map.size();
		if (Count == 0)
			return nullptr;
		MAP::mapped_type * pData = (MAP::mapped_type *)malloc(Count * sizeof(MAP::mapped_type));
		MAP::iterator iter;
		Count = 0;
		for (iter = Map.begin(); iter != Map.end(); iter++)
		{
			*pData = iter->second;
			if (pFilterFunc == nullptr || pFilterFunc(*pData))
			{
				pData++;
				Count++;
			}
		}
		pData -= Count;
		if (Count == 0)
		{
			free(pData);
			return nullptr;
		}
		if ((size_t)Count < Map.size())
		{
			void * pNew = realloc(pData, Count*sizeof(MAP::mapped_type));
			pData = (MAP::mapped_type *)pNew;
		}
		return pData;
	};
	template <class MAP, class _Class, typename _pMemberFunc>
	static void * MapToArray(MAP & Map, uint & Count, _Class * pClass, _pMemberFunc pMemberFunc)
	{
		typedef bool (CALLBACK * _pFilterFunc)(void *);
		ACCallback<_pFilterFunc, _Class, _pMemberFunc> mFilterFunc(pClass, pMemberFunc);

		return MapToArray(Map, Count, (_pFilterFunc)mFilterFunc);
	}

	//将Map中的数据全部拷贝到List 或 Vector里 
	template <class LIST, class IDATA> static void ListToIData(LIST & Sour, IDATA & Dest, bool (CALLBACK *pFilterFunc)(void *) = nullptr)
	{
		if (Sour.size() == 0)
			return;

		for (LIST::iterator iter = Sour.begin(); iter != Sour.end(); iter++)
			if (pFilterFunc == nullptr || pFilterFunc(*iter))
				Dest.push_back(*iter);
	};
	template <class LIST, class IDATA, class _Class, typename _pMemberFunc>
	static void ListToIData(LIST & Sour, IDATA & Dest, _Class * pClass, _pMemberFunc pMemberFunc)
	{
		typedef bool (CALLBACK * _pFilterFunc)(void *);
		ACCallback<_pFilterFunc, _Class, _pMemberFunc> mFilterFunc(pClass, pMemberFunc);

		ListToIData(Sour, Dest, (_pFilterFunc)mFilterFunc);
	}
	//将Map中的数据全部拷贝到List 或 Vector里 
	template <class MAP, class LIST> static void MapCopyToList(MAP & Map, LIST & List, bool (CALLBACK *pFilterFunc)(void *) = nullptr)
	{
		if (Map.size() == 0)
			return;

		for (MAP::iterator iter = Map.begin(); iter != Map.end(); iter++)
			if (pFilterFunc == nullptr || pFilterFunc(iter->second))
				List.push_back(iter->second);
	};
	template <class MAP, class LIST, class _Class, typename _pMemberFunc>
	static void MapCopyToList(MAP & Map, LIST & List, _Class * pClass, _pMemberFunc pMemberFunc)
	{
		typedef bool (CALLBACK * _pFilterFunc)(void *);
		ACCallback<_pFilterFunc, _Class, _pMemberFunc> mFilterFunc(pClass, pMemberFunc);

		MapCopyToList(Map, List, (_pFilterFunc)mFilterFunc);
	}
	//将Map中的数据全部拷贝到List 或 Vector里 
	template <class MAP, class LIST> static void MapMoveToList(MAP & Map, LIST * List, bool (CALLBACK *pFilterFunc)(void *) = nullptr)
	{
		if (Map.size() == 0)
			return;

		MAP::iterator iter = Map.begin();
		while (iter != Map.end())
		{
			if (pFilterFunc == nullptr || pFilterFunc(iter->second))
			{
				if (List != nullptr)
					List->push_back(iter->second);
				iter = Map.erase(iter);
			}
			else 
				iter++;
		}
	};
	template <class MAP, class LIST, class _Class, typename _pMemberFunc>
	static void MapMoveToList(MAP & Map, LIST * List, _Class * pClass, _pMemberFunc pMemberFunc)
	{
		if (pClass != nullptr && pMemberFunc != nullptr)
		{
			typedef bool (CALLBACK * _pFilterFunc)(void *);
			ACCallback<_pFilterFunc, _Class, _pMemberFunc> mFilterFunc(pClass, pMemberFunc);
			
			MapMoveToList(Map, List, (_pFilterFunc)mFilterFunc);
		}
		else
			MapMoveToList(Map, List);
	}
	//按关键字返回Map中的数据
	template <class MULTIMAP, typename KEYTYPE> static void * MapFindMulti(MULTIMAP & MultiMap, const KEYTYPE & Key, uint & Count, bool (CALLBACK *pFilterFunc)(void *) = nullptr)
	{
		Count = MultiMap.count(Key);
		if (Count == 0)
			return nullptr;
		MULTIMAP::mapped_type * pData = (MULTIMAP::mapped_type *)malloc(Count * sizeof(MULTIMAP::mapped_type));
		MULTIMAP::iterator iter;
		Count = 0;
		for (iter = MultiMap.lower_bound(Key); iter != MultiMap.upper_bound(Key); iter++)
		{
			*pData = iter->second;
			if (pFilterFunc == nullptr || pFilterFunc(*pData))
			{
				pData++;
				Count++;
			}
		}
		pData -= Count;
		if (Count == 0)
		{
			free(pData);
			return nullptr;
		}
		if ((size_t)Count < MultiMap.count(Key))
		{
			void * pNew = realloc(pData, Count*sizeof(MULTIMAP::mapped_type));
			pData = (MULTIMAP::mapped_type *)pNew;
		}
		return pData;
	};
	template <class MULTIMAP, typename KEYTYPE, class _Class, typename _pMemberFunc>
	static void * MapFindMulti(MULTIMAP & MultiMap, const KEYTYPE & Key, uint & Count, _Class * pClass, _pMemberFunc pMemberFunc)
	{
		typedef bool (CALLBACK * _pFilterFunc)(void *);
		ACCallback<_pFilterFunc, _Class, _pMemberFunc> mFilterFunc(pClass, pMemberFunc);

		return MapFindMulti(MultiMap, Key, Count, (_pFilterFunc)mFilterFunc);
	}
	//按关键字返回Map中的数据
	template <class MULTIMAP, class LIST, typename KEYTYPE> static void MapFindMulti(MULTIMAP & MultiMap, const KEYTYPE & Key, LIST & List, bool (CALLBACK *pFilterFunc)(void *) = nullptr)
	{
		if (MultiMap.count(Key) == 0)
			return;

		for (MULTIMAP::iterator iter = MultiMap.lower_bound(Key); iter != MultiMap.upper_bound(Key); iter++)
		{
			if (pFilterFunc == nullptr || pFilterFunc(iter->second))
				List.push_back(iter->second);
		}
	};
	template <class MULTIMAP, class LIST, typename KEYTYPE, class _Class, typename _pMemberFunc>
	static void MapFindMulti(MULTIMAP & MultiMap, const KEYTYPE & Key, LIST & List, _Class * pClass, _pMemberFunc pMemberFunc)
	{
		typedef bool (CALLBACK * _pFilterFunc)(void *);
		ACCallback<_pFilterFunc, _Class, _pMemberFunc> mFilterFunc(pClass, pMemberFunc);

		MapFindMulti(MultiMap, Key, List, (_pFilterFunc)mFilterFunc);
	}
	//按关键字返回Map中的数据
	template <class MAP, typename KEYTYPE> static void * MapFindOnly(MAP & Map, const KEYTYPE & Key)
	{
		MAP::iterator iter = Map.find(Key);
		if (iter == Map.end())
			return nullptr;
		return reinterpret_cast<void *>(iter->second);
	};
	//按关键字返回Map中的数据
	template <class MAP, typename KEYTYPE, typename VALUETYPE> static VALUETYPE MapFindOnly(MAP & Map, const KEYTYPE & Key, const VALUETYPE & def)
	{
		//if (typeid(MAP::key_type) != typeid(KEYTYPE))
		//	return def;

		MAP::iterator iter = Map.find(Key);
		if (iter == Map.end())
			return def;
		return iter->second;
	};
	//将数据存储到Map
	template <class MAP, class RECORD, typename KEYTYPE> static RECORD CiteToMultiMap(MAP & Map, RECORD & Data, const KEYTYPE & Key)
	{
		return Map.insert(MAP::value_type(Key, Data))->second;
	}
	//将数据存储到Map
	template <class MAP, class RECORD, typename KEYTYPE> static RECORD * CiteToMap(MAP & Map, RECORD & Data, const KEYTYPE & Key)
	{
		pair<MAP::iterator, bool> ret = Map.insert(MAP::value_type(Key, Data));

		MAP::iterator iter = ret.first;
		if (!ret.second && memcmp(&iter->second, &Data, sizeof(RECORD))!=0)
			iter->second = Data;

		return &iter->second;
	}
	//重新创建一份数据，并将其指针保存到Map
	template <class MAP, class RECORD, typename KEYTYPE> static RECORD * SaveToMultiMap(MAP & Map, RECORD & Data, const KEYTYPE & Key)
	{
		RECORD * pData = new RECORD;
		*pData = Data;
		Map.insert(MAP::value_type(Key, pData));
		return pData;
	}
	//重新创建一份数据，并将其指针保存到Map
	template <class MAP, class RECORD, typename KEYTYPE> static RECORD * SaveToMap(MAP & Map, RECORD & Data, const KEYTYPE & Key)
	{
		pair<MAP::iterator, bool> ret = Map.insert(MAP::value_type(Key, &Data));
		if (ret.second)
			ret.first->second = new RECORD;
		if (ret.first->second != &Data)
			*(ret.first->second) = Data;

		return ret.first->second;
	}
	template <class LIST, class RECORD> static RECORD * SaveToList(LIST & List, RECORD & Data)
	{
		RECORD * pData = new RECORD;
		*pData = Data;
		List.push_back(pData);
		return pData;
	}
	//将List中的数据全部拷贝到数组里，返回数组首指针//需要在外部用free释放返回的指针 
	template <class LIST> static void * ListToArray(LIST & List, uint & Count, bool (CALLBACK *pFilterFunc)(void *) = nullptr)
	{
		Count = List.size();
		if (Count == 0)
			return nullptr;
		LIST::value_type * pData = (LIST::value_type *)malloc(Count * sizeof(LIST::value_type));
		LIST::iterator iter;
		Count = 0;
		for (iter = List.begin(); iter != List.end(); iter++)
		{
			*pData = *iter;
			if (pFilterFunc == nullptr || pFilterFunc((void *)*pData))
			{
				pData++;
				Count++;
			}
		}
		pData -= Count;
		if (Count == 0)
		{
			free(pData);
			return nullptr;
		}
		if ((size_t)Count < List.size())
		{
			void * pNew = realloc(pData, Count*sizeof(LIST::value_type));
			pData = (LIST::value_type *)pNew;
		}
		return pData;
	};

	//释放Map中的指针，并清空Map
	template <class MAP> static void DeleteMap(MAP & Map)
	{
		for (MAP::iterator iter = Map.begin(); iter != Map.end(); iter++)
			if (iter->second)
				delete iter->second;
		Map.clear();
	};
	//释放Vector中的指针，并清空Map
	template <class VECTOR> static void DeleteVector(VECTOR & Vector)
	{
		for (VECTOR::iterator iter = Vector.begin(); iter != Vector.end(); iter++)
			if (*iter)
				delete (*iter);
		Vector.clear();
	};
	//释放List中的指针，并清空List
	template <class LIST> static void DeleteList(LIST & List)
	{
		for (LIST::iterator iter = List.begin(); iter != List.end(); iter++)
			if (*iter)
				delete (*iter);
		List.clear();
	};
	template <class _LIST> static void DeleteListEx(_LIST & List)
	{
		for (size_t i = 0; i < List.size(); i++)
			delete (List[i]);
		List.clear();
	};

	////////////////////////////////////////////////////////////////////////////////////
	//将普通指针转为共享指针
	template<typename _Ty> shared_ptr<_Ty> to_shared_ptr(_Ty * p)
	{
		shared_ptr<_Ty> q(p);
		return q;
	}
};

class ValueIn
{
public:
	//返回一个值在一个同类型的数组之内的索引位置
	template<typename TYPE, size_t LENG> static int IndexOf(TYPE Value, TYPE(&Array)[LENG])
	{
		for (int i = 0; i < LENG; i++)
			if (Value == Array[i])
				return i;
		return -1;
	}
	//返回一个值是否在一个同类型的数组内
	template<typename TYPE, size_t LENG> static bool In(TYPE Value, TYPE(&Array)[LENG])
	{
		return IndexOf(Value, Array) >= 0;
	}
	//返回一个值在一个同类型的lst列表之内的索引位置
	template<class TYPE> static int IndexOf(TYPE Value, int ItemCount, va_list lst)
	{
		for (int i = 0; i < ItemCount; i++)
			if (Value == va_arg(lst, TYPE))
				return i;
		return -1;
	}
	//以下两个函数可以替代在数组中查找某值，其速度比数组操作快一些，比switch语句慢很多
	//返回一个值在一组同类型的数据集之内的索引位置
	template<class TYPE> static int IndexOf(TYPE Value, int ItemCount, ...)
	{
		va_list arg_ptr;
		int Result = -1;

		va_start(arg_ptr, ItemCount);
		Result = IndexOf(Value, ItemCount, arg_ptr);
		va_end(arg_ptr);

		return Result;
	}
	//判断一个值是否存在于同一类型的数据集之内，类似集合的In功能
	template<class TYPE> static bool In(TYPE Value, int ItemCount, ...)
	{
		va_list arg_ptr;
		bool Result = false;

		va_start(arg_ptr, ItemCount);
		Result = IndexOf(Value, ItemCount, arg_ptr) >= 0;
		va_end(arg_ptr);

		return Result;
	}
	static bool ValidPassword(string Psw, int PswSafeClass = 1, uint MinLength = 8)
	{
		if (PswSafeClass == 0)
			return true;

		bool bUpper = false;
		bool bLower = false;
		bool bDigit = false;
		for (uint i = 0; i < Psw.length(); i++)
		{
			bUpper = bUpper || isupper(Psw.at(i));
			bLower = bLower || islower(Psw.at(i));
			bDigit = bDigit || isdigit(Psw.at(i));
			if (bUpper && bLower && bDigit)
				break;
		}
		return bUpper && bLower && bDigit && Psw.length() >= MinLength;
	}
};

//Lab颜色操作，自己定义的位保存方式，不知道标准的保存方式什么样
#define Lab(r,g,b)          ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)))
#define GetLValue(lab)      (LOBYTE(lab))
#define GetaValue(lab)      (char)(LOBYTE(((WORD)(lab)) >> 8))
#define GetbValue(lab)      (char)(LOBYTE((lab)>>16))
class DrawFun
{
public:
	static SIZE GetTextSize(HWND hWnd)
	{
		int leng = ::GetWindowTextLength(hWnd);
		char * pText = (char *)malloc(leng + 1);
		::GetWindowTextA(hWnd, pText, leng + 1);
		HDC dc = ::GetDC(hWnd);
		SIZE size;
		::GetTextExtentPoint32A(dc, pText, leng, &size);
		ReleaseDC(hWnd, dc);
		free(pText);

		return size;
	}
	static COLORREF GetBrushColor(LOGBRUSH lBrsh)
	{
		return lBrsh.lbColor;
	}
	static COLORREF ColorToRGB(COLORREF Color)
	{
		if (Color < 0)
			return GetSysColor(Color & 0x000000FF);
		else
			return Color;
	}
	static COLORREF AlphaColor(COLORREF Color, COLORREF BkColor, BYTE Alpha)//透明色
	{
		int r, g, b, bkr, bkg, bkb;

		Color = ColorToRGB(Color);
		r = Color & 0x000000FF;
		g = (Color & 0x0000FF00) >> 8;
		b = (Color & 0x00FF0000) >> 16;

		BkColor = ColorToRGB(BkColor);
		bkr = BkColor & 0x000000FF;
		bkg = (BkColor & 0x0000FF00) >> 8;
		bkb = (BkColor & 0x00FF0000) >> 16;

		r = (bkr * (255 - Alpha) + r * Alpha) / 255;
		g = (bkg * (255 - Alpha) + g * Alpha) / 255;
		b = (bkb * (255 - Alpha) + b * Alpha) / 255;

		return RGB(r, g, b);
	}
	//过渡色 Count总步长, Step当前步长
	static COLORREF GetGradientColor(COLORREF First, COLORREF Last, UINT Count, UINT Step)
	{
		COLORREF sCol = ColorToRGB(First);
		byte	sR = byte(sCol & 0x000000FF);
		byte	sG = byte((sCol & 0x0000FF00) >> 8);
		byte	sB = byte((sCol & 0x00FF0000) >> 16);

		COLORREF eCol = ColorToRGB(Last);
		byte	eR = byte(eCol & 0x000000FF);
		byte	eG = byte((eCol & 0x0000FF00) >> 8);
		byte	eB = byte((eCol & 0x00FF0000) >> 16);

		byte	R = sR + (eR - sR) * Step / Count;
		byte	G = sG + (eG - sG) * Step / Count;
		byte	B = sB + (eB - sB) * Step / Count;

		return RGB(R, G, B);
	}
	//绘制过渡色画刷
	static HBRUSH GetGradientBrush(HDC  hDC, int Height, COLORREF First, COLORREF Last)
	{
		HDC mDC = ::CreateCompatibleDC(hDC);						//建立与屏幕显示兼容的内存显示设备 
		HBITMAP Bitmap = ::CreateCompatibleBitmap(hDC, 1, Height);	//建立一个与屏幕显示兼容的位图，位图的大小可以与窗口的大小一样 
		HGDIOBJ OldBit = ::SelectObject(mDC, Bitmap);				//将位图选入到内存显示设备中, 因为只有选入了位图的内存显示设备才有地方绘图，画到选入的位图上 

		for (int i = 0; i <= Height; i++)
		{
			RECT rect = { 0, i, 1, i };
			HBRUSH brush = ::CreateSolidBrush(GetGradientColor(First, Last, Height, i));
			::FillRect(mDC, &rect, brush);
			::DeleteObject(brush);
		}

		HBRUSH Result = ::CreatePatternBrush(Bitmap);

		//绘图完成后的清理 
		::SelectObject(mDC, OldBit);
		::DeleteObject(Bitmap);
		::DeleteDC(mDC);

		return Result;
	}
	static COLORREF RGBToLab(COLORREF RGB)//RGB颜色转Lab颜色
	{
		double BLACK = 20;
		double YELLOW = 70;
		double R = GetRValue(RGB);
		double G = GetGValue(RGB);
		double B = GetBValue(RGB);

		double L, a, b, X, Y, Z, fX, fY, fZ;
		L = a = b = X = Y = Z = fX = fY = fZ = 0;

		X = 0.412453*R + 0.357580*G + 0.180423*B;
		Y = 0.212671*R + 0.715160*G + 0.072169*B;
		Z = 0.019334*R + 0.119193*G + 0.950227*B;

		X /= (255 * 0.950456);
		Y /= 255;
		Z /= (255 * 1.088754);

		if (Y > 0.008856)
		{
			fY = pow(Y, 1.0 / 3.0);
			L = 116.0*fY - 16.0;
		}
		else
		{
			fY = 7.787*Y + 16.0 / 116.0;
			L = 903.3*Y;
		}

		if (X > 0.008856)
			fX = pow(X, 1.0 / 3.0);
		else
			fX = 7.787*X + 16.0 / 116.0;

		if (Z > 0.008856)
			fZ = pow(Z, 1.0 / 3.0);
		else
			fZ = 7.787*Z + 16.0 / 116.0;

		a = 500.0*(fX - fY);
		b = 200.0*(fY - fZ);

		if (L < BLACK)
		{
			a *= exp((L - BLACK) / (BLACK / 4));
			b *= exp((L - BLACK) / (BLACK / 4));
			L = BLACK;
		}
		if (b > YELLOW)
			b = YELLOW;

		return Lab(L, a, b);
	}
	static COLORREF LabToRGB(COLORREF aLab)//Lab颜色转RGB颜色
	{
		double L = GetLValue(aLab);
		double a = GetaValue(aLab);
		double b = GetbValue(aLab);

		double X, Y, Z, fX, fY, fZ, RR, GG, BB;
		X = Y = Z = fX = fY = fZ = RR = GG = BB = 0;

		fY = pow((L + 16.0) / 116.0, 3.0);
		if (fY < 0.008856)
			fY = L / 903.3;
		else
			Y = fY;

		if (fY > 0.008856)
			fY = pow(fY, 1.0 / 3.0);
		else
			fY = 7.787 * fY + 16.0 / 116.0;

		fX = a / 500.0 + fY;
		if (fX > 0.206893)
			X = pow(fX, 3.0);
		else
			X = (fX - 16.0 / 116.0) / 7.787;

		fZ = fY - b / 200.0;
		if (fZ > 0.206893)
			Z = pow(fZ, 3.0);
		else
			Z = (fZ - 16.0 / 116.0) / 7.787;

		X *= (0.950456 * 255);
		Y *= 255;
		Z *= (1.088754 * 255);

		RR = 3.240479*X - 1.537150*Y - 0.498535*Z;
		GG = -0.969256*X + 1.875992*Y + 0.041556*Z;
		BB = 0.055648*X - 0.204043*Y + 1.057311*Z;

		BYTE R = (byte)(RR < 0 ? 0 : RR > 255 ? 255 : RR);
		BYTE G = (byte)(GG < 0 ? 0 : GG > 255 ? 255 : GG);
		BYTE B = (byte)(BB < 0 ? 0 : BB > 255 ? 255 : BB);

		return RGB(R, G, B);
	}

	//排序箭头
	static void DrawArrow(const HDC hDC, const RECT & rect, COLORREF & colBk, const bool Asc = true)
	{
		POINT Points[3];
		if (Asc)
		{
			Points[0].x = rect.left;
			Points[0].y = rect.bottom;
			Points[1].x = rect.right;
			Points[1].y = rect.bottom;
			Points[2].x = rect.right - (rect.right - rect.left) / 2;
			Points[2].y = rect.top;
		}
		else
		{
			Points[0].x = rect.right;
			Points[0].y = rect.top;
			Points[1].x = rect.left;
			Points[1].y = rect.top;
			Points[2].x = rect.right - (rect.right - rect.left) / 2;
			Points[2].y = rect.bottom;
		}

		COLORREF oldBk = ::SetBkColor(hDC, colBk);
		Polygon(hDC, &Points[0], sizeof(Points) / sizeof(POINT));
		::SetBkColor(hDC, oldBk);
	}
	//亮度调整,offset在-100到100之间
	static COLORREF OffsetLight(COLORREF rgb_color, int offset = 20)
	{
		COLORREF lab_color = RGBToLab(rgb_color);
		BYTE L = (GetLValue(lab_color) + offset) % 100;
		return LabToRGB(Lab(L, GetaValue(lab_color), GetbValue(lab_color)));
	}

	//排序箭头
	static void DrawArrow(const HDC hDC, const RECT & rect, const bool Asc = true)
	{
		BYTE ofset = 100 / 3;
		COLORREF colBk = ::GetPixel(hDC, (rect.right + rect.left) / 2, (rect.top + rect.bottom) / 2);
		COLORREF colLab = RGBToLab(colBk);
		BYTE L = GetLValue(colLab);
		BYTE L1, L2;
		if (L + ofset > 100)
		{
			L1 = L - ofset;
			L2 = L - ofset * 2;
		}
		else if (L - ofset < 0)
		{
			L1 = L + ofset * 2;
			L2 = L + ofset;
		}
		else
		{
			L1 = L + ofset;
			L2 = L - ofset;
		}
		COLORREF colLight = LabToRGB(Lab(L1, GetaValue(colLab), GetbValue(colLab)));
		COLORREF colShadow = LabToRGB(Lab(L2, GetaValue(colLab), GetbValue(colLab)));

		HPEN penLight = ::CreatePen(PS_SOLID, 0, colLight);
		HPEN penShadow = ::CreatePen(PS_SOLID, 0, colShadow);
		if (Asc)
		{
			HGDIOBJ OldPen = ::SelectObject(hDC, penLight);
			MoveToEx(hDC, rect.left, rect.bottom, nullptr);
			LineTo(hDC, rect.right, rect.bottom);
			LineTo(hDC, rect.right - (rect.right - rect.left) / 2, rect.top);

			SelectObject(hDC, &penShadow);
			LineTo(hDC, rect.left - 1, rect.bottom + 1);
			SelectObject(hDC, OldPen);
		}
		else
		{
			HGDIOBJ OldPen = SelectObject(hDC, penShadow);
			MoveToEx(hDC, rect.right, rect.top, nullptr);
			LineTo(hDC, rect.left, rect.top);
			LineTo(hDC, rect.right - (rect.right - rect.left) / 2, rect.bottom);

			SelectObject(hDC, &penLight);
			LineTo(hDC, rect.right, rect.top);
			SelectObject(hDC, OldPen);
		}
		DeleteObject(penLight);
		DeleteObject(penShadow);
	}
	//画线段上的箭头
	static void DrawArrows(HDC hDC, POINT& pStart, POINT& pEnd, int Len = 8, int Style = 0)
	{
		// N 和 M 类似于实现定点数
		const int N = 386;
		const int M = N * N;
		const int DST = Len * 3 / 4;	// D 点到 A 点的距离
		const int HGT = DST / 2;		// 两翼距轴的距离

		Assert(DST);

		POINT Points[4];
		Points[0] = pEnd;

		int dx = pStart.x - Points[0].x;
		int dy = pStart.y - Points[0].y;
		int nLength = dx * dx + dy * dy;
		if (nLength == 0)					// 始末是同一个点？！
			return;

		int nVal = (int)sqrt((double)M / nLength);
		int sx = dx * nVal;
		int sy = dy * nVal;

		Points[2].x = Points[0].x + sx * DST / N;
		Points[2].y = Points[0].y + sy * DST / N;

		POINT pK;						
		pK.x = Points[0].x + sx * Len / N;
		pK.y = Points[0].y + sy * Len / N;

		int wy = sx * HGT / N;
		int wx = sy * HGT / N;
		Points[1].x = pK.x - wx;
		Points[1].y = pK.y + wy;

		Points[3].x = pK.x + wx;
		Points[3].y = pK.y - wy;

		if (Style == 0)
		{
			MoveToEx(hDC, Points[0].x, Points[0].y, nullptr);
			LineTo(hDC, Points[1].x, Points[1].y);
			MoveToEx(hDC, Points[0].x, Points[0].y, nullptr);
			LineTo(hDC, Points[3].x, Points[3].y);
		}
		else
			Polygon(hDC, &Points[0], sizeof(Points) / sizeof(POINT));
	}
};

//class BinString
//{
//private:
//	char * m_pdata;
//	size_t count;
//public:
//	BinString() : m_pdata(nullptr), count(0){}
//	~BinString(){ clear(); }
//	const char * append(char * strbuf)
//	{
//
//	}
//	void clear()
//	{
//		delete m_pdata;
//		m_pdata = nullptr;
//		count = 0;
//	}
//};


//控件输入合法性检查
class InputCheck
{
private:
	string	m_sInfo;
	HWND 	m_hWnd;
public:
	InputCheck()
		: m_sInfo("")
		, m_hWnd(0)
	{}
	//三个参数依次为 检查条件，错误信息，当前控件
	bool DoCheck(const bool bCheck, const char * sErrorInfo, const HWND hWnd)
	{
		if (!bCheck)
			return bCheck;

		m_sInfo.append("\n");
		m_sInfo.append(sErrorInfo);
		if (m_hWnd == nullptr)
			m_hWnd = hWnd;

		return bCheck;
	}
	bool ShowError(const HWND hWnd, const char * sCaption, const UINT uType)
	{
		bool bError = !m_sInfo.empty();
		if (bError)
		{
			::MessageBoxA(hWnd, m_sInfo.c_str(), sCaption, uType);
			if (m_hWnd != 0 && ::IsWindowEnabled(m_hWnd) && ::IsWindowVisible(m_hWnd))
				::SetFocus(m_hWnd);
		}
		m_sInfo.clear();
		m_hWnd = 0;

		return bError;
	}
};

//显示模态窗口
class ModalAssist
{
private:
	static BOOL __stdcall do_disable_window(HWND wnd, LPARAM data)
	{
		_ASSERT(data);
		list<HWND> * wnds = (list<HWND> *)data;
		if (wnd != ::GetActiveWindow() && ::IsWindowVisible(wnd) && ::IsWindowEnabled(wnd))
		{
			::EnableWindow(wnd, FALSE);
			wnds->push_front(wnd);
		}
		return TRUE;
	}

	list<HWND> m_wnds;
	void disable_window()
	{
		if (m_wnds.size() > 0)
			return;

		::EnumThreadWindows(::GetCurrentThreadId(), &do_disable_window, (LPARAM)&m_wnds);
	}
	void enable_window()
	{
		for (list<HWND>::iterator it = m_wnds.begin(); it != m_wnds.end(); it++)
			if (::IsWindow(*it))
				::EnableWindow(*it, TRUE);
		m_wnds.clear();
	}
private:	
	typedef LRESULT (ModalAssist::*DLGWNDPROC)(HWND, UINT, WPARAM, LPARAM);
	ACCallback<WNDPROC, ModalAssist, DLGWNDPROC> m_wnd_proc;
	WNDPROC old_wnd_proc;
	LRESULT wnd_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
	{
		if (message == WM_INITDIALOG || message == WM_SHOWWINDOW && wparam == TRUE)
			disable_window();
		else if (message == WM_NCDESTROY || message == WM_CLOSE || message == WM_SHOWWINDOW && wparam == FALSE)
			enable_window();
		return CallWindowProc(old_wnd_proc, hwnd, message, wparam, lparam);
	}

public:
	ModalAssist(): old_wnd_proc(nullptr), m_wnd_proc(this, &ModalAssist::wnd_proc){}
	void init(HWND wnd)
	{
		old_wnd_proc = (WNDPROC)::SetWindowLong(wnd, GWL_WNDPROC, (LONG)(WNDPROC)m_wnd_proc);
		disable_window();
	}
};

class WindowFun
{
public:
	static void ProcessMessages()
	{
		MSG  msg;
		if (PeekMessage(&msg, 0, 0, 0, PM_NOREMOVE))
		{
			if (!PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
				return;
			if (msg.message == WM_QUIT)
				PostMessage(nullptr, -1, 0, 0);
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	//最顶层窗口是否弹出窗口
	static bool HasPopup()
	{
		HWND hwnd = ::GetActiveWindow();
		HWND NewHwnd = 0;
		while (hwnd)
		{
			NewHwnd = ::GetParent(hwnd);
			if (NewHwnd == ::GetParent(NewHwnd) == 0 || NewHwnd == 0)
				break;
			hwnd = NewHwnd;
		}
		DWORD WinStyle = GetWindowLong(hwnd, GWL_STYLE);

		return (WinStyle & WS_POPUP) == WS_POPUP;
	}	
	static HWND GetFirstParentDlg(HWND wnd, const char * parent_class)
	{
		char str[MAX_PATH];
		HWND hwnd = wnd;
		while (hwnd)
		{
			GetClassNameA(hwnd, str, sizeof(str));
			if (strcmp(parent_class, str)==0)
				return hwnd;
			hwnd = GetParent(hwnd);
		}
		return nullptr;
	}
	static HWND GetTopParent(HWND hwnd)
	{
		HWND Hwnd = hwnd;
		while (::GetParent(Hwnd))
			Hwnd = ::GetParent(Hwnd);
		return Hwnd;
	}
	//最顶层窗口是否模式窗口
	static bool HasModel()
	{
		HWND hwnd = ::GetActiveWindow();
		HWND NewHwnd = 0;
		while (hwnd)
		{
			NewHwnd = ::GetParent(hwnd);
			if (NewHwnd == ::GetParent(NewHwnd) == 0 || NewHwnd == 0)
				break;
			hwnd = NewHwnd;
		}
		DWORD WinStyle = GetWindowLong(hwnd, GWL_STYLE);

		return ((WinStyle & DS_MODALFRAME) == DS_MODALFRAME) || ((WinStyle & DS_SYSMODAL) == DS_SYSMODAL);
	}	
	//在pParentWnd窗口内查询第一个Wnd_Class类的实例，并用pDestWnd返回
	static HWND FindWnd(HWND parent, const char * class_name)
	{
		char name[MAX_PATH];
		HWND wnd = GetTopWindow(parent);
		while (wnd)
		{
			GetClassNameA(wnd, name, sizeof(name));
			if (strcmp(class_name, name)==0)
				return wnd;
			wnd = GetNextWindow(wnd, GW_HWNDNEXT);
		}
		return 0;
	};
	//获得指定进程下，子窗口中包含child_class的顶层窗口句柄，常用来获得进程的主窗口
	static HWND GetWndByPID(DWORD process_id, const char * child_class)
	{
		DWORD _id;
		HINSTANCE res = GetModuleHandle(nullptr);

		HWND _h = GetTopWindow(0 );
		while (_h)
		{
			if (::FindWindowExA(_h, 0, child_class, ""))
			{
				::GetWindowThreadProcessId(_h, &_id);
				if (_id == process_id)
					return _h;
			}
        
			_h = GetNextWindow(_h, GW_HWNDNEXT);
		} 

		return nullptr;
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////
	//获得指定窗口在其父窗口中的位置信息，判断是否顶层窗口，或其所有者是否null
	static void GetWindowRectEx(HWND wnd, TRect & rect)
	{
		HWND parent;
		GetWindowRect(wnd, &rect);
		parent = GetParent(wnd);

		if (parent)
			::MapWindowPoints(nullptr, parent, (LPPOINT)&rect, 2);
	}
	static void MoveWindow(const HWND wnd, const TRect & rect, const bool ref = true)
	{
		::MoveWindow(wnd, rect.left, rect.top, rect.Width(), rect.Height(), ref ? TRUE : FALSE);
	}

	//在父窗口内平移
	static void OffsetWindow(HWND wnd, TSize Size)
	{
		TRect rect;
		GetWindowRectEx(wnd, rect);
		rect.OffsetRect(Size.cx, Size.cy);
		MoveWindow(wnd, rect);
	}

	//左
	static int GetWindowLeft(HWND wnd)
	{
		TRect rect;
		GetWindowRectEx(wnd, rect);
		return rect.left;
	}
	static void SetWindowLeft(HWND wnd, int value)
	{
		TRect rect;
		GetWindowRectEx(wnd, rect);
		rect.OffsetRect(value - rect.left, 0);
		MoveWindow(wnd, rect);
	}
	//上
	static int GetWindowTop(HWND wnd)
	{
		TRect rect;
		GetWindowRectEx(wnd, rect);
		return rect.top;
	}
	static void SetWindowTop(HWND wnd, int value)
	{
		TRect rect;
		GetWindowRectEx(wnd, rect);
		rect.OffsetRect(0, value - rect.top);
		MoveWindow(wnd, rect);
	}
	//右
	static int GetWindowRight(HWND wnd)
	{
		TRect rect;
		GetWindowRectEx(wnd, rect);
		return rect.right;
	}
	static void SetWindowRight(HWND wnd, int value)
	{
		TRect rect;
		GetWindowRectEx(wnd, rect);
		rect.OffsetRect(value - rect.right, 0);
		MoveWindow(wnd, rect);
	}
	//下
	static int GetWindowBottom(HWND wnd)
	{
		TRect rect;
		GetWindowRectEx(wnd, rect);
		return rect.bottom;
	}
	static void SetWindowBottom(HWND wnd, int value)
	{
		TRect rect;
		GetWindowRectEx(wnd, rect);
		rect.OffsetRect(0, value - rect.bottom);
		MoveWindow(wnd, rect);
	}
	//宽
	static int GetClientWidth(HWND wnd)
	{
		TRect rect;
		GetClientRect(wnd, rect);
		return rect.Width();
	}
	static int GetWindowWidth(HWND wnd)
	{
		TRect rect;
		GetWindowRect(wnd, rect);
		return rect.Width();
	}
	static void SetWindowWidth(HWND wnd, int value)
	{
		TRect rect;
		GetWindowRectEx(wnd, rect);
		rect.right=rect.left + value;
		MoveWindow(wnd, rect);
	}
	//高
	static int GetClientHeight(HWND wnd)
	{
		TRect rect;
		GetClientRect(wnd, rect);
		return rect.Height();
	}
	static int GetWindowHeight(HWND wnd)
	{
		TRect rect;
		GetWindowRect(wnd, rect);
		return rect.Height();
	}
	static void SetWindowHeight(HWND wnd, int value)
	{
		TRect rect;
		GetWindowRectEx(wnd, rect);
		rect.bottom=rect.top + value;
		MoveWindow(wnd, rect);
	}
	//左上	
	static TPoint GetWindowTopLeft(HWND wnd)
	{
		TRect rect;
		GetWindowRectEx(wnd, rect);
		return rect.TopLeft();
	}
	static void SetWindowLeftTop(HWND wnd, POINT & pt)
	{
		TRect rect;
		GetWindowRectEx(wnd, rect);
		rect.OffsetRect(pt.x - rect.left, pt.y - rect.top);
		MoveWindow(wnd, rect);
	}
	//右下
	static TPoint GetWindowBottomRight(HWND wnd)
	{
		TRect rect;
		GetWindowRectEx(wnd, rect);
		return rect.BottomRight();
	}
	static void SetWindowBottomRight(HWND wnd, TPoint Pt)
	{
		TRect rect;
		GetWindowRectEx(wnd, rect);
		rect.OffsetRect(Pt.x-rect.right, Pt.y-rect.bottom);
		MoveWindow(wnd, rect);
	}
	//显示与置灰
	static void SetVisibleAndEnabeled(HWND wnd, bool visible, bool enabel)
	{
		EnableWindow(wnd, enabel);
		SetWindowVisible(wnd, visible);
	}
	static void SetVisibleAndEnabeled(HWND wnd, bool value)
	{
		SetVisibleAndEnabeled(wnd, value, value);
	}
	static void SetWindowVisible(HWND wnd, bool visible)
	{
		if (visible)
			ShowWindow(wnd, SW_SHOW);
		else
			ShowWindow(wnd, SW_HIDE);
	}

	//扩大窗口
	static void InflateWindow(HWND wnd, TSize size)
	{
		TRect rect;
		GetWindowRectEx(wnd, rect);
		rect.InflateRect(size);
		MoveWindow(wnd, rect);
	}
	static void InflateWindow(HWND wnd, TRect rect)
	{
		TRect mrect;
		GetWindowRectEx(wnd, mrect);
		mrect.InflateRect(rect);
		MoveWindow(wnd, rect);
	}

	//缩小窗口
	static void DeflateWindow(HWND wnd, TSize size)
	{
		TRect rect;
		GetWindowRectEx(wnd, rect);
		rect.DeflateRect(size);
		MoveWindow(wnd, rect);
	}
	static void DeflateWindow(HWND wnd, TRect rect)
	{
		TRect mrect;
		GetWindowRectEx(wnd, mrect);
		mrect.DeflateRect(rect);
		MoveWindow(wnd, rect);
	}
	//黄金位置
	static void GetWorkAreaRect(TRect * pRect)
	{
		SystemParametersInfo(SPI_GETWORKAREA, 0, pRect, 0);
	}
	//设置窗口wnd中线到dest窗口竖向黄金分割点，dest默认为屏幕窗口
	static void MoveToGolden(HWND wnd, HWND dest = 0)
	{
		TRect rect(0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
		if (dest)
			GetWindowRect(dest, rect);

		int left =  rect.left + (int)((rect.Width()  - GetWindowWidth(wnd))/2);
		int top = max(rect.top  + (int)((rect.Height()*(1-0.618) - GetWindowHeight(wnd)/2)), 0);
		
		SetWindowLeftTop(wnd, TPoint(left, top));
	}
	static void MoveToMainWindowGolden(HWND wnd)
	{	
		HWND hwnd = GetTopParent(wnd);

		TRect main, self;
		GetWindowRect(hwnd, &main);
		GetWindowRect(wnd, &self);

		LONG left = main.left + (main.Width() - self.Width()) / 2;
		LONG top = max(main.top  + (int(main.Height() * (1 - 0.618)) - self.Height() / 2), 0);

		SetWindowLeftTop(wnd, TPoint(left, top));
	}

	static void SetClientRect(HWND wnd, TRect & rect)
	{
		TRect winRect, cliRect;		
		GetWindowRect(wnd, winRect);
		GetClientRect(wnd, cliRect);
		winRect.right  += (rect.Width()  - cliRect.Width());
		winRect.bottom += (rect.Height() - cliRect.Height());
		MoveWindow(wnd, winRect);
	}
	//根据子窗口自适应大小，Margins表示四边的边距,bOnlyVisibleChild表示只根据可视的子窗口自适应大小
	static void DoAuotSize(HWND wnd, MARGINS & Margins, bool bOnlyVisibleChild = true)
	{
		TRect cliRect, mRect;		

		int dxLeft = 0;
		int dxTop  = 0;
		bool bFirst = false;
		HWND client = GetTopWindow(wnd);
		while (client)
		{
			if (IsWindowVisible(client) || !bOnlyVisibleChild)
			{
				GetWindowRect(client, mRect);
				if (!bFirst)//第一个子窗口的偏移量决定了其他子窗口的偏移量
				{					
					bFirst = true;
					dxLeft = Margins.cxLeftWidth-mRect.left;
					dxTop  = Margins.cyTopHeight-mRect.top;
				}

				cliRect.UnionRect(cliRect, mRect);

				mRect.OffsetRect(dxLeft, dxTop);
				MoveWindow(client, mRect);
			}
			client = GetNextWindow(client, GW_HWNDNEXT);
		}

		cliRect.right	+= (Margins.cxLeftWidth + Margins.cxRightWidth);
		cliRect.bottom	+= (Margins.cyTopHeight + Margins.cyBottomHeight);

		SetClientRect(wnd, cliRect);
	}

	static DWORD GetStyle(HWND wnd) 
	{
		return (DWORD)GetWindowLong(wnd, GWL_STYLE);
	}
	static DWORD GetExStyle(HWND wnd) 
	{
		return (DWORD)GetWindowLong(wnd, GWL_EXSTYLE);
	}
	static bool ModifyStyle(HWND wnd, DWORD dwRemove, DWORD dwAdd, UINT nFlags)
	{
		_ASSERT(::IsWindow(wnd));
		return ModifyStyle(wnd, dwRemove, dwAdd, nFlags);
	}
	static bool ModifyStyleEx(HWND wnd, DWORD dwRemove, DWORD dwAdd, UINT nFlags)
	{
		return ModifyStyleEx(wnd, dwRemove, dwAdd, nFlags);
	}
	static HFONT GetFont(HWND wnd)
	{
		return (HFONT)::SendMessage(wnd, WM_GETFONT, 0, 0);
	}
	static void SetFont(HWND wnd, HFONT font)
	{
		::SendMessage(wnd, WM_SETFONT, (WPARAM)font, 1);
	}
	static void GetWindowText(HWND wnd, string & text)
	{
		int len = ::GetWindowTextLength(wnd);
		if (text.max_size() < (size_t)len)
			text.resize(len);
		::GetWindowTextA(wnd, (char *)text.c_str(), len);
	}
	static void ModifyControlStyle(HWND & wnd, DWORD dwRemove, DWORD dwAdd, DWORD dwRemoveEx, DWORD dwAddEx)
	{
		char class_name[MAX_PATH];
		GetClassNameA(wnd, class_name, sizeof(class_name));
		HWND parent = GetParent(wnd);
		HFONT font = GetFont(wnd);
		string text;
		GetWindowText(wnd, text);
		{
			// 在此添加保存控件其他属性的代码,我只保存了字体和文字
		}
		TRect rc_wnd;
		GetWindowRectEx(wnd, rc_wnd);
		DWORD dwNewStyle = (GetStyle(wnd) & ~dwRemove) | dwAdd;
		DWORD dwNewStyleEx = (GetExStyle(wnd) & ~dwRemoveEx) | dwAddEx;
		ShowWindow(wnd, SW_HIDE);
		DestroyWindow(wnd);
		wnd = ::CreateWindowExA(dwNewStyleEx, class_name, text.c_str(), dwNewStyle, rc_wnd.left, rc_wnd.top, rc_wnd.Width(), rc_wnd.Height(), parent, 0, FileFun::GetSelfHandle(), 0);
		SetFont(wnd, font);		
	}
};

class MenuFun
{
public:
	static void TrimSeparator(HMENU hmenu)				//整理分割线,删除首尾和重复的分割线
	{
		if (!hmenu) return;
		bool bOldSeparator = true;
		for(int i = GetMenuItemCount(hmenu) - 1; i>=0; i--)
		{
			UINT uState = ::GetMenuState(hmenu, i, MF_BYPOSITION);

			bool bCurrSeparator = (uState & MF_SEPARATOR)==MF_SEPARATOR;
			if (bCurrSeparator && (bOldSeparator || i==0))
				::RemoveMenu(hmenu, i, MF_BYPOSITION);

			bOldSeparator = bCurrSeparator;
		}
	}
	static void VisableMenuItem(HMENU hmenu, UINT id, bool visable)
	{
		if (visable)
			AddMenuItem(hmenu, id);
		else
			::RemoveMenu(hmenu, id, MF_BYCOMMAND);
	}
	static void AddSeparator(HMENU hmenu)//增加菜单项
	{
		if (!hmenu) return;
		::AppendMenu(hmenu, MF_SEPARATOR, 0, 0);
	}
	static bool AddMenuItem(HMENU hmenu, UINT id)//增加菜单项
	{
		if (!hmenu) return false;
		if (GetMenuState(hmenu, id, MF_BYCOMMAND)==0xFFFFFFFF)			
		{
			char str[MAX_PATH];
			::GetMenuStringA(hmenu, id, str, MF_BYCOMMAND, sizeof(str));
			::AppendMenuA(hmenu, MF_STRING, id, str);
			return true;
		}
		return false;
	}
	static bool AddMenuItemByPos(HMENU hmenu, UINT Position)//增加菜单项
	{
		if (!hmenu) return false;
		if (GetMenuState(hmenu, Position, MF_BYPOSITION)==0xFFFFFFFF)			
		{
			char str[MAX_PATH];
			int id = ::GetMenuStringA(hmenu, Position, str, MF_BYPOSITION, sizeof(str));
			::AppendMenuA(hmenu, MF_STRING, id, str);
			return true;
		}
		return false;
	}
	static void InsertSeparator(HMENU hmenu, UINT pre_id)//增加菜单项
	{
		if (!hmenu) return;
		::InsertMenu(hmenu, pre_id, MF_SEPARATOR | MF_BYCOMMAND, 0, 0);
	}
	static bool InsertMenuEx(HMENU hmenu, UINT id, UINT pre_id)//增加菜单项
	{
		if (!hmenu) return false;
		if (GetMenuState(hmenu, id, MF_BYCOMMAND)==0xFFFFFFFF)			
		{
			char str[MAX_PATH];
			::GetMenuStringA(hmenu, id, str, MF_BYCOMMAND, sizeof(str));
			::InsertMenuA(hmenu, pre_id, MF_BYCOMMAND | MF_STRING, id, str);
			return true;
		}
		return false;
	}
	static bool InsertMenuExByPos(HMENU hmenu, UINT position, UINT pre_position)//增加菜单项
	{
		if (!hmenu) return false;
		if (GetMenuState(hmenu, position, MF_BYPOSITION)==0xFFFFFFFF)			
		{
			char str[MAX_PATH];
			::GetMenuStringA(hmenu, position, str, MF_BYCOMMAND, sizeof(str));
			::InsertMenuA(hmenu, pre_position, MF_BYPOSITION | MF_STRING, position, str);
			return true;
		}
		return false;
	}
	static bool IsVisableMenuItem(HMENU hmenu, UINT id)
	{
		if (!hmenu) return false;
		return ::GetMenuState(hmenu, id, MF_BYCOMMAND)!=0xFFFFFFFF;
	}
	static bool IsVisableMenuItemByPos(HMENU hmenu, UINT position)
	{
		if (!hmenu) return false;
		return ::GetMenuState(hmenu, position, MF_BYPOSITION)!=0xFFFFFFFF;
	}	
	static void EnableMenuItem(HMENU hmenu, UINT id, bool enable)
	{
		if (!hmenu) return;
		::EnableMenuItem(hmenu, id, MF_BYCOMMAND | (enable ? MF_ENABLED : MF_GRAYED));
	}
	static int GetMenuPos(const HMENU hmenu, const char * name)
	{
		int cnt = GetMenuItemCount(hmenu);
		char str[MAX_PATH];
		for(int i = 0; i < cnt; i++)
		{
			GetMenuStringA(hmenu, i, str, sizeof(str), MF_BYPOSITION);
			if (strcmp(str, name) == 0)
				return i;
		}
		return -1;
	}
	static HMENU GetSubMenu(const HMENU hmenu, const char * name)
	{
		int cnt = GetMenuItemCount(hmenu);
		char str[MAX_PATH];
		for(int i = 0; i < cnt; i++)
		{
			GetMenuStringA(hmenu, i, str, sizeof(str), MF_BYPOSITION);
			if (strcmp(str, name) == 0)
				return ::GetSubMenu(hmenu, i);
		}
		return 0;
	}
};