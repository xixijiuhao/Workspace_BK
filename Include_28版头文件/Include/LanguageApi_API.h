#ifndef _LANGUAGEAPI_API_
#define _LANGUAGEAPI_API_

const wchar_t PLUG_LANGUAGEAPI_NAME[] = L"LanguageApi";
const wchar_t PLUG_LANGUAGEAPI_VERSION[] = L"9.3.0.1";

class ILanguageApi
{
public:
	//设置模块名
	virtual bool SetModule(const wchar_t module[]) = 0;
	//获取当前语言id： CHS = 0x0804; CHT=0x0404; ENU = 0x0409;
	virtual const int __cdecl LangId() = 0;
	//获取语言index：  CHS = 0; CHT = 1; ENU = 2;
	virtual const int __cdecl LangIndex() = 0;
	//获取语言名称： L"CHS"; L"CHT"; L"ENU"; 
	virtual const wchar_t* __cdecl LangName() = 0;
	//获取语言资源：根据定义好的资源索引，暂定res_index = [0-9999]，lang_index为-1当前语言
	virtual const wchar_t* __cdecl LangText(const int res_index, const int lang_index = -1) = 0;
	//语言id 转换为 语言index
	virtual const int __cdecl LangIdToIndex(const int lang_id) = 0;
	//语言index 转换为 语言id
	virtual const int __cdecl LangIndexToId(const int lang_index) = 0;
};

#endif