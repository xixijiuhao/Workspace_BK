#ifndef StarApi_TYPE_h
#define StarApi_TYPE_h

//基础数据类型=======================================================================================
typedef signed char                             B8;

typedef signed char                             I8;
typedef signed short                            I16;
typedef signed int                              I32;
typedef signed long long                        I64;

typedef unsigned char                           U8;
typedef unsigned short                          U16;
typedef unsigned int                            U32;
typedef unsigned long long                      U64;

typedef float                                   F32;
typedef double                                  F64;

typedef char                                    C8;
typedef unsigned short                          C16;

typedef void*                                   PTR;

typedef C8                                      STR10[11];
typedef C8                                      STR20[21];
typedef C8                                      STR30[31];
typedef C8                                      STR40[41];
typedef C8                                      STR50[51];
typedef C8                                      STR100[101];
typedef C8                                      STR200[201];
typedef C8                                      STR500[501];

//业务数据类型=======================================================================================
typedef I32                                     SRetType;							//返回值类型

typedef U8                                      SSrvSrcType;						//服务类型
static const SSrvSrcType						S_SRVSRC_QUOTE			  = 'Q';	//行情服务
static const SSrvSrcType						S_SRVSRC_HISQUOTE		  = 'H';	//历史行情服务
static const SSrvSrcType						S_SRVSRC_TRADE			  = 'T';	//交易服务
																		  
typedef U8                                      SSrvEventType;			  			//服务事件类型
static const SSrvEventType						S_SRVEVENT_CONNECT		  = 'C';	//连接 
static const SSrvEventType						S_SRVEVENT_DISCONNECT	  = 'D';	//断开  
static const SSrvEventType						S_SRVEVENT_LOGIN		  = 'L';	//登录 
static const SSrvEventType						S_SRVEVENT_INITCOMPLETED  = 'I';	//初始化完成 
static const SSrvEventType						S_SRVEVENT_QUOTE		  = 'Q';	//即时行情 
static const SSrvEventType						S_SRVEVENT_HISQUOTE		  = 'H';	//历史行情
static const SSrvEventType						S_SRVEVENT_TRADE		  = 'T';	//交易
static const SSrvEventType						S_SRVEVENT_TRADE_ORDER	  = 'o';	//交易委托变化
static const SSrvEventType						S_SRVEVENT_TRADE_MATCH	  = 'm';	//交易成交变化
static const SSrvEventType						S_SRVEVENT_TRADE_POSITION = 'p';	//交易持仓变化
static const SSrvEventType						S_SRVEVENT_TRADE_FUND	  = 'f';	//交易资金变化

typedef PTR										SSrvTradeDataType;					//服务交易数据

typedef C8*                                     SDataType;				  		  
typedef U32                                     SLenType;				  		  
typedef I32                                     SIndexType;				  		  
typedef B8                                      SNextType;				  		  
static const SNextType							S_NEXT_NO				  = 0;		//从当前开始
static const SNextType							S_NEXT_YES				  = 1;		//从下一个开始
																		  
typedef I32										SErrCodeType;	
static const SErrCodeType						S_ERRCODE_SUCCESS		  = 0;		//登录成功

typedef C8                                      SLicenseType[128+1];	  			//授权类型
typedef U64                                     SDateTimeType;			  			//日期时间
typedef U32                                     SDateType;				  			//日期
typedef U32                                     STimeType;				  			//时间
typedef U16										SDaysType;				  			//日期数
																		  			
typedef U16                                     SLangType;				  			//语言类型
static const SLangType							S_LANG_NONE               = 0x0000;	//未指定
static const SLangType							S_LANG_CHS                = 0x0804;	//简体中文
static const SLangType							S_LANG_CHT                = 0x0404;	//繁体中文
static const SLangType							S_LANG_ENU                = 0x0409;	//英文美国
																				  																				  
//接入服务数据类型-----------------------------------------------------------	  
typedef STR10									SPackageNoType;						//安装包编号	前四位是CompanyNo
typedef STR10									SCompanyNoType;						//公司编号		易盛编制
typedef STR50									SCompanyNameType;					//公司名称		UTF8
typedef STR20									SSiteAddrNoType;					//站点编号		易盛编制
typedef STR50									SSiteAddrNameType;					//站点名称		UTF8
typedef C8										SQuoteAddrType;						//行情地址类型
typedef C8										STradeAddrType;						//交易接入类型	直连、前置、命名服务
typedef STR20									STradeApiType;						//交易API类型	易盛、CTP、金仕达等
typedef C8										STradeUserTypeType;					//交易账户类型
typedef C8										SIsSimulateTradeType;				//模拟或实盘交易地址
typedef I16										STradeAddrOrderType;				//交易地址显示顺序
typedef I16										STradeGroupOrderType;				//交易分组显示顺序
typedef STR10									STradeGroupNoType;					//交易分组号
typedef STR50									STradeGroupNameType;				//交易分组名	UTF8
																					
typedef STR50									SIpType;							//IP地址		长度支持ipv6
typedef U16										SPortType;							//端口号
typedef STR20									SBrokerType;						//CTP用brokerno
typedef STR20									STradeCertType;						//CTP用授权字串
typedef STR20									SUserNoType;						//登录帐号、资金账号
typedef STR20									SPassType;							//登录密码
typedef STR20									SLinkType;							//链接方式

typedef STR20									SWhiteListNameType;					//白名单
typedef U16										SVerfyIndexType;					//签名索引

//云端地址类型（手机端）-----------------------------------------------------
typedef STR20									SAddrTypeNoType;					//公司系统类型
typedef STR50									SAddrTypeNameType;
typedef C8										SSimulateTradeType;					//模拟或实盘
typedef STR20									SCloudSiteNoType;					//云端站点key
typedef STR50									SCloudSiteNameType;				  
																				  
//管理数据类型---------------------------------------------------------------	    
typedef STR10                                   SCurrencyNoType;					//币种编号
typedef F64                                     SExchangeRateType;					//汇率
typedef F64                                     SInterestRateType;					//利率
typedef F64                                     SProportionType;					//折抵
																					
typedef STR10                                   SExchangeNoType;					//交易所编号
typedef STR50                                   SExchangeNameType;					//交易所名称
typedef STR20                                   SCommodityNoType;					//品种编号（含交易所和品种类型）
typedef STR50                                   SCommodityNameType;					//品种名称
typedef C8										SCommodityTypeType;					//品种类型
																					
