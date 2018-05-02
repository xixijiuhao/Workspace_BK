#include "BaseInclude.h"


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

