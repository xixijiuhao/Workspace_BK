//功  能：资源文件读取，在资源文件中读取字符串、窗体、图片、图片组
//		  读取资源时可以指定语言，如果该资源在指定的语言下不存在，则会在默认语言中查找
//		  简体中文字符串转繁体
//创建人：刘明祥

#include "PreInclude.h"

//资源模块句柄
HMODULE g_hInstance = 0;
LANGID LoadRC::m_lang_id = 0;
LANGID LoadRC::m_lang_id_def = 0;
char LoadRC::buf[4096] = "";


///////////////////////////////////////////////// LoadRC ////////////////////////////////////////////////
//获得默认的资源句柄
HMODULE LoadRC::get_default_module()
{
	HMODULE ret = nullptr;
	GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT | GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCWSTR)&LoadRC::get_default_module, &ret);
	return ret;
}

HGLOBAL LoadRC::load_res(HINSTANCE instance, LPWSTR res_type, UINT res_id, WORD lang_id, __out HRSRC & res_hwnd)
{
	if (res_type == RT_STRING)
	{
		/*
		The and operation (& static_cast<WORD>(~0)) protects the expression from being greater
		than WORD - this would cause a runtime error when the application is compiled with /RTCc flag.
		*/
		res_hwnd = ::FindResourceExW(instance, (LPWSTR)res_type, MAKEINTRESOURCEW((((res_id >> 4) + 1) & static_cast<WORD>(~0))), lang_id);
		if (res_hwnd == nullptr && lang_id)
			res_hwnd = ::FindResourceExW(instance, (LPWSTR)res_type, MAKEINTRESOURCEW((((res_id >> 4) + 1) & static_cast<WORD>(~0))), 0);
	}
	else
	{
		res_hwnd = ::FindResourceEx(instance, res_type, MAKEINTRESOURCE(res_id), lang_id);
		if (res_hwnd == nullptr && lang_id)
			res_hwnd = ::FindResourceEx(instance, res_type, MAKEINTRESOURCE(res_id), 0);
	}
	if (res_hwnd == nullptr)
		return nullptr;

	return ::LoadResource(instance, res_hwnd);
}

//从资源里读取指定语言的字符串, 如果系统里不存在这个语言的资源则从系统默认语言的资源里读取
wstring LoadRC::w_load_str(HINSTANCE instance, UINT str_id, WORD lang_id)
{
	HRSRC res_hwnd;
	HGLOBAL global = load_res(instance, RT_STRING, str_id, lang_id, res_hwnd);
	if (global == nullptr)
		return L"";

	const StrSourceImage* image = (const StrSourceImage*)::LockResource(global);
	if (image == nullptr)
		return L"";

	ULONG res_size = ::SizeofResource(instance, res_hwnd);
	const StrSourceImage* image_end = (const StrSourceImage*)(LPBYTE(image) + res_size);
	UINT index = str_id & 0x000f;

	//从字符串镜像首地址开始向后找到当前str_id对应的字符串资源
	while ((index > 0) && (image < image_end))
	{
		image = (const StrSourceImage*)(LPBYTE(image) + (sizeof(StrSourceImage) + (image->length * sizeof(WCHAR))));
		index--;
	}
	if (image >= image_end)
		return L"";
	if (image->length == 0)
		return L"";
	
	return wstring(image->res_str, image->length);
}

string LoadRC::load_str(HINSTANCE instance, UINT str_id, WORD lang_id)
{
	wstring ret = w_load_str(instance, str_id, lang_id);

	//转为多字节字符串，用静态buf，减少字符串赋值拷贝
	int buf_len = WideCharToMultiByte(CP_ACP, 0, ret.c_str(), ret.length(), nullptr, 0, nullptr, nullptr);
	buf_len = min(buf_len, sizeof(buf) - 1);
	WideCharToMultiByte(CP_ACP, 0, ret.c_str(), ret.length(), buf, buf_len, nullptr, nullptr);
	buf[buf_len] = 0;

	return buf;
}

HRESULT LoadRC::load_data(HINSTANCE instance, LPWSTR res_type, UINT res_id, WORD lang_id, __out IStream & stream)
{
	HRSRC res_hwnd;
	HGLOBAL global = load_res(instance, res_type, res_id, lang_id, res_hwnd);
	if (global == nullptr)
		return 0;

	const void* image = (const void*)::LockResource(global);
	if (image == nullptr)
	{
		FreeResource(global);
		return 0;
	}
	ULONG res_size = ::SizeofResource(instance, res_hwnd);

	HRESULT ret = stream.Write(image, res_size, &res_size);

	FreeResource(global);

	return ret;
}

