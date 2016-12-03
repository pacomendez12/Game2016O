#include "stdafx.h"
#include "InputProcessor.h"
#include "ActionEvent.h"
#include <iostream>

CInputProcessor::CInputProcessor(CStateMachineManager * pHSMOwner)
{
	m_pHSMOwner = pHSMOwner;
	SetJoysticMode(JoysticMode::JOYSTIC);
	m_jState.resize(8);
}

unsigned long CInputProcessor::OnEvent(CEventBase * pEvent)
{
	if (INPUT_EVENT == pEvent->m_ulEventType)
	{
		//1.- Construir copia del estado que ingresa
		CInputEvent* pInput = (CInputEvent*)pEvent;
		CInputEvent* pCopy = new CInputEvent(
			pInput->m_nSource,
			pInput->m_ulTime,
			pInput->m_js2);
		auto GamePad = m_mapLastEvents.find(pInput->m_nSource);
		//2.- Procesar diferencias
		if (GamePad != m_mapLastEvents.end())
		{
			auto Last = GamePad->second->m_js2;
			auto Curr = pInput->m_js2;

			for (int i = 0; i < 4; i++)
			{
				if (!Last.rgbButtons[i] && Curr.rgbButtons[i])
				{
					auto Action = new CActionEvent(GamePad->first,
							pInput->m_ulTime, (i * 2) + 1);
					m_pHSMOwner->PostEvent(Action);
					//printf("posting %d\n", (i * 2) + 1); fflush(stdout);
				}
				if (Last.rgbButtons[i] && !Curr.rgbButtons[i])
				{
					auto Action =
						new CActionEvent(GamePad->first,
							pInput->m_ulTime, (i * 2) + 2);
					m_pHSMOwner->PostEvent(Action);
					//printf("posting %d\n", (i * 2) + 2); fflush(stdout);
				}
			}

			//Reportar el estado de los ejes
			{
				auto Action = new CActionEvent(GamePad->first, pInput->m_ulTime, JOY_AXIS_LX);
				Action->m_fAxis = (Curr.lX - 0x7fff) / 32768.0f;
				m_pHSMOwner->PostEvent(Action);
			    Action = new CActionEvent(GamePad->first, pInput->m_ulTime, JOY_AXIS_LY);
				Action->m_fAxis = -(Curr.lY - 0x7fff) / 32768.0f;
				m_pHSMOwner->PostEvent(Action);

				/* cuando el modo es de joystic podemos postear los eventos, si no, solo posteamos 
				cuando haya un cambio */

				if (JoysticMode::JOYSTIC == m_jMode)
				{
					Action = new CActionEvent(GamePad->first, pInput->m_ulTime, JOY_AXIS_RX);
					Action->m_fAxis = (Curr.lZ - 0x7fff) / 32768.0f; //lZ es X para el segundo eje
					//printf("%d\n", Curr.lRz); fflush(stdin);
					m_pHSMOwner->PostEvent(Action);
					Action = new CActionEvent(GamePad->first, pInput->m_ulTime, JOY_AXIS_RY);
					Action->m_fAxis = -(Curr.lRz - 0x7fff) / 32768.0f; //lRz es y para el segundo eje
					m_pHSMOwner->PostEvent(Action);
					//printf("%f\n", Action->m_fAxis); fflush(stdin);
				}
				else if (JoysticMode::KEYBOARD == m_jMode)
				{
					float x = (Curr.lZ - 0x7fff) / 32768.0f; //lZ es X para el segundo eje
					float y = -(Curr.lRz - 0x7fff) / 32768.0f; ////lRz es y para el segundo eje

					//printf("%f\n", y); fflush(stdin);
					JoyKeyState state;
					const float deadZone = 0.3;
					if (x > deadZone && y < deadZone)
					{
						state = JoyKeyState::JOY_AXIS_RIGHT_PRESSED;
					}
					else if (x < (-deadZone) && y > (-deadZone * 2))
					{
						state = JoyKeyState::JOY_AXIS_LEFT_PRESSED;
					}
					else if (y > (deadZone) && x <(deadZone * 2))
					{
						state = JoyKeyState::JOY_AXIS_UP_PRESSED;
					}
					else if (y < (-deadZone) && x >(-deadZone * 2))
					{
						state = JoyKeyState::JOY_AXIS_DOWN_PRESSED;
					}
					else
					{
						state = JoyKeyState::JOY_AXIS_RELEASED;
					}

					if (m_jState[pInput->m_nSource] != state)
					{
						//printf("posting nuevo key = %d\n", state); fflush(stdout);
						m_jState[pInput->m_nSource] = state;
						Action = new CActionEvent(GamePad->first, pInput->m_ulTime, state);
						m_pHSMOwner->PostEvent(Action);
					}
				}
				
			}
		}
		//3.- Almacenar el estado anterior, que es el actual.
		if (GamePad == m_mapLastEvents.end())
			m_mapLastEvents.insert(
				make_pair(pInput->m_nSource, pCopy));
		else
		{
			delete GamePad->second;
			GamePad->second = pCopy;
		}

	}
	return 0;
}


CInputProcessor::~CInputProcessor()
{
}

void CInputProcessor::SetJoysticMode(JoysticMode pMode)
{
	if (pMode == JoysticMode::JOYSTIC || pMode == JoysticMode::KEYBOARD)
	{
		m_jMode = pMode;
		if (pMode == JoysticMode::KEYBOARD)
		{
			for (auto state : m_jState)
				state = JoyKeyState::JOY_AXIS_RELEASED;
		}
	}
	else
	{
		std::cout << "Error el modo para joystic " << pMode << " no es valido" << std::endl;
	}
}
