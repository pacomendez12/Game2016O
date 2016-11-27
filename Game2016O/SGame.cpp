#include "stdafx.h"
#include "SGame.h"
#include <cstdio>
#include "SMain.h"
#include "HSM\StateMachineManager.h"
#include "BlenderImporter.h"
#include <memory>
#include <iostream>


void CSGame::createScenarioElements(int totalSpheres)
{
	barnMesh = MAIN->GetMeshByString("casa");
	henMesh = MAIN->GetMeshByString("esfera");

	markerColors.push_back({255,255,255,0}); // White
	markerColors.push_back({ 255,0,255,0 }); // Moradp
	markerColors.push_back({ 0,0,0,0 }); // Black

	barnColors.push_back({ 255,255,0,0 }); // Yellow
	barnColors.push_back({ 255,0,0,0 }); // Red
	barnColors.push_back({0, 255, 0, 0}); // Green
	barnColors.push_back({0, 0, 255, 0}); //Blue

	int newScenarioObjectId = 0;
	
	barnScenarioPositions[0] = new  ScenarioPosition(1.5, -7, 0);
	barnScenarioPositions[1] = new  ScenarioPosition(0, -2, 0);
	barnScenarioPositions[2] = new  ScenarioPosition(0, 2.25, 0);
	barnScenarioPositions[3] = new  ScenarioPosition(1.5, 7, 0);

	// Creating barns
	for (int i = 0; i < 4; i++) {
		newScenarioObjectId = staticScenario->getNewScenarioObjectId();
		staticScenario->addElementToScenario(newScenarioObjectId, new Barn(newScenarioObjectId, 1,
			barnMesh, barnScenarioPositions[newScenarioObjectId], true, barnColors[i]));
	}

	// Creating hens
	srand(time(NULL));
	int barnId = 0;
	int x;
	int y;
	int steptsToTarget;
	for (int i = 0; i < totalSpheres; i++) {
		barnId = rand() % 4;
		x = rand() % 100 + 70;
		y = rand() % 200 - 100;
		//x = 1;
		//y = 1;
		steptsToTarget = rand() % 1000 + 500;
		//cout << barnId << " (" << x << "," << y << ")" << endl;
		newScenarioObjectId = dynamicScenario->getNewScenarioObjectId();
		dynamicScenario->addElementToScenario(
			newScenarioObjectId,
			new Hen(newScenarioObjectId, 0.2, henMesh,
				new ScenarioPosition(x, y, 0), false, barnScenarioPositions[barnId],
				steptsToTarget, barnId, {1,1,1,0}));
	}
}

void CSGame::OnEntry()
{
	printf("CSGame::OnEntry()\n"); fflush(stdout);
	
	staticScenario = new Scenario();
	dynamicScenario = new Scenario();

	m_pCamera = new CCamera(MAIN->m_pDXPainter);
	m_pCamera->ChangeView(CCamera::ViewMode::Default);

	game = false;
	userInteraction = false;
	greatestHensInBarn = 0;

	totalPlayers = TOTAL_PLAYERS;
	totalBarns = TOTAL_BARNS;

	for (int i = 0; i < totalPlayers; i++) {
		userSelectedBarn.push_back(0);
	}

	createScenarioElements(TOTAL_HENS);
}

