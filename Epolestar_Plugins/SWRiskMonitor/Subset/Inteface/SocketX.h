#ifndef _SOCKET_X_
#define _SOCKET_X_

//Udp_Client
class IUdpClient
{
public:
	virtual void SetDestAddr(const char* ip, const unsigned short port) = 0;
	virtual bool SendTo(const char* buf, const int len) = 0;

	virtual bool SendTo(const char* buf, const int len, const char* ip, const unsigned short port) = 0;
public:
	static IUdpClient* New();
};

//Udp_Server
class IUdpNotify
{
public:
	virtual void OnUdpRecv(const char* buf, const int len) = 0;
	virtual void OnError() = 0;
};

class IUdpServer
{
public:
	virtual void SetNotify(IUdpNotify* notify) = 0;
	virtual bool Listen(const unsigned short port) = 0;

	virtual void FromIP(char* ip, int len) = 0;
	virtual unsigned short FromPort() = 0;
public:
	static IUdpServer* New();
};

//Tcp
enum
{ 
	TCP_CLIENT_RECV_BUFSIZE = 8*1024
};

enum
{ 
	TCP_SERVER_RECV_BUFSIZE = 8*1024
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
	REASON_CONNECT_TIMEOUT		//连接超时
};

//Tcp_Client
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

public:
	virtual void LocalIp(char* ip, int len) = 0;
	virtual unsigned short LocalPort() = 0;
	virtual void PeerIp(char* ip, int len) = 0;
	virtual unsigned short PeerPort() = 0;
	virtual void LocalMac(char* mac, int len) = 0;

public:
	static ITcpClient* New();
};

//Tcp_Server
class ITcpServerNotify
{
public:
	virtual int OnServerRecv(const int id, const char* buf, const int len) = 0;
	virtual void OnConnect(const int id, const char* ip, const unsigned short port) = 0;
	virtual void OnDisconnect(const int id, DisReason reason) = 0;
};

class ITcpServer
{
public:
	virtual void SetServerNotify(ITcpServerNotify* notify) = 0;

	virtual bool Listen(const unsigned short port) = 0;
	virtual void StopListen() = 0;
	virtual bool IsListening() = 0;

	virtual void CloseClient(const int id) = 0;
	virtual bool Send(const int id, const char* buf, const int len) = 0;
public:
	static ITcpServer* New();
};

#endif