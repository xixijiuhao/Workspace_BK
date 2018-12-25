#include "TcpClient.h"
#include  <mstcpip.h>

#pragma comment(lib, "Ws2_32.lib")
#pragma comment (lib, "mswsock.lib")


TTcpClient::TTcpClient() : m_Notify(NULL), m_IsOpen(false), m_Socket(INVALID_SOCKET), m_RecvLen(0), m_SendLen(0),
	m_EventConnect(false), m_EventDisconnect(false), m_EventSend(false)
{
	WSADATA wsa;
	WSAStartup(0x0202, &wsa);

	m_Event[0] = WSACreateEvent();	//��һ���¼�Ϊ�����¼�
	m_Event[1] = WSACreateEvent();  //socket�����¼�
}

TTcpClient::~TTcpClient()
{
	Terminate();
	WSASetEvent(m_Event[0]);
	Sleep(50);
	WSACleanup();
}

int TTcpClient::Open(const char* ip, const unsigned short port)
{
	//��鵱ǰsocket����Ч���˳������ȵ���close
	if (INVALID_SOCKET != m_Socket)
		return 1;

	//����socket, ��Ч�򷵻�
	m_Socket = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
	if (INVALID_SOCKET == m_Socket)
		return 2;

	//����socket������
	unsigned long ub(1);
	ioctlsocket(m_Socket, FIONBIO, &ub);

	//����nagle
	BOOL noDelay = TRUE;
	setsockopt(m_Socket, IPPROTO_TCP, TCP_NODELAY, (const char*)&noDelay, sizeof(BOOL));

	//keeplive 
	BOOL keepalive = TRUE;
	setsockopt(m_Socket, SOL_SOCKET, SO_KEEPALIVE, (const char*)&keepalive, sizeof(BOOL));

	tcp_keepalive inKeepAlive = { 0 };
	unsigned long ulInLen = sizeof(tcp_keepalive);
	tcp_keepalive outKeepAlive = { 0 };
	unsigned long ulOutLen = sizeof(tcp_keepalive);
	unsigned long ulBytesReturn = 0;

	inKeepAlive.onoff = 1;
	inKeepAlive.keepaliveinterval = 5000; //��λΪ����
	inKeepAlive.keepalivetime =		5000; //��λΪ����
	WSAIoctl(m_Socket, SIO_KEEPALIVE_VALS, (LPVOID)&inKeepAlive, ulInLen, (LPVOID)&outKeepAlive, ulOutLen, &ulBytesReturn, NULL, NULL);

	//ϵͳ������ ���ͻ���Ϊ0�����ջ���64KB
	int buflen(32 * 1024);
	setsockopt(m_Socket, SOL_SOCKET, SO_SNDBUF, (const char*)&buflen, sizeof(buflen));
	
	buflen = 64 * 1024;
	setsockopt(m_Socket, SOL_SOCKET, SO_RCVBUF, (const char*)&buflen, sizeof(buflen));

	//�¼�����socket
	if (SOCKET_ERROR == WSAEventSelect(m_Socket, m_Event[1], FD_CONNECT | FD_CLOSE | FD_READ))
	{
		closesocket(m_Socket);
		m_Socket = INVALID_SOCKET;
		return 3;
	}		

	//��¼���ӵ�ַ�˿�
	memset(&m_Addr, 0, sizeof(sockaddr_in));
	m_Addr.sin_family = AF_INET;
	//m_Addr.sin_addr.s_addr = inet_addr(ip);
	inet_pton(AF_INET, ip, &m_Addr.sin_addr.s_addr);
	m_Addr.sin_port = htons(port);

	//�����̣߳��ڶ�������Ϊ��Ч����
	Start();

	//���������¼�
	m_EventConnect = true;
	WSASetEvent(m_Event[0]);

	return 0;
}

void TTcpClient::Close()
{
	m_EventDisconnect = true;
	WSASetEvent(m_Event[0]);
}

int TTcpClient::Send(const char* buf, const int len)
{
	if (len > (int)sizeof(m_SendBuf) - m_SendLen)
		return 1;

	TCriticalSection::Lock lock(m_Crit);
	memcpy_s(&m_SendBuf[m_SendLen], sizeof(m_SendBuf)-m_SendLen, buf, len);
	m_SendLen += len;

	m_EventSend = true;
	WSASetEvent(m_Event[0]);

	return 0;
}

