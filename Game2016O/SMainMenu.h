#pragma once

#include "Graphics\FX.h"
#include "HSM\StateBase.h"
#include "DXTextRenderer.h"
#include <d3d11.h>
#include <vector>
#define CLSID_CSMainMenu 0x12345566


#define NUMBER_BUTTONS 3
#define BUTTON_STATES_NUMBER 2
#define DEFAULT_BUTTON 0

class CSMainMenu :
	public CStateBase
{
public:
	const char* GetClassString() { return "CSMainMenu"; }
	unsigned long GetClassID() { return CLSID_CSMainMenu; }
public:
	CSMainMenu();
	virtual ~CSMainMenu();
private:

	CDXTextRenderer *m_pTextRenderer;
	ID3D11ShaderResourceView
		*m_pSRVBackGround,
		*m_pSRVMainOption1,
		*m_pSRVMainOption2;

	enum ButtonState {
		NORMAL = 0, OVER
	};

	struct MenuOption {
		ID3D11ShaderResourceView *pSRV[BUTTON_STATES_NUMBER];
		//CFX::VERTEX frame[4];
		CDXBasicPainter::VERTEX frame[4];
		unsigned long indices[6];
		ButtonState state;
	};

	int m_nOption;    //0: Option1, 1: Option2
	std::vector<MenuOption> m_vMenuButtons;

	bool LoadButtons();
	void OnEntry();
	unsigned long OnEvent(CEventBase* pEvent);
	void OnExit();
	float m_fOffsetX;
	float m_fOffsetY;
};

