#include "global.h"
#include "XIniFile.h"
#include "CharCodes.h"


/***************************************** XStrings *******************************************/
size_t XStrings::size()
{
	return m_strs.size();
}
string XStrings::at(size_t i)
{
	if (i < 0 || i >= m_strs.size())
		return "";
	return m_strs[i];
}
void XStrings::clear()
{
	m_strs.clear();
}
void XStrings::push_back(const string & str)
{
	m_strs.push_back(str);
}
/***************************************** XWStrings *******************************************/
size_t XWStrings::size()
{
	return m_strs.size();
}
wstring XWStrings::at(size_t i)
{
	if (i < 0 || i >= m_strs.size())
		return L"";
	return m_strs[i];
}
void XWStrings::clear()
{
	m_strs.clear();
}
void XWStrings::push_back(const wstring & str)
{
	m_strs.push_back(str);
}

/***************************************** TCustomIniFile *******************************************/
TCustomIniFile::TCustomIniFile(const char * FileName)
{
	strncpy_s(m_FileName, FileName);
}

bool TCustomIniFile::SetFileName(const char * FileName)
{
	strncpy_s(m_FileName, FileName);
	return true;
}

int TCustomIniFile::ReadInteger(const char * Section, const char * Ident, int Default)
{
	string IntStr = ReadString(Section, Ident, "");

	try
	{
		if ((IntStr.length() > 2) && 
			(IntStr[0] == '0') && 
			((IntStr[1] == 'X') || (IntStr[1] == 'x'))
			)
			return MathFun::HexToDec((char *)IntStr.c_str()); 
		else if (IntStr.length() > 0)
			return atoi((char *)IntStr.c_str());
		else
			return Default;
	}
	catch (int)
	{
		return Default;
	}
}

void TCustomIniFile::WriteInteger(const char * Section, const char * Ident, int Value)
{
	char IntStr[20];
	_itoa_s(Value, IntStr, 10);
	WriteString(Section, Ident, IntStr);
}

bool TCustomIniFile::ReadBool(const char * Section, const char * Ident, bool Default)
{
	return ReadInteger(Section, Ident, Default) != 0;
}

void TCustomIniFile::WriteBool(const char * Section, const char * Ident, bool Value)
{
	WriteInteger(Section, Ident, int(Value));
}

double TCustomIniFile::ReadFloat(const char * Section, const char * Name, double Default)
{
	string fStr = ReadString(Section, Name, "");
	try
	{
		if (fStr.length()>0)
			return atof((char *)fStr.c_str());
		else
			return Default;
	}
	catch (double)
	{
		return Default;
	}
}

void TCustomIniFile::WriteFloat(const char * Section, const char * Name, double Value)
{
	char fStr[50];
	_snprintf_s(fStr, "%f", Value);
	WriteString(Section, Name, fStr);
}

time_t TCustomIniFile::ReadDate(const char *  Section, const char * Name, time_t Default)
{
	string DateStr = ReadString(Section, Name, "");
	tm Date;
	
	if (DateStr.length()>0 && TimeFun::StrToTime((char *)DateStr.c_str(), "%Y-%m-%d", Date))
	{
		Date.tm_hour=0;
		Date.tm_min=0;
		Date.tm_sec=0;
		return mktime(&Date);
	}
	else
		return Default;
}

void TCustomIniFile::WriteDate(const char * Section, const char * Name, time_t Value)
{
	tm Time; 
	localtime_s(&Time, &Value);
	char Buf[50];
	int BufSize=strftime(&Buf[0], sizeof(Buf), "%Y-%m-%d", &Time);
	Buf[BufSize]=0;

	WriteString(Section, Name, &Buf[0]);
}

time_t TCustomIniFile::ReadTime(const char * Section, const char * Name, time_t Default)
{
	string TimeStr = ReadString(Section, Name, "");
	tm tim;
	if (TimeStr.length()>0 && TimeFun::StrToTime((char *)TimeStr.c_str(), "%H:%M:%S", tim))
	{
		time_t now=time(nullptr);
		tm tt;
		localtime_s(&tt, &now);

		tt.tm_hour=tim.tm_hour;
		tt.tm_min=tim.tm_min;
		tt.tm_sec=tim.tm_sec;

		return mktime(&tt);
	}
	else
		return Default;
}

