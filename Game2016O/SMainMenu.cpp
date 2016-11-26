#include <string>
#include "stdafx.h"
#include "SMainMenu.h"
#include "HSM\EventWin32.h"
#include "HSM\StateMachineManager.h"
#include "Graphics\ImageBMP.h"
#include "SMain.h"
#include "InputProcessor.h"
#include <iostream>

CSMainMenu::CSMainMenu()
{
	m_pSRVBackGround =	nullptr;
	m_pSRVMainOption1 = nullptr;
	m_pSRVMainOption2 = nullptr;
}
CSMainMenu::~CSMainMenu()
{
}

bool CSMainMenu::LoadButtons()
{
	return false;
}

void CSMainMenu::OnEntry()
{
	CSMain * mainState = (CSMain *)GetSuperState();
	m_nOption = 0;
	printf("Iniciando menú principal...\n"); fflush(stdout);
	{
		printf("Cargando recurso de fondo...\n"); fflush(stdout);
		auto img = CImageBMP::CreateBitmapFromFile("..\\Assets\\MainMenu.bmp", NULL);
		if (!img) { printf("Recurso no encontrado\n"); fflush(stdout); }
		else
		{
			auto tex = img->CreateTexture(MAIN->m_pDXManager);
			MAIN->m_pDXManager->GetDevice()->CreateShaderResourceView(
				tex, NULL, &m_pSRVBackGround);
		}
	}

	/* Generar imagenes: https://dabuttonfactory.com/#t=Salir&f=Ubuntu-Bold&ts=90&tc=fff&tshs=1&tshc=999&hp=180&vp=11&c=0&bgt=gradient&bgc=f90&ebgc=e69138&be=1&bs=1&bc=f90*/

	/* Cargando Recursos de botones */
	const char *menuLabel[NUMBER_BUTTONS] { "jugar", "unirse", "servidor", "salir" };
	const char *menuOptionsLabel[BUTTON_STATES_NUMBER] { "n", "f" }; // n = on, f = off

	m_vMenuButtons.resize(NUMBER_BUTTONS);

	/* Cargando las imagenes */
	for (int btnIdx = 0; btnIdx < NUMBER_BUTTONS; btnIdx++)
	{
		for (int stsIdx = 0; stsIdx < BUTTON_STATES_NUMBER; ++stsIdx)
		{
			std::string filePath = std::string("..\\Assets\\") + menuLabel[btnIdx] + "_" + menuOptionsLabel[stsIdx] + ".bmp";
			auto image = CImageBMP::CreateBitmapFromFile((char *) filePath.c_str(), nullptr);
			if (image) {
				std::cout << "IMG: " << filePath << std::endl;
				auto texture = image->CreateTexture(MAIN->m_pDXManager);
				MAIN->m_pDXManager->GetDevice()->CreateShaderResourceView(
					texture, NULL, &m_vMenuButtons[btnIdx].pSRV[stsIdx]);
			}
			else {
				std::cout << "No se pudo cargar la imagen " << filePath << std::endl;
				return;
			}
		}

		const float BUTTON_Y_OFFSET = 0.0f;
		const float SEPARATION = 0.26f;

		/* eligiendo las coordenadas */
		m_vMenuButtons[btnIdx].frame[0].Position = { -0.8f, 1.8f / float(NUMBER_BUTTONS) - 
			BUTTON_Y_OFFSET - (btnIdx * SEPARATION), 0, 1 };
		m_vMenuButtons[btnIdx].frame[1].Position = { -0.2f, 1.8f / float(NUMBER_BUTTONS) - 
			BUTTON_Y_OFFSET - (btnIdx * SEPARATION), 0, 1 };
		m_vMenuButtons[btnIdx].frame[2].Position = { -0.8f, 1.0f / float(NUMBER_BUTTONS) - 
			BUTTON_Y_OFFSET - (btnIdx * SEPARATION), 0, 1 };
		m_vMenuButtons[btnIdx].frame[3].Position = { -0.2f, 1.0f / float(NUMBER_BUTTONS) - 
			BUTTON_Y_OFFSET - (btnIdx * SEPARATION), 0, 1 };

		m_vMenuButtons[btnIdx].frame[0].Color = { 1, 1, 1, 1 };
		m_vMenuButtons[btnIdx].frame[1].Color = { 1, 1, 1, 1 };
		m_vMenuButtons[btnIdx].frame[2].Color = { 1, 1, 1, 1 };
		m_vMenuButtons[btnIdx].frame[3].Color = { 1, 1, 1, 1 };

		m_vMenuButtons[btnIdx].frame[0].TexCoord = { 0, 0, 0, 0 };
		m_vMenuButtons[btnIdx].frame[1].TexCoord = { 1, 0, 0, 0 };
		m_vMenuButtons[btnIdx].frame[2].TexCoord = { 0, 1, 0, 0 };
		m_vMenuButtons[btnIdx].frame[3].TexCoord = { 1, 1, 0, 0 };

		const unsigned long indices[6]{ 0, 1, 2, 2, 1, 3 };

		memcpy(m_vMenuButtons[btnIdx].indices, indices, sizeof(unsigned long) * 6);
		*(m_vMenuButtons[btnIdx].indices) = *indices;
		m_vMenuButtons[btnIdx].state = ButtonState::NORMAL;
	}

	/* Por defecto el primer boton es el seleccionado */
	m_nOption = DEFAULT_BUTTON;
	m_vMenuButtons[m_nOption].state = ButtonState::OVER;

	MAIN->m_pSndManager->ClearEngine();
	MAIN->m_pSndManager->LoadSoundFx(L"..\\Assets\\Explosion.wav", 1);
	m_fOffsetX = 0.0f;
	m_fOffsetY = 0.0f;

	// configurar inputProcessor
	MAIN->m_pInputProcessor->SetJoysticMode(CInputProcessor::JoysticMode::KEYBOARD);


}
void CSMainMenu::OnExit()
{
	SAFE_RELEASE(m_pSRVBackGround);
	

	for (int btnIdx = 0; btnIdx < NUMBER_BUTTONS; btnIdx++)
	{
		for (int stsIdx = 0; stsIdx < BUTTON_STATES_NUMBER; ++stsIdx)
		{
			SAFE_RELEASE(m_vMenuButtons[btnIdx].pSRV[stsIdx]);
		}
	}



	MAIN->m_pInputProcessor->SetJoysticMode(CInputProcessor::JoysticMode::JOYSTIC);
}

