#pragma once
#include "stdafx.h"

enum S_STATE {ST_FREE, ST_ALLOC, ST_INGAME};

class CObject
{
public:
	char m_name[NAME_SIZE];
	short m_id;
	short m_x;
	short m_y;
	short m_exp;
	short m_level;
	short m_hp;

	mutex m_s_lock;
	atomic<S_STATE> m_state;

	unsigned m_move_time;
};

