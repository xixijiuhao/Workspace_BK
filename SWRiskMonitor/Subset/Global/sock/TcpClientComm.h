#pragma once


#include "TcpClient.h"


class CTcpClientComm : public TTcpClient, public TTcpClient::TNotify
{
public:
	CTcpClientComm();
private:
	ITcpClientNotify*	m_Notify;

//TTcpClient::TNotify
	virtual void OnConnect(TTcpClient* self);
	virtual void OnDisconnect(TTcpClient* self, DisReason reason);
	virtual int OnRecvData(TTcpClient* self, const char* buf, const int len);
public:
//ITcpClient
	virtual void SetClientNotify(ITcpClientNotify* notify);
	virtual bool Connect(const char* ip, const unsigned short port, const int timeout = 10 * 1000 * 1000);
	virtual bool Disconnect();
	virtual bool Send(const char* buf, const int len);
	virtual bool IsConnected();

	virtual void LocalIp(char* ip, int len);
	virtual unsigned short LocalPort();
	virtual void PeerIp(char* ip, int len);
	virtual unsigned short PeerPort();
	virtual void LocalMac(char* mac, int len);
private:
	CTcpClientComm(const CTcpClientComm &rhs);
	CTcpClientComm &operator=(const CTcpClientComm &rhs);
};