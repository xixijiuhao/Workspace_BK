#pragma once
#include "Protocol.h"
//�ص�����
typedef std::map<std::string, ACSpiData> ACDataMap;
typedef std::map<std::string, ACDataMap> ExchangeNoACDataMap;
typedef std::vector<ACConsecutiveCandlesParam> ACConsecutiveVec;
typedef std::map<std::string, ACConsecutiveVec> ACConsecutiveDataMap;
typedef std::vector<ACBigMovementParam> ACBigMovementVec;
typedef std::map<std::string, ACBigMovementVec> ACBigMovementDataMap;
typedef std::function<void(ACSpiData)> HttpReceiver;
//typedef std::function<void(ACDataMap, ACBigMovementDataMap, ACConsecutiveDataMap)> HttpInitReciver;
typedef std::function<void(ACDataMap)> HttpInitReciver; 
class httpTools : public TThread
{
public :
	httpTools() : m_Mutex()
	{ 
		m_AllData = false; 
	}
	~httpTools() {};
public:
	//�첽��ȡ����
	void AsynGetInterNetURLText(ACUrlParams parames);
	//ͬ����ȡ����
	void GetInterNetURLText(ACUrlParams params, ACSpiData& data);
	//�첽��ȡȫ�����ݣ���ʱ�������ַ�ʽ��
	void AsynGetAllInterNetURLText(ACUrlParams &params);

	//ע��ص�
	bool RegistHttpRecv(HttpReceiver recv);
	bool RegistHttpInitRecv(HttpInitReciver recv);
protected:
	virtual void Run();
private:
	void InitURL(ACUrlParams params, std::vector<std::string> &interNetURLVec);
	void GetXMLInfo(const char* buf, ACSpiData& data);
	void GetAllXMLInfo(std::vector<std::string> &strXmlRequest);

	void InitArticleNode(pugi::xml_node article, ACSpiData& data);
	void InitBigMovementArticleNode(pugi::xml_node article, ACBigMovementParam& data);
	void InitConsecutiveArticleNode(pugi::xml_node article, ACConsecutiveCandlesParam& data);

	std::string ConvertUTF8toGB2312(const char* pData, size_t size);
	std::string GetURLInfo(const char* szUrl);

	void GetTradeMapData(std::string &tradeStr);
	void GetBigMovementMapData(std::string &bigmovementStr);
	void GetConsecutiveMapData(std::string &consecutiveStr);

private:
	int						m_initIndex;
	bool					m_AllData;
	TCriticalSection		m_Crit;
	HttpReceiver			m_HttpReceive;
	HttpInitReciver			m_HttpInitReciver;
	//��Ϣ����
	TBoundedQueue			m_BoundeQuote;
	//����ȫ��TC���ݣ�Key = "Ticker|TC_Term"��
	ACDataMap				m_TCDataMap;
	ACConsecutiveDataMap    m_ConsecutiveMap;
	ACBigMovementDataMap    m_BigMovementMap;

	mutable std::mutex m_Mutex;
};