void TCustomIniFile::WriteTime(const char * Section, const char * Name, time_t Value)
{
	tm Time;
	localtime_s(&Time, &Value);
	char Buf[50];
	int BufSize=strftime(&Buf[0], sizeof(Buf), "%H:%M:%S", &Time);
	Buf[BufSize]=0;

	WriteString(Section, Name, &Buf[0]);
}

time_t TCustomIniFile::ReadDateTime(const char *  Section, const char * Name, time_t Default)
{
	string DateTimeStr = ReadString(Section, Name, "");
	tm DateTime;
	if (DateTimeStr.length()>0 && TimeFun::StrToTime((char *)DateTimeStr.c_str(), "%Y-%m-%d %H:%M:%S", DateTime))
		return mktime(&DateTime);
	else
		return Default;
}

void TCustomIniFile::WriteDateTime(const char * Section, const char * Name, time_t Value)
{
	tm Time;
	localtime_s(&Time, &Value);
	char Buf[50];
	int BufSize=strftime(&Buf[0], sizeof(Buf), "%Y-%m-%d %H:%M:%S", &Time);
	Buf[BufSize]=0;

	WriteString(Section, Name, &Buf[0]);
}

bool TCustomIniFile::ReadBinaryStream(const char *  Section, const char * Name, void * & Buffer, int & BufSize)
{
	BufSize=0;
	string BufStr = ReadString(Section, Name, "");
	if (BufStr.length() > 0)
	{
		void * pBuf = realloc(Buffer, BufStr.length() / 2 + 1);
		BufSize = MathFun::HexToBin((char *)BufStr.c_str(), (char *)pBuf, BufStr.length() / 2);
		Buffer = realloc(pBuf, BufSize+1);
		((char *)Buffer)[BufSize]=0;
		return true;
	}
	else
		return false;
}
int TCustomIniFile::ReadBinaryData(const char *  Section, const char * Name, void * Buffer, const int BufSize)
{
	if (BufSize<=0)
		return 0;

	void * pMem = malloc(BufSize);

	int mBufSize;
	bool Result = ReadBinaryStream(Section, Name, pMem, mBufSize);
	if (Result)
	{
		mBufSize = min(mBufSize, BufSize);			//数据长度自适应
		memcpy_s(Buffer, mBufSize, pMem, mBufSize);
	}
	else
		mBufSize = 0;

	free(pMem);
	return mBufSize;
}

void TCustomIniFile::WriteBinaryStream(const char *  Section, const char * Name, void * Buffer, int BufSize)
{
	char * pText = (char *)malloc(BufSize * 2+1);
	pText[BufSize * 2]=0;

	MathFun::BinToHex((char *)Buffer, pText, BufSize);
	WriteString(Section, Name, pText);

	free(pText);
}

void TCustomIniFile::ReadSections(const char * Section, IStrings & Sections)
{
	XStrings strings;
	ReadSections(strings);

	for (size_t i = 0; i < strings.size(); i++)
		if (strstr(strings.at(i).c_str(), Section)==strings.at(i).c_str())
			Sections.push_back(strings.at(i));
}

void TCustomIniFile::ReadSections(const wchar_t *  Section, IWStrings & Sections)
{
	XWStrings strings;
	ReadSections(strings);

	for (size_t i = 0; i < strings.size(); i++)
		if (wcsstr(strings.at(i).c_str(), Section)==strings.at(i).c_str())
			Sections.push_back(strings.at(i));
}

bool TCustomIniFile::SectionExists(const char * Section)
{
	XStrings strings;
	ReadSection(Section, strings);
	return strings.size() > 0;
}

bool TCustomIniFile::ValueExists(const char * Section, const char * Ident)
{
	return ReadString(Section, Ident, "0")==ReadString(Section, Ident, "1");
}

