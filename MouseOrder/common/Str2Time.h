#pragma once

#include <ctype.h>
#include <string.h>
#include <time.h>

//We do not implement alternate representations. However, we always
//check whether a given modifier is allowed for a certain conversion.
#define ALT_E          0x01
#define ALT_O          0x02
//#define LEGAL_ALT(x)       { if (alt_format & ~(x)) return (0); }
#define LEGAL_ALT(x)   { ; }
#define TM_YEAR_BASE   (1900)


static const char *day[7] = 
{
	"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday",	"Friday", "Saturday"
};
static const char *abday[7] = 
{
	"Sun","Mon","Tue","Wed","Thu","Fri","Sat"
};
static const char *mon[12] = 
{
	"January", "February", "March", "April", "May", "June", "July",
	"August", "September", "October", "November", "December"
};
static const char *abmon[12] = 
{
	"Jan", "Feb", "Mar", "Apr", "May", "Jun",
	"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};
static const char *am_pm[2] = 
{
	"AM", "PM"
};

static int conv_num(const char **buf, int *dest, int llim, int ulim)
{
	int result = 0;

	//根据范围确定有效数字
	int rulim = ulim;

	if (**buf < '0' || **buf > '9')
		return (0);

	do 
	{
		result *= 10;
		result += *(*buf)++ - '0';
		rulim /= 10;
	} while ((result * 10 <= ulim) && rulim && **buf >= '0' && **buf <= '9');

	if (result < llim || result > ulim)
		return (0);

	*dest = result;
	return (1);
}
//
static void CharToWChar(WCHAR * Dest, char * Souc)
{
	int nLen = strlen(Souc) + 1; 
	int nwLen = MultiByteToWideChar(CP_ACP, 0, Souc, nLen, NULL, 0); 

	MultiByteToWideChar(CP_ACP, 0, Souc, nLen, Dest, nwLen); 
}
static void WCharToChar(char * Dest, WCHAR * Souc)
{
	int nLen = wcslen((wchar_t *)Souc)+1;    
	WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)Souc, nLen, (LPSTR)Dest, 2*nLen, NULL, NULL);
}
//时区全称
static void GetStandardName(char & Name)
{
	_TIME_ZONE_INFORMATION tmInfo;
	::GetTimeZoneInformation((LPTIME_ZONE_INFORMATION)&tmInfo);

	WCharToChar(&Name, (WCHAR *)&(tmInfo.StandardName));
}
//时区简称
static void GetDaylightName(char & Name)
{
	_TIME_ZONE_INFORMATION tmInfo;
	::GetTimeZoneInformation((LPTIME_ZONE_INFORMATION)&tmInfo);

	WCharToChar(&Name, (WCHAR *)&(tmInfo.DaylightName));
}
//字符串前n位比较(不分大小写)
//static int StrCaseCmpN(char *s1, char *s2, size_t n)
//{
//	if (n==0)
//		return 0;
//
//	while (n-- && toupper((unsigned char)*s1) == toupper((unsigned char)*s2++))
//		if (n == 0 || *s1++ == '\0')
//			return 0;
//
//	return toupper((unsigned char)*s1) - toupper((unsigned char)*--s2);
//}
////字符串比较(不分大小写)
//static int StrCaseCmp(char *s1, char *s2)
//{
//	while (toupper((unsigned char)*s1) == toupper((unsigned char)*s2++))
//		if (*s1++ == '\0')
//			return 0;
//
//	return toupper((unsigned char)*s1) - toupper((unsigned char)*--s2);
//}

