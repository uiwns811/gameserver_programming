#pragma once
#include "stdafx.h"
#include "expover.h"
#include "Object.h"

class CPlayer : public CObject
{
	EXP_OVER m_recv_over;

public:
	SOCKET m_socket;
	int m_prev_remain;

	std::unordered_set<short> m_viewlist;
	mutex m_vl_lock;

	short m_requiredExp = START_EXP * 2;
	chrono::system_clock::time_point recovery_time;

public:
	CPlayer()
	{
		m_socket = 0;
		m_prev_remain = 0;
	}

	~CPlayer() {}

	void Initialize();

	void Disconnect();

	void Tick();
	void CheckExpAndLevel();
	void Respawn();
	void RecoverHp();

	void SendPacket(void* packet);
	void Recv();

	bool CanSee(short to);
	void CheckViewList();
	
	void SendLoginOkPacket();
	void SendLoginFailPacket();
	void SendAddPlayerPacket(short c_id);
	void SendMovePlayerPacket(short c_id);
	void SendRemovePlayerPacket(short c_id);
};

