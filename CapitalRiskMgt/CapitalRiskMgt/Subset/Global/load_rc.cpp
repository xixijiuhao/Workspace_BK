//功  能：资源文件读取，在资源文件中读取字符串、窗体、图片、图片组
//		  读取资源时可以指定语言，如果该资源在指定的语言下不存在，则会在默认语言中查找
//		  简体中文字符串转繁体
//创建人：刘明祥

#include "load_rc.h"
#include "def_lang.h"

//资源模块句柄
HMODULE g_hInstance = 0;


LANGID LoadRC::m_lang_id = 0;
LANGID LoadRC::m_lang_id_def = 0;
char LoadRC::buf[4096] = "";


/*
 * Predefined Resource Types
 */
#define RT_CURSORA           MAKEINTRESOURCEA(1)
#define RT_BITMAPA           MAKEINTRESOURCEA(2)
#define RT_ICONA             MAKEINTRESOURCEA(3)
#define RT_MENUA             MAKEINTRESOURCEA(4)
#define RT_DIALOGA           MAKEINTRESOURCEA(5)
#define RT_STRINGA           MAKEINTRESOURCEA(6)
#define RT_FONTDIRA          MAKEINTRESOURCEA(7)
#define RT_FONTA             MAKEINTRESOURCEA(8)
#define RT_ACCELERATORA      MAKEINTRESOURCEA(9)
#define RT_RCDATAA           MAKEINTRESOURCEA(10)
#define RT_MESSAGETABLEA     MAKEINTRESOURCEA(11)


///////////////////////////////////////////////// LoadRC ////////////////////////////////////////////////
//获得默认的资源句柄
HMODULE LoadRC::get_default_module()
{
	HMODULE ret = nullptr;
	GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT | GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCSTR)&LoadRC::get_default_module, &ret);
	return ret;
}

HGLOBAL LoadRC::load_res(HINSTANCE instance, LPSTR res_type, UINT res_id, WORD lang_id, __out HRSRC & res_hwnd)
{
	if (res_type == RT_STRINGA)
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
		res_hwnd = ::FindResourceExA(instance, res_type, MAKEINTRESOURCEA(res_id), lang_id);
		if (res_hwnd == nullptr && lang_id)
			res_hwnd = ::FindResourceExA(instance, res_type, MAKEINTRESOURCEA(res_id), 0);
	}
	if (res_hwnd == nullptr)
		return nullptr;

	return ::LoadResource(instance, res_hwnd);
}

//从资源里读取指定语言的字符串, 如果系统里不存在这个语言的资源则从系统默认语言的资源里读取
bool LoadRC::w_load_str(wstring & ret, HINSTANCE instance, UINT str_id, WORD lang_id)
{
	ret = L"";

	HRSRC res_hwnd;
	HGLOBAL global = load_res(instance, RT_STRINGA, str_id, lang_id, res_hwnd);
	if (global == nullptr)
		return false;

	const StrSourceImage* image = (const StrSourceImage*)::LockResource(global);
	if (image == nullptr)
		return false;

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
		return false;
	if (image->length == 0)
		return false;
	
	ret.append(image->res_str, image->length);
	return true;
}

bool LoadRC::load_str(string & ret, HINSTANCE instance, UINT str_id, WORD lang_id)
{
	wstring w_ret;
	if (!w_load_str(w_ret, instance, str_id, lang_id))
		return false;

	//转为多字节字符串，用静态buf，减少字符串赋值拷贝
	int buf_len = WideCharToMultiByte(curr_codepage(), 0, w_ret.c_str(), w_ret.length(), nullptr, 0, nullptr, nullptr);
	buf_len = min(buf_len, sizeof(buf) - 1);
	WideCharToMultiByte(curr_codepage(), 0, w_ret.c_str(), w_ret.length(), buf, buf_len, nullptr, nullptr);
	buf[buf_len] = 0;

	ret = buf;
	return true;
}

HRESULT LoadRC::load_data(HINSTANCE instance, LPSTR res_type, UINT res_id, WORD lang_id, __out IStream & stream)
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

