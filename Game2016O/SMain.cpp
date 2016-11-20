#include "stdafx.h"
#include "SMain.h"
#include <stdio.h>
#include <iostream>
#include <memory>
#include <string>
#include "BlenderImporter.h"

#include "InputProcessor.h"

const char *g_cszMeshesNames[]{
	"table", "mallet"
};

const char *g_cszMeshesFileNames[]{
	"table.blend", "mallet.blend"
};


CSMain::CSMain()
{
	m_hWnd = nullptr;
	m_pDXManager = nullptr;
	m_pDXPainter = nullptr;
	m_pInputManager = nullptr;
	m_pSndManager = nullptr;
	m_bInitializationCorrect = true;
	m_pInputProcessor = nullptr;
	m_pNetProcessor = nullptr;
	m_FX = nullptr;

	InitializeCriticalSection(&m_csLock);
	
}


CSMain::~CSMain()
{
	for (auto mesh : m_mMeshes)
	{
		delete mesh.second;
	}

	m_mMeshes.clear();
	DeleteCriticalSection(&m_csLock);
}

short CSMain::GetModelsLoadingPercentage()
{
	return AreModelsLoaded()? 100 : 0;
}

CMesh *CSMain::GetMeshByString(std::string sMesh)
{
	Lock();
	auto fnd = m_mMeshes.find(sMesh);
	return m_mMeshes.end() == fnd ? nullptr : fnd->second;
	Unlock();
}

void CSMain::OnEntry(void)
{
	std::cout << "OnEntry: CSMain" << std::endl;

	m_pInputProcessor = new CInputProcessor(m_pSMOwner);
	printf("Iniciando motores...\n");
	printf("Graphics Init...\n");
	fflush(stdout);
	m_pDXManager = new CDXManager();
	m_pDXPainter = new CDXBasicPainter(m_pDXManager);
	m_FX = new CFX(m_pDXManager);
	if (!m_pDXManager->Initialize(m_hWnd, CDXManager::EnumAndChooseAdapter(NULL)))
	{
		m_bInitializationCorrect = false;
		MessageBox(m_hWnd, L"No se pudo iniciar DirectX 11", L"Error", MB_ICONERROR);
	}
	if (!m_pDXPainter->Initialize())
	{
		m_bInitializationCorrect = false;
		MessageBox(m_hWnd, L"Error no se pudo iniciar shaders", L"Error", MB_ICONERROR);
	}
	if (m_bInitializationCorrect)
	{
		printf("Success...\n"); fflush(stdout);
	}
	else
	{
		printf("BAD ):...\n"); fflush(stdout);
	}

	

	printf("Sound init...\n"); fflush(stdout);
	CSndFactory* pFactory = new CSndFactory();
	m_pSndManager=(CSndManager*)pFactory->CreateObject(L"CSndManager");
	if (!m_pSndManager->InitSoundEngine(m_hWnd))
		m_bInitializationCorrect = false;
	if (m_bInitializationCorrect)
	{
		printf("Success\n"); fflush(stdout);
	}
	else { printf("BAD  ): ...\n"); fflush(stdout); }
	printf("Input init...\n"); fflush(stdout);
	m_pInputManager = new CInputManager();
	if (!m_pInputManager->InitializeDirectInputSession(m_hInstance))
	{
		printf("Bad input init...\n");
		m_bInitializationCorrect = false;
	}
	else if (!m_pInputManager->ConfigureDevices(m_hWnd))
	{
		printf("Unable to aquire input devices... :( ...\n");
		m_bInitializationCorrect = false;
	}
	else
	{
		printf("OK :)... \n");
	}

	// cargar los modelos asyncronamente una vez que tegamos video y audio
	// funcionando.
	LoadModels();

	printf("Initializing network engine...");
	fflush(stdout);

	m_pNetProcessor = new CNetProcessor(m_pSMOwner);
	if (!m_pNetProcessor->InitNetwork())
	{
		printf("Unable to work as server, connections are allowed instead");
	}
	fflush(stdout);
}
#include "HSM\EventWin32.h"
#include "HSM\StateMachineManager.h"
unsigned long CSMain::OnEvent(CEventBase * pEvent)
{
	if (APP_LOOP == pEvent->m_ulEventType)
	{
		if (m_pSndManager)
			m_pSndManager->RemoveAllSndFxStopped();
		for (int iSource = 0; iSource < m_pInputManager->GetDeviceCount(); iSource++)
		{
			DIJOYSTATE2 js2;
			if (m_pInputManager->ReadState(js2, iSource))
			{
				CInputEvent *pInput = new CInputEvent(iSource,0,js2);
				m_pSMOwner->PostEvent(pInput);
				m_pInputProcessor->OnEvent(pInput);
			}
		}
		//if (m_bInitializationCorrect)
		//{
		//	ID3D11RenderTargetView* pRTV = m_pDXManager->GetMainRTV();
		//	ID3D11DepthStencilView* pDSV = m_pDXManager->GetMainDSV();
		//	ID3D11DeviceContext* pCtx = m_pDXManager->GetContext();
		//	//Limpieza el render target
		//	VECTOR4D Color = { 0,0,0,0 };
		//	pCtx->ClearRenderTargetView(pRTV, (float*)&Color);
		//	pCtx->ClearDepthStencilView(pDSV, D3D11_CLEAR_STENCIL | D3D11_CLEAR_DEPTH, 1.0f, 0);
		//	//Establecer Render Target y Buffer Z a través de DXBasicPainter
		//	m_pDXPainter->SetRenderTarget(pRTV, pDSV);
		//	CDXBasicPainter::VERTEX Triangle[] =
		//	{
		//		//Position  | Normal | Binormal | Tangent | Color | TexCoord
		//		{ { 0,1,0,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,0,0,0 },{ 0,0,0,0 } },
		//		{ { 1,-1,0,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,1,0,0 },{ 0,0,0,0 } },
		//		{ { -1,-1,0,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,1,0 },{ 0,0,0,0 } }
		//	};
		//	unsigned long TriangleIndices[] = { 0,1,2 };
		//	//Establecer el modelo de iluminación a Vértice Pre-iluminado 
		//	//Así no se hacen calculos de color adicionales en GPU ya que el atributo Color del vértice lo especifica
		//	m_pDXPainter->m_Params.Flags = VERTEX_PRELIGHTEN;
		//	m_pDXPainter->DrawIndexed(Triangle, 3, TriangleIndices, 3);
		//	m_pDXManager->GetSwapChain()->Present(1, 0);
	}
	if (EVENT_WIN32 == pEvent->m_ulEventType)
	{
		CEventWin32* pWin32 = (CEventWin32*)pEvent;
		switch (pWin32->m_msg)
		{
		case WM_SIZE:
			if (m_pDXManager->GetSwapChain())
			{
				m_pDXManager->Resize(LOWORD(pWin32->m_lParam), HIWORD(pWin32->m_lParam));
			}
			break;
		case WM_CLOSE:
			m_pSMOwner->Transition(CLSID_CStateNull);
			return 0;
		case WM_CHAR:
			switch (pWin32->m_lParam)
			{
			case 'c': case 'C':
				MAIN->m_pNetProcessor->Connect(L"127.0.0.1");
			}
			break;
		}
	}
	return __super::OnEvent(pEvent);
}

