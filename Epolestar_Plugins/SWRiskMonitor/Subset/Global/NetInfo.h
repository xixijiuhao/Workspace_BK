#pragma once

//#ifndef UNICODE
//#define UNICODE
//#endif
//#ifdef UNICODE
//#ifndef _UNICODE
//#define _UNICODE
//#endif
//#endif

#include <stdio.h>
#include <IPHlpApi.h>
#include "CharCodes.h"

#pragma comment(lib, "IPHlpApi.lib")

typedef struct TIpAddress
{
	TIpAddress *NEXT;
	TCHAR AdapterAddress[16];
}*PIpAddress;
typedef struct TDnsAddress
{
	TDnsAddress *NEXT;
	TCHAR AdapterDNS[16];
}*PDnsAddress;
typedef struct TAddressInfo
{
	PIpAddress AdapterAddresses; 
	PDnsAddress AdapterDNSs;
	TCHAR AdapterMac[18];
	TCHAR AdapterName[24];
	TAddressInfo *NEXT;
}*PAddressInfo;

#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

class CNetInfo
{
private:
	PIP_ADAPTER_ADDRESSES m_pAdapterAddr;
	PAddressInfo m_pAddrInfo;
	DWORD m_dwFamily;
	PIpAddress m_pUnicast[MAX_PATH];
	PDnsAddress m_pDns[MAX_PATH];

