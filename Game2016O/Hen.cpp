#include "stdafx.h"

#include <iostream>
#include "Hen.h"

Hen::Hen()
{
}

Hen::Hen(int objectId, CMesh * mesh, bool paint):
	ScenarioObject(objectId, mesh, paint)
{
	d_x = 0;
	d_y = 0;
	d_z = 0;
	slope = 0;;
	direction = 1;
	incrementX = 0;
	incrementY = 0;
	traveledDistance = 0;
	i_barnId = 0;
}

Hen::Hen(int objectId, double scale, CMesh * mesh,
	ScenarioPosition * initialPostion, bool paint,
	ScenarioPosition * finalPostion, int totalSteps, int barnId):
	ScenarioObject(objectId, scale, mesh, initialPostion,
		paint)
{

	//cout << "FINAL: " << finalPostion->getX() << "," << finalPostion->getY() << endl;
	//cout << "Origin: " << d_x << "," << d_y << endl;

	double distanceX = finalPostion->getX() - d_x;
	double distanceY = finalPostion->getY() - d_y;

	//cout << distanceX << endl;
	//cout << distanceY << endl;

	incrementX = distanceX / totalSteps;
	incrementY = distanceY / totalSteps;
	traveledDistance = 0;
	targetDistance = sqrt(distanceX * distanceX + distanceY * distanceY);
	i_barnId = barnId;

	//cout << ((incrementX * totalSteps) + d_x) << "," <<((incrementY * totalSteps) + d_y) << endl;
}

void Hen::move()
{
	d_x += incrementX;
	d_y += incrementY;

	traveledDistance += sqrt(incrementX*incrementX + incrementY*incrementY);
	//cout << d_x << "," << d_y << endl;

	// If you wanna make the hen to jump
	// incremente the z axis
}

int Hen::getBarnId()
{
	return i_barnId;
}

boolean Hen::alreadyInBarn() {
	if (traveledDistance >= targetDistance)
		return true;
	return false;
}

Hen::~Hen()
{
}