#pragma once
#include "stdafx.h"

class CPlayer
{
private:
	bool m_bIsAvatar;
	char m_name[NAME_SIZE];
	short m_id;
	short m_exp;
	short m_level;
	short m_hp;
	short m_x;
	short m_y;

	unsigned m_move_time;

public:
	sf::Sprite m_sprite;

public:
	CPlayer()
	{
		m_bIsAvatar = false;
		m_name[0] = 0;
		m_id = -1;
		m_x = -1;
		m_y = -1;

		m_exp = 0;
		m_level = 0;
		m_hp = 0;
	}

	CPlayer(char* name, short id, sf::Texture& t, int left, int top, int x, int y, bool isAvatar)
	{
		m_bIsAvatar = isAvatar;
		strncpy_s(m_name, name, NAME_SIZE);
		m_id = id;
		m_sprite.setTexture(t);
		m_sprite.setTextureRect(sf::IntRect(left, top, TILE_SIZE, TILE_SIZE));
		m_x = x;
		m_y = y;
		m_sprite.setPosition(m_x, m_y);

		m_exp = 0;
		m_level = 0;
		m_hp = 0;
	}
	
	void SetName(char* name) { strncpy_s(m_name, name, NAME_SIZE); }
	void SetId(short id) { m_id = id; }
	void SetExp(short exp) { m_exp = exp; }
	void SetHp(short hp) { m_hp = hp; }
	void Move(short x, short y) { m_x = x; m_y = y; }
	void GetPos(short& x, short& y) { x = m_x; y = m_y; }

	void Render(short left, short top);
};

