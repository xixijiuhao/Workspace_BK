#ifndef StarApi_TYPE_h
#define StarApi_TYPE_h

//������������=======================================================================================
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

//ҵ����������=======================================================================================
typedef I32                                     SRetType;							//����ֵ����

typedef U8                                      SSrvSrcType;						//��������
static const SSrvSrcType						S_SRVSRC_QUOTE			  = 'Q';	//�������
static const SSrvSrcType						S_SRVSRC_HISQUOTE		  = 'H';	//��ʷ�������
static const SSrvSrcType						S_SRVSRC_TRADE			  = 'T';	//���׷���
																		  
typedef U8                                      SSrvEventType;			  			//�����¼�����
static const SSrvEventType						S_SRVEVENT_CONNECT		  = 'C';	//���� 
static const SSrvEventType						S_SRVEVENT_DISCONNECT	  = 'D';	//�Ͽ�  
static const SSrvEventType						S_SRVEVENT_LOGIN		  = 'L';	//��¼ 
static const SSrvEventType						S_SRVEVENT_INITCOMPLETED  = 'I';	//��ʼ����� 
static const SSrvEventType						S_SRVEVENT_QUOTE		  = 'Q';	//��ʱ���� 
static const SSrvEventType						S_SRVEVENT_HISQUOTE		  = 'H';	//��ʷ����
static const SSrvEventType						S_SRVEVENT_TRADE		  = 'T';	//����
static const SSrvEventType						S_SRVEVENT_TRADE_ORDER	  = 'o';	//����ί�б仯
static const SSrvEventType						S_SRVEVENT_TRADE_MATCH	  = 'm';	//���׳ɽ��仯
static const SSrvEventType						S_SRVEVENT_TRADE_POSITION = 'p';	//���׳ֱֲ仯
static const SSrvEventType						S_SRVEVENT_TRADE_FUND	  = 'f';	//�����ʽ�仯

typedef PTR										SSrvTradeDataType;					//����������

typedef C8*                                     SDataType;				  		  
typedef U32                                     SLenType;				  		  
typedef I32                                     SIndexType;				  		  
typedef B8                                      SNextType;				  		  
static const SNextType							S_NEXT_NO				  = 0;		//�ӵ�ǰ��ʼ
static const SNextType							S_NEXT_YES				  = 1;		//����һ����ʼ
																		  
typedef I32										SErrCodeType;	
static const SErrCodeType						S_ERRCODE_SUCCESS		  = 0;		//��¼�ɹ�

typedef C8                                      SLicenseType[128+1];	  			//��Ȩ����
typedef U64                                     SDateTimeType;			  			//����ʱ��
typedef U32                                     SDateType;				  			//����
typedef U32                                     STimeType;				  			//ʱ��
typedef U16										SDaysType;				  			//������
																		  			
typedef U16                                     SLangType;				  			//��������
static const SLangType							S_LANG_NONE               = 0x0000;	//δָ��
static const SLangType							S_LANG_CHS                = 0x0804;	//��������
static const SLangType							S_LANG_CHT                = 0x0404;	//��������
static const SLangType							S_LANG_ENU                = 0x0409;	//Ӣ������
																				  																				  
//���������������-----------------------------------------------------------	  
typedef STR10									SPackageNoType;						//��װ�����	ǰ��λ��CompanyNo
typedef STR10									SCompanyNoType;						//��˾���		��ʢ����
typedef STR50									SCompanyNameType;					//��˾����		UTF8
typedef STR20									SSiteAddrNoType;					//վ����		��ʢ����
typedef STR50									SSiteAddrNameType;					//վ������		UTF8
typedef C8										SQuoteAddrType;						//�����ַ����
typedef C8										STradeAddrType;						//���׽�������	ֱ����ǰ�á���������
typedef STR20									STradeApiType;						//����API����	��ʢ��CTP�����˴��
typedef C8										STradeUserTypeType;					//�����˻�����
typedef C8										SIsSimulateTradeType;				//ģ���ʵ�̽��׵�ַ
typedef I16										STradeAddrOrderType;				//���׵�ַ��ʾ˳��
typedef I16										STradeGroupOrderType;				//���׷�����ʾ˳��
typedef STR10									STradeGroupNoType;					//���׷����
typedef STR50									STradeGroupNameType;				//���׷�����	UTF8
																					
typedef STR50									SIpType;							//IP��ַ		����֧��ipv6
typedef U16										SPortType;							//�˿ں�
typedef STR20									SBrokerType;						//CTP��brokerno
typedef STR20									STradeCertType;						//CTP����Ȩ�ִ�
typedef STR20									SUserNoType;						//��¼�ʺš��ʽ��˺�
typedef STR20									SPassType;							//��¼����
typedef STR20									SLinkType;							//���ӷ�ʽ

typedef STR20									SWhiteListNameType;					//������
typedef U16										SVerfyIndexType;					//ǩ������

//�ƶ˵�ַ���ͣ��ֻ��ˣ�-----------------------------------------------------
typedef STR20									SAddrTypeNoType;					//��˾ϵͳ����
typedef STR50									SAddrTypeNameType;
typedef C8										SSimulateTradeType;					//ģ���ʵ��
typedef STR20									SCloudSiteNoType;					//�ƶ�վ��key
typedef STR50									SCloudSiteNameType;				  
																				  
