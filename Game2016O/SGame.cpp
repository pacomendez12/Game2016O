#include "stdafx.h"
#include "SGame.h"
#include <cstdio>
#include "SMain.h"
#include "HSM\StateMachineManager.h"


void CSGame::OnEntry()
{
	printf("CSGame::OnEntry()\n"); fflush(stdout);
	//projection Matrix
	MAIN->m_pDXPainter->m_Params.Projection = PerspectiveWidthHeightLH(0.05, 0.05, 0.1, 100);
	VECTOR4D EyePos = { 5, 5, 5, 1 };
	VECTOR4D Target = { 0, 0, 0, 1 };
	VECTOR4D Up = { 0, 1, 0, 0 };
	MAIN->m_pDXPainter->m_Params.View = View(EyePos, Target, Up);
	MAIN->m_pDXPainter->m_Params.World = Identity();
	//creating mesh
	m_pGeometry = new CMesh;
	m_pGeometry->LoadSuzzane();
}

#include "ActionEvent.h"
unsigned long CSGame::OnEvent(CEventBase * pEvent)
{
	if (ACTION_EVENT == pEvent->m_ulEventType)
	{
		auto Action = (CActionEvent *)pEvent;
		MATRIX4D Camera = FastInverse(MAIN->m_pDXPainter->m_Params.View);
		VECTOR4D Pos, Dir;
		MATRIX4D Orientation = Camera;
		Orientation.vec[3] = { 0, 0, 0, 1 };
		Pos = Camera.vec[3];
		Dir = Camera.vec[2];
		if (JOY_AXIS_RY == Action->m_nAction)
		{	
			//Dead zone
			float Stimulus = (fabs(Action->m_fAxis) < 0.2 ? 0.0f: Action->m_fAxis);
			Pos = Pos + Dir * Stimulus * 0.1;
		}

		if (JOY_AXIS_LX == Action->m_nAction)
		{
			float Stimulus = (fabs(Action->m_fAxis) < 0.2 ? 0.0f : Action->m_fAxis);
			Orientation = Orientation * RotationAxis(Stimulus * 0.01, Camera.vec[1]); //sobre el eje y
		}

		if (JOY_AXIS_LY == Action->m_nAction)
		{
			float Stimulus = (fabs(Action->m_fAxis) < 0.2 ? 0.0f : Action->m_fAxis * -1);
			Orientation = Orientation * RotationAxis(Stimulus * 0.01, Camera.vec[0]); //sobre el eje x
		}
		
		Camera.vec[0] = Orientation.vec[0];
		Camera.vec[1] = Orientation.vec[1];
		Camera.vec[2] = Orientation.vec[2];
		Camera.vec[3] = Pos;
		MAIN->m_pDXPainter->m_Params.View = Orthogonalize(FastInverse(Camera));
	}
	if (APP_LOOP == pEvent->m_ulEventType)
	{
		auto Paint = MAIN->m_pDXPainter;
		auto DXManager = MAIN->m_pDXManager;
		Paint->SetRenderTarget(DXManager->GetMainRTV(), //Backbuffer
			DXManager->GetMainDSV()); //ZBuffer

		VECTOR4D DeepBlue = { 0.2, 0.2, 0.7, 0 };
		DXManager->GetContext()->ClearDepthStencilView(DXManager->GetMainDSV(),
					D3D11_CLEAR_STENCIL | D3D11_CLEAR_DEPTH, 1.0F, 0.0);

		DXManager->GetContext()->ClearRenderTargetView(DXManager->GetMainRTV(), (float *)&DeepBlue);

		Paint->m_Params.Lights[0].Type = LIGHT_DIRECTIONAL;
		Paint->m_Params.Lights[0].Direction = { 0, -1, 0, 0 };
		Paint->m_Params.Lights[0].Diffuse = { 1, 1, 1, 1 };
		Paint->m_Params.Flags = LIGHTING_DIFFUSE;
		Paint->m_Params.Material.Diffuse = { 1, 1, 1, 0 };
		Paint->m_Params.Material.Emissive = { 0, 0, 0, 0 };

		Paint->DrawIndexed(&m_pGeometry->m_Vertices[0],
			m_pGeometry->m_Vertices.size(),
			&m_pGeometry->m_Indices[0],
			m_pGeometry->m_Indices.size());

		DXManager->GetSwapChain()->Present(1, 0);
	}
	return __super::OnEvent(pEvent);
}

void CSGame::OnExit()
{
	SAFE_DELETE(m_pGeometry);
	printf("CSGame::OnExit()\n"); fflush(stdout);
}

CSGame::CSGame()
{
}


CSGame::~CSGame()
{
}
