#include "BaseInclude.h"



TGlobalModel::TGlobalModel()
{
   
}

TGlobalModel::~TGlobalModel()
{

}
int TGlobalModel::GetSuperPriceOffset(const TContractSingleKey *pKey, TDirect direct)
{
	char szKey[100] = { 0 };
	sprintf_s(szKey, sizeof(szKey), "%s%c%s%s%s%c", pKey->ExchangeNo, pKey->CommodityType, pKey->CommodityNo,
		pKey->ContractNo, pKey->StrikePrice, pKey->OptionType);
	auto iter = m_mapSuperPrice.find(szKey);

	if (iter != m_mapSuperPrice.end())
	{
		if (direct == dBuy)
			return iter->second.iLongSuperPrice;
		return iter->second.iShortSuperPrice;
	}

	if (direct == dBuy)
		return 1;
	return -1;
}
double TGlobalModel::GetSuperPrice(const TContractSingleKey *pKey, TDirect direct)
{
	double dMinChangePrice = CConvert::GetMinChangePrice(pKey);
	return dMinChangePrice*GetSuperPriceOffset(pKey, direct);
}

void TGlobalModel::ForeignCoverOrder(TContractSingleKey & stKey, vector<uint>* pOrderID)
{
	if (!pOrderID) return;
	char szKey[100] = { 0 };
	sprintf_s(szKey, sizeof(szKey), "%s%c%s%s%s%c", stKey.ExchangeNo, stKey.CommodityType, stKey.CommodityNo,
		stKey.ContractNo, stKey.StrikePrice, stKey.OptionType);
	for (uint i = 0; i < pOrderID->size(); i++)
	{
		m_mapForeignParOrder.insert(make_pair(szKey, pOrderID->at(i)));
	}
}
int TGlobalModel::GetForeignCloseNum(TContractSingleKey &stKey, TDirect direct)
{
	char szKey[100] = { 0 };
	sprintf_s(szKey, sizeof(szKey), "%s%c%s%s%s%c", stKey.ExchangeNo, stKey.CommodityType, stKey.CommodityNo,
		stKey.ContractNo, stKey.StrikePrice, stKey.OptionType);
	auto mapRange = m_mapForeignParOrder.equal_range(szKey);
	int iCount = 0;
	map<string, uint>::iterator iter;
	for (iter = mapRange.first; iter != mapRange.second; iter++)
	{
		const TOrder* pOrder = g_pTradeData->GetOrder(iter->second);
		if (pOrder->Direct == direct)
		{
			if (pOrder->OrderState == osQueued || pOrder->OrderState == osPartFilled)
				iCount += pOrder->OrderQty;
		}
	}
	return iCount;
}

void TGlobalModel::WriteConfig()
{
	WritePosConfile();
	WriteOptionConfigFile();
	WriteDefaultConfigFile();
	WriteSuperConfigFile();
	WriteChaseConfigFile();
	WriteStopLossConfigFile();
}


void TGlobalModel::ReadConfig()
{
	ReadPosConfile();
	ReadOptionConfigFile();
	ReadDefaultConfigFile();
	ReadSuperConfigFile();
	ReadChaseConfigFile();
	ReadStopLossConfigFile();
}

