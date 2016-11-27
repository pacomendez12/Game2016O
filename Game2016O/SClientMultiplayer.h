#pragma once
#include "HSM\StateBase.h"
#include "SGame.h"
#define CLSID_CSClientMultiplayer 0xe14bac0
class CSClientMultiplayer :
	public CStateBase
{
protected:
	void OnEntry(void);
	unsigned long OnEvent(CEventBase* pEvent);
	void OnExit(void);
public:
	unsigned long GetClassID() { return CLSID_CSClientMultiplayer; }
	const char* GetClassString() { return "CSClientMultiplayer"; }
	CSClientMultiplayer();
	virtual ~CSClientMultiplayer();
};

