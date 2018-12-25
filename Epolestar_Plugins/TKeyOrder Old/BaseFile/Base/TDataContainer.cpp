#include "BaseInclude.h"

void GetFilePath(TCHAR* filePath, int length, TCHAR* fileName)
{
	GetModuleFileName(0, filePath, length);
	WCHAR* cp_pos = wcsrchr(filePath, '\\');
	if (0 != cp_pos)
		cp_pos[0] = 0;
	wcscat_s(filePath, length, fileName);
}
void SplitString(TCHAR *strSource, TCHAR split, vector<wstring>& vecContent)
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


LRESULT OnCtrlColor(WPARAM wParam, LPARAM lParam)
{

	SetBkMode((HDC)wParam, TRANSPARENT);
	return (LRESULT)g_ColorRefData.g_brush_bk;
}