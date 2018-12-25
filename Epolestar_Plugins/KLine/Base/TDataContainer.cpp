#include "BaseInclude.h"

void GetFilePathW(TCHAR* filePath, int length, TCHAR* fileName)
{
	GetModuleFileName(0, filePath, length);
	WCHAR* cp_pos = wcsrchr(filePath, '\\');
	if (0 != cp_pos)
		cp_pos[0] = 0;
	wcscat_s(filePath, length, fileName);
}
void GetFilePath(char* filePath, int length, char* fileName)
{
	GetModuleFileNameA(0, filePath, length);
	char* cp_pos = strrchr(filePath, '\\');
	if (0 != cp_pos)
		cp_pos[0] = 0;
	strcat_s(filePath, length, fileName);
}

void SplitStringW(TCHAR *strSource, TCHAR split, vector<wstring>& vecContent)
{
	wstring strContent = strSource;
	wstring::size_type  iPos = strContent.find(split);
	wstring strTemp;
	while (iPos != wstring::npos)
	{
		strTemp = strContent.substr(0, iPos);
		strContent = strContent.substr(++iPos, wstring::npos);
		vecContent.push_back(strTemp);
		iPos = strContent.find(split);
	}
	vecContent.push_back(strContent);

}

void SplitString(char *strSource, char split, vector<string>& vecContent)
{
	string strContent = strSource;
	string::size_type  iPos = strContent.find(split);
	string strTemp;
	while (iPos != wstring::npos)
	{
		strTemp = strContent.substr(0, iPos);
		strContent = strContent.substr(++iPos, wstring::npos);
		vecContent.push_back(strTemp);
		iPos = strContent.find(split);
	}
	vecContent.push_back(strContent);

}
