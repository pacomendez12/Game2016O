#pragma once
#include "Graphics\Mesh.h"
class ScenarioObject
{
public:
	ScenarioObject();
	ScenarioObject(int objectId, CMesh *mesh);
	ScenarioObject(int objectId, double scale, CMesh * mesh, double x, double y);
	ScenarioObject(int objectId, double scale, CMesh * mesh, double x, double y, double z);
	double getX();
	double getY();
	double getZ();
	double getScale();
	int getObjectId();
	CMesh * getObjectMesh();
	void setObjectId(int objectId);
	void moveTo(double x, double y);
	void moveTo(double x, double y, double z);
	void setX(double x);
	void setY(double y);
	void setZ(double z);
	void setScale(double scale);
protected:
	int i_objectId;
	double d_x;
	double d_y;
	double d_z;
	double d_scale;
	CMesh* m_objectMesh;
	virtual ~ScenarioObject();
};

