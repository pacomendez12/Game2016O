#include "stdafx.h"
#include "SGame.h"
#include <cstdio>
#include "SMain.h"
#include "HSM\StateMachineManager.h"
#include "BlenderImporter.h"
#include <memory>
#include <string>
#include <iostream>
#include "Graphics\ImageBMP.h"
#include "ActionEvent.h"
#include "HSM\EventWin32.h"
#include "ComputerPlayer.h"
#include "SGameOver.h"
#include <atlstr.h>

vector<bool> CPlayer::m_vBoardBarnsChoosed;
CRITICAL_SECTION CPlayer::m_csLock;

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
	bool ret = true;
	EnterCriticalSection(&m_csLock);
	if (m_bBarnChoosed == true || m_vBoardBarnsChoosed[barn] == true)
	{
		ret = false;
	}
	else 
	{
		m_dCurrentBarn = barn;
		m_pSelecter->moveTo(m_pOwner->GetBarnPositions()[m_dCurrentBarn]);
		m_pSelecter->setZ(Z_MARKER_POSITION);
		m_pOwner->fixingSelector();
	}
	LeaveCriticalSection(&m_csLock);
	return ret;
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

	// if is not available move to next
	if (!MoveSelector(current))
	{
		m_dCurrentBarn = current;
		MoveSelector(move);
	}
}

bool CPlayer::ChooseBarn()
{
	bool ret = false;
	EnterCriticalSection(&m_csLock);
	if (!m_vBoardBarnsChoosed[GetCurrentBarnChoosed()])
	{
		m_bBarnChoosed = true;
		m_vBoardBarnsChoosed[GetCurrentBarnChoosed()] = true;
		ret = true;
		m_pOwner->OwnBarn(this);
	}
	LeaveCriticalSection(&m_csLock);
	return ret;
}

/* ***************************** SGame code *********************************************************/

void CSGame::createScenarioElements(int totalSpheres)
{
	barnTotals.resize(TOTAL_BARNS);
	int barnId = 0;
	int max = 0;
	int index = 0;
	int x = 0;
	int y = 0;
	int steptsToTarget;
	int newScenarioObjectId = 0;

	// Setting colors and positions
	{
		//"casa", "hen", "esfera"
		barnMesh = MAIN->GetMeshByString("casa");
		henMesh = MAIN->GetMeshByString("hen");
		sphereMesh = MAIN->GetMeshByString("esfera");

		markerColors.push_back({ 1, 1, 1,0 }); // White
		markerColors.push_back({ 1,0, 1,0 }); // Morado
		markerColors.push_back({ 0,0,0,0 }); // Black

		barnColors.push_back({ 1, 1,0,0 }); // Yellow
		barnColors.push_back({ 1,0,0,0 }); // Red
		barnColors.push_back({ 0, 1, 0, 0 }); // Green
		barnColors.push_back({ 0, 0, 1, 0 }); //Blue

		barnScenarioPositions[0] = new  ScenarioPosition(1.5, -7, 0);
		barnScenarioPositions[1] = new  ScenarioPosition(0, -2, 0);
		barnScenarioPositions[2] = new  ScenarioPosition(0, 2.25, 0);
		barnScenarioPositions[3] = new  ScenarioPosition(1.5, 7, 0);
	}

	// Creating barns
	{
		for (int i = 0; i < 4; i++)
		{
			newScenarioObjectId = staticScenario->getNewScenarioObjectId();
			staticScenario->addElementToScenario(newScenarioObjectId,
				new Barn(newScenarioObjectId,
					1,
					new CMesh(*barnMesh),
					barnScenarioPositions[newScenarioObjectId],
					true,
					barnColors[i]));
		}
	}

	// Creating Player selectors
	{
		for (int i = 0; i < totalPlayers; i++)
		{ // i and id are the same
			int id = dynamicScenario->getNewScenarioObjectId();
			ScenarioObject *so = new ScenarioObject(i, 0.2, new CMesh(*sphereMesh),
				barnScenarioPositions[i], true, markerColors[i]);
			so->setZ(Z_MARKER_POSITION);
			dynamicScenario->addElementToScenario(id, so);
		}
	}

	// Creating hens
	{
		srand(time(NULL));

		// Painting hen mesh
		for (int i = 0; i < henMesh->m_Vertices.size(); i++)
		{
			henMesh->m_Vertices[i].Color = { 1,1,1,0 };
		}

		// Getting total hens to a barn
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

		//para testear los graneros
		//barnTotals = { 20, 15, 10, 5 };

		// Adding hens to scenario
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < barnTotals[i]; j++) {
				x = rand() % 50 + 15;
				y = rand() % 30 - 20;
				steptsToTarget = rand() % 500 + 200;
				//cout << barnId << " (" << x << "," << y << ")" << endl;
				newScenarioObjectId = dynamicScenario->getNewScenarioObjectId();
				dynamicScenario->addElementToScenario(
					newScenarioObjectId,
					new Hen(newScenarioObjectId, 0.2, henMesh,
						new ScenarioPosition(x, y, 0), false, barnScenarioPositions[i],
						steptsToTarget, i, { 1,1,1,0 }));
			}
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
		if (!p->BarnIsChoosed())
			so->setZ(Z_MARKER_POSITION + (offset * playersPerBarn[p->GetCurrentBarnChoosed()]));
		playersPerBarn[p->GetCurrentBarnChoosed()]++;
	}
}


