#ifndef _TRADE_QUOTE_STRING_
#define _TRADE_QUOTE_STRING_

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

class TVecInt:public IVecInt
{
public:
	//------------------------------�̳�begin--------------------------------------//
	virtual unsigned int __cdecl size()
	{
		return m_VecData.size();
	}
	virtual int __cdecl at(unsigned int i)
	{
		return m_VecData.at(i);
	}
	virtual void __cdecl clear()
	{
		m_VecData.clear();
	}
	virtual void __cdecl push_back(int data)
	{
		m_VecData.push_back(data);
	}
	//-------------------------------�̳�end---------------------------------------//

	//------------------------------begin--------------------------------------//
	VecIntData & GetData()
	{
		return m_VecData;
	}
	//-------------------------------end---------------------------------------//
public:
	VecIntData m_VecData;
};
//Ʒ����Ϣ
struct SOFCommodityInf
{
	char		Sign[21];
	bool		bCanNegative;					//С��ʱ�Ƿ��ܹ�Ϊ���� 
	bool		bIfFractionPrice;				//�Ƿ��������
	char		cCoverMode;						//���ݴ��������Ƿ���ʾ��ƽ����
	double		dMiniChangePrice;				//bIfFractionPrice��Ϊ1��������ķ���
	int			iPrecision;						//bIfFractionPrice��Ϊ1��������ķ�ĸ
	uint		cUserIdentity;					//���׵�½�û���ݣ����ڵ�½�ص������ͳ�ʼ�������и�ֵ,Ĭ��Ϊ���ͻ�(uiUser) �µ���(uiProxy) ������(uiBroker) ����Ա(uiTrader) ����ͻ�(uiQUser)
	bool		bAddOne;
	double		ProductDot;						//ÿ�ֳ���
public:
	SOFCommodityInf()
	{
		Default();
	}
	void Default()
	{
		memset(this, 0, sizeof(SOFCommodityInf));
		//Ĭ��
		dMiniChangePrice = 1.00;
		iPrecision = 1;
		cCoverMode = cmCover;
		ProductDot = 1;
	}
};

//�����۸�
struct SFractionPrice
{
	int	iInteger;			//��������
	int	iMolecules;			//���Ӳ���
	int	idenominator;		//��ĸ����
public:
	SFractionPrice() :iInteger(0), iMolecules(0), idenominator(0) {}
};

//�ֲ���Ϣ
struct PosCount
{
	//��һ�ȳ������ �ڶ��ȳ������
	uint BuyOpenCount;	//���뿪�� �ֲ���(��һ�ȳ���) ��һ���� �ڶ�����ƽ 
	uint BuyCoverCount;	//����ƽ�� �ֲ���(�ڶ��ȳ���) ��һ��ƽ�� �ڶ�������

	uint SellOpenCount;	//�������� �ֲ���(��һ�ȳ���) ��һ������ �ڶ�����ƽ	
	uint SellCoverCount;//����ƽ�� �ֲ���(�ڶ��ȳ���)  ��һ����ƽ �ڶ�����
public:
	PosCount()
	{
		memset(this, 0, sizeof(PosCount));
	}
};

//�ַ����ָ���Ϣ
struct NoticeContent
{
	std::string strKey;
	std::string strContent;
};

//��ƽ �ǻ���
struct CanCoverQty
{
	uint uCanCoverQty;
	uint uCanCoverTQty;
public:
	CanCoverQty()
	{
		memset(this, 0, sizeof(CanCoverQty));
	}
};

