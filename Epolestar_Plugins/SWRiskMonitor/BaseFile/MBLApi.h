#define MBL_IP "120.136.161.148"
#define MBL_SHFE_Port 30256
#define MBL_INE_Port 30275

#define MAX_RECV_BUFFER_LEN 102400
#define MAX_SEND_INSTRUMENT_LEN 240   //���ĺ�Լ�б��ַ�����󳤶�

#define MBL_REQ_Login 'a'
#define MBL_REQ_SubMarket 'b'
#define MBL_REQ_Heart 'c'
#define MBL_RSP_Login '1'
#define MBL_RSP_SubMarket '2'
#define MBL_RSP_Heart '3'

//�û���¼����
struct MBLUserData
{
    char username[32];
    char password[32];
};
//ȫ���������ݣ�40�ֽ�
struct MBLData
{
	char timestamp[16];	//ʱ���
	char instrumentID[7];//��Լ��
	char direction;//����
	double price;
	int volume;	
	int dataSource;
};
//��������ö��
enum MBLErrorInfo
{
    MBL_NO_ERROR = 0,         //����
    MBL_LOGIN_ERROR,		  //��¼����
    MBL_SUB_ERROR,			  //���Ĵ���
    MBL_OTHER_ERROR			  //��������
};

//�ص��ӿ���
class MBLSpi
{
public:
	//���ӷ������ɹ���ص�
	virtual void OnMBLConnected(){};
	//��¼�ɹ���ص�
	virtual void OnMBLRspUserLogin(MBLErrorInfo ErrorInfo){};
	//���ĺ�Լ�ɹ���ص�
	virtual void OnMBLRspSubcribe(MBLErrorInfo ErrorInfo){};
	//ȫ���������ݻص�����
	virtual void OnMBLRspMarketData(MBLData *pMBLMarketData){};
};

//Api�ӿ���
class __declspec(dllexport) MBLApi
{
public:
	//����MBLʵ��
	static MBLApi *CreatMBLApi();
	//ע��SPI
	virtual void MBLRegisterSpi(MBLSpi *pSpi);
	//���÷�����ip�˿�
	//ʾ����pMBLApi->MBLSetIPInfo("101.95.28.58", 28600);
	virtual void MBLSetIPInfo(char*, int);
	//��ʼ��
	virtual void MBLInit();
	//�ͷ�ʵ��
	virtual void MBLRelease();
	//��¼�����
	virtual void MBLReqUserLogin(MBLUserData *pUserData);
	//��������Ҫ�ĺ�Լ�����Ķ����Լ�Զ��ŷָ�
	virtual void MBLReqSubcribe(char*);
	//���ĳɹ����֪ϵͳ����Լ�ѿ�ʼ����
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

