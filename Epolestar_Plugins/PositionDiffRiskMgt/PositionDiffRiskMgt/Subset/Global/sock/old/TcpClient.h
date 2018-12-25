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
	REASON_CLOSE_INIT = 1,		//�����Ͽ�
	REASON_CLOSE_PASS,			//�����Ͽ�
	REASON_READ_ERROR,			//������
	REASON_WRITE_ERROR,			//д����
	REASON_BUF_FULL,			//��������
	REASON_IOCP_ERROR,			//�첽��������
	REASON_PARSE_ERROR,			//�������ݴ���
	REASON_CONNECT_TIMEOUT		//���ӳ�ʱ
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
	//����0Ϊ�ɹ�
	int Open(const char* ip, const unsigned short port);
	void Close();
	//����0Ϊ�ɹ�
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