#pragma once
#include "HSM\StateBase.h"
#include "SGame.h"
#define CLSID_CSServerMultiplayer 0x004bac0
class CSServerMultiplayer :
	public CStateBase
{
private:
	unsigned long m_ulKeyGen;
protected:
	void OnEntry(void);
	unsigned long OnEvent(CEventBase* pEvent);
	void OnExit(void);
	CSGame* m_pGame;
public:
	unsigned long GetClassID() { return CLSID_CSServerMultiplayer; }
	const char* GetClassString() { return "CSServerMultiplayer"; }
	CSServerMultiplayer();
	virtual ~CSServerMultiplayer();
};

