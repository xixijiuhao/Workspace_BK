#ifndef _MOUSE_TYPE_H_
#define _MOUSE_TYPE_H_

typedef int     MouPopType;
#define mptPrice				0
#define mptSpreadTrig			1

#define LEN_RET					21
#define N_NORMAL_WIDYH			310

#define N_MAX_CHILD				20

typedef wchar_t	TMouPriceTypeStrW[21];			//�۸�����

typedef wchar_t	TMouCanQtyStrW[51];				//�ɿ���ƽ �Գ� ���� Ԥ����

typedef wchar_t	TMouBuySellStrW[51];			//����һ����Ϣ

typedef wchar_t	TMouFormatPriceStrW[100];		//��ʽ���۸�

typedef wchar_t	TMouErrorTipStrW[100];			//��ʾ��Ϣ

typedef wchar_t	TMouContractStrW[51];			//��Լ

typedef wchar_t	TMouUserSummuryStrW[250];		//��Ҫ��Ϣ

typedef wchar_t	TMouLockTipStrW[250];			//��ʾ��Ϣ

typedef TMouPriceTypeStrW	TMouModeStrW;		//����ģʽ

typedef TMouPriceTypeStrW	TMouConditionStrW;	//��������

typedef TMouPriceTypeStrW	TMouValidStrW;		//��Ч����

typedef TMouPriceTypeStrW	TMouQtyStrW;		//����

typedef TMouPriceTypeStrW	TMouQtyRateStrW;	//��������

typedef TMouPriceTypeStrW	TMouHeadStrW;		//��ͷ

typedef TMouUserSummuryStrW	TMouPriceExceed;	//�۸��� У��


typedef char	TMouPriceStrA[21];				//�۸�

typedef char	TMouContractStrA[101];			//��Լ

typedef char	TMouLinkMsgStrA[100];			//������Ϣ

typedef char	TMouIntStrA[21];				//����

typedef char	TMouCalStrA[2];					//�����

typedef char	TMouUserNoStrA[21];				//�ʽ��˺�

typedef TMouUserNoStrA		TMouSignStrA;		//��������ʶ

#endif