// se apropia de un granero y hace que si algun otro player
// tenia su selector sobre el granero seleccionado, sus 
// selectores se muevan al siguiente granero a la derecha
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
	auto so = player->GetScenarioObject();
	so->setZ(Z_MARKER_POSITION - 0.7);
}

void CSGame::StartIaPlayers()
{
	for (auto cPlayer : m_vComputerPlayers)
	{
		cPlayer->StartPlayer();
	}
}

void CSGame::StopIaPlayers()
{
	for (auto cPlayer : m_vComputerPlayers)
	{
		cPlayer->StopPlayer();
	}
}

void CSGame::OnEntry()
{
	printf("CSGame::OnEntry()\n"); fflush(stdout);
	m_dCurrentCounting = 4;

	m_vPlayers.clear();
	m_vComputerPlayers.clear();
	m_vHenFxs.clear();
	incrementInBarns.clear();
	removeHens.clear();
	barnTotals.clear();
	markerColors.clear();
	barnColors.clear();
	barnScenarioPositions.clear();

	staticScenario = new Scenario();
	dynamicScenario = new Scenario();

	m_pCamera = nullptr;

	m_pCamera = new CCamera(MAIN->m_pDXPainter);
	m_pCamera->ChangeView(CCamera::ViewMode::Default);

	CImageBMP* background =
		CImageBMP::CreateBitmapFromFile("..\\Assets\\background.bmp", NULL);
	ID3D11Texture2D* backgroundTexture = background->CreateTexture(MAIN->m_pDXManager);
	CImageBMP::DestroyBitmap(background);
	MAIN->m_pDXManager->GetDevice()->CreateShaderResourceView(backgroundTexture, NULL, &m_pSRVBackground);

	CImageBMP* winner =	CImageBMP::CreateBitmapFromFile("..\\Assets\\winner.bmp", nullptr);
	auto textureWiner = winner->CreateTexture(MAIN->m_pDXManager);
	MAIN->m_pDXManager->GetDevice()->CreateShaderResourceView(textureWiner, NULL, &m_pWinnerResourceView);
	CImageBMP::DestroyBitmap(winner);
	
	CImageBMP* loser = CImageBMP::CreateBitmapFromFile("..\\Assets\\loser.bmp", nullptr);
	auto textureLoser = loser->CreateTexture(MAIN->m_pDXManager);
	MAIN->m_pDXManager->GetDevice()->CreateShaderResourceView(textureLoser, NULL, &m_pLoserResourceView);
	CImageBMP::DestroyBitmap(loser);

	game = false;
	userInteraction = false;
	selectionDone = false;
	hensOutPainted = false;
	showWinner = false;
	m_bShowCounter = false;
	greatestHensInBarn = 0;

	totalPlayers = MAIN->m_dPlayersNumber;
	//totalPlayers = TOTAL_PLAYERS;
	totalBarns = TOTAL_BARNS;
	m_dHensInBarn = 0;

	createScenarioElements(TOTAL_HENS);

	//initialize critical section
	InitializeCriticalSection(&CPlayer::m_csLock);

	CPlayer::InitializeBoard(TOTAL_BARNS);
	int realPlayers = MAIN->GetRealPlayersNumber();
	for (int i = 0; i < totalPlayers; i++)
	{
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

		//adding ia players
		if (!player->IsHuman())
		{
			auto cPlayer = new CComputerPlayer(player, CComputerPlayer::IaLevel::NORMAL);
			m_vComputerPlayers.push_back(cPlayer);
		}
	}
	fixingSelector();


	// loading sounds
	m_vHenFxs.clear();
	MAIN->m_pSndManager->ClearEngine();
	m_pSndBackground = MAIN->m_pSndManager->LoadSoundFx(L"..\\Assets\\Banjo.wav", SND_BACKGROUND);


	auto snd = MAIN->m_pSndManager->LoadSoundFx(L"..\\Assets\\hen1.wav", SND_HEN1);
	if (snd)
	{
		m_vHenFxs.push_back(snd);
	}
	else
	{
		std::cout << "failed when opening ..\\Assets\\hen1.wav file" << std::endl;
	}

	snd = MAIN->m_pSndManager->LoadSoundFx(L"..\\Assets\\hen2.wav", SND_HEN2);
	if (snd)
	{
		m_vHenFxs.push_back(snd);
	}
	else
	{
		std::cout << "failed when opening ..\\Assets\\hen2.wav file" << std::endl;
	}

	snd = MAIN->m_pSndManager->LoadSoundFx(L"..\\Assets\\hen3.wav", SND_HEN3);
	if (snd)
	{
		m_vHenFxs.push_back(snd);
	}
	else
	{
		std::cout << "failed when opening ..\\Assets\\hen3.wav file" << std::endl;
	}

	snd = MAIN->m_pSndManager->LoadSoundFx(L"..\\Assets\\start.wav", SND_COUNTER);
	snd = MAIN->m_pSndManager->LoadSoundFx(L"..\\Assets\\win.wav", SND_WIN);
	snd = MAIN->m_pSndManager->LoadSoundFx(L"..\\Assets\\loser.wav", SND_LOSER);
		
	//Start timers
	SetTimer(MAIN->m_hWnd, TIMER_START, 1000, nullptr);
}

