#pragma once
#include "HSM\StateBase.h"
#include "SGame.h"
#define CLSID_CSNetworkGame 0x0bab47ef
class CSNetworkGame :public CStateBase
{
private:
	CSGame* m_pGame;
	ID3D11ShaderResourceView* m_pSRVNetBackground;
protected:
	void OnEntry(void);
	unsigned long OnEvent(CEventBase* pEvent);
	void OnExit(void);
public:
	unsigned long GetClassID() { return CLSID_CSNetworkGame; }
	const char* GetClassString() { return "CSNetworkGame"; }
	CSNetworkGame();
	virtual ~CSNetworkGame();
};

