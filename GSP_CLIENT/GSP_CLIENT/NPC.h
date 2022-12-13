#pragma once
#include "stdafx.h"

class CNPC
{
private:
	bool m_bActive;
	char m_name[NAME_SIZE];
	int m_id;
	short m_x;
	short m_y;
	int m_level;
	int m_hp;

	unsigned m_move_time;

	bool m_is_attack;

public:
	sf::Sprite m_sprite;

public:
	CNPC()
	{
		m_bActive = false;
		m_is_attack = false;
		m_name[0] = 0;
		m_id = -1;
		m_x = -1;
		m_y = -1;

		m_level = 0;
		m_hp = 0;
	}

	CNPC(char* name, short id, sf::Texture& t, int left, int top, int x, int y)
	{
		m_bActive = false;
		m_is_attack = false;
		strncpy_s(m_name, name, NAME_SIZE);
		m_id = id;
		m_sprite.setTexture(t);
		m_sprite.setTextureRect(sf::IntRect(left, top, TILE_SIZE, TILE_SIZE));
		m_x = x;
		m_y = y;
		m_sprite.setPosition(m_x, m_y);

		m_level = 0;
		m_hp = 0;
	}

	CNPC(sf::Texture& t, int left, int top)
	{
		m_bActive = false;
		m_is_attack = false;
		m_sprite.setTexture(t);
		m_sprite.setTextureRect(sf::IntRect(left, top, TILE_SIZE, TILE_SIZE));
		m_level = 0;
		m_hp = 0;

		//m_attack.Initialize();
	}

	void SetTexture(sf::Texture& t, int left, int top) { m_sprite.setTexture(t); m_sprite.setTextureRect(sf::IntRect(left, top, TILE_SIZE, TILE_SIZE)); }
	void SetActive(bool bTrue) { m_bActive = bTrue; }
	void SetName(char* name) { strncpy_s(m_name, name, NAME_SIZE); }
	void SetId(int id) { m_id = id; }
	void SetHp(int hp) { m_hp = hp; }
	void SetX(short x) { m_x = x; }
	void SetY(short y) { m_y = y; }

	char* GetName() { return m_name; }
	int GetId() { return m_id; }
	int GetX() { return m_x; }
	int GetY() { return m_y; }
	int GetLevel() { return m_level; }
	int GetHp() { return m_hp; }

	void Move(short x, short y) { m_x = x; m_y = y; }
	void Attack(short x, short y);
	void SetAttack(bool _bTrue) { m_is_attack = _bTrue; }
	//void SetAttackTime(chrono::system_clock::time_point time) { m_attack.m_attack_time = time; }

	void Render(short left, short top);
};