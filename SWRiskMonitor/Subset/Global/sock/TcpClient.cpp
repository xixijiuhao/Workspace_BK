#include "TcpClient.h"

TTcpClient::TTcpClient() : m_Notify(NULL), m_IsOpen(false), m_Socket(INVALID_SOCKET), m_ReadIndex(0), m_WriteIndex(0)
{
	WSADATA wsa;
	if (!(WSAStartup(0x0202, &wsa)))
	{
		return;
	}
}

TTcpClient::~TTcpClient()
{
	Terminate();
	Sleep(100);
	WSACleanup();
}

int TTcpClient::Open(const char* ip, const unsigned short port)
{
	//�Ƿ��Ѿ�����
	if (IsOpen())
		TriggerClose(REASON_CLOSE_INIT_ONREOPEN);
	//��鵱ǰsocket����Ч���˳������ȵ���close
	if (INVALID_SOCKET != m_Socket)
		return 1;

	//����socket, ��Ч�򷵻�
	SOCKET s = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == s)
		return 2;

	//����socket������
	unsigned long ub(1);
	::ioctlsocket(s, FIONBIO, &ub);

	//����nagle
	BOOL noDelay = TRUE;
	::setsockopt(s, IPPROTO_TCP, TCP_NODELAY, (const char*)&noDelay, sizeof(BOOL));

	//keeplive 
	BOOL keepalive = TRUE;
	::setsockopt(s, SOL_SOCKET, SO_KEEPALIVE, (const char*)&keepalive, sizeof(BOOL));

	tcp_keepalive inKeepAlive = { 0 };
	unsigned long ulInLen = sizeof(tcp_keepalive);
	tcp_keepalive outKeepAlive = { 0 };
	unsigned long ulOutLen = sizeof(tcp_keepalive);
	unsigned long ulBytesReturn = 0;

	inKeepAlive.onoff = 1;
	inKeepAlive.keepaliveinterval = 5000; //��λΪ����
	inKeepAlive.keepalivetime =		5000; //��λΪ����
	::WSAIoctl(s, SIO_KEEPALIVE_VALS, (LPVOID)&inKeepAlive, ulInLen, (LPVOID)&outKeepAlive, ulOutLen, &ulBytesReturn, NULL, NULL);

	//ϵͳ������ ���ͻ���Ϊ32KB�����ջ���128KB
	int buflen(32 * 1024);
	::setsockopt(s, SOL_SOCKET, SO_SNDBUF, (const char*)&buflen, sizeof(buflen));
	
	buflen = 128 * 1024;
	::setsockopt(s, SOL_SOCKET, SO_RCVBUF, (const char*)&buflen, sizeof(buflen));

	//��¼���ӵ�ַ�˿�
	sockaddr_in addr;
	memset(&addr, 0, sizeof(sockaddr_in));
	addr.sin_family = AF_INET;
	//addr.sin_addr.s_addr = inet_addr(ip);
	inet_pton(AF_INET, ip, &addr.sin_addr.s_addr);
	addr.sin_port = htons(port);

	//����
	fd_set writeset;
	FD_ZERO(&writeset);
	FD_SET(s, &writeset);
	timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 1000 * 500;

	::connect(s, (sockaddr*)&addr, sizeof(sockaddr));
	int sret = ::select(0, 0, &writeset, 0, &tv);
	if (sret <= 0)
	{
		::closesocket(s);
		m_IsOpen = false;
		if (m_Notify)
			m_Notify->OnDisconnect(this, REASON_CONNECT_TIMEOUT);
		return 3;
	}

	//����socket����
	ub = 0;
	::ioctlsocket(s, FIONBIO, &ub);

	m_IsOpen = true;
	m_Socket = s;	//OnConnect�л����send���˾������OnConnect֮ǰ

	if (m_Notify)
		m_Notify->OnConnect(this);
	
	//�����̣߳��ڶ�������Ϊ��Ч����
	Start();

	return 0;
}

void TTcpClient::Close()
{
	TriggerClose(REASON_CLOSE_INIT);
}

//����0Ϊ�ɹ�
int TTcpClient::Send(const char* buf, const int len)
{
	int ret = ::send(m_Socket, buf, len, 0);
	if (len != ret)
	{
		TriggerClose(REASON_CONNECT_FAILED);
		return 1;
	}

	return 0;
}

void TTcpClient::TriggerClose(DisReason reason)
{
	if (INVALID_SOCKET == m_Socket)
		return;

	SOCKET s(m_Socket);
	m_Socket = INVALID_SOCKET;
	m_IsOpen = false;
	m_ReadIndex = 0;
	m_WriteIndex = 0;

	////����socket����
	//unsigned long ub(0);
	//::ioctlsocket(s, FIONBIO, &ub);
	::closesocket(s);
	if (m_Notify)
		m_Notify->OnDisconnect(this, reason);
}

void TTcpClient::Run()
{
	timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 1000 * 300;

	while (!isTerminated())
	{
		if (INVALID_SOCKET == m_Socket)
		{
			Sleep(100);
			continue;
		}

		fd_set readset;
		FD_ZERO(&readset);
		FD_SET(m_Socket, &readset);

		int sret = ::select(0, &readset, 0, 0, &tv);

		if (sret > 0)
		{	
			//�жϻ������Ƿ�����
			if (m_WriteIndex >= sizeof(m_RecvBuf))
			{
				TriggerClose(REASON_CONNECT_FAILED);
				continue;
			}

			int ret = ::recv(m_Socket, &m_RecvBuf[m_WriteIndex], sizeof(m_RecvBuf) - m_WriteIndex, 0);
			
			if (ret <= 0)
			{
				TriggerClose(REASON_CONNECT_FAILED);
				continue;
			}

			m_WriteIndex += ret;
			if (m_Notify)
			{
				ret = m_Notify->OnRecvData(this, &m_RecvBuf[m_ReadIndex], m_WriteIndex - m_ReadIndex);
				if (ret < 0)
				{
					TriggerClose(REASON_CONNECT_FAILED);
					continue;
				}
				m_ReadIndex += ret;
				if (m_ReadIndex >= m_WriteIndex)
				{
					m_ReadIndex = 0;
					m_WriteIndex = 0;
				}
				else if (m_WriteIndex >= sizeof(m_RecvBuf) / 2)
				{
					memmove_s(m_RecvBuf, sizeof(m_RecvBuf), &m_RecvBuf[m_ReadIndex], m_WriteIndex - m_ReadIndex);
					m_WriteIndex -= m_ReadIndex;
					m_ReadIndex = 0;
				}	
			}

		}
		else if (SOCKET_ERROR == sret)
		{
			TriggerClose(REASON_CONNECT_FAILED);
		}
		
	}
}