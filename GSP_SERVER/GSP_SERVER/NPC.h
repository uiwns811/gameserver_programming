#pragma once
#include "stdafx.h"
#include "Object.h"

class CNPC : public CObject
{
public:
	NPC_ATTACK_TYPE m_attack_type;
	NPC_MOVE_TYPE m_move_type;
	NPC_STATE m_npc_state;

	atomic_bool m_is_active;

	atomic_bool m_peace_is_attacked;

	lua_State* m_L;
	mutex m_ll;
	atomic_bool m_is_run;

	int m_target_id;

	unsigned m_move_time;

public:
	void Initialize();
	void Tick();

	void Move();
	void Attack();

	bool Agro_IsInArea();
};

void InitializeNPC();