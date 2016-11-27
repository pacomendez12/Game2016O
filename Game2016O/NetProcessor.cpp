#include "stdafx.h"
#include "NetProcessor.h"
unsigned long CNetProcessor::OnEvent(CEventBase * pEvent)
{
	Lock();
	m_lstEventsToSend.push_back((CEventNetwork*)pEvent);
	Unlock();
	return 0;
}
bool CNetProcessor::InitNetwork(void)
{
	printf("Initializing network resources...\n"); fflush(stdout);
	WSAStartup(MAKEWORD(2, 2), &m_wsa);
	return true;
}
void CNetProcessor::Uninitialize(void)
{
	printf("Uninitializing network resources...\n"); fflush(stdout);
	WSACleanup();
}

bool CNetProcessor::GetClientBySocket(SOCKET sktClient,CLIENT& OutClient)
{
	bool bResult=false;
	Lock();
	auto it=m_mapClients.find(sktClient);
	if (it != m_mapClients.end())
	{
		OutClient = it->second;
		bResult = true;
	}
	Unlock();
	return bResult;
}

bool CNetProcessor::StartListening()
{
	printf("Seting up game service..."); fflush(stdout);
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
	printf("Done.\n"); fflush(stdout);
	CreateThread(NULL, 4096, (LPTHREAD_START_ROUTINE)CNetProcessor::ServiceListener, this, 0, &dwThreadID);
	return true;
}
void CNetProcessor::StopListening()
{
	printf("Stoping listening...."); fflush(stdout);
	closesocket(m_Listener);
	printf("Done.\n"); fflush(stdout);
	m_Listener = 0;
}


void CNetProcessor::DoNetworkTasks(void)
{
	Lock();
	//Enviar respuestas especificas a un equipo 
	while (!m_lstEventsToSend.empty())
	{
		auto e = m_lstEventsToSend.front();
		if (e->m_sktSourceDestiny != 0 && !e->m_bBradcast)
		{
			send(e->m_sktSourceDestiny, (char*)&e->m_nDataSize, sizeof(unsigned long), 0);
			send(e->m_sktSourceDestiny, (char*)e->m_Data, e->m_nDataSize, 0);
		}
		else
		{
			for (auto c : m_mapClients)
			{
				send(c.first, (char*)&e->m_nDataSize, sizeof(unsigned long), 0);
				send(c.first, (char*)e->m_Data, e->m_nDataSize, 0);
			}
		}
		delete e;
		m_lstEventsToSend.pop_front();
	}
	for (auto e : m_lstEventsReceived)
	{
		m_pHSMOwner->PostEvent(e);
		if (e->m_bBradcast)
		{
			for (auto c : m_mapClients)
			{
				if (e->m_sktSourceDestiny != c.first)
				{
					send(c.first, (char*)&e->m_nDataSize, sizeof(unsigned long), 0);
					send(c.first, (char*)e->m_Data, e->m_nDataSize, 0);
				}
			}
		}
	}
	m_lstEventsReceived.clear();
	Unlock();
}

bool CNetProcessor::Connect(wchar_t * pszAddress,int nPort)
{
	ADDRINFOW hint,*result=0;
	SOCKADDR_IN ServerAddress;
	memset(&hint, 0, sizeof(hint));
	memset(&ServerAddress, 0, sizeof(ServerAddress));
	hint.ai_family = AF_INET;
	hint.ai_socktype = SOCK_DGRAM;
	hint.ai_protocol = IPPROTO_TCP;
	if (GetAddrInfoW(pszAddress, NULL, &hint, &result))
		return false;
	ServerAddress = *(SOCKADDR_IN*)result->ai_addr;
	FreeAddrInfoW(result);
	m_Server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	ServerAddress.sin_family = AF_INET;
	ServerAddress.sin_port = htons(nPort);
	if(SOCKET_ERROR==connect(m_Server, (SOCKADDR*)&ServerAddress, sizeof(ServerAddress)))
	{
		closesocket(m_Server);
		m_Server = 0;
		return false;
	}
	CLIENT Cliente;
	Cliente.NickName = "Anonimo";
	Cliente.pOwner = this;
	Cliente.Client = m_Server;
	Cliente.Address = ServerAddress;
	Lock();
	m_mapClients.insert_or_assign(m_Server, Cliente);
	Unlock();
	return true;
}

void CNetProcessor::Disconnect()
{
	if (m_Server)
		closesocket(m_Server);
}