//������������---------------------------------------------------------------	    
typedef STR10                                   SCurrencyNoType;					//���ֱ��
typedef F64                                     SExchangeRateType;					//����
typedef F64                                     SInterestRateType;					//����
typedef F64                                     SProportionType;					//�۵�
																					
typedef STR10                                   SExchangeNoType;					//���������
typedef STR50                                   SExchangeNameType;					//����������
typedef STR20                                   SCommodityNoType;					//Ʒ�ֱ�ţ�����������Ʒ�����ͣ�
typedef STR50                                   SCommodityNameType;					//Ʒ������
typedef C8										SCommodityTypeType;					//Ʒ������
																					
static const SCommodityTypeType					S_COMMODITYTYPE_NONE	  = 'N';	//��
static const SCommodityTypeType					S_COMMODITYTYPE_SPOT	  = 'P';	//�ֻ�
static const SCommodityTypeType					S_COMMODITYTYPE_DEFER	  = 'Y';	//����
static const SCommodityTypeType					S_COMMODITYTYPE_FUTURES	  = 'F';	//�ڻ�
static const SCommodityTypeType					S_COMMODITYTYPE_OPTION	  = 'O';	//��Ȩ
static const SCommodityTypeType					S_COMMODITYTYPE_MONTH	  = 'S';	//��������
static const SCommodityTypeType					S_COMMODITYTYPE_COMMODITY = 'M';	//��Ʒ������
static const SCommodityTypeType					S_COMMODITYTYPE_BUL		  = 'U';	//���Ǵ�ֱ����
static const SCommodityTypeType					S_COMMODITYTYPE_BER		  = 'E';	//������ֱ����
static const SCommodityTypeType					S_COMMODITYTYPE_STD		  = 'D';	//��ʽ����
static const SCommodityTypeType					S_COMMODITYTYPE_STG		  = 'G';	//���ʽ����
static const SCommodityTypeType					S_COMMODITYTYPE_PRT		  = 'R';	//�������
static const SCommodityTypeType					S_COMMODITYTYPE_BLT		  = 'L';	//����ˮƽ��Ȩ
static const SCommodityTypeType					S_COMMODITYTYPE_BRT		  = 'Q';	//����ˮƽ��Ȩ
static const SCommodityTypeType					S_COMMODITYTYPE_DIRECT	  = 'X';	//��� ֱ�ӻ��� USD�ǻ������� USDxxx
static const SCommodityTypeType					S_COMMODITYTYPE_INDIRECT  = 'I';	//��� ��ӻ��� xxxUSD
static const SCommodityTypeType					S_COMMODITYTYPE_CROSS	  = 'C';	//��� ������� xxxxxx
static const SCommodityTypeType					S_COMMODITYTYPE_INDEX	  = 'Z';	//ָ��
static const SCommodityTypeType					S_COMMODITYTYPE_STOCK	  = 'T';	//��Ʊ
static const SCommodityTypeType					S_COMMODITYTYPE_SPDMON	  = 's';	//���ǿ��� SPD|s|SR|801|805
static const SCommodityTypeType					S_COMMODITYTYPE_SPDCOM	  = 'm';	//���ǿ�Ʒ�� SPD|m|A+M2-B|805
static const SCommodityTypeType					S_COMMODITYTYPE_SPDDEFER  = 'y';	//���� SPD|m|A+M2-B|805

typedef F64                                     SCommodityNumeType;					//��С�䶯�� ����
typedef U16                                     SCommodityDenoType;					//��С�䶯�� ��ĸ
typedef F64                                     SCommodityTickType;					//��С�䶯�� ����/��ĸ
typedef U8                                      SCommodityPrecType;					//��С�䶯�� ����
typedef F32                                     SPriceMultipleType;					//ִ�м۸�������(��Ȩ��Լ������ִ�м۳��Դ˱�������������ͬ���ļ۸�)
																					
typedef F64                                     SCommodityDotType;					//��Ʒ����

typedef C8                                      SCoverModeType;						//ƽ�ַ�ʽ
static const SCoverModeType						S_COVERMODE_NONE		  = 'N';    //�����ֿ�ƽ
static const SCoverModeType						S_COVERMODE_UNFINISH	  = 'U';    //ƽ��δ�˽�
static const SCoverModeType						S_COVERMODE_COVER		  = 'C';    //���֡�ƽ��
static const SCoverModeType						S_COVERMODE_TODAY		  = 'T';    //���֡�ƽ�֡�ƽ��
																		  
typedef C8										SSpreadDirectType;		  			//��Ϸ���
static const SSpreadDirectType					S_SPREADDIRECT_NONE		  = '0';	//��
static const SSpreadDirectType					S_SPREADDIRECT_FIRST	  = '1';	//��һ��
static const SSpreadDirectType					S_SPREADDIRECT_SECOND	  = '2';	//�ڶ���

typedef STR50									SQuoteUnitNameType;					//���۵�λ����
typedef STR50									STradingUnitNameType;				//���׵�λ����
typedef STR10									SDepositGroupNoType;				//��߱�֤�������

typedef F64                                     SPriceType;							//�۸�����
typedef U64                                     SQtyType;							//��������
typedef C8                                      SStateType;							//״̬����
typedef C8                                      SStrType[8];						//�����ֶζ��ַ�������
typedef PTR                                     SPtrType;							//ָ������
typedef F64                                     SGreekType;							//ϣ����ĸ����
typedef F64                                     SVolatilityType;					//����������
typedef STR20									SPriceStrType;						//�۸��ʽ��Ϊ��ʾ���ַ���
typedef U16                                     SWidthType;							//�������

