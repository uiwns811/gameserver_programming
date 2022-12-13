#pragma once
#include "stdafx.h"
#include "Object.h"

class CNPC : public CObject
{
public:
	NPC_ATTACK_TYPE m_attack_type;
	NPC_MOVE_TYPE m_move_type;
	atomic<NPC_STATE> m_npc_state;

	atomic_bool m_is_active;

	lua_State* m_L;
	mutex m_ll;

	int m_target_id;

	unsigned m_move_time;

	short m_loaming_x;
	short m_loaming_y;

public:
	void Initialize();

	void Attack();

	void SetTarget(int id) { m_target_id = id; }

	void SetDamage(int damage);
	void Die();

	bool Agro_IsInArea();

	void NPCAI();

	void MoveRandom();
	void MoveToTarget();
};

void InitializeNPC();
void WakeUpNPC(int npc_id, int player_id);