static const SCommodityTypeType					S_COMMODITYTYPE_NONE	  = 'N';	//无
static const SCommodityTypeType					S_COMMODITYTYPE_SPOT	  = 'P';	//现货
static const SCommodityTypeType					S_COMMODITYTYPE_DEFER	  = 'Y';	//延期
static const SCommodityTypeType					S_COMMODITYTYPE_FUTURES	  = 'F';	//期货
static const SCommodityTypeType					S_COMMODITYTYPE_OPTION	  = 'O';	//期权
static const SCommodityTypeType					S_COMMODITYTYPE_MONTH	  = 'S';	//跨期套利
static const SCommodityTypeType					S_COMMODITYTYPE_COMMODITY = 'M';	//跨品种套利
static const SCommodityTypeType					S_COMMODITYTYPE_BUL		  = 'U';	//看涨垂直套利
static const SCommodityTypeType					S_COMMODITYTYPE_BER		  = 'E';	//看跌垂直套利
static const SCommodityTypeType					S_COMMODITYTYPE_STD		  = 'D';	//跨式套利
static const SCommodityTypeType					S_COMMODITYTYPE_STG		  = 'G';	//宽跨式套利
static const SCommodityTypeType					S_COMMODITYTYPE_PRT		  = 'R';	//备兑组合
static const SCommodityTypeType					S_COMMODITYTYPE_BLT		  = 'L';	//看涨水平期权
static const SCommodityTypeType					S_COMMODITYTYPE_BRT		  = 'Q';	//看跌水平期权
static const SCommodityTypeType					S_COMMODITYTYPE_DIRECT	  = 'X';	//外汇 直接汇率 USD是基础货币 USDxxx
static const SCommodityTypeType					S_COMMODITYTYPE_INDIRECT  = 'I';	//外汇 间接汇率 xxxUSD
static const SCommodityTypeType					S_COMMODITYTYPE_CROSS	  = 'C';	//外汇 交叉汇率 xxxxxx
static const SCommodityTypeType					S_COMMODITYTYPE_INDEX	  = 'Z';	//指数
static const SCommodityTypeType					S_COMMODITYTYPE_STOCK	  = 'T';	//股票
static const SCommodityTypeType					S_COMMODITYTYPE_SPDMON	  = 's';	//极星跨期 SPD|s|SR|801|805
static const SCommodityTypeType					S_COMMODITYTYPE_SPDCOM	  = 'm';	//极星跨品种 SPD|m|A+M2-B|805
static const SCommodityTypeType					S_COMMODITYTYPE_SPDDEFER  = 'y';	//延期 SPD|m|A+M2-B|805

typedef F64                                     SCommodityNumeType;					//最小变动价 分子
typedef U16                                     SCommodityDenoType;					//最小变动价 分母
typedef F64                                     SCommodityTickType;					//最小变动价 分子/分母
typedef U8                                      SCommodityPrecType;					//最小变动价 精度
typedef F32                                     SPriceMultipleType;					//执行价格倍数类型(期权合约描述的执行价乘以此倍数，数量级等同与标的价格)
																					
typedef F64                                     SCommodityDotType;					//商品乘数

typedef C8                                      SCoverModeType;						//平仓方式
static const SCoverModeType						S_COVERMODE_NONE		  = 'N';    //不区分开平
static const SCoverModeType						S_COVERMODE_UNFINISH	  = 'U';    //平仓未了解
static const SCoverModeType						S_COVERMODE_COVER		  = 'C';    //开仓、平仓
static const SCoverModeType						S_COVERMODE_TODAY		  = 'T';    //开仓、平仓、平今
																		  
typedef C8										SSpreadDirectType;		  			//组合方向
static const SSpreadDirectType					S_SPREADDIRECT_NONE		  = '0';	//无
static const SSpreadDirectType					S_SPREADDIRECT_FIRST	  = '1';	//第一腿
static const SSpreadDirectType					S_SPREADDIRECT_SECOND	  = '2';	//第二腿

typedef STR50									SQuoteUnitNameType;					//报价单位名称
typedef STR50									STradingUnitNameType;				//交易单位名称
typedef STR10									SDepositGroupNoType;				//大边保证金组组号

typedef F64                                     SPriceType;							//价格类型
typedef U64                                     SQtyType;							//数量类型
typedef C8                                      SStateType;							//状态类型
typedef C8                                      SStrType[8];						//行情字段短字符串类型
typedef PTR                                     SPtrType;							//指针类型
typedef F64                                     SGreekType;							//希腊字母类型
typedef F64                                     SVolatilityType;					//波动率类型
typedef STR20									SPriceStrType;						//价格格式化为显示的字符串
typedef U16                                     SWidthType;							//宽度类型

typedef STR100                                  SContractNoType;					//定长合约编号
typedef STR100									SContractCodeType;					//合约显示代码
typedef STR100									SContractNameType;					//合约名称
																					
typedef STR50                                   SOptionSeriesNoType;				//期权系列编号
typedef STR100									SOptionSeriesNameType;				//期权系列名称
typedef STR10                                   SStrikePriceType;					//执行价字符串类型
typedef U16                                     SOptionDaysType;					//期权到期天数类型
																					
typedef STR20                                   SPlateNoType;						//板块编号
typedef STR50                                   SPlateNameType;						//板块名称
typedef STR50									SPlateCodeType;						//板块Code
		
typedef C8										SSpreadSrcType;						//套利来源
static const SSpreadSrcType						S_SPREADSRC_NONE		  = 'N';	//无
static const SSpreadSrcType						S_SPREADSRC_ESUNNY		  = 'E';	//极星套利
static const SSpreadSrcType						S_SPREADSRC_SELF		  = 'S';	//自设套利

typedef C8										SSpreadRateModeType;				//套利比例模式
static const SSpreadRateModeType				S_SPREADMODE_DIFF		  = '-';	//价格做差值计算 
static const SSpreadRateModeType				S_SPREADMODE_RATIO		  = '/';	//价格做比值计算
static const SSpreadRateModeType				S_SPREADMODE_SUM		  = '+';	//价格做和值计算

