#ifndef _MOUSE_TYPE_H_
#define _MOUSE_TYPE_H_

typedef int     MouPopType;
#define mptPrice				0
#define mptSpreadTrig			1

#define LEN_RET					21
#define N_NORMAL_WIDYH			310

#define N_MAX_CHILD				20

typedef wchar_t	TMouPriceTypeStrW[21];			//价格类型

typedef wchar_t	TMouCanQtyStrW[51];				//可开可平 对冲 建仓 预冻结

typedef wchar_t	TMouBuySellStrW[51];			//买卖一档信息

typedef wchar_t	TMouFormatPriceStrW[100];		//格式化价格

typedef wchar_t	TMouErrorTipStrW[100];			//提示信息

typedef wchar_t	TMouContractStrW[51];			//合约

typedef wchar_t	TMouUserSummuryStrW[250];		//概要信息

typedef wchar_t	TMouLockTipStrW[250];			//提示信息

typedef TMouPriceTypeStrW	TMouModeStrW;		//触发模式

typedef TMouPriceTypeStrW	TMouConditionStrW;	//触发条件

typedef TMouPriceTypeStrW	TMouValidStrW;		//有效类型

typedef TMouPriceTypeStrW	TMouQtyStrW;		//数量

typedef TMouPriceTypeStrW	TMouQtyRateStrW;	//数量倍率

typedef TMouPriceTypeStrW	TMouHeadStrW;		//列头

typedef TMouUserSummuryStrW	TMouPriceExceed;	//价格检测 校正


typedef char	TMouPriceStrA[21];				//价格

typedef char	TMouContractStrA[101];			//合约

typedef char	TMouLinkMsgStrA[100];			//联动消息

typedef char	TMouIntStrA[21];				//整型

typedef char	TMouCalStrA[2];					//运算符

typedef char	TMouUserNoStrA[21];				//资金账号

typedef TMouUserNoStrA		TMouSignStrA;		//服务器标识

#endif