string LoadRC::get_lang_str(const LANGID lang_id)
{		
	LCID lcid = 0;
	if (lang_id == 0)
		lcid = MAKELCID(m_lang_id, SORT_DEFAULT);
	else 
		lcid = MAKELCID(lang_id, SORT_DEFAULT);

	char lang_code[4];		
	int ret = ::GetLocaleInfoA(lcid, LOCALE_SABBREVLANGNAME, lang_code, sizeof(lang_code));
	return lang_code;
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
	string ret;
	load_str(ret, get_instance(), str_id, m_lang_id);
	return ret;
}
//读取文本资源
wstring LoadRC::w_load_str(UINT str_id)
{
	wstring ret;
	w_load_str(ret, get_instance(), str_id, m_lang_id);
	return ret;
}

HGLOBAL LoadRC::load_dlg_template(UINT dlg_id, HINSTANCE instance)
{
	if (instance == 0)
		instance = get_instance();

	HRSRC res_hwnd;
	return load_res(instance, RT_DIALOGA, dlg_id, m_lang_id, res_hwnd);
}

LPCDLGTEMPLATE LoadRC::load_dlg_template_ex(UINT dlg_id)
{
	return (DLGTEMPLATE *)LockResource(load_dlg_template(dlg_id));
}

HGLOBAL LoadRC::load_menu_template(UINT menu_id)
{
	HRSRC res_hwnd;
	return load_res(get_instance(), RT_MENUA, menu_id, m_lang_id, res_hwnd);
}

HRESULT LoadRC::load_image(UINT img_id, __out IStream & stream)
{
	return load_data(get_instance(), RT_BITMAPA, img_id, m_lang_id, stream);
}
HRESULT LoadRC::load_data(char * res_type, const UINT rse_id, __out IStream & stream)
{
	return load_data(get_instance(), res_type, rse_id, m_lang_id, stream);
}

//不指定语言
HBITMAP LoadRC::load_image(UINT img_id)
{
	return LoadBitmapW(get_instance(), MAKEINTRESOURCEW(img_id));
}

//不指定语言
#if defined(COMMCTRL)

void LoadRC::load_png(UINT png_id, __out CImage & img)
{	
	IStream* pstm;
	CreateStreamOnHGlobal(nullptr, TRUE, &pstm);
	LoadRC::load_data("png", png_id, *pstm);
	img.Load(pstm);
	pstm->Release();
}
//void LoadRC::load_png(UINT png_id, __out CBitmap & bmp)
//{
//	CImage img;
//	LoadRC::load_png(png_id, img);
//	if (img.GetBits() == 0)
//		return;
//	bmp.Attach(img.Detach());
//}
HIMAGELIST LoadRC::load_img_list(UINT img_id, int cx, int grow, COLORREF mask)
{
	return ImageList_LoadBitmap(get_instance(), (LPCTSTR)(DWORD_PTR)img_id, cx, grow, mask);
}
HIMAGELIST LoadRC::load_img_list_form_png(UINT png_id, int cx, int cy, COLORREF mask, int grow)
{
	HIMAGELIST img_lst = ImageList_Create(cx, cy, ILC_MASK | ILC_COLOR24, 1, grow);
	if (img_lst == nullptr)
		return nullptr;

	CImage img;
	LoadRC::load_png(png_id, img);

	if (ImageList_AddMasked(img_lst, img.Detach(), mask) >= 0) 
		return img_lst;

	ImageList_Destroy(img_lst);
	return nullptr;
}

#endif