typedef C8										SSpreadExRateModeType;				//汇率引入方式
static const SSpreadExRateModeType				S_SPREADEXMODE_MULTI	  = '*';	//价格汇率做乘积计算
static const SSpreadExRateModeType				S_SPREADEXMODE_RATIO      = '/';	//价格汇率做比值计算
																		  
typedef C8										SSpreadPriorityLeg;					//套利优先腿
static const SSpreadPriorityLeg					S_PRIORITYLEG_FIRST		  = 'F';	//第一腿优先
static const SSpreadPriorityLeg					S_PRIORITYLEG_SECOND	  = 'S';	//第二腿优先

typedef C8										SOptionExerciseType;	  			//期权履约方式
static const SOptionExerciseType				S_OPTIONEXE_NONE		  = 'N';	//无
static const SOptionExerciseType				S_OPTIONEXE_AMERICAN	  = 'A';	//美式期权
static const SOptionExerciseType				S_OPTIONEXE_EUROPEAN	  = 'E';	//欧式期权
static const SOptionExerciseType				S_OPTIONEXE_BERMUDA		  = 'B';	//百慕大期权
																		  
typedef C8										SOptionPropertyType;		  		//期权类型
static const SOptionPropertyType				S_OPTIONTYPE_NONE		  = 'N';	//无
static const SOptionPropertyType				S_OPTIONTYPE_FUTURE		  = 'F';	//期货期权
static const SOptionPropertyType				S_OPTIONTYPE_STOCK		  = 'S';	//股票期权
static const SOptionPropertyType				S_OPTIONTYPE_INDEX		  = 'I';	//指数期权
static const SOptionPropertyType				S_OPTIONTYPE_RATE		  = 'R';	//利率期权
static const SOptionPropertyType				S_OPTIONTYPE_CURRENCY	  = 'C';	//货币期权
																		  
typedef C8                                      SPlateContAttrType;		  			//板块显示虚拟或真实合约
static const SPlateContAttrType					S_PCODE_REAL_CONTRACT	  = 'R';
static const SPlateContAttrType					S_PCODE_VIRTUAL_CONTRACT  = 'V';
static const SPlateContAttrType					S_PCODE_COMMODITY		  = 'C';

//行情数据类型---------------------------------------------------------------
typedef U8										SFidMeanType;			  			//行情字段含义
static const SFidMeanType						S_FID_PRECLOSINGPRICE	  = 0;		//昨收盘价
static const SFidMeanType						S_FID_PRESETTLEPRICE	  = 1;		//昨结算价
static const SFidMeanType						S_FID_PREPOSITIONQTY	  = 2;		//昨持仓量
static const SFidMeanType						S_FID_OPENINGPRICE		  = 3;		//开盘价
static const SFidMeanType						S_FID_LASTPRICE			  = 4;		//最新价
static const SFidMeanType						S_FID_HIGHPRICE			  = 5;		//最高价
static const SFidMeanType						S_FID_LOWPRICE			  = 6;		//最低价
static const SFidMeanType						S_FID_HISHIGHPRICE		  = 7;		//历史最高价
static const SFidMeanType						S_FID_HISLOWPRICE		  = 8;		//历史最低价
static const SFidMeanType						S_FID_LIMITUPPRICE		  = 9;		//涨停价
static const SFidMeanType						S_FID_LIMITDOWNPRICE	  = 10;		//跌停价
static const SFidMeanType						S_FID_TOTALQTY			  = 11;		//当日总成交量
static const SFidMeanType						S_FID_POSITIONQTY		  = 12;		//持仓量
static const SFidMeanType						S_FID_AVERAGEPRICE		  = 13;		//均价
static const SFidMeanType						S_FID_CLOSINGPRICE		  = 14;		//收盘价
static const SFidMeanType						S_FID_SETTLEPRICE		  = 15;		//结算价
static const SFidMeanType						S_FID_LASTQTY			  = 16;		//最新成交量
static const SFidMeanType						S_FID_BESTBIDPRICE		  = 17;		//最优买价
static const SFidMeanType						S_FID_BESTBIDQTY		  = 18;		//最优买量
static const SFidMeanType						S_FID_BESTASKPRICE		  = 19;		//最优卖价
static const SFidMeanType						S_FID_BESTASKQTY		  = 20;		//最优卖量
static const SFidMeanType						S_FID_IMPLIEDBIDPRICE	  = 21;		//隐含买价
static const SFidMeanType						S_FID_IMPLIEDBIDQTY		  = 22;		//隐含买量
static const SFidMeanType						S_FID_IMPLIEDASKPRICE	  = 23;		//隐含卖价
static const SFidMeanType						S_FID_IMPLIEDASKQTY		  = 24;		//隐含卖量
static const SFidMeanType						S_FID_TOTALBIDQTY		  = 25;		//委买总量
static const SFidMeanType						S_FID_TOTALASKQTY		  = 26;		//委卖总量
static const SFidMeanType						S_FID_TOTALTURNOVER		  = 27;		//总成交额
static const SFidMeanType						S_FID_CAPITALIZATION	  = 28;		//总市值
static const SFidMeanType						S_FID_CIRCULATION		  = 29;		//流通市值
static const SFidMeanType						S_FID_THEORETICALPRICE	  = 30;		//理论价
static const SFidMeanType						S_FID_RATIO				  = 31;		//波动率 非价格处理
static const SFidMeanType						S_FID_DELTA				  = 32;		//Delta
static const SFidMeanType						S_FID_GAMMA				  = 33;		//Gamma
static const SFidMeanType						S_FID_VEGA				  = 34;		//Vega
static const SFidMeanType						S_FID_THETA				  = 35;		//Theta
static const SFidMeanType						S_FID_RHO				  = 36;		//Rho
static const SFidMeanType						S_FID_INTRINSICVALUE	  = 37;		//期权内在价值
static const SFidMeanType						S_FID_UNDERLYCONT		  = 38;		//虚拟合约对应的标的合约
static const SFidMeanType						S_FID_SubBidPrice1		  = 39;		//买价1
static const SFidMeanType						S_FID_SubBidPrice2		  = 40;		//买价2
static const SFidMeanType						S_FID_SubBidPrice3		  = 41;		//买价3
static const SFidMeanType						S_FID_SubBidPrice4		  = 42;		//买价4
static const SFidMeanType						S_FID_SubAskPrice1		  = 43;		//卖价1
static const SFidMeanType						S_FID_SubAskPrice2		  = 44;		//卖价2
static const SFidMeanType						S_FID_SubAskPrice3		  = 45;		//卖价3
static const SFidMeanType						S_FID_SubAskPrice4		  = 46;		//卖价4
static const SFidMeanType						S_FID_SubLastPrice1		  = 47;		//最新价1
static const SFidMeanType						S_FID_SubLastPrice2		  = 48;		//最新价2
static const SFidMeanType						S_FID_SubLastPrice3		  = 49;		//最新价3
static const SFidMeanType						S_FID_SubLastPrice4		  = 50;		//最新价4
static const SFidMeanType						S_FID_PreAveragePrice	  = 51;		//昨日均价