//窗口位置数据
void TGlobalModel::ReadPosConfile()
{
    char Name[10] = "窗口位置";
    char buffer[250];
    char cKey[10] ="0";
	wchar_t  szFilePath[MAX_PATH] = { 0 };
	GetFilePath(szFilePath, sizeof(szFilePath) / sizeof(szFilePath[0]), configFilePath);
	if (GetPrivateProfileStringA(Name, cKey, "Over", buffer, sizeof(buffer), LoadRC::unicode_to_ansi(szFilePath).c_str()))
    {
        if (!strcmp(buffer, "Over"))return;
        std::string PriBuffer = buffer;
        std::vector<std::string> vecPriBuffer;
        vecPriBuffer = CConvert::SplitString(PriBuffer, ',');
		if (vecPriBuffer.size() == 4)
		{
			m_PosRect.left = atoi(vecPriBuffer[0].c_str());
			m_PosRect.top = atoi(vecPriBuffer[1].c_str());
			m_PosRect.right = atoi(vecPriBuffer[2].c_str());
			m_PosRect.bottom = atoi(vecPriBuffer[3].c_str());
		}
    }
}
void TGlobalModel::WritePosConfile()
{
    char Name[10] = "窗口位置";
    char cKey[5] = { 0 };
    char Pos[20] = "0";
    //if (m_stOptionConfig.bWndPosition)
    //    GetWindowRect(m_Hwnd, &m_PosRect);
    sprintf_s(Pos, "%d,%d,%d,%d", m_PosRect.left, m_PosRect.top, m_PosRect.right, m_PosRect.bottom);
	wchar_t  szFilePath[MAX_PATH] = { 0 };
	GetFilePath(szFilePath, sizeof(szFilePath) / sizeof(szFilePath[0]), configFilePath);
	WritePrivateProfileStringA(Name, cKey, Pos, LoadRC::unicode_to_ansi(szFilePath).c_str());
}

void TGlobalModel::ReadOptionConfigFile()
{
	wchar_t  szFilePath[MAX_PATH] = { 0 };
	GetFilePath(szFilePath, sizeof(szFilePath) / sizeof(szFilePath[0]), configFilePath);
    LPSTR lpBuffer = new char[250];
	lpBuffer[0] = '\0';
    vector<string> strvec;
	if (::GetPrivateProfileStringA("OptionConfig", "OptionConfig", "Over", lpBuffer, 250, LoadRC::unicode_to_ansi(szFilePath).c_str()))
    {
        int cnt = 0;
		if (!strcmp(lpBuffer, "Over"))return;
        strvec = CConvert::SplitString(lpBuffer, '|');

		if (strvec.size() == 10)
		{
			m_stOptionConfig.bWndPosition = (bool)atoi(strvec[cnt++].c_str());  //下单窗口位置记忆

			m_stOptionConfig.bDoublePositionConfirm = (bool)atoi(strvec[cnt++].c_str()); //双击确认
			m_stOptionConfig.iPositionQty = atoi(strvec[cnt++].c_str());  //点击持仓列表填手数
			m_stOptionConfig.iBackFillOrderMod = atoi(strvec[cnt++].c_str()); //反手默认下单方式
			m_stOptionConfig.iProportionFillOrderMod = atoi(strvec[cnt++].c_str()); //比例平仓下单方式

			m_stOptionConfig.iCloseAllFillOrderMod = atoi(strvec[cnt++].c_str());//清仓下单方式

			m_stOptionConfig.bOrderConfirm = (bool)atoi(strvec[cnt++].c_str()); //三键委托确认
			m_stOptionConfig.bDoubleClickPositionClose = (bool)atoi(strvec[cnt++].c_str()); //双击持仓列表平仓

			m_stOptionConfig.bBigOrderSplit = (bool)atoi(strvec[cnt++].c_str()); //大单拆分
			m_stOptionConfig.Hedge = atoi(strvec[cnt++].c_str()); //投机套保
		}
    }
    delete[] lpBuffer;
}

