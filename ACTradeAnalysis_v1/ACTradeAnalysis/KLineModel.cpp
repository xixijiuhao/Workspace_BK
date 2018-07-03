#include "BaseInclude.h"
KLineModel::KLineModel()
{
	m_dUnit = 1;
	m_dMaxPrice = 0;
	m_dMinPrice = DBL_MAX;

	m_iEndIndex = 0;
	m_iStartIndex = 0;
	m_iFocusIndex = 0;

	m_bShowMainFrm = true;

	m_rcCenter = { 0 };
}

KLineModel::~KLineModel()
{
}

void KLineModel::SetPresenter(ACAnalysisPresenter* presenter)
{
	if (presenter) 
	{
		m_clPreseter = presenter;
	}

	//注册一个回调函数，将KLineModel::OnHttpReceive函数 传给httpTool：：m_HttpReceive
	m_httpTool.RegistHttpRecv(std::bind(&KLineModel::OnHttpReceive, this, placeholders::_1));
	//注册一个回调函数，将KLineModel::OnHttpInitReceive函数 传给httpTool：：m_HttpInitReciver
	m_httpTool.RegistHttpInitRecv(std::bind(&KLineModel::OnHttpInitReceive, this, placeholders::_1));
	//新创建一个线程，调用 httpTools::Run() 从Http中取出数据，放入m_httpData，m_IntroductText
	m_httpTool.Start();
}
void KLineModel::SetKContract(ACContract contract)
{
	memcpy(&m_KContract, &contract, sizeof(ACContract));
	SetEndIndex(0);
	InitDataFromTCMap();
	SSessionIdType sessionID = 0;
	if (g_pQuoteApi) 
	{
		int iResult = g_pQuoteApi->SubHisQuote(
			contract.SubContractNo, 
			KLineUtil::GetKLineType(), 
			KLineUtil::GetKLineSlice(),
			KLineMaxCount, 
			sessionID, 
			m_clPreseter);

		if (iResult == 2) 
		{
			UpdateTotalData();
		}
	}
}

void KLineModel::SetContractInfoItem(ContractInfoViewItemData contractItemData)
{
	//memcpy(&m_KContract, &contract, sizeof(ACContract));
	auto iter = m_clPreseter->m_mapSymbolContract.find(contractItemData.symbol);
	if (iter != m_clPreseter->m_mapSymbolContract.end())
	{
		strcpy_s(m_KContract.SubContractNo, iter->second.c_str());
	}
	else
	{
		//TODO
		return;
	}
	strcpy_s(m_KContract.commoditySymbol, contractItemData.symbol);
	strcpy_s(m_KContract.pattern, contractItemData.pattern);
	m_KContract.resultuid = contractItemData.resultuid;

	SContract* contract[1] = { 0 };
	SContract* underLay = NULL;
	int icount = g_pQuoteApi->GetContractData("", m_KContract.SubContractNo, contract, 1, false);
	//区分主力合约和非主力合约
	if (icount)
	{
		underLay = g_pQuoteApi->GetContractUnderlay(contract[0]->ContractNo);
		if (underLay)
		{
			m_KContract.pContract = underLay;
		}
		else
		{
			m_KContract.pContract = contract[0];
		}
	}
	//将Kline的起点复位
	SetEndIndex(0);
	InitDataFromTCMap();
	SSessionIdType sessionID = 0;
	if (g_pQuoteApi)
	{
		int iResult = g_pQuoteApi->SubHisQuote(
			m_KContract.SubContractNo,
			KLineUtil::GetKLineType(),
			KLineUtil::GetKLineSlice(),
			KLineMaxCount,
			sessionID,
			m_clPreseter);

		//if (iResult == 2)
		//{	
		//}
		UpdateTotalData();
	}
}

