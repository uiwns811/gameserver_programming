#pragma once
#include "stdafx.h"

struct Attack {
	sf::Sprite m_attack_sprite;
	chrono::system_clock::time_point m_attack_time;

	void InitPlayerAttack()
	{
		sf::Texture* attackTex = new sf::Texture;
		attackTex->loadFromFile("Resource/attack.png");
		m_attack_sprite.setTexture(*attackTex);
		m_attack_sprite.setTextureRect(sf::IntRect(0, 0, TILE_SIZE, TILE_SIZE));
	}

	void InitNPCAttack()
	{
		sf::Texture* attackTex = new sf::Texture;
		attackTex->loadFromFile("Resource/m_attack.png");
		m_attack_sprite.setTexture(*attackTex);
		m_attack_sprite.setTextureRect(sf::IntRect(0, 0, TILE_SIZE, TILE_SIZE));
	}
};

class CObject
{
private:
	bool m_bActive;
	char m_name[NAME_SIZE];
	int m_id;
	short m_x;
	short m_y;
	int m_exp;
	int m_level;
	int m_hp;
	int m_maxhp;

	unsigned m_move_time;

	bool m_is_attack;
	Attack m_attack;

	sf::RectangleShape m_maxhpbar;
	sf::RectangleShape m_hpbar;

public:
	sf::Sprite m_sprite;
	sf::Text m_name_info;

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

		m_attack.InitPlayerAttack();
	}

	CObject(char* name, int id, sf::Texture& t, int left, int top, int x, int y)
	{
		m_bActive = false;
		m_is_attack = false;
		//strncpy_s(m_name, name, NAME_SIZE);
		m_id = id;
		m_sprite.setTexture(t);
		m_sprite.setTextureRect(sf::IntRect(left, top, TILE_SIZE, TILE_SIZE));
		m_x = x;
		m_y = y;
		m_sprite.setPosition(m_x, m_y);

		m_exp = 0;
		m_level = 0;
		m_hp = 0;
		m_maxhp = m_hp;

		if (m_id < MAX_USER) m_attack.InitPlayerAttack();
		else m_attack.InitNPCAttack();

		SetName(name);
	}

	CObject(char* name, int id, sf::Texture& t, int left, int top)
	{
		//strncpy_s(m_name, name, NAME_SIZE);
		m_id = id;
		m_bActive = false;
		m_is_attack = false;
		m_sprite.setTexture(t);
		m_sprite.setTextureRect(sf::IntRect(left, top, TILE_SIZE, TILE_SIZE));
		m_exp = 0;
		m_level = 0;
		m_hp = 0;
		m_maxhp = m_hp;
		
		if (m_id < MAX_USER) m_attack.InitPlayerAttack();
		else m_attack.InitNPCAttack();

		SetName(name);
	}
	
	void SetTexture(sf::Texture& t, int left, int top) { m_sprite.setTexture(t); m_sprite.setTextureRect(sf::IntRect(left, top, TILE_SIZE, TILE_SIZE)); }
	void SetActive(bool bTrue) { m_bActive = bTrue; }
	void SetName(char* name) { sprintf_s(m_name, name); SetNameInfo(m_name); };
	void SetNameInfo(const char name[]);
	void SetId(int id) { m_id = id; }
	void SetExp(int exp) { m_exp = exp; }
	void SetLevel(int level) { m_level = level; }
	void SetHp(int hp) { m_hp = hp; }
	void SetMaxHp(int maxhp) { m_maxhp = maxhp; }
	void SetX(short x) { m_x = x; }
	void SetY(short y) { m_y = y; }
	void SetStat(int exp, int level, int hp, int maxhp) { m_exp = exp; m_level = level; m_hp = hp; m_maxhp = maxhp; }

	char* GetName() { return m_name; }
	int GetId() { return m_id; }
	short GetX() { return m_x; }
	short GetY() { return m_y; }
	int GetExp() { return m_exp; }
	int GetLevel() { return m_level; }
	int GetHp() { return m_hp; }
	int GetMaxHp() { return m_maxhp; }

	void Move(short x, short y) { m_x = x; m_y = y; }
	void Move(char direction);
	void Attack(short x, short y);
	void SetAttack(bool _bTrue) { m_is_attack = _bTrue; }
	void SetAttackTime(chrono::system_clock::time_point time) { m_attack.m_attack_time = time; }

	void Render(short left, short top);
	void RenderHpBar(short x, short y);
};

