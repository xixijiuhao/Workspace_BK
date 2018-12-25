#pragma once

#include "stdint.h"

class TCRC
{
private:
	uint32_t Crc32Table[256];

	void MakeCRC32Table()
	{
		uint32_t c;
		for (int i = 0; i< (int)(sizeof(Crc32Table) / sizeof(Crc32Table[0])); i++)
		{
			c = i;
			for (int j = 0; j <= 7; j++)
			{
				if (c & 1)
					c = 0xEDB88320 ^ (c >> 1);
				else
					c = c >> 1;
			}
			Crc32Table[i] = c;
		}
	}
public:
	TCRC(){	MakeCRC32Table();	}
protected:
	void CRC32Init(uint32_t & crc)
	{
		crc = 0xFFFFFFFF;
	}
	void CRC32Update(uint32_t &crc, const void * buf, uint32_t len)
	{
		uint8_t *P = (uint8_t*)buf;
		while (len > 0)
		{
			crc = (crc >> 8) ^ Crc32Table[(crc ^ *P) & 0xFF];
			P++;
			len--;
		}
	}
	uint32_t CRC32Final(uint32_t &crc)
	{
		return ~crc;
	}
public:	
	uint32_t Crc32Buf(const void * buf, uint32_t len)
	{
		uint32_t Context;
		CRC32Init(Context);
		CRC32Update(Context, buf, len);
		return CRC32Final(Context);
	}

	//uint Crc32File(const char * FileName, uint32_t StartPos=0)
	//{
	//	CFile mFile;
	//	if (! mFile.Open(FileName, CFile::modeReadWrite | CFile::shareDenyWrite))
	//		return 0;

	//	uint32_t Context, BytesRead;
	//	char Buf[65536];

	//	CRC32Init(Context);

	//	mFile.Seek(StartPos, CFile::begin);
	//	do
	//	{
	//		BytesRead = mFile.Read(&Buf, sizeof(Buf));
	//		if (BytesRead<=sizeof(Buf) && BytesRead>0)
	//			CRC32Update(Context, &Buf[0], BytesRead);
	//		else
	//			break;
	//	}while (BytesRead <= 0);

	//	mFile.Close();

	//	return CRC32Final(Context);
	//}
};