void CSGame::OnExit()
{
	SAFE_DELETE(m_pCamera);
	m_pSndBackground->Stop();
	for (auto snd : m_vHenFxs)
	{
		snd->Stop();
	}

	for (int i = 0; i < 3; i++)
	{
		KillTimer(MAIN->m_hWnd, i + 1);
	}
	SAFE_DELETE(staticScenario);
	SAFE_DELETE(dynamicScenario);
	DeleteCriticalSection(&CPlayer::m_csLock);

	// stopping ia threads
	StopIaPlayers();
	for (auto p : m_vPlayers)
	{
		delete p;
	}

	for (auto p : m_vComputerPlayers)
	{
		delete p;
	}
	printf("CSGame::OnExit()\n"); fflush(stdout);
}

unsigned long CSGame::OnEvent(CEventBase * pEvent)
{
	if (EVENT_WIN32 == pEvent->m_ulEventType)
	{
		CEventWin32* pWin32 = (CEventWin32*)pEvent;
		switch (pWin32->m_msg)
		{
		case WM_TIMER:
			if (SND_HEN1 == pWin32->m_wParam || SND_HEN2 == pWin32->m_wParam ||
				SND_HEN3 == pWin32->m_wParam)
			{
				KillTimer(MAIN->m_hWnd, pWin32->m_wParam);
				MAIN->m_pSndManager->PlayFx(pWin32->m_wParam, 1, 0, 1);
				std::cout << "Timer Hen " << pWin32->m_wParam << std::endl;
			}
			else if (TIMER_START == pWin32->m_wParam)
			{
				StartCounting();
			}
			else if (TIMER_START_STEEP == pWin32->m_wParam)
			{
				m_bShowCounter = true;
				m_dCurrentCounting--;
				MAIN->m_pSndManager->PlayFx(SND_COUNTER, 1, 0, 1);
			}
			else if (TIMER_NEXT_STATE == pWin32->m_wParam)
			{
				KillTimer(MAIN->m_hWnd, TIMER_NEXT_STATE);
				m_pSMOwner->Transition(CLSID_CSGameOver);
				InvalidateRect(MAIN->m_hWnd, nullptr, false);
				return 0;
			}
			break;
		case WM_CHAR:
			switch (pWin32->m_wParam)
			{
			case 'r':
				m_pSMOwner->Transition(CLSID_CSGame);
				InvalidateRect(MAIN->m_hWnd, nullptr, false);
				return 0;
			}
		}
	}
	if (ACTION_EVENT == pEvent->m_ulEventType)
	{
		auto Action = (CActionEvent *)pEvent;
		auto player = m_vPlayers[Action->m_nSource];
		if (userInteraction) {
			float Stimulus;
			switch (Action->m_nAction) {
			case JOY_AXIS_RIGHT_PRESSED:
				player->MoveSelector(MoveEnum::RIGHT);
				cout << "Player: " << player->GetPlayerName() << " is in barn = " << player->GetCurrentBarnChoosed() << endl;
				break;
			case JOY_AXIS_LEFT_PRESSED:
				player->MoveSelector(MoveEnum::LEFT);
				cout << "Player: " << player->GetPlayerName() << " is in barn = " << player->GetCurrentBarnChoosed() << endl;
				break;
			case JOY_BUTTON_A_PRESSED:
				if (player->BarnIsChoosed()) break;
				player->ChooseBarn();


				cout << "Button A pressed" << endl;
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
		/*else {
			float Stimulus;
			switch (Action->m_nAction)
			{

			case JOY_BUTTON_A_PRESSED: {
				cout << "Button A pressed fomr joystic: " << Action->m_nSource << endl;
				StartGame();
			}
				break;
			}
		}*/
	}

	if (APP_LOOP == pEvent->m_ulEventType)
	{
		auto Paint = MAIN->m_pDXPainter;
		auto DXManager = MAIN->m_pDXManager;

		MAIN->m_pDXManager->GetContext()->OMSetBlendState(nullptr, nullptr, -1);
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

		MATRIX4D p = Paint->m_Params.Projection; //* AC;
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

		// All game states
		{
			// Game when hens are moving
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
				manageHensMovement();
			}

			// Game when user interacts
			if (userInteraction) {
				// Check that the selection has been done by all users
				if (!selectionDone) {
					verifyUserSelectionDone();
				}
				if (hensOutPainted) {
					moveHensBackwards();
				}
			}

			// Game when the winner need to be shown
			if (showWinner) {
				cout << "Ya deberia renderear " << endl;
				// Show the total of hens in each barn with text
				// with blender text
				drawHensInBarn();

				//Limpiando text blender
				MAIN->m_pDXManager->GetContext()->OMSetBlendState(nullptr, nullptr, -1);
			}
			
		}

		m_pCamera->setView(p, v, w);
		Paint->m_Params.Flags = LIGHTING_DIFFUSE;

		// Painting elements in the scenario
		staticScenario->paintScenarioObjects(Paint);
		dynamicScenario->paintScenarioObjects(Paint);

		// show counter
		if (m_bShowCounter || showWinner)
		{
			Paint->SetRenderTarget(DXManager->GetMainRTV(), //Backbuffer
				DXManager->GetMainDSV()); //ZBuffer

										  // Fondo
			VECTOR4D DeepBlue = { .2, .3, 4, 0 };
			DXManager->GetContext()->ClearDepthStencilView(DXManager->GetMainDSV(),
				D3D11_CLEAR_STENCIL | D3D11_CLEAR_DEPTH, 1.0F, 0.0);
			MATRIX4D p = Paint->m_Params.Projection; //* AC;
			MATRIX4D v = Paint->m_Params.View;
			MATRIX4D w = Paint->m_Params.World;

			Paint->m_Params.Projection =
				Paint->m_Params.View =
				Paint->m_Params.World = Identity();

			if (m_bShowCounter)
				ShowCounting();
			if (showWinner)
				ShowWinner();

			MAIN->m_pDXManager->GetContext()->OMSetBlendState(nullptr, nullptr, -1);
			m_pCamera->setView(p, v, w);
			Paint->m_Params.Flags = LIGHTING_DIFFUSE;
		}

		DXManager->GetSwapChain()->Present(1, 0);
	}
	return __super::OnEvent(pEvent);
}

void CSGame::manageHensMovement(){
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
		StartIaPlayers();
	}
}

