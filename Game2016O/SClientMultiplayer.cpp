#include "stdafx.h"
#include "SClientMultiplayer.h"
#include "SMain.h"
#include "SMainMenu.h"

CSClientMultiplayer::CSClientMultiplayer()
{
}


CSClientMultiplayer::~CSClientMultiplayer()
{
}

void CSClientMultiplayer::OnEntry(void)
{

}
unsigned long CSClientMultiplayer::OnEvent(CEventBase* pEvent)
{
	if (APP_LOOP)
	{
		__super::OnEvent(pEvent);
		MAIN->m_pDXManager->GetSwapChain()->Present(1, 0);
		if (!MAIN->m_pNetProcessor->ShowConnectDialog(MAIN->m_hWnd, MAIN->m_hInstance))
		{
			m_pSMOwner->Transition(CLSID_CSMainMenu);
			return 0;
		}
		return 0;
	}
	else
		return __super::OnEvent(pEvent);
}
void CSClientMultiplayer::OnExit(void)
{

}
