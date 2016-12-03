#include "stdafx.h"
#include "SGame.h"
#include <cstdio>
#include "SMain.h"
#include "HSM\StateMachineManager.h"
#include "BlenderImporter.h"
#include <memory>
#include <iostream>
#include "Graphics\ImageBMP.h"

vector<bool> CPlayer::m_vBoardBarnsChoosed;

CPlayer::CPlayer(bool i_isHuman, int i_joysticNumber, ScenarioObject *i_pSelecter, CSGame *i_pOwner) :
	m_bIsHuman(i_isHuman), m_dJoysticNumber(i_joysticNumber), m_pSelecter(i_pSelecter),
	m_dCurrentBarn(0), m_bBarnChoosed(false), m_pOwner(i_pOwner)
{
	std::string str((m_bIsHuman ? "Player " : "IA ") + to_string(m_dJoysticNumber + 1));
	strcpy_s(m_szName, str.c_str());
	MoveSelector(0);
}

bool CPlayer::MoveSelector(int barn)
{
	if (m_bBarnChoosed == true) return false;
	if (m_vBoardBarnsChoosed[barn] == true) return false;
	m_dCurrentBarn = barn;
	m_pSelecter->moveTo(m_pOwner->GetBarnPositions()[m_dCurrentBarn]);
	m_pSelecter->setZ(Z_MARKER_POSITION);
	return true;
}

void CPlayer::MoveSelector(MoveEnum move)
{
	if (m_bBarnChoosed) return;
	int current = m_dCurrentBarn;
	switch (move)
	{
	case MoveEnum::LEFT:
		current++;
		current = current % 4;
		if (current < 0)
		{
			current += 4;
		}
		break;
	case MoveEnum::RIGHT:
		current--;
		current = current % 4;
		if (current < 0) {
			current += 4;
		}
		break;
	}

	// if is not abailable move to next
	if (!MoveSelector(current))
	{
		m_dCurrentBarn = current;
		MoveSelector(move);
	}
}

void CPlayer::ChooseBarn()
{
	m_bBarnChoosed = true;
	m_vBoardBarnsChoosed[GetCurrentBarnChoosed()] = true;
}

void CSGame::createScenarioElements(int totalSpheres)
{
	int barnTotals[] = { 0,0,0,0 };
	int barnId = 0;
	int max = 0;
	int index = 0;
	int x = 0;
	int y = 0;
	int steptsToTarget;

	//"casa", "hen", "esfera"
	barnMesh = MAIN->GetMeshByString("casa");
	henMesh = MAIN->GetMeshByString("hen");
	sphereMesh = MAIN->GetMeshByString("esfera");

	markerColors.push_back({ 255,255,255,0 }); // White
	markerColors.push_back({ 255,0,255,0 }); // Moradp
	markerColors.push_back({ 0,0,0,0 }); // Black

	barnColors.push_back({ 255,255,0,0 }); // Yellow
	barnColors.push_back({ 255,0,0,0 }); // Red
	barnColors.push_back({ 0, 255, 0, 0 }); // Green
	barnColors.push_back({ 0, 0, 255, 0 }); //Blue

	int newScenarioObjectId = 0;

	barnScenarioPositions[0] = new  ScenarioPosition(1.5, -7, 0);
	barnScenarioPositions[1] = new  ScenarioPosition(0, -2, 0);
	barnScenarioPositions[2] = new  ScenarioPosition(0, 2.25, 0);
	barnScenarioPositions[3] = new  ScenarioPosition(1.5, 7, 0);

	// Creating barns
	for (int i = 0; i < 4; i++)
	{
		newScenarioObjectId = staticScenario->getNewScenarioObjectId();
		staticScenario->addElementToScenario(newScenarioObjectId, new Barn(newScenarioObjectId, 1,
			new CMesh(*barnMesh), barnScenarioPositions[newScenarioObjectId], true, barnColors[i]));
	}

	// creating Player selectors
	for (int i = 0; i < totalPlayers; i++)
	{ // i and id are the same
		int id = dynamicScenario->getNewScenarioObjectId();
		ScenarioObject *so = new ScenarioObject(i, 0.2, new CMesh(*sphereMesh),
			barnScenarioPositions[i], true, markerColors[i]);
		so->setZ(Z_MARKER_POSITION);
		dynamicScenario->addElementToScenario(id, so);
	}

	// Creating hens
	srand(time(NULL));

	for (int i = 0; i < totalSpheres; i++) {
		barnId = rand() % 4;
		barnTotals[barnId]++;
	}

	index = 0;
	max = barnTotals[0];
	for (int i = 1; i < 4; i++) {
		if (barnTotals[i] > max) {
			max = barnTotals[i];
			index = i;
		}
	}

	for (int i = 0; i < 4; i++) {
		if (barnTotals[i] == max && index != i) {
			barnTotals[index] += 5;
			barnTotals[i] -= 5;
			break;
		}
	}

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < barnTotals[i]; j++){
			x = rand() % 50 + 15;
			y = rand() % 30 - 20;
			steptsToTarget = rand() % 500 + 200;
			//cout << barnId << " (" << x << "," << y << ")" << endl;
			newScenarioObjectId = dynamicScenario->getNewScenarioObjectId();
			dynamicScenario->addElementToScenario(
				newScenarioObjectId,
				new Hen(newScenarioObjectId, 0.2, new CMesh(*henMesh),
					new ScenarioPosition(x, y, 0), false, barnScenarioPositions[i],
					steptsToTarget, i, { 1,1,1,0 }));
		}
	}
}

