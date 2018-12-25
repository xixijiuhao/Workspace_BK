//功  能：资源文件读取，在资源文件中读取字符串、窗体、图片、图片组
//		  读取资源时可以指定语言，如果该资源在指定的语言下不存在，则会在默认语言中查找
//		  简体中文字符串转繁体
//创建人：刘明祥


#pragma once 

#include "windows.h"
#include <string>
#include "ObjIdl.h"
#include "CharCodes.h"

#if defined(COMMCTRL)
#include "commctrl.h"
#endif

using namespace std;


#define LOADRC_STR_BUF_LENGTH (4096)

//加载资源
class LoadRC
{
public:
	inline static HMODULE get_default_module();
private:
	static char buf[LOADRC_STR_BUF_LENGTH];
	static LANGID m_lang_id;				//本软件要显示的语言
	static LANGID m_lang_id_def;			//操作系统当前语言

	#pragma warning(push)
	#pragma warning(disable: 4200)
	struct StrSourceImage
	{
		WORD length;
		WCHAR res_str[];
	};
	#pragma warning(pop)

	inline static wstring w_load_str(HINSTANCE instance, UINT str_id, WORD lang_id = 0);
	inline static string load_str(HINSTANCE instance, UINT str_id, WORD lang_id = 0);	//从资源里读取指定语言的字符串
	inline static HGLOBAL load_res(HINSTANCE instance, LPWSTR res_type, UINT res_id, WORD lang_id, __out HRSRC & hRsrc);
	inline static HRESULT load_data(HINSTANCE instance, LPWSTR res_type, UINT res_id, WORD lang_id, __out IStream & stream);
public:	
	//设置程序语言
	static void set_language(const LANGID lang_id);
	//获得程序语言
	inline static LANGID get_lang_id(){ return m_lang_id; };
	//获得系统默认语言
	inline static LANGID get_lang_id_def(){ return m_lang_id_def; };
	//获得当前dll资源句柄
	static HMODULE get_instance();
	//读取字符串
	static wstring w_load_str(UINT str_id);	
	static string load_str(UINT str_id);											
	//读取对话框模版
	static HGLOBAL load_dlg_template(UINT dlg_id, HINSTANCE instance = 0);
	static LPCDLGTEMPLATE load_dlg_template_ex(UINT dlg_id);
	//读取菜单模版
	static HGLOBAL load_menu_template(UINT menu_id);
	//读取BITMAP资源到流，成功返回S_OK。调用时可以这样创建一个流 LPSTREAM pStream = nullptr; ::CreateStreamOnHGlobal(nullptr, TRUE, &pStream);
	static HRESULT load_image(UINT img_id, __out IStream & stream);
	//读取BITMAP资源
	static HBITMAP load_image(UINT img_id);
	
#if defined(COMMCTRL)
	//读取图片到IMAGELIST
	static HIMAGELIST load_img_list(UINT img_id, int cx, int grow, COLORREF mask);
#endif
		
	//简体转繁体
	//static inline size_t chs_to_cht(char * _dest, size_t _size, const char * _sour);
	//简体转繁体
	//static void chs_to_cht(string & _dest, const char * _sour);
	//简体转繁体
	template<size_t _size>
	static size_t chs_to_cht(char (&_dest)[_size], const char * _sour)
	{
		return chs_to_cht(_dest, _size, _sour);
	}
	//简体转繁体
	template<size_t _size>
	static size_t chs_to_cht_ex(char (&_str)[_size])
	{
		if (PRIMARYLANGID(m_lang_id)!=LANG_CHINESE || SUBLANGID(m_lang_id)==SUBLANG_CHINESE_SIMPLIFIED || SUBLANGID(m_lang_id)==SUBLANG_CHINESE_SINGAPORE) 
			return 0;

		string _dest;
		chs_to_cht(_dest, _str);
		strncpy_s(_str, _size, _dest.c_str(), _size-1);
		return _dest.size();
	}
	static bool is_chinese()
	{
		return PRIMARYLANGID(m_lang_id)==LANG_CHINESE && (SUBLANGID(m_lang_id)==SUBLANG_CHINESE_SIMPLIFIED || SUBLANGID(m_lang_id)==SUBLANG_CHINESE_SINGAPORE) ||
			PRIMARYLANGID(m_lang_id)==LANG_CHINESE_TRADITIONAL && (SUBLANGID(m_lang_id)==SUBLANG_CHINESE_TRADITIONAL || SUBLANGID(m_lang_id)==SUBLANG_CHINESE_HONGKONG ||
			SUBLANGID(m_lang_id)==SUBLANG_CHINESE_MACAU);
	}
	static wstring ansi_to_unicode(const string & str)
	{
		return CCodes::ANSIToUnicode(str);
	}
	static wstring ansi_to_unicode(const char * str)
	{
		if (str == nullptr)
			return L"";

		return CCodes::ANSIToUnicode(str);
	}
	static string unicode_to_ansi(const wstring & str)
	{
		return CCodes::UnicodeToANSI(str);
	}
	static string unicode_to_ansi(const wchar_t * str)
	{
		if (str == nullptr)
			return "";

		return CCodes::UnicodeToANSI(str);
	}
};