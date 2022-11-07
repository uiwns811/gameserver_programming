#pragma once
#include "stdafx.h"
#include "expover.h"
#include "NPC.h"

class SESSION : public CNPC
{
	EXP_OVER m_recv_over;

public:

	SOCKET m_socket;
	int m_prev_remain;

public:
	SESSION()
	{
		m_socket = 0;
		m_prev_remain = 0;
	}

	~SESSION() {}

	void SendPacket(void* packet);
	void Recv();

	void SendLoginOkPacket();
	void SendLoginFailPacket();
	void SendAddPlayerPacket(short c_id);
	void SendMovePlayerPacket(short c_id);
	void SendRemovePlayerPacket(short c_id);
};

