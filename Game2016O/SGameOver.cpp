#include "stdafx.h"
#include "SGameOver.h"
#include "SMain.h"
#include <cstdio>
#include <iostream>
#include "HSM\EventWin32.h"
#include "SCredits.h"
#include "Sound\SndManager.h"

#define SND_BACKGROUND 1

CSGameOver::CSGameOver()
{
}


CSGameOver::~CSGameOver()
{
}

void CSGameOver::OnEntry()
{
	SetTimer(MAIN->m_hWnd, 1, 5000, nullptr);

	// loading sounds
	MAIN->m_pSndManager->ClearEngine();
	auto snd = MAIN->m_pSndManager->LoadSoundFx(L"..\\Assets\\gameOver.wav", SND_BACKGROUND);

	if (snd)
		snd->Play(false);

}

unsigned long CSGameOver::OnEvent(CEventBase * pEvent)
{
	if (EVENT_WIN32 == pEvent->m_ulEventType)
	{
		CEventWin32* pWin32 = (CEventWin32*)pEvent;
		switch (pWin32->m_msg)
		{
		case WM_TIMER:
			if (1 == pWin32->m_wParam)
			{
				KillTimer(MAIN->m_hWnd, pWin32->m_wParam);
				m_pSMOwner->Transition(CLSID_CSCredits);
				InvalidateRect(MAIN->m_hWnd, nullptr, false);
				return 0;
			}
		}
	}
	if (APP_LOOP == pEvent->m_ulEventType)
	{
		auto Painter = MAIN->m_pDXPainter;
		auto Ctx = MAIN->m_pDXManager->GetContext();

		Painter->SetRenderTarget(MAIN->m_pDXManager->GetMainRTV(), //Backbuffer
			MAIN->m_pDXManager->GetMainDSV()); //ZBuffer

		Ctx->ClearDepthStencilView(MAIN->m_pDXManager->GetMainDSV(),
			D3D11_CLEAR_DEPTH, 1.0f, 0);
		Painter->SetRenderTarget(MAIN->m_pDXManager->GetMainRTV(), NULL);
		Painter->m_Params.View =
			Painter->m_Params.World =
			Painter->m_Params.Projection = Identity();
		VECTOR4D Black = { 0, 0, 0, 0 };
		MAIN->m_pDXManager->GetContext()->ClearRenderTargetView(MAIN->m_pDXManager->GetMainRTV(), (float *)&Black);


		MATRIX4D ST = Translation(0.5, -0.5, 0) * //Centro del caracter
			Scaling(0.1, 0.2, 1) * // Tamanio del caracter
									/*RotationZ(3.141592 / 4) * */ // Orientacion del text
			Translation(-0.7, 0.2, 0); // Posicion del texto
		VECTOR4D blanco = { 1, 1, 1, 1 };
		MAIN->m_pTextRenderer->RenderText(ST, "Game Over", blanco);
		MAIN->m_pDXManager->GetSwapChain()->Present(1, 0);
	}
	return __super::OnEvent(pEvent);
}

void CSGameOver::OnExit()
{
}
