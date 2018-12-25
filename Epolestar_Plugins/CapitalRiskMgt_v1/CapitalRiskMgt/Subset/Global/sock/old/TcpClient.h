#ifndef _TCP_CLIENT_
#define _TCP_CLIENT_

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif 

#include "wintools.h"
#include <WinSock2.h>
#include <ws2tcpip.h>

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

class TTcpClient : public TThread
{
public:
	class TNotify
	{
	public:
		virtual void OnConnect(TTcpClient* self) = 0;
		virtual void OnDisconnect(TTcpClient* self, DisReason reason) = 0;
		virtual int OnRecvData(TTcpClient* self, const char* buf, const int len) = 0;
	};

public:
	TTcpClient();
	~TTcpClient();
	void SetNotify(TNotify* notify) { m_Notify = notify;  }
	//返回0为成功
	int Open(const char* ip, const unsigned short port);
	void Close();
	//返回0为成功
	int Send(const char* buf, const int len);
	bool IsOpen() { return m_IsOpen; }

protected:
	virtual void Run();

	void TriggleClose(DisReason reason);

	TNotify* m_Notify;
	bool m_IsOpen;
	SOCKET m_Socket;
	char m_RecvBuf[128 * 1024];
	int m_RecvLen;
	char m_SendBuf[4 * 1024];
	int m_SendLen;
	WSAEVENT m_Event[2];
	sockaddr_in m_Addr;
	TCriticalSection m_Crit;

	bool m_EventConnect;
	bool m_EventDisconnect;
	bool m_EventSend;
};


#endif