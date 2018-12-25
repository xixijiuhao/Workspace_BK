#include "stdafx.h"
#include "log_fun.h"
#include "windows.h"
#include "stdio.h"
#include "global.h"
#include "Logging_API.h"

//extern
ILoggingApi * g_log;
unsigned int	g_log_fid;


bool is_out(bool right, const char * fmt, ...)
{
	if (right && g_log)
	{
		va_list ap;
		va_start(ap, fmt);
		string str;
		StrFun::format(str, fmt, ap);
		va_end(ap);

		g_log->OutputLog(g_log_fid, ltString, llError, str.c_str(), str.length());
	}

	return right;
}