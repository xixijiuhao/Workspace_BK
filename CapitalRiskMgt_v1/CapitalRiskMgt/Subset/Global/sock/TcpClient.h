#ifndef _TCP_CLIENT_
#define _TCP_CLIENT_

#include "WinTools.h"
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <mstcpip.h>
#include "Comm_API.h"

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
	SOCKET m_Socket;

	void TriggerClose(DisReason reason);
private:
	TNotify* m_Notify;
	bool m_IsOpen;				//是否已经登录
	

	char m_RecvBuf[512 * 1024];	//缓冲区
	int m_ReadIndex;			//标记数据起始位置
	int m_WriteIndex;			//标记最后一个数据的下一个位置
};


#endif