typedef STR100                                  SContractNoType;					//������Լ���
typedef STR100									SContractCodeType;					//��Լ��ʾ����
typedef STR100									SContractNameType;					//��Լ����
																					
typedef STR50                                   SOptionSeriesNoType;				//��Ȩϵ�б��
typedef STR100									SOptionSeriesNameType;				//��Ȩϵ������
typedef STR10                                   SStrikePriceType;					//ִ�м��ַ�������
typedef U16                                     SOptionDaysType;					//��Ȩ������������
																					
typedef STR20                                   SPlateNoType;						//�����
typedef STR50                                   SPlateNameType;						//�������
typedef STR50									SPlateCodeType;						//���Code
		
typedef C8										SSpreadSrcType;						//������Դ
static const SSpreadSrcType						S_SPREADSRC_NONE		  = 'N';	//��
static const SSpreadSrcType						S_SPREADSRC_ESUNNY		  = 'E';	//��������
static const SSpreadSrcType						S_SPREADSRC_SELF		  = 'S';	//��������

typedef C8										SSpreadRateModeType;				//��������ģʽ
static const SSpreadRateModeType				S_SPREADMODE_DIFF		  = '-';	//�۸�����ֵ���� 
static const SSpreadRateModeType				S_SPREADMODE_RATIO		  = '/';	//�۸�����ֵ����
static const SSpreadRateModeType				S_SPREADMODE_SUM		  = '+';	//�۸�����ֵ����

typedef C8										SSpreadExRateModeType;				//�������뷽ʽ
static const SSpreadExRateModeType				S_SPREADEXMODE_MULTI	  = '*';	//�۸�������˻�����
static const SSpreadExRateModeType				S_SPREADEXMODE_RATIO      = '/';	//�۸��������ֵ����
																		  
typedef C8										SSpreadPriorityLeg;					//����������
static const SSpreadPriorityLeg					S_PRIORITYLEG_FIRST		  = 'F';	//��һ������
static const SSpreadPriorityLeg					S_PRIORITYLEG_SECOND	  = 'S';	//�ڶ�������

typedef C8										SOptionExerciseType;	  			//��Ȩ��Լ��ʽ
static const SOptionExerciseType				S_OPTIONEXE_NONE		  = 'N';	//��
static const SOptionExerciseType				S_OPTIONEXE_AMERICAN	  = 'A';	//��ʽ��Ȩ
static const SOptionExerciseType				S_OPTIONEXE_EUROPEAN	  = 'E';	//ŷʽ��Ȩ
static const SOptionExerciseType				S_OPTIONEXE_BERMUDA		  = 'B';	//��Ľ����Ȩ
																		  
typedef C8										SOptionPropertyType;		  		//��Ȩ����
static const SOptionPropertyType				S_OPTIONTYPE_NONE		  = 'N';	//��
static const SOptionPropertyType				S_OPTIONTYPE_FUTURE		  = 'F';	//�ڻ���Ȩ
static const SOptionPropertyType				S_OPTIONTYPE_STOCK		  = 'S';	//��Ʊ��Ȩ
static const SOptionPropertyType				S_OPTIONTYPE_INDEX		  = 'I';	//ָ����Ȩ
static const SOptionPropertyType				S_OPTIONTYPE_RATE		  = 'R';	//������Ȩ
static const SOptionPropertyType				S_OPTIONTYPE_CURRENCY	  = 'C';	//������Ȩ
																		  
typedef C8                                      SPlateContAttrType;		  			//�����ʾ�������ʵ��Լ
static const SPlateContAttrType					S_PCODE_REAL_CONTRACT	  = 'R';
static const SPlateContAttrType					S_PCODE_VIRTUAL_CONTRACT  = 'V';
static const SPlateContAttrType					S_PCODE_COMMODITY		  = 'C';