static char * strptime(const char *buf, const char *fmt, struct tm *tm)
{
	char c;
	const char *bp;
	size_t len = 0;
	int alt_format, i, split_year = 0;
	char Name[255];

	bp = buf;

	while ((c = *fmt) != '\0') 
	{
		//Clear `alternate' modifier prior to new conversion.
		alt_format = 0;

		//跳过空格字符
		if (isspace(c)) 
		{
			while (isspace(*bp))
				bp++;

			fmt++;
			continue;
		}

		if ((c = *fmt++) != '%')
			goto percent_sign;


again:        
		switch (c = *fmt++) 
		{
			case '%':	//"%%" %的转义字符.
percent_sign:
				if (c != *bp++)
					return (0);
				break;


		//"Alternative" modifiers. 只是跳到标记again处,重新开始。
			case 'E':	//"%E?" alternative conversion modifier.
				LEGAL_ALT(0);
				alt_format |= ALT_E;
				goto again;

			case 'O':	//"%O?" alternative conversion modifier.
				LEGAL_ALT(0);
				alt_format |= ALT_O;
				goto again;


		//将复杂转换规则替换为基础转换规则, 通过递归进行重新转换
			case 'c':	//默认的首选的日期和时间表示法 date time，相当与"%x %X"
				LEGAL_ALT(ALT_E);
				if (!(bp = strptime(bp, "%x %X", tm)))
					return (0);
				break;

			case 'D':	//日期格式，相当与“%m/%d/%y”
				LEGAL_ALT(0);
				if (!(bp = strptime(bp, "%m/%d/%y", tm)))
					return (0);
				break;

			case 'F':	//日期格式，相当与“%Y-%m-%d”
				LEGAL_ALT(0);
				if (!(bp = strptime(bp, "%Y-%m-%d", tm)))
					return (0);
				break;

			case 'R':	//用24小时制表示的时间，相当与"%H:%M".
				LEGAL_ALT(0);
				if (!(bp = strptime(bp, "%H:%M", tm)))
					return (0);
				break;

			case 'r':	//用24小时制表示的时间 带am或pm，相当与"%I:%M:%S %p"
				LEGAL_ALT(0);
				if (!(bp = strptime(bp, "%I:%M:%S %p", tm)))
					return (0);
				break;

			case 'T':	//当前时间格式，相当于“%H:%M:%S”
				LEGAL_ALT(0);
				if (!(bp = strptime(bp, "%H:%M:%S", tm)))
					return (0);
				break;

			case 'X':	//选取除去日期[date]的时间[time]，相当与"%H:%M:%S"
				LEGAL_ALT(ALT_E);
				if (!(bp = strptime(bp, "%H:%M:%S", tm)))
					return (0);
				break;

			case 'x':	//选取除去时间[time]的日期[date]，相当于"%m/%d/%y"
				LEGAL_ALT(ALT_E);
				if (!(bp = strptime(bp, "%m/%d/%y", tm)))
					return (0);
				break;


		//基础的转换规则
			case 'A':	//星期几的全称
			case 'a':	//星期几的简称
				LEGAL_ALT(0);
				for (i = 0; i < 7; i++) 
				{
					//全称
					len = strlen(day[i]);
					if (_strnicmp((char *)(day[i]), (char *)bp, len) == 0)
						break;

					//简称
					len = strlen(abday[i]);
					if (_strnicmp((char *)(abday[i]), (char *)bp, len) == 0)
						break;
				}

				//不匹配
				if (i == 7)
					return (0);

				tm->tm_wday = i;
				bp += len;
				break;

			case 'B':	//月份的全称
			case 'b':	//月份的简称
			case 'h':	//与%b相同
				LEGAL_ALT(0);
				for (i = 0; i < 12; i++) 
				{
					//全称
					len = strlen(mon[i]);
					if (_strnicmp((char *)(mon[i]), (char *)bp, len) == 0)
						break;

					//简称
					len = strlen(abmon[i]);
					if (_strnicmp((char *)(abmon[i]),(char *) bp, len) == 0)
						break;
				}

				//不匹配
				if (i == 12)
					return (0);

				tm->tm_mon = i;
				bp += len;
				break;

			case 'C':	//年份前两位（年份除以100取整，范围从00到99）
				LEGAL_ALT(ALT_E);
				if (!(conv_num(&bp, &i, 0, 99)))
					return (0);

				if (split_year) 
					tm->tm_year = (tm->tm_year % 100) + (i * 100);
				else 
				{
					tm->tm_year = i * 100;
					split_year = 1;
				}
				break;

			case 'd':	//一个月包含的天数（从01到31）
			case 'e':	//一个月包含的天数，数字前不包括0（从1到31）
				LEGAL_ALT(ALT_O);
				if (!(conv_num(&bp, &tm->tm_mday, 1, 31)))
					return (0);
				break;

			case 'k':	//小时，使用24小时时钟（00到23）
				LEGAL_ALT(0);
			//FALLTHROUGH
			case 'H':	//小时，使用24小时时钟（00到23）
				LEGAL_ALT(ALT_O);
				if (!(conv_num(&bp, &tm->tm_hour, 0, 23)))
					return (0);
				break;

			case 'l':	//小时，使用12小时时钟（01到12）
				LEGAL_ALT(0);
			//FALLTHROUGH
			case 'I':	//小时，使用12小时时钟（01到12）
				LEGAL_ALT(ALT_O);
				if (!(conv_num(&bp, &tm->tm_hour, 1, 12)))
					return (0);
				if (tm->tm_hour == 12)
					tm->tm_hour = 0;
				break;

			case 'j':	//一年的天数（001到366）
				LEGAL_ALT(0);
				if (!(conv_num(&bp, &i, 1, 366)))
					return (0);
				tm->tm_yday = i - 1;
				break;

			case 'M':	//分钟
				LEGAL_ALT(ALT_O);
				if (!(conv_num(&bp, &tm->tm_min, 0, 59)))
					return (0);
				break;

			case 'm':	//月份（01到12）
				LEGAL_ALT(ALT_O);
				if (!(conv_num(&bp, &i, 1, 12)))
					return (0);
				tm->tm_mon = i - 1;
				break;

	       case 'p':	//与给定的时间值相对应的AM或PM
	            LEGAL_ALT(0);
	            //AM?
	            if (_strnicmp((char *)(am_pm[0]), (char *)bp, strlen(am_pm[0])) == 0) 
	            {
	                 if (tm->tm_hour > 11)
	                     return (0);
	
	                 bp += strlen(am_pm[0]);
	                 break;
	            }
	            //PM?
	            else if (_strnicmp((char *)(am_pm[1]), (char *)bp, strlen(am_pm[1])) == 0) 
	            {
	                 if (tm->tm_hour > 11)
	                     return (0);
	
	                 tm->tm_hour += 12;
	                 bp += strlen(am_pm[1]);
	                 break;
	            }
	
	            //不匹配
	            return (0);

			case 'S':	//秒
				LEGAL_ALT(ALT_O);
				if (!(conv_num(&bp, &tm->tm_sec, 0, 61)))
					return (0);
				break;

			case 'u':	//以数字形式表示星期几（1到7），Monday=1。提醒：在SUN Sloaris系统中，Sunday=1
				LEGAL_ALT(ALT_O);
				if (!(conv_num(&bp, &tm->tm_wday, 0, 6)))
					return (0);

				if (tm->tm_wday==0)
					tm->tm_wday=7;
				break;

			//csae 'V':	//在当今年份中所包含的ISO 8601格式下的周的总数（01到53），week 1表示第一周，以周一作为每周的第一天（基于周的年）

			case 'U':	//当今年份中包含的周的总数，以第一个星期日作为第一周的第一天
			case 'W':	//当前年份中包含的周的总数，以第一个星期一作为第一周的第一天
				LEGAL_ALT(ALT_O);
				//XXX This is bogus, as we can not assume any valid information present in the tm structure at this
				//point to calculate a real value, so just check the range for now.无法得到有效数字，没有使用返回值
				if (!(conv_num(&bp, &i, 0, 53)))
					return (0);
				break;
			
			case 'w':	//以数字的形式表示星期几，Sunday[星期日]=0
				LEGAL_ALT(ALT_O);
				if (!(conv_num(&bp, &tm->tm_wday, 0, 6)))
					return (0);
				break;

			//case 'G':	//四位数字表示的年份（基于周的年）
			case 'Y':	//四位数字表示的年份，如：1999，2001等
				LEGAL_ALT(ALT_E);
				if (!(conv_num(&bp, &i, 0, 9999)))
					return (0);

				tm->tm_year = i - TM_YEAR_BASE;
				break;

			//case 'g':	//年份后两位（基于周的年）
			case 'y':	//年份后两位
				LEGAL_ALT(ALT_E | ALT_O);
				if (!(conv_num(&bp, &i, 0, 99)))
					return (0);

				if (split_year) 
				{
					tm->tm_year = ((tm->tm_year / 100) * 100) + i;
					break;
				}
				split_year = 1;
				if (i <= 68)
					tm->tm_year = i + 2000 - TM_YEAR_BASE;
				else
					tm->tm_year = i + 1900 - TM_YEAR_BASE;
				break;

			case 'z':	//时区名称的简称
				GetDaylightName(Name[0]);
				if (_strnicmp(Name, (char *)bp, strlen(Name)))
				{
					bp += strlen(Name);
					break;
				}
				return (0);
			case 'Z':	//时区名称的全称
				GetStandardName(Name[0]);
				if (_strnicmp(Name, (char *)bp, strlen(Name)))
				{
					bp += strlen(Name);
					break;
				}
				return (0);

		//其他转换
			case 'n':	//换行符
			case 't':	//tab键制表符
				LEGAL_ALT(0);
				while (isspace(*bp))
					bp++;
				break;

			default:	//不支持的/未知的
				return (0);
		}//case
	}//switch

	//LINTED functional specification
	return ((char *)bp);
}

//测试
/*
#include <conio.h>
void main()
{
	struct tm newtime;
	memset(&newtime,0x00,sizeof(struct tm));

	strptime("Sat 2009-10-31 05:35:53","%a %Y-%m-%d %H:%M:%S",&newtime);

	getch();
}
*/
/*
tm结构中个成员定义：
tm_sec		当前分钟内的秒数（0-61）
tm_min		当前小时内的分钟数（0-59）
tm_hour		午夜起的小时数（0-23）
tm_mday		月份中的第几天（1-31）
tm_mon		自一月起过了几个月（0-11）
tm_year		年份-1900
tm_wday		自星期天起过了几天（0-6）
tm_yday		本年自一月一日起过了多少天（0-365）
unparsed	date 中未能通过指定的 format 识别的部分
*/