static const SFidMeanType						S_FID_TIMEVALUE			  = 111;	//期权时间价值
static const SFidMeanType						S_FID_UPDOWN			  = 112;	//涨跌
static const SFidMeanType						S_FID_GROWTH			  = 113;	//涨幅
static const SFidMeanType						S_FID_NOWINTERST		  = 114;	//增仓
static const SFidMeanType						S_FID_TURNRATE			  = 115;	//换手率

static const SFidMeanType						S_FID_CODE				  = 122;	//合约代码
static const SFidMeanType						S_FID_SRCCODE			  = 123;	//原始合约代码
static const SFidMeanType						S_FID_NAME				  = 124;	//合约名称
static const SFidMeanType						S_FID_DATETIME			  = 125;	//更新时间												   
static const SFidMeanType						S_FID_SPREADRATIO		  = 126;	//套利行情系数

static const SFidMeanType						S_FID_MEAN_COUNT		  = 128;	//字段总数量
																		  
typedef C8										SFidAttrType;			  			//行情字段属性
static const SFidAttrType						S_FIDATTR_NONE			  = 0;		//无值
static const SFidAttrType						S_FIDATTR_VALID			  = 1;		//有值
static const SFidAttrType						S_FIDATTR_IMPLIED		  = 2;		//隐含
																	  			
typedef C8										SFidTypeType;			  			//字段类型类型
static const SFidTypeType						S_FIDTYPE_NONE			  = 0;		//无效
static const SFidTypeType						S_FIDTYPE_PRICE			  = 1;		//价格
static const SFidTypeType						S_FIDTYPE_QTY			  = 2;		//数量
static const SFidTypeType						S_FIDTYPE_GREEK			  = 3;		//希腊字母
static const SFidTypeType						S_FIDTYPE_DATETIME		  = 4;		//日期时间
static const SFidTypeType						S_FIDTYPE_DATE			  = 5;		//日期
static const SFidTypeType						S_FIDTYPE_TIME			  = 6;		//时间
static const SFidTypeType						S_FIDTYPE_STATE			  = 7;		//状态
static const SFidTypeType						S_FIDTYPE_STR			  = 8;		//字符串 最大7字节
static const SFidTypeType						S_FIDTYPE_PTR			  = 9;		//指针

static const SFidTypeType S_FIDTYPE_ARRAY[] =
{
    S_FIDTYPE_PRICE, S_FIDTYPE_PRICE, S_FIDTYPE_QTY  , S_FIDTYPE_PRICE, S_FIDTYPE_PRICE, S_FIDTYPE_PRICE, S_FIDTYPE_PRICE, S_FIDTYPE_PRICE, //0
    S_FIDTYPE_PRICE, S_FIDTYPE_PRICE, S_FIDTYPE_PRICE, S_FIDTYPE_QTY  , S_FIDTYPE_QTY  , S_FIDTYPE_PRICE, S_FIDTYPE_PRICE, S_FIDTYPE_PRICE, //8
    S_FIDTYPE_QTY  , S_FIDTYPE_PRICE, S_FIDTYPE_QTY  , S_FIDTYPE_PRICE, S_FIDTYPE_QTY  , S_FIDTYPE_PRICE, S_FIDTYPE_QTY  , S_FIDTYPE_PRICE, //16
    S_FIDTYPE_QTY  , S_FIDTYPE_QTY  , S_FIDTYPE_QTY  , S_FIDTYPE_PRICE, S_FIDTYPE_PRICE, S_FIDTYPE_PRICE, S_FIDTYPE_PRICE, S_FIDTYPE_GREEK, //24
	S_FIDTYPE_GREEK, S_FIDTYPE_GREEK, S_FIDTYPE_GREEK, S_FIDTYPE_GREEK, S_FIDTYPE_GREEK, S_FIDTYPE_PRICE , S_FIDTYPE_STR , S_FIDTYPE_PRICE, //32
	S_FIDTYPE_PRICE, S_FIDTYPE_PRICE, S_FIDTYPE_PRICE, S_FIDTYPE_PRICE, S_FIDTYPE_PRICE, S_FIDTYPE_PRICE, S_FIDTYPE_PRICE, S_FIDTYPE_PRICE, //40
	S_FIDTYPE_PRICE, S_FIDTYPE_PRICE, S_FIDTYPE_PRICE, S_FIDTYPE_NONE , S_FIDTYPE_NONE , S_FIDTYPE_NONE , S_FIDTYPE_NONE , S_FIDTYPE_NONE , //48
    S_FIDTYPE_NONE , S_FIDTYPE_NONE , S_FIDTYPE_NONE , S_FIDTYPE_NONE , S_FIDTYPE_NONE , S_FIDTYPE_NONE , S_FIDTYPE_NONE , S_FIDTYPE_NONE , //56
    S_FIDTYPE_NONE , S_FIDTYPE_NONE , S_FIDTYPE_NONE , S_FIDTYPE_NONE , S_FIDTYPE_NONE , S_FIDTYPE_NONE , S_FIDTYPE_NONE , S_FIDTYPE_NONE , //64
    S_FIDTYPE_NONE , S_FIDTYPE_NONE , S_FIDTYPE_NONE , S_FIDTYPE_NONE , S_FIDTYPE_NONE , S_FIDTYPE_NONE , S_FIDTYPE_NONE , S_FIDTYPE_NONE , //72
    S_FIDTYPE_NONE , S_FIDTYPE_NONE , S_FIDTYPE_NONE , S_FIDTYPE_NONE , S_FIDTYPE_NONE , S_FIDTYPE_NONE , S_FIDTYPE_NONE , S_FIDTYPE_NONE , //80
    S_FIDTYPE_NONE , S_FIDTYPE_NONE , S_FIDTYPE_NONE , S_FIDTYPE_NONE , S_FIDTYPE_NONE , S_FIDTYPE_NONE , S_FIDTYPE_NONE , S_FIDTYPE_NONE , //88
    S_FIDTYPE_NONE , S_FIDTYPE_NONE , S_FIDTYPE_NONE , S_FIDTYPE_NONE , S_FIDTYPE_NONE , S_FIDTYPE_NONE , S_FIDTYPE_NONE , S_FIDTYPE_NONE , //96
    S_FIDTYPE_NONE , S_FIDTYPE_NONE , S_FIDTYPE_NONE , S_FIDTYPE_NONE , S_FIDTYPE_NONE , S_FIDTYPE_NONE , S_FIDTYPE_NONE , S_FIDTYPE_PRICE, //104
	S_FIDTYPE_PRICE, S_FIDTYPE_PRICE, S_FIDTYPE_QTY  , S_FIDTYPE_PRICE, S_FIDTYPE_NONE , S_FIDTYPE_NONE , S_FIDTYPE_NONE , S_FIDTYPE_NONE , //112
	S_FIDTYPE_NONE , S_FIDTYPE_NONE , S_FIDTYPE_STR  , S_FIDTYPE_STR  , S_FIDTYPE_STR  , S_FIDTYPE_DATETIME, S_FIDTYPE_STR, S_FIDTYPE_NONE, //120
};

