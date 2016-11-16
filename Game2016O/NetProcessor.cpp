#include "stdafx.h"
#include "NetProcessor.h"


unsigned long CNetProcessor::OnEvent(CEventBase * pEvent)
{
	((CEventNetwork *)pEvent->m_ulEventType)->Source = 0;
	Lock();
	m_lstEventsToSend.push_back((CEventNetwork *)pEvent);
	Unlock();
	return 0;
}

bool CNetProcessor::InitNetwork(void)
{
	WSAStartup(MAKEWORD(2, 2), &m_wsa);
	DWORD dwThreadID;
	SOCKADDR_IN Service;
	m_Listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	InetPton(AF_INET, L"0.0.0.0", &Service.sin_addr);
	Service.sin_port = htons(6112);
	Service.sin_family = AF_INET;
	if (SOCKET_ERROR == bind(m_Listener, (SOCKADDR*)&Service, sizeof(SOCKADDR_IN)))
	{
		closesocket(m_Listener);
		m_Listener = 0;
		return false;
	}
	listen(m_Listener, SOMAXCONN);
	CreateThread(NULL, 4096, (LPTHREAD_START_ROUTINE)CNetProcessor::ServiceListener, this, 0, &dwThreadID);
	return true;
}

void CNetProcessor::DoNetworkTasks(void)
{
	Lock();
	for (auto s : m_lstEventsReceived)
	{
		auto d = new CEventNetwork();
		*d = *s;
		m_lstEventsToSend.push_back(d);
	}

	while (!m_lstEventsToSend.empty()) {
		CEventNetwork *pEvent = m_lstEventsToSend.front();
		for (auto c : m_mapClients)
		{
			if (pEvent->Source != c.first) // No regresarlo al origen
			{
				send(c.second.Client, (char *)pEvent->m_nDataSize, sizeof(unsigned long), 0);
				send(c.second.Client, (char *)pEvent->m_Data, pEvent->m_nDataSize, 0);
			}
		}
		m_lstEventsToSend.pop_front();
		delete pEvent;
	}

	for (auto s : m_lstEventsReceived)
	{
		m_pHSMOwner->PostEvent(s);
	}
	Unlock();
}

bool CNetProcessor::Connect(wchar_t * pszAddress)
{
	SOCKET Server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	SOCKADDR_IN ServerAddress;
	memset(&ServerAddress, 0, sizeof(ServerAddress));
	InetPton(AF_INET, pszAddress, &ServerAddress.sin_addr);
	ServerAddress.sin_family = AF_INET;
	ServerAddress.sin_port = htons(6112);
	if (SOCKET_ERROR == connect(Server, (SOCKADDR *)&ServerAddress, sizeof(ServerAddress)))
	{
		closesocket(Server);
		return false;
	}
	
	CLIENT Cliente;
	Cliente.NickName = "Anonimo";
	Cliente.pOwner = this;
	Cliente.Client = Server;
	Lock();
	m_mapClients.insert_or_assign(Server, Cliente);
	Unlock();
	return true;
}

void CNetProcessor::Uninitialize(void)
{
}

DWORD CNetProcessor::ServiceListener(CNetProcessor * pNP)
{
	/*while (true)
	{
		DWORD dwThreadID;
		SOCKADDR_IN ClientAddress;
		int nClientAddrSize = sizeof(SOCKADDR_IN);;
		SOCKET Client = accept(pNP->m_Listener, (SOCKADDR*)&ClientAddress, &nClientAddrSize);
		CLIENT &NewClient = *new CLIENT;
		NewClient.Client = Client;
		NewClient.NickName = "Anonymous";
		NewClient.pOwner = pNP;
		pNP->Lock();
		pNP->m_mapClients.insert_or_assign(Client, NewClient);
		pNP->Unlock();

		// Thread que atiende a los clientes
		CreateThread(NULL, 4096, (LPTHREAD_START_ROUTINE)CNetProcessor::ClientThread, &NewClient, 0, &dwThreadID );
	}*/
	return 0;
}

DWORD CNetProcessor::ClientThread(CLIENT * pClient)
{
	while (true)
	{
		CEventNetwork *pEN = new CEventNetwork();
		pEN->m_ulEventType = EVENT_NETWORK;
		if (SOCKET_ERROR == recv(pClient->Client, (char *)pEN->m_nDataSize, sizeof(unsigned long), 0))
		{
			delete pEN;
			break;
		}
		if (SOCKET_ERROR == recv(pClient->Client, (char *)pEN->m_Data, pEN->m_nDataSize, 0))
		{
			delete pEN;
			break;
		}
		
		pEN->Source = pClient->Client;
		pClient->pOwner->Lock();
		pClient->pOwner->m_lstEventsReceived.push_back(pEN);
		pClient->pOwner->Unlock();
	}

	//unsubscription
	pClient->pOwner->Lock();
	pClient->pOwner->m_mapClients.erase(pClient->Client);
	pClient->pOwner->Unlock();
	closesocket(pClient->Client);
	delete pClient;
	return 0;
}

CNetProcessor::CNetProcessor(CStateMachineManager *pHSMOwner):
	m_pHSMOwner(pHSMOwner)
{
	InitializeCriticalSection(&m_csLock);
}


CNetProcessor::~CNetProcessor()
{
	DeleteCriticalSection(&m_csLock);
}
