#include "stdafx.h"
#include "Camera.h"
#include <iostream>


CCamera::CCamera(CDXBasicPainter * pDXPainter) :
	m_pDXPainter(pDXPainter)
{
	ResetCamera();
}

CCamera::~CCamera()
{
}

void CCamera::ChangeView(ViewMode view)
{
	m_currentView = view;
	//projection Matrix
	m_pDXPainter->m_Params.Projection = PerspectiveWidthHeightLH(0.05, 0.05, 0.1, 100);
	//VECTOR4D EyePos = { 5, 5, 0, 1 };
	VECTOR4D EyePos;

	switch (view)
	{
	case ViewMode::PlayerA:
		EyePos = { 8, 10, 0, 1 };
			break;
	case ViewMode::PlayerB:
		EyePos = { -8, 10, 0, 1 };
		break;
	case ViewMode::Top:
		EyePos = { 0, 0, 20, 1 };
		//EyePos = { 0, 12, 1, 1 };
		break;
	}
	
	VECTOR4D Target = { 0, 0, 0, 1 };
	VECTOR4D Up = { 0, 1, 0, 0 };
	m_pDXPainter->m_Params.View = View(EyePos, Target, Up);
	m_pDXPainter->m_Params.World = Identity();
}

void CCamera::ResetCamera()
{
	ChangeView(m_cDefaultView);
}

void CCamera::RotateXAxe(float factor)
{
	MATRIX4D Camera = FastInverse(m_pDXPainter->m_Params.View);
	MATRIX4D Orientation = Camera;
	Orientation.vec[3] = { 0, 0, 0, 1 };

	Orientation = Orientation * RotationAxis(factor * 0.01, Camera.vec[1]); //sobre el eje y
	//std::cout << "x = " << Pos.x << ", y = " << Pos.y << ", z = " << Pos.z << ", w = " << Pos.w << std::endl;
	Camera.vec[0] = Orientation.vec[0];
	Camera.vec[1] = Orientation.vec[1];
	Camera.vec[2] = Orientation.vec[2];
	ApplyChanges(Camera);
}

void CCamera::RotateYAxe(float factor)
{
	MATRIX4D Camera = FastInverse(m_pDXPainter->m_Params.View);
	MATRIX4D Orientation = Camera;
	Orientation.vec[3] = { 0, 0, 0, 1 };

	Orientation = Orientation * RotationAxis(factor * 0.01, Camera.vec[0]); //sobre el eje x
	//std::cout << "x = " << Pos.x << ", y = " << Pos.y << ", z = " << Pos.z << ", w = " << Pos.w << std::endl;
	Camera.vec[0] = Orientation.vec[0];
	Camera.vec[1] = Orientation.vec[1];
	Camera.vec[2] = Orientation.vec[2];
	ApplyChanges(Camera);
}

void CCamera::MoveZAxe(float factor)
{
	MATRIX4D Camera = FastInverse(m_pDXPainter->m_Params.View);
	VECTOR4D Pos, Dir;
	Pos = Camera.vec[3];
	Dir = Camera.vec[2];

	Pos = Pos + Dir * factor * 0.1;
	Camera.vec[3] = Pos;
	ApplyChanges(Camera);
}

void CCamera::MoveXAxe(float factor)
{
	/*MATRIX4D Camera = FastInverse(m_pDXPainter->m_Params.View);
	VECTOR4D Pos, Dir;
	Pos = Camera.vec[3];
	Dir = Camera.vec[2];

	Pos = Pos + Dir * factor * 0.1;
	Camera.vec[3] = Pos;
	ApplyChanges(Camera);*/
}

void CCamera::ApplyChanges(MATRIX4D &cam)
{
	m_pDXPainter->m_Params.View = Orthogonalize(FastInverse(cam));
}




