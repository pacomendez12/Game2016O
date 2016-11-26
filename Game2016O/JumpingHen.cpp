#include "stdafx.h"
#include "JumpingHen.h"


JumpingHen::JumpingHen()
{
}

JumpingHen::JumpingHen(int objectId, double scale, CMesh * mesh, ScenarioPosition * initialPostion, bool paint, ScenarioPosition * finalPostion, int totalSteps, int barnId):
	Hen(objectId, scale, mesh, initialPostion, paint, finalPostion, totalSteps, barnId)
{
}

void JumpingHen::move()
{
	d_x += incrementX;
	d_y += incrementY;
	if (d_z == 0) {
		d_z += 10;
	}else {
		d_z = 0;
	}

	traveledDistance += sqrt(incrementX*incrementX + incrementY*incrementY);
}


JumpingHen::~JumpingHen()
{
}
