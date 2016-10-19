#include "stdafx.h"
#include "SMainMenu.h"
#include "HSM\EventWin32.h"
#include "HSM\StateMachineManager.h"
#include "Graphics\ImageBMP.h"
#include "SGame.h"
#include "SMain.h"


CSMainMenu::CSMainMenu()
{
}


CSMainMenu::~CSMainMenu()
{
}

void CSMainMenu::OnEntry()
{
	//things

	MAIN->m_pSndManager->ClearEngine();
	MAIN->m_pSndManager->LoadSoundFx(L"\\Assets\\Explosion.wav", 1);
}

unsigned long CSMainMenu::OnEvent(CEventBase *pEvent)
{
	if (INPUT_EVENT == pEvent->m_ulEventType)
	{
		CInputEvent *pInput = (CInputEvent*)pEvent;
		if (pInput->m_js2.rgbButtons[0] != 0)
		{
			MAIN->m_pSndManager->PlayFx(1);
		}
	}

	if (APP_LOOP == pEvent->m_ulEventType)
	{

	}

	return __super::OnEvent(pEvent);
}

void CSMainMenu::OnExit()
{

}