typedef U8										SQuoteDepthType;					//深度数量
static const SQuoteDepthType					MAX_L2_DEPTH			  = 10;		//L2最大深度
																		  
//历史行情类型------------------------------	---------------------------------	
typedef C8										STimeBucketTradeStateType;			//交易时段状态 交易时段仅用'3','4','5'，行情交易状态用所有
static const STimeBucketTradeStateType			S_TRADESTATE_BID          = '1';	//集合竞价
static const STimeBucketTradeStateType			S_TRADESTATE_MATCH        = '2';	//集合竞价撮合
static const STimeBucketTradeStateType			S_TRADESTATE_CONTINUOUS   = '3';	//连续交易
static const STimeBucketTradeStateType			S_TRADESTATE_PAUSED       = '4';	//暂停
static const STimeBucketTradeStateType			S_TRADESTATE_CLOSE        = '5';	//闭式
static const STimeBucketTradeStateType			S_TRADESTATE_DEALLAST     = '6';	//闭市处理时间
static const STimeBucketTradeStateType			S_TRADESTATE_SWITCHTRADE  = '0';	//交易日切换时间
static const STimeBucketTradeStateType			S_TRADESTATE_UNKNOWN      = 'N';	//未知状态
static const STimeBucketTradeStateType			S_TRADESTATE_INITIALIZE   = 'I';	//正初始化
static const STimeBucketTradeStateType			S_TRADESTATE_READY        = 'R';	//准备就绪
																		  			
typedef C8										STimeBucketDateFlagType; 			//交易时段日期标志 T-1,T,T+1
static const STimeBucketDateFlagType			S_DATEFLAG_PREDAY         = '0';	//T-1
static const STimeBucketDateFlagType			S_DATEFLAG_CURDAY         = '1';	//T
static const STimeBucketDateFlagType			S_DATEFLAG_NEXTDAY        = '2';	//T+1
																		  			
typedef C8										SKLineTypeType;				 		//k线类型
static const SKLineTypeType						S_KLINE_TICK              = 'T';	//分笔 RawKLineSliceType 为0
static const SKLineTypeType						S_KLINE_MINUTE            = 'M';	//分钟线
static const SKLineTypeType						S_KLINE_DAY               = 'D';	//日线

typedef U8										SKLineSliceType;					//k线片段类型 多秒，多分钟，多日
typedef I32										SKLineIndexType;					//k线索引
typedef U32										SKLineCountType;					//k线数量

typedef I32										SKLineSampleRateType;				//k线采样比例 大于等于1
typedef STR10									STimeBucketTemplateNoType;			//交易时段模板号
typedef C8										STimeBucketIsDstType;				//交易时段是否夏令
typedef I16										STimeBucketIndexType;				//交易时段模板顺序
typedef STR10									STimeBucketBeginTimeType;			//交易时段开始时间
typedef I16										STimeBucketCalCountType;			//交易时段计算分钟数
																					
typedef U32										SSessionIdType;						//订阅会话序号
typedef U32										SLastQtyType;						//明细现手变化
typedef I32										SPositionChgType;					//明细持仓变化

//接入服务控制类型
typedef STR50									SConfigSwitchNoType;
typedef U16										SConfigSwitchValueType;
static const SConfigSwitchNoType				S_CONFIGNO_INNER_FOREIGN			= "Global.Market";				//内外盘
static const SConfigSwitchValueType				S_CONFIGVALUE_INNER					= 0;							//内盘
static const SConfigSwitchValueType				S_CONFIGVALUE_FOREIGN				= 1;							//外盘

static const SConfigSwitchNoType				S_CONFIGNO_COLORUPDOWN				= "Global.Color";				//红涨蓝跌0 蓝涨红跌1
static const SConfigSwitchValueType				S_CONFIGVALUE_UPRED					= 0;							//红涨蓝跌
static const SConfigSwitchValueType				S_CONFIGVALUE_DOWNRED				= 1;							//蓝涨红跌

static const SConfigSwitchNoType				S_CONFIGNO_CANCASHMODE				= "Foreign&Dipper.CanCash";		//今可提(0默认 1永安、中期 2迈克 3中信)
static const SConfigSwitchValueType				S_CONFIGVALUE_DEFAULT				= 0;							//默认
static const SConfigSwitchValueType				S_CONFIGVALUE_YONGAN_ZHONGQI		= 1;							//永安 中期
static const SConfigSwitchValueType				S_CONFIGVALUE_MAIKE					= 2;							//迈克
static const SConfigSwitchValueType				S_CONFIGVALUE_ZHONGXIN				= 3;							//中信

