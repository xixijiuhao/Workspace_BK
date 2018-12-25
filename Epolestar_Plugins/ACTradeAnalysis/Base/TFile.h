#ifndef _T_READ_FILE_
#define _T_READ_FILE_


class TFile
{
public:
	TFile();
	~TFile();
	void GetFilePath(TCHAR* filePath, int length, TCHAR* fileName);
	bool Open(string filePath, string mode);
	bool ReadOneLine(string &str);
	bool WriteOneLine(string str);
	FILE * GetFile(){ return m_file; }
	UINT GetFileType(string filePath);
private:
	
private:
	FILE * m_file;
	UINT   m_fileFormat;  //文件编码格式
	enum{ftANSI,ftUTF8,ftUTF16B,ftUTF16L,ftUTF32B,ftUTF32L,ftError,ftOpenError};
};

#endif