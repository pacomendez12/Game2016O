#pragma once
#include "Graphics\Mesh.h"
#include "ScenarioPosition.h"
class ScenarioObject
{
public:
	ScenarioObject();
	ScenarioObject(int objectId, CMesh *mesh, bool paint);
	ScenarioObject(int objectId, double scale, CMesh * mesh, ScenarioPosition *scenarioPosition, bool paint);
	double getX();
	double getY();
	double getZ();
	double getScale();
	int getObjectId();
	bool getPaint();
	CMesh * getObjectMesh();
	void setObjectId(int objectId);
	void moveTo(double x, double y);
	void moveTo(double x, double y, double z);
	void setX(double x);
	void setY(double y);
	void setZ(double z);
	void setScale(double scale);
	void setPaint(bool paint);
	virtual void incrementScore(int i);
	virtual ~ScenarioObject();
protected:
	int i_objectId;
	double d_x;
	double d_y;
	double d_z;
	double d_scale;
	bool b_paint;
	CMesh* m_objectMesh;
};

