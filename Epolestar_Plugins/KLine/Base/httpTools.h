#pragma once
#include "Protocol.h"
//�ص�����
typedef std::map<std::string, TCSpiData> TCDataMap;
typedef std::function<void(TCParamSpiData, IntroductText)> HttpReceiver;
typedef std::function<void(TCDataMap)> HttpInitReciver;
class httpTools : public TThread
{
public :
	httpTools() { m_AllData = false; };
	~httpTools() {};
public:
	//�첽��ȡ����
	void AsynGetInterNetURLText(TCParams parames);
	//ͬ����ȡ����
	void GetInterNetURLText(TCParams params, TCParamSpiData& data, IntroductText& story);
	//�첽��ȡȫ�����ݣ���ʱ�������ַ�ʽ��
	void AsynGetAllInterNetURLText();
	//ע��ص�
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
	//��Ϣ����
	TBoundedQueue			m_BoundeQuote;
	//����ȫ��TC���ݣ�Key = "Ticker|TC_Term"��
	TCDataMap				m_TCDataMap;
};
