#include "TcpClientComm.h"
#include "NetInfo.h"

#pragma comment(lib, "ws2_32.lib")

CTcpClientComm::CTcpClientComm(){}

//TTcpClient::TNotify
void CTcpClientComm::OnConnect(TTcpClient* self)
{ 
	if (m_Notify) 
		m_Notify->OnConnect(); 
}
void CTcpClientComm::OnDisconnect(TTcpClient* self, DisReason reason)
{ 
	if (m_Notify) 
		m_Notify->OnDisconnect(reason); 
}
int CTcpClientComm::OnRecvData(TTcpClient* self, const char* buf, const int len)
{ 
	if (m_Notify) 
		return m_Notify->OnClientRecv(buf, len); 
	else
		return 0;
}

//ITcpClient
void CTcpClientComm::SetClientNotify(ITcpClientNotify* notify) 
{ 
	m_Notify = notify; 
	SetNotify(this); 
}
bool CTcpClientComm::Connect(const char* ip, const unsigned short port, const int timeout)
{		
	return Open(ip, port) == 0;
}
bool CTcpClientComm::Disconnect()
{
	Close();
	return true;
}
bool CTcpClientComm::Send(const char* buf, const int len)
{
	return TTcpClient::Send(buf, len) == 0;
}
bool CTcpClientComm::IsConnected()
{ 
	return IsOpen(); 
}

void CTcpClientComm::LocalIp(char* ip, int len)
{
	sockaddr_in addr;
	int addrlen(sizeof(sockaddr));
	::getsockname(m_Socket, (sockaddr*)&addr, &addrlen);
	inet_ntop(AF_INET, &addr.sin_addr, ip, len);				//二进制格式-->ASCII字符串点分十进制
	//strncpy_s(ip, len, inet_ntop(addr.sin_addr), len-1);
}
unsigned short CTcpClientComm::LocalPort()
{		
	sockaddr_in addr;
	int addrlen(sizeof(sockaddr));
	::getsockname(m_Socket, (sockaddr*)&addr, &addrlen);
	return ntohs(addr.sin_port);								//将一个16位数由网络字节顺序转换为主机字节顺序
}
void CTcpClientComm::PeerIp(char* ip, int len)
{
	sockaddr_in addr;
	int addrlen(sizeof(sockaddr));
	::getpeername(m_Socket, (sockaddr*)&addr, &addrlen);
	inet_ntop(AF_INET, &addr.sin_addr, ip, len);
	//strncpy_s(ip, len, inet_ntop(addr.sin_addr), len-1);
}
unsigned short CTcpClientComm::PeerPort()
{
	sockaddr_in addr;
	int addrlen(sizeof(sockaddr));
	::getpeername(m_Socket, (sockaddr*)&addr, &addrlen);
	return ntohs(addr.sin_port);
}
void CTcpClientComm::LocalMac(char* mac, int len)
{
	char ip[40];
	LocalIp(ip, sizeof(ip));

	CNetInfo NetInfo;
	NetInfo.GetMacByIp(mac, len, ip);
}
