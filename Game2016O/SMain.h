#pragma once
#include "HSM\StateBase.h"
#include "Graphics\DXManager.h"
#include "Graphics\DXBasicPainter.h"
#include "Graphics\FX.h"
#include "Graphics\Mesh.h"
#include "Sound\SndFactory.h"
#include "Sound\SndManager.h"
#include "Sound\SndControl.h"
#include "Sound\SndFx.h"
#include "Input\InputManager.h"
#include "NetProcessor.h"
#include <unordered_map>
#include <string>
#include "DXTextRenderer.h"


#define MAIN ((CSMain*)m_pSMOwner->GetObjectByID(CLSID_CSMain))
#define CLSID_CSMain 0x0dccd3ed

#define INPUT_EVENT 0x12345678


#define MESHES_NUMBER 2

class CInputProcessor;
class CInputEvent :public CEventBase
{
public:
	int             m_nSource; 
	unsigned long   m_ulTime;
	DIJOYSTATE2     m_js2;
	CInputEvent(int nSource, unsigned long ulTime, DIJOYSTATE2& js2)
	{
		m_ulEventType = INPUT_EVENT;
		m_nSource = nSource;
		m_ulTime = ulTime;
		m_js2 = js2;
	}
};

class CSMain :
	public CStateBase
{
public:
	HINSTANCE   m_hInstance;
	HWND		m_hWnd;
	CDXManager* m_pDXManager;
	CDXBasicPainter* m_pDXPainter;
	CSndManager* m_pSndManager;
	CInputManager* m_pInputManager;
	CInputProcessor* m_pInputProcessor;
	CNetProcessor *m_pNetProcessor;
	CDXTextRenderer *m_pTextRenderer;
	CFX *m_FX;


	bool        m_bInitializationCorrect;
	unsigned long GetClassID() { return CLSID_CSMain; }
	const char* GetClassString() { return "CSMain"; }
	CSMain();
	virtual ~CSMain();

	inline bool AreModelsLoaded() { 
		bool result;
		Lock();
		result = m_bMedelsLoaded;
		Unlock();
		return result;
	}
	short GetModelsLoadingPercentage();
	CMesh * GetMeshByString(std::string sMesh);

	static DWORD CSMain::LoaderThread(LPVOID obj);
protected:
	void OnEntry(void);
	unsigned long OnEvent(CEventBase* pEvent);
	void OnExit(void);

private:
	// data
	bool	m_bMedelsLoaded;
	CMesh *m_pGeometry;
	std::unordered_map<std::string, CMesh *> m_mMeshes;
	CRITICAL_SECTION m_csLock;
	inline void Lock(CSMain * obj = nullptr)
	{
		if (!obj)
			obj = this;
		EnterCriticalSection(&obj->m_csLock);
	}
	inline void Unlock(CSMain * obj = nullptr)
	{
		if (!obj)
			obj = this;
		LeaveCriticalSection(&obj->m_csLock);
	}

	inline static void LockS(CSMain * obj = nullptr)
	{
		if (obj)
			obj->Lock();
	}
	inline static void UnlockS(CSMain * obj = nullptr)
	{
		if (obj)
			obj->Unlock(obj);
	}

	void LoadModels();

};

