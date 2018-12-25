//��  �ܣ���Դ�ļ���ȡ������Դ�ļ��ж�ȡ�ַ��������塢ͼƬ��ͼƬ��
//		  ��ȡ��Դʱ����ָ�����ԣ��������Դ��ָ���������²����ڣ������Ĭ�������в���
//		  ���������ַ���ת����
//�����ˣ�������


#pragma once 

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif 

#include "windows.h"
#include <string>
#include "ObjIdl.h"

#ifdef COMMCTRL
#include "commctrl.h"
#include "atlimage.h"
#endif

using namespace std;


#define LOADRC_STR_BUF_LENGTH (4096)

//������Դ
class LoadRC
{
private:
	static char buf[LOADRC_STR_BUF_LENGTH];
	static LANGID m_lang_id;				//�����Ҫ��ʾ������
	static LANGID m_lang_id_def;			//����ϵͳ��ǰ����

	#pragma warning(push)
	#pragma warning(disable: 4200)
	struct StrSourceImage
	{
		WORD length;
		WCHAR res_str[];
	};
	#pragma warning(pop)

	inline static HMODULE get_default_module();
	inline static bool w_load_str(wstring & ret, HINSTANCE instance, UINT str_id, WORD lang_id = 0);
	inline static bool load_str(string & ret, HINSTANCE instance, UINT str_id, WORD lang_id = 0);	//����Դ���ȡָ�����Ե��ַ���
	inline static HGLOBAL load_res(HINSTANCE instance, LPSTR res_type, UINT res_id, WORD lang_id, __out HRSRC & hRsrc);
	inline static HRESULT load_data(HINSTANCE instance, LPSTR res_type, UINT res_id, WORD lang_id, __out IStream & stream);	
	static unsigned int local_codepage();
	static unsigned int curr_codepage();
public:	
	//���ó�������
	static void set_language(const LANGID lang_id);
	//��ó�������
	inline static LANGID get_lang_id(){ return m_lang_id; };
	//���ϵͳĬ������
	inline static LANGID get_lang_id_def(){ return m_lang_id_def; };
	//��LANGIDתΪCHS��CHT��
	static string get_lang_str(const LANGID lang_id = 0);
	//��õ�ǰdll��Դ���
	static HMODULE get_instance();
	//��ȡ�ַ���
	static wstring w_load_str(UINT str_id);	
	static string load_str(UINT str_id);											
	//��ȡ�Ի���ģ��
	static HGLOBAL load_dlg_template(UINT dlg_id, HINSTANCE instance = 0);
	static LPCDLGTEMPLATE load_dlg_template_ex(UINT dlg_id);
	//��ȡ�˵�ģ��
	static HGLOBAL load_menu_template(UINT menu_id);
	//��ȡ��Դ�������ɹ�����S_OK������ʱ������������һ���� LPSTREAM pStream = nullptr; ::CreateStreamOnHGlobal(nullptr, TRUE, &pStream);
	static HRESULT load_data(char * res_type, const UINT rse_id, __out IStream & stream);
	//��ȡBITMAP��Դ�������ɹ�����S_OK������ʱ������������һ���� LPSTREAM pStream = nullptr; ::CreateStreamOnHGlobal(nullptr, TRUE, &pStream);
	static HRESULT load_image(UINT img_id, __out IStream & stream);
	//��ȡBITMAP��Դ
	static HBITMAP load_image(UINT img_id);
	
#if defined(COMMCTRL)
	static void load_png(UINT png_id, __out CImage & img);
	//static void load_png(UINT png_id, __out CBitmap & bmp);
	//��ȡͼƬ��IMAGELIST
	static HIMAGELIST load_img_list(UINT img_id, int cx, int grow, COLORREF mask);
	static HIMAGELIST load_img_list_form_png(UINT png_id, int cx, int cy, COLORREF mask, int grow);
#endif
		
	//����ת����
	static size_t chs_to_cht(char * _dest, size_t _size, const char * _sour);
	//����ת����
	static void chs_to_cht(string & _dest, const char * _sour);
	//����ת����
	template<size_t _size>
	static size_t chs_to_cht(char (&_dest)[_size], const char * _sour)
	{
		return chs_to_cht(_dest, _size, _sour);
	}
	//����ת����,ԭʼ�ַ��������Ǽ���
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
	static wstring ansi_to_unicode(const string & str, unsigned int code_page = 0);//CP_ACP
	static wstring ansi_to_unicode(const char * str, unsigned int coee_page = 0)//CP_ACP
	{
		return str == nullptr ? L"" : ansi_to_unicode(string(str), coee_page);
	}
	static string unicode_to_ansi(const wstring & str, unsigned int code_page = 0);//CP_ACP
	static string unicode_to_ansi(const wchar_t * str, unsigned int code_page = 0)//CP_ACP
	{
		return str == nullptr ? "" : unicode_to_ansi(wstring(str), code_page);
	}
};