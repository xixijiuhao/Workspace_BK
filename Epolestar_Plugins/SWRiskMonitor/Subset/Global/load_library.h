#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif 

#include <string>
#include "shlwapi.h"
#include <windows.h>

using namespace std;

//һ����̬���Ӧһ����
class LoadLib
{
private:	
	string m_lib_name;
	HMODULE m_hlib;
public:
	//������������Ŀ¼�����·��תΪ����·��
	string get_abs_path(const char * relative_path)
	{
		char path[MAX_PATH];
		GetModuleFileNameA(GetModuleHandle(nullptr), path, MAX_PATH);
		char* p = strrchr(path, '\\');
		*++p = 0;

		char ret[MAX_PATH] = {0};   
		string base = path;
		string relative = relative_path;
		if (base[base.size()-1]!='\\')
			base.append("\\");
		if (relative.length()>0 && relative[0]=='\\')
			relative.erase(0, 1);

		PathCombineA(ret, base.c_str(), relative.c_str()); 
		return ret;
	}
public:
	LoadLib(const char * lib_name): m_hlib(nullptr)
	{
		m_lib_name = get_abs_path(lib_name);
		m_hlib = LoadLibraryExA(m_lib_name.c_str(), 0, LOAD_WITH_ALTERED_SEARCH_PATH);
		_ASSERT(m_hlib);
	}
	virtual ~LoadLib()
	{
		if(m_hlib != nullptr )
			FreeLibrary(m_hlib);
	}
	void * get_function(const char * func_name) const
	{
		if(m_hlib != nullptr )
			return GetProcAddress(m_hlib, func_name);
		else
			return nullptr;
	}
	bool is_free()
	{
		return GetModuleHandleA(m_lib_name.c_str()) == 0;
	}
};


/*
LoadLibraryEx�������������˳���������ļ���
1)pszDLLPathName�������趨��Ŀ¼��	(�趨��LOAD_WITH_ALTERED_SEARCH_PATH��־�����Ӵ�Ŀ¼)
2)���̵ĵ�ǰĿ¼��
3)��ǰĿ¼��							(���ð�ȫDLL����ģʽ�����Ӵ�·��)
4)Windows��ϵͳĿ¼��
5)16λϵͳ��Ŀ¼��
6)WindowsĿ¼��
7)PATH�����������г���Ŀ¼��
*/