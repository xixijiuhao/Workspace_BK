/*
文件结构：
TFileHead		//文件头
PlusHead		//附加文件头起始位置，长度为TFileHead.PlusHeadSize
Fields[0]		//第一个字段定义
Fields[1]		//第二个字段定义
Fields[2]		//第三个字段定义
...				//...
Fields[n]		//第n个字段定义（n=TFileHead.FieldCount）
Data			//数据区起始位置，数据区每个记录前面是一个WORD型的记录校验码，紧接着才是该条记录的数据
*/

#pragma once

#include <string>
#include <vactor>
#include "Global.h"

using namespace std;

#define RecFileVerstion	1;

enum TFieldType
{
	ftVarChar,
	ftByte,
	ftInt,
	ftBool,
	ftDate,
	ftTime,
	ftDateTime,
	ftDouble
};

struct TField
{
	char		Name[16];		//字段名
	char		Title[20];		//字段标题
	TFieldType	Type;			//字段类型
	UINT		Size;			//字段长度
	byte		IndexFlag;		//是否索引字段
	byte		Precision;		//浮点数精度
}
struct TFileHead
{
	byte		Verstion;		//文件版本
	time_t		CreateTime;		//创建时间
	time_t		UpdateTime;		//更新时间
	UINT		FileSize;		//文件大小
	UINT		PlusHeadSize;	//附加文件头长度
	WORD		FieldCount;		//字段数
public:
	int PlusHead()
	{
		return sizeof(TFileHead);
	}
	int FieldHead()
	{
		return PlusHead() + PlusHeadSize;
	}
	int DataHead()
	{
		return FieldHead() + sizeof(TField) * FieldCount;
	}
};
struct TFieldEx
{
	TField *	pField;
	void *      pData;
public:
	char toChar(char def)
	{
		try
		{
			switch(pField->Type)
			{
			case ftBool		: 
				if (*(bool*)pData)
					return 'T';
				else
					return 'F';
			case ftDouble	:
			case ftInt		:
			case ftDate		:
			case ftTime		:
			case ftDateTime	:
			case ftByte		:	
			case ftVarChar	:	
				return *(char*)pData;
			}
		}
		catch(...)
		{
			return def;
		}
		return def;
	}
	string toString(char * def)
	{
		try
		{
			switch(pField->Type)
			{
			case ftVarChar:
				{
					char * Buf = (char*)malloc(pField->Size);
					memcpy(Buf, pData, pField->Size);
					return Buf;
				}
			case ftDate	:
				{
					time_t t = *(time_t*)pData;
					tm * lt = localtime(&t);
					char Buf[11];
					strftime(Buf, sizeof(Buf), "%Y-%m-%d", lt);
					return (char *)&Buf;
				}
			case ftTime:
				{
					time_t t = *(time_t*)pData;
					tm * lt = localtime(&t);
					char Buf[9];
					strftime(Buf, sizeof(Buf), "%H:%M:%S", lt);
					return (char *)&Buf;
				}
			case ftDateTime:
				{
					time_t t = *(time_t*)pData;
					tm * lt = localtime(&t);
					char Buf[20];
					strftime(Buf, sizeof(Buf), "%Y-%m-%d %H:%M:%S", lt);
					return (char *)&Buf;
				}
			case ftByte:
				{
					byte b = *(byte*)pData;

					char Buf[4];
					itoa(b, Buf, 10);
					return (char *)&Buf;
				}
			case ftInt:
				{
					int n = *(int*)pData;

					char Buf[21];
					itoa(n, Buf, 10);
					return (char *)&Buf;
				}
			case ftDouble:
				{
					double d = *(double*)pData;
					char Buf[350];
					int len = sprintf_s(Buf, "%.*f", pField->Precision, d);
					return (char *)&Buf;
				}
			case ftBool:
				{
					if (*(bool*)pData)
						return "True";
					else
						return "False";
				}
			case default:
				return def;
			}
		}
		catch(...)
		{
			return def;
		}
		return def;
	}
	double toDouble(double def)
	{
		try
		{
			switch(pField->Type)
			{
			case ftVarChar:
			case ftByte:
				return (double)*(byte*)pData;
			case ftDate	:
			case ftTime:
			case ftDateTime:
				return (double)*(time_t*)pData;
			case ftInt:
				return (double)*(int*)pData;
			case ftDouble:
				return *(double*)pData;
			case ftBool:
				return (double)*(bool*)pData;
			case default:
				return def;
			}
		}
		catch(...)
		{
			return def;
		}
		return def;
	}
	int toInt(int def)
	{
		try
		{
			switch(pField->Type)
			{
			case ftVarChar:
			case ftByte:
				return (int)*(byte*)pData;
			case ftDate	:
			case ftTime:
			case ftDateTime:
				return (int)*(time_t*)pData;
			case ftInt:
				return *(int*)pData;
			case ftDouble:
				return (int)*(double*)pData;
			case ftBool:
				return (int)*(bool*)pData;
			case default:
				return def;
			}
		}
		catch(...)
		{
			return def;
		}
		return def;

	}
	bool toBool(int def)
	{
		try
		{
			switch(pField->Type)
			{
			case ftVarChar:
			case ftByte:
				return (bool)*(byte*)pData;
			case ftDate	:
			case ftTime:
			case ftDateTime:
				return (bool)*(time_t*)pData;
			case ftInt:
				return (bool)*(int*)pData;
			case ftDouble:
				return (bool)*(double*)pData;
			case ftBool:
				return *(bool*)pData;
			case default:
				return def;
			}
		}
		catch(...)
		{
			return def;
		}
		return def;

	}
	byte toByte(byte def)
	{
		try
		{
			switch(pField->Type)
			{
			case ftBool:
				if (*(bool*)pData)
					return 1;
				else
					return 0;
			case default:
				return *(byte*)pData;
			}
		}
		catch(...)
		{
			return def;
		}
		return def;

	}
	time_t toDateTime(time_t def)
	{
		try
		{
			switch(pField->Type)
			{
			case ftVarChar:
				{
					char * Buf = (char*)malloc(pField->Size);
					memcpy(Buf, pData, pField->Size);
					time_t t;
					if (CDateTime::StrToTime(Buf, "%Y-%m-%d %H:%M:%S", t)
						return t;
					else
						return def;
				}
			case ftByte:
				return (time_t)*(byte*)pData;
			case ftDate	:
			case ftTime:
			case ftDateTime:
				return *(time_t*)pData;
			case ftInt:
				return (time_t)*(int*)pData;
			case ftDouble:
				return (time_t)*(double*)pData;
			case ftBool:
				return (time_t)*(bool*)pData;
			case default:
				return def;
			}
		}
		catch(...)
		{
			return def;
		}
		return def;
	}
};
class TRecord
{
private:
	vector<TField>	m_Fields;
	vector<UINT>	m_FieldOffsets;		//字段在记录内的偏移
	void *			m_pRecData;
public:
	TRecord():m_pRecData(NULL)
	{

	}
	~TRecord()
	{

	}

public:
	
};
/******************************************* CRecFile ******************************************/
template <class _Record>
class CRecFile
{
private:
	CFile		m_File;
	TFileHead 	m_Head;				//文件头
	TField		m_Fields[1];			//字段信息
	UINT		m_DataHead;			//数据起始位置，因为用频率的较高，所以以变量形式保存下来
	UINT		m_RecordSize;		//单记录长度
	void *		m_pCurrRecord;		//当前记录上的数据
	TRecord		m_Record;
public:
	CRecFile()
	{
		memset(m_Head, 0, sizeof(TFileHead));
	}
	~CRecFile(): m_DataHead(0), m_RecordSize(0), m_pCurrRecord(NULL)
	{
		m_Head->UpdateTime	= time(NULL);
		m_Head.FileSize		= m_File.GetLength();
		m_File.SeekToBegin();
		m_File.Write(&m_Head, sizeof(TFileHead));

		m_File.Close();
		if (m_pCurrRecord)
			free(m_pCurrRecord);
	}
private:
	WORD GetCheckCode(void * pRecord)
	{
		return 1;
	}
	//创建新文件
	void CreateFileHead(void * pPulsHead, UINT nPulsHeadSize, TField[] Fields, UINT nFieldCount)
	{
		m_Head.Verstion		= RecFileVerstion;
		m_Head.CreateTime	= time(NULL);
		m_Head.UpdateTime	= time(NULL);					//更新时间
		m_Head.PlusHeadSize	= nPulsHeadSize;				//附加文件头长度
		m_Head.FieldCount	= nFieldCount;					//字段数

		m_File.SeekToBegin();
		m_File.Write(&m_Head, sizeof(TFileHead));			//写入文件头
		m_File.Write(pPulsHead, nPulsHeadSize);				//写入附加头
		m_File.Write(&Fields, nFieldCount * sizeof(TField));//写入字段定义

		UpdateFields();										//更新字段信息

		m_File.SetLength(m_DataHead);						//清空数据
	}
	//读取附加文件头
	int GetPulsHead(void * pPlusHead)
	{
		ULONGLONG Pos=m_File.GetPosition();

		m_File.Seek(m_Head.PlusHead(), CFile::begin);
		UINT leng = m_File.Read(pPlusHead, m_Head->PlusHeadSize);

		m_File.Seek(Pos, CFile::begin);

		return leng;
	}
	void UpdateFields()
	{
		//更新字段信息
		m_File.Seek(m_Head.FieldHead(), CFile::begin);
		m_File.Read(&m_Fields, m_Head.FieldCount * sizeof(TField));

		//更新字段偏移位置
		int Offset = 0;
		for (int i = 0; i < m_Head.FieldCount; i++)
		{
			m_FieldOffsets[i] = Offset + m_Fields[i].Size;
			Offset = m_FieldOffsets[i];
		}
		
		m_RecordSize = Offset + m_Fields[m_Head.FieldCount - 1].Size;		//更新记录长度
		m_DataHead	 = m_Head.DataHead();									//更新数据起始位置
	}
protected:
	//附加文件头校验
	virtual int CheckPulsHead(void * pPulsHead)=1;														
public:
	int Open(char* FileName, void * pPulsHead, UINT nPulsHeadSize, TField[] Fields, UINT nFieldCount, bool ForceInit=false)//ForceInit是否强制初始化为新文件
	{
		if (! m_File.Open(FileName, CFile::modeReadWrite | CFile::shareDenyWrite))	
			if (! m_File.Open(FileName, CFile::modeCreate | CFile::modeReadWrite | CFile::shareDenyWrite))
				return -1;

		if (ForceInit || 0 == m_File.GetLength())
			CreateFileHead(pPulsHead, nPulsHeadSize, Fields, nFieldCount);	
		else
		{
			m_File.Read(m_Head, sizeof(TFileHead));

			void * pPlus = malloc(m_Head.PlusHeadSize);
			GetPulsHead(pPlus);

			int Res = CheckPulsHead(pPlus);	//附加头校验
			if (1 == Res || 
				m_Head.FileSize != m_File.GetLength() || (m_File.GetLength() < m_Head.DataHead()) || 0 != (m_File.GetLength() - m_Head.DataHead()) % m_RecordSize ||
				m_Head.FieldCount != nFieldCount || m_Head.PlusHeadSize != nPulsHeadSize || m_RecordSize != sizeof(_Record)|| m_Head.Verstion != RecFileVerstion)
				Res = -2;
			if (1 != Res)
			{
				m_File.Close();	
				return Res;
			}

			UpdateFields();
		}
		m_pCurrRecord = malloc(m_Head.RecordSize);
		return 1;
	}
	//增加字段（简化版，如果增加字段的时候已经有记录数据则会增加失败）
	bool AddField(TField Field)
	{
		if (m_DataHead - m_File.GetLength() >0)
			return false;//这时其实应该将数据下移一个字段结构的长度，然后再修改每一笔记录的结构，以后有时间再实现

		m_File.Seek(m_DataHead, CFile::begin);
		m_File.Write(&Field, sizeof(TField));
		m_Head.FieldCount++;
		UpdateFields();

		return true;
	}
	int RecordCount()
	{
		return int((m_File.GetLength() - m_DataHead) / m_RecordSize);
	}
	//在当前位置写入一条记录
	void Write(_Record * pRecord)
	{		
		WORD CheckCode= GetCheckCode(pRecord);
		m_File.Write(&CheckCode, sizeof(WORD));							//写入校验码
		m_File.Write(pRecord, m_RecordSize);							//写入数据
	}
	//在当前位置读取
	bool Read(_Record * pRecord)
	{
		WORD CheckCode;
		m_File.Read(&CheckCode, sizeof(WORD));
		UINT leng = m_File.Read(pRecord, m_RecordSize);

		return (leng == m_RecordSize && CheckCode == GetCheckCode(pRecord));
	}
	//增加
	void AddRecord(_Record * pRecord)
	{
		m_File.SeekToEnd();
		Write(pRecord);	
	}
	//按记录号写入
	bool UpdateRecord(int Index, _Record * pRecord)
	{
		if (Index >= RecordCount() || Index < 0)	
			return false;

		m_File.Seek(m_DataHead + Index * m_RecordSize, CFile::begin);
		Write(pRecord);

		return true;
	}
	//按记录号读取
	bool GetRecord(int Index, _Record * pRecord)
	{
		if(Index >= RecordCount() || Index < 0)	
			return false;

		m_File.Seek(m_DataHead + Index * m_RecordSize, CFile::begin);
		return Read(pRecord);
	}
	//清空数据
	void ClearRecord()
	{
		m_Head->UpdateTime = time(NULL);							//更新时间

		m_File.SetLength(m_DataHead);
		m_File.SeekToBegin();
		m_File.Write(m_Head, sizeof(TFileHead));
		m_File.SeekToEnd();
	}
	//设置当前行号
	void SetRowIndex(int RowIndex)
	{
		if(RowIndex >= RecordCount() || RowIndex < 0)
			return;
		m_File.Seek(m_DataHead + RowIndex * m_RecordSize);
	}
	//获得当前行号
	int GetRowIndex()
	{
		return int((m_File.GetPosition() - m_DataHead) / m_RecordSize);
	}
	//返回一个字段
	void GetRecord(int Index, TRecord * pRecord)					//将字段数据转为字符串输出
	{
		if (Index >= m_Head.FieldCount || Index < 0)	
			return;

		m_File.Read(m_pCurrRecord, m_RecordSize);					//更新当前记录缓冲
		pRecord->pData = m_pCurrRecord;

		pFieldEx->pField	= &m_Fields[Index];
		pFieldEx->pData		= (char*)m_pCurrRecord + m_FieldOffsets[Index];
	}
};
/********************************************* CIndexFile *****************************************/
class CIndexFile:public CRecFile
{
private:
	map<std::string, int> m_Indexs;

