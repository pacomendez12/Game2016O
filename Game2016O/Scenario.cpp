#include "stdafx.h"
#include "Scenario.h"


int Scenario::getNewScenarioObjectId()
{
	idIndex++;
	return idIndex;
}

void Scenario::addElementToScenario(int scenarioObjectId, ScenarioObject * scenarioObject)
{
	scenarioObjects[scenarioObjectId] = scenarioObject;
}

void Scenario::removeElementsFromScenario()
{
	idIndex = -1;
	scenarioObjects.clear();
}

void Scenario::paintScenarioObjects(CDXBasicPainter* m_pDXPainter)
{
	for (it = scenarioObjects.begin(); it != scenarioObjects.end(); it++) {
		ScenarioObject *scenarioObject = it->second;
		if (scenarioObject->getPaint()) {
			CMesh* currentMesh = scenarioObject->getObjectMesh();

			MATRIX4D scaling = Scaling(scenarioObject->getScale(), scenarioObject->getScale(), scenarioObject->getScale());
			MATRIX4D traslation = Translation(scenarioObject->getX(), scenarioObject->getY(), scenarioObject->getZ());

			// Object in it's right position and size
			m_pDXPainter->m_Params.World =  scaling * traslation;
			m_pDXPainter->m_Params.Brightness = scenarioObject->getObjectColor();
			
			// Painter drawing current Mesh
			m_pDXPainter->DrawIndexed(&currentMesh->m_Vertices[0],
				currentMesh->m_Vertices.size(),
				&currentMesh->m_Indices[0],
				currentMesh->m_Indices.size());
		}
	}
}

void Scenario::removeScenarioObjectById(int scenarioObjectId)
{
	scenarioObjects.erase(scenarioObjectId);
}

ScenarioObject * Scenario::getScenarioObect(int scenarioObjectId)
{
	return scenarioObjects[scenarioObjectId];
}

map<int, ScenarioObject*> Scenario::getScenarioObjects()
{
	return scenarioObjects;
}

void Scenario::resetIds()
{
	idIndex = -1;
}

Scenario::Scenario()
{
	idIndex = -1;
}

Scenario::~Scenario()
{
}
