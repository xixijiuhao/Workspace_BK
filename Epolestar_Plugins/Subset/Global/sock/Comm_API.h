#pragma once

#define PLUG_COMM_NAME 		"Comm"
#define PLUG_COMM_VERSION	"1.0.0.1"

//Tcp
enum
{ 
	TCP_CLIENT_RECV_BUFSIZE = 128*1024
};


enum DisReason
{
	REASON_CLOSE_INIT = 1,		//�����Ͽ�
	REASON_CLOSE_PASS,			//�����Ͽ�
	REASON_READ_ERROR,			//������
	REASON_WRITE_ERROR,			//д����
	REASON_BUF_FULL,			//��������
	REASON_IOCP_ERROR,			//�첽��������
	REASON_PARSE_ERROR,			//�������ݴ���
	REASON_CONNECT_TIMEOUT,		//���ӳ�ʱ
	REASON_CLOSE_INIT_ONREOPEN,	//����ʱ�ڲ������Ͽ�����ʱ�ⲿ���÷�����������

	REASON_CONNECT_FAILED	= 13,	//��������ʧ�ܣ���������״̬�Ƿ�����
	REASON_CHANGEPSW_FAILED = 14	//�ݲ�֧���޸��ʽ�����
};


class ITcpClientNotify
{
public:
	virtual int OnClientRecv(const char* buf, const int len) = 0;
	virtual void OnConnect() = 0;
	virtual void OnDisconnect(DisReason reason) = 0;
};

class ITcpClient
{
public:
	virtual void SetClientNotify(ITcpClientNotify* notify) = 0;
	virtual bool Connect(const char* ip, const unsigned short port, const int timeout = 10 * 1000 * 1000) = 0;
	virtual bool Disconnect() = 0;
	virtual bool Send(const char* buf, const int len) = 0;
	virtual bool IsConnected() = 0;
	virtual void LocalIp(char* ip, int len) = 0;
	virtual unsigned short LocalPort() = 0;
	virtual void PeerIp(char* ip, int len) = 0;
	virtual unsigned short PeerPort() = 0;	
	virtual void LocalMac(char* mac, int len) = 0;
};