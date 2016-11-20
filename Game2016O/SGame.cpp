#include "stdafx.h"
#include "SGame.h"
#include <cstdio>
#include "SMain.h"
#include "HSM\StateMachineManager.h"
#include "BlenderImporter.h"
#include <memory>
#include <iostream>


void CSGame::OnEntry()
{
	printf("CSGame::OnEntry()\n"); fflush(stdout);

	m_pCamera = new CCamera(MAIN->m_pDXPainter);
	m_pCamera->ChangeView(CCamera::ViewMode::Top);
	//m_pCamera->ChangeView(CCamera::ViewMode::PlayerA);

	m_pTable = MAIN->GetMeshByString("table");
	m_pMallet = MAIN->GetMeshByString("mallet");
}

#include "ActionEvent.h"
unsigned long CSGame::OnEvent(CEventBase * pEvent)
{
	if (ACTION_EVENT == pEvent->m_ulEventType)
	{
		auto Action = (CActionEvent *)pEvent;
		float Stimulus;
		switch (Action->m_nAction)
		{
		case JOY_AXIS_RX:
			//Dead zone
			Stimulus = (fabs(Action->m_fAxis) < 0.2 ? 0.0f : Action->m_fAxis);
			if (Stimulus != 0.0)
			{
				//m_pCamera->MoveXAxe(Stimulus);
				if (Stimulus > 0)
					m_pCamera->ChangeView(CCamera::ViewMode::PlayerA);
				else
					m_pCamera->ChangeView(CCamera::ViewMode::PlayerB);
				//std::cout << "hola1 " << Stimulus << std::endl;
			}
			break;

		case JOY_AXIS_RY:
			//Dead zone
			Stimulus = (fabs(Action->m_fAxis) < 0.2 ? 0.0f : Action->m_fAxis);
			if (Stimulus != 0.0)
			{
				//m_pCamera->MoveZAxe(Stimulus);
				m_pCamera->ChangeView(CCamera::ViewMode::Top);
				//std::cout << "hola1 " << Stimulus << std::endl;
			}
			break;

		case JOY_AXIS_LX:
			Stimulus = (fabs(Action->m_fAxis) < 0.2 ? 0.0f : Action->m_fAxis);
			if (Stimulus != 0.0)
			{
				m_pCamera->RotateXAxe(Stimulus);
				//std::cout << "hola2 " << Stimulus << std::endl;
			}
			break;
		case JOY_AXIS_LY:
			Stimulus = (fabs(Action->m_fAxis) < 0.2 ? 0.0f : Action->m_fAxis * -1);
			if (Stimulus != 0.0)
			{
				m_pCamera->RotateYAxe(Stimulus);
				//std::cout << "hola3 " << Stimulus << std::endl;
			}
			break;
		}
	}
	if (APP_LOOP == pEvent->m_ulEventType)
	{
		auto Paint = MAIN->m_pDXPainter;
		auto DXManager = MAIN->m_pDXManager;
		Paint->SetRenderTarget(DXManager->GetMainRTV(), //Backbuffer
			DXManager->GetMainDSV()); //ZBuffer

		VECTOR4D DeepBlue = { 0.9, 0.9, 0.9, 0 };
		DXManager->GetContext()->ClearDepthStencilView(DXManager->GetMainDSV(),
					D3D11_CLEAR_STENCIL | D3D11_CLEAR_DEPTH, 1.0F, 0.0);

		DXManager->GetContext()->ClearRenderTargetView(DXManager->GetMainRTV(), (float *)&DeepBlue);

		Paint->m_Params.Lights[0].Type = LIGHT_DIRECTIONAL;
		Paint->m_Params.Lights[0].Direction = { 0, -1, 0, 0 };
		Paint->m_Params.Lights[0].Diffuse = { 1, 1, 1, 1 };
		Paint->m_Params.Flags = LIGHTING_DIFFUSE;
		Paint->m_Params.Material.Diffuse = { 1, 1, 1, 0 };
		Paint->m_Params.Material.Emissive = { 0, 0, 0, 0 };

		Paint->DrawIndexed(&m_pTable->m_Vertices[0],
			m_pTable->m_Vertices.size(),
			&m_pTable->m_Indices[0],
			m_pTable->m_Indices.size());

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

CSGame::CSGame()
{
	m_pCamera = nullptr;
}


CSGame::~CSGame()
{
}
