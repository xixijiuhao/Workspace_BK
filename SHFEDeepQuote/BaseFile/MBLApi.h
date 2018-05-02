#define MBL_IP "120.136.161.148"
#define MBL_SHFE_Port 30256
#define MBL_INE_Port 30275

#define MAX_RECV_BUFFER_LEN 102400
#define MAX_SEND_INSTRUMENT_LEN 240   //订阅合约列表字符串最大长度

#define MBL_REQ_Login 'a'
#define MBL_REQ_SubMarket 'b'
#define MBL_REQ_Heart 'c'
#define MBL_RSP_Login '1'
#define MBL_RSP_SubMarket '2'
#define MBL_RSP_Heart '3'

//用户登录数据
struct MBLUserData
{
    char username[32];
    char password[32];
};
//全档行情数据，40字节
struct MBLData
{
	char timestamp[16];	//时间戳
	char instrumentID[7];//合约名
	char direction;//方向
	double price;
	int volume;	
	int dataSource;
};
//错误类型枚举
enum MBLErrorInfo
{
    MBL_NO_ERROR = 0,         //正常
    MBL_LOGIN_ERROR,		  //登录错误
    MBL_SUB_ERROR,			  //订阅错误
    MBL_OTHER_ERROR			  //其他错误
};

//回调接口类
class MBLSpi
{
public:
	//连接服务器成功后回调
	virtual void OnMBLConnected(){};
	//登录成功后回调
	virtual void OnMBLRspUserLogin(MBLErrorInfo ErrorInfo){};
	//订阅合约成功后回调
	virtual void OnMBLRspSubcribe(MBLErrorInfo ErrorInfo){};
	//全档行情数据回调推送
	virtual void OnMBLRspMarketData(MBLData *pMBLMarketData){};
};

//Api接口类
class __declspec(dllexport) MBLApi
{
public:
	//创建MBL实例
	static MBLApi *CreatMBLApi();
	//注册SPI
	virtual void MBLRegisterSpi(MBLSpi *pSpi);
	//设置服务器ip端口
	//示例：pMBLApi->MBLSetIPInfo("101.95.28.58", 28600);
	virtual void MBLSetIPInfo(char*, int);
	//初始化
	virtual void MBLInit();
	//释放实例
	virtual void MBLRelease();
	//登录服务端
	virtual void MBLReqUserLogin(MBLUserData *pUserData);
	//订阅所需要的合约，订阅多个合约以逗号分隔
	virtual void MBLReqSubcribe(char*);
	//订阅成功后告知系统，合约已开始推送
	virtual void SetMBLDataPushingFlag(bool flag);


protected:
	MBLApi();
	~MBLApi(){};

private:
	//static MBLApi *m_pMBLApiInstance;
	static MBLSpi* m_pMBLSpi;
	bool m_MBLDataPushingFlag;
	int m_port;
	char m_ip[20];
	int tempbuflen;
	char tempbuffer[1000000];
	SOCKET sockClient;
	//pthread_t hThread;
	HANDLE  hThread;
	HANDLE  hThread_Heart;

	bool initSocket();
	void SocketLoop();
	void SendHeart();
	static DWORD WINAPI  SocketFunc(void *pParam);
	static DWORD WINAPI  SocketHeart(void *pParam);

};

