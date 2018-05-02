#ifndef _TRADE_QUOTE_STRING_
#define _TRADE_QUOTE_STRING_

typedef int		N_SwitchType;
#define NST_NONE	0	//��ת��
#define NST_T2Q	1		//���׺�Լת�����Լ
#define NST_Q2T	2		//�����Լת���׺�Լ


namespace CConvert
{
	//��ʼ��������Ϣ
	  void DefaultSendOrder(TSendOrder&SendOrder);//***
	//LME�ж�
	  bool IsLME(const TExchangeKey&Exchange);
	//�ж��Ƿ��Ǹ۽����ڻ�
	  bool IsHK(const char *szCode);//*
	//�Ƿ�������
	  bool IsInnerDiskExchange(const char *szExchange);
	  //�ж�������
	  int  ReadVersionType();
	  //�ж��Ƿ�����Ϻ�Լ
	  bool IsCombContract(const char CommodityType);
	  //�Ƿ���Ȩ��Լ
	  bool IsOptionContract(const char CommodityType);
      //�ж��Ƿ�����Ч�Ľ���Ʒ��
	  bool IsValidTradeCommodity(const TCommodityKey & ContractKey);//
	  //�Ƿ�����ƽ����ƽ��(�Ϻ�) ����-1������  ֻ�����ֿ���ƽ�ֵĲ���Ҫ���ô˺���
	  bool IsDistinguishCoverType(const TCommodityKey*pCommoditykey);
	  //�Ƿ��������Լ
	  bool IsValidQuotContract(const  char * ContractId);
	  //�Ƿ���������Լ
	  bool IsValidSpreadContract(const  char * ContractId);
	  
	//�ַ���ת���׺�Լ
	  bool TContractNoToTContractKey(const char * szCode, TContractKey & ContractKey);//��֤���������ַ����������˶�ȥ���ո���***
      //���׺�Լת�ַ��� ��Ϣ��ʾ
      bool TContractKeyToTContractNo(const TContractKey & ContractKey, std::wstring & strCode, std::wstring str = L"");//
      //���׺�Լת�ַ���
      bool TContractKeyToTContractNo(const TContractKey & ContractKey, std::string & strCode, bool bOnlyContract = false);//
	//���Ƚ��׺�Լת�ַ��� Ĭ�ϲ���������
	  bool SingTradeContractToStr(std::string & strCode, const TContractSingleKey & SingContractKey, bool bHaveExchange = false, bool bHaveSign = true);
	//���Ƚ��׺�Լת�ַ���
	  bool PosContractToStr(std::string & strCode, const TContractSingleKey & SingContractKey);
	//��ʽ�����׺�Լ�ṹ��
	  void FormatTradeContract(const TContractKey & ContractKey, std::string &str);

	  //����Ʒ������ת����
	  TCommodityType QuoteTypeToTradeType(char cType);
	  //����Ʒ������ת����
	  TCommodityType TradeTypeToQuoteType(char cType);

      //����Ʒ��IDת���׹ؼ���
      bool SCommodityNoToTCommodityKey(SCommodityNoType commodityno, TCommodityKey &CommodityKey);
	  //����IDת���׹ؼ���
      void SContractNoTypeToTradeContract(const char *szSign, const std::string &id, TContractKey &ContractKey);
      //����IDתƷ�ֹؼ���
      void SContractNoTypeToTradeCommodity(const char *szSign, const std::string &id, TCommodityKey &CommodityKey);

	  //���׺�Լת�����ַ���
	  void TradeContractToQContractNoType(const TContractKey &ContractKey, char(&ContractId)[101]);
	  //���Ƚ��׺�Լת�����Լ
	  void SingleTradeContractToQContractNoType(const TContractSingleKey &ContractKey, char(&ContractId)[101]);
	  //��Ȩ���׺�Լת��Ȩ����
	  bool TradeContractToOptionTarget(const TContractKey &ContractKey, char(&ContractId)[101]);
	  //�����Լת���׺�Լ�ַ���
	  void QContractNoTypeToStr(std::string &sQuoteCode, std::string &str, bool bOnlyContract = false);
	  //�����ַ���������id
	  bool StrToQContractNoType(const char *code, char(&ContractId)[101]);
	  //������Լ�ַ����ֽ�
	  bool SpreadStrToSpreadInfo(const char *code, char& cRateMode,char(&sFirst)[101], char(&sSecond)[101],int N_SwitchType =NST_NONE);//ntype :0:  1: 2:Q2T
	  //�����Ȩ�ֶ�
	  bool ParseOptionContractStr(const std::string& strValue, char(&ContractNo)[11], TOptionType &OptionType, char(&StrikePrice)[11]);