void CSGame::createUserSelectionMarker()
{
	for (auto player : m_vPlayers) {
		player->Show();
	}
}

void CSGame::repaintHens()
{
	hensOutPainted = true;

	// Repaint all the hens
	int firstHen = totalPlayers;
	for (int i = firstHen; i < dynamicScenario->count(); i++)
	{
		auto hen = dynamic_cast<Hen*>(dynamicScenario->getScenarioObect(i));
		hen->setPaint(true);
		// increment velocity to make it funy
		hen->incrementX *= 2.5;
		hen->incrementY *= 2.5;

		// change mesh direction

		// move backwards
		hen->moveBackwards();
	}
}

void CSGame::verifyUserSelectionDone()
{
	bool allSelected = true;
	for (int i = 0; i < totalPlayers; i++) {
		allSelected &= m_vPlayers[i]->BarnIsChoosed();
	}
	if (allSelected) {
		cout << "Se han terminado de seleccionar los graneros" << endl;
		selectionDone = true;
		StopIaPlayers();
		//Once all selection is done set all the hens to draw again
		repaintHens();

		//start music again
		m_pSndBackground->SetSpeed(1.09f);
		m_pSndBackground->SetPlayPosition(0);
		m_pSndBackground->Play(true);
	}
}

void CSGame::moveHensBackwards()
{
	int totalHensOrigin = 0;
	int firstHen = totalPlayers;
	for (int i = firstHen; i < dynamicScenario->count(); i++)
	{
		auto hen = dynamic_cast<Hen*>(dynamicScenario->getScenarioObect(i));
		hen->moveBackwards();
		if (hen->alreadyInOrigin()) totalHensOrigin++;
	}

	if (totalHensOrigin == TOTAL_HENS) {
		//active check winner
		showWinner = true;
		hensOutPainted = false;
		userInteraction = false;
		m_pSndBackground->Stop();


		// snd if some won
		int winner = 0;
		int max = -1;
		for (int i = 0; i < barnTotals.size(); i++)
		{
			if (barnTotals[i] > max)
			{
				max = barnTotals[i];
				winner = i;
			}
		}


		if (CPlayer::m_vBoardBarnsChoosed[winner] == true)
			MAIN->m_pSndManager->PlayFx(SND_WIN, 1, 0, 1);
		else
			MAIN->m_pSndManager->PlayFx(SND_LOSER, 1, 0, 1);

		//set timer to go to next state
		SetTimer(MAIN->m_hWnd, TIMER_NEXT_STATE, 6000, nullptr);
	}
}

