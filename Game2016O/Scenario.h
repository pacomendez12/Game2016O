#pragma once

#include <vector>
#include <map>

#include "ScenarioObject.h"

using namespace std;

class Scenario
{
public:
	int getNewScenarioObjectId();
	void addElementToScenario(int scenarioObjectId, ScenarioObject *scenarioObject);
	void removeElementsFromScenario();
	void paintScenarioObjects(CDXBasicPainter* m_pDXPainter);
	void removeScenarioObjectById(int scenarioObjectId);
	ScenarioObject* getScenarioObect(int scenarioObjectId);
	map<int, ScenarioObject *> &getScenarioObjects();
	inline int count() { return scenarioObjects.size(); }
	void resetIds();
protected:
	map<int, ScenarioObject *> scenarioObjects;
	map<int, ScenarioObject *>::iterator it;
	int idIndex;
public:
	Scenario();
	virtual ~Scenario();
};