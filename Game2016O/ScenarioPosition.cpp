#include "stdafx.h"
#include "ScenarioPosition.h"


ScenarioPosition::ScenarioPosition()
{
	d_x = 0;
	d_y = 0;
	d_z = 0;
}

ScenarioPosition::ScenarioPosition(double x, double y, double z)
{
	d_x = x;
	d_y = y;
	d_z = z;
}


ScenarioPosition::~ScenarioPosition()
{
}

double ScenarioPosition::getX()
{
	return d_x;
}

double ScenarioPosition::getY()
{
	return d_y;
}

double ScenarioPosition::getZ()
{
	return d_z;
}
