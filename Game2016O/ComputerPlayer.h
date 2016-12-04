#pragma once
#include <random>

class CPlayer;


class CComputerPlayer
{
public:

	enum class IaLevel {NORMAL, ADVANCED};

	void StartPlayer();
	void StopPlayer();

	CComputerPlayer(CPlayer *i_pOwner, IaLevel i_iaLevel = IaLevel::NORMAL);
	~CComputerPlayer();

	static DWORD WINAPI IaThread(CComputerPlayer* i_ia);

private:
	CPlayer * m_pOwner;
	IaLevel m_level;
	bool m_bRunning;
	static std::uniform_real_distribution<double> m_dist;
	static std::mt19937 m_mt;

};

