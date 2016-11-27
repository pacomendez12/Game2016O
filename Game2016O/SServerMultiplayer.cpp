#include "stdafx.h"
#include "SServerMultiplayer.h"
#include "SMainMenu.h"
#include "SMain.h"
CSServerMultiplayer::CSServerMultiplayer()
{
	m_ulKeyGen = 0;
	m_pGame = 0;
}
CSServerMultiplayer::~CSServerMultiplayer()
{
}
void CSServerMultiplayer::OnEntry(void)
{
	MAIN->m_pNetProcessor->StartListening();
	m_pGame=(CSGame*)m_pSMOwner->QueryObjectByID(CLSID_CSGame);
}
unsigned long CSServerMultiplayer::OnEvent(CEventBase* pEvent)
{
	if (APP_LOOP == pEvent->m_ulEventType)
	{
		MATRIX4D ST =
			Translation(0.5, -0.5, 0)* //Centro de caracter!!!!
			Scaling(0.05, 0.1, 1)* //Tamaño del caracter!!!
			Translation(-1, 1, 0); //Posición del texto;
		VECTOR4D TexColor = { 1,1,1,1 };
		MAIN->m_pTextRender->RenderText(ST, "Client list", TexColor);
		int i = 0;
		for (auto RP : m_pGame->m_mapKey2Socket)
		{
			ST= Translation(0.5, -0.5, 0)* //Centro de caracter!!!!
				Scaling(0.05, 0.1, 1)* //Tamaño del caracter!!!
				Translation(-0.8, 1-0.11*i, 0); //Posición del texto;
			char sz[256];
			char ip[32];
			CLIENT Client;
			VECTOR4D TexColor = { 1,1,1,1 };
			MAIN->m_pNetProcessor->GetClientBySocket(RP.second, Client);
			InetNtopA(AF_INET, &Client.Address.sin_addr,ip,32);
			wsprintfA(sz, "Player IP:%s",ip);
			MAIN->m_pTextRender->RenderText(ST, sz,TexColor);
			i++;
		}
		MAIN->m_pDXManager->GetSwapChain()->Present(1, 0);
	}
	if (EVENT_NETWORK == pEvent->m_ulEventType)
	{
		CEventNetwork* pEN = (CEventNetwork*)pEvent;
		GAMEDGRAM* pGDRAM = (GAMEDGRAM*)pEN->m_Data;
		switch (pGDRAM->m_ulCommand)
		{
		case GAMEDGRAM::REQUEST_KEY:
		{
			CEventNetwork* pNetResponse = new CEventNetwork;
			pNetResponse->m_bBradcast = 0;
			pNetResponse->m_sktSourceDestiny = pEN->m_sktSourceDestiny;
			pNetResponse->m_nDataSize = sizeof(GAMEDGRAM);
			auto &gdgram = *(GAMEDGRAM*)pNetResponse->m_Data;
			memset(&gdgram, 0, sizeof(GAMEDGRAM));
			gdgram.i = ++m_ulKeyGen;
			gdgram.m_ulCommand = GAMEDGRAM::SET_KEY;
			MAIN->m_pNetProcessor->OnEvent(pNetResponse);
		}
		break;
		case GAMEDGRAM::PLAYER_REQUEST_CONNECT:
		{
			m_pGame->m_mapKey2Socket.insert(make_pair(pGDRAM->m_ulSourceKey, pEN->m_sktSourceDestiny));
			m_pGame->m_mapSocket2Key.insert(make_pair(pEN->m_sktSourceDestiny,pGDRAM->m_ulSourceKey));
			CEventNetwork* pNetResponse = new CEventNetwork;
			pNetResponse->m_bBradcast = 0;
			auto &gdgram = *(GAMEDGRAM*)pNetResponse->m_Data;
			memset(&gdgram, 0, sizeof(GAMEDGRAM));
			gdgram.i = ++m_ulKeyGen;
			MAIN->m_pNetProcessor->OnEvent(pNetResponse);
		}
		break;
		}
	}
	return __super::OnEvent(pEvent);
}
void CSServerMultiplayer::OnExit(void)
{
	MAIN->m_pNetProcessor->StopListening();
}