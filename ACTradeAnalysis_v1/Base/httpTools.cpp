#include "BaseInclude.h"

#define  BUF_SIZE  1024 * 512
//"http://china.autochartist.com/aclite/CHINAFUTURESDEMOXMLAPI?username=cfddcejk01&password=!20180516DceKJ01&exchange=CZCE"
string urlHead = "http://china.autochartist.com/aclite/CHINAFUTURESDEMOXMLAPI?";
string urlUsername = "username=";
string urlPassword = "password=";
string urlExchange = "exchange=";
string urlRequest = "request=";
using namespace pugi;

inline I32 GetIntValue(xml_node &node)
{
	if (node)
	{
		return atoi(node.text().as_string());
	}
	return 0;
}

inline string GetSTRValue(xml_node &node)
{
	if (node)
	{
		return node.text().as_string();
	}
	return "";
}

inline double GetDoubleValue(xml_node &node)
{
	if (node)
	{
		return node.text().as_double();
	}
	return 0.0f;
}

bool httpTools::RegistHttpRecv(HttpReceiver recv)
{
	if (recv != NULL) 
	{
		//给 httpTools::m_Crit 加锁
		TCriticalSection::Lock lock(m_Crit);
		m_HttpReceive = recv;
		return true;
	}
	return false;
}

bool httpTools::RegistHttpInitRecv(HttpInitReciver recv)
{
	if (recv != NULL) 
	{
		TCriticalSection::Lock lock(m_Crit);
		m_HttpInitReciver = recv;
		return true;
	}
	return false;
}

void httpTools::AsynGetInterNetURLText(ACUrlParams parames)
{
	char buff[sizeof(ACUrlParams)];
	ACUrlParams* data = (ACUrlParams*)buff;
	memcpy(data, &parames, sizeof(parames));
	m_BoundeQuote.Put(buff, sizeof(ACUrlParams));
}

void httpTools::AsynGetAllInterNetURLText(ACUrlParams &params)
{
	m_initIndex = 0;
	m_AllData = true;

	AsynGetInterNetURLText(params);
}

void httpTools::Run()
{
	//isTerminated() 判断TThread：：m_Terminated是否结束，如果没有结束一直循环
	while (!isTerminated())
	{
		char* buf;
		int len;
		m_BoundeQuote.Take(buf, len);
		int pos(0);
		while (pos < len)
		{	
			ACUrlParams* data = (ACUrlParams*)&buf[pos];
			ACSpiData spi;
			spi.InitData();
			GetInterNetURLText(*data, spi);
			pos += sizeof(ACUrlParams);
			if (!m_AllData) 
			{
				m_HttpReceive(spi);
			}
		}
	}
}

void httpTools::InitURL(ACUrlParams params, std::vector<std::string> &interNetURLVec)
{
	//拼出 图标分析、大波动、连续性的分析
	char tradeInterNetURL[500] = { 0 };
	sprintf_s(tradeInterNetURL,sizeof(tradeInterNetURL), "%s%s%s&%s%s&%s%s",
		urlHead.c_str(), 
		urlUsername.c_str(),
		params.sUsername,
		urlPassword.c_str(), 
		params.sPassword,
		urlExchange.c_str(), 
		params.sExchangeNo
	);
	interNetURLVec.push_back(tradeInterNetURL);

	char bigmovementInterNetURL[500] = { 0 };
	sprintf_s(bigmovementInterNetURL, sizeof(bigmovementInterNetURL), "%s%s%s&%s%s&%s%s&%s%s",
		urlHead.c_str(),
		urlUsername.c_str(),
		params.sUsername,
		urlPassword.c_str(),
		params.sPassword,
		urlExchange.c_str(),
		params.sExchangeNo,
		urlRequest.c_str(),
		AC_BigMovement
	);
	interNetURLVec.push_back(bigmovementInterNetURL);

	char consecutiveInterNetURL[500] = { 0 };
	sprintf_s(consecutiveInterNetURL, sizeof(consecutiveInterNetURL), "%s%s%s&%s%s&%s%s&%s%s",
		urlHead.c_str(),
		urlUsername.c_str(),
		params.sUsername,
		urlPassword.c_str(),
		params.sPassword,
		urlExchange.c_str(),
		params.sExchangeNo,
		urlRequest.c_str(),
		AC_ConsecutiveCandles
	);
	interNetURLVec.push_back(consecutiveInterNetURL);
}

std::string httpTools::GetURLInfo(const char* szUrl)
{
	HANDLE hFile = INVALID_HANDLE_VALUE;
	HINTERNET hInet = NULL;
	HINTERNET hUrl = NULL;

	DWORD dwBuf = 1024 * 1024, dwRead = 0; //1M
	auto_ptr<char> szBuf(new char[dwBuf]);
	memset(szBuf.get(), 0, dwBuf);
	std::string strTmp = "";
	bool bRet = false;

	try
	{
		hInet = InternetOpen(NULL, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);

		if (hInet == NULL)
			throw "error";

		hUrl = InternetOpenUrlA(hInet, szUrl, NULL, 0,
			INTERNET_FLAG_TRANSFER_BINARY | INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_RELOAD, 0);
		if (hUrl == NULL)
			throw "error";

		for (;;)
		{
			if (!InternetReadFile(hUrl, szBuf.get(), dwBuf, &dwRead))
			{
				bRet = false;
				break;
			}

			if (dwRead == 0)
			{
				bRet = true;
				break;
			}

			strTmp += std::string(szBuf.get(),dwRead);
		}

		throw "ok";
	}
	catch (...)
	{
		if (hFile != INVALID_HANDLE_VALUE)
			CloseHandle(hFile);
		if (hUrl != NULL)
			InternetCloseHandle(hUrl);
		if (hInet != NULL)
			InternetCloseHandle(hInet);
	}

	return bRet ? strTmp : "";
}

void httpTools::GetInterNetURLText(ACUrlParams params, ACSpiData& data)
{
	char lpcInterNetURL[500] = { 0 };
	std::vector<std::string> interNetUrlVec;
	InitURL(params, interNetUrlVec);
	std::vector<std::string> strXmlRequest;

	HINTERNET hSession;
	hSession = InternetOpen(NULL, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if (hSession != NULL) 
	{
		for (auto iter : interNetUrlVec)
		{
			HINTERNET hRequest;
			std::string str = "";
			strcpy_s(lpcInterNetURL,iter.c_str());
			hRequest = InternetOpenUrlA(hSession, lpcInterNetURL, NULL, 0, INTERNET_FLAG_RELOAD, 0);
			if (hRequest != NULL)
			{
				//采用循环读取方式
				DWORD readed = 0;
				char buffer[BUF_SIZE] = {};
				do {
					memset(buffer, 0, BUF_SIZE);
					if (InternetReadFile(hRequest, buffer, BUF_SIZE - 1, &readed))
					{
						string text = std::string(buffer, readed);
						buffer[readed] = '\0';
						str += text;
					}
				} while (readed);

				strXmlRequest.push_back(str);
			}
			InternetCloseHandle(hRequest);
		}
	}
	InternetCloseHandle(hSession);

	if (m_AllData) 
	{
		GetAllXMLInfo(strXmlRequest);
	}
	else
	{
		//GetXMLInfo(str.c_str(), data);
	}
	
}
void httpTools::GetAllXMLInfo(std::vector<std::string> &strXmlRequest)
{
	using namespace pugi;
	
	if (strXmlRequest.size() < 3)
		return;
	m_TCDataMap.clear();
	m_BigMovementMap.clear();
	m_ConsecutiveMap.clear();

	std::string tradeStr = strXmlRequest.at(0);
	GetTradeMapData(tradeStr);
	
	std::string bigmovementStr = strXmlRequest.at(1);
	GetBigMovementMapData(bigmovementStr);

	std::string consecutiveStr = strXmlRequest.at(2);
	GetConsecutiveMapData(consecutiveStr);

	//将大波动/连续性的分析放入对应的结构
	if (m_TCDataMap.size())
	{
		STR50 symbol = {};
		for (auto acIter = m_TCDataMap.begin();acIter != m_TCDataMap.end();++acIter)
		{
			int pos = acIter->first.find('|');
			if (pos != std::string::npos)
			{
				string str = acIter->first.substr(0, pos);
				strcpy_s(symbol, str.c_str());
			}
			else
			{
				continue;
			}

			auto iterBM = m_BigMovementMap.find(symbol);
			if (iterBM != m_BigMovementMap.end())
			{
				acIter->second.m_BigMovementParamVec = iterBM->second;
			}

			auto iterCC = m_ConsecutiveMap.find(symbol);
			if (iterCC != m_ConsecutiveMap.end())
			{
				acIter->second.m_ConsecutiveParamVec = iterCC->second;
			}
		}
		m_HttpInitReciver(m_TCDataMap);
	}
}

void httpTools::GetTradeMapData(std::string &tradeStr)
{
	ACSpiData spi;
	xml_document doc;
	doc.load_string(tradeStr.c_str());
	xml_node items = doc.child("AutochartistAPI");
	if (items)
	{
		xml_node patternNode = items.child("ChartPatternResult");
		if (patternNode == NULL)
		{
			return;
		}
		else
		{
			char key[256] = { 0 };
			for (; patternNode; patternNode = patternNode.next_sibling())
			{
				InitArticleNode(patternNode, spi);
				sprintf_s(key, "%s|%s", spi.m_sCommoditySymbol, spi.m_PatternParam.pattern);
				m_TCDataMap.emplace(make_pair(key, spi));
			}
		}
	}
}

void httpTools::GetBigMovementMapData(std::string &bigmovementStr)
{
	ACBigMovementParam data;
	xml_document doc;
	doc.load_string(bigmovementStr.c_str());
	xml_node items = doc.child("AutochartistAPI");
	if (items)
	{
		xml_node patternNode = items.child("BigMovementResult");
		if (patternNode == NULL)
		{
			return;
		}
		else
		{
			for (; patternNode; patternNode = patternNode.next_sibling())
			{
				InitBigMovementArticleNode(patternNode, data);
				m_BigMovementMap[data.m_sCommoditySymbol].push_back(data);
			}
		}
	}
}

void httpTools::GetConsecutiveMapData(std::string &consecutiveStr)
{
	ACConsecutiveCandlesParam data;
	xml_document doc;
	doc.load_string(consecutiveStr.c_str());
	xml_node items = doc.child("AutochartistAPI");
	if (items)
	{
		xml_node patternNode = items.child("ConsecutiveCandlesResult");
		if (patternNode == NULL)
		{
			return;
		}
		else
		{
			for (; patternNode; patternNode = patternNode.next_sibling())
			{
				InitConsecutiveArticleNode(patternNode, data);
				m_ConsecutiveMap[data.m_sCommoditySymbol].push_back(data);
			}
		}
	}
}

void httpTools::InitBigMovementArticleNode(pugi::xml_node article, ACBigMovementParam& data)
{
	if (!article)
		return;
	xml_node symbol = article.child("symbol");
	xml_node patternendtime = article.child("patternendtime");
	xml_node patternstarttime = article.child("patternstarttime");
	xml_node direction = article.child("direction");

	strcpy_s(data.m_sCommoditySymbol, GetSTRValue(symbol).c_str());
	data.patternendtime = GetIntValue(patternendtime);
	data.patternstarttime = GetIntValue(patternstarttime);
	data.direction = GetIntValue(direction);
}

void httpTools::InitConsecutiveArticleNode(pugi::xml_node article, ACConsecutiveCandlesParam& data)
{
	if (!article)
		return;

	xml_node symbol = article.child("symbol");
	xml_node patternendtime = article.child("patternendtime");
	xml_node patternstarttime = article.child("patternstarttime");
	xml_node direction = article.child("direction");

	strcpy_s(data.m_sCommoditySymbol, GetSTRValue(symbol).c_str());
	data.patternendtime = GetIntValue(patternendtime);
	data.patternstarttime = GetIntValue(patternstarttime);
	data.direction = GetIntValue(direction);
}
void httpTools::InitArticleNode(pugi::xml_node article, ACSpiData& data)
{
	if (!article)	
		return;
	string strtmp = article.name();
	if (strcmp(strtmp.c_str(), "ChartPatternResult") == 0)
	{
		strcpy_s(data.m_sACPattern, "ChartPattern");

		//ChartPatternParam
		xml_node trend = article.child("trend");
		xml_node initialtrend = article.child("initialtrend");
		xml_node volumeincrease = article.child("volumeincrease");
		xml_node uniformity = article.child("uniformity");
		xml_node clarity = article.child("clarity");
		xml_node breakout = article.child("breakout");

		xml_node resistancex0 = article.child("resistancex0");
		xml_node resistancex1 = article.child("resistancex1");
		xml_node resistancey0 = article.child("resistancey0");
		xml_node resistancey1 = article.child("resistancey1");
		xml_node supportx0 = article.child("supportx0");
		xml_node supportx1 = article.child("supportx1");
		xml_node supporty0 = article.child("supporty0");
		xml_node supporty1 = article.child("supporty1");
				
		strcpy_s(data.m_PatternParam.trend, GetSTRValue(trend).c_str());
		data.m_PatternParam.initialtrend = GetIntValue(initialtrend);
		data.m_PatternParam.volumeincrease = GetDoubleValue(volumeincrease);
		data.m_PatternParam.uniformity = GetIntValue(uniformity);
		data.m_PatternParam.clarity_c = GetIntValue(clarity);
		data.m_PatternParam.breakout = GetIntValue(breakout);

		data.m_PatternParam.resistancex0 = GetIntValue(resistancex0);
		data.m_PatternParam.resistancex1 = GetIntValue(resistancex1);
		data.m_PatternParam.resistancey0 = GetDoubleValue(resistancey0);
		data.m_PatternParam.resistancey1 = GetDoubleValue(resistancey1);
		data.m_PatternParam.supportx0 = GetIntValue(supportx0);
		data.m_PatternParam.supportx1 = GetIntValue(supportx1);
		data.m_PatternParam.supporty0 = GetDoubleValue(supporty0);
		data.m_PatternParam.supporty1 = GetDoubleValue(supporty1);


	}
	else if (strcmp(strtmp.c_str(), "FibonacciPatternResult") == 0)
	{
		strcpy_s(data.m_sACPattern, "FibonacciPattern");

		//FibonacciPatternParam
		xml_node timesymmetry = article.child("timesymmetry");
		xml_node pricesymmetry = article.child("pricesymmetry");
		xml_node clarity = article.child("clarity");
		xml_node target0 = article.child("target0");
		xml_node target3 = article.child("target3");
		xml_node target5 = article.child("target5");
		xml_node target6 = article.child("target6");
		xml_node target7 = article.child("target7");
		xml_node target10 = article.child("target10");
		xml_node target12 = article.child("target12");
		xml_node target16 = article.child("target16");

		xml_node priceA = article.child("priceA");
		xml_node priceB = article.child("priceB");
		xml_node priceC = article.child("priceC");
		xml_node priceD = article.child("priceD");
		xml_node priceX = article.child("priceX");
		xml_node timeA = article.child("timeA");
		xml_node timeB = article.child("timeB");
		xml_node timeC = article.child("timeC");
		xml_node timeD = article.child("timeD");
		xml_node timeX = article.child("timeX");

		data.m_PatternParam.timesymmetry = GetIntValue(timesymmetry);
		data.m_PatternParam.pricesymmetry = GetIntValue(pricesymmetry);
		data.m_PatternParam.clarity_c = GetIntValue(clarity);
		data.m_PatternParam.target0 = GetDoubleValue(target0);
		data.m_PatternParam.target3 = GetDoubleValue(target3);
		data.m_PatternParam.target5 = GetDoubleValue(target5);
		data.m_PatternParam.target6 = GetDoubleValue(target6);
		data.m_PatternParam.target7 = GetDoubleValue(target7);
		data.m_PatternParam.target10 = GetDoubleValue(target10);
		data.m_PatternParam.target12 = GetDoubleValue(target12);
		data.m_PatternParam.target16 = GetDoubleValue(target16);

		data.m_PatternParam.priceA = GetDoubleValue(priceA);
		data.m_PatternParam.priceB = GetDoubleValue(priceB);
		data.m_PatternParam.priceC = GetDoubleValue(priceC);
		data.m_PatternParam.priceD = GetDoubleValue(priceD);
		data.m_PatternParam.priceX = GetDoubleValue(priceX);
		data.m_PatternParam.timeA = GetIntValue(timeA);
		data.m_PatternParam.timeB = GetIntValue(timeB);
		data.m_PatternParam.timeC = GetIntValue(timeC);
		data.m_PatternParam.timeD = GetIntValue(timeD);
		data.m_PatternParam.timeX = GetIntValue(timeX);
	}
	else if (strcmp(strtmp.c_str(), "KeyLevelsPatternResult") == 0)
	{
		strcpy_s(data.m_sACPattern, "KeyLevelsPattern");

		//KeyLevelsPatternParam
		xml_node price = article.child("price");
		//xml_node predictionpricefrom = article.child("predictionpricefrom");
		xml_node point1 = article.child("point1");

		data.m_PatternParam.price = GetDoubleValue(price);
		//data.m_PatternParam.predictionpricefrom_k = GetDoubleValue(predictionpricefrom);
		data.m_PatternParam.point1 = GetIntValue(point1);
	}

	strcpy_s(data.m_sPlateName, "Inner");

	xml_node symbol = article.child("symbol");
	xml_node resultuid = article.child("resultuid");
	xml_node pattern = article.child("pattern");
	xml_node patternimageurl = article.child("patternimageurl");

	data.m_nResultuid = GetIntValue(resultuid);
	strcpy_s(data.m_sCommoditySymbol, GetSTRValue(symbol).c_str());
	//strcpy_s(data.m_sACPattern, GetSTRValue(pattern).c_str());
	strcpy_s(data.m_sPatternImageUrl, GetSTRValue(patternimageurl).c_str());
	
	xml_node exchange			= article.child("exchange");
	xml_node interval			= article.child("interval");
	xml_node length				= article.child("length");
	xml_node direction			= article.child("direction");
	xml_node identified_time	= article.child("patternendtime");
	xml_node quality			= article.child("quality");
	xml_node completed			= article.child("completed");

	xml_node predictionpricefrom = article.child("predictionpricefrom");
	xml_node predictionpriceto = article.child("predictionpriceto");
	xml_node predictiontimeto = article.child("predictiontimeto");
	xml_node predictiontimefrom = article.child("predictiontimefrom");
	
	strcpy_s(data.m_PatternParam.exchange, GetSTRValue(exchange).c_str()) ;
	strcpy_s(data.m_PatternParam.symbol, GetSTRValue(symbol).c_str());
	data.m_PatternParam.interval = GetIntValue(interval);
	strcpy_s(data.m_PatternParam.pattern, GetSTRValue(pattern).c_str());
	data.m_PatternParam.length = GetIntValue(length);
	data.m_PatternParam.direction = GetIntValue(direction);
	data.m_PatternParam.identified_time = GetIntValue(identified_time);
	data.m_PatternParam.quality = GetIntValue(quality);
	data.m_PatternParam.completed = GetIntValue(completed);

	data.m_PatternParam.predictionpricefrom = GetDoubleValue(predictionpricefrom);
	data.m_PatternParam.predictionpriceto = GetDoubleValue(predictionpriceto);
	data.m_PatternParam.predictiontimeto = GetIntValue(predictiontimeto);
	data.m_PatternParam.predictiontimefrom = GetIntValue(predictiontimefrom);
}
void httpTools::GetXMLInfo(const char* buf, ACSpiData& data)
{
	using namespace pugi;
	memset(&data, sizeof(ACPatternParam), 0);
	xml_document doc;
	doc.load_string(buf);
	xml_node items = doc.child("AutochartistAPI");

	if (items == NULL) 
	{
		data.m_bValid = false;
		return;
	}
	else
	{
		xml_node result = items.child("ChartPatternResult");
		if (!result)
		{
			data.m_bValid = false;
			return;
		}
		else
		{
			InitArticleNode(result, data);
		}
	}
}

string httpTools::ConvertUTF8toGB2312(const char* pData, size_t size)
{
	size_t n = MultiByteToWideChar(CP_UTF8, 0, pData, (int)size, NULL, 0);
	WCHAR*   pChar = new  WCHAR[n + 1];
	memset(pChar, 0, n + 1);
	n = MultiByteToWideChar(CP_UTF8, 0, pData, (int)size, pChar, n);
	pChar[n] = 0;
	n = WideCharToMultiByte(936, 0, pChar, -1, 0, 0, 0, 0);
	char *p = new char[n + 1];
	n = WideCharToMultiByte(936, 0, pChar, -1, p, (int)n, 0, 0);
	string result(p);
	delete[]pChar;
	delete[]p;
	return result;
}
