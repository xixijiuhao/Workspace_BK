#ifndef _TRADE_QUOTE_STRING_
#define _TRADE_QUOTE_STRING_

#define S_ZCE  "ZCE"
#define S_DCE  "DCE"
#define S_CFFEX "CFFEX"
#define S_SHFE  "SHFE"

namespace CConvert
{
	//��ʼ��������Ϣ
	  void DefaultSendOrder(TSendOrder&SendOrder);//***
	  //�ж��Ƿ�����Ϻ�Լ
	  bool IsCombContract(const char CommodityType);
	  //�Ƿ��������Լ
	  bool IsValidQuotContract(const  char * ContractId);

	//�ַ���ת���׺�Լ
	  bool StrToTradeContract(const char * szCode, TContractKey & ContractKey);//��֤���������ַ����������˶�ȥ���ո���***
	//���Ƚ��׺�Լת�ַ���
	  bool PosContractToStr(std::string & strCode, const TContractSingleKey & SingContractKey);

	  //����Ʒ������ת����
	  TCommodityType QuoteTypeToTradeType(char cType);
	  //����Ʒ������ת����
	  TCommodityType TradeTypeToQuoteType(char cType);
	  //����IDת���׹ؼ���
	  void RawContractIdTypeToTradeContract(const std::string &id, TContractKey &ContractKey);
	  //���׺�Լת�����ַ���
	  void TradeContractToRawContractIdType(const TContractKey &ContractKey, char(&ContractId)[51]);
	  //���Ƚ��׺�Լת�����Լ
	  void SingleTradeContractToRawContractIdType(const TContractSingleKey &ContractKey, char(&ContractId)[51]);
	  //�����Լת���׺�Լ�ַ���
	  void RawContractIdTypeToStr(std::string &sQuoteCode, std::string &str);
	  //�����ַ���������id
	  bool StrToRawContractIdType(const char *code, char(&ContractId)[51]);
	  //�����Ȩ�ֶ�
	  bool ParseOptionContractStr(const std::string& strValue, char(&ContractNo)[11], TOptionType &OptionType, char(&StrikePrice)[11]);

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
	  //��ȡ�ʽ��˺�
	  bool GetAllUserNo(std::vector<CStringsVec>&vecUserNo);

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
	
	  bool IsTraderLogined();

	  void SplitString(string str, char split, vector<string> &content);
	  void TradeSingleContractToRawId(const TContractSingleKey* ContractSingleKey, char(&ContractId)[51]);

	  bool newGUID(char* buf);
	  bool IsSHFE(const char * exNo);
	  void GetAvailableQty(const TPosition* pst, int& AvaQty, int& TAvaQty);
	  void DefaultDealClose(const TPosition *stPosition, TSendOrder &stOrder);
	  void GetCloseNum(const char* UserNo, TContractSingleKey* ContractKey, TDirect direct, const int iCoverQty, int& iCoverNum, int& iTCoverNum);
	  bool IsEsunnyUser();

	  char* WcharToChar(const wchar_t* wc);

	  wstring UTF8ToUnicode(const string& str);
};
#endif