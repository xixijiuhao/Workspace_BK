#ifndef _MOUSEORDER_STRING_
#define _MOUSEORDER_STRING_

class CMouseString
{
public:
	//�ַ���ת��������
	static char WStrToOrderType(const wchar_t *Str);//
	//�ַ���ת��Ч����
	static char WStrToValidType(const wchar_t *Str);
	//��Ч����ת�ַ���
	static wstring ValidTypeToWStr(char ctype);
};

#endif
