#pragma once
#include "HSM\EventListener.h"
#include "HSM\EventBase.h"
#include  "HSM\StateMachineManager.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <list>
#include <string>
#include <map>

class CNetProcessor;

struct CLIENT
{
	SOCKET Client;
	std::string NickName;
	CNetProcessor *pOwner;
	//Add data for game
};


#define EVENT_NETWORK 0xaa55aa55
class CEventNetwork : public CEventBase
{
	friend class CNetProcessor;
protected:
	SOCKET			Source;			// Socket fuente del evento: Server, un cliente

public:
	unsigned long	m_nDataSize;	// Datagrama de aplicacion
	unsigned char	m_Data[512];
};

class CNetProcessor :
	public CEventListener
{
protected:
	WSADATA m_wsa;
	//unificando el modelo cliente servidor
	//as a server
	SOCKET m_Listener;
	std::map<SOCKET, CLIENT> m_mapClients;

	//as a client
	SOCKET m_Server;

	//data
	std::list<CEventNetwork *> m_lstEventsToSend;
	std::list<CEventNetwork *> m_lstEventsReceived;
	CRITICAL_SECTION m_csLock;
	CStateMachineManager *m_pHSMOwner;
public:
	inline void Lock()
	{
		EnterCriticalSection(&m_csLock);
	}
	inline void Unlock()
	{
		LeaveCriticalSection(&m_csLock);
	}
	unsigned long OnEvent(CEventBase* pEvent);
	bool InitNetwork(void);
	void DoNetworkTasks(void);
	bool Connect(wchar_t *pszAddress);	//conectarse a una partida remota
	void Uninitialize(void);
	static DWORD WINAPI ServiceListener(CNetProcessor *pNP);
	static DWORD WINAPI ClientThread(CLIENT * pClient);
	CNetProcessor(CStateMachineManager *pHSMOwner);
	virtual ~CNetProcessor();
};