#include "ActionEvent.h"
unsigned long CSGame::OnEvent(CEventBase * pEvent)
{
	if (ACTION_EVENT == pEvent->m_ulEventType)
	{
		auto Action = (CActionEvent *)pEvent;
		if (userInteraction) {
			//cout << "User interaction enabled" << endl;
			float Stimulus;
			switch (Action->m_nAction) {
			case JOY_AXIS_RIGHT_PRESSED:
				//cout << userSelectedBarn << endl;
				userSelectedBarn[0]--;
				userSelectedBarn[0] = userSelectedBarn[0] % 4;
				if (userSelectedBarn[0] < 0) {
					userSelectedBarn[0] += 4;
				}
				dynamicScenario->getScenarioObect(userIds[0])->moveTo(barnScenarioPositions[userSelectedBarn[0]]);
				dynamicScenario->getScenarioObect(userIds[0])->setZ(Z_MARKER_POSITION);
				//cout << userSelectedBarn << endl;
				break;
			case JOY_AXIS_LEFT_PRESSED:
				//cout << userSelectedBarn << endl;
				userSelectedBarn[0]++;
				userSelectedBarn[0] = userSelectedBarn[0] % 4;
				if (userSelectedBarn[0] < 0) {
					userSelectedBarn[0] += 4;
				}
				dynamicScenario->getScenarioObect(userIds[0])->moveTo(barnScenarioPositions[userSelectedBarn[0]]);
				dynamicScenario->getScenarioObect(userIds[0])->setZ(Z_MARKER_POSITION);
				//cout << userSelectedBarn << endl;
				break;
			case JOY_BUTTON_B_PRESSED:
				cout << "Button B pressed" << endl;
				for (int i = 0; i < totalBarns; i++) {
					Barn *barn = dynamic_cast<Barn*>(staticScenario->getScenarioObect(i));
					cout << barn->getHensInHouse() << endl;
					if (barn->getHensInHouse() > greatestHensInBarn) {
						greatestBarnId = barn->getObjectId();
						greatestHensInBarn = barn->getHensInHouse();
					}
				}
				cout << "GreatestBarn " << greatestBarnId << ":" << greatestHensInBarn << endl;

				for (int i = 0; i < totalPlayers; i++) {
					cout << "User: " << i << " selected Barn " << userSelectedBarn[i] << endl;
				}

				for (int i = 0; i < totalPlayers; i++) {
					if (userSelectedBarn[i] == greatestBarnId) {
						cout << "User: " << i << " GANO" << endl;
					}
					else {
						cout << "User: " << i << " PERDIO" << endl;
					}
				}
				break;
			case JOY_BUTTON_Y_PRESSED:
				cout << "Button Y pressed" << endl;
				break;
			}
		}
		else {
			float Stimulus;
			switch (Action->m_nAction)
			{
			case JOY_AXIS_RX:
				//Dead zone
				Stimulus = (fabs(Action->m_fAxis) < 0.2 ? 0.0f : Action->m_fAxis);
				if (Stimulus != 0.0){
					m_pCamera->MoveXAxe(Stimulus);
				}
				break;
			case JOY_AXIS_RY:
				//Dead zone
				Stimulus = (fabs(Action->m_fAxis) < 0.2 ? 0.0f : Action->m_fAxis);
				if (Stimulus != 0.0){
					m_pCamera->MoveZAxe(Stimulus);
				}
				break;
			case JOY_AXIS_LX:
				Stimulus = (fabs(Action->m_fAxis) < 0.2 ? 0.0f : Action->m_fAxis);
				if (Stimulus != 0.0){
					m_pCamera->RotateXAxe(Stimulus);
				}
				break;
			case JOY_AXIS_LY:
				Stimulus = (fabs(Action->m_fAxis) < 0.2 ? 0.0f : Action->m_fAxis * -1);
				if (Stimulus != 0.0){
					m_pCamera->RotateYAxe(Stimulus);
				}
				break;
			case JOY_BUTTON_A_PRESSED: {
				cout << "Button A pressed" << endl;
				cout << game << endl;
					map<int, ScenarioObject *> objects = dynamicScenario->getScenarioObjects();
					map<int, ScenarioObject *>::iterator iterator;
					for (iterator = objects.begin(); iterator != objects.end(); iterator++) {
						ScenarioObject *scenarioObject = iterator->second;
						scenarioObject->setPaint(true);
					}
					if (!game)
						game = true;
					else
						game = false;
					cout << game << endl;
			}
				break;
			}
		}
	}

	if (APP_LOOP == pEvent->m_ulEventType)
	{
		auto Paint = MAIN->m_pDXPainter;
		auto DXManager = MAIN->m_pDXManager;
		Paint->SetRenderTarget(DXManager->GetMainRTV(), //Backbuffer
			DXManager->GetMainDSV()); //ZBuffer

		// Fondo
		VECTOR4D DeepBlue = { 0.2, 0.2, 0.7, 0 };
		DXManager->GetContext()->ClearDepthStencilView(DXManager->GetMainDSV(),
					D3D11_CLEAR_STENCIL | D3D11_CLEAR_DEPTH, 1.0F, 0.0);

		// Configuras el color de fondo
		DXManager->GetContext()->ClearRenderTargetView(DXManager->GetMainRTV(), (float *)&DeepBlue);

		// Luces
		Paint->m_Params.Lights[0].Type = LIGHT_DIRECTIONAL;
		Paint->m_Params.Lights[0].Flags = LIGHT_ON;

		Paint->m_Params.Lights[0].Direction = { -1, 0, 0}; // Modificar la direccion de la luz
		Paint->m_Params.Lights[0].Diffuse = { 1, 1, 1, 0 };
		Paint->m_Params.Lights[0].Position = { -100, 0, 0 , 0};
		Paint->m_Params.Flags = 0;
		Paint->m_Params.Material.Diffuse = { 1, 1, 1, 0 };
		Paint->m_Params.Material.Emissive = { 0, 0, 0, 0 };

		// Moving elements in scenario
		if (game) {
			map<int, ScenarioObject *> scenarioObjects = dynamicScenario->getScenarioObjects();
			for (it = scenarioObjects.begin(); it != scenarioObjects.end(); it++) {
				hen = dynamic_cast<Hen*>(it->second);
				hen->move();
				if (hen->alreadyInBarn()) {
					incrementInBarns.push_back(hen->getBarnId());
					removeHens.push_back(hen->getObjectId());
				}
			}
			manageScenarioObjectUpdates();
		}

		// Painting elements in the scenario
		staticScenario->paintScenarioObjects(Paint);
		dynamicScenario->paintScenarioObjects(Paint);

		DXManager->GetSwapChain()->Present(1, 0);
	}
	return __super::OnEvent(pEvent);
}

