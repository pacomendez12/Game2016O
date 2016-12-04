#include "stdafx.h"
#include "ComputerPlayer.h"
#include "SGame.h"
#include <cstdlib>
#include <random>
#include <climits>


std::uniform_real_distribution<double> CComputerPlayer::m_dist;
std::mt19937 CComputerPlayer::m_mt;

void CComputerPlayer::StartPlayer()
{
	DWORD dwThreadID;
	m_bRunning = true;
	CreateThread(nullptr, 4096, (LPTHREAD_START_ROUTINE)CComputerPlayer::IaThread, this, 0, &dwThreadID);
}

void CComputerPlayer::StopPlayer()
{
	m_bRunning = false;
}

CComputerPlayer::CComputerPlayer(CPlayer *i_pOwner, IaLevel i_iaLevel) :
	m_pOwner(i_pOwner), m_level(i_iaLevel), m_bRunning(false)
{
	std::random_device rd;
	std::mt19937 mt(rd());
	m_mt = mt;
	std::uniform_real_distribution<double> dist(0, (double)INT_MAX);
	m_dist = dist;
	
	
}


CComputerPlayer::~CComputerPlayer()
{
	if (m_bRunning)
	{
		StopPlayer();
	}
}

DWORD CComputerPlayer::IaThread(CComputerPlayer * i_ia)
{
	int val = m_dist(m_mt);
	auto p = i_ia->m_pOwner;
	while (i_ia->m_bRunning)
	{
		int cBarn = i_ia->m_pOwner->GetCurrentBarnChoosed();
		vector<int> goals;
		vector<int> barnsGenerator;
		for (int i = 0; i < TOTAL_BARNS; i++)
		{
			barnsGenerator.push_back(i);
		}

		// select the goal barn
		while (!barnsGenerator.empty())
		{
			int goalIdx = int(m_dist(m_mt)) % barnsGenerator.size();
			int goal = barnsGenerator[goalIdx];
			barnsGenerator[goalIdx] = barnsGenerator[barnsGenerator.size() - 1];
			barnsGenerator.pop_back();
			goals.push_back(goal);
		}

		//goals allready has the order of the goals, it will try to select one after other
		// the barns in the vector are in inverse order, back is the most wanted barn

		// Sleep some milliseconds to let the players think
		Sleep(int(m_dist(m_mt)) % 800 + 500);

		// start moving
		bool choosed = false;

		do {
			// move until the goal is reached
			MoveEnum dir;

			// move to left or to right
			dir = (int(int(m_dist(m_mt)) % 2) == 0 ? MoveEnum::LEFT : MoveEnum::RIGHT);
			while (p->GetCurrentBarnChoosed() != goals.back()) 
			{
				if (CPlayer::m_vBoardBarnsChoosed[goals.back()] == true)
				{
					goals.pop_back();
				}
				p->MoveSelector(dir);
				Sleep(int(m_dist(m_mt)) % 500 + 500);
			}
		
			// Try to choose the barn
			if (!(choosed = p->ChooseBarn()))
			{
				goals.pop_back();
			}
			
		} while (!choosed);
	}
	return 0;
}
