#pragma once
#include "HSM\StateBase.h"
#include "Camera.h"
#include "Sound\SndManager.h"

#define CLSID_CSCredits 0x52f5ac25

#define SND_BACKGROUND 1

class CSCredits :
	public CStateBase
{
public:
	CSCredits();
	virtual ~CSCredits();

	unsigned long GetClassID() { return CLSID_CSCredits; }
	const char* GetClassString() { return "CSCredits"; }

	unsigned long OnEvent(CEventBase* pEvent);
	void OnEntry();
	void OnExit();

private:
	CCamera *m_pCamera;
	CSndControl* m_pSndBackground;
	float m_fOffset;
};

