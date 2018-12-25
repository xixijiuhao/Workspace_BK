#pragma once

#include "..\..\Include\TradeApi_Struct.h"
#include <iostream>
#include "csv_dsv.h"
#include "load_rc.h"

class TContractToFile
{
private:
	FILE *	m_File;

public:
	TContractToFile() :m_File(nullptr)
	{
	}
	virtual ~TContractToFile()
	{
		Close();
	}
	bool IsOpened()
	{
		return m_File != nullptr;
	}
	//文件初始化
	bool Open(const char * FileName)
	{
		if (0 == strcmp(FileName, ""))
			return false;

		m_File = _fsopen(FileName, "wb", _SH_DENYNO);
		if (m_File == nullptr)
			return false;

		fseek(m_File, 0, SEEK_END);

		return true;
	}
	void Close()
	{
		if (nullptr != m_File)
			fclose(m_File);
		m_File = nullptr;
	}

protected:
	void AddBin(const void * pData, UINT DataLen, bool Flush = true)
	{
		if (!IsOpened())
			return;

		fwrite(pData, 1, DataLen, m_File);
		if (Flush)
			fflush(m_File);
	}

public:
	void AddContract(const TContract & contract, bool add_tick = true, const char * name = "")
	{
		if (!IsOpened())
			return;

		Strings items;
		items.push_back(contract.ExchangeNo);
		items.push_back(string(1, contract.CommodityType));
		items.push_back(contract.CommodityNo);
		items.push_back(contract.ContractNo);
		items.push_back(contract.StrikePrice);
		items.push_back(string(1, contract.OptionType));
		items.push_back(contract.ContractNo2);
		items.push_back(contract.StrikePrice2);
		items.push_back(string(1, contract.OptionType2));
		if (add_tick)
		{
			char tick[50];
			_snprintf_s(tick, contract.pCommodity ? contract.pCommodity->PriceFormat : "%f", contract.pCommodity ? contract.pCommodity->UpperTick : 1);
			items.push_back(tick);
		}		
		if (name)
			items.push_back(name);//contract.pCommodity ? LoadRC::unicode_to_ansi(contract.pCommodity->CommodityName) : contract.ContractName);		

		string line;
		TCsv::StringsToCsv(items, line);
		AddBin(line.c_str(), line.length(), false);
	}
};
