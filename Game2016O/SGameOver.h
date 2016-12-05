#pragma once
#include "HSM\StateBase.h"

#define CLSID_CSGameOver 0xa5860cf6
class CSGameOver :
	public CStateBase
{
public:
	CSGameOver();
	virtual ~CSGameOver();

	unsigned long GetClassID() { return CLSID_CSGameOver; }
	const char* GetClassString() { return "CSGameOver"; }
	unsigned long OnEvent(CEventBase* pEvent);
	void OnEntry();
	void OnExit();
};

