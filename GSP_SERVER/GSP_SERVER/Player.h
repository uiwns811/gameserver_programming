#pragma once
#include "stdafx.h"
#include "expover.h"
#include "NPC.h"

class CPlayer : public CNPC
{
	EXP_OVER m_recv_over;

public:
	SOCKET m_socket;
	int m_prev_remain;

	std::unordered_set<int> m_viewlist;
	mutex m_vl_lock;

public:
	CPlayer()
	{
		m_socket = 0;
		m_prev_remain = 0;
	}

	~CPlayer() {}

	void SendPacket(void* packet);
	void Recv();

	bool CanSee(short to);
	
	void SendLoginOkPacket();
	void SendLoginFailPacket();
	void SendAddPlayerPacket(short c_id);
	void SendMovePlayerPacket(short c_id);
	void SendRemovePlayerPacket(short c_id);
};

