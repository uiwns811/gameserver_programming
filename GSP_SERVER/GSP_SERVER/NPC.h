#pragma once
#include "stdafx.h"
#include "Object.h"

class CAstar;
class CNPC : public CObject
{
public:
	NPC_ATTACK_TYPE m_attack_type;
	NPC_MOVE_TYPE m_move_type;
	NPC_STATE m_npc_state;
	NPC_TYPE m_type;

	atomic_bool m_is_active;

	lua_State* m_L;
	mutex m_ll;

	int m_target_id;

	unsigned m_move_time;

	short m_loaming_x;
	short m_loaming_y;

	int m_last_level;

	CAstar* m_astar;

public:
	void Initialize();

	void Attack();

	void SetTarget(int id) { m_target_id = id; }

	void SetDamage(int damage);
	void Die();

	void CheckTargetRange();
	bool CheckAttackRange();

	void NPCAI();

	void MoveRandom();
	void MoveToTarget();
	void Move(unordered_set<int>& old_vl);
};

void InitializeNPC();
void WakeUpNPC(int npc_id, int player_id);
void SleepNPC(int npc_id, int player_id);

int API_InitializePos(lua_State* L);