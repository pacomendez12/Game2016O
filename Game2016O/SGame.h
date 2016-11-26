#pragma once
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
#define TOTAL_HENS 100

using namespace std;

class CSGame :
	public CStateBase
{
public:
	unsigned long GetClassID() { return CLSID_CSGame; }
	const char* GetClassString() { return "CSGame"; }
protected:
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
	int greatestHensInBarn;
	int greatestBarnId;
	int userSelectedBarn;

public:
	CSGame();
	virtual ~CSGame();
};