char * TCustomIniFile::FileName()
{
	return m_FileName;
}
/************************************** XIniFile ***************************************/
XIniFile::XIniFile(const char * FileName)
: TCustomIniFile(FileName)
{
}
string XIniFile::ReadString(const char * Section, const char * Ident, const char * Default)
{
	char Buffer[4096];
	int Count = GetPrivateProfileStringA(Section, Ident, Default, Buffer, sizeof(Buffer), FileName());
	string str=(char *)&Buffer;
	return str;
}
wstring XIniFile::ReadString(const wchar_t * Section, const wchar_t * Ident, const wchar_t * Default)
{
	wchar_t Buffer[4096];
	int Count = GetPrivateProfileStringW(Section, Ident, Default, Buffer, sizeof(Buffer) / sizeof(Buffer[0]), CCodes::ANSIToUnicode(FileName()).c_str());
	wstring str = (wchar_t *)&Buffer;
	return str;
}

void XIniFile::WriteString(const char * Section, const char * Ident, const char * Value)
{
	if (!WritePrivateProfileStringA(Section, Ident, Value, FileName()))
		_ASSERT(0);
}

void XIniFile::WriteString(const wchar_t * Section, const wchar_t * Ident, const wchar_t * Value)
{
	if (!WritePrivateProfileStringW(Section, Ident, Value, CCodes::ANSIToUnicode(FileName()).c_str()))
		_ASSERT(0);
}

void XIniFile::ReadSections(IStrings & Sections)
{
	char buf[1024 * 4];
	if (GetPrivateProfileStringA(nullptr, nullptr, nullptr, buf, sizeof(buf), FileName()) <= 0)
		return;

	char * p = buf;
	while (*p != 0)
	{
		Sections.push_back(p);
		p += strlen(p) + 1;
	}
}

void XIniFile::ReadSection(const char *  Section, IStrings & Idents)
{	
	char buf[1024 * 4];
	if (GetPrivateProfileStringA(Section, nullptr, nullptr, buf, sizeof(buf), FileName()) <= 0)
		return;

	char * p = buf;
	while (*p != 0)
	{		
		Idents.push_back(p);
		p += strlen(p) + 1;
	}
}

void XIniFile::ReadSectionValues(const char *  Section, IStrings & IdentValues)
{
	XStrings Idents;
	ReadSection(Section, Idents);

	char buf[1024 * 4];
	for (size_t i = 0; i < Idents.size(); i++)
	{
		_snprintf_s(buf, sizeof(buf), "%s=%s", Idents.at(i).c_str(), ReadString(Section, Idents.at(i).c_str(), "").c_str());
		IdentValues.push_back(buf);
	}
}

void XIniFile::ReadSections(IWStrings & Sections)
{
	wchar_t buf[1024 * 4];
	if (GetPrivateProfileStringW(nullptr, nullptr, nullptr, buf, sizeof(buf) / sizeof(buf[0]), CCodes::ANSIToUnicode(FileName()).c_str()) <= 0)
		return;

	wchar_t * p = buf;
	while (*p != 0)
	{
		Sections.push_back(p);
		p += wcslen(p) + 1;
	}
}

void XIniFile::ReadSection(const wchar_t *  Section, IWStrings & Idents)
{	
	wchar_t buf[1024 * 4];
	if (GetPrivateProfileStringW(Section, nullptr, nullptr, buf, sizeof(buf) / sizeof(buf[0]), CCodes::ANSIToUnicode(FileName()).c_str()) <= 0)
		return;

	wchar_t * p = buf;
	while (*p != 0)
	{		
		Idents.push_back(p);
		p += wcslen(p) + 1;
	}
}

void XIniFile::ReadSectionValues(const wchar_t *  Section, IWStrings & IdentValues)
{
	XWStrings Idents;
	ReadSection(Section, Idents);

	wchar_t buf[1024 * 4];
	for (size_t i = 0; i < Idents.size(); i++)
	{
		_snwprintf_s(buf, sizeof(buf) / sizeof(buf[0]), L"%s=%s", Idents.at(i).c_str(), ReadString(Section, Idents.at(i).c_str(), L"").c_str());
		IdentValues.push_back(buf);
	}
}

void XIniFile::EraseSection(const char *  Section)
{
	if (! WritePrivateProfileStringA(Section, nullptr, nullptr, FileName()))
		_ASSERT(0);
}

void XIniFile::DeleteKey(const char *  Section, const char * Ident)
{
	WritePrivateProfileStringA(Section, Ident, nullptr, FileName());
}

void XIniFile::UpdateFile()
{
	WritePrivateProfileStringA(nullptr, nullptr, nullptr, FileName());
}