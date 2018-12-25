#ifndef _MOUSEORDER_STRING_
#define _MOUSEORDER_STRING_

class CMouseString
{
public:
	//字符串转定单类型
	static char WStrToOrderType(const wchar_t *Str);//
	//字符串转有效类型
	static char WStrToValidType(const wchar_t *Str);
	//有效类型转字符串
	static wstring ValidTypeToWStr(char ctype);
};

#endif