//设置程序语言
void LoadRC::set_language(const LANGID lang_id)
{
	DefaultLanguage def_lang;
	m_lang_id_def = def_lang.get_default_langid_ex();

	m_lang_id = lang_id;	
	if (m_lang_id == 0)
		m_lang_id = m_lang_id_def;
	else if (m_lang_id != m_lang_id_def)
		::SetThreadUILanguage(lang_id);
}
HMODULE LoadRC::get_instance()
{
	//if (g_hInstance == 0)
		g_hInstance = get_default_module();
	return g_hInstance;
}
//读取文本资源
string LoadRC::load_str(UINT str_id)
{
	return load_str(get_instance(), str_id, m_lang_id);
}
//读取文本资源
wstring LoadRC::w_load_str(UINT str_id)
{
	return w_load_str(get_instance(), str_id, m_lang_id);
}

HGLOBAL LoadRC::load_dlg_template(UINT dlg_id, HINSTANCE instance)
{
	if (instance == 0)
		instance = get_instance();

	HRSRC res_hwnd;
	return load_res(instance, RT_DIALOG, dlg_id, m_lang_id, res_hwnd);
}

LPCDLGTEMPLATE LoadRC::load_dlg_template_ex(UINT dlg_id)
{
	return (DLGTEMPLATE *)LockResource(load_dlg_template(dlg_id));
}

HGLOBAL LoadRC::load_menu_template(UINT menu_id)
{
	HRSRC res_hwnd;
	return load_res(get_instance(), RT_MENU, menu_id, m_lang_id, res_hwnd);
}

HRESULT LoadRC::load_image(UINT img_id, __out IStream & stream)
{
	return load_data(get_instance(), RT_BITMAP, img_id, m_lang_id, stream);
}
//不指定语言
HBITMAP LoadRC::load_image(UINT img_id)
{
	return LoadBitmapW(get_instance(), MAKEINTRESOURCEW(img_id));
}

//不指定语言
#if defined(COMMCTRL)
HIMAGELIST LoadRC::load_img_list(UINT img_id, int cx, int grow, COLORREF mask)
{
	return ImageList_LoadBitmap(get_instance(), (LPCTSTR)(DWORD_PTR)img_id, cx, grow, mask);
}
#endif


//简体转繁体
//size_t LoadRC::chs_to_cht(char * _dest, size_t _size, const char * _sour)
//{
//	_dest[0] = 0;
//	if (_size==0 || !_sour || _sour[0]==0)
//		return 0;
//
//	_dest[0] = 0;
//	if (PRIMARYLANGID(m_lang_id)!=LANG_CHINESE || SUBLANGID(m_lang_id)==SUBLANG_CHINESE_SIMPLIFIED || SUBLANGID(m_lang_id)==SUBLANG_CHINESE_SINGAPORE)
//		return strncpy_s(_dest, _size, _sour, _size-1);
//
//	size_t len = min(strlen(_sour), _size);	
//	if (len==0)
//		return 0;
//
//	DWORD lcid = MAKELCID(MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED), SORT_CHINESE_PRC);
//	LCMapString(lcid, LCMAP_TRADITIONAL_CHINESE, ansi_to_unicode(_sour).c_str(), len, ansi_to_unicode(_dest).c_str(), len);
//	_dest[len] = 0;
//
//	//简体系统中没必要往下执行，虽然不会出错，但效率稍高一些
//	if (SUBLANGID(m_lang_id_def)==SUBLANG_CHINESE_SIMPLIFIED || SUBLANGID(m_lang_id_def)==SUBLANG_CHINESE_SINGAPORE) 
//		return len;
//
//	//将GBK转到unicode，再将unicode转到本地字符集
//	wchar_t * pws = new wchar_t[len + 1];
//	MultiByteToWideChar(936, 0, _dest, len, pws, len + 1);
//	WideCharToMultiByte(CP_ACP, 0, pws, len, _dest, len + 1, nullptr, nullptr);
//	_dest[len] = 0;
//	delete[] pws;
//
//	return len;
//}
////简体转繁体
//void LoadRC::chs_to_cht(string & _dest, const char * _sour)
//{
//	if (PRIMARYLANGID(m_lang_id)!=LANG_CHINESE || SUBLANGID(m_lang_id)==SUBLANG_CHINESE_SIMPLIFIED || SUBLANGID(m_lang_id)==SUBLANG_CHINESE_SINGAPORE) 
//	{
//		_dest = _sour;
//		return;
//	}
//
//	size_t len = strlen(_sour);
//	char * _buf = (char *)malloc(len + 1);
//
//	chs_to_cht(_buf, len, _sour);
//
//	_dest = _buf;
//	delete _buf;
//}