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
	d_initialX = d_x;
	d_initialY = d_y;
	d_scale = 1.0;
	b_paint = paint;
	v4D_objectColor = { 1,1,1,0 };
}

ScenarioObject::ScenarioObject(int objectId, double scale, CMesh * mesh,
	ScenarioPosition *scenarioPosition, bool paint, VECTOR4D objectColor)
{
	i_objectId = objectId;
	m_objectMesh = mesh;
	d_x = scenarioPosition->getX();
	d_y = scenarioPosition->getY();
	d_z = scenarioPosition->getZ();
	d_initialX = d_x;
	d_initialY = d_y;
	d_scale = scale;
	b_paint = paint;
	v4D_objectColor = objectColor;

	for (int i = 0; i < m_objectMesh->m_Vertices.size(); i++)
	{
		m_objectMesh->m_Vertices[i].Color = objectColor;
	}
}

double ScenarioObject::getX()
{
	return d_x;
}

double ScenarioObject::getY()
{
	return d_y;
}

double ScenarioObject::getInitialX()
{
	return d_initialX;
}

double ScenarioObject::getInitialY()
{
	return d_initialY;
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

void ScenarioObject::moveTo(ScenarioPosition *scenarioPosition)
{
	d_x = scenarioPosition->getX();
	d_y = scenarioPosition->getY();
	d_z = scenarioPosition->getZ();
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

VECTOR4D ScenarioObject::getObjectColor() {
	return v4D_objectColor;
}

ScenarioObject::~ScenarioObject()
{
	/* TODO: Fix this memory leak*/
	//delete m_objectMesh;
}