void TGlobalModel::WriteOptionConfigFile()
{
	wchar_t  szFilePath[MAX_PATH] = { 0 };
	GetFilePath(szFilePath, sizeof(szFilePath) / sizeof(szFilePath[0]), configFilePath);
    LPSTR lpstring = new char[100];
    sprintf_s(lpstring, 100, "%d|%d|%d|%d|%d|%d|%d|%d|%d|%d", m_stOptionConfig.bWndPosition,  m_stOptionConfig.bDoublePositionConfirm,
         m_stOptionConfig.iPositionQty, m_stOptionConfig.iBackFillOrderMod, m_stOptionConfig.iProportionFillOrderMod,
        m_stOptionConfig.iCloseAllFillOrderMod, m_stOptionConfig.bOrderConfirm, m_stOptionConfig.bDoubleClickPositionClose,
        m_stOptionConfig.bBigOrderSplit,m_stOptionConfig.Hedge);

	bool success = ::WritePrivateProfileString(L"OptionConfig", L"OptionConfig", LoadRC::ansi_to_unicode(lpstring).c_str(), szFilePath);
    delete[] lpstring;
}
void TGlobalModel::ReadDefaultConfigFile()
{
    char Name[10] = "默认手数";
    int iKey = 0;
    char buffer[250];
    char cKey[10] = { 0 };
	wchar_t  szFilePath[MAX_PATH] = { 0 };
	GetFilePath(szFilePath, sizeof(szFilePath) / sizeof(szFilePath[0]), configFilePath);
    while (strcmp(buffer, "Over") != 0)
    {
        sprintf_s(cKey, "%d", iKey);
		if (!GetPrivateProfileStringA(Name, cKey, "Over", buffer, sizeof(buffer), LoadRC::unicode_to_ansi(szFilePath).c_str()))
            return;
        //初始化Map
        InitDefaultMapFromPri(buffer);
        iKey++;
    }
    m_DefaultQtySize = m_mapDefaultQty.size();
}
void TGlobalModel::InitDefaultMapFromPri(char* buffer)
{
    if (strcmp(buffer, "Over") == 0)
        return;
    std::string PriBuffer = buffer;
    std::vector<std::string> vecPriBuffer;
    stDefaultQty stQty;
    memset(&stQty, 0, sizeof(stDefaultQty));
    vecPriBuffer = CConvert::SplitString(PriBuffer, ',');
	if (vecPriBuffer.size() == 2)
	{
		stQty.iQty = atoi(vecPriBuffer.at(1).c_str());
		vecPriBuffer = CConvert::SplitString(vecPriBuffer[0], '|');
		if (vecPriBuffer.size() == 9)
		{
			int i = vecPriBuffer.size();
			stQty.stContract.OptionType2 = vecPriBuffer[--i].c_str()[0];
			memcpy(&stQty.stContract.StrikePrice2, vecPriBuffer[--i].c_str(), sizeof(stQty.stContract.StrikePrice));
			memcpy(&stQty.stContract.ContractNo2, vecPriBuffer[--i].c_str(), sizeof(stQty.stContract.ContractNo));
			stQty.stContract.OptionType = vecPriBuffer[--i].c_str()[0];
			memcpy(&stQty.stContract.StrikePrice, vecPriBuffer[--i].c_str(), sizeof(stQty.stContract.StrikePrice));
			memcpy(&stQty.stContract.ContractNo, vecPriBuffer[--i].c_str(), sizeof(stQty.stContract.ContractNo));
			memcpy(&stQty.stContract.CommodityNo, vecPriBuffer[--i].c_str(), sizeof(stQty.stContract.CommodityNo));
			stQty.stContract.CommodityType = (vecPriBuffer[--i].c_str())[0];
			memcpy(&stQty.stContract.ExchangeNo, vecPriBuffer[--i].c_str(), sizeof(stQty.stContract.ExchangeNo));
			char cContractKey[150] = { 0 };
			GetContractKey(cContractKey, stQty.stContract);
			m_mapDefaultQty.insert(std::make_pair(cContractKey, stQty));
		}
	}
}
void TGlobalModel::GetContractKey(char* cContractKey, TContractKey stContract)
{
    sprintf_s(cContractKey, 150, "%s%c%s%s%s%c",
        stContract.ExchangeNo, stContract.CommodityType, stContract.CommodityNo,
        stContract.ContractNo, stContract.StrikePrice, stContract.OptionType);
}
void TGlobalModel::GetDefaultPriKey(char* cContractKey, stDefaultQty stQty)
{
    sprintf_s(cContractKey, 150, "%s|%c|%s|%s|%s|%c|%s|%s|%c,%d",
        stQty.stContract.ExchangeNo, stQty.stContract.CommodityType, stQty.stContract.CommodityNo,
        stQty.stContract.ContractNo, stQty.stContract.StrikePrice, stQty.stContract.OptionType,
        stQty.stContract.ContractNo2, stQty.stContract.StrikePrice2, stQty.stContract.OptionType2, stQty.iQty);
}
void TGlobalModel::WriteDefaultConfigFile()
{
    char Name[10] = "默认手数";
    int iKey = 0;
    char cKey[5] = { 0 };
    char cContractKey[150] = { 0 };
    std::map<std::string, stDefaultQty>::iterator iter = m_mapDefaultQty.begin();
	wchar_t  szFilePath[MAX_PATH] = { 0 };
	GetFilePath(szFilePath, sizeof(szFilePath) / sizeof(szFilePath[0]), configFilePath);
    for (; iter != m_mapDefaultQty.end(); iter++)
    {
        sprintf_s(cKey, "%d", iKey);
        GetDefaultPriKey(cContractKey, iter->second);
		WritePrivateProfileStringA(Name, cKey, cContractKey, LoadRC::unicode_to_ansi(szFilePath).c_str());
        iKey++;
    }

	//清空剩余记录
    if (m_mapDefaultQty.size() < m_DefaultQtySize)
    {
        sprintf_s(cKey, "%d", iKey);
        for (; iKey < m_DefaultQtySize; iKey++)
        {
			WritePrivateProfileStringA(Name, cKey, NULL, LoadRC::unicode_to_ansi(szFilePath).c_str());
        }
    }
    m_DefaultQtySize = m_mapDefaultQty.size();
}

