#ifndef _LANGUAGEAPI_API_
#define _LANGUAGEAPI_API_

const wchar_t PLUG_LANGUAGEAPI_NAME[] = L"LanguageApi";
const wchar_t PLUG_LANGUAGEAPI_VERSION[] = L"9.3.0.1";

class ILanguageApi
{
public:
	//����ģ����
	virtual bool SetModule(const wchar_t module[]) = 0;
	//��ȡ��ǰ����id�� CHS = 0x0804; CHT=0x0404; ENU = 0x0409;
	virtual const int __cdecl LangId() = 0;
	//��ȡ����index��  CHS = 0; CHT = 1; ENU = 2;
	virtual const int __cdecl LangIndex() = 0;
	//��ȡ�������ƣ� L"CHS"; L"CHT"; L"ENU"; 
	virtual const wchar_t* __cdecl LangName() = 0;
	//��ȡ������Դ�����ݶ���õ���Դ�������ݶ�res_index = [0-9999]��lang_indexΪ-1��ǰ����
	virtual const wchar_t* __cdecl LangText(const int res_index, const int lang_index = -1) = 0;
	//����id ת��Ϊ ����index
	virtual const int __cdecl LangIdToIndex(const int lang_id) = 0;
	//����index ת��Ϊ ����id
	virtual const int __cdecl LangIndexToId(const int lang_index) = 0;
};

#endif