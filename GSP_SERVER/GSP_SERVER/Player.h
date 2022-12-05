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

	atomic_bool m_is_active;

	unsigned m_move_time;

	std::unordered_set<short> m_viewlist;
	mutex m_vl_lock;

	short m_requiredExp = START_EXP * 2;

	chrono::system_clock::time_point m_recovery_cooltime;
	chrono::system_clock::time_point m_attack_cooltime;
	bool m_is_attack;

public:
	CPlayer()
	{
		m_socket = 0;
		m_prev_remain = 0;
		m_state = ST_FREE;
	}

	~CPlayer() {}

	void Initialize();

	void Disconnect();

	void Tick();

	void CheckExpAndLevel();
	void Respawn();
	void RecoverHp();
	void Attack();
	void SetAttack(bool _bTrue) { m_is_attack = _bTrue; }

	void SendPacket(void* packet);
	void Recv();

	void CheckViewList();
	
	void SendLoginOkPacket();
	void SendLoginFailPacket();
	void SendAddPlayerPacket(short c_id);
	void SendMovePlayerPacket(short c_id);
	void SendRemovePlayerPacket(short c_id);
	void SendAttackPlayerPacket();
};