//������������---------------------------------------------------------------
typedef U8										SFidMeanType;			  			//�����ֶκ���
static const SFidMeanType						S_FID_PRECLOSINGPRICE	  = 0;		//�����̼�
static const SFidMeanType						S_FID_PRESETTLEPRICE	  = 1;		//������
static const SFidMeanType						S_FID_PREPOSITIONQTY	  = 2;		//��ֲ���
static const SFidMeanType						S_FID_OPENINGPRICE		  = 3;		//���̼�
static const SFidMeanType						S_FID_LASTPRICE			  = 4;		//���¼�
static const SFidMeanType						S_FID_HIGHPRICE			  = 5;		//��߼�
static const SFidMeanType						S_FID_LOWPRICE			  = 6;		//��ͼ�
static const SFidMeanType						S_FID_HISHIGHPRICE		  = 7;		//��ʷ��߼�
static const SFidMeanType						S_FID_HISLOWPRICE		  = 8;		//��ʷ��ͼ�
static const SFidMeanType						S_FID_LIMITUPPRICE		  = 9;		//��ͣ��
static const SFidMeanType						S_FID_LIMITDOWNPRICE	  = 10;		//��ͣ��
static const SFidMeanType						S_FID_TOTALQTY			  = 11;		//�����ܳɽ���
static const SFidMeanType						S_FID_POSITIONQTY		  = 12;		//�ֲ���
static const SFidMeanType						S_FID_AVERAGEPRICE		  = 13;		//����
static const SFidMeanType						S_FID_CLOSINGPRICE		  = 14;		//���̼�
static const SFidMeanType						S_FID_SETTLEPRICE		  = 15;		//�����
static const SFidMeanType						S_FID_LASTQTY			  = 16;		//���³ɽ���
static const SFidMeanType						S_FID_BESTBIDPRICE		  = 17;		//�������
static const SFidMeanType						S_FID_BESTBIDQTY		  = 18;		//��������
static const SFidMeanType						S_FID_BESTASKPRICE		  = 19;		//��������
static const SFidMeanType						S_FID_BESTASKQTY		  = 20;		//��������
static const SFidMeanType						S_FID_IMPLIEDBIDPRICE	  = 21;		//�������
static const SFidMeanType						S_FID_IMPLIEDBIDQTY		  = 22;		//��������
static const SFidMeanType						S_FID_IMPLIEDASKPRICE	  = 23;		//��������
static const SFidMeanType						S_FID_IMPLIEDASKQTY		  = 24;		//��������
static const SFidMeanType						S_FID_TOTALBIDQTY		  = 25;		//ί������
static const SFidMeanType						S_FID_TOTALASKQTY		  = 26;		//ί������
static const SFidMeanType						S_FID_TOTALTURNOVER		  = 27;		//�ܳɽ���
static const SFidMeanType						S_FID_CAPITALIZATION	  = 28;		//����ֵ
static const SFidMeanType						S_FID_CIRCULATION		  = 29;		//��ͨ��ֵ
static const SFidMeanType						S_FID_THEORETICALPRICE	  = 30;		//���ۼ�
static const SFidMeanType						S_FID_RATIO				  = 31;		//������ �Ǽ۸���
static const SFidMeanType						S_FID_DELTA				  = 32;		//Delta
static const SFidMeanType						S_FID_GAMMA				  = 33;		//Gamma
static const SFidMeanType						S_FID_VEGA				  = 34;		//Vega
static const SFidMeanType						S_FID_THETA				  = 35;		//Theta
static const SFidMeanType						S_FID_RHO				  = 36;		//Rho
static const SFidMeanType						S_FID_INTRINSICVALUE	  = 37;		//��Ȩ���ڼ�ֵ
static const SFidMeanType						S_FID_UNDERLYCONT		  = 38;		//�����Լ��Ӧ�ı�ĺ�Լ
static const SFidMeanType						S_FID_SubBidPrice1		  = 39;		//���1
static const SFidMeanType						S_FID_SubBidPrice2		  = 40;		//���2
static const SFidMeanType						S_FID_SubBidPrice3		  = 41;		//���3
static const SFidMeanType						S_FID_SubBidPrice4		  = 42;		//���4
static const SFidMeanType						S_FID_SubAskPrice1		  = 43;		//����1
static const SFidMeanType						S_FID_SubAskPrice2		  = 44;		//����2
static const SFidMeanType						S_FID_SubAskPrice3		  = 45;		//����3
static const SFidMeanType						S_FID_SubAskPrice4		  = 46;		//����4
static const SFidMeanType						S_FID_SubLastPrice1		  = 47;		//���¼�1
static const SFidMeanType						S_FID_SubLastPrice2		  = 48;		//���¼�2
static const SFidMeanType						S_FID_SubLastPrice3		  = 49;		//���¼�3
static const SFidMeanType						S_FID_SubLastPrice4		  = 50;		//���¼�4
static const SFidMeanType						S_FID_PreAveragePrice	  = 51;		//���վ���

static const SFidMeanType						S_FID_TIMEVALUE			  = 111;	//��Ȩʱ���ֵ
static const SFidMeanType						S_FID_UPDOWN			  = 112;	//�ǵ�
static const SFidMeanType						S_FID_GROWTH			  = 113;	//�Ƿ�
static const SFidMeanType						S_FID_NOWINTERST		  = 114;	//����
static const SFidMeanType						S_FID_TURNRATE			  = 115;	//������

static const SFidMeanType						S_FID_CODE				  = 122;	//��Լ����
static const SFidMeanType						S_FID_SRCCODE			  = 123;	//ԭʼ��Լ����
static const SFidMeanType						S_FID_NAME				  = 124;	//��Լ����
static const SFidMeanType						S_FID_DATETIME			  = 125;	//����ʱ��												   
static const SFidMeanType						S_FID_SPREADRATIO		  = 126;	//��������ϵ��

static const SFidMeanType						S_FID_MEAN_COUNT		  = 128;	//�ֶ�������
																		  
typedef C8										SFidAttrType;			  			//�����ֶ�����
static const SFidAttrType						S_FIDATTR_NONE			  = 0;		//��ֵ
static const SFidAttrType						S_FIDATTR_VALID			  = 1;		//��ֵ
static const SFidAttrType						S_FIDATTR_IMPLIED		  = 2;		//����
																	  			
typedef C8										SFidTypeType;			  			//�ֶ���������
static const SFidTypeType						S_FIDTYPE_NONE			  = 0;		//��Ч
static const SFidTypeType						S_FIDTYPE_PRICE			  = 1;		//�۸�
static const SFidTypeType						S_FIDTYPE_QTY			  = 2;		//����
static const SFidTypeType						S_FIDTYPE_GREEK			  = 3;		//ϣ����ĸ
static const SFidTypeType						S_FIDTYPE_DATETIME		  = 4;		//����ʱ��
static const SFidTypeType						S_FIDTYPE_DATE			  = 5;		//����
static const SFidTypeType						S_FIDTYPE_TIME			  = 6;		//ʱ��
static const SFidTypeType						S_FIDTYPE_STATE			  = 7;		//״̬
static const SFidTypeType						S_FIDTYPE_STR			  = 8;		//�ַ��� ���7�ֽ�
static const SFidTypeType						S_FIDTYPE_PTR			  = 9;		//ָ��

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

