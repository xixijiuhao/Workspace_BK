#pragma once
#include "AuxiliaryClassStruct.h"
#include <string>
#include "TradeApi_Struct.h"
#include "TradeApi_API.h"
#include <vector>
#include "SysConfig_Struct.h"
#include "share_struct.h"
#include "load_rc.h"
using namespace std;

typedef vector<int> IntVec;
typedef vector<string> CStringsVec;

//����
class UserNoOrder
{
public:
	bool operator()(CStringsVec &vecStr1, CStringsVec &vecStr2)
	{
		return vecStr1[0]<vecStr2[0];
	}
};


//�ҵ� ������� �����ǲ����ֿ�ƽ�� ��Ϊ����Ŀǰ��������û�� ���˾�Ҫ���¿���
struct PartOrderQty
{
	//����
	uint	BuyOpenOpenQty;//���뿪�� ���ֹҵ��� ��һ���� �ڶ�����ƽ 
	uint	BuyOpenCoverQty;//���뿪�� ƽ�ֹҵ��� ��һ���� �ڶ�������
	uint	BuyCoverOpenQty;//����ƽ�� ���ֹҵ��� ��һ����ƽ �ڶ�������
	uint	BuyCoverCoverQty;//����ƽ�� ƽ�ֹҵ��� ��һ����ƽ �ڶ�������

	//����
	uint	SellOpenOpenQty;//�������� ���ֹҵ��� ��һ������ �ڶ�����ƽ	
	uint	SellOpenCoverQty;//�������� ƽ�ֹҵ��� ��һ������ �ڶ�����ƽ
	uint	SellCoverOpenQty;//�������� ���ֹҵ��� ��һ����ƽ �ڶ�����
	uint	SellCoverCoverQty;//����ƽ�� ƽ�ֹҵ��� ��һ����ƽ �ڶ�����
public:
	PartOrderQty()
	{
		memset(this, 0, sizeof(PartOrderQty));
	}
};

//������
enum Exchange_Type
{
	NONE_TYPE,		//��Ч
	ZCE_TYPE,		//֣����
	DCE_TYPE,		//������
	SHFE_TYPE,		//������
	CFFEF_TYPE		//�н���
};



enum eCancelOrderType
{
	AUTOCANCEL_ZUBI,
	AUTOCANCEL_QUANBU,
	CANCEL_BUY,				//������
	CANCEL_SELL,			//������
	CANCEL_KAICANG,			//������
	CANCEL_PINGCANG,		//��ƽ��
	CANCEL_ALL,				//������
	CANCEL_QINGCANG			//������
};

//ƽ���Զ������������ͣ���ƽȫ������ֺ���֣�����ƽ��֡���ƽ��
enum AutoCancel_Type
{
	NoCancel_Type,	//����Ҫ���� �жϵ��ڴ˾Ͳ���Ҫִ�г�����
	Total_Type,		//ȫ��
	Today_Type,		//������Ŷӵ�
	Yday_Type		//������Ŷӵ�
};

//ƽ���������ͣ����ݴ��ж�Ҫ�¼��ʵ���
enum eCoverNumType
{
	SendOrder_One_NoDistinguish = 1,	//��һ�ʵ��� �������� ȡoffset�ֶ�
	SendOrder_One_Distinguish,		//��һ�ʵ��� �������� ȡoffsetT�ֶ�
	SendOrder_Two,					//�����ʵ���
	SendOrder_Three					//�����ʵ���
};

