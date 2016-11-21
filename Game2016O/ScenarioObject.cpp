#include "stdafx.h"
#include "ScenarioObject.h"


ScenarioObject::ScenarioObject()
{
}

ScenarioObject::ScenarioObject(int objectId, CMesh * mesh)
{
	i_objectId = objectId;
	m_objectMesh = mesh;
	d_x = 0.0;
	d_y = 0.0;
	d_z = 0.0;
	d_scale = 1.0;
}

ScenarioObject::ScenarioObject(int objectId, double scale, CMesh * mesh, double x, double y)
{
	i_objectId = objectId;
	m_objectMesh = mesh;
	d_x = x;
	d_y = y;
	d_z = 0.0;
	d_scale = scale;
}

ScenarioObject::ScenarioObject(int objectId, double scale, CMesh * mesh, double x, double y, double z)
{
	i_objectId = objectId;
	m_objectMesh = mesh;
	d_x = x;
	d_y = y;
	d_z = z;
	d_scale = scale;
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


ScenarioObject::~ScenarioObject()
{
}
