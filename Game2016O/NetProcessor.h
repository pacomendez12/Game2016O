#pragma once
#include "HSM\EventListener.h"
#include "HSM\EventBase.h"
#include "HSM\StateMachineManager.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string>
#include <list>
#include <map>
using namespace std;
class CNetProcessor;
struct CLIENT
{
	SOCKET Client;        //Socket key
	SOCKADDR_IN Address;  //Client Address
	string NickName;      //Nickname
	int    AppKey;        //Application key
	CNetProcessor* pOwner; //Net processor owner.
};

#define EVENT_NETWORK 0xaa55aa55
class CEventNetwork :public CEventBase
{
public:
	enum NetworkMessage{
		NM_FAIL = -1,
		NM_OK = 0,
		NM_CLIENT_DISCONNECTED,
		NM_CLIENT_CONNECTED,
		NM_LISTENING,
		NM_NOTLISTENING,
		NM_READY,
		NM_DATAGRAM
	};
	NetworkMessage  m_msg;
	SOCKET		  m_sktSourceDestiny;       //Socket fuente/destino del evento
	unsigned long m_bBradcast;      //Paquete solicita difusión. 
	unsigned long m_nDataSize;  //Datagrama de aplicación
	unsigned char m_Data[512];
	CEventNetwork() { 
		m_ulEventType = EVENT_NETWORK;
		m_msg = NM_OK;
		m_bBradcast = 0; 
		m_sktSourceDestiny = 0; }
};
class CNetProcessor :
	public CEventListener
{
protected:
	WSADATA m_wsa;
	//Unificando el modelo cliente/servidor
	SOCKET  m_Listener;
	map<SOCKET, CLIENT> m_mapClients;
	SOCKET  m_Server;
	list<CEventNetwork*> m_lstEventsToSend;
	list<CEventNetwork*> m_lstEventsReceived;
	CRITICAL_SECTION m_csLock; //Mutex
	CStateMachineManager* m_pHSMOwner;
	static INT_PTR CALLBACK DlgConnect(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
public:
	void Lock() { EnterCriticalSection(&m_csLock); };
	void Unlock() { LeaveCriticalSection(&m_csLock); }
	unsigned long OnEvent(CEventBase* pEvent);
	bool InitNetwork(void);
	void DoNetworkTasks(void);
	bool Connect(wchar_t * pszAddress,int nPort=6112); //Conectarse a una partida remota
	void Disconnect(); //Desconectarse de la partida remota.
	bool IsListening() { return m_Listener != 0; };
	bool GetClientBySocket(SOCKET sktClient,CLIENT& OutClient);
	bool StartListening();
	void StopListening();
	bool ShowConnectDialog(HWND hWnd, HINSTANCE hInstance);
	void Uninitialize(void);
	static DWORD WINAPI ServiceListener(CNetProcessor* pNP);
	static DWORD WINAPI ClientThread(CLIENT* pClient);
	CNetProcessor(CStateMachineManager* pHSMOwner);
	virtual ~CNetProcessor();
};

