#pragma once

typedef std::multimap<std::string, ShiftListInfo> MULTIMAP;
typedef MULTIMAP::iterator CIT;
typedef pair<CIT, CIT> Range;
class ShiftQuoteApi/*: public ISQuoteDataSpi, public TIxFrm*/
{
public:
	ShiftQuoteApi();
	~ShiftQuoteApi();
	bool SendConditionOrder(ShiftCondition &Condition);
public:
	bool QuoteShiftContract(ShiftListInfo& ShiftContract,char* sign, char* sign2);
	void Regist(std::map<string, ShiftListInfo>* pShiftContract, std::map<int, ShiftCondition>* pShiftCondition);
	void DealOnQuote(WPARAM wParam);
private:
	bool SubQuote(string sCode1, string sCode2, char* Sign, char* Sing2);
	bool GetShiftContractNo(ShiftListInfo& ShiftContract);
	void GetSingleContractNo(vector<string> vecstr, char Condition, int Qty, char (&ContractNo)[25]);
	void GetQuoteData(SContract* pst);
	
	void InitShiftDiffPrice(double Price[2][7],  ShiftListInfo& shiftInfo);
	void UpdateShiftQuote(ShiftListInfo& shiftQuote, ShiftListInfo &shiftInfo);
	bool MonitorCondition(ShiftListInfo& shiftInfo);
	

private:
	std::map<string, ShiftListInfo>* m_mapShiftContract;//��¼�б����ʾ��Ϣ��
	std::map<int, ShiftCondition>* m_pmapShiftCondition;//��¼�������б���Ϣ;
	MULTIMAP m_multiQuoteContract;   //���ڻ�ȡ���ȵ��Ʋֺ�Լ�۸�


	//��һ����ۣ����ң����¼ۣ����ۣ����̣���ߣ���ͣ�
	enum{
		buy = 0, sell, last, average, opening, high, low
	};
};