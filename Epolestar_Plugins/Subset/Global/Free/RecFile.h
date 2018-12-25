/*
�ļ��ṹ��
TFileHead		//�ļ�ͷ
PlusHead		//�����ļ�ͷ��ʼλ�ã�����ΪTFileHead.PlusHeadSize
Fields[0]		//��һ���ֶζ���
Fields[1]		//�ڶ����ֶζ���
Fields[2]		//�������ֶζ���
...				//...
Fields[n]		//��n���ֶζ��壨n=TFileHead.FieldCount��
Data			//��������ʼλ�ã�������ÿ����¼ǰ����һ��WORD�͵ļ�¼У���룬�����Ų��Ǹ�����¼������
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
	char		Name[16];		//�ֶ���
	char		Title[20];		//�ֶα���
	TFieldType	Type;			//�ֶ�����
	UINT		Size;			//�ֶγ���
	byte		IndexFlag;		//�Ƿ������ֶ�
	byte		Precision;		//����������
}
struct TFileHead
{
	byte		Verstion;		//�ļ��汾
	time_t		CreateTime;		//����ʱ��
	time_t		UpdateTime;		//����ʱ��
	UINT		FileSize;		//�ļ���С
	UINT		PlusHeadSize;	//�����ļ�ͷ����
	WORD		FieldCount;		//�ֶ���
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
	vector<UINT>	m_FieldOffsets;		//�ֶ��ڼ�¼�ڵ�ƫ��
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
	TFileHead 	m_Head;				//�ļ�ͷ
	TField		m_Fields[1];			//�ֶ���Ϣ
	UINT		m_DataHead;			//������ʼλ�ã���Ϊ��Ƶ�ʵĽϸߣ������Ա�����ʽ��������
	UINT		m_RecordSize;		//����¼����
	void *		m_pCurrRecord;		//��ǰ��¼�ϵ�����
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
	//�������ļ�
	void CreateFileHead(void * pPulsHead, UINT nPulsHeadSize, TField[] Fields, UINT nFieldCount)
	{
		m_Head.Verstion		= RecFileVerstion;
		m_Head.CreateTime	= time(NULL);
		m_Head.UpdateTime	= time(NULL);					//����ʱ��
		m_Head.PlusHeadSize	= nPulsHeadSize;				//�����ļ�ͷ����
		m_Head.FieldCount	= nFieldCount;					//�ֶ���

		m_File.SeekToBegin();
		m_File.Write(&m_Head, sizeof(TFileHead));			//д���ļ�ͷ
		m_File.Write(pPulsHead, nPulsHeadSize);				//д�븽��ͷ
		m_File.Write(&Fields, nFieldCount * sizeof(TField));//д���ֶζ���

		UpdateFields();										//�����ֶ���Ϣ

		m_File.SetLength(m_DataHead);						//�������
	}
	//��ȡ�����ļ�ͷ
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
		//�����ֶ���Ϣ
		m_File.Seek(m_Head.FieldHead(), CFile::begin);
		m_File.Read(&m_Fields, m_Head.FieldCount * sizeof(TField));

		//�����ֶ�ƫ��λ��
		int Offset = 0;
		for (int i = 0; i < m_Head.FieldCount; i++)
		{
			m_FieldOffsets[i] = Offset + m_Fields[i].Size;
			Offset = m_FieldOffsets[i];
		}
		
		m_RecordSize = Offset + m_Fields[m_Head.FieldCount - 1].Size;		//���¼�¼����
		m_DataHead	 = m_Head.DataHead();									//����������ʼλ��
	}
protected:
	//�����ļ�ͷУ��
	virtual int CheckPulsHead(void * pPulsHead)=1;														
public:
	int Open(char* FileName, void * pPulsHead, UINT nPulsHeadSize, TField[] Fields, UINT nFieldCount, bool ForceInit=false)//ForceInit�Ƿ�ǿ�Ƴ�ʼ��Ϊ���ļ�
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

			int Res = CheckPulsHead(pPlus);	//����ͷУ��
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
	//�����ֶΣ��򻯰棬��������ֶε�ʱ���Ѿ��м�¼�����������ʧ�ܣ�
	bool AddField(TField Field)
	{
		if (m_DataHead - m_File.GetLength() >0)
			return false;//��ʱ��ʵӦ�ý���������һ���ֶνṹ�ĳ��ȣ�Ȼ�����޸�ÿһ�ʼ�¼�Ľṹ���Ժ���ʱ����ʵ��

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
	//�ڵ�ǰλ��д��һ����¼
	void Write(_Record * pRecord)
	{		
		WORD CheckCode= GetCheckCode(pRecord);
		m_File.Write(&CheckCode, sizeof(WORD));							//д��У����
		m_File.Write(pRecord, m_RecordSize);							//д������
	}
	//�ڵ�ǰλ�ö�ȡ
	bool Read(_Record * pRecord)
	{
		WORD CheckCode;
		m_File.Read(&CheckCode, sizeof(WORD));
		UINT leng = m_File.Read(pRecord, m_RecordSize);

		return (leng == m_RecordSize && CheckCode == GetCheckCode(pRecord));
	}
	//����
	void AddRecord(_Record * pRecord)
	{
		m_File.SeekToEnd();
		Write(pRecord);	
	}
	//����¼��д��
	bool UpdateRecord(int Index, _Record * pRecord)
	{
		if (Index >= RecordCount() || Index < 0)	
			return false;

		m_File.Seek(m_DataHead + Index * m_RecordSize, CFile::begin);
		Write(pRecord);

		return true;
	}
	//����¼�Ŷ�ȡ
	bool GetRecord(int Index, _Record * pRecord)
	{
		if(Index >= RecordCount() || Index < 0)	
			return false;

		m_File.Seek(m_DataHead + Index * m_RecordSize, CFile::begin);
		return Read(pRecord);
	}
	//�������
	void ClearRecord()
	{
		m_Head->UpdateTime = time(NULL);							//����ʱ��

		m_File.SetLength(m_DataHead);
		m_File.SeekToBegin();
		m_File.Write(m_Head, sizeof(TFileHead));
		m_File.SeekToEnd();
	}
	//���õ�ǰ�к�
	void SetRowIndex(int RowIndex)
	{
		if(RowIndex >= RecordCount() || RowIndex < 0)
			return;
		m_File.Seek(m_DataHead + RowIndex * m_RecordSize);
	}
	//��õ�ǰ�к�
	int GetRowIndex()
	{
		return int((m_File.GetPosition() - m_DataHead) / m_RecordSize);
	}
	//����һ���ֶ�
	void GetRecord(int Index, TRecord * pRecord)					//���ֶ�����תΪ�ַ������
	{
		if (Index >= m_Head.FieldCount || Index < 0)	
			return;

		m_File.Read(m_pCurrRecord, m_RecordSize);					//���µ�ǰ��¼����
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

	//ϵͳ�Ƿ�ΪBigEndian
	bool IsBigEndian(void)
	{
		union
		{
			unsigned long k;
			unsigned char c[4];
		} u = {0xFF000000};

		return (0xFF == u.c[0]);	//��λ������ϵ�ṹ����͵ĵ�ַ���������Чλ
	}
	//�������ֶ�תΪ��¼�ؼ��֣��������ֶε�ʮ����������һ�飩
	void GetKey(const char * pRecord, char * Key)
	{
		int KeyLeng=0;																				//��������
		int k;
		char * P=NULL;

		for (int i=0; i<m_pHead->FieldCount; i++)
		{
			if (!m_pHead->Fields[i].IndexFlag)														//�����ֶ�
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
				//���������ڴ��еĸߵ�λ˳���й�
				if (IsBigEndian())	//���ֽ����ȱ���
				{
					char * Q = m_pHead->Fields[i].Leng-1;
					for (k=m_pHead->Fields[i].Leng-1; k>=0; k--)
						if (*Q--)						
							CMathEx::BinToHex(P, &(Key[KeyLeng]), k+1);

					KeyLeng += 2 * (k+1);
				}
				else				//���ֽڱ���
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

			Key[KeyLeng++]='_';																	//�����ֶμ������
		}//for

		Key[KeyLeng]=0;
	}
	//�ؽ�����
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
			iter=m_Indexs.find(Key);																//���Ҽ�¼
			if (iter==m_Indexs.end())							
				m_Indexs.insert(map<std::string, int>::value_type(Key, Pos));						//��������

			Pos=GetPosition();
		}
	}
public:
	//�����������¼
	void SaveRecord(void * pRecord)
	{
		int Pos=-1;
		char Key[256];
		GetKey((char *)pRecord, &Key);
		map<std::string, int>::iterator iter=m_Indexs.find(Key);									//���Ҽ�¼
		if (iter!=m_Indexs.end())
			Pos=iter->second;  
																									//ά������
		if (Pos==-1)
		{
			Pos=Seek(0, end);
			m_Indexs.insert(map<std::string, int>::value_type(Key, Pos));
		}

		Seek(Pos, begin);																			//��λ����λ��
		CRecFile::Write(pRecord);
	}
};
/********************************************* CTradeFile *****************************************/
struct TPlusHead
{
	char		UserNo[16];		//�ļ��������ʺ�
	char		ProcessName[50];//�����߽�����
	char		FileName[50];	//ԭʼ�ļ���
	char		SeverIP[16];	//������IP
	char		LocalIP[16];	//����IP
	char		LocalMac[36];	//����MAC
	WORD		Port;			//���Ӷ˿�
	time_t		TradeDate;		//��������
	time_t		InitTime;		//��ʼ��ʱ��
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