void CSMainMenu::ActivateButton(int pIdx)
{
	if (pIdx >= 0 && pIdx < NUMBER_BUTTONS)
	{
		m_vMenuButtons[m_nOption].state = ButtonState::NORMAL;
		m_nOption = pIdx;
		m_vMenuButtons[m_nOption].state = ButtonState::OVER;
	}
}

#include "ActionEvent.h"
#include "SGame.h"
unsigned long CSMainMenu::OnEvent(CEventBase* pEvent)
{
	if (ACTION_EVENT == pEvent->m_ulEventType)
	{
		CActionEvent* pInput = (CActionEvent*)pEvent;
		if (JOY_BUTTON_A_PRESSED == pInput->m_nAction)
		{
			//std::cout << "recibi = " << pInput->m_nAction << std::endl;
			switch (m_nOption)
			{
			case JUGAR:
				m_pSMOwner->Transition(CLSID_CSGame);
				return 0;
			case SALIR:
				exit(EXIT_SUCCESS);
			default:
				// por ahora
				m_pSMOwner->Transition(CLSID_CSGame);
				return 0;
			}
			
			//MAIN->m_pSndManager->PlayFx(1);
		}

		if (JOY_AXIS_UP_PRESSED == pInput->m_nAction)
		{	
			ActivateButton(m_nOption > 0 ? m_nOption - 1 : NUMBER_BUTTONS - 1);
		} 
		else if (JOY_AXIS_DOWN_PRESSED == pInput->m_nAction)
		{
			ActivateButton(m_nOption < (NUMBER_BUTTONS - 1)? m_nOption + 1 : 0);
		}

		/*if (JOY_AXIS_LX == pInput->m_nAction)
		{
			m_fOffsetX = m_fOffsetX + (pInput->m_fAxis - m_fOffsetX)*0.1;
		}
		if (JOY_AXIS_LY == pInput->m_nAction)
		{
			m_fOffsetY =m_fOffsetY + (pInput->m_fAxis - m_fOffsetY)*0.1;
		}*/
	}
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
		Ctx->ClearDepthStencilView(MAIN->m_pDXManager->GetMainDSV(),
			D3D11_CLEAR_DEPTH, 1.0f, 0);
		Painter->SetRenderTarget(MAIN->m_pDXManager->GetMainRTV(), NULL);
		Painter->m_Params.View =
			Painter->m_Params.World =
			Painter->m_Params.Projection = Identity();
		Painter->m_Params.World = Translation(m_fOffsetX, m_fOffsetY, 0);
		Painter->m_Params.Flags = MAPPING_EMISSIVE;
		Ctx->PSSetShaderResources(3, 1, &m_pSRVBackGround);
		Painter->DrawIndexed(Frame, 4, FrameIndex, 6);
		

		Painter->m_Params.World = Identity();

		/* Dibujar botones */

		for (auto option : m_vMenuButtons)
		{
			Ctx->PSSetShaderResources(3, 1, &(option.pSRV[option.state]));
			Painter->DrawIndexed(option.frame, 4, option.indices, 6);
		}

		{
			//CDXBasicPainter::VERTEX

			//Dibujar la opcion 1
			/*CDXBasicPainter::VERTEX Frame1[4]
			{
				{ { -0.5f,2.0f / 3.0f,0,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 0,0,0,0 } },
				{ { 0.5f, 2.0f / 3.0f,0,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 1,0,0,0 } },
				{ { -0.5f,1.0f / 3.0f,0,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 0,1,0,0 } },
				{ { 0.5,  1.0f / 3.0f,0,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 1,1,0,0 } }
			};
			Ctx->PSSetShaderResources(3, 1, &m_pSRVMainOption1);
			Painter->DrawIndexed(Frame1, 4, FrameIndex, 6);
			//Dibujar la opción 2
			CDXBasicPainter::VERTEX Frame2[4]
			{
				{ { -0.5f,-1.0f / 3.0f,0,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 0,0,0,0 } },
				{ { 0.5f, -1.0f / 3.0f,0,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 1,0,0,0 } },
				{ { -0.5f,-2.0f / 3.0f,0,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 0,1,0,0 } },
				{ { 0.5,  -2.0f / 3.0f,0,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,1,1,1 },{ 1,1,0,0 } }
			};
			Ctx->PSSetShaderResources(3, 1, &m_pSRVMainOption2);
			Painter->DrawIndexed(Frame2, 4, FrameIndex, 6);*/

			//TODO: resolver por que no se muestran los objetos cuando dibujo texto
		}
//#if 0
		//text
		MATRIX4D ST = Translation(0.5, -0.5, 0) * //Centro del caracter
			Scaling(0.05, 0.1, 1) * // Tamanio del caracter
			/*RotationZ(3.141592 / 4) * */ // Orientacion del text
			Translation(-1, 1, 0); // Posicion del texto
		
		MAIN->m_pTextRenderer->RenderText(ST, "Francisco Mendez");
//#endif

		MAIN->m_pDXManager->GetSwapChain()->Present(1, 0);
	}
	if (EVENT_WIN32 == pEvent->m_ulEventType)
	{
		CEventWin32* pWin32 = (CEventWin32*)pEvent;
		switch (pWin32->m_msg)
		{
		case WM_CHAR:
			break;
		}
	}
	return __super::OnEvent(pEvent);
}