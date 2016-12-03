#pragma once
#include "Graphics\Mesh.h"
#include "ScenarioPosition.h"
#include "Graphics\Matrix4D.h"

class ScenarioObject
{
public:
	ScenarioObject();
	ScenarioObject(int objectId, CMesh *mesh, bool paint);
	ScenarioObject(int objectId, double scale, CMesh * mesh,
		ScenarioPosition *scenarioPosition, bool paint, VECTOR4D objectColor);
	double getX();
	double getY();
	double getInitialX();
	double getInitialY();
	double getZ();
	double getScale();
	int getObjectId();
	bool getPaint();
	VECTOR4D getObjectColor();
	CMesh * getObjectMesh();
	void setObjectId(int objectId);
	void moveTo(double x, double y);
	void moveTo(double x, double y, double z);
	void moveTo(ScenarioPosition *scenarioPosition);
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
	double d_initialX;
	double d_initialY;
	CMesh* m_objectMesh;
	VECTOR4D v4D_objectColor;
	
};