void TTcpClient::TriggleClose(DisReason reason)
{
	if (INVALID_SOCKET == m_Socket)
		return;

	closesocket(m_Socket);
	m_Socket = INVALID_SOCKET;
	m_IsOpen = false;
	if (m_Notify)
		m_Notify->OnDisconnect(this, reason);
}

void TTcpClient::Run()
{
	while (!isTerminated())
	{
		DWORD ret = WSAWaitForMultipleEvents(2, m_Event, FALSE, WSA_INFINITE, FALSE);
		if (INVALID_SOCKET == m_Socket || ret == WSA_WAIT_FAILED)
		{
			Sleep(100);
			continue;
		}
			
		ret -= WSA_WAIT_EVENT_0;
		for (int i = ret; i < 2; i++)
		{
			if (i > (int)ret) //��һ���¼�������֤
			{
				int eret = WSAWaitForMultipleEvents(1, &m_Event[i], TRUE, 0, FALSE);
				if ((WSA_WAIT_FAILED == eret) || (WSA_WAIT_TIMEOUT == eret))
					continue;
			}

			switch (i)
			{
			case 0: //�����¼�
				{
					WSAResetEvent(m_Event[i]);

					if (m_EventSend)//�����
					{
						m_EventSend = false;

						TCriticalSection::Lock lock(m_Crit);
						int c = send(m_Socket, m_SendBuf, m_SendLen, 0);
						if (SOCKET_ERROR == c)
						{
							int err = WSAGetLastError();
							TriggleClose(REASON_WRITE_ERROR);
						}
						else
						{
							m_SendLen -= c;
							if (m_SendLen > 0)
							{
								memmove_s(m_SendBuf, sizeof(m_SendBuf), &m_SendBuf[c], m_SendLen);
								m_EventSend = true;
								WSASetEvent(m_Event[i]);
							}								
						}						
					}

					if (m_EventConnect)//��������
					{
						connect(m_Socket, (sockaddr*)&m_Addr, sizeof(sockaddr));
						m_EventConnect = false;
					}

					if (m_EventDisconnect)//����Ͽ�
					{
						TriggleClose(REASON_CLOSE_INIT);
						m_EventDisconnect = false;
					}					
				}
				break;
			case 1: //�����¼�
				{
					if (INVALID_SOCKET == m_Socket)
						break;

					WSANETWORKEVENTS NetworkEvents;
					if (0 != WSAEnumNetworkEvents(m_Socket, m_Event[i], &NetworkEvents))
						break;

					if (NetworkEvents.lNetworkEvents & FD_READ)
					{
						int c = recv(m_Socket, &m_RecvBuf[m_RecvLen], sizeof(m_RecvBuf)-m_RecvLen, 0);
						if (SOCKET_ERROR == c)
							TriggleClose(REASON_READ_ERROR);
						else
						{
							m_RecvLen += c;
							if (m_Notify)
							{
								c = m_Notify->OnRecvData(this, m_RecvBuf, m_RecvLen);
								if (c < 0)
									TriggleClose(REASON_PARSE_ERROR);
								else
								{
									m_RecvLen -= c;
									if (m_RecvLen > 0)
										memmove_s(m_RecvBuf, sizeof(m_RecvBuf), &m_RecvBuf[c], m_RecvLen);
								}
							}
							if (sizeof(m_RecvBuf) == m_RecvLen)
								TriggleClose(REASON_BUF_FULL);
						}
					}// end FD_READ

					if (NetworkEvents.lNetworkEvents & FD_CONNECT)
					{
						if (0 == NetworkEvents.iErrorCode[FD_CONNECT_BIT])
						{
							m_IsOpen = true;
							if (m_Notify)
								m_Notify->OnConnect(this);
						}
						else
						{
							TriggleClose(REASON_CONNECT_TIMEOUT);
						}
					}// end FD_CONNECT

					if (NetworkEvents.lNetworkEvents & FD_CLOSE)
					{
						TriggleClose(REASON_CLOSE_PASS);
					}// end FD_CLOSE

				}// end 1
				break;
			default:
				break;
			}
		}
	}
}