static const SConfigSwitchNoType				S_CONFIGNO_RISKMODE					= "Global.RiskMode";			//风险率（0 保证金/权益  1权益/保证金）
static const SConfigSwitchValueType				S_CONFIGVALUE_DEPOSIT_BALANCE		= 0;							//保证金/权益
static const SConfigSwitchValueType				S_CONFIGVALUE_BALANCE_DEPOSIT		= 1;							//权益/保证金
static const SConfigSwitchValueType				S_CONFIGVALUE_DEPOSITNOF_BALANCE	= 2;							//保证金/权益 无冻结
static const SConfigSwitchValueType				S_CONFIGVALUE_BALANCE_DEPOSITNOF	= 3;							//权益/保证金 无冻结

static const SConfigSwitchNoType				S_CONFIGNO_HISORDER					= "TradeDisplay.HisOrder";		//历史委托
static const SConfigSwitchNoType				S_CONFIGNO_HISMATCH					= "TradeDisplay.HisMatch";		//历史成交
static const SConfigSwitchValueType				S_CONFIGVALUE_DAYS_0				= 0;							//查询天数
static const SConfigSwitchValueType				S_CONFIGVALUE_DAYS_1				= 1;
static const SConfigSwitchValueType				S_CONFIGVALUE_DAYS_3				= 3;
static const SConfigSwitchValueType				S_CONFIGVALUE_DAYS_5				= 5;
static const SConfigSwitchValueType				S_CONFIGVALUE_DAYS_7				= 7;
static const SConfigSwitchValueType				S_CONFIGVALUE_DAYS_15				= 15;
static const SConfigSwitchValueType				S_CONFIGVALUE_DAYS_30				= 30;

static const SConfigSwitchNoType				S_CONFIGNO_VALIDTYPE				= "MouseOrder.ValidType";		//有效类型 按位与
static const SConfigSwitchValueType				S_CONFIGVALUE_GFD					= 1;							//当日有效
static const SConfigSwitchValueType				S_CONFIGVALUE_GTC					= 2;							//长期有效
static const SConfigSwitchValueType				S_CONFIGVALUE_GTD					= 4;							//限期有效
static const SConfigSwitchValueType				S_CONFIGVALUE_FOK					= 8;							//即时全部						
static const SConfigSwitchValueType				S_CONFIGVALUE_IOC					= 16;							//即时部分

static const SConfigSwitchNoType				S_CONFIGNO_SWPSW					= "SWRiskMonitor.Value";		//申万密码
static const SConfigSwitchValueType				S_CONFIGVALUE_PSW					= 2017;							//可编辑  Es****

static const SConfigSwitchNoType				S_CONFIGNO_EQUITYTHRESHOLD			= "PositionDif.EquityThreshold";//仓差监测 权益阈值 （0-3200） 单位万

static const SConfigSwitchNoType				S_CONFIGNO_LADDERORDER				= "MouseOrder.LadderOrder";		//点价
static const SConfigSwitchNoType				S_CONFIGNO_ICEBERG					= "MouseOrder.Iceberg";			//冰山单
static const SConfigSwitchNoType				S_CONFIGNO_GHOST					= "MouseOrder.Ghost";			//影子单
static const SConfigSwitchNoType				S_CONFIGNO_EXE_ENQUIRY				= "MouseOrder.Execute&Enquiry";	//行权 询价
static const SConfigSwitchNoType				S_CONFIGNO_OFFER					= "MouseOrder.Offer";			//应价
static const SConfigSwitchNoType				S_CONFIGNO_REMARK					= "MouseOrder.Remark";			//备注信息
static const SConfigSwitchNoType				S_CONFIGNO_STOP_PROFIT				= "StrategyTrade.Stop&Profit";	//止损止盈
static const SConfigSwitchNoType				S_CONFIGNO_LOCALSPREAD				= "StrategyTrade.ClientSpread";	//本地套利
static const SConfigSwitchNoType				S_CONFIGNO_DRAWLINE					= "StrategyTrade.DrawLine";		//画线
static const SConfigSwitchNoType				S_CONFIGNO_BATCHORDER				= "CommonModule.BatchOrder";	//批量
static const SConfigSwitchNoType				S_CONFIGNO_COMBPOSITION				= "TradeDisplay.CombPosition";	//合并持仓
static const SConfigSwitchNoType				S_CONFIGNO_ENQUIRYLIST				= "TradeDisplay.EnquiryList";	//询价列表
static const SConfigSwitchNoType				S_CONFIGNO_ENTRUSTTIP				= "TradeDisplay.EntrustTip";	//委托提示列
static const SConfigSwitchNoType				S_CONFIGNO_AUTHMONEY				= "TradeDisplay.AuthMoney";		//授信资金
static const SConfigSwitchNoType				S_CONFIGNO_DELETED					= "TradeDisplay.DeletedOrder";	//删除订单
static const SConfigSwitchNoType				S_CONFIGNO_MULTIACC					= "LoginFrm.MutilAcc";			//多账户 开启关闭
static const SConfigSwitchNoType				S_CONFIGNO_POPWINNOTICE				= "CommonModule.PopWinNotice";	//赛事通知
static const SConfigSwitchNoType				S_CONFIGNO_BILLS					= "TradeDisplay.Bills";			//账单查询

static const SConfigSwitchValueType				S_CONFIGVALUE_TURNON				= 0;							//开启
static const SConfigSwitchValueType				S_CONFIGVALUE_TURNOFF				= 1;							//关闭

#pragma pack(push, 1)

//业务数据结构=======================================================================================

//直连交易地址
typedef struct SDirectTradeLink
{
	SCompanyNoType								CompanyNo;							//key = CompanyNo + SiteAddrNo
	SCompanyNameType							CompanyName;
	SSiteAddrNoType								SiteAddrNo;
	SSiteAddrNameType							SiteAddrName;
	STradeAddrType								AddrType;
	STradeApiType								ApiType;
	STradeUserTypeType							UserType;
	SIpType										Ip;
	SPortType									Port;
	SBrokerType									Broker;
	STradeCertType								Cert;
	SIsSimulateTradeType						IsSimulate;
	SUserNoType									UserNo;
	SPassType									Pass;
	STradeAddrOrderType							AddrOrder;
	STradeGroupNoType							GroupNo;
	STradeGroupNameType							GroupName;
	STradeGroupOrderType						GroupOrder;
	SPortType									PortEx;
	SLinkType									LinkType;
} SDirectTradeLink;