	void GetIP(TCHAR * pIp, int IpLeng, PSOCKET_ADDRESS pAddr)
	{
		_snprintf_s(pIp, IpLeng, IpLeng-1, "%d.%d.%d.%d",
			(BYTE)pAddr->lpSockaddr->sa_data[2], (BYTE)pAddr->lpSockaddr->sa_data[3],
			(BYTE)pAddr->lpSockaddr->sa_data[4], (BYTE)pAddr->lpSockaddr->sa_data[5]);
	}
	void GetMAC(TCHAR * pMac, size_t MacLeng, PIP_ADAPTER_ADDRESSES pAddr)
	{
		WORD Start=0;
		for (ULONG i=0; i<pAddr->PhysicalAddressLength && Start<MacLeng; i++)
		{
			_snprintf_s(pMac+Start, MacLeng-Start, MacLeng-Start-1, "%02X-", pAddr->PhysicalAddress[i]);
			Start+=3;
		}
		if (Start<MacLeng)
			pMac[Start]=0;
	}
	BOOL GetAdapterAddr(DWORD family)
	{
		if(m_pAdapterAddr == nullptr)
			m_pAdapterAddr = (PIP_ADAPTER_ADDRESSES)MALLOC(sizeof(IP_ADAPTER_ADDRESSES));

		ULONG sizePointer = 0;
		DWORD dwError = GetAdaptersAddresses(family, GAA_FLAG_INCLUDE_PREFIX, 0, m_pAdapterAddr, &sizePointer);
		if(dwError == ERROR_BUFFER_OVERFLOW)
		{
			if(m_pAdapterAddr != nullptr)
			{    
				FREE(m_pAdapterAddr);
				m_pAdapterAddr = nullptr;
			}
			m_pAdapterAddr = (PIP_ADAPTER_ADDRESSES)MALLOC(sizePointer);
			dwError = GetAdaptersAddresses(family, GAA_FLAG_INCLUDE_PREFIX, 0, m_pAdapterAddr, &sizePointer);
		}

		if (NO_ERROR==dwError)
			m_dwFamily = family;
		return NO_ERROR==dwError;
	}
public:
	CNetInfo()
		: m_dwFamily(255)
		, m_pAdapterAddr(nullptr)
		, m_pAddrInfo(nullptr)
	{
		memset(m_pUnicast, 0, sizeof(m_pUnicast));
		memset(m_pDns, 0, sizeof(m_pDns));
	}
	virtual ~CNetInfo()
	{
		if(m_pAdapterAddr)
			FREE(m_pAdapterAddr);
		FreeResult();
	}
	PAddressInfo GetAddressInfo(DWORD family)
	{
		if (family!=m_dwFamily)
			if (!GetAdapterAddr(family))
				return nullptr;
			

		FreeResult();
		if (m_pAddrInfo==nullptr)
		{
			m_pAddrInfo = (PAddressInfo)MALLOC(sizeof(TAddressInfo));
			memset(m_pAddrInfo, 0, sizeof(TAddressInfo));
		}

		int i=0, j=0;
		PAddressInfo pCurrInfo = m_pAddrInfo;
		PIP_ADAPTER_ADDRESSES pCurrentAddr = m_pAdapterAddr;
		while(pCurrentAddr)
		{		
			pCurrInfo->AdapterAddresses = (PIpAddress)MALLOC(sizeof(TIpAddress));
			memset(pCurrInfo->AdapterAddresses, 0, sizeof(TIpAddress));
			pCurrInfo->AdapterDNSs = (PDnsAddress)MALLOC(sizeof(TDnsAddress));
			memset(pCurrInfo->AdapterDNSs, 0, sizeof(TDnsAddress));

			strncpy_s(pCurrInfo->AdapterName, CCodes::UnicodeToANSI(pCurrentAddr->FriendlyName).c_str(), sizeof(pCurrInfo->AdapterName)-1);

			PIpAddress pIpInfo = pCurrInfo->AdapterAddresses;
			PIP_ADAPTER_UNICAST_ADDRESS_XP pUnicast = (PIP_ADAPTER_UNICAST_ADDRESS_XP)pCurrentAddr->FirstUnicastAddress;
			while(pUnicast)
			{
				GetIP(pIpInfo->AdapterAddress, sizeof(pIpInfo->AdapterAddress), &pUnicast->Address);
				m_pUnicast[i++]=pIpInfo;

				pUnicast = pUnicast->Next;
				if (pUnicast)
				{
					PIpAddress pIp = (PIpAddress)MALLOC(sizeof(TIpAddress));
					memset(pIp, 0, sizeof(*pIp));
					pIpInfo->NEXT = pIp;
					pIpInfo = pIp;
				}
			}

			PDnsAddress pDnsInfo = pCurrInfo->AdapterDNSs; 
			PIP_ADAPTER_DNS_SERVER_ADDRESS_XP pDnsAddr = pCurrentAddr->FirstDnsServerAddress;
			while(pDnsAddr)
			{						
				GetIP(pDnsInfo->AdapterDNS, sizeof(pDnsInfo->AdapterDNS), &pDnsAddr->Address);
				m_pDns[j++]=pDnsInfo;

				pDnsAddr = pDnsAddr->Next;
				if (pDnsAddr)
				{
					PDnsAddress pDns = (PDnsAddress)MALLOC(sizeof(TDnsAddress));
					memset(pDns, 0, sizeof(*pDns));
					pDnsInfo->NEXT = pDns;
					pDnsInfo = pDns;
				}
			}

			GetMAC(pCurrInfo->AdapterMac, sizeof(pCurrInfo->AdapterMac), pCurrentAddr);

			pCurrentAddr = pCurrentAddr->Next;				
			if (pCurrentAddr)
			{
				PAddressInfo pAdd = (PAddressInfo)MALLOC(sizeof(TAddressInfo));
				memset(pAdd, 0, sizeof(TAddressInfo));
				pCurrInfo->NEXT = pAdd;
				pCurrInfo = pAdd;
			}
		}
		
		return m_pAddrInfo;
	}
	void GetMacByIp(char * Mac, size_t MacLeng, char * IP)
	{
		PAddressInfo pAddr = GetAddressInfo(AF_INET);
		if (pAddr==nullptr)
			return;

		while(pAddr)
		{
			while(pAddr->AdapterAddresses)
			{
				if (strcmp(IP, pAddr->AdapterAddresses->AdapterAddress)==0)
				{
					strcpy_s(Mac, MacLeng, pAddr->AdapterMac);
					return;
				}
				pAddr->AdapterAddresses = pAddr->AdapterAddresses->NEXT;
			}
			pAddr = pAddr->NEXT;
		}
	}
	void FreeResult()
	{
		PAddressInfo pAddr;
		while(m_pAddrInfo)
		{
			pAddr = m_pAddrInfo;
			m_pAddrInfo = m_pAddrInfo->NEXT;
			FREE(pAddr);
			pAddr=nullptr;
		}	
	
		for (int i=0; i<MAX_PATH && m_pUnicast[i]; i++)
		{
			FREE(m_pUnicast[i]);
			m_pUnicast[i]=nullptr;
		}

		for (int i=0; i<MAX_PATH && m_pDns[i]; i++)
		{
			FREE(m_pDns[i]);
			m_pDns[i]=nullptr;
		}
	}
	void GetHostName(char * Name, size_t NameLeng)
	{
		::gethostname(Name, NameLeng);	
	};
};

