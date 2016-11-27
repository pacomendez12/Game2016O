#include "stdafx.h"
#include "Barn.h"


Barn::Barn()
{
	i_hensInHouse = 0;
}

Barn::Barn(int objectId, double scale, CMesh * mesh, ScenarioPosition *scenarioPosition, bool paint, VECTOR4D objectColor):
	ScenarioObject(objectId, scale, mesh, scenarioPosition, paint, objectColor)
{
	i_hensInHouse = 0;
}

Barn::~Barn()
{
}

void Barn::incrementScore(int i)
{
	i_hensInHouse += i;
}

int Barn::getHensInHouse()
{
	return i_hensInHouse;
}
