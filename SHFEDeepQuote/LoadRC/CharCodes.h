#pragma once

#include <string>
#include <windows.h>

using namespace std;

class CCodes
{
public:
	static wstring ANSIToUnicode( const string& str )
	{
		size_t  len = 0;
		len = str.length();
		int  unicodeLen = ::MultiByteToWideChar( 936/*CP_ACP*/,0,str.c_str(),-1,nullptr,0 );  

		wchar_t *  pUnicode;  
		pUnicode = new  wchar_t[unicodeLen+1];  
		memset(pUnicode,0,(unicodeLen+1)*sizeof(wchar_t));  

		::MultiByteToWideChar( 936/*CP_ACP*/,0,str.c_str(),-1,(LPWSTR)pUnicode,unicodeLen );  
		wstring  rt = ( wchar_t* )pUnicode;
		delete[]  pUnicode; 

		return  rt;  
	}

	static string UnicodeToANSI( const wstring& str )
	{
		char*   pElementText;
		int    iTextLen;
		iTextLen = WideCharToMultiByte( CP_ACP,0,str.c_str(),-1,nullptr,0,nullptr,nullptr );

		pElementText = new char[iTextLen + 1];
		memset( ( void* )pElementText, 0, sizeof( char ) * ( iTextLen + 1 ) );

		::WideCharToMultiByte( CP_ACP,0,str.c_str(),-1,pElementText,iTextLen,nullptr,nullptr );
		string strText = pElementText;
		delete[] pElementText;

		return strText;
	}

	static wstring UTF8ToUnicode( const string& str )
	{
		size_t  len = 0;
		len = str.length();
		int  unicodeLen = ::MultiByteToWideChar( CP_UTF8,0,str.c_str(),-1,nullptr,0 );  

		wchar_t *  pUnicode;  
		pUnicode = new  wchar_t[unicodeLen+1];  
		memset(pUnicode,0,(unicodeLen+1)*sizeof(wchar_t));  

		::MultiByteToWideChar( CP_UTF8,0,str.c_str(),-1,(LPWSTR)pUnicode,unicodeLen );  
		wstring   rt= ( wchar_t* )pUnicode;
		delete[]  pUnicode; 

		return  rt;  
	}

	static string UnicodeToUTF8( const wstring& str )
	{
		char*     pElementText;
		int    iTextLen;
		// wide char to multi char
		iTextLen = WideCharToMultiByte( CP_UTF8,0,str.c_str(),-1,nullptr,0,nullptr,nullptr );
		pElementText = new char[iTextLen + 1];
		memset( ( void* )pElementText, 0, sizeof( char ) * ( iTextLen + 1 ) );
		::WideCharToMultiByte( CP_UTF8,0,str.c_str(),-1,pElementText,iTextLen,nullptr,nullptr );
		string  strText = pElementText;
		delete[] pElementText;
		return strText;
	}
};