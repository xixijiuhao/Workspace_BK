//��  �ܣ���ò���ϵͳĬ������ID
//�����ˣ�������

#pragma once 

#include "windows.h"
#include <vector>

using namespace std;


//���ϵͳĬ������ID
class DefaultLanguage 
{
private:
	vector<LCID> m_licds;
	LCID update_locale(LANGID langid)
	{
		LCID lcid = MAKELCID(langid, SORT_DEFAULT);

		for (size_t i = 0; i < m_licds.size(); ++i)
			if (m_licds[i] == lcid)
				return lcid;

		//LANGID langid = LANGIDFROMLCID(lcid);
		m_licds.push_back(::ConvertDefaultLocale(lcid));

		LANGID nPrimaryLang = PRIMARYLANGID(langid);
		lcid = MAKELCID(MAKELANGID(nPrimaryLang, SUBLANG_NEUTRAL), SORT_DEFAULT);
		m_licds.push_back(::ConvertDefaultLocale(lcid));

		return lcid;
	}
public:
	DefaultLanguage(LANGID langid = 0)
	{
		if (langid)
			update_locale(langid);
		update_locale(GetUserDefaultUILanguage());
		update_locale(GetSystemDefaultUILanguage());

		update_locale(GetUserDefaultLangID());
		update_locale(GetSystemDefaultLangID());
	}

public:
	//���ϵͳĬ������
	LANGID get_default_langid()
	{
		wchar_t szLangCode[4];
		LCID lcid = 0;

		for (size_t i = 0; i < m_licds.size(); ++i)
		{
			lcid = m_licds[i];
			if (lcid == 0)
				continue;

			int nRes = ::GetLocaleInfo(lcid, LOCALE_SABBREVLANGNAME, szLangCode, sizeof(szLangCode) / sizeof(szLangCode[0]));
			if (nRes)
				break;
		}

		return LANGIDFROMLCID(lcid);
	}	
	//�����ʢϵͳĿǰ����֧�ֵļ�������
	LANGID get_default_langid_ex()
	{
		LANGID lang_id = get_default_langid();

		if (PRIMARYLANGID(lang_id)!=LANG_CHINESE)
			lang_id = MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT);
		else if (SUBLANGID(lang_id)==SUBLANG_CHINESE_SIMPLIFIED || SUBLANGID(lang_id)==SUBLANG_CHINESE_SINGAPORE)
			lang_id = MAKELCID(MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED), SORT_DEFAULT);
		else 
			lang_id = MAKELCID(MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_TRADITIONAL), SORT_DEFAULT);	
		return lang_id;
	}
};