//行情地址
typedef struct SQuoteLink
{
	SCompanyNoType								CompanyNo;							//key = CompanyNo + SiteAddrNo
	SCompanyNameType							CompanyName;
	SSiteAddrNoType								SiteAddrNo;
	SSiteAddrNameType							SiteAddrName;
	SQuoteAddrType								AddrType;
	SIpType										Ip;
	SPortType									Port;
	SBrokerType									Broker;
	SUserNoType									UserNo;
	SPassType									Pass;
} SQuoteLink;

//服务信息
typedef struct SServiceInfo
{
	SSrvSrcType									SrvSrc;		
	SSrvEventType								SrvEvent;
	SSrvTradeDataType							SrvTradeData;
	union
	{
		SUserNoType								UserNo;
		SContractNoType							ContractNo;
	};	
	SKLineTypeType								KLineType;
	SSessionIdType								SessionId;
} SServiceInfo;

//币种
typedef struct SCurrency
{
    SCurrencyNoType								CurrencyNo;
    SExchangeRateType							ExchangeRate;
    SInterestRateType							InterestRate;
} SCurrency;

typedef struct SMortgageData
{
	SCurrencyNoType								MortgageNo;							//抵押品编号
	SCurrencyNoType								CurrencyNo;							//折抵货币编号
	SExchangeRateType							ExchangeRate;						//抵押品汇率
	SProportionType								Proportion;							//抵押比例
}SMortgageData;

//交易所
typedef struct SExchange
{
    SExchangeNoType								ExchangeNo;
    SExchangeNameType							ExchangeName;
} SExchange;

//品种
typedef struct SCommodity
{
    SCurrency*									Currency;							//币种指针
    SExchange*									Exchange;							//交易所指针
	union
	{
		struct SCommodity*						TargetCommodity[4];					//关联品种1
		struct SContract*						TargetContract[4];					//关联合约1
	};
	SCommodityNoType							CommodityNo;						//品种代码
	SCommodityTypeType							CommodityType;						//品种类型
    SCommodityNameType							CommodityName;						//品种名称
    SCommodityNumeType							PriceNume;							//分子
    SCommodityDenoType							PriceDeno;							//分母
    SCommodityTickType							PriceTick;							//最小变动价
    SCommodityPrecType							PricePrec;							//价格精度
    SPriceMultipleType							PriceMultiple;						//执行价格倍数
    SCommodityDotType							TradeDot;							//每手乘数
    SCommodityDotType							ExerciseDot;						//执行价倍数
    SCoverModeType								CoverMode;							//平仓方式
    SSpreadDirectType							SpreadDirect;						//组合方向 哪一腿为准
    SOptionExerciseType							OptionExercise;						//期权履约方式
    SOptionPropertyType							OptionProperty;						//期权类型
    SQtyType									MaxSingleOrderQty;					//单笔最大成交量
    SQtyType									MaxPositionQty;						//最大持仓量
	SQuoteUnitNameType							QuoteUnitName;						//报价单位名称
	STradingUnitNameType						TradingUnitName;					//交易单位名称
	SDepositGroupNoType							DepositGroupNo;						//大边保证金组
	SSpreadRateModeType							Operator[3];						//运算符1	
	SPriceType									PriceProp[4];						//价格配比1
	SQtyType									QtyProp[4];							//数量配比1	
} SCommodity;

//合约
typedef struct SContract
{
    SCommodity*									Commodity;
    SContractNoType								ContractNo;
	struct SQuoteSnapShot*						SnapShot;							//可能为空，订阅收到行情才创建
	struct SQuoteSnapShotL2*					BidL2;								//买挂单深度
	struct SQuoteSnapShotL2*					AskL2;								//卖挂单深度
} SContract;

//期权系列
typedef struct SOptionSeries
{
    SCommodity*									Commodity;
    SContract*									TargetContract;
    SOptionSeriesNoType							SeriesNo;
    SDateType									ExpiryDate;							//到期日期
    SOptionDaysType								ExpiryDays;							//到期天数
} SOptionSeries;

//期权合约对
typedef struct SOptionContractPair
{
    SStrikePriceType							StrikePrice;
    SContract*									Contract1;
    SContract*									Contract2;
} SOptionContractPair;

//行情板块
typedef struct SPlate
{
    SPlateNoType								PlateNo;
    SPlateNameType								PlateName;
	SPlateNoType								ParentPlateNo;
} SPlate;

//板块品种合约
typedef struct SPlateContent
{
	SPlateNoType								PlateNo;
    SPlateContAttrType							ContAttr;
	SPlateCodeType								PlateCode;
} SPlateContent;

//行情快照
typedef struct SQuoteField
{
    union
    {
        SFidMeanType							FidMean;							//变化行情使用标识
        SFidAttrType							FidAttr;							//固定行情使用属性
    };
    union
    {
        SPriceType								Price;
        SQtyType								Qty;
        SGreekType								Greek;
        SVolatilityType							Volatility;
        SDateTimeType							DateTime;
        SDateType								Date;
        STimeType								Time;
        SStateType								State;
        SStrType								Str;
        SPtrType								Ptr;
    };
} SQuoteField;

//行情变化结构
typedef struct SQuoteUpdate																		
{
	SDateTimeType								DateTime;
	SFidMeanType								Count;
	SQuoteField									Field[1];
} SQuoteUpdate;

typedef struct SQuoteSnapShot
{
    SQuoteField									Data[S_FID_MEAN_COUNT];
} SQuoteSnapShot;

//行情快照L2
typedef struct SQuoteFieldL2
{
    SPriceType									Price;
    SQtyType									Qty;
} SQuoteFieldL2;

typedef struct SQuoteSnapShotL2
{
    SQuoteFieldL2								Data[MAX_L2_DEPTH];
} SQuoteSnapShotL2;

typedef struct SImpliedDepthL2
{
	SQuoteDepthType								BidPriceDepth;
	SQuoteDepthType								BidQtyDepth;
	SQuoteDepthType								AskPriceDepth;
	SQuoteDepthType								AskQtyDepth;
} SQuoteSnapShotL2Flag;