void CSMain::OnExit(void)
{
	m_pDXPainter->Uninitialize();
	m_pDXManager->Uninitialize();
	m_pSndManager->UnitializeSoundEngine();
	m_pInputManager->FinalizeDirectInputSession();
	SAFE_DELETE(m_pDXPainter);
	SAFE_DELETE(m_pDXManager);
	CSndFactory Factory;
	Factory.DestroyObject(m_pSndManager);
	SAFE_DELETE(m_pInputManager);
	SAFE_DELETE(m_pInputProcessor);
	std::cout << "onExit: " << GetClassString() << std::endl;
}


//thread process
DWORD CSMain::LoaderThread(LPVOID obj)
{
	CSMain *csmain = (CSMain *)obj;
	std::cout << "Iniciando el hilo cargador de meshes..." << std::endl;
	for (int i = 0; i < MESHES_NUMBER; ++i)
	{
		std::cout << "Loading mesh " << g_cszMeshesNames[i] << std::endl;
		std::unique_ptr<CMesh> geometry = CBlenderImporter::ImportObject((std::string("..\\Assets\\") + std::string(g_cszMeshesFileNames[i])).c_str());
		CMesh *ptrMesh = geometry.release();
		csmain->m_mMeshes.insert(std::make_pair(std::string(g_cszMeshesNames[i]), ptrMesh));

		std::cout << "Mesh " << g_cszMeshesNames[i] << " has " << ptrMesh->m_Vertices.size() << std::endl;
	}
	return 0;
}

void CSMain::LoadModels()
{
	if (!AreModelsLoaded())
	{
		Lock();
		DWORD dwThreadID;
		CreateThread(nullptr, 4096, (LPTHREAD_START_ROUTINE)CSMain::LoaderThread, this, 0, &dwThreadID);
		Unlock();
	}
}
