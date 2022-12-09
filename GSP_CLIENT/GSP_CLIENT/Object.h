#pragma once
#include "stdafx.h"

struct Attack {
	sf::Sprite m_attack_sprite;
	chrono::system_clock::time_point m_attack_time;

	void Initialize()
	{
		sf::Texture* attackTex = new sf::Texture;
		attackTex->loadFromFile("Resource/attack.png");
		m_attack_sprite.setTexture(*attackTex);
		m_attack_sprite.setTextureRect(sf::IntRect(0, 0, TILE_SIZE, TILE_SIZE));
	}
};

class CObject
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
	short m_maxhp;

	unsigned m_move_time;

	bool m_is_attack;
	Attack m_attack;

public:
	sf::Sprite m_sprite;

public:
	CObject()
	{
		m_bActive = false;
		m_is_attack = false;
		m_name[0] = 0;
		m_id = -1;
		m_x = -1;
		m_y = -1;

		m_exp = 0;
		m_level = 0;
		m_hp = 0;

		m_attack.Initialize();
	}

	CObject(char* name, short id, sf::Texture& t, int left, int top, int x, int y)
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

		m_exp = 0;
		m_level = 0;
		m_hp = 0;
		m_attack.Initialize();
	}

	CObject(sf::Texture& t, int left, int top)
	{
		m_bActive = false;
		m_is_attack = false;
		m_sprite.setTexture(t);
		m_sprite.setTextureRect(sf::IntRect(left, top, TILE_SIZE, TILE_SIZE));
		m_exp = 0;
		m_level = 0;
		m_hp = 0;
		
		m_attack.Initialize();
	}
	
	void SetTexture(sf::Texture& t, int left, int top) { m_sprite.setTexture(t); m_sprite.setTextureRect(sf::IntRect(left, top, TILE_SIZE, TILE_SIZE)); }
	void SetActive(bool bTrue) { m_bActive = bTrue; }
	void SetName(char* name) { strncpy_s(m_name, name, NAME_SIZE); }
	void SetId(short id) { m_id = id; }
	void SetExp(short exp) { m_exp = exp; }
	void SetLevel(short level) { m_level = level; }
	void SetHp(short hp) { m_hp = hp; }
	void SetMaxHp(short maxhp) { m_maxhp = maxhp; }
	void SetX(short x) { m_x = x; }
	void SetY(short y) { m_y = y; }
	void SetStat(short exp, short level, short hp, short maxhp) { m_exp = exp; m_level = level; m_hp = hp; m_maxhp = maxhp; }

	char* GetName() { return m_name; }
	short GetId() { return m_id; }
	short GetX() { return m_x; }
	short GetY() { return m_y; }
	short GetExp() { return m_exp; }
	short GetLevel() { return m_level; }
	short GetHp() { return m_hp; }
	short GetMaxHp() { return m_maxhp; }

	void Move(short x, short y) { m_x = x; m_y = y; }
	void Move(char direction);
	void Attack(short x, short y);
	void SetAttack(bool _bTrue) { m_is_attack = _bTrue; }
	void SetAttackTime(chrono::system_clock::time_point time) { m_attack.m_attack_time = time; }

	void Render(short left, short top);
};