void CSGame::OnExit()
{
	//SAFE_DELETE(m_pGeometry);
	SAFE_DELETE(m_pCamera)
	printf("CSGame::OnExit()\n"); fflush(stdout);
}

void CSGame::manageScenarioObjectUpdates(){
	int incrementSize = incrementInBarns.size();
	int removeHensSize = removeHens.size();

	for (int i = 0; i < incrementSize; i++) {
		staticScenario->getScenarioObect(incrementInBarns[i])->incrementScore(1);
	}

	for (int i = 0; i < removeHensSize; i++) {
		dynamicScenario->removeScenarioObjectById(removeHens[i]);
	}

	incrementInBarns.clear();
	removeHens.clear();

	if (dynamicScenario->getScenarioObjects().empty()) {
		// Stop hens to move
		game = false;

		// Allow user to move
		userInteraction = true;
		MAIN->m_pInputProcessor->SetJoysticMode(CInputProcessor::JoysticMode::KEYBOARD);

		// Create user selector
		createUserSelectionMarker();
	}
}

void CSGame::createUserSelectionMarker()
{
	userIds.resize(totalPlayers);
	for (int i = 0; i < totalPlayers; i++) {
		userIds[i] = dynamicScenario->getNewScenarioObjectId();
		ScenarioObject *so = new ScenarioObject(userIds[i], 0.2, henMesh, barnScenarioPositions[i], true, markerColors[i]);
		so->setZ(Z_MARKER_POSITION);
		dynamicScenario->addElementToScenario(userIds[i], so);
	}
}

CSGame::CSGame()
{
	m_pCamera = nullptr;
}

CSGame::~CSGame()
{
}
