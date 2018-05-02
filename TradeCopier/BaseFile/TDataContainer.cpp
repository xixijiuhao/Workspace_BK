#include "PreInclude.h"


void GetModulePath(char *filePath, int length, char * fileName)
{
	GetModuleFileNameA(0, filePath, length);
	char* cp_pos = strrchr(filePath, '\\');
	if (0 != cp_pos)
		cp_pos[0] = '\0';
	strcat_s(filePath, length, fileName);
}