typedef U8										SQuoteDepthType;					//�������
static const SQuoteDepthType					MAX_L2_DEPTH			  = 10;		//L2������
																		  
//��ʷ��������------------------------------	---------------------------------	
typedef C8										STimeBucketTradeStateType;			//����ʱ��״̬ ����ʱ�ν���'3','4','5'�����齻��״̬������
static const STimeBucketTradeStateType			S_TRADESTATE_BID          = '1';	//���Ͼ���
static const STimeBucketTradeStateType			S_TRADESTATE_MATCH        = '2';	//���Ͼ��۴��
static const STimeBucketTradeStateType			S_TRADESTATE_CONTINUOUS   = '3';	//��������
static const STimeBucketTradeStateType			S_TRADESTATE_PAUSED       = '4';	//��ͣ
static const STimeBucketTradeStateType			S_TRADESTATE_CLOSE        = '5';	//��ʽ
static const STimeBucketTradeStateType			S_TRADESTATE_DEALLAST     = '6';	//���д���ʱ��
static const STimeBucketTradeStateType			S_TRADESTATE_SWITCHTRADE  = '0';	//�������л�ʱ��
static const STimeBucketTradeStateType			S_TRADESTATE_UNKNOWN      = 'N';	//δ֪״̬
static const STimeBucketTradeStateType			S_TRADESTATE_INITIALIZE   = 'I';	//����ʼ��
static const STimeBucketTradeStateType			S_TRADESTATE_READY        = 'R';	//׼������
																		  			
typedef C8										STimeBucketDateFlagType; 			//����ʱ�����ڱ�־ T-1,T,T+1
static const STimeBucketDateFlagType			S_DATEFLAG_PREDAY         = '0';	//T-1
static const STimeBucketDateFlagType			S_DATEFLAG_CURDAY         = '1';	//T
static const STimeBucketDateFlagType			S_DATEFLAG_NEXTDAY        = '2';	//T+1
																		  			
typedef C8										SKLineTypeType;				 		//k������
static const SKLineTypeType						S_KLINE_TICK              = 'T';	//�ֱ� RawKLineSliceType Ϊ0
static const SKLineTypeType						S_KLINE_MINUTE            = 'M';	//������
static const SKLineTypeType						S_KLINE_DAY               = 'D';	//����

typedef U8										SKLineSliceType;					//k��Ƭ������ ���룬����ӣ�����
typedef I32										SKLineIndexType;					//k������
typedef U32										SKLineCountType;					//k������

typedef I32										SKLineSampleRateType;				//k�߲������� ���ڵ���1
typedef STR10									STimeBucketTemplateNoType;			//����ʱ��ģ���
typedef C8										STimeBucketIsDstType;				//����ʱ���Ƿ�����
typedef I16										STimeBucketIndexType;				//����ʱ��ģ��˳��
typedef STR10									STimeBucketBeginTimeType;			//����ʱ�ο�ʼʱ��
typedef I16										STimeBucketCalCountType;			//����ʱ�μ��������
																					
typedef U32										SSessionIdType;						//���ĻỰ���
typedef U32										SLastQtyType;						//��ϸ���ֱ仯
typedef I32										SPositionChgType;					//��ϸ�ֱֲ仯

//��������������
typedef STR50									SConfigSwitchNoType;
typedef U16										SConfigSwitchValueType;
static const SConfigSwitchNoType				S_CONFIGNO_INNER_FOREIGN			= "Global.Market";				//������
static const SConfigSwitchValueType				S_CONFIGVALUE_INNER					= 0;							//����
static const SConfigSwitchValueType				S_CONFIGVALUE_FOREIGN				= 1;							//����

static const SConfigSwitchNoType				S_CONFIGNO_COLORUPDOWN				= "Global.Color";				//��������0 ���Ǻ��1
static const SConfigSwitchValueType				S_CONFIGVALUE_UPRED					= 0;							//��������
static const SConfigSwitchValueType				S_CONFIGVALUE_DOWNRED				= 1;							//���Ǻ��

static const SConfigSwitchNoType				S_CONFIGNO_CANCASHMODE				= "Foreign&Dipper.CanCash";		//�����(0Ĭ�� 1���������� 2���� 3����)
static const SConfigSwitchValueType				S_CONFIGVALUE_DEFAULT				= 0;							//Ĭ��
static const SConfigSwitchValueType				S_CONFIGVALUE_YONGAN_ZHONGQI		= 1;							//���� ����
static const SConfigSwitchValueType				S_CONFIGVALUE_MAIKE					= 2;							//����
static const SConfigSwitchValueType				S_CONFIGVALUE_ZHONGXIN				= 3;							//����

static const SConfigSwitchNoType				S_CONFIGNO_RISKMODE					= "Global.RiskMode";			//�����ʣ�0 ��֤��/Ȩ��  1Ȩ��/��֤��
static const SConfigSwitchValueType				S_CONFIGVALUE_DEPOSIT_BALANCE		= 0;							//��֤��/Ȩ��
static const SConfigSwitchValueType				S_CONFIGVALUE_BALANCE_DEPOSIT		= 1;							//Ȩ��/��֤��
static const SConfigSwitchValueType				S_CONFIGVALUE_DEPOSITNOF_BALANCE	= 2;							//��֤��/Ȩ�� �޶���
static const SConfigSwitchValueType				S_CONFIGVALUE_BALANCE_DEPOSITNOF	= 3;							//Ȩ��/��֤�� �޶���

static const SConfigSwitchNoType				S_CONFIGNO_HISORDER					= "TradeDisplay.HisOrder";		//��ʷί��
static const SConfigSwitchNoType				S_CONFIGNO_HISMATCH					= "TradeDisplay.HisMatch";		//��ʷ�ɽ�
static const SConfigSwitchValueType				S_CONFIGVALUE_DAYS_0				= 0;							//��ѯ����
static const SConfigSwitchValueType				S_CONFIGVALUE_DAYS_1				= 1;
static const SConfigSwitchValueType				S_CONFIGVALUE_DAYS_3				= 3;
static const SConfigSwitchValueType				S_CONFIGVALUE_DAYS_5				= 5;
static const SConfigSwitchValueType				S_CONFIGVALUE_DAYS_7				= 7;
static const SConfigSwitchValueType				S_CONFIGVALUE_DAYS_15				= 15;
static const SConfigSwitchValueType				S_CONFIGVALUE_DAYS_30				= 30;

static const SConfigSwitchNoType				S_CONFIGNO_VALIDTYPE				= "MouseOrder.ValidType";		//��Ч���� ��λ��
static const SConfigSwitchValueType				S_CONFIGVALUE_GFD					= 1;							//������Ч
static const SConfigSwitchValueType				S_CONFIGVALUE_GTC					= 2;							//������Ч
static const SConfigSwitchValueType				S_CONFIGVALUE_GTD					= 4;							//������Ч
static const SConfigSwitchValueType				S_CONFIGVALUE_FOK					= 8;							//��ʱȫ��						
static const SConfigSwitchValueType				S_CONFIGVALUE_IOC					= 16;							//��ʱ����

static const SConfigSwitchNoType				S_CONFIGNO_SWPSW					= "SWRiskMonitor.Value";		//��������
static const SConfigSwitchValueType				S_CONFIGVALUE_PSW					= 2017;							//�ɱ༭  Es****

static const SConfigSwitchNoType				S_CONFIGNO_EQUITYTHRESHOLD			= "PositionDif.EquityThreshold";//�ֲ��� Ȩ����ֵ ��0-3200�� ��λ��

static const SConfigSwitchNoType				S_CONFIGNO_LADDERORDER				= "MouseOrder.LadderOrder";		//���
static const SConfigSwitchNoType				S_CONFIGNO_ICEBERG					= "MouseOrder.Iceberg";			//��ɽ��
static const SConfigSwitchNoType				S_CONFIGNO_GHOST					= "MouseOrder.Ghost";			//Ӱ�ӵ�
static const SConfigSwitchNoType				S_CONFIGNO_EXE_ENQUIRY				= "MouseOrder.Execute&Enquiry";	//��Ȩ ѯ��
static const SConfigSwitchNoType				S_CONFIGNO_OFFER					= "MouseOrder.Offer";			//Ӧ��
static const SConfigSwitchNoType				S_CONFIGNO_REMARK					= "MouseOrder.Remark";			//��ע��Ϣ
static const SConfigSwitchNoType				S_CONFIGNO_STOP_PROFIT				= "StrategyTrade.Stop&Profit";	//ֹ��ֹӯ
static const SConfigSwitchNoType				S_CONFIGNO_LOCALSPREAD				= "StrategyTrade.ClientSpread";	//��������
static const SConfigSwitchNoType				S_CONFIGNO_DRAWLINE					= "StrategyTrade.DrawLine";		//����
static const SConfigSwitchNoType				S_CONFIGNO_BATCHORDER				= "CommonModule.BatchOrder";	//����
static const SConfigSwitchNoType				S_CONFIGNO_COMBPOSITION				= "TradeDisplay.CombPosition";	//�ϲ��ֲ�
static const SConfigSwitchNoType				S_CONFIGNO_ENQUIRYLIST				= "TradeDisplay.EnquiryList";	//ѯ���б�
static const SConfigSwitchNoType				S_CONFIGNO_ENTRUSTTIP				= "TradeDisplay.EntrustTip";	//ί����ʾ��
static const SConfigSwitchNoType				S_CONFIGNO_AUTHMONEY				= "TradeDisplay.AuthMoney";		//�����ʽ�
static const SConfigSwitchNoType				S_CONFIGNO_DELETED					= "TradeDisplay.DeletedOrder";	//ɾ������
static const SConfigSwitchNoType				S_CONFIGNO_MULTIACC					= "LoginFrm.MutilAcc";			//���˻� �����ر�
static const SConfigSwitchNoType				S_CONFIGNO_POPWINNOTICE				= "CommonModule.PopWinNotice";	//����֪ͨ
static const SConfigSwitchNoType				S_CONFIGNO_BILLS					= "TradeDisplay.Bills";			//�˵���ѯ

static const SConfigSwitchValueType				S_CONFIGVALUE_TURNON				= 0;							//����
static const SConfigSwitchValueType				S_CONFIGVALUE_TURNOFF				= 1;							//�ر�

