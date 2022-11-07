#pragma once
#include "stdafx.h"

class CNPC
{
private:
	char m_name[NAME_SIZE];
	short m_id;
	short m_exp;
	short m_level;
	short m_hp;
	short m_x;
	short m_y;
	mutex _s_lock;
	S_STATE _state;

	unsigned m_move_time;

public:
	short GetID() { return m_id; }
	short GetX() { return m_x; }
	short GetY() { return m_y; }
	short GetExp() { return m_exp; }
	short GetLevel() { return m_level; }
	char* GetName() { return m_name; }
	unsigned GetMoveTime() { return m_move_time; }
};