DWORD CNetProcessor::ServiceListener(CNetProcessor * pNP)
{
	printf("Listening...\n"); fflush(stdout);
	listen(pNP->m_Listener, SOMAXCONN);
	while (1)
	{
		DWORD dwThreadID;
		SOCKADDR_IN ClientAddress;
		int nClientAddrSize = sizeof(SOCKADDR_IN);
		SOCKET Client = accept(pNP->m_Listener,(SOCKADDR*)&ClientAddress,&nClientAddrSize);
		if (INVALID_SOCKET == Client) break;
		CLIENT &NewClient=*new CLIENT;
		NewClient.Client = Client;
		NewClient.NickName = "Anonimo";
		NewClient.pOwner = pNP;
		NewClient.Address = ClientAddress;
		pNP->Lock();
		pNP->m_mapClients.insert_or_assign(Client, NewClient);
		pNP->Unlock();
		CreateThread(NULL, 4096,(LPTHREAD_START_ROUTINE)CNetProcessor::ClientThread,&NewClient, 0, &dwThreadID);
		printf("Client accept"); fflush(stdout);
	}
	printf("Service shutdown... no more clients accepted.\n"); fflush(stdout);
	return 0;
}
DWORD CNetProcessor::ClientThread(CLIENT * pClient)
{
	while (1)
	{
		CEventNetwork* pEN=new CEventNetwork;
		pEN->m_ulEventType = EVENT_NETWORK;
		pEN->m_msg = CEventNetwork::NM_DATAGRAM;
		if (SOCKET_ERROR == recv(pClient->Client, (char*)pEN->m_bBradcast, sizeof(unsigned long), 0))
		{
			delete pEN;
			break;
		}
		if (SOCKET_ERROR == recv(pClient->Client, (char*)pEN->m_nDataSize, sizeof(unsigned long), 0))
		{
			delete pEN;
			break;
		}
		if (SOCKET_ERROR == recv(pClient->Client,(char*)pEN->m_Data, pEN->m_nDataSize, 0))
		{
			delete pEN;
			break;
		}
		pClient->pOwner->Lock();
		pEN->m_sktSourceDestiny = pClient->Client;
		pClient->pOwner->m_lstEventsReceived.push_back(pEN);
		pClient->pOwner->Unlock();
	}
	//Unsubscript
	pClient->pOwner->Lock();
	pClient->pOwner->m_mapClients.erase(pClient->Client);
	pClient->pOwner->Unlock();
	closesocket(pClient->Client);
	delete pClient;
	return 0;
}
CNetProcessor::CNetProcessor(CStateMachineManager* pHSMOwner)
{
	m_pHSMOwner = pHSMOwner;
	m_Server = 0;
	m_Listener = 0;
	InitializeCriticalSection(&m_csLock);
}
CNetProcessor::~CNetProcessor()
{
	bool bExit = false;
	Lock();
	for (auto c : m_mapClients) closesocket(c.first);
	Unlock();
	while (!bExit)
	{
		Lock();
		bExit = m_mapClients.size()==0;
		Unlock();
		Sleep(100);
	}
	DeleteCriticalSection(&m_csLock);
}

struct DLG_CONNECT_INFO
{
	wchar_t szAdressName[1024];
	wchar_t szPort[1024];
};
#include "Resource.h"
INT_PTR CALLBACK CNetProcessor::DlgConnect(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static DLG_CONNECT_INFO* ptr=0;
	switch (msg)
	{
	case WM_INITDIALOG:
		ptr = (DLG_CONNECT_INFO*)(lParam);
		SetWindowText(GetDlgItem(hDlg, IDC_EDIT_ADDRESS_NAME), ptr->szAdressName);
		SetWindowText(GetDlgItem(hDlg, IDC_EDIT_PORT), ptr->szPort);
		return (INT_PTR)TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDCANCEL:
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		case IDOK:
			GetWindowText(GetDlgItem(hDlg, IDC_EDIT_ADDRESS_NAME), ptr->szAdressName, 1023);
			GetWindowText(GetDlgItem(hDlg, IDC_EDIT_PORT), ptr->szPort, 1023);
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	}
	return FALSE;
}
bool CNetProcessor::ShowConnectDialog(HWND hWnd, HINSTANCE hInstance)
{
	static DLG_CONNECT_INFO Connection={L"127.0.0.1",L"6112"};
	switch (DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_CONNECT), hWnd, CNetProcessor::DlgConnect,(LPARAM)&Connection))
	{
	case IDOK:
		if (!Connect(Connection.szAdressName, _wtoi(Connection.szPort)))
		{
			MessageBox(hWnd, L"Unable to connect to specified server and service", L"Connection error", MB_ICONERROR);
			return false;
		}
		return true;
	}
	return false;
}