namespace CConvert
{
	  //��ʼ��������Ϣ
	  void DefaultSendOrder(TSendOrder&SendOrder);//***
	  //�Ƿ���Ա��¼
	  bool IsTraderLogined();
	  //LME�ж�
	  bool IsLME(const TExchangeKey &Exchange);
	  //�ж��Ƿ�����Ϻ�Լ
	  bool IsCombContract(const char CommodityType);
	  //�Ƿ�����ƽ����ƽ��(�Ϻ�) ����-1������  ֻ�����ֿ���ƽ�ֵĲ���Ҫ���ô˺���
	  bool IsDistinguishCoverType(const TCommodityKey*pCommoditykey);
	  //�ж��Ƿ�����Ч�Ľ���Ʒ��
	  bool IsValidTradeCommodity(const TCommodityKey &CommodityKey);//
	  //�ж��Ƿ�����Ч�������Լ
	  bool IsValidQuotContract(const  char * ContractId);
	  //�Ƿ�ƽ��Լ
	  bool IsCoverOrCoverTCommodity(const TCommodityKey*pCommoditykey);

	  //����Ʒ������ת����
	  TCommodityType QuoteTypeToTradeType(char cType);
	  //����Ʒ������ת����
	  TCommodityType TradeTypeToQuoteType(char cType);

	  //�ַ���ת���׺�Լ
	  bool TContractNoToTContractKey(const char * szCode, TContractKey & ContractKey);//��֤���������ַ����������˶�ȥ���ո���***
	  //���׺�Լת�ַ��� bOnlyContractΪtrueʱֻ��Ҫ��ʽ����Լ���� ����Ҫ����������Ʒ
	  bool TContractKeyToTContractNo(const TContractKey & ContractKey, std::string & strCode);//
	  //���Ƚ��׺�Լת�ַ���
	  bool TContractSingleKeyToTContractNo(const TContractSingleKey & SingContractKey,std::string & strCode);

	  //����Ʒ��IDת���׹ؼ���
	  bool SCommodityNoToTCommodityKey(SCommodityNoType commodityno, TCommodityKey &CommodityKey);
	  //����IDת���׹ؼ���
	  void SContractNoToTContractKey(const char *szSign, const std::string &id, TContractKey &ContractKey);
	  //����IDתƷ�ֹؼ���
	  void SContractNoToTCommodityKey(const char *szSign, const std::string &id, TCommodityKey &CommodityKey);

	  //���׺�Լת�����ַ���
	  void TContractKeyToSContractNo(const TContractKey &ContractKey, char(&ContractId)[101]);
	  //���Ƚ��׺�Լת�����Լ
	  void TContractSingleKeyToSContractNo(const TContractSingleKey &ContractKey, char(&ContractId)[101]);

	  //�����Լת���׺�Լ�ַ���
	  void SContractNoToTContractNo(std::string &sQuoteCode, std::string &str);
	  //�����ַ���������id
	  bool TContractNoToSContractNo(const char *code, char(&ContractId)[101]);

	//�����Ȩ�ֶ�
	  bool ParseOptionContractStr(const std::string& strValue, char(&ContractNo)[11], TOptionType &OptionType, char(&StrikePrice)[11]);

	  //��ϱ䵥��
	  void CombTContractKeyToTContractSingleKey(const TContractKey &ContractKey, TContractSingleKey &SigCont1, TContractSingleKey &SigCont2);
	  //��Ϻ�Լ�Ƿ��Ե�һ��Ϊ׼��1�ǣ�0�Եڶ���Ϊ׼
	  bool CombContractIsFirst(const TCommodityKey & CommodityKey);

	  //�����ϻ�ȡ����Ʒ����Ϣ ��С�䶯�� ���� �۸����͵�
	  bool GetCommodityInf(const TCommodityKey &sCommodityKey, SOFCommodityInf & sCommodityInfo);
	  //�����ϻ�ȡ����Ʒ����Ϣ ��С�䶯�� ���� �۸����͵�
	  bool GetQuotCommodityInf(const char*code, SOFCommodityInf & sCommodityInfo);
	  //��ȡ��С�䶯�۸�
	  double GetMinChangePrice(const SOFCommodityInf & sCommodityInfo);

