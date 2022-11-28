#pragma once
#include "stdafx.h"

class CPlayer
{
private:
	bool m_bActive;
	char m_name[NAME_SIZE];
	short m_id;
	short m_x;
	short m_y;
	short m_exp;
	short m_level;
	short m_hp;

	unsigned m_move_time;

public:
	sf::Sprite m_sprite;

public:
	CPlayer()
	{
		m_bActive = false;
		m_name[0] = 0;
		m_id = -1;
		m_x = -1;
		m_y = -1;

		m_exp = 0;
		m_level = 0;
		m_hp = 0;
	}

	CPlayer(char* name, short id, sf::Texture& t, int left, int top, int x, int y)
	{
		m_bActive = false;
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

	CPlayer(sf::Texture& t, int left, int top)
	{
		m_bActive = false;
		m_sprite.setTexture(t);
		m_sprite.setTextureRect(sf::IntRect(left, top, TILE_SIZE, TILE_SIZE));
		m_exp = 0;
		m_level = 0;
		m_hp = 0;
	}
	
	void SetTexture(sf::Texture& t, int left, int top) { m_sprite.setTexture(t); m_sprite.setTextureRect(sf::IntRect(left, top, TILE_SIZE, TILE_SIZE)); }
	void SetActive(bool bTrue) { m_bActive = bTrue; }
	void SetName(char* name) { strncpy_s(m_name, name, NAME_SIZE); }
	void SetId(short id) { m_id = id; }
	void SetExp(short exp) { m_exp = exp; }
	void SetHp(short hp) { m_hp = hp; }
	void SetX(short x) { m_x = x; }
	void SetY(short y) { m_y = y; }
	void Move(short x, short y) { m_x = x; m_y = y; }
	void Move(char direction);

	short GetX() { return m_x; }
	short GetY() { return m_y; }

	void Render(short left, short top);
};