void KLineModel::InitSubData(RECT center)
{
	m_rcCenter = center;
	m_rcCenter.right = center.right - KLineUtil::GetKLineTotalWidth() * 10;

	int Width = m_rcCenter.right - m_rcCenter.left;
	int klineCount = Width / KLineUtil::GetKLineTotalWidth();

	m_iEndIndex = 0;
	m_iStartIndex = m_iEndIndex + klineCount - 1;
	m_iFocusIndex = m_iEndIndex;

	//EndIndex > 获取总数量
	if (m_iStartIndex >= m_iCount) 
	{
		m_iStartIndex = m_iCount - 1;
	}
	
	UpdateSubData();
	m_clPreseter->UpdateKLineView(m_KContract);
}
void KLineModel::UpdateSubData(BOOL updateView)
{
	int iHeight = m_rcCenter.bottom - m_rcCenter.top;
	if (!iHeight) { return;}

	if (updateView)
	{
		int Width = m_rcCenter.right - m_rcCenter.left;
		int klineCount = Width / KLineUtil::GetKLineTotalWidth();

		m_iEndIndex = 0;
		m_iStartIndex = m_iEndIndex + klineCount - 1;
		m_iFocusIndex = m_iEndIndex;

		//EndIndex > 获取总数量
		if (m_iStartIndex >= m_iCount)
		{
			m_iStartIndex = m_iCount - 1;
		}
	}
	
	m_vtKLineDrawData.clear();
	m_dMaxPrice = 0;
	m_dMinPrice = DBL_MAX;
	for (int index = m_iEndIndex; index <= m_iStartIndex; index++)
	{
		SHisQuoteData data = m_stTotalData[index];
		m_vtKLineDrawData.push_back(data);
		if (data.QHighPrice > m_dMaxPrice) 
		{
			m_dMaxPrice = data.QHighPrice;
		}
		if (data.QLowPrice < m_dMinPrice) 
		{
			m_dMinPrice = data.QLowPrice;
		}
	}
	UpdateSubDataPos();
}
void KLineModel::UpdateSubDataPos()
{
	m_vtKLineDrawPoint.clear();
	int iHeight = m_rcCenter.bottom - m_rcCenter.top;
	KLinePoint point = { 0 };
	SHisQuoteData data;
	int iSize = m_vtKLineDrawData.size();

	m_dUnit = (m_dMaxPrice - m_dMinPrice) / (iHeight);
	for (int index = 0; index < iSize; index++)
	{
		data = m_vtKLineDrawData[index];
		point.HighY = m_rcCenter.top + iHeight - (data.QHighPrice - m_dMinPrice) / m_dUnit;
		point.LowY = m_rcCenter.top + iHeight - (data.QLowPrice - m_dMinPrice) / m_dUnit;
		point.OpenY = m_rcCenter.top + iHeight - (data.QOpeningPrice - m_dMinPrice) / m_dUnit;
		point.CloseY = m_rcCenter.top + iHeight - (data.QLastPrice - m_dMinPrice) / m_dUnit;
		point.x = (iSize - index - 1) * KLineUtil::GetKLineTotalWidth() + m_rcCenter.left;
		m_vtKLineDrawPoint.push_back(point);
	}
	//保存最新价坐标
	data = m_stTotalData[0];
	m_LastPricePoint = m_rcCenter.top + iHeight - (data.QLastPrice - m_dMinPrice) / m_dUnit;
}
void KLineModel::Slide(int changePix)
{

}
void KLineModel::SetEndIndex(int endIndex)
{
	int LineCount = (m_rcCenter.right - m_rcCenter.left) / (KLineUtil::GetKLineTotalWidth());
	if (endIndex < 0 ) 
	{
		m_iEndIndex = 0;
		m_iStartIndex = m_iEndIndex + LineCount;
		if (m_iStartIndex >= m_iCount) {
			m_iStartIndex = m_iCount - 1;
		}
	}
	else if (endIndex >= m_iCount) 
	{
		m_iStartIndex = m_iCount - 1;
		m_iEndIndex = m_iStartIndex - LineCount;
		if (m_iEndIndex < 0) {
			m_iEndIndex = 0;
		}
	}
	else 
	{
		m_iEndIndex = endIndex;
		m_iStartIndex = m_iEndIndex + LineCount;
		if (m_iStartIndex >= m_iCount) {
			m_iStartIndex = m_iCount - 1;
			m_iEndIndex = m_iStartIndex - LineCount;
			if (m_iEndIndex < 0) {
				m_iEndIndex = 0;
			}
		}
	}
}
int KLineModel::GetYPosByPrice(SPriceType price)
{
	int pos = m_rcCenter.top + (m_rcCenter.bottom - m_rcCenter.top) - (price - m_dMinPrice) / m_dUnit;
	return pos;
}
SPriceType KLineModel::GetPriceByYPos(int Ypos)
{
	SPriceType price = m_dUnit * (m_rcCenter.top + (m_rcCenter.bottom - m_rcCenter.top) - Ypos) + m_dMinPrice;
	return price;
}
int KLineModel::GetXIndex(int x)
{
	int index = (x - KLineView_LeftWidth) / KLineUtil::GetKLineTotalWidth();
	if (index < m_vtKLineDrawPoint.size()) 
	{
		return (m_vtKLineDrawPoint.size() - 1 - index);
	}
	return m_vtKLineDrawPoint.size() - 1;
}
int KLineModel::GetDateTimeIndex(SDateTimeType dateTime)
{
	for (int index = 0 ; index < m_vtKLineDrawData.size(); index++)
	{
		if (m_vtKLineDrawData[index].DateTimeStamp >= dateTime && 
			index + 1 < m_vtKLineDrawData.size()  && 
			m_vtKLineDrawData[index + 1].DateTimeStamp < dateTime) 
		{
			return index;
		}
	}
	return -1;
}
void KLineModel::OnHisQuote(const SContract* contract, const SKLineTypeType klinetype)
{
	UpdateTotalData();
}
void KLineModel::OnHttpReceive(ACSpiData data)
{
	memcpy(&m_httpData, &data, sizeof(ACSpiData));
	PostMessage(m_clPreseter->GetHwnd(), SSWM_HTTP_SPI, 0, 0);
}
void KLineModel::InitDataFromTCMap()
{
	//拼接出key
	char key[256] = { 0 };
	sprintf_s(key, "%s|%s", m_KContract.commoditySymbol, m_KContract.pattern);
	ACDataMap::iterator iter = m_TCDataMap.find(key);

	if (iter == m_TCDataMap.end()) 
	{
		m_httpData.m_bValid = false;
		char tips[256] = { 0 };
		m_IntroductText.clear();
		if (m_TCDataMap.size() == 0) 
		{
			WCharToMByte(g_pLanguageApi->LangText(GettingDataStrID), tips, sizeof(tips));
			m_IntroductText.push_back(tips);
		}
		else 
		{
			WCharToMByte(g_pLanguageApi->LangText(NoDataTipStrID), tips, sizeof(tips));
			m_IntroductText.push_back(tips);
		}	
	}
	else 
	{
		memcpy(&m_httpData, &iter->second, sizeof(ACSpiData));
		//m_httpData = iter->second;
		//m_IntroductText = iter->second.text;
	}
}
void KLineModel::OnHttpInitReceive(ACDataMap& DataMap)
{
	m_ExchangeNoACDataMap.emplace(make_pair(m_clPreseter->m_sCurExchangeNo, DataMap));

	m_TCDataMap = DataMap;
	InitContractFromTCMap();
	InitDataFromTCMap();
	m_clPreseter->getCContractInfoListView()->SetTotalData(m_TCDataMap);

	SSessionIdType sessionID = 0;
	if (g_pQuoteApi)
	{
		int iResult = g_pQuoteApi->SubHisQuote(
			m_KContract.SubContractNo,
			KLineUtil::GetKLineType(),
			KLineUtil::GetKLineSlice(),
			KLineMaxCount,
			sessionID,
			m_clPreseter);
	}
	PostMessage(m_clPreseter->GetHwnd(), SSWM_HTTP_SPI, 0, 0);
}

