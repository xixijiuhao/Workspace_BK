//��  �ܣ���Դ�ļ���ȡ������Դ�ļ��ж�ȡ�ַ��������塢ͼƬ��ͼƬ��
//		  ��ȡ��Դʱ����ָ�����ԣ��������Դ��ָ���������²����ڣ������Ĭ�������в���
//		  ���������ַ���ת����
//�����ˣ�������


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
	inline static wstring w_load_str(HINSTANCE instance, UINT str_id, WORD lang_id = 0);
	inline static string load_str(HINSTANCE instance, UINT str_id, WORD lang_id = 0);	//����Դ���ȡָ�����Ե��ַ���
	inline static HGLOBAL load_res(HINSTANCE instance, LPWSTR res_type, UINT res_id, WORD lang_id, __out HRSRC & hRsrc);
	inline static HRESULT load_data(HINSTANCE instance, LPWSTR res_type, UINT res_id, WORD lang_id, __out IStream & stream);
public:	
	//���ó�������
	static void set_language(const LANGID lang_id);
	//��ó�������
	inline static LANGID get_lang_id(){ return m_lang_id; };
	//���ϵͳĬ������
	inline static LANGID get_lang_id_def(){ return m_lang_id_def; };
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
	//��ȡBITMAP��Դ�������ɹ�����S_OK������ʱ������������һ���� LPSTREAM pStream = nullptr; ::CreateStreamOnHGlobal(nullptr, TRUE, &pStream);
	static HRESULT load_image(UINT img_id, __out IStream & stream);
	//��ȡBITMAP��Դ
	static HBITMAP load_image(UINT img_id);
	
#if defined(COMMCTRL)
	//��ȡͼƬ��IMAGELIST
	static HIMAGELIST load_img_list(UINT img_id, int cx, int grow, COLORREF mask);
#endif
		
	//����ת����
	//static inline size_t chs_to_cht(char * _dest, size_t _size, const char * _sour);
	//����ת����
	//static void chs_to_cht(string & _dest, const char * _sour);
	//����ת����
	template<size_t _size>
	static size_t chs_to_cht(char (&_dest)[_size], const char * _sour)
	{
		return chs_to_cht(_dest, _size, _sour);
	}
	//����ת����
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
	static unsigned int curr_codepage()
	{
		char buf[6] = { 0 };
		GetLocaleInfoA(m_lang_id, LOCALE_IDEFAULTCODEPAGE, buf, sizeof(buf));
		return atoi(buf);
	}
	static wstring ansi_to_unicode(const string & str)
	{
		if (m_lang_id == 0x0404)
			return CCodes::ANSIToUnicode(str, 950);
		else
			return CCodes::ANSIToUnicode(str, 936);
	}
	static wstring ansi_to_unicode(const char * str)
	{
		if (str == nullptr)
			return L"";

		if (m_lang_id == 0x0404)
			return CCodes::ANSIToUnicode(str, 950);
		else
			return CCodes::ANSIToUnicode(str, 936);
	}	
	static string unicode_to_ansi(const wstring & str)
	{
		return CCodes::UnicodeToANSI(str, CP_ACP);
	}
	static string unicode_to_ansi(const wchar_t * str)
	{
		if (str == nullptr)
			return "";

		return CCodes::UnicodeToANSI(str, CP_ACP);
	}
	static string unicode_to_ansi_local(const wchar_t * str)
	{
		if (str == nullptr)
			return "";

		if (m_lang_id == 0x0404)
			return CCodes::UnicodeToANSI(str, 950);
		else
			return CCodes::UnicodeToANSI(str, 936);
	}
};