#pragma pack(push, 1)

//ҵ�����ݽṹ=======================================================================================

//ֱ�����׵�ַ
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

//�����ַ
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

//������Ϣ
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

//����
typedef struct SCurrency
{
    SCurrencyNoType								CurrencyNo;
    SExchangeRateType							ExchangeRate;
    SInterestRateType							InterestRate;
} SCurrency;

typedef struct SMortgageData
{
	SCurrencyNoType								MortgageNo;							//��ѺƷ���
	SCurrencyNoType								CurrencyNo;							//�۵ֻ��ұ��
	SExchangeRateType							ExchangeRate;						//��ѺƷ����
	SProportionType								Proportion;							//��Ѻ����
}SMortgageData;

//������
typedef struct SExchange
{
    SExchangeNoType								ExchangeNo;
    SExchangeNameType							ExchangeName;
} SExchange;

//Ʒ��
typedef struct SCommodity
{
    SCurrency*									Currency;							//����ָ��
    SExchange*									Exchange;							//������ָ��
	union
	{
		struct SCommodity*						TargetCommodity[4];					//����Ʒ��1
		struct SContract*						TargetContract[4];					//������Լ1
	};
	SCommodityNoType							CommodityNo;						//Ʒ�ִ���
	SCommodityTypeType							CommodityType;						//Ʒ������
    SCommodityNameType							CommodityName;						//Ʒ������
    SCommodityNumeType							PriceNume;							//����
    SCommodityDenoType							PriceDeno;							//��ĸ
    SCommodityTickType							PriceTick;							//��С�䶯��
    SCommodityPrecType							PricePrec;							//�۸񾫶�
    SPriceMultipleType							PriceMultiple;						//ִ�м۸���
    SCommodityDotType							TradeDot;							//ÿ�ֳ���
    SCommodityDotType							ExerciseDot;						//ִ�м۱���
    SCoverModeType								CoverMode;							//ƽ�ַ�ʽ
    SSpreadDirectType							SpreadDirect;						//��Ϸ��� ��һ��Ϊ׼
    SOptionExerciseType							OptionExercise;						//��Ȩ��Լ��ʽ
    SOptionPropertyType							OptionProperty;						//��Ȩ����
    SQtyType									MaxSingleOrderQty;					//�������ɽ���
    SQtyType									MaxPositionQty;						//���ֲ���
	SQuoteUnitNameType							QuoteUnitName;						//���۵�λ����
	STradingUnitNameType						TradingUnitName;					//���׵�λ����
	SDepositGroupNoType							DepositGroupNo;						//��߱�֤����
	SSpreadRateModeType							Operator[3];						//�����1	
	SPriceType									PriceProp[4];						//�۸����1
	SQtyType									QtyProp[4];							//�������1	
} SCommodity;

//��Լ
typedef struct SContract
{
    SCommodity*									Commodity;
    SContractNoType								ContractNo;
	struct SQuoteSnapShot*						SnapShot;							//����Ϊ�գ������յ�����Ŵ���
	struct SQuoteSnapShotL2*					BidL2;								//��ҵ����
	struct SQuoteSnapShotL2*					AskL2;								//���ҵ����
} SContract;

//��Ȩϵ��
typedef struct SOptionSeries
{
    SCommodity*									Commodity;
    SContract*									TargetContract;
    SOptionSeriesNoType							SeriesNo;
    SDateType									ExpiryDate;							//��������
    SOptionDaysType								ExpiryDays;							//��������
} SOptionSeries;

//��Ȩ��Լ��
typedef struct SOptionContractPair
{
    SStrikePriceType							StrikePrice;
    SContract*									Contract1;
    SContract*									Contract2;
} SOptionContractPair;

//������
typedef struct SPlate
{
    SPlateNoType								PlateNo;
    SPlateNameType								PlateName;
	SPlateNoType								ParentPlateNo;
} SPlate;

//���Ʒ�ֺ�Լ
typedef struct SPlateContent
{
	SPlateNoType								PlateNo;
    SPlateContAttrType							ContAttr;
	SPlateCodeType								PlateCode;
} SPlateContent;

