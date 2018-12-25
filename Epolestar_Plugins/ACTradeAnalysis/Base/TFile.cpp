#include "BaseInclude.h"

TFile::TFile()
{

}
TFile::~TFile()
{
	if (m_file)
		fclose(m_file);
}
bool TFile::WriteOneLine(string str)
{
	if (feof(m_file))
		return false;
	int pRet = -1;
	switch (m_fileFormat)
	{
	case ftANSI:
		pRet = fputs(str.c_str(), m_file);
		break;
	case ftUTF16L:
		pRet = fputws(LoadRC::ansi_to_unicode(str.c_str()).c_str(), m_file);
		break;
	default:
		return false;
	}
	if (pRet != -1)
		return true;
	return false;
}
bool TFile::ReadOneLine(string &str)
{
	if (feof(m_file))
		return false;
	void * pRet = NULL;
	switch (m_fileFormat)
	{
	case ftANSI:
	{
		char szLine[256] = { 0 };
		pRet = fgets(szLine, sizeof(szLine), m_file);
		str.assign(szLine);
	}
	break;
	case ftUTF16L:
	{
		wchar_t szLine[256] = { 0 };
		pRet = fgetws(szLine, sizeof(szLine) / sizeof(wchar_t), m_file);
		str.assign(LoadRC::unicode_to_ansi(szLine).c_str());
	}
	break;
	default:
		return false;
	}
	if (pRet)
		return true;
	return false;
}

void TFile::GetFilePath(TCHAR* filePath, int length, TCHAR* fileName)
{
	GetModuleFileName(0, filePath, length);
	WCHAR* cp_pos = wcsrchr(filePath, '\\');
	if (0 != cp_pos)
		cp_pos[0] = 0;
	wcscat_s(filePath, length, fileName);
}
bool TFile::Open(string filePath, string mode)
{
	m_fileFormat = GetFileType(filePath);
	switch (m_fileFormat)
	{
	case ftANSI:
		fopen_s(&m_file, filePath.c_str(), mode.c_str());
		break;
	case ftUTF16L:
	{
		char szMode[50] = { 0 };
		sprintf_s(szMode, "%s,ccs=Unicode", mode.c_str());
		_wfopen_s(&m_file, LoadRC::ansi_to_unicode(filePath).c_str(), LoadRC::ansi_to_unicode(szMode).c_str());
		if (m_file)
			fseek(m_file, 2, SEEK_SET);
	}
		break;
	}
	if (!m_file)
		return false;
	return true;
}

UINT TFile::GetFileType(string filePath)
{
	FILE *file = NULL;
	fopen_s(&file, filePath.c_str(), "r");
	UINT fileType = ftOpenError;
	if (!file)
		return fileType;
	unsigned char buf[10] = { 0 };
	size_t  rdCount = fread(buf, 1, 4, file);
	if (rdCount < 2) //读取到的内容小于2，说明是ANSI
	{
		fileType = ftANSI;
	}
	else
	{
		if (rdCount == 4 && buf[0] == 0 && buf[1] == 0 && buf[2] == 0xFE && buf[3] == 0xFF)
		{
			fileType = ftUTF32B;
		}
		else if (rdCount == 4 && buf[0] == 0xFF && buf[1] == 0xFE && buf[2] == 0 && buf[3] == 0)
		{
			fileType = ftUTF32L;
		}
		else if (buf[0] == 0xFE && buf[1]==0xFF)
		{
			fileType = ftUTF16B;
		}
		else if (buf[0] == 0xFF && buf[1] == 0xFE)
		{
			fileType = ftUTF16L;
		}
		else if (buf[0] == 0xEE && buf[1]== 0xBB && buf[2] == 0xBF)
		{
			fileType = ftUTF8;
		}
		else
			fileType = ftANSI;
	}
	fclose(file);
	return fileType;
}