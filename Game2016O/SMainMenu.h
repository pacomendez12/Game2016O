#pragma once
#include "HSM\StateBase.h"
#include <d3d11.h>

#define CLSID_CSMainMenu 0x52f0ca

class CSMainMenu :
	public CStateBase
{
public:
	inline const char* GetClassString() { return "CSMainMenu"; }
	inline unsigned long GetClassID() { return CLSID_CSMainMenu; }
	CSMainMenu();
	virtual ~CSMainMenu();

private:
	ID3D11ShaderResourceView* m_pSRVBackGround;
	ID3D11ShaderResourceView* m_pSRVMainOption1;
	ID3D11ShaderResourceView* m_pSRVMainOption2;
	ID3D11ShaderResourceView* m_pSRVSelectionBar;
	int m_nOption; // 0: option1, 1:option2

protected:
	void OnEntry();
	unsigned long OnEvent(CEventBase*  pEvent);
	void OnExit();
};

