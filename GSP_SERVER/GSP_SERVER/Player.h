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

public:
	CPlayer()
	{
		m_socket = 0;
		m_prev_remain = 0;
	}

	~CPlayer() {}

	void Initialize();

	void Disconnect();

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

