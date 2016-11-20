#pragma once
#include "HSM\StateBase.h"
#include "Graphics\Mesh.h"
#include "Camera.h"

#define CLSID_CSGame 0x33221100
class CSGame :
	public CStateBase
{
public:
	unsigned long GetClassID() { return CLSID_CSGame; }
	const char* GetClassString() { return "CSGame"; }
protected:
	CMesh *m_pTable;
	CMesh *m_pMallet;
	CCamera *m_pCamera;
	void OnEntry();
	unsigned long OnEvent(CEventBase* pEvent);
	void OnExit();
public:
	CSGame();
	virtual ~CSGame();
};

