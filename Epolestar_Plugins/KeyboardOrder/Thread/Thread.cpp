#include "../PreInclude.h"

extern ITradeApi *g_pTradeApi;
extern ITradeData *g_pTradeData;
extern IStarApi *g_StarApi;

CThread::CThread() :m_hThread(NULL)
, m_ThreadID(0)
, m_bInitComple(false)
, m_bRunning(false)
, m_iversion(0)
, m_sign("")
{

}
CThread::~CThread()
{


}
DWORD CThread::stdThreadProc(LPVOID lpParameter)
{
	CThread*pThread = (CThread*)lpParameter;
	if (pThread)
	{
		pThread->SetRunning(true);
		pThread->Run();
		pThread->SetRunning(false);
	}
	return 0;
}
void CThread::Start()
{
	/*if (GetInitComple())
		return;*/
	SetInitComple(false);
	Close();

	CreateThread();
	if (m_hThread&& GetVersion() ==0)
		ResumeThread(m_hThread);
}
void CThread::Close()
{
	if (m_hThread)
	{
		::CloseHandle(m_hThread);
		m_hThread = NULL;
	}
}
void CThread::Run()
{
	UpdateMap();
	SetInitComple(true);
}
void CThread::UpdateMap()
{
	m_mapContract.clear();
	TDataContainer tempExcDataCon;
	TDataContainer tempComData;
	TDataContainer tempConDataCon;
	char cc[101] = { 0 };
	g_pTradeData->GetAllExchange(tempExcDataCon, m_sign);
	for (uint i = 0; i < tempExcDataCon.size(); i++)//遍历交易所
	{
		const char *strExchange = ((const char*)tempExcDataCon.at(i));
		//根据品种类型获得所有商品
		tempComData.clear();
		SCommodity* commodityData[100] = { 0 };
		SCommodityNoType commodityNo;
		SRetType count = g_StarApi->GetCommodityData(strExchange,"",commodityData,100,true);
		while (count)
		{
			for (int i = 0; i < count; i++) {
				tempComData.push_back(commodityData[i]);
			}
			if (count < 100)break;
			strcpy_s(commodityNo, commodityData[count - 1]->CommodityNo);
			count = g_StarApi->GetCommodityData(strExchange, commodityNo, commodityData, 100, true);
		}

		//TExchangeKey exchangeKey = { 0 };
		//strcpy_s(exchangeKey.ExchangeNo, strExchange);
		//strcpy_s(exchangeKey.Sign, m_sign);
		//g_pTradeData->GetCommoditys(tempComData, &exchangeKey);
		for (vector<SCommodity*>::size_type k = 0; k < tempComData.size(); k++)//遍历品种
		{
			tempConDataCon.clear();
			SCommodity* pCommodity = (SCommodity*)tempComData.m_VecData[k];
			if (pCommodity&&pCommodity->CommodityType != ctFutures)
				continue;

			SContract* contractData[100] = { 0 };
			SContractNoType contractNo;
			SRetType count = g_StarApi->GetContractData(pCommodity->CommodityNo, "", contractData, 100, true);
			while (count)
			{
				for (int i = 0; i < count; i++) {
					tempConDataCon.push_back(contractData[i]);
				}
				if (count < 100)break;
				strcpy_s(contractNo, contractData[count - 1]->ContractNo);
				count = g_StarApi->GetContractData(strExchange, contractNo, contractData, 100, true);
			}

			for (uint i = 0; i < tempConDataCon.size(); i++)//根据交易所、品种类型、品种获取合约 遍历
			{
				SContract* pContract = ((SContract*)tempConDataCon.at(i));
				if (pContract)
				{
					std::vector<std::string> vecStr;
					vecStr = CConvert::SplitString(pContract->ContractNo, '|');
					if (vecStr.size() == 4) {
						sprintf_s(cc, "%s%s", vecStr[2].c_str(), vecStr[3].c_str());
						m_mapContract[cc] = pContract;
						if (vecStr[3].length() == 4)
							vecStr[3] = vecStr[3].substr(1, string::npos);
						else if (vecStr[3].length() == 3)
							vecStr[3] = "1" + vecStr[3];
						sprintf_s(cc, "%s%s", vecStr[2].c_str(), vecStr[3].c_str());
						m_mapContract[cc] = pContract;
					}

				}
			}
		}
	}
}
void CThread::CreateThread()
{
	if (!m_hThread)
		m_hThread = ::CreateThread(NULL, 0, &(CThread::stdThreadProc), this, CREATE_SUSPENDED, &m_ThreadID);
}
bool CThread::GetInitComple()
{
	CMyLock::Guard lock(m_lock);
	return m_bInitComple;
}
void CThread::SetInitComple(bool b)
{
	CMyLock::Guard lock(m_lock);
	m_bInitComple = b;
}
bool CThread::GetRunning()
{
	CMyLock::Guard lock(m_lock);
	return m_bRunning;
}
void CThread::SetRunning(bool b)
{
	CMyLock::Guard lock(m_lock);
	m_bRunning = b;
}
int CThread::GetVersion()
{
	CMyLock::Guard lock(m_lock);
	return m_iversion ;
}
void CThread::SetVersion(int i)
{
	CMyLock::Guard lock(m_lock);
	m_iversion = i;
}
void CThread::SetSign(const char * Sign) {
	CMyLock::Guard lock(m_lock);
	strncpy_s(m_sign, Sign, 21);
}
SContract*CThread::GetContractPtr(const string &strcode)
{
	if (GetInitComple())
	{
		mapContract::iterator it=m_mapContract.find(strcode);
		if (it != m_mapContract.end())
			return it->second;
	}
	return NULL;
}