	  //API �������޹� ����0�ɹ� ���ظ���ʧ��  ��ѯ����ʧ��
	  int QrySingleContractCanOpen(const TSendOrder &SendOrder);
	  //�µ�->��Էǿ���ƽ���µ������ֽ�Ϊ��������ȡ��   ������ݿ����µ� ���һ����ȡ����Ϊ�������õ�ָ���˻���ָ����Լ��ָ������ĳֲ� 
	  void  GetPosNum(const char *UserNo, const TContractKey * ContractKey, char Direct,
		  uint &iPosT, uint &iPos);
	  void  GetPosCanCoverNum(const char *UserNo, const TContractKey * ContractKey, char Direct, bool bDisCover,
		  uint &iPosT, uint &iPos, uint &CanCoverNum, uint&CanCoverTNum);
	  //��ȡ�ɿ� ����ϵͳ������Ϣ���ֲ���Ϣ 
	  int GetCanOpenQty(char cCoverMode, const TSendOrder &SendOrder, bool bOpenMaxAutoCancel);
	  //��ȡ��ƽ ����ϵͳ������Ϣ���ֲ���Ϣ 
	  void GetCanCoverQty(char cCoverMode, const TContractKey &ContractKey, const char *UserNo, char Direct, CanCoverQty& CanCoverQty, bool bLiquidateAutoCancel);
	  //��ȡ�����ֲ�
	  void GetSwapPositionVol(const char * UserNo, TContractSingleKey &SigCont1, TContractSingleKey&SigCont2, PosCount&posCount);
	  //��ȡ�ֲ�
	  void GetPositionVol(const char * UserNo, const TContractKey * ContractKey
		  , uint &LongCount, uint &ShortCount, uint &DayLongCount, uint &DayShortCount);
	  //�жϿ�ƽ
	  bool OrderOffsetIsOpen(char direct, const char * UserNo, const TContractKey * ContractKey, char OrderType);
	  //��ƽ����
	  TOffset GetOffset(const char* UserNo, const TContractSingleKey * ContractKey, TDirect direct);
	  TOffset GetOffset(const char* UserNo, const TContractKey * ContractKey, TDirect direct);
	  
	  //�۸�ת�ַ���
	  std::string GetStrPriceByPrice(double price, bool bFraction, int nPrecision);
	  //С�������
	  bool DecimalsToFractions(double inPrice, int denominator, SFractionPrice & out);
	  //��ʽ���۸��ַ���
	  string FormatPriceToStr(double inPrice, SOFCommodityInf &mInfo);
	  //�Ƿ�����
	  bool IsDiv(const double Value, const double SubValue, const double ErrorDiff = 1e-10);
	  //����ƥ��
	  bool MatchPattern(const char *pattern, const char *target);
	  //С����ֵ�ж�
	  bool DoubleSameValue(const double d1, const double d2, double Epsilon);

	  //�ָ��ַ���
	  bool SplitNotice(const std::string &strContent, char cSplit, std::vector<NoticeContent> &vecstr);

	  bool SplitNoticePos(const std::string &strContent, char cSplit, std::vector<NoticeContent> &vecstr, int nContNum);//nContNum  5 �ֲֺϼ�  6 �ֲ���ϸ

	  std::vector<std::string> SplitString(const std::string & str, char cSplit);

	  void TrimString(std::string &str);

	  void GetPriceQty(const SQuoteSnapShot& QuoteSnap, double& dbuy, double& dsell, SQtyType& nbuy, SQtyType& nsell);

	  long Round(double value,double dDif=0.5);

	  bool SameValue(const double A, const double B, double Epsilon = 0);
	 
	  int CmpValue(const double A, const double B, double Epsilon = 0);

	  SPriceType GetPriceByContract(const SContract* SContract, SFidMeanType nField);//����������

	  SQtyType GetQtyByContract(const SContract* SContract, SFidMeanType nField);//����������

	  SPriceType GetPriceBySnapShot(const SQuoteSnapShot* QuoteSnap, SFidMeanType nField);//

	  SQtyType GetQtyBySnapShot(const SQuoteSnapShot* QuoteSnap, SFidMeanType nField);//

	  double FormatPriceToDouble(double dPrice,int nPrecision,double dMinChange);

	  wstring LoadResString(int strID);

	  wstring UTF8ToUnicode(const string& str);
};
#endif