	  //��ϱ䵥��
      void CombTContractKeyToTContractSingleKey(const TContractKey &ContractKey, TContractSingleKey &SigCont1, TContractSingleKey &SigCont2);
	  //��Ϻ�Լ�Ƿ��Ե�һ��Ϊ׼��1�ǣ�0�Եڶ���Ϊ׼
	  bool CombContractIsFirst(const TCommodityKey & CommodityKey);

	  //�����ϻ�ȡ����Ʒ����Ϣ ��С�䶯�� ���� �۸����͵�
	  bool GetCommodityInf(const TCommodityKey &sCommodityKey, SOFCommodityInf & sCommodityInfo);
	  //�����ϻ�ȡ����Ʒ����Ϣ ��С�䶯�� ���� �۸����͵�
	  bool GetQuoteCommodityInf(const char*code, SOFCommodityInf & sCommodityInfo);
	  //��ȡ��С�䶯�۸�
	  double GetMinChangePrice(const SOFCommodityInf & sCommodityInfo);

	  //API �������޹� ����0�ɹ� ���ظ���ʧ��  ��ѯ����ʧ��
	  int QrySingleContractCanOpen(const TSendOrder &SendOrder);
	  //API �������޹� ����0�ɹ� ���ظ���ʧ��  ��ѯ����ʧ��
	  int QrySingleContractPreFrozen(const TSendOrder &SendOrder,double& dValue);
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
	  //��ȡ�ʽ��˺�
	  bool GetAllUserNo(std::vector<CStringsVec>&vecUserNo);

	  //�۸�ת�ַ���
	  std::string GetStrPriceByPrice(double price, bool bFraction, int nPrecision);
	  //
	  long Round(double Value);
	  //С�������
	  bool DecimalsToFractions(double inPrice, int denominator, SFractionPrice & out);
	  //��ʽ���۸��ַ���
	  std::string FormatPriceToStr(double inPrice, const SOFCommodityInf &mInfo);
	  //�Ƿ�����
	  bool IsDiv(const double Value, const double SubValue, const double ErrorDiff = 1e-10);
	  //����ƥ��
	  bool MatchPattern(const char *pattern, const char *target);
	  //�Ƿ���Ч������
	  bool IsValidNumber(const char * sValue, bool bOnlyInt = false, bool bCanNegative = false);
	  //С����ֵ�ж�
	  bool DoubleSameValue(const double d1, const double d2, double Epsilon);

	  //�ָ��ַ���
	  bool SplitNotice(const std::string &strContent, char cSplit, char cSplit2, std::vector<NoticeContent> &vecstr);

	  bool SplitNoticePos(const std::string &strContent, char cSplit, std::vector<NoticeContent> &vecstr, int nContNum);//nContNum  5 �ֲֺϼ�  6 �ֲ���ϸ

	  std::vector<std::string> SplitString(const std::string & str, char cSplit);

	  void TrimString(std::string &str);
	
	  bool IsTraderLogined();

	  SPriceType GetPriceByContract(const SContract* qContract, SFidMeanType nField);//����������

	  SQtyType GetQtyByContract(const SContract* qContract, SFidMeanType nField);//����������

	  SPriceType GetPriceBySnapShot(const SQuoteSnapShot* QuoteSnap, SFidMeanType nField);//

	  SQtyType GetQtyBySnapShot(const SQuoteSnapShot* QuoteSnap, SFidMeanType nField);//

      wstring UTF8ToUnicode(const string& str);

	  void GetFilePath(TCHAR* filePath, int length, TCHAR* fileName);
	  void GetFilePath(char* filePath, int length, char* filenName);

	  bool IsUserLogin();
	  bool IsLogined();
};
#endif