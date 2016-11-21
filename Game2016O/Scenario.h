#pragma once

#include <vector>

#include "ScenarioObject.h"

using namespace std;

class Scenario
{
public:
	int getNewScenarioObjectId();
	void addElementToScenario(ScenarioObject *scenarioObject);
	void removeElementsFromScenario();
	void paintScenarioObjects(CDXBasicPainter* m_pDXPainter);
protected:
	vector<ScenarioObject *> scenarioObjects;
public:
	Scenario();
	virtual ~Scenario();
};