#pragma once
#include "ScenarioObject.h"
class Barn :
	public ScenarioObject
{
public:
	Barn();
	Barn(int objectId, double scale, CMesh * mesh, ScenarioPosition *scenarioPosition, bool paint, VECTOR4D objectColor);
	virtual ~Barn();
	void incrementScore(int i);
	int getHensInHouse();
protected:
	int i_hensInHouse;
};