void CSGame::drawHensInBarn()
{
	VECTOR4D color = { 1, 1, 1, 1 };

	/* TODO: check if the barns was inverted*/
	Barn *barn0 = dynamic_cast<Barn*>(staticScenario->getScenarioObect(3));
	Barn *barn1 = dynamic_cast<Barn*>(staticScenario->getScenarioObect(2));
	Barn *barn2 = dynamic_cast<Barn*>(staticScenario->getScenarioObect(1));
	Barn *barn3 = dynamic_cast<Barn*>(staticScenario->getScenarioObect(0));

	// Translation Matrix

	MATRIX4D matrix0 = Translation(0.5, -0.5, 0) * //Character centre
			Scaling(0.05, 0.1, 1) * // Character size
			Translation(-0.55, 0.13, 0); // Text Position/
	MATRIX4D matrix1 = Translation(0.5, -0.5, 0) * Scaling(0.05, 0.1, 1) * Translation(-0.18, 0.175, 0);
	MATRIX4D matrix2 = Translation(0.5, -0.5, 0) * Scaling(0.05, 0.1, 1) * Translation(0.12, 0.175, 0);
	MATRIX4D matrix3 = Translation(0.5, -0.5, 0) * Scaling(0.05, 0.1, 1) * Translation(0.515, 0.13, 0);

	// Rendering text
	MAIN->m_pTextRenderer->RenderText(matrix0, to_string(barn0->getHensInHouse()).c_str(), color);
	MAIN->m_pTextRenderer->RenderText(matrix1, to_string(barn1->getHensInHouse()).c_str(), color);
	MAIN->m_pTextRenderer->RenderText(matrix2, to_string(barn2->getHensInHouse()).c_str(), color);
	MAIN->m_pTextRenderer->RenderText(matrix3, to_string(barn3->getHensInHouse()).c_str(), color);

}

