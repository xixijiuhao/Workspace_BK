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
	REASON_CLOSE_INIT = 1,		//主动断开
	REASON_CLOSE_PASS,			//被动断开
	REASON_READ_ERROR,			//读错误
	REASON_WRITE_ERROR,			//写错误
	REASON_BUF_FULL,			//缓冲区满
	REASON_IOCP_ERROR,			//异步操作错误
	REASON_PARSE_ERROR,			//解析数据错误
	REASON_CONNECT_TIMEOUT,		//连接超时
	REASON_CLOSE_INIT_ONREOPEN,	//重连时内部主动断开，此时外部不用发起重连操作

	REASON_CONNECT_FAILED	= 13,	//网络连接失败，请检查网络状态是否正常
	REASON_CHANGEPSW_FAILED = 14	//暂不支持修改资金密码
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