string KLineModel::GetSymbolName(string symbol)
{
	string name = "";
	int pos = symbol.find('_');
	if (pos == string::npos)
		return "";

	symbol = symbol.substr(pos + 1, string::npos);
	int rpos = symbol.rfind('_');
	if (rpos == string::npos)
		return "";

	name = symbol.substr(0, rpos);
	return name;
}

void KLineModel::InitContractFromTCMap()
{
	ACDataMap::iterator iter;
	char key[256] = { 0 };
	sprintf_s(key, "%s|%s", m_KContract.commoditySymbol, m_KContract.pattern);
	if (m_TCDataMap.find(key) != m_TCDataMap.end())
	{
		return;
	}
	else
	{
		iter = m_TCDataMap.begin();
	}

	ACPatternParam paramData;
	bool hasContract = false;

	for (; iter != m_TCDataMap.end(); ++iter)
	{
		paramData = iter->second.m_PatternParam;
		string symbol = paramData.symbol;
		string symbolname = GetSymbolName(symbol);

		auto iter2 = m_clPreseter->m_mapSymbolNameContractInfo.find(symbolname);
		if (iter2 != m_clPreseter->m_mapSymbolNameContractInfo.end())
		{

			int ypos = symbol.find(iter2->second.years);
			if (ypos == string::npos)
				continue;

			int mpos = symbol.find(iter2->second.months);
			if (mpos == string::npos)
				continue;

			m_clPreseter->m_mapSymbolContract.emplace(make_pair(symbol, iter2->second.SubContractNo));

			if (!hasContract)
			{
				strcpy_s(m_KContract.SubContractNo, iter2->second.SubContractNo);
				strcpy_s(m_KContract.pattern, iter->second.m_PatternParam.pattern);
				strcpy_s(m_KContract.commoditySymbol, iter->second.m_sCommoditySymbol);
				m_KContract.resultuid = iter->second.m_nResultuid;

				SContract* contract[1] = { 0 };
				SContract* underLay = NULL;

				int icount = g_pQuoteApi->GetContractData("", m_KContract.SubContractNo, contract, 1, false);
				//区分主力合约和非主力合约
				if (icount)
				{
					underLay = g_pQuoteApi->GetContractUnderlay(contract[0]->ContractNo);
					if (underLay)
					{
						m_KContract.pContract = underLay;
					}
					else
					{
						m_KContract.pContract = contract[0];
					}
				}
				hasContract = true;
			}
		}
		else
		{
			continue;
		}
	}
}

