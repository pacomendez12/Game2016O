#pragma once
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <map>
#include <time.h>
#include "HSM\StateBase.h"
#include "Graphics\Mesh.h"
#include "Camera.h"
#include "Scenario.h"
#include "ScenarioPosition.h"
#include "Barn.h"
#include "Hen.h"
#include "JumpingHen.h"
#include "InputProcessor.h"

#define CLSID_CSGame 0x33221100
#define TOTAL_HENS 50
#define TOTAL_PLAYERS 3
#define TOTAL_BARNS 4
#define Z_MARKER_POSITION 2.5

using namespace std;

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
	// Meshes
	CMesh *barnMesh;
	CMesh *henMesh;
	CMesh *sphereMesh;

	// General Variables
	CMesh *m_pTable;
	CMesh *m_pMallet;
	CCamera *m_pCamera;
	Scenario *staticScenario;
	Scenario *dynamicScenario;
	bool game;
	bool userInteraction;
	Hen *hen;
	vector<int> incrementInBarns;
	vector<int> removeHens;
	map<int, ScenarioPosition *> barnScenarioPositions;
	map<int, ScenarioObject *>::iterator it;

	// HSM methods
	unsigned long OnEvent(CEventBase* pEvent);
	void OnEntry();
	void OnExit();

	// Game actions, control and configuration
	void manageScenarioObjectUpdates();
	void createScenarioElements(int totalSpheres);

	// User control selection and score
	int totalPlayers;
	int totalBarns;
	int greatestHensInBarn;
	int greatestBarnId;
	vector<int> userIds;
	vector<int> userSelectedBarn;
	vector<VECTOR4D> markerColors;
	vector<VECTOR4D> barnColors;
	void createUserSelectionMarker();

	// BG
	ID3D11ShaderResourceView* m_pSRVBackground;

public:
	CSGame();
	virtual ~CSGame();
};

