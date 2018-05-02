#ifndef _TRADE_QUOTE_STRING_
#define _TRADE_QUOTE_STRING_

typedef	int N_ExchangeType;
#define N_ZCE	0	
#define N_DCE	1	
#define N_SHFE	2	
#define N_CFFEX	3	
#define N_NONE	4	

typedef int		N_SwitchType;
#define NST_NONE	0	//��ת��
#define NST_T2Q		1	//���׺�Լת�����Լ
#define NST_Q2T		2	//�����Լת���׺�Լ

typedef std::vector<const void *> VecVoidData;

class TDataContainer :public IDataContainer
{
public:
	//------------------------------�̳�begin--------------------------------------//
	virtual uint __cdecl size()
	{
		return m_VecData.size();
	}
	virtual const void * __cdecl at(uint i)
	{
		return m_VecData.at(i);
	}
	virtual void __cdecl clear()
	{
		m_VecData.clear();
	}
	virtual void __cdecl push_back(const void * data)
	{
		m_VecData.push_back(data);
	}
	//-------------------------------�̳�end---------------------------------------//

	//------------------------------begin--------------------------------------//
	VecVoidData & GetData()
	{
		return m_VecData;
	}
	//-------------------------------end---------------------------------------//
public:
	VecVoidData m_VecData;
};

typedef std::vector<int> VecIntData;

namespace CConvert
{
    //����IDת���׹ؼ���
    void SContractNoTypeToTradeContract(const char *szSign, const std::string &id, TContractKey &ContractKey);

    //����Ʒ������ת����
    TCommodityType QuoteTypeToTradeType(char cType);
    //����Ʒ������ת����
    TCommodityType TradeTypeToQuoteType(char cType);

    std::wstring UTF8ToUnicode(const std::string& str);

    //����IDתƷ�ֹؼ���
    void SContractNoTypeToTradeCommodity(const char *szSign, const std::string &id, TCommodityKey &CommodityKey);

	 //��ʼ��������Ϣ
	  void DefaultSendOrder(TSendOrder&SendOrder);//***
	 //LME�ж�
	  bool IsLME(const TExchangeKey&Exchange);
	 //�ж��Ƿ��Ǹ۽����ڻ�
	  bool IsHK(const char *szCode);//*
	  //�Ƿ�������
	  bool IsInnerDiskExchange(const char *szExchange);
	  //��ȡ����������
	  N_ExchangeType GetExchangeType(const char* sExchangeNo);
	  //�ж��Ƿ�����Ϻ�Լ
	  bool IsCombContract(const char CommodityType);
	  //�Ƿ���Ȩ��Լ
	  bool IsOptionContract(const char CommodityType);
	  //�Ƿ��������Լ
	  bool IsValidQuotContract(const  char * ContractId);
	  //�Ƿ�����Ч������Ʒ��
	  bool IsValidQuoteCommodity(const char * ExchangeNo,const char * CommodityNo);

	  //�ַ���ת���׺�Լ
	  bool TContractNoToTContractKey(const char * szCode, TContractKey & ContractKey);//��֤���������ַ����������˶�ȥ���ո���***

	  //���׺�Լת�ַ���
      bool TContractKeyToTContractNo(const TContractKey & ContractKey, std::string & strCode);//
	  //��Ȩ���׺�Լת��Ȩ����
	  bool TradeContractToOptionTarget(const TContractSingleKey &ContractKey, char(&ContractId)[101]);
	  //���Ƚ��׺�Լת�ַ��� Ĭ�ϲ���������
	  bool SingTradeContractToStr(std::string & strCode, const TContractSingleKey & SingContractKey, bool bHaveExchange = false, bool bHaveSign = true);
	  //���Ƚ��׺�Լת�ַ���
	  bool PosContractToStr(std::string & strCode, const TContractSingleKey & SingContractKey);
	  //��ʽ�����׺�Լ�ṹ��
	  void FormatTradeContract(const TContractKey & ContractKey, std::string &str);

      //����Ʒ��IDת���׹ؼ���
      bool SCommodityNoToTCommodityKey(SCommodityNoType commodityno, TCommodityKey &CommodityKey);

	  //���׺�Լת�����ַ���
	  void TradeContractToSContractNoType(const TContractKey &ContractKey, char(&ContractId)[101]);
	  //���׺�Լת��������ַ���
	  void TradeContractToRelateContractNoType(const TContractKey &ContractKey, char(&ContractId)[101]);
	  //�����Լת���׺�Լ�ַ���
	  void SContractNoTypeToStr(std::string &sQuoteCode, std::string &str);
	  //�����ַ���������id
	  bool StrToSContractNoType(const char *code, char(&ContractId)[101]);
	  //������Լ�ַ����ֽ�
	  bool SpreadStrToSpreadInfo(const char *code, char& cRateMode, char(&sFirst)[101], char(&sSecond)[101], int N_SwitchType = NST_NONE);//ntype :0:  1: 2:Q2T
	  //�����Ȩ�ֶ�
	  bool ParseOptionContractStr(const std::string& strValue, char(&ContractNo)[11], TOptionType &OptionType, char(&StrikePrice)[11]);

	  long Round(double Value);

	  //�Ƿ�����
	  bool IsDiv(const double Value, const double SubValue, const double ErrorDiff = 1e-10);
	  //����ƥ��
	  bool MatchPattern(const char *pattern, const char *target);
	  //�Ƿ���Ч������
	  bool IsValidNumber(const char * sValue, bool bOnlyInt = false, bool bCanNegative = false);
	  //С����ֵ�ж�
	  bool DoubleSameValue(const double d1, const double d2, double Epsilon);

	  std::vector<std::string> SplitString(const std::string & str, char cSplit);

	  void TrimString(std::string &str);

	  SPriceType GetPriceByContract(const SContract* SContract, SFidMeanType nField,int nlevel = 0);//����������

	  SQtyType GetQtyByContract(const SContract* SContract, SFidMeanType nField, int nlevel = 0);//����������

	  SPriceType GetPriceBySnapShot(const SQuoteSnapShot* QuoteSnap, SFidMeanType nField);//
	  SPriceType GetPriceBySnapShot(const SQuoteSnapShotL2 QuoteSnap[2], SFidMeanType nField, int nlevel = 0);//

	  SQtyType GetQtyBySnapShot(const SQuoteSnapShot* QuoteSnap, SFidMeanType nField);//
	  SQtyType GetQtyBySnapShot(const SQuoteSnapShotL2 QuoteSnap[2], SFidMeanType nField, int nlevel = 0);//

      
      
};
#endif