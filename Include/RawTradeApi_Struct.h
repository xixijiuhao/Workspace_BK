#pragma once


#include "tradeapi_struct.h"

#ifdef STRUCT_FUNCTION
//trade_struct.h�����¶�������������нṹ�壬�������ƺ��ڴ沼�ֶ��������һ����ֻ��Ϊ�˵��÷��������˳�Ա����
#include "trade_struct.h"
#else


#pragma pack(push, 1)

//�ʽ��˺Ŷ�Ӧ���������ѯ�ṹ
struct	TFundCalcParamReq : public TContractSingleKey
{
	char					UserNo[21];					//�ʽ��˻�
};

//�ʽ��˺Ŷ�Ӧ�������Ӧ��ṹ
struct	TFundCalcParam : public TFundCalcParamReq
{
	char					CurrencyNo[11];				//���ֱ��

	double					OpenFeeRatio;				//���������ѱ���
	double					ColseFeeRatio;				//ƽ�������ѱ���
	double					ColseTodayFeeRatio;			//ƽ�������ѱ���
	double					OpenFeeAmount;				//��������������
	double					CoverFeeAmount;				//ƽ������������
	double					CoverTodayFeeAmount;		//ƽ������������
	
	double					StrikeFeeRatio;				//��Ȩ�����ѱ���	
	double					StrikeFeeAmount;			//��Ȩ����������

	double                  SLongDepositRatio;			//��ͷͶ����֤�����
	double                  SLongDepositAmount;			//��ͷͶ����֤������
	double                  SShortDepositRatio;			//��ͷͶ����֤�����
	double                  SShortDepositAmount;		//��ͷͶ����֤������
	double                  HLongDepositRatio;			//��ͷ�ױ���֤�����
	double                  HLongDepositAmount;			//��ͷ�ױ���֤������
	double                  HShortDepositRatio;			//��ͷ�ױ���֤�����
	double                  HShortDepositAmount;		//��ͷ�ױ���֤������
	double                  ALongDepositRatio;			//��ͷ������֤�����
	double                  ALongDepositAmount;			//��ͷ������֤������
	double                  AShortDepositRatio;			//��ͷ������֤�����
	double                  AShortDepositAmount;		//��ͷ������֤������
	double                  MLongDepositRatio;			//��ͷ���б�֤�����
	double                  MLongDepositAmount;			//��ͷ���б�֤������
	double                  MShortDepositRatio;			//��ͷ���б�֤�����
	double                  MShortDepositAmount;		//��ͷ���б�֤������

	const TCurrency *		pCurrency;
};

#pragma pack(pop)

#endif