#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif 

#include <string>
#include "shlwapi.h"
#include <windows.h>

using namespace std;

//一个动态库对应一个类
class LoadLib
{
private:	
	string m_lib_name;
	HMODULE m_hlib;
public:
	//将基于主程序目录的相对路径转为绝对路径
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
LoadLibraryEx函数按照下面的顺序来搜索文件：
1)pszDLLPathName参数中设定的目录。	(设定了LOAD_WITH_ALTERED_SEARCH_PATH标志会增加此目录)
2)进程的当前目录。
3)当前目录。							(禁用安全DLL搜索模式会增加此路径)
4)Windows的系统目录。
5)16位系统的目录。
6)Windows目录。
7)PATH环境变量中列出的目录。
*/