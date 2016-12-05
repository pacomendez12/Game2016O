#include "stdafx.h"
#include "SCredits.h"
#include "SMain.h"
#include <cstdio>
#include <iostream>
#include "SIntroduction.h"
#include "HSM\EventWin32.h"

#define MOVE_TIMER 1

CSCredits::CSCredits()
{
}


CSCredits::~CSCredits()
{
}

void CSCredits::OnEntry()
{
	printf("CSCredits::OnEntry()\n"); fflush(stdout);

	// loading sounds
	MAIN->m_pSndManager->ClearEngine();
	m_pSndBackground = MAIN->m_pSndManager->LoadSoundFx(L"..\\Assets\\credits.wav", SND_BACKGROUND);

	if (m_pSndBackground)
		m_pSndBackground->Play(true);
	m_fOffset = -1;

	//Start timers
	//SetTimer(MAIN->m_hWnd, MOVE_TIMER, 1, nullptr);
}

unsigned long CSCredits::OnEvent(CEventBase * pEvent)
{
	if (EVENT_WIN32 == pEvent->m_ulEventType)
	{
		CEventWin32* pWin32 = (CEventWin32*)pEvent;
		switch (pWin32->m_msg)
		{
		case WM_TIMER:
			if (MOVE_TIMER == pWin32->m_wParam)
			{
				//KillTimer(MAIN->m_hWnd, pWin32->m_wParam);
				//m_pSMOwner->Transition(CLSID_CSCredits);
				//InvalidateRect(MAIN->m_hWnd, nullptr, false);
				//m_fOffset += 0.005;
				return 0;
			}
		}
	}
	if (APP_LOOP == pEvent->m_ulEventType)
	{
		if (m_fOffset >= 3.17)
		{
			m_pSMOwner->Transition(CLSID_CSIntroduction);
			InvalidateRect(MAIN->m_hWnd, nullptr, false);
		}
		m_fOffset += 0.025;
		auto Painter = MAIN->m_pDXPainter;
		auto Ctx = MAIN->m_pDXManager->GetContext();

		/*Painter->SetRenderTarget(MAIN->m_pDXManager->GetMainRTV(), //Backbuffer
			MAIN->m_pDXManager->GetMainDSV()); //ZBuffer*/

		Ctx->ClearDepthStencilView(MAIN->m_pDXManager->GetMainDSV(),
			D3D11_CLEAR_DEPTH, 1.0f, 0);
		Painter->SetRenderTarget(MAIN->m_pDXManager->GetMainRTV(), NULL);

		VECTOR4D Black = { 0, 0, 0, 0 };
		MAIN->m_pDXManager->GetContext()->ClearRenderTargetView(MAIN->m_pDXManager->GetMainRTV(), (float *)&Black);
		VECTOR4D blanco = { 1, 1, 1, 1 };

		MATRIX4D ST = Translation(0.5, -0.5, 0) * Scaling(0.025, 0.05, 1) * Translation(-1, m_fOffset + 0.0, 0); 
		MAIN->m_pTextRenderer->RenderText(ST, "          ########### CREDITOS ###########", blanco);
		ST = Translation(0.5, -0.5, 0) * Scaling(0.025, 0.05, 1) * Translation(-1, m_fOffset - 0.15, 0);
		MAIN->m_pTextRenderer->RenderText(ST, "Gallinas Correlonas", blanco);
		ST = Translation(0.5, -0.5, 0) * Scaling(0.025, 0.05, 1) * Translation(-1, m_fOffset - 0.3, 0);
		MAIN->m_pTextRenderer->RenderText(ST, "Desarrollo ........................", blanco);
		ST = Translation(0.5, -0.5, 0) * Scaling(0.025, 0.05, 1) * Translation(-1, m_fOffset - 0.45, 0);
		MAIN->m_pTextRenderer->RenderText(ST, "     (Quien hace los creditos se escribe primero)", blanco);
		ST = Translation(0.5, -0.5, 0) * Scaling(0.025, 0.05, 1) * Translation(-1, m_fOffset - 0.7, 0);
		MAIN->m_pTextRenderer->RenderText(ST, "            - Francisco Mendez Pelayo", blanco);
		ST = Translation(0.5, -0.5, 0) * Scaling(0.025, 0.05, 1) * Translation(-1, m_fOffset - .85, 0);
		MAIN->m_pTextRenderer->RenderText(ST, "            - Victor Cruz Hernandez", blanco);

		ST = Translation(0.5, -0.5, 0) * Scaling(0.025, 0.05, 1) * Translation(-1, m_fOffset - 1.1, 0);
		MAIN->m_pTextRenderer->RenderText(ST, " Objetos 3D .......................", blanco);

		ST = Translation(0.5, -0.5, 0) * Scaling(0.02, 0.04, 1) * Translation(-1, m_fOffset - 1.25, 0);
		MAIN->m_pTextRenderer->RenderText(ST, "     - Gallinas: http://www.blendswap.com/blends/view/76321", blanco);
		ST = Translation(0.5, -0.5, 0) * Scaling(0.02, 0.04, 1) * Translation(-1, m_fOffset - 1.35, 0);
		MAIN->m_pTextRenderer->RenderText(ST, "     - Casas: Francisco Mendez Pelayo", blanco);

		ST = Translation(0.5, -0.5, 0) * Scaling(0.025, 0.05, 1) * Translation(-1, m_fOffset - 1.50, 0);
		MAIN->m_pTextRenderer->RenderText(ST, " Sonidos ..........................", blanco);
		ST = Translation(0.5, -0.5, 0) * Scaling(0.02, 0.04, 1) * Translation(-1, m_fOffset - 1.60, 0);
		MAIN->m_pTextRenderer->RenderText(ST, "     - Intro: https://www.jamendo.com/track/1187038/country", blanco);
		ST = Translation(0.5, -0.5, 0) * Scaling(0.02, 0.04, 1) * Translation(-1, m_fOffset - 1.70, 0);
		MAIN->m_pTextRenderer->RenderText(ST, "     - Creditos: https://www.youtube.com/watch?v=msSc7Mv0QHY", blanco);

		ST = Translation(0.5, -0.5, 0) * Scaling(0.025, 0.05, 1) * Translation(-1, m_fOffset - 1.85, 0);
		MAIN->m_pTextRenderer->RenderText(ST, " Imagenes .........................", blanco);
		ST = Translation(0.5, -0.5, 0) * Scaling(0.015, 0.03, 1) * Translation(-1, m_fOffset - 1.95, 0);
		MAIN->m_pTextRenderer->RenderText(ST, "     - fondo: chupamobile.com/ui-graphic-assets/6-vector-game-backgrounds-8003", blanco);

		MAIN->m_pDXManager->GetSwapChain()->Present(1, 0);
		//return 0;
	}
	return __super::OnEvent(pEvent);
}

void CSCredits::OnExit()
{
}
