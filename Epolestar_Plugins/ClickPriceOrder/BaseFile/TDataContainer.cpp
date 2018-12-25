#include "PreInclude.h"

#define  path "\\config\\ThreeKeyOrderSimplify.pri"

char TFilePath::m_szFilePath[200] = "";
const char * TFilePath::GetFilePath()
{
	if (!strlen(m_szFilePath))
	{
		CConvert::GetFilePath(m_szFilePath, sizeof(m_szFilePath), path);
	}
	return m_szFilePath;
}