//简体转繁体
size_t LoadRC::chs_to_cht(char * _dest, size_t _size, const char * _sour)
{
	_dest[0] = 0;
	if (_size==0 || !_sour || _sour[0]==0)
		return 0;

	_dest[0] = 0;
	if (PRIMARYLANGID(m_lang_id)!=LANG_CHINESE || SUBLANGID(m_lang_id)==SUBLANG_CHINESE_SIMPLIFIED || SUBLANGID(m_lang_id)==SUBLANG_CHINESE_SINGAPORE)
		return strncpy_s(_dest, _size, _sour, _size-1);

	size_t len = min(strlen(_sour), _size);	
	if (len==0)
		return 0;

	DWORD lcid = MAKELCID(MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED), SORT_CHINESE_PRC);
	LCMapStringA(lcid, LCMAP_TRADITIONAL_CHINESE, _sour, len, _dest, len);
	_dest[len] = 0;

	//简体系统中没必要往下执行，虽然不会出错，但效率稍高一些
	//if (SUBLANGID(m_lang_id_def)==SUBLANG_CHINESE_SIMPLIFIED || SUBLANGID(m_lang_id_def)==SUBLANG_CHINESE_SINGAPORE) 
	//	return len;

	//将GBK转到unicode，再将unicode转到本地字符集
	wchar_t * pws = new wchar_t[len + 1];
	MultiByteToWideChar(936, 0, _dest, len, pws, len + 1);
	WideCharToMultiByte(950, 0, pws, len, _dest, len + 1, nullptr, nullptr);
	_dest[len] = 0;
	delete[] pws;

	return len;
}
//简体转繁体
void LoadRC::chs_to_cht(string & _dest, const char * _sour)
{
	if (PRIMARYLANGID(m_lang_id)!=LANG_CHINESE || SUBLANGID(m_lang_id)==SUBLANG_CHINESE_SIMPLIFIED || SUBLANGID(m_lang_id)==SUBLANG_CHINESE_SINGAPORE) 
	{
		_dest = _sour;
		return;
	}

	size_t len = strlen(_sour);
	char * _buf = (char *)malloc(len + 1);

	chs_to_cht(_buf, len, _sour);

	_dest = _buf;
	free(_buf);
}

unsigned int LoadRC::local_codepage()  
{  
    char buf[6] = {0};
    GetLocaleInfoA(LOCALE_SYSTEM_DEFAULT, LOCALE_IDEFAULTCODEPAGE, buf, sizeof(buf));  
    return atoi(buf);  
}

unsigned int LoadRC::curr_codepage()  
{  
    char buf[6] = {0};
    GetLocaleInfoA(m_lang_id, LOCALE_IDEFAULTCODEPAGE, buf, sizeof(buf));		//得到本地代码页 
    return atoi(buf);  //ansi转成int
}

wstring LoadRC::ansi_to_unicode(const string & str, unsigned int code_page)
{
	if (code_page == 0)
		code_page = curr_codepage();

	size_t  len = 0;
	len = str.length();
	int  unicodeLen = ::MultiByteToWideChar(code_page, 0, str.c_str(), -1, nullptr, 0 );  

	wchar_t *  pUnicode;  
	pUnicode = new  wchar_t[unicodeLen+1];  
	memset(pUnicode,0,(unicodeLen+1)*sizeof(wchar_t));  

	::MultiByteToWideChar(code_page,0,str.c_str(),-1,(LPWSTR)pUnicode,unicodeLen );  
	wstring  rt = ( wchar_t* )pUnicode;
	delete[]  pUnicode; 

	return  rt;  
}
string LoadRC::unicode_to_ansi(const wstring & str, unsigned int code_page)
{
	if (code_page == 0)
		code_page = curr_codepage();

	char*   pElementText;
	int    iTextLen;
	iTextLen = WideCharToMultiByte(code_page, 0, str.c_str(), -1, nullptr, 0, nullptr, nullptr);

	pElementText = new char[iTextLen + 1];
	memset((void*)pElementText, 0, sizeof(char) * (iTextLen + 1));

	::WideCharToMultiByte(code_page, 0, str.c_str(), -1, pElementText, iTextLen, nullptr, nullptr);
	string strText;
	if (code_page == 936 && local_codepage() == 950)
		chs_to_cht(strText, pElementText);			//936代表简体中文，950代表繁体中文
	else
		strText = pElementText;
	delete[] pElementText;

	return strText;
}