//�������
typedef struct SQuoteField
{
    union
    {
        SFidMeanType							FidMean;							//�仯����ʹ�ñ�ʶ
        SFidAttrType							FidAttr;							//�̶�����ʹ������
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

//����仯�ṹ
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

//�������L2
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
	SContractNoType								ContractNo[4];					//��Լ
	SSpreadRateModeType							RateMode[3];					//�������㷽ʽ1   
	SPriceType									SpreadRate[4];					//�����۸����1
	SQtyType									SpreadQty[4];					//������������1
	SSpreadPriorityLeg							PriorityLeg;					//������ ������
	SQtyType									ValidLegs;						//��Ч����
	SSpreadExRateModeType						RateContractMode[4];			//���ʹ�ϵ  ֻ֧��* /
	SContractNoType								RateContractNo[4];				//����Լ
}SAddSPreadInfo;

//������Լ�ؼ���Ϊ Contract1 + RateMode1 + Contract2 + RateMode2 + Contract3 + RateMode3 + Contract4
typedef struct SSpreadContract
{
	SContractNoType								ContractNo;
	SContract*									Contract[4];
	SSpreadRateModeType							RateMode[3];					//�������㷽ʽ1  
	SPriceType									SpreadRate[4];					//�����۸����1
	SQtyType									SpreadQty[4];					//������������1
	SSpreadPriorityLeg							PriorityLeg;					//������ ������
	SQtyType									ValidLegs;						//��Ч����
	SCommodityTickType							PriceTick;						//��С�䶯��
	SCommodityPrecType							PricePrec;						//�۸񾫶�
	SCoverModeType								CoverMode;
	SQuoteSnapShot*								SnapShot;						//�����������
	SSpreadExRateModeType						RateContractMode[4];			//���ʹ�ϵ  ֻ֧��* /
	SContract*									RateContract[4];				//���ʺ�Լ
} SSpreadContract;

//��ʷ����
typedef struct SHisQuoteTimeBucket
{
    STimeBucketIndexType						Index;
    STimeType									BeginTime;
    STimeType									EndTime;
    STimeBucketTradeStateType					TradeState;
    STimeBucketDateFlagType						DateFlag;
    STimeBucketCalCountType						CalCount;						//����ģ���Ӧ����ģ��ķ�����
} SHisQuoteTimeBucket;

typedef struct SHisQuoteData //sizeof 80�ֽ�	
{
    SKLineIndexType								KLineIndex;						//K������|tickÿ��������ţ�min���׷�����ţ�day��Ч
    SDateType									TradeDate;						//������|tick��Ч��min���ܺ�ʱ�����ͬ��day��ʱ�����ͬ
    SDateTimeType								DateTimeStamp;					//ʱ���|��ͬ�������ͣ����Ȳ�ͬ
    
    SQtyType									QTotalQty;						//�������|�ܳɽ���
    SQtyType									QPositionQty;					//�������|�ֲ���
    SPriceType									QLastPrice;						//���¼ۣ����̼ۣ�
    
    union
    {
        struct
        {
            SQtyType							QKLineQty;						//K�߳ɽ���     day  min
            SPriceType							QOpeningPrice;					//���̼�		day  min
            SPriceType							QHighPrice;						//��߼�		day  min
            SPriceType							QLowPrice;						//��ͼ�		day  min
            SPriceType							QSettlePrice;					//�����		day  min
            
        };
        struct
        {
            SLastQtyType						QLastQty;						//��ϸ����      tick
            SPositionChgType					QPositionChg;					//�ֲ����仯	tick
            SPriceType							QBuyPrice;						//���			tick
            SPriceType							QSellPrice;						//����			tick
            SQtyType							QBuyQty;						//����			tick
            SQtyType							QSellQty;						//����			tick
        };
    };
} SHisQuoteData;

typedef struct SHisVolatilityData //sizeof  �ֽ�	
{
	SKLineIndexType								KLineIndex;						//K������|tickÿ��������ţ�min���׷�����ţ�day��Ч
	SDateType									TradeDate;						//������|tick��Ч��min���ܺ�ʱ�����ͬ��day��ʱ�����ͬ
	SDateTimeType								DateTimeStamp;					//ʱ���|��ͬ�������ͣ����Ȳ�ͬ

	SPriceType									TheoryPrice;					//���ۼ�
	SVolatilityType								Sigma;							//����������
	SGreekType									Delta;							//��Ȩ�۸�䶯�������ʲ��۸�䶯�ı���
	SGreekType									Gamma;							//delta�ı仯�������ʲ��۸�仯�ı���
	SGreekType									Vega;							//������ϼ�ֵ�仯�������ʲ������ʱ仯�ı���
	SGreekType									Theta;							//������ϼ�ֵ�仯��ʱ��仯�ı���
	SGreekType									Rho;							//������ϼ�ֵ�������ʱ仯��������

} SHisVolatilityData;

//��Ȩ����
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

//�ƶ˽��׹�˾ key=CompanyNo+AddrTypeNo
typedef struct SCloudTradeCompany
{
    SCompanyNoType								CompanyNo;
    SCompanyNameType							CompanyName;
    SAddrTypeNoType								AddrTypeNo;
    SAddrTypeNameType							AddrTypeName;
    STradeApiType								TradeApi;
    SSimulateTradeType							SimulateTrade;
} SCloudTradeCompany;

//�ƶ˽���վ�� key=SiteNo
typedef struct SCloudTradeSite
{
	SCloudSiteNoType							SiteNo;
	SCloudSiteNameType							SiteName;
	SIpType										Ip;
	SPortType									Port;
} SCloudTradeSite;

//���������ƿ���
typedef struct SConfigSwitchInfo
{
	SConfigSwitchNoType							ConfigSwitchNo;
	SConfigSwitchValueType						ConfigSwitchValue;
} SConfigSwitchInfo;

//���������
typedef struct SWhiteListInfo
{
	SWhiteListNameType							WhiteListName;
	SVerfyIndexType								VerfyIndex;
} SWhiteListInfo;

//����ӳ��
typedef struct SCodeMappingInfo
{
	SCommodityNoType							SrcCode;
	SCommodityNoType							ShowCode;
	SPriceType									PriceMultiple;
} SCodeMappingInfo;

//��Ȩִ�м۸�ʽ������
typedef struct SOptionPrecisionInfo
{
	SCommodityNoType							CommodityNo;
	SPriceType									PriceMultiple;
	SWidthType									IntegerWidth;
	SWidthType									TotalWidth;
} SOptionPrecisionInfo;

#pragma pack(pop)

#endif /* StarApi_Type_h */
