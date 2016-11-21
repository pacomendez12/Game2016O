#include "stdafx.h"
#include "Scenario.h"


int Scenario::getNewScenarioObjectId()
{
	return scenarioObjects.size();
}

void Scenario::addElementToScenario(ScenarioObject *scenarioObject)
{
	scenarioObjects.push_back(scenarioObject);
}

void Scenario::removeElementsFromScenario()
{
	scenarioObjects.clear();
}

void Scenario::paintScenarioObjects(CDXBasicPainter* m_pDXPainter)
{
	int totalScenarioObjects = scenarioObjects.size();
	for (int i = 0; i < totalScenarioObjects; i++) {
		ScenarioObject *scenarioObject = scenarioObjects[i];
		CMesh* currentMesh = scenarioObject->getObjectMesh();

		MATRIX4D scaling = Scaling(scenarioObject->getScale(), scenarioObject->getScale(), scenarioObject->getScale());
		MATRIX4D traslation = Translation(scenarioObject->getX(), scenarioObject->getY(), scenarioObject->getZ());

		// Object in it's right position and size
		m_pDXPainter->m_Params.World = traslation * scaling;
			
		// Painter drawing current Mesh
		m_pDXPainter->DrawIndexed(&currentMesh->m_Vertices[0],
			currentMesh->m_Vertices.size(),
			&currentMesh->m_Indices[0],
			currentMesh->m_Indices.size());
	}
}

Scenario::Scenario()
{
}


Scenario::~Scenario()
{
}
