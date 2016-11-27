#include "stdafx.h"
#include "SNetworkGame.h"
#include "SMain.h"
#include "Graphics\ImageBMP.h"
#include "SGame.h"
CSNetworkGame::CSNetworkGame()
{
	m_pSRVNetBackground = NULL;
}


CSNetworkGame::~CSNetworkGame()
{
}
void CSNetworkGame::OnEntry(void)
{
	MAIN->m_pNetProcessor->InitNetwork();
	auto bmp=CImageBMP::CreateBitmapFromFile("..\\Assets\\NetGame.bmp", NULL);
	if (bmp)
	{
		auto tex = bmp->CreateTexture(MAIN->m_pDXManager);
		MAIN->m_pDXManager->GetDevice()->CreateShaderResourceView(tex, 0, &m_pSRVNetBackground);
		SAFE_RELEASE(tex);
		CImageBMP::DestroyBitmap(bmp);
	}
	m_pGame = (CSGame*)m_pSMOwner->GetObjectByID(CLSID_CSGame);
}
unsigned long CSNetworkGame::OnEvent(CEventBase* pEvent)
{
	if (APP_LOOP == pEvent->m_ulEventType)
	{
		auto Painter = MAIN->m_pDXPainter;
		auto Ctx = MAIN->m_pDXManager->GetContext();
		CDXBasicPainter::VERTEX Frame[4]
		{
			{ { -1,1,1,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 0,0,0,0 } },
			{ { 1,1,1,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 1,0,0,0 } },
			{ { -1,-1,1,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 0,1,0,0 } },
			{ { 1,-1,1,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 1,1,0,0 } }
		};
		unsigned long FrameIndex[6] = { 0,1,2,2,1,3 };
		Ctx->ClearDepthStencilView(MAIN->m_pDXManager->GetMainDSV(),D3D11_CLEAR_DEPTH, 1.0f, 0);
		Painter->SetRenderTarget(MAIN->m_pDXManager->GetMainRTV(), NULL);
		Painter->m_Params.View =Painter->m_Params.World =Painter->m_Params.Projection = Identity();
		Painter->m_Params.Flags = MAPPING_EMISSIVE;
		Ctx->PSSetShaderResources(3, 1, &m_pSRVNetBackground);
		Painter->DrawIndexed(Frame, 4, FrameIndex, 6);
		return 0;
	}
	if (EVENT_NETWORK == pEvent->m_ulEventType)
	{
		CEventNetwork* pEN = (CEventNetwork*)pEvent;
		GAMEDGRAM* pGDRAM = (GAMEDGRAM*)pEN->m_Data;
		switch (pGDRAM->m_ulCommand)
		{
		case GAMEDGRAM::PLAYER_SET_STATE:
			m_pGame->m_mapRemotePlayers[pGDRAM->m_ulSourceKey] = pGDRAM->PlayerState;
			break;
		}
	}
	return __super::OnEvent(pEvent);
}
void CSNetworkGame::OnExit(void)
{
	SAFE_RELEASE(m_pSRVNetBackground);
	MAIN->m_pNetProcessor->Uninitialize();
}
