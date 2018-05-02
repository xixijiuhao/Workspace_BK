#ifndef _HISQUOTESHELL_API_
#define _HISQUOTESHELL_API_

const wchar_t PLUG_HISQUOTESHELL_NAME[] = L"HisQuoteShell";
const wchar_t PLUG_HISQUOTESHELL_VERSION[] = L"9.3.3.2";


#pragma pack(push, 1)

typedef int								ShellKLineIndexType;	
typedef unsigned long long				ShellDateTimeType;					//��������ʱ�����ͣ�������
typedef unsigned int					ShellDateType;						//������������
typedef unsigned long long				ShellQtyFieldType;	
typedef double							ShellPriceFieldType;	
typedef unsigned int					ShellLastQtyType;								//��ϸ���ֱ仯
typedef int								ShellPositionChgType;

const char KLINE_TYPE_TICK				= 'T';						//�ֱ���ϸ
const char KLINE_TYPE_MINUTE			= 'M';						//������
const char KLINE_TYPE_DAY				= 'D';						//����

struct HisQuoteShellData
{
    ShellKLineIndexType					KLineIndex;                 //K������    tickÿ��������ţ�min���׷�����ţ�day��Ч
    ShellDateType						TradeDate;                  //������     tick��Ч��min���ܺ�ʱ�����ͬ��day��ʱ�����ͬ
    ShellDateTimeType					DateTimeStamp;				//ʱ�������ͬ�������ͣ����Ȳ�ͬ
    ShellQtyFieldType					QTotalQty;                  //������� �ܳɽ���        
    ShellQtyFieldType					QPositionQty;				//������� �ֲ���  
    ShellPriceFieldType					QLastPrice;					//���¼ۣ����̼ۣ�   
    
    union
    {
        struct
        {
            ShellQtyFieldType			QKLineQty;              //K�߳ɽ���     day  min     
            ShellPriceFieldType			QOpeningPrice;          //���̼�		day  min    
            ShellPriceFieldType			QHighPrice;				//��߼�		day  min       
            ShellPriceFieldType			QLowPrice;				//��ͼ�		day  min    
            ShellPriceFieldType			QSettlePrice;		    //�����		day  min    
            
        };
        struct
        {
            ShellLastQtyType			QLastQty;               //��ϸ����      tick    
            ShellPositionChgType		QPositionChg;           //�ֲ����仯	tick
            ShellPriceFieldType			QBuyPrice;              //���			tick        
            ShellPriceFieldType			QSellPrice;             //����			tick          
            ShellQtyFieldType			QBuyQty;                //����			tick       
            ShellQtyFieldType			QSellQty;               //����			tick
        };
    };    
};

#pragma pack(pop)

class IHisQuoteShellSpi
{
public:
	virtual void __cdecl OnHisQuote(const char* contractid, const char klinetype, const unsigned int sessionid) = 0;
};

class IHisQuoteShellApi
{
public:
	virtual void RegSpi(IHisQuoteShellSpi* spi) = 0;

	//������ʷ���ݣ�0��ʾ��Լ��Ч��1��ʾ�ɹ� �ȴ�sessionid������2��ʾ�����Ѵ���
	virtual int __cdecl SubHisQuote(const char* contractid, const char klinetype, unsigned int& sessionid) = 0;	
	virtual int __cdecl UnsubHisQuote(const char* contractid) = 0;	
	
	//��ȡ������ʷ���ݣ�end������β����Ϊ0����������data�����������ݱ��棬�˴����ܺ�������
	virtual int __cdecl GetHisQuote(const char* contractid, const char klinetype, HisQuoteShellData rdata[], int rsize) = 0;

};















#endif