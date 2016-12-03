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
#define TOTAL_PLAYERS 1
#define TOTAL_BARNS 4
#define Z_MARKER_POSITION 2.5
#define SND_HEN1 1
#define SND_HEN2 2
#define SND_HEN3 3
#define SND_BACKGROUND 4

using namespace std;

enum MoveEnum {LEFT, RIGHT};

class CSGame;

class CPlayer
{
public:
	MATRIX4D m_View;
	MATRIX4D m_LastView;
	int m_nScore;
	int m_nHP;
	char m_szName[32];

private:
	bool m_bIsHuman;
	int m_dJoysticNumber;
	bool m_bBarnChoosed;

	int m_dCurrentBarn;
	ScenarioObject * m_pSelecter;
	CSGame *m_pOwner;

	static vector<bool> m_vBoardBarnsChoosed;

public:
	CPlayer() {} // avoid using this
	CPlayer(bool i_isHuman, int i_joysticNumber, ScenarioObject *i_pSelecter, CSGame *i_pOwner);

	void MoveSelector(MoveEnum move);
	bool MoveSelector(int barn);
	void ChooseBarn();
	inline int GetCurrentBarnChoosed() { return m_dCurrentBarn; }
	inline void Show() { m_pSelecter->setPaint(true); }
	inline void Hide() { m_pSelecter->setPaint(false); }
	inline const char * GetPlayerName() { return m_szName; }
	inline ScenarioObject *GetScenarioObject() { return m_pSelecter; }
	inline static void InitializeBoard(int total) { m_vBoardBarnsChoosed.resize(total, false); }
	inline bool BarnIsChoosed() { return m_bBarnChoosed; }
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
	map<int, ScenarioPosition *> &GetBarnPositions() { return barnScenarioPositions; }
protected:
	// Meshes
	CMesh *barnMesh;
	CMesh *henMesh;
	CMesh *sphereMesh;
	CSndControl* m_pSndBackground;

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
	int m_dHensInBarn;
	map<int, ScenarioObject *>::iterator it;

	// HSM methods
	unsigned long OnEvent(CEventBase* pEvent);
	void OnEntry();
	void OnExit();

	// Game actions, control and configuration
	void manageHensMovement();
	void createScenarioElements(int totalSpheres);
	void fixingSelector();
	void OwnBarn(CPlayer *player);

	// User control selection and score
	int totalPlayers;
	int totalBarns;
	int greatestHensInBarn;
	int greatestBarnId;
	vector<CPlayer *> m_vPlayers;
	vector<VECTOR4D> markerColors;
	vector<VECTOR4D> barnColors;
	void createUserSelectionMarker();
	bool selectionDone;
	bool hensOutPainted;
	bool showWinner;
	void repaintHens();
	void verifyUserSelectionDone();
	void moveHensBackwards();

	// BG
	ID3D11ShaderResourceView* m_pSRVBackground;

public:
	CSGame();
	virtual ~CSGame();
};