void CSGame::fixingSelector()
{
	const int offset = 1;
	vector<int> playersPerBarn(TOTAL_BARNS, 0);
	for (auto p : m_vPlayers)
	{
		//move z axis
		auto so = p->GetScenarioObject();
		//cout << "Z = " <<  << endl;
		so->setZ(Z_MARKER_POSITION + (offset * playersPerBarn[p->GetCurrentBarnChoosed()]));
		playersPerBarn[p->GetCurrentBarnChoosed()]++;
	}
}

void CSGame::OwnBarn(CPlayer *player)
{
	int barn = player->GetCurrentBarnChoosed();
	for (auto p : m_vPlayers)
	{
		if (player == p) continue;
		if (p->GetCurrentBarnChoosed() == barn)
		{
			p->MoveSelector(MoveEnum::RIGHT);
		}
	}
	fixingSelector();
}

void CSGame::OnEntry()
{
	printf("CSGame::OnEntry()\n"); fflush(stdout);
	
	staticScenario = new Scenario();
	dynamicScenario = new Scenario();

	m_pCamera = new CCamera(MAIN->m_pDXPainter);
	m_pCamera->ChangeView(CCamera::ViewMode::Default);

	CImageBMP* background =
		CImageBMP::CreateBitmapFromFile("..\\Assets\\background.bmp", NULL);
	ID3D11Texture2D* backgroundTexture = background->CreateTexture(MAIN->m_pDXManager);
	CImageBMP::DestroyBitmap(background);
	MAIN->m_pDXManager->GetDevice()->CreateShaderResourceView(backgroundTexture, NULL, &m_pSRVBackground);

	game = false;
	userInteraction = false;
	greatestHensInBarn = 0;

	totalPlayers = TOTAL_PLAYERS;
	totalBarns = TOTAL_BARNS;
	m_dHensInBarn = 0;

	createScenarioElements(TOTAL_HENS);

	CPlayer::InitializeBoard(TOTAL_BARNS);
	int realPlayers = MAIN->GetRealPlayersNumber();
	for (int i = 0; i < totalPlayers; i++) {
		bool isHuman = false;
		int joyPlayer = -1;
		if (realPlayers)
		{
			isHuman = true;
			joyPlayer = MAIN->GetRealPlayersNumber() - realPlayers;
			realPlayers--;
		}

		auto so = dynamicScenario->getScenarioObect(i);

		// Creating player
		
		CPlayer *player = new CPlayer(isHuman, joyPlayer, so, this);
		player->Hide();
		m_vPlayers.push_back(player);
		fixingSelector();
		MAIN->m_pSndManager->ClearEngine();
		m_pSndBackground = MAIN->m_pSndManager->LoadSoundFx(L"..\\Assets\\Banjo.wav", SND_BACKGROUND);
		/*if (m_pSndBackground)
			m_pSndBackground->Play(true);*/
	}

}

