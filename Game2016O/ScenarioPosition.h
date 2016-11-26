#pragma once
class ScenarioPosition
{
public:
	ScenarioPosition();
	ScenarioPosition(double x, double y, double z);
	virtual ~ScenarioPosition();
	double getX();
	double getY();
	double getZ();

protected:
	double d_x;
	double d_y;
	double d_z;
};