void CSGame::StartGame()
{
	m_pSndBackground->Play(true);
	for (int i = 0; i < 3; i++)
	{
		int timeTimer = rand() % 3000 + (1500 * (i + 1));
		SetTimer(MAIN->m_hWnd, i + 1, timeTimer, nullptr);
	}
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

void CSGame::StartCounting()
{
	KillTimer(MAIN->m_hWnd, TIMER_START);
	SetTimer(MAIN->m_hWnd, TIMER_START_STEEP, 1000, nullptr);
}

void CSGame::ShowCounting()
{
	if (m_dCurrentCounting)
	{
		VECTOR4D color = { 1, 1, 1, 1 };

		MATRIX4D matrix = Translation(0.5, -0.5, 0) * //Character centre
			Scaling(0.25, 0.5, 1) * // Character size
			Translation(-0.14, 0.13, 0); // Text Position/
		// Rendering text
		MAIN->m_pTextRenderer->RenderText(matrix, to_string(m_dCurrentCounting).c_str(), color);
	}
	else
	{
		m_bShowCounter = false;
		KillTimer(MAIN->m_hWnd, TIMER_START_STEEP);
		StartGame();
	}
	
}

void CSGame::ShowWinner()
{

	auto Paint = MAIN->m_pDXPainter;
	auto DXManager = MAIN->m_pDXManager;
	auto Ctx = MAIN->m_pDXManager->GetContext();

	int winner = 0;
	int max = -1;
	for (int i = 0; i < barnTotals.size(); i++)
	{
		if (barnTotals[i] > max)
		{
			max = barnTotals[i];
			winner = i;
		}
	}

	//winner = barnTotals.size() - 1 - winner;

	float large = 0.5;
	float startX = -0.725;
	float y = 0.8;
	float x1, x2, x3, x4;

	int oWinner = CPlayer::m_vBoardBarnsChoosed.size() - 1 - winner;

	bool isThereWinner = CPlayer::m_vBoardBarnsChoosed[winner] == true;

	if (isThereWinner)
	{
		int oWinner = CPlayer::m_vBoardBarnsChoosed.size() - 1 - winner;
		x1 = x3 = startX + (oWinner * 0.37);
		x2 = x4 = x1 + (large / 2 + (large / 4));
	}
	else
	{
		x1 = x3 = -0.18;
		x2 = x4 = x1 + (large / 2 + (large / 4));
	}
	

	Paint->m_Params.Flags = MAPPING_EMISSIVE;
	ID3D11ShaderResourceView *pSRV;
	CDXBasicPainter::VERTEX frame[4]
	{
		{ { x1, y,0,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 0,0,0,0 } },
		{ { x2, y,0,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 1,0,0,0 } },
		{ { x3, y - large,0,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 0,1,0,0 } },
		{ { x4, y - large,0,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 1,1,0,0 } }
	};
	unsigned long indices[6]{ 0, 1, 2, 2, 1, 3 };

	if (isThereWinner)
		Ctx->PSSetShaderResources(3, 1, &m_pWinnerResourceView);
	else 
		Ctx->PSSetShaderResources(3, 1, &m_pLoserResourceView);
	Paint->DrawIndexed(frame, 4, indices, 6);
}

CSGame::CSGame()
{
	m_pCamera = nullptr;
}

CSGame::~CSGame()
{
}
