#pragma once
#include "HSM\StateBase.h"
#include "Graphics\Mesh.h"
#include "Camera.h"
#include "Scenario.h"

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
	Scenario *scenario;
	ID3D11ShaderResourceView* m_pSRVBackground;
	void OnEntry();
	unsigned long OnEvent(CEventBase* pEvent);
	void OnExit();
public:
	CSGame();
	virtual ~CSGame();
};

