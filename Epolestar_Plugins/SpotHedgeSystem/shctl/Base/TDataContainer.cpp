#include "BaseInclude.h"

void GetFilePath(TCHAR* filePath, int length, TCHAR* fileName)
{
	GetModuleFileName(0, filePath, length);
	WCHAR* cp_pos = wcsrchr(filePath, '\\');
	if (0 != cp_pos)
		cp_pos[0] = 0;
	wcscat_s(filePath, length, fileName);
}
