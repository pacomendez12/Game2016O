#pragma once
#include "HSM\EventListener.h"
#include <map>
#include "HSM\StateMachineManager.h"
#include "SMain.h"
#include "ActionEvent.h"

using namespace std;
class CInputProcessor :
	public CEventListener
{
public:
	CStateMachineManager* m_pHSMOwner;
	map<int, CInputEvent*> m_mapLastEvents;
	CInputProcessor(CStateMachineManager* pHSMOwner);
	virtual unsigned long OnEvent(CEventBase* pEvent);
	virtual ~CInputProcessor();

	enum JoysticMode { JOYSTIC, KEYBOARD };
	void SetJoysticMode(JoysticMode pMode);
private:
	JoysticMode m_jMode;

	// para keyboard mode
	JoyKeyState m_jState;
};

