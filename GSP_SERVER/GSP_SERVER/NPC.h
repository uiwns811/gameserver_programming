#pragma once
#include "stdafx.h"
#include "Object.h"

class CNPC : public CObject
{
public:
	NPC_ATTACK_TYPE m_attack_type;
	NPC_MOVE_TYPE m_move_type;

	atomic_bool m_is_active;

	lua_State* m_L;
	mutex m_ll;
	atomic_bool m_is_run;

	unsigned m_move_time;

public:
	void Initialize();
	void Tick();
};

void InitializeNPC();