void TGlobalModel::ReadSuperConfigFile()
{
    char Name[10] = "超价参数";
    int iKey = 0;
    char cKey[5] = { 0 };
    char buffer[250] = { 0 };
	wchar_t  szFilePath[MAX_PATH] = { 0 };
	GetFilePath(szFilePath, sizeof(szFilePath) / sizeof(szFilePath[0]), configFilePath);
    while (strcmp(buffer, "Over") != 0)
    {
        sprintf_s(cKey, "%d", iKey);
		if (!GetPrivateProfileStringA(Name, cKey, "Over", buffer, sizeof(buffer), LoadRC::unicode_to_ansi(szFilePath).c_str()))
            return;
        InitSuperPriceMapFromPri(buffer);
        iKey++;
    }
    m_SuperPriceSize = m_mapSuperPrice.size();
}
void TGlobalModel::WriteSuperConfigFile()
{
    char Name[10] = "超价参数";
    int iKey = 0;
    char cKey[5] = { 0 };
    char cContractKey[150] = { 0 };
	wchar_t  szFilePath[MAX_PATH] = { 0 };
	GetFilePath(szFilePath, sizeof(szFilePath) / sizeof(szFilePath[0]), configFilePath);
    std::map<std::string, stSuperPrice>::iterator iter = m_mapSuperPrice.begin();
    for (; iter != m_mapSuperPrice.end(); iter++)
    {
        sprintf_s(cKey, "%d", iKey);
        GetSuperPrcePriKey(cContractKey, iter->second);
		WritePrivateProfileStringA(Name, cKey, cContractKey, LoadRC::unicode_to_ansi(szFilePath).c_str());
        iKey++;
    }

	//清空剩余记录
    if (m_mapSuperPrice.size() < m_SuperPriceSize)
    {
        sprintf_s(cKey, "%d", iKey);
        for (; iKey < m_SuperPriceSize; iKey)
        {
			WritePrivateProfileStringA(Name, cKey, NULL, LoadRC::unicode_to_ansi(szFilePath).c_str());
        }
    }
    m_SuperPriceSize = m_mapSuperPrice.size();
}
void TGlobalModel::InitSuperPriceMapFromPri(char* buffer)
{
    if (strcmp(buffer, "Over") == 0)
        return;
    std::string PriBuffer = buffer;
    std::vector<std::string> vecPriBuffer;
    stSuperPrice stPrice;
    memset(&stPrice, 0, sizeof(stPrice));
    vecPriBuffer = CConvert::SplitString(PriBuffer, ',');

	if (vecPriBuffer.size() == 4)
	{
		stPrice.iLongSuperPrice = atoi(vecPriBuffer.at(1).c_str());
		stPrice.iShortSuperPrice = atoi(vecPriBuffer.at(2).c_str());
		stPrice.dMinChangePrice = atof(vecPriBuffer.at(3).c_str());

		vecPriBuffer = CConvert::SplitString(vecPriBuffer[0], '|');

		int i = vecPriBuffer.size();
		if (i == 9)
		{
			stPrice.stContract.OptionType2 = vecPriBuffer[--i].c_str()[0];
			memcpy(&stPrice.stContract.StrikePrice2, vecPriBuffer[--i].c_str(), sizeof(stPrice.stContract.StrikePrice));
			memcpy(&stPrice.stContract.ContractNo2, vecPriBuffer[--i].c_str(), sizeof(stPrice.stContract.ContractNo));
			stPrice.stContract.OptionType = vecPriBuffer[--i].c_str()[0];
			memcpy(&stPrice.stContract.StrikePrice, vecPriBuffer[--i].c_str(), sizeof(stPrice.stContract.StrikePrice));
			memcpy(&stPrice.stContract.ContractNo, vecPriBuffer[--i].c_str(), sizeof(stPrice.stContract.ContractNo));
			memcpy(&stPrice.stContract.CommodityNo, vecPriBuffer[--i].c_str(), sizeof(stPrice.stContract.CommodityNo));
			stPrice.stContract.CommodityType = (vecPriBuffer[--i].c_str())[0];
			memcpy(&stPrice.stContract.ExchangeNo, vecPriBuffer[--i].c_str(), sizeof(stPrice.stContract.ExchangeNo));
			char cContractKey[150] = { 0 };
			GetContractKey(cContractKey, stPrice.stContract);
			m_mapSuperPrice.insert(std::make_pair(cContractKey, stPrice));
		}
	}
}
void TGlobalModel::GetSuperPrcePriKey(char* cContractKey, stSuperPrice st)
{
    sprintf_s(cContractKey, 150, "%s|%c|%s|%s|%s|%c|%s|%s|%c,%d,%d,%f",
        st.stContract.ExchangeNo, st.stContract.CommodityType, st.stContract.CommodityNo,
        st.stContract.ContractNo, st.stContract.StrikePrice, st.stContract.OptionType,
        st.stContract.ContractNo2, st.stContract.StrikePrice2, st.stContract.OptionType2, st.iLongSuperPrice, st.iShortSuperPrice, st.dMinChangePrice);
}
void TGlobalModel::ReadChaseConfigFile()
{
    char Name[10] = "追价参数";
    char cKey[2] = "0";
    char buffer[250] = { 0 };
	wchar_t  szFilePath[MAX_PATH] = { 0 };
	GetFilePath(szFilePath, sizeof(szFilePath) / sizeof(szFilePath[0]), configFilePath);
	if (GetPrivateProfileStringA(Name, cKey, "Over", buffer, sizeof(buffer), LoadRC::unicode_to_ansi(szFilePath).c_str()))
        InitChasePriceStFromPri(buffer);

}
void TGlobalModel::WriteChaseConfigFile()
{
    char Name[10] = "追价参数";
    int iKey = 0;
    char cKey[2] = { 0 };
    char PriKey[150] = { 0 };
    sprintf_s(cKey, "%d", iKey);
	wchar_t  szFilePath[MAX_PATH] = { 0 };
	GetFilePath(szFilePath, sizeof(szFilePath) / sizeof(szFilePath[0]), configFilePath);
    sprintf_s(PriKey, 150, "%d|%d|%d", m_stChaseConfig.iSecond, m_stChaseConfig.iChasePriceCom, m_stChaseConfig.iChaseOver);
	WritePrivateProfileStringA(Name, cKey, PriKey, LoadRC::unicode_to_ansi(szFilePath).c_str());
}
void TGlobalModel::InitChasePriceStFromPri(char* buffer)
{
    if (strcmp(buffer, "Over") == 0)
        return;
    int i = 0;
    std::string PriBuffer = buffer;
    std::vector<std::string> vecPriBuffer;
    vecPriBuffer = CConvert::SplitString(PriBuffer, '|');
	if (vecPriBuffer.size() == 3)
	{
		memset(&m_stChaseConfig, 0, sizeof(m_stChaseConfig));
		m_stChaseConfig.iSecond = atoi(vecPriBuffer[i++].c_str());
		m_stChaseConfig.iChasePriceCom = atoi(vecPriBuffer[i++].c_str());
		m_stChaseConfig.iChaseOver = atoi(vecPriBuffer[i++].c_str());
	}
}

