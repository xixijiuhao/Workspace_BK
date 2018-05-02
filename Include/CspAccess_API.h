#ifndef _CSPACCESSAPI_API_
#define _CSPACCESSAPI_API_

const wchar_t PLUG_ACCESSAPI_NAME[] = L"CspAccessApi";
const wchar_t PLUG_ACCESSAPI_VERSION[] = L"9.3.0.1";

#include "StarApi_TYPE.h"

//�������ص�
class ICspAccessSpi
{
public:
	//��ʼ����� ��һ��
	virtual void __stdcall OnInitCompleteCsp() = 0;
};


//ҵ��ӿ�--------------------------------------------------------------------------------------------------------------
class ICspAccessApi
{
public:
	//��ע���ȡ�����
	virtual SRetType __stdcall RegEventSpi(ICspAccessSpi* spi) = 0;
	virtual SRetType __stdcall UnregEventSpi(ICspAccessSpi* spi) = 0;

	//������������Ϣ
	virtual SRetType __stdcall RequireAccessInfo(SLangType LangId) = 0;

	//��ȡ��ǰ����
	virtual SRetType __stdcall GetPackageNo(SPackageNoType pkgno) = 0;

	//��ȡ������
	virtual SRetType __stdcall GetWhiteListCount() = 0;
	virtual SRetType __stdcall GetWhiteListInfo(const SWhiteListNameType begin_configno, SWhiteListInfo* out[], int len, bool next) = 0;

	//��ȡ���ÿ�����Ϣ
	virtual SRetType __stdcall GetConfigSwitchCount() = 0;
	virtual SRetType __stdcall GetConfigSwitchInfo(const SConfigSwitchNoType begin_configno, SConfigSwitchInfo* out[], int len, bool next) = 0;

	//��ȡ�����ַ
	virtual SRetType __stdcall GetQuoteAddrCount() = 0;
	virtual SRetType __stdcall GetQuoteAddr(const SCompanyNoType begin_company, const SSiteAddrNoType begin_site,
		SQuoteLink out[], SLenType len, SNextType next) = 0;

	//��ȡ��ʷ�����ַ
	virtual SRetType __stdcall GetHisQuoteAddrCount() = 0;
	virtual SRetType __stdcall GetHisQuoteAddr(const SCompanyNoType begin_company, const SSiteAddrNoType begin_site,
		SQuoteLink out[], SLenType len, SNextType next) = 0;

	//������
	virtual SRetType __stdcall GetPlateInfoCount() = 0;
	virtual SRetType __stdcall GetPlateInfo(const SIndexType begin_index, SPlate* out[], SLenType len) = 0;

	//���Ʒ��
	virtual SRetType __stdcall GetPlateCodeCount(const SPlateNoType plateno) = 0;
	virtual SRetType __stdcall GetPlateCode(const SPlateNoType plateno, const SIndexType begin_index, SPlateContent* out[], SLenType len) = 0;

	//��ȡֱ�����׵�ַ
	virtual SRetType __stdcall GetDirectTradeAddrCount() = 0;
	virtual SRetType __stdcall GetDirectTradeAddr(const SCompanyNoType begin_company, const SSiteAddrNoType begin_site,
		SDirectTradeLink out[], SLenType len, SNextType next) = 0;

	//��ȡ���δ���
	virtual SRetType __stdcall GetCodeForbidCount() = 0;
	virtual SRetType __stdcall GetCodeForbid(const SContractNoType begin_no, SContractNoType out[], SLenType len, SNextType next) = 0;

	//��ȡ����ת����Ϣ
	virtual SRetType __stdcall GetCodeMappingCount() = 0;
	virtual SRetType __stdcall GetCodeMapping(const SContractNoType begin_no, SCodeMappingInfo out[], SLenType len, SNextType next) = 0;

	//��ȡ��Ȩִ�м۲���
	virtual SRetType __stdcall  GetOptionPrecisionCount() = 0;
	virtual SRetType __stdcall  GetOptionPrecision(const SCommodityNoType begin_no, SOptionPrecisionInfo out[], SLenType len, SNextType next) = 0;
};

#endif