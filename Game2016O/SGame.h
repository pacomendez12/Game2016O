#pragma once
#include "HSM\StateBase.h"
#include "Graphics\Mesh.h"
#include "Camera.h"
#include "Scenario.h"
#include <WinSock2.h>
#include <WS2tcpip.h>

#define CLSID_CSGame 0x33221100

class CPlayer
{
public:
	MATRIX4D m_View;
	MATRIX4D m_LastView;
	int m_nScore;
	int m_nHP;
	char m_szName[32];
};
struct GAMEDGRAM
{
	enum {
		NOP,
		REQUEST_KEY,
		GET_KEY,
		SET_KEY,
		PLAYER_REQUEST_CONNECT,
		PLAYER_REQUEST_GRANTED,
		PLAYER_REQUEST_DENIED,
		PLAYER_DISCONNECT,
		PLAYER_NETWORK_READY,
		PLAYER_SET_STATE,
		PLAYER_GET_STATE,
		PLAYER_REQUEST_PAUSE,
		PLAYER_REQUEST_CONTINUE,
		PLAYER_BROADCAST_MESSAGE,
		GAME_PAUSE,
		GAME_CONTINUE,
		GAME_OVER,
		GAME_START,
		_unused = 0x7fffffff
	};
	unsigned long m_ulCommand;
	unsigned long m_ulSourceKey;
	union
	{
		MATRIX4D M;
		VECTOR4D V;
		float    f;
		int		 i;
		char	 szCadena[256];
		CPlayer  PlayerState;
	};
};


class CSGame :
	public CStateBase
{
public:
	map<unsigned long, CPlayer> m_mapLocalPlayers;  //Ordinal,Jugador
	map<unsigned long, CPlayer> m_mapRemotePlayers; //Ordinal,Jugador
	map<unsigned long, SOCKET> m_mapKey2Socket;
	std::map<SOCKET, unsigned long> m_mapSocket2Key;
	unsigned long GetClassID() { return CLSID_CSGame; }
	const char* GetClassString() { return "CSGame"; }
protected:
	CMesh *m_pTable;
	CMesh *m_pMallet;
	CCamera *m_pCamera;
	Scenario *scenario;
	ID3D11ShaderResourceView* m_pSRVBackground;
	void OnEntry();
	unsigned long OnEvent(CEventBase* pEvent);
	void OnExit();
public:
	CSGame();
	virtual ~CSGame();
};