void TGlobalModel::ReadStopLossConfigFile()
{
    char Name[20] = "止损参数列表";
    int iKey = 0;
    char cKey[5] = { 0 };
    char buffer[250] = { 0 };
	wchar_t  szFilePath[MAX_PATH] = { 0 };
	GetFilePath(szFilePath, sizeof(szFilePath) / sizeof(szFilePath[0]), configFilePath);
    while (strcmp(buffer, "Over") != 0)
    {
        sprintf_s(cKey, "%d", iKey);
		if (!GetPrivateProfileStringA(Name, cKey, "Over", buffer, sizeof(buffer), LoadRC::unicode_to_ansi(szFilePath).c_str()))
            return;
        InitStopLossMapFromPri(buffer);
        iKey++;
    }
    m_StopLossSize = m_stStopLossConfig.stdmapContract.size();

    LPSTR lpBuffer = new char[250];
    vector<string> strvec;
	if (::GetPrivateProfileStringA("止损参数", "止损参数", 0, lpBuffer, 250, LoadRC::unicode_to_ansi(szFilePath).c_str()))
    {
        int cnt = 0;
        strvec = CConvert::SplitString(lpBuffer, '|');
		
		if (strvec.size() == 6)
		{
			m_stStopLossConfig.iTriggerCount = atoi(strvec[cnt++].c_str());
			m_stStopLossConfig.iOpenAutoStopLossBasicPrice = atoi(strvec[cnt++].c_str());
			m_stStopLossConfig.iDefaultTactic = atoi(strvec[cnt++].c_str());

			m_stStopLossConfig.bOpenAutoStopLoss = (bool)atoi(strvec[cnt++].c_str());

			m_stStopLossConfig.iStopLossOrderPrice = atoi(strvec[cnt++].c_str());
			m_stStopLossConfig.iStopProfitOrderPrice = atoi(strvec[cnt++].c_str());
		}
    }
    delete[] lpBuffer;
}
void TGlobalModel::WriteStopLossConfigFile()
{

    char Name[20] = "止损参数列表";
    int iKey = 0;
    char cKey[5] = { 0 };
    char cContractKey[150] = { 0 };
    std::map<std::string, stSLContractPriceMargin>::iterator iter = m_stStopLossConfig.stdmapContract.begin();
	wchar_t  szFilePath[MAX_PATH] = { 0 };
	GetFilePath(szFilePath, sizeof(szFilePath) / sizeof(szFilePath[0]), configFilePath);
    for (; iter != m_stStopLossConfig.stdmapContract.end(); iter++)
    {
        sprintf_s(cKey, "%d", iKey);
        GetStopLossPriKey(cContractKey, iter->second);
		WritePrivateProfileStringA(Name, cKey, cContractKey, LoadRC::unicode_to_ansi(szFilePath).c_str());
        iKey++;
    }
    if (m_stStopLossConfig.stdmapContract.size() < m_StopLossSize)
    {
        sprintf_s(cKey, "%d", iKey);
        for (; iKey < m_StopLossSize; iKey)
        {
			WritePrivateProfileStringA(Name, cKey, NULL, LoadRC::unicode_to_ansi(szFilePath).c_str());
        }
    }
    m_StopLossSize = m_stStopLossConfig.stdmapContract.size();

    LPSTR lpstring = new char[100];
    sprintf_s(lpstring, 100, "%d|%d|%d|%d|%d|%d", m_stStopLossConfig.iTriggerCount,
        m_stStopLossConfig.iOpenAutoStopLossBasicPrice, m_stStopLossConfig.iDefaultTactic,
        m_stStopLossConfig.bOpenAutoStopLoss,
        m_stStopLossConfig.iStopLossOrderPrice, m_stStopLossConfig.iStopProfitOrderPrice);

	bool success = ::WritePrivateProfileStringA("止损参数", "止损参数", lpstring, LoadRC::unicode_to_ansi(szFilePath).c_str());
    delete[] lpstring;
}
void TGlobalModel::InitStopLossMapFromPri(char* buffer)
{
    if (strcmp(buffer, "Over") == 0)
        return;
    std::string PriBuffer = buffer;
    std::vector<std::string> vecPriBuffer;
    stSLContractPriceMargin stSLContract;
    memset(&stSLContract, 0, sizeof(stSLContract));
    vecPriBuffer = CConvert::SplitString(PriBuffer, ',');
	
	if (vecPriBuffer.size() == 4)
	{
		stSLContract.iStopLossPriceGap = atoi(vecPriBuffer.at(1).c_str());
		stSLContract.iStopProfitPriceGap = atoi(vecPriBuffer.at(2).c_str());
		stSLContract.dMinChangePrice = atof(vecPriBuffer.at(3).c_str());
		vecPriBuffer = CConvert::SplitString(vecPriBuffer[0], '|');
		int i = vecPriBuffer.size();

		if (i == 9)
		{
			stSLContract.stContract.OptionType2 = vecPriBuffer[--i].c_str()[0];
			memcpy(&stSLContract.stContract.StrikePrice2, vecPriBuffer[--i].c_str(), sizeof(stSLContract.stContract.StrikePrice));
			memcpy(&stSLContract.stContract.ContractNo2, vecPriBuffer[--i].c_str(), sizeof(stSLContract.stContract.ContractNo));
			stSLContract.stContract.OptionType = vecPriBuffer[--i].c_str()[0];
			memcpy(&stSLContract.stContract.StrikePrice, vecPriBuffer[--i].c_str(), sizeof(stSLContract.stContract.StrikePrice));
			memcpy(&stSLContract.stContract.ContractNo, vecPriBuffer[--i].c_str(), sizeof(stSLContract.stContract.ContractNo));
			memcpy(&stSLContract.stContract.CommodityNo, vecPriBuffer[--i].c_str(), sizeof(stSLContract.stContract.CommodityNo));
			stSLContract.stContract.CommodityType = (vecPriBuffer[--i].c_str())[0];
			memcpy(&stSLContract.stContract.ExchangeNo, vecPriBuffer[--i].c_str(), sizeof(stSLContract.stContract.ExchangeNo));

			char cContractKey[150] = { 0 };
			GetContractKey(cContractKey, stSLContract.stContract);
			m_stStopLossConfig.stdmapContract.insert(std::make_pair(cContractKey, stSLContract));
		}
	}
}
void TGlobalModel::GetStopLossPriKey(char* cContractKey, stSLContractPriceMargin st)
{
    sprintf_s(cContractKey, 150, "%s|%c|%s|%s|%s|%c|%s|%s|%c,%d,%d,%f",
        st.stContract.ExchangeNo, st.stContract.CommodityType, st.stContract.CommodityNo,
        st.stContract.ContractNo, st.stContract.StrikePrice, st.stContract.OptionType,
        st.stContract.ContractNo2, st.stContract.StrikePrice2, st.stContract.OptionType2, st.iStopLossPriceGap, st.iStopProfitPriceGap, st.dMinChangePrice);
}