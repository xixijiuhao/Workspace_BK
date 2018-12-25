#pragma once

#include <string>
#include <vector>

using namespace std;

template<typename str_type>
class istrings
{
public:
	virtual size_t size() = 0;
	virtual str_type at(size_t i) = 0;
	virtual void clear() = 0;
	virtual void push_back(const str_type & str) = 0;
};

template<typename str_type>
class xstrings : public istrings<str_type>
{
private:
	vector<str_type> m_strs;
public:	
	virtual size_t size();
	virtual str_type at(size_t i);
	virtual void clear();
	virtual void push_back(const str_type & str);
};

typedef istrings<string>	IStrings;
typedef xstrings<string>	XStrings;
typedef istrings<wstring>	IWStrings;
typedef xstrings<wstring>	XWStrings;

class TCustomIniFile
{
private:
	char m_FileName[MAX_PATH*4];
public:
	TCustomIniFile(const char * FileName);
	virtual bool SetFileName(const char * FileName);

	virtual string ReadString(const char * Section, const char * Ident, const char * Default=nullptr) = 0;
	virtual wstring ReadString(const wchar_t * Section, const wchar_t * Ident, const wchar_t * Default=nullptr) = 0;
	virtual void WriteString(const char *  Section, const char * Ident, const char * Value) = 0;
	virtual void WriteString(const wchar_t * Section, const wchar_t * Ident, const wchar_t * Value) = 0;

	virtual int ReadInteger(const char *  Section, const char * Ident, int Default=0);
	virtual void WriteInteger(const char *  Section, const char * Ident, int Value);

	virtual bool ReadBool(const char *  Section, const char * Ident, bool Default=false);
	virtual void WriteBool(const char *  Section, const char * Ident, bool Value);

	virtual double ReadFloat(const char *  Section, const char * Name, double Default=0);
	virtual void WriteFloat(const char *  Section, const char * Name, double Value);

	virtual time_t ReadDate(const char *  Section, const char * Name, time_t Default=0);
	virtual void WriteDate(const char *  Section, const char * Name, time_t Value);

	virtual time_t ReadTime(const char *  Section, const char * Name, time_t Default=0);
	virtual void WriteTime(const char *  Section, const char * Name, time_t Value);

	virtual time_t ReadDateTime(const char *  Section, const char * Name, time_t Default=0);
	virtual void WriteDateTime(const char *  Section, const char * Name, time_t Value);

	virtual bool ReadBinaryStream(const char *  Section, const char * Name, void * & Buffer, int & BufSize);//返回buffer的长度BufSize
	virtual int  ReadBinaryData(const char *  Section, const char * Name, void * Buffer, const int BufSize);//返回实际读取的长度
	virtual void WriteBinaryStream(const char *  Section, const char * Name, void * Buffer, int BufSize);

	virtual void ReadSections(IStrings & Sections)=0; 
	//返回以Section为前缀的节
	virtual void ReadSections(const char *  Section, IStrings & Sections);
	virtual void ReadSection(const char *  Section, IStrings & Idents)=0; 
	virtual void ReadSectionValues(const char *  Section, IStrings & IdentValues)=0; 
	
	virtual void ReadSections(IWStrings & Sections)=0; 
	//返回以Section为前缀的节
	virtual void ReadSections(const wchar_t *  Section, IWStrings & Sections);
	virtual void ReadSection(const wchar_t *  Section, IWStrings & Idents)=0; 
	virtual void ReadSectionValues(const wchar_t *  Section, IWStrings & IdentValues)=0; 

	bool SectionExists(const char * Section);
	virtual bool ValueExists(const char *  Section, const char * Ident);

	virtual void EraseSection(const char *  Section){}; 
	virtual void DeleteKey(const char *  Section, const char * Ident){}; 

	virtual void UpdateFile(){}; 
	
	char * FileName(); 
};

class XIniFile : public TCustomIniFile
{
public:
	XIniFile(const char * FileName);
	virtual string ReadString(const char * Section, const char * Ident, const char * Default=nullptr);
	virtual wstring ReadString(const wchar_t * Section, const wchar_t * Ident, const wchar_t * Default=nullptr);
	virtual void WriteString(const char * Section, const char * Ident, const char * Value);	
	virtual void WriteString(const wchar_t * Section, const wchar_t * Ident, const wchar_t * Value);
	
	virtual void ReadSections(IStrings & Sections); 
	virtual void ReadSection(const char *  Section, IStrings & Idents); 
	virtual void ReadSectionValues(const char *  Section, IStrings & IdentValues); 

	virtual void ReadSections(IWStrings & Sections); 
	virtual void ReadSection(const wchar_t *  Section, IWStrings & Idents); 
	virtual void ReadSectionValues(const wchar_t *  Section, IWStrings & IdentValues); 

	virtual void EraseSection(const char *  Section);
	virtual void DeleteKey(const char *  Section, const char * Ident);

	virtual void UpdateFile();
};