void KLineModel::UpdateTotalData()
{
	if (g_pQuoteApi)
	{
		memset(&m_stTotalData, 0, sizeof(m_stTotalData));
		int timeIndex = 0;
		while (timeIndex < 300)
		{
			m_iCount = g_pQuoteApi->GetHisQuote(
				m_KContract.SubContractNo, 
				KLineUtil::GetKLineType(), 
				KLineUtil::GetKLineSlice(), 
				1, 0, 
				m_stTotalData, 
				KLineMaxCount);

			if (m_iCount)
			{
				break;
			}
			else
			{
				Sleep(100);
			}
			++timeIndex;
		}
		UpdateSubData();
		if (m_rcCenter.bottom)
		{
			m_clPreseter->UpdateKLineView(m_KContract);
		}
	}
}
void KLineModel::GetHttpData(AC_Service_Type service, GetHttpType type)
{
	ACUrlParams params;
	memset(&params, 0, sizeof(params));
	strcpy_s(params.sUsername, sizeof(params.sUsername), username);
	strcpy_s(params.sPassword, sizeof(params.sPassword), password);
	strcpy_s(params.sExchangeNo, sizeof(params.sExchangeNo), service);
	
	if (type == AsynGetAll) 
	{
		//异步请求全部数据
		m_httpTool.AsynGetAllInterNetURLText(params);
	}
	else if (type == SynGet) 
	{
		//同步请求数据,请求之前先将valid置为false
		m_httpData.m_bValid = false;
		m_httpTool.GetInterNetURLText(params, m_httpData);
		UpdateSubData(true);
		m_clPreseter->UpdateKLineView(m_KContract);
	}
	else if (type == AsynGet)
	{
		//异步请求单组数据
		m_httpData.m_bValid = false;
		m_httpTool.AsynGetInterNetURLText(params);
	}
}