#include "ActionEvent.h"
unsigned long CSGame::OnEvent(CEventBase * pEvent)
{
	if (ACTION_EVENT == pEvent->m_ulEventType)
	{
		auto Action = (CActionEvent *)pEvent;
		auto player = m_vPlayers[Action->m_nSource];
		if (userInteraction) {
			//cout << "User interaction enabled" << endl;
			float Stimulus;
			switch (Action->m_nAction) {
			case JOY_AXIS_RIGHT_PRESSED:
				player->MoveSelector(MoveEnum::RIGHT);
				cout << "Player: " << player->GetPlayerName() << " is in barn = " << player->GetCurrentBarnChoosed() << endl;
				fixingSelector();
				break;
			case JOY_AXIS_LEFT_PRESSED:
				player->MoveSelector(MoveEnum::LEFT);
				cout << "Player: " << player->GetPlayerName() << " is in barn = " << player->GetCurrentBarnChoosed() << endl;
				fixingSelector();
				break;
			case JOY_BUTTON_B_PRESSED:
				if (player->BarnIsChoosed()) break;
				player->ChooseBarn();
				// drop others that are in the same barn when we choose one barn
				OwnBarn(player);

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
					//cout << "User: " << i << " selected Barn " << userSelectedBarn[i] << endl;
				}

				for (int i = 0; i < totalPlayers; i++) {
					/*if (userSelectedBarn[i] == greatestBarnId) {
						cout << "User: " << i << " GANO" << endl;
					}
					else {
						cout << "User: " << i << " PERDIO" << endl;
					}*/
				}
				break;
			case JOY_BUTTON_Y_PRESSED:
				cout << "Button Y pressed" << endl;
				break;
			}
		} // end user interaction
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
				cout << "Button A pressed fomr joystic: " << Action->m_nSource << endl;
				m_pSndBackground->Play(true);
				cout << game << endl;
					map<int, ScenarioObject *> objects = dynamicScenario->getScenarioObjects();
					map<int, ScenarioObject *>::iterator iterator;
					int firstHen = totalPlayers;
					for (int i = firstHen; i < dynamicScenario->count(); i++)
					{
						ScenarioObject *scenarioObject = dynamicScenario->getScenarioObect(i);
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
		VECTOR4D DeepBlue = { .2, .3, 4, 0 };
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

		// Imagen de fondo
		CDXBasicPainter::VERTEX Frame[4]
		{
			{ { -1,1, 0,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 0,0,0,0 } },
			{ { 1, 1, 0,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 1,0,0,0 } },
			{ { -1,-1,0,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 0,1,0,0 } },
			{ { 1,-1, 0,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 1,1,0,0 } }
		};
		unsigned long FrameIndex[6] = { 0,1,2,2,1,3 };

		MATRIX4D p = Paint->m_Params.Projection;
		MATRIX4D v = Paint->m_Params.View;
		MATRIX4D w = Paint->m_Params.World;

		Paint->m_Params.Projection =
		Paint->m_Params.View =
		Paint->m_Params.World = Identity();

		MAIN->m_pDXManager->GetContext()->PSSetShaderResources(3, 1, &m_pSRVBackground);
		Paint->m_Params.Flags = MAPPING_EMISSIVE | LIGHTING_DIFFUSE;
		Paint->DrawIndexed(Frame, 4, FrameIndex, 6);

		DXManager->GetContext()->ClearDepthStencilView(DXManager->GetMainDSV(),
			D3D11_CLEAR_STENCIL | D3D11_CLEAR_DEPTH, 1.0F, 0.0);

//#if 0
		MATRIX4D ST = Translation(0.5, -0.5, 0) * //Centro del caracter
			Scaling(0.05, 0.1, 1) * // Tamanio del caracter
									/*RotationZ(3.141592 / 4) * */ // Orientacion del text
			Translation(-1, 1, 0); // Posicion del text
		VECTOR4D blanco = { 1, 1, 1, 1 };
		MAIN->m_pTextRenderer->RenderText(ST, "Francisco Mendez", blanco);
//#endif

		//Limpiando text blender
		MAIN->m_pDXManager->GetContext()->OMSetBlendState(nullptr, nullptr, -1);
		m_pCamera->setView(p, v, w);
		Paint->m_Params.Flags = LIGHTING_DIFFUSE;
		
		// Moving elements in scenario
		if (game) {
			int firstHen = totalPlayers;
			for (int i = firstHen; i < dynamicScenario->count(); i++)
			{
				auto hen = dynamic_cast<Hen*>(dynamicScenario->getScenarioObect(i));
				if (!hen->getPaint()) continue;

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

		// Moving the Player selector when two or more are in the same barn


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
	m_dHensInBarn += removeHensSize;

	for (int i = 0; i < incrementSize; i++) {
		staticScenario->getScenarioObect(incrementInBarns[i])->incrementScore(1);
	}

	for (int i = 0; i < removeHensSize; i++) {
		//dynamicScenario->removeScenarioObjectById(removeHens[i]);
		dynamicScenario->getScenarioObect(removeHens[i])->setPaint(false);
	}

	incrementInBarns.clear();
	removeHens.clear();

	if (!userInteraction && m_dHensInBarn == TOTAL_HENS) {
		// Stop hens to move
		game = false;

		m_pSndBackground->Stop();

		// Allow user to move
		userInteraction = true;
		MAIN->m_pInputProcessor->SetJoysticMode(CInputProcessor::JoysticMode::KEYBOARD);

		// Create user selector
		createUserSelectionMarker();
	}
}

void CSGame::createUserSelectionMarker()
{
	for (auto player : m_vPlayers) {
		player->Show();
	}
}

CSGame::CSGame()
{
	m_pCamera = nullptr;
}

CSGame::~CSGame()
{
}