typedef struct SAddSPreadInfo
{
	SContractNoType								ContractNo[4];					//合约
	SSpreadRateModeType							RateMode[3];					//套利计算方式1   
	SPriceType									SpreadRate[4];					//套利价格比例1
	SQtyType									SpreadQty[4];					//套利数量比例1
	SSpreadPriorityLeg							PriorityLeg;					//优先腿 主动腿
	SQtyType									ValidLegs;						//有效腿数
	SSpreadExRateModeType						RateContractMode[4];			//汇率关系  只支持* /
	SContractNoType								RateContractNo[4];				//外汇合约
}SAddSPreadInfo;

//套利合约关键字为 Contract1 + RateMode1 + Contract2 + RateMode2 + Contract3 + RateMode3 + Contract4
typedef struct SSpreadContract
{
	SContractNoType								ContractNo;
	SContract*									Contract[4];
	SSpreadRateModeType							RateMode[3];					//套利计算方式1  
	SPriceType									SpreadRate[4];					//套利价格比例1
	SQtyType									SpreadQty[4];					//套利数量比例1
	SSpreadPriorityLeg							PriorityLeg;					//优先腿 主动腿
	SQtyType									ValidLegs;						//有效腿数
	SCommodityTickType							PriceTick;						//最小变动价
	SCommodityPrecType							PricePrec;						//价格精度
	SCoverModeType								CoverMode;
	SQuoteSnapShot*								SnapShot;						//套利行情快照
	SSpreadExRateModeType						RateContractMode[4];			//汇率关系  只支持* /
	SContract*									RateContract[4];				//汇率合约
} SSpreadContract;

//历史行情
typedef struct SHisQuoteTimeBucket
{
    STimeBucketIndexType						Index;
    STimeType									BeginTime;
    STimeType									EndTime;
    STimeBucketTradeStateType					TradeState;
    STimeBucketDateFlagType						DateFlag;
    STimeBucketCalCountType						CalCount;						//基础模版对应计算模版的分钟数
} SHisQuoteTimeBucket;

typedef struct SHisQuoteData //sizeof 80字节	
{
    SKLineIndexType								KLineIndex;						//K线索引|tick每笔连续序号，min交易分钟序号，day无效
    SDateType									TradeDate;						//交易日|tick无效，min可能和时间戳不同，day和时间戳相同
    SDateTimeType								DateTimeStamp;					//时间戳|不同数据类型，精度不同
    
    SQtyType									QTotalQty;						//行情快照|总成交量
    SQtyType									QPositionQty;					//行情快照|持仓量
    SPriceType									QLastPrice;						//最新价（收盘价）
    
    union
    {
        struct
        {
            SQtyType							QKLineQty;						//K线成交量     day  min
            SPriceType							QOpeningPrice;					//开盘价		day  min
            SPriceType							QHighPrice;						//最高价		day  min
            SPriceType							QLowPrice;						//最低价		day  min
            SPriceType							QSettlePrice;					//结算价		day  min
            
        };
        struct
        {
            SLastQtyType						QLastQty;						//明细现手      tick
            SPositionChgType					QPositionChg;					//持仓量变化	tick
            SPriceType							QBuyPrice;						//买价			tick
            SPriceType							QSellPrice;						//卖价			tick
            SQtyType							QBuyQty;						//买量			tick
            SQtyType							QSellQty;						//卖量			tick
        };
    };
} SHisQuoteData;

typedef struct SHisVolatilityData //sizeof  字节	
{
	SKLineIndexType								KLineIndex;						//K线索引|tick每笔连续序号，min交易分钟序号，day无效
	SDateType									TradeDate;						//交易日|tick无效，min可能和时间戳不同，day和时间戳相同
	SDateTimeType								DateTimeStamp;					//时间戳|不同数据类型，精度不同

	SPriceType									TheoryPrice;					//理论价
	SVolatilityType								Sigma;							//隐含波动率
	SGreekType									Delta;							//期权价格变动与其标的资产价格变动的比率
	SGreekType									Gamma;							//delta的变化与其标的资产价格变化的比率
	SGreekType									Vega;							//交易组合价值变化与其标的资产波动率变化的比率
	SGreekType									Theta;							//交易组合价值变化与时间变化的比率
	SGreekType									Rho;							//交易组合价值对于利率变化的敏感性

} SHisVolatilityData;

//期权计算
typedef struct SOptionPriceRet
{
	SPriceType									CallPrice;
	SPriceType									PutPrice;
	SGreekType									CallDelta;
	SGreekType									PutDelta;
	SGreekType									CallGamma;
	SGreekType									PutGamma;
	SGreekType									CallTheta;
	SGreekType									PutTheta;
	SGreekType									CallVega;
	SGreekType									PutVega;
	SGreekType									CallRho;
	SGreekType									PutRho;
} SOptionPriceRet;

//云端交易公司 key=CompanyNo+AddrTypeNo
typedef struct SCloudTradeCompany
{
    SCompanyNoType								CompanyNo;
    SCompanyNameType							CompanyName;
    SAddrTypeNoType								AddrTypeNo;
    SAddrTypeNameType							AddrTypeName;
    STradeApiType								TradeApi;
    SSimulateTradeType							SimulateTrade;
} SCloudTradeCompany;

//云端交易站点 key=SiteNo
typedef struct SCloudTradeSite
{
	SCloudSiteNoType							SiteNo;
	SCloudSiteNameType							SiteName;
	SIpType										Ip;
	SPortType									Port;
} SCloudTradeSite;

//接入服务控制开关
typedef struct SConfigSwitchInfo
{
	SConfigSwitchNoType							ConfigSwitchNo;
	SConfigSwitchValueType						ConfigSwitchValue;
} SConfigSwitchInfo;

//插件白名单
typedef struct SWhiteListInfo
{
	SWhiteListNameType							WhiteListName;
	SVerfyIndexType								VerfyIndex;
} SWhiteListInfo;

//代码映射
typedef struct SCodeMappingInfo
{
	SCommodityNoType							SrcCode;
	SCommodityNoType							ShowCode;
	SPriceType									PriceMultiple;
} SCodeMappingInfo;

//期权执行价格式化参数
typedef struct SOptionPrecisionInfo
{
	SCommodityNoType							CommodityNo;
	SPriceType									PriceMultiple;
	SWidthType									IntegerWidth;
	SWidthType									TotalWidth;
} SOptionPrecisionInfo;

#pragma pack(pop)

#endif /* StarApi_Type_h */