namespace CAuxiliary
{
	//��ʼ��TSendOrder
	void DefaultSendOrder(TSendOrder&SendOrder);
	//�ж��Ƿ��Ǹ۽����ڻ�
	bool IsHK(const char *szCode);
	//�ж���������
	int  ReadVersionType();
	//�ַ���ת���׺�Լ
	bool StrToTradeContract(const char * szCode, TContractKey & ContractKey);
	//���׺�Լת�ַ��� bOnlyContractΪtrueʱֻ��Ҫ��ʽ����Լ���� ����Ҫ����������Ʒ
	bool TradeContractToStr(const TContractKey & ContractKey, string & strCode, bool bOnlyContract = false, bool bName = false, bool bOnlyName = false);
	//���׺�Լת�ַ��� ֻҪƷ�ֺͺ�Լ ��Ҫ������
	bool TradeContractToStr(string & strCode, const TContractKey & ContractKey);
	//�����ϻ�ȡ����Ʒ����Ϣ ��С�䶯�� ���� �۸����͵�
	bool GetCommodityInf(const TCommodityKey &sCommodityKey, SOFCommodityInf & sCommodityInfo);
	//�����ϻ�ȡƷ����Ϣ ��С�䶯�� ���� �۸����͵�
	bool GetQuotCommodityInf(const char*code, SOFCommodityInf & sCommodityInfo);
	//��ȡ��С�䶯�۸�
	double GetMinChangePrice(const SOFCommodityInf & sCommodityInfo);
	//�����Ȩ�ֶ�
	bool ParseOptionContractStr(const std::string& strValue, char(&ContractNo)[11], TOptionType &OptionType, char(&StrikePrice)[11]);
	//�ָ��ַ���
	void StringSplit(const char* strSrc, char Separator, vector<string>& vecDest);
	//���׺�ԼתΪ����ID
	void TradeContractToRawContractIdType(const TContractKey &ContractKey, char(&ContractId)[51]);
	//����IDתΪ���׺�Լ
	void RawContractIdTypeToTradeContract(const char *szSign, const string &id, TContractKey &ContractKey);
	//�����ַ���������id
	bool StrToRawContractIdType(const char *code, char(&ContractId)[51]);
	//����id�������ַ���
	void RawContractIdTypeToStr(char(&ContractId)[51], string &str);
	//�ж��Ƿ�����Ч�������Լ
	bool IsValidQuotContract(const  char * ContractId);
	//����Ƿ��н���Ա��¼��
	bool IsTraderLogined();
	//�ж��Ƿ�����Ϻ�Լ
	bool IsCombContract(const char CommodityType);
	//��ȡȱʡĬ������
	int  GetDefaultQty(const char *szSign, int iCommodityVol, CommodityVol *pCommodityVol,const TContractKey &ContractKey, int iType);
	//�ж��Ƿ�����Ч�Ľ��׺�Լ
	bool IsValidTradeContract(const TContractKey & ContractKey);
	//��ʽ�����׺�Լ�ṹ��
	void FormatTradeContract(const TContractKey & ContractKey, string &str);
	//��ȡ�ʽ��˻���Ϣ
	bool GetAllUserNo(vector<CStringsVec>&vecUserNo);
	//�Ƿ�����ƽ����ƽ��(�Ϻ�) ����-1������  ֻ�����ֿ���ƽ�ֵĲ���Ҫ���ô˺���
	bool IsDistinguishCoverType(const TCommodityKey*pCommoditykey);
	//API �������޹� ����0�ɹ� ���ظ���ʧ��  ��ѯ����ʧ��
	int QrySingleContractCanOpen(const TSendOrder &SendOrder);
	//��ȡ�ɿ� ����ϵͳ������Ϣ���ֲ���Ϣ 
	int GetCanOpenQty(char cCoverMode, const TSendOrder &SendOrder, bool bOpenMaxAutoCancel);
	//��ȡ��ƽ ����ϵͳ������Ϣ���ֲ���Ϣ 
	void GetCanCoverQty(char cCoverMode, const TContractKey &ContractKey, const char *UserNo, char Direct,CanCoverQty& CanCoverQty, bool bLiquidateAutoCancel);
	//����Ϻ�ԼתΪ�������Ⱥ�Լ
	void CombContractToSingleContract(const TContractKey &ContractKey, TContractSingleKey &SigCont1, TContractSingleKey &SigCont2);
	//��Ϻ�Լ�Ƿ��Ե�һ��Ϊ׼��1�ǣ�0�Եڶ���Ϊ׼
	bool CombContractIsFirst(const TCommodityKey & CommodityKey);
	//�õ��ֲ֡���ƽ ���ݿͻ� ����������  �ǻ���
	void GetPosCanCoverNum(const char *UserNo, const TContractKey * ContractKey, char Direct, bool bDisCover,
		uint &iPosT, uint &iPos, uint &CanCoverNum, uint&CanCoverTNum);
	//�µ�->��Էǿ���ƽ���µ������ֽ�Ϊ��������ȡ��   ������ݿ����µ� ���һ����ȡ����Ϊ�������õ�ָ���˻���ָ����Լ��ָ������ĳֲ� 
	void  GetPosNum(const char *UserNo, const TContractKey * ContractKey, char Direct,
		uint &iPosT, uint &iPos);
	//�ֲ����� ��������ͨ����㷨��һ�� ������һ��һƽ ��ֲ�����ֻ��Ҫ��ƽ�ֵ���һ��
	void GetPositionVol(const char * UserNo, const TContractKey * ContractKey
		, uint &LongCount, uint &ShortCount, uint &DayLongCount, uint &DayShortCount);
	//�ж��Ƿ�����Ȩ��Լ
	bool IsOptionContract(const char CommodityType);
	//���Ƚ��׺�Լת�ַ��� Ĭ�ϲ���������
	bool SingTradeContractToStr(string & strCode, const TContractSingleKey & SingContractKey, bool bHaveExchange);
	//��ȡswap�ֲ�
	void GetSwapPositionVol(const char * UserNo, TContractSingleKey &SigCont1, TContractSingleKey&SigCont2, PosCount&posCount);
	//��������
	bool IsInnerDiskExchange(const AbnormalTradeLimit& AbnormalTradeLimit, const char *szSign, const char *szExchange, Exchange_Type &ExchangeType);
	//����ת���� ��Լ����
	TCommodityType QuoteTypeToTradeType(char cType);
}