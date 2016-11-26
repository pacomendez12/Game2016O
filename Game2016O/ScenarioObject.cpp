#include "stdafx.h"
#include "ScenarioObject.h"


ScenarioObject::ScenarioObject()
{
}

ScenarioObject::ScenarioObject(int objectId, CMesh * mesh, bool paint)
{
	i_objectId = objectId;
	m_objectMesh = mesh;
	d_x = 0.0;
	d_y = 0.0;
	d_z = 0.0;
	d_scale = 1.0;
	b_paint = paint;
}

ScenarioObject::ScenarioObject(int objectId, double scale, CMesh * mesh, ScenarioPosition *scenarioPosition, bool paint)
{
	i_objectId = objectId;
	m_objectMesh = mesh;
	d_x = scenarioPosition->getX();
	d_y = scenarioPosition->getY();
	d_z = scenarioPosition->getZ();
	d_scale = scale;
	b_paint = paint;
}

double ScenarioObject::getX()
{
	return d_x;
}

double ScenarioObject::getY()
{
	return d_y;
}

double ScenarioObject::getZ()
{
	return d_z;
}

double ScenarioObject::getScale()
{
	return d_scale;
}

int ScenarioObject::getObjectId()
{
	return i_objectId;
}

bool ScenarioObject::getPaint()
{
	return b_paint;
}

CMesh * ScenarioObject::getObjectMesh()
{
	return m_objectMesh;
}

void ScenarioObject::setObjectId(int objectId)
{
	i_objectId = objectId;
}

void ScenarioObject::moveTo(double x, double y)
{
	d_x = x;
	d_y = y;
}

void ScenarioObject::moveTo(double x, double y, double z)
{
	d_x = x;
	d_y = y;
	d_z = z;
}

void ScenarioObject::setX(double x)
{
	d_x = x;
}

void ScenarioObject::setY(double y)
{
	d_y = y;
}

void ScenarioObject::setZ(double z)
{
	d_z = z;
}

void ScenarioObject::setScale(double scale)
{
	d_scale = scale;
}

void ScenarioObject::setPaint(bool paint)
{
	b_paint = paint;
}

void ScenarioObject::incrementScore(int i)
{
}


ScenarioObject::~ScenarioObject()
{
}