	//系统是否为BigEndian
	bool IsBigEndian(void)
	{
		union
		{
			unsigned long k;
			unsigned char c[4];
		} u = {0xFF000000};

		return (0xFF == u.c[0]);	//高位优先体系结构在最低的地址上是最高有效位
	}
	//将索引字段转为记录关键字（将索引字段的十六进制连到一块）
	void GetKey(const char * pRecord, char * Key)
	{
		int KeyLeng=0;																				//索引长度
		int k;
		char * P=NULL;

		for (int i=0; i<m_pHead->FieldCount; i++)
		{
			if (!m_pHead->Fields[i].IndexFlag)														//索引字段
				continue;

			P = pRecord+m_pHead->Fields[i].Offset;

			switch (m_pHead->Fields[i].Type)
			{
			case ftChar:
				if (*P)
					Key[KeyLeng++] = *P;
				break;
			case ftString:
				for (k=0; (k<m_pHead->Fields[i].Leng && *P); k++)
					Key[KeyLeng++] = *P++;
				break;
			case ftInt:
			case ftDate:
				//与数据在内存中的高地位顺序有关
				if (IsBigEndian())	//高字节优先编码
				{
					char * Q = m_pHead->Fields[i].Leng-1;
					for (k=m_pHead->Fields[i].Leng-1; k>=0; k--)
						if (*Q--)						
							CMathEx::BinToHex(P, &(Key[KeyLeng]), k+1);

					KeyLeng += 2 * (k+1);
				}
				else				//低字节编码
				{
					for (k=0; k<m_pHead->Fields[i].Leng; k++)
						if (*P++)						
							CMathEx::BinToHex(--P, &(Key[KeyLeng]), m_pHead->Fields[i].Leng-k);

					KeyLeng += 2 * (m_pHead->Fields[i].Leng-k);
				}
			case ftDouble:
			default:
				CMathEx::BinToHex(P, &(Key[KeyLeng]), m_pHead->Fields[i].Leng);
				KeyLeng += 2 * m_pHead->Fields[i].Leng;
				break;
			}

			Key[KeyLeng++]='_';																	//索引字段间隔符号
		}//for

		Key[KeyLeng]=0;
	}
	//重建索引
	void MakeIndex()											
	{
		char Key[512];
		char Record[m_pHead->RecordLeng];
		map<std::string, int>::iterator iter;
		Seek(m_DataHead, begin);
		int Pos=GetPosition();
		for (int i=0; i<m_RecordCount; i++)
		{
			CRecFile::Read(&Record);
			GetKey(&Record, &Key);
			iter=m_Indexs.find(Key);																//查找记录
			if (iter==m_Indexs.end())							
				m_Indexs.insert(map<std::string, int>::value_type(Key, Pos));						//增加索引

			Pos=GetPosition();
		}
	}
public:
	//按索引保存记录
	void SaveRecord(void * pRecord)
	{
		int Pos=-1;
		char Key[256];
		GetKey((char *)pRecord, &Key);
		map<std::string, int>::iterator iter=m_Indexs.find(Key);									//查找记录
		if (iter!=m_Indexs.end())
			Pos=iter->second;  
																									//维护索引
		if (Pos==-1)
		{
			Pos=Seek(0, end);
			m_Indexs.insert(map<std::string, int>::value_type(Key, Pos));
		}

		Seek(Pos, begin);																			//定位保存位置
		CRecFile::Write(pRecord);
	}
};
/********************************************* CTradeFile *****************************************/
struct TPlusHead
{
	char		UserNo[16];		//文件保存者帐号
	char		ProcessName[50];//保存者进程名
	char		FileName[50];	//原始文件名
	char		SeverIP[16];	//服务器IP
	char		LocalIP[16];	//本地IP
	char		LocalMac[36];	//本地MAC
	WORD		Port;			//连接端口
	time_t		TradeDate;		//交易日期
	time_t		InitTime;		//初始化时间
};
class CTradeFile:public CCIndexFile
{
protected:
	int CheckPulsHead(void * pPulsHead)
	{
		TPlusHead mTradeHead, TradeHead;
		if (CRecFile::GetPulsHead(&mTradeHead)!=sizeof(TPlusHead))
			return 0;
		TradeHead=*(TPlusHead *)pPulsHead;
		if (strcmp(TradeHead.UserNo,mTradeHead.UserNo)!=0)
			return -1;
		if (strcmp(TradeHead.ProcessName,mTradeHead.ProcessName)!=0)
			return -2;
		if (strcmp(TradeHead.FileName,mTradeHead.FileName)!=0)
			return -3;
		if (strcmp(TradeHead.SeverIP,mTradeHead.SeverIP)!=0)
			return -4;
		if (strcmp(TradeHead.LocalIP,mTradeHead.LocalIP)!=0)
			return -5;
		if (strcmp(TradeHead.LocalMac,mTradeHead.LocalMac)!=0)
			return -6;
		if (TradeHead.Port!=mTradeHead.Port)
			return -7;
		if (TradeHead.TradeDate!=mTradeHead.TradeDate)
			return -8;
		if (TradeHead.InitTime!=mTradeHead.InitTime)
			return -9;

		return 1;
	}
public:
	int Open(char* filename, size_t RecordLeng, TField[] Fields, size_t FieldCount, TPlusHead TradeHead)
	{
		return CRecFile::Open(filename, CFile::typeBinary | CFile::shareDenyWrite | CFile::modeReadWrite | 
			CFile::modeCreate | CFile::modeNoTruncate, RecordLeng, Fields, FieldCount, TradeHead);
	}
};