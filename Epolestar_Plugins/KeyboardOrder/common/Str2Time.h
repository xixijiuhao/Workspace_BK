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

	//���ݷ�Χȷ����Ч����
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
//ʱ��ȫ��
static void GetStandardName(char & Name)
{
	_TIME_ZONE_INFORMATION tmInfo;
	::GetTimeZoneInformation((LPTIME_ZONE_INFORMATION)&tmInfo);

	WCharToChar(&Name, (WCHAR *)&(tmInfo.StandardName));
}
//ʱ�����
static void GetDaylightName(char & Name)
{
	_TIME_ZONE_INFORMATION tmInfo;
	::GetTimeZoneInformation((LPTIME_ZONE_INFORMATION)&tmInfo);

	WCharToChar(&Name, (WCHAR *)&(tmInfo.DaylightName));
}
//�ַ���ǰnλ�Ƚ�(���ִ�Сд)
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
////�ַ����Ƚ�(���ִ�Сд)
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

		//�����ո��ַ�
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
			case '%':	//"%%" %��ת���ַ�.
percent_sign:
				if (c != *bp++)
					return (0);
				break;


		//"Alternative" modifiers. ֻ���������again��,���¿�ʼ��
			case 'E':	//"%E?" alternative conversion modifier.
				LEGAL_ALT(0);
				alt_format |= ALT_E;
				goto again;

			case 'O':	//"%O?" alternative conversion modifier.
				LEGAL_ALT(0);
				alt_format |= ALT_O;
				goto again;


		//������ת�������滻Ϊ����ת������, ͨ���ݹ��������ת��
			case 'c':	//Ĭ�ϵ���ѡ�����ں�ʱ���ʾ�� date time���൱��"%x %X"
				LEGAL_ALT(ALT_E);
				if (!(bp = strptime(bp, "%x %X", tm)))
					return (0);
				break;

			case 'D':	//���ڸ�ʽ���൱�롰%m/%d/%y��
				LEGAL_ALT(0);
				if (!(bp = strptime(bp, "%m/%d/%y", tm)))
					return (0);
				break;

			case 'F':	//���ڸ�ʽ���൱�롰%Y-%m-%d��
				LEGAL_ALT(0);
				if (!(bp = strptime(bp, "%Y-%m-%d", tm)))
					return (0);
				break;

			case 'R':	//��24Сʱ�Ʊ�ʾ��ʱ�䣬�൱��"%H:%M".
				LEGAL_ALT(0);
				if (!(bp = strptime(bp, "%H:%M", tm)))
					return (0);
				break;

			case 'r':	//��24Сʱ�Ʊ�ʾ��ʱ�� ��am��pm���൱��"%I:%M:%S %p"
				LEGAL_ALT(0);
				if (!(bp = strptime(bp, "%I:%M:%S %p", tm)))
					return (0);
				break;

			case 'T':	//��ǰʱ���ʽ���൱�ڡ�%H:%M:%S��
				LEGAL_ALT(0);
				if (!(bp = strptime(bp, "%H:%M:%S", tm)))
					return (0);
				break;

			case 'X':	//ѡȡ��ȥ����[date]��ʱ��[time]���൱��"%H:%M:%S"
				LEGAL_ALT(ALT_E);
				if (!(bp = strptime(bp, "%H:%M:%S", tm)))
					return (0);
				break;

			case 'x':	//ѡȡ��ȥʱ��[time]������[date]���൱��"%m/%d/%y"
				LEGAL_ALT(ALT_E);
				if (!(bp = strptime(bp, "%m/%d/%y", tm)))
					return (0);
				break;


		//������ת������
			case 'A':	//���ڼ���ȫ��
			case 'a':	//���ڼ��ļ��
				LEGAL_ALT(0);
				for (i = 0; i < 7; i++) 
				{
					//ȫ��
					len = strlen(day[i]);
					if (_strnicmp((char *)(day[i]), (char *)bp, len) == 0)
						break;

					//���
					len = strlen(abday[i]);
					if (_strnicmp((char *)(abday[i]), (char *)bp, len) == 0)
						break;
				}

				//��ƥ��
				if (i == 7)
					return (0);

				tm->tm_wday = i;
				bp += len;
				break;

			case 'B':	//�·ݵ�ȫ��
			case 'b':	//�·ݵļ��
			case 'h':	//��%b��ͬ
				LEGAL_ALT(0);
				for (i = 0; i < 12; i++) 
				{
					//ȫ��
					len = strlen(mon[i]);
					if (_strnicmp((char *)(mon[i]), (char *)bp, len) == 0)
						break;

					//���
					len = strlen(abmon[i]);
					if (_strnicmp((char *)(abmon[i]),(char *) bp, len) == 0)
						break;
				}

				//��ƥ��
				if (i == 12)
					return (0);

				tm->tm_mon = i;
				bp += len;
				break;

			case 'C':	//���ǰ��λ����ݳ���100ȡ������Χ��00��99��
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

			case 'd':	//һ���°�������������01��31��
			case 'e':	//һ���°���������������ǰ������0����1��31��
				LEGAL_ALT(ALT_O);
				if (!(conv_num(&bp, &tm->tm_mday, 1, 31)))
					return (0);
				break;

			case 'k':	//Сʱ��ʹ��24Сʱʱ�ӣ�00��23��
				LEGAL_ALT(0);
			//FALLTHROUGH
			case 'H':	//Сʱ��ʹ��24Сʱʱ�ӣ�00��23��
				LEGAL_ALT(ALT_O);
				if (!(conv_num(&bp, &tm->tm_hour, 0, 23)))
					return (0);
				break;

			case 'l':	//Сʱ��ʹ��12Сʱʱ�ӣ�01��12��
				LEGAL_ALT(0);
			//FALLTHROUGH
			case 'I':	//Сʱ��ʹ��12Сʱʱ�ӣ�01��12��
				LEGAL_ALT(ALT_O);
				if (!(conv_num(&bp, &tm->tm_hour, 1, 12)))
					return (0);
				if (tm->tm_hour == 12)
					tm->tm_hour = 0;
				break;

			case 'j':	//һ���������001��366��
				LEGAL_ALT(0);
				if (!(conv_num(&bp, &i, 1, 366)))
					return (0);
				tm->tm_yday = i - 1;
				break;

			case 'M':	//����
				LEGAL_ALT(ALT_O);
				if (!(conv_num(&bp, &tm->tm_min, 0, 59)))
					return (0);
				break;

			case 'm':	//�·ݣ�01��12��
				LEGAL_ALT(ALT_O);
				if (!(conv_num(&bp, &i, 1, 12)))
					return (0);
				tm->tm_mon = i - 1;
				break;

	       case 'p':	//�������ʱ��ֵ���Ӧ��AM��PM
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
	
	            //��ƥ��
	            return (0);

			case 'S':	//��
				LEGAL_ALT(ALT_O);
				if (!(conv_num(&bp, &tm->tm_sec, 0, 61)))
					return (0);
				break;

			case 'u':	//��������ʽ��ʾ���ڼ���1��7����Monday=1�����ѣ���SUN Sloarisϵͳ�У�Sunday=1
				LEGAL_ALT(ALT_O);
				if (!(conv_num(&bp, &tm->tm_wday, 0, 6)))
					return (0);

				if (tm->tm_wday==0)
					tm->tm_wday=7;
				break;

			//csae 'V':	//�ڵ����������������ISO 8601��ʽ�µ��ܵ�������01��53����week 1��ʾ��һ�ܣ�����һ��Ϊÿ�ܵĵ�һ�죨�����ܵ��꣩

			case 'U':	//��������а������ܵ��������Ե�һ����������Ϊ��һ�ܵĵ�һ��
			case 'W':	//��ǰ����а������ܵ��������Ե�һ������һ��Ϊ��һ�ܵĵ�һ��
				LEGAL_ALT(ALT_O);
				//XXX This is bogus, as we can not assume any valid information present in the tm structure at this
				//point to calculate a real value, so just check the range for now.�޷��õ���Ч���֣�û��ʹ�÷���ֵ
				if (!(conv_num(&bp, &i, 0, 53)))
					return (0);
				break;
			
			case 'w':	//�����ֵ���ʽ��ʾ���ڼ���Sunday[������]=0
				LEGAL_ALT(ALT_O);
				if (!(conv_num(&bp, &tm->tm_wday, 0, 6)))
					return (0);
				break;

			//case 'G':	//��λ���ֱ�ʾ����ݣ������ܵ��꣩
			case 'Y':	//��λ���ֱ�ʾ����ݣ��磺1999��2001��
				LEGAL_ALT(ALT_E);
				if (!(conv_num(&bp, &i, 0, 9999)))
					return (0);

				tm->tm_year = i - TM_YEAR_BASE;
				break;

			//case 'g':	//��ݺ���λ�������ܵ��꣩
			case 'y':	//��ݺ���λ
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

			case 'z':	//ʱ�����Ƶļ��
				GetDaylightName(Name[0]);
				if (_strnicmp(Name, (char *)bp, strlen(Name)))
				{
					bp += strlen(Name);
					break;
				}
				return (0);
			case 'Z':	//ʱ�����Ƶ�ȫ��
				GetStandardName(Name[0]);
				if (_strnicmp(Name, (char *)bp, strlen(Name)))
				{
					bp += strlen(Name);
					break;
				}
				return (0);

		//����ת��
			case 'n':	//���з�
			case 't':	//tab���Ʊ��
				LEGAL_ALT(0);
				while (isspace(*bp))
					bp++;
				break;

			default:	//��֧�ֵ�/δ֪��
				return (0);
		}//case
	}//switch

	//LINTED functional specification
	return ((char *)bp);
}

//����
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
tm�ṹ�и���Ա���壺
tm_sec		��ǰ�����ڵ�������0-61��
tm_min		��ǰСʱ�ڵķ�������0-59��
tm_hour		��ҹ���Сʱ����0-23��
tm_mday		�·��еĵڼ��죨1-31��
tm_mon		��һ������˼����£�0-11��
tm_year		���-1900
tm_wday		������������˼��죨0-6��
tm_yday		������һ��һ������˶����죨0-365��
unparsed	date ��δ��ͨ��ָ���� format ʶ��Ĳ���
*/