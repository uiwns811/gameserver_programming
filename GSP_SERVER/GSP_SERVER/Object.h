#pragma once
#include "stdafx.h"

class CObject
{
public:
	char m_name[NAME_SIZE];
	int m_id;
	short m_x;
	short m_y;
	int m_exp;
	int m_level;
	int m_hp;
	int m_maxhp;

	mutex m_s_lock;
	atomic<S_STATE> m_state;

	unsigned m_move_time;
};

