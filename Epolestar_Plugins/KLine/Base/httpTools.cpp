#include "BaseInclude.h"
#define  BUF_SIZE 1024*5
string urlHead = "https://feed.tradingcentral.com/ws_ta.asmx/GetFeed?culture=";
string type_product = "type_product=";
string product = "product=";
string term = "term=";
string days = "days=";
string last_ta = "last_ta=";
string partner = "partner=";
string token = "token=";
bool httpTools::RegistHttpRecv(HttpReceiver recv)
{
	if (recv != NULL) {
		//给 httpTools::m_Crit 加锁
		TCriticalSection::Lock lock(m_Crit);
		m_HttpReceive = recv;
		return true;
	}
	return false;
}
bool httpTools::RegistHttpInitRecv(HttpInitReciver recv)
{
	if (recv != NULL) {
		TCriticalSection::Lock lock(m_Crit);
		m_HttpInitReciver = recv;
		return true;
	}
	return false;
}
void httpTools::AsynGetInterNetURLText(TCParams parames)
{
	char buff[sizeof(TCParams)];
	TCParams* data = (TCParams*)buff;
	memcpy(data, &parames, sizeof(parames));
	m_BoundeQuote.Put(buff, sizeof(TCParams));
}
void httpTools::AsynGetAllInterNetURLText()
{
	m_initIndex = 0;
	m_AllData = true;
	TCParams params;
	memset(&params, 0, sizeof(params));
	strcpy_s(params.product_type, sizeof(params.product_type), TC_typenull);
	strcpy_s(params.day_slice, sizeof(params.day_slice), "1");
	strcpy_s(params.last, sizeof(params.last), "true");
	strcpy_s(params.partnerNo, sizeof(params.partnerNo), TC_esunnyNo);
	strcpy_s(params.token, sizeof(params.token), TC_esunnyToken);
	//多语言
	if (g_language == CHS) {
		strcpy_s(params.culture, sizeof(params.culture), TC_zh_CN);
	}
	else if (g_language == CHT) {
		strcpy_s(params.culture, sizeof(params.culture), TC_zh_TW);
	}
	else if (g_language == ENU)
	{
		strcpy_s(params.culture, sizeof(params.culture), TC_en_US);
	}
	//四个不同周期
	strcpy_s(params.product, sizeof(params.product), "null");
	//高频 - 对应我们系统
	strcpy_s(params.term, sizeof(params.term), TC_15Min);
	AsynGetInterNetURLText(params);
	//每日
	strcpy_s(params.term, sizeof(params.term), TC_intraday);
	AsynGetInterNetURLText(params);
	//短期
	strcpy_s(params.term, sizeof(params.term), TC_shortTerm);
	AsynGetInterNetURLText(params);
	//中期
	strcpy_s(params.term, sizeof(params.term), TC_midTerm);
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
			TCParams* data = (TCParams*)&buf[pos];
			TCSpiData spi;
			spi.InitData();
			GetInterNetURLText(*data, spi.data , spi.text);
			pos += sizeof(TCParams);
			if (!m_AllData) {
				m_HttpReceive(spi.data, spi.text);
			}
		}
	}
}
void httpTools::InitURL(TCParams params, char* lpcInterNetURL, size_t size)
{
	sprintf_s(lpcInterNetURL,size, "%s%s&%s%s&%s%s&%s%s&%s%s&%s%s&%s%s&%s%s",
		urlHead.c_str(), 
		params.culture,
		type_product.c_str(), 
		params.product_type,
		product.c_str(), 
		params.product,
		term.c_str(), 
		params.term,
		days.c_str(), 
		params.day_slice,
		last_ta.c_str(), 
		params.last,
		partner.c_str(), 
		params.partnerNo,
		token.c_str(), 
		params.token);
}
void httpTools::GetInterNetURLText(TCParams params, TCParamSpiData& data, IntroductText& introduct)
{
	char lpcInterNetURL[BUF_SIZE] = { 0 };
	std::string str;
	InitURL(params, lpcInterNetURL, sizeof(lpcInterNetURL));
	char buff[BUF_SIZE] = { 0 };
	HINTERNET hSession;
	hSession = InternetOpen(L"WinInet", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if (hSession != NULL) {
		HINTERNET hRequest;
		hRequest = InternetOpenUrlA(hSession, lpcInterNetURL, NULL, 0, INTERNET_FLAG_RELOAD, 0);
		if (hRequest != NULL) {
			//采用循环读取方式 DWORD readed = 0;
			char buffer[BUF_SIZE];
			DWORD readed;
			do {
				ZeroMemory(buffer, BUF_SIZE);
				InternetReadFile(hRequest, buffer, BUF_SIZE -1, &readed);
				string text = ConvertUTF8toGB2312(buffer, BUF_SIZE - 1);
				buffer[readed] = '\0';
				str += text;
			} while (readed);
		}
		InternetCloseHandle(hRequest);
	}
	InternetCloseHandle(hSession);
	if (m_AllData) {
		GetAllXMLInfo(str);
	}
	else {
		GetXMLInfo(str.c_str(), data, introduct);
	}
	
}
void httpTools::GetAllXMLInfo(std::string str)
{
	using namespace pugi;
	TCSpiData spi;
	xml_document doc;
	doc.load_string(str.c_str());
	xml_node items = doc.child("items");
	if (items) {
		xml_node article = items.child("article");
		if (article == NULL) {
			m_initIndex++;
		}
		for (; article; article = article.next_sibling("article"))
		{
			InitArticleNode(article, spi.data, spi.text);
			char key[256] = { 0 };
			sprintf_s(key, "%s|%s", spi.data.ticker, spi.data.term);
			m_TCDataMap[key] = spi;
		}
		if (!strcmp(spi.data.term, TC_15Min)){
			m_initIndex++;
		}else if (!strcmp(spi.data.term, TC_intraday))
		{
			m_initIndex++;
		}else if (!strcmp(spi.data.term, TC_shortTerm))
		{
			m_initIndex++;
		}else if (!strcmp(spi.data.term, TC_midTerm))
		{
			m_initIndex++;
		}
		if (m_initIndex == 4 || m_initIndex == 3) {
			m_HttpInitReciver(m_TCDataMap);
		}
	}
}
void httpTools::InitArticleNode(pugi::xml_node article, TCParamSpiData& data, IntroductText& introduct)
{
	using namespace pugi;
	if (!article)
		return;
	data.articleID = article.attribute("id").as_int();

	xml_node analysis = article.child("analysis");
	xml_node content = analysis.child("content");
	xml_node header = content.child("header");
	xml_node story = content.child("story");

	//时间、品种名称、周期等
	xml_node date = header.child("date");
	xml_node hour = header.child("hour");
	xml_node author = header.child("author");
	xml_node media = header.child("media");
	xml_node ISIN = header.child("code");
	xml_node ticker = ISIN.next_sibling();
	xml_node RIC = ticker.next_sibling();
	xml_node name = header.child("name");
	xml_node product = header.child("product");
	xml_node term = header.child("term");
	strcpy_s(data.date, sizeof(data.date), date.text().as_string());
	strcpy_s(data.hour, sizeof(data.hour), hour.text().as_string());
	KLineUtil::TCTimeToQuoteTime(data.date, sizeof(data.date), data.hour, sizeof(data.hour), data.dateTime);
	strcpy_s(data.media, sizeof(data.media), media.text().as_string());
	strcpy_s(data.ISIN, sizeof(data.ISIN), ISIN.text().as_string());
	strcpy_s(data.ticker, sizeof(data.ticker), ticker.text().as_string());
	strcpy_s(data.RIC, sizeof(data.RIC), RIC.text().as_string());
	strcpy_s(data.name, sizeof(data.name), name.text().as_string());
	strcpy_s(data.product, sizeof(data.product), product.text().as_string());
	strcpy_s(data.term, sizeof(data.term), term.text().as_string());

	//趋势
	xml_node option = header.child("option");
	xml_node optionST = option.child("watch");
	xml_node deltaST = optionST.next_sibling();
	xml_node optionMT = deltaST.next_sibling();
	xml_node deltaMT = optionMT.next_sibling();
	data.optionST = atoi(optionST.text().as_string());
	data.deltaSt = atoi(deltaST.text().as_string());
	data.optionMT = atoi(optionMT.text().as_string());
	data.deltaMt = atoi(deltaMT.text().as_string());

	//压力支撑线
	xml_node chartlevels = option.child("chartlevels");
	xml_node res3 = chartlevels.child("resistance3");
	xml_node res2 = res3.next_sibling();
	xml_node res1 = res2.next_sibling();
	xml_node last = res1.next_sibling();
	xml_node pivot = last.next_sibling();
	xml_node sup1 = pivot.next_sibling();
	xml_node sup2 = sup1.next_sibling();
	xml_node sup3 = sup2.next_sibling();
	data.res3 = res3.text().as_double();
	data.res2 = res2.text().as_double();
	data.res1 = res1.text().as_double();
	data.pivot = pivot.text().as_double();
	data.sup1 = sup1.text().as_double();
	data.sup2 = sup2.text().as_double();
	data.sup3 = sup3.text().as_double();

	//初始化story信息
	introduct.clear();
	xml_node keywords = story.child("keywords");
	char key[256] = { 0 };
	if (keywords) {
		//显示更新日期
		sprintf_s(key, "%s  Updated Date:%s %s", keywords.text().as_string(), data.date, data.hour);
		introduct.push_back(key);
	}
	xml_node title = story.child("title");
	if (title) {
		introduct.push_back(title.text().as_string());
	}
	xml_node summary = story.child("summary");
	if (summary) {
		introduct.push_back(summary.text().as_string());
	}
	for (xml_node paragragh = story.child("paragraph"); paragragh; paragragh = paragragh.next_sibling())
	{
		introduct.push_back(paragragh.text().as_string());
	}
	data.valid = true;
}
void httpTools::GetXMLInfo(const char* buf, TCParamSpiData& data, IntroductText& introduct)
{
	using namespace pugi;
	memset(&data, sizeof(TCParamSpiData), 0);
	xml_document doc;
	doc.load_string(buf);
	xml_node items = doc.child("items");
	introduct.clear();
	if (items == NULL) {
		//错误信息
		xml_node error = doc.child("errors");
		if (error) {
			data.valid = false;
			xml_node error_message = error.child("error_message");
			strcpy_s(data.error_message, sizeof(data.error_message), error_message.text().as_string());
			introduct.push_back(error_message.text().as_string());
		}
	}
	else {
		xml_node article = items.child("article");
		if (article) {
			InitArticleNode(article, data, introduct);
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