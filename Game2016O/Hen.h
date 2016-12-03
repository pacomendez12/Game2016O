#pragma once
#include <math.h>
#include "ScenarioObject.h"

using namespace std;

class Hen :
	public ScenarioObject
{
public:
	Hen();
	Hen(int objectId, CMesh *mesh, bool paint);
	Hen(int objectId, double scale, CMesh * mesh,
		ScenarioPosition *initialPostion, bool paint,
		ScenarioPosition *finalPostion,
		int totalSteps, int barnId, VECTOR4D objectColor);
	void move();
	void moveBackwards();
	int getBarnId();
	boolean alreadyInBarn();
	boolean alreadyInOrigin();
	virtual ~Hen();
protected:
	double slope;
	double direction;
	double incrementX;
	double incrementY;
	double traveledDistance;
	double targetDistance;
	int i_barnId;
};

