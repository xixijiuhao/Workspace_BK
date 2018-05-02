#pragma once
#include "Protocol.h"
//回调函数
typedef std::map<std::string, TCSpiData> TCDataMap;
typedef std::function<void(TCParamSpiData, IntroductText)> HttpReceiver;
typedef std::function<void(TCDataMap)> HttpInitReciver;
class httpTools : public TThread
{
public :
	httpTools() { m_AllData = false; };
	~httpTools() {};
public:
	//异步获取数据
	void AsynGetInterNetURLText(TCParams parames);
	//同步获取数据
	void GetInterNetURLText(TCParams params, TCParamSpiData& data, IntroductText& story);
	//异步获取全部数据（暂时采用这种方式）
	void AsynGetAllInterNetURLText();
	//注册回调
	bool RegistHttpRecv(HttpReceiver recv);
	bool RegistHttpInitRecv(HttpInitReciver recv);
protected:
	virtual void Run();
private:
	void InitURL(TCParams params, char* url, size_t size);
	void GetXMLInfo(const char* buf, TCParamSpiData& data, IntroductText& story);
	void GetAllXMLInfo(std::string str);
	void InitArticleNode(pugi::xml_node article, TCParamSpiData& data, IntroductText& introduct);
	std::string ConvertUTF8toGB2312(const char* pData, size_t size);
private:
	int						m_initIndex;
	bool					m_AllData;
	TCriticalSection		m_Crit;
	HttpReceiver			m_HttpReceive;
	HttpInitReciver			m_HttpInitReciver;
	//消息队列
	TBoundedQueue			m_BoundeQuote;
	//保存全部TC数据（Key = "Ticker|TC_Term"）
	TCDataMap				m_TCDataMap;
};
