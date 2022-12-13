#pragma once
#include "stdafx.h"
#include "Object.h"

class CObject;

class CObjectMgr : public TSingleton<CObjectMgr>
{
private:
	std::unordered_map<int, CObject*> players;
	std::unordered_map<int, CObject*> npc;
	int m_avatarID = -1;

	sf::Texture* playerTexAvatar;
	sf::Texture* playerTex;
	sf::Texture* npcTex1;
	sf::Texture* npcTex2;
	sf::Texture* npcTex3;
	sf::Texture* npcTex4;


	short m_left = 0;
	short m_top = 0;

public:
	void Init();
	void Render();
	void CleanUp();

	void Move(int id, short x, short y);
	void Move(char direction);

	void AddObject(char* name, int id, short x, short y);
	void RemoveObject(int id);

	void Attack(int id);

	void SetExp(int id, int exp) { players[id]->SetExp(exp); }
	void SetLevel(int id, int level) { players[id]->SetLevel(level); }
	void SetHP(int id, int hp) { players[id]->SetHp(hp); }
	void SetMaxHp(int id, int maxhp) { players[id]->SetMaxHp(maxhp); }
	void SetStat(int id, int exp, int level, int hp, int maxhp) { players[id]->SetStat(exp, level, hp, maxhp); }

	void SetAvatar(int id);

	int GetAvatarID() { return m_avatarID; }
	void GetAvatarInfo(char* name, short& x, short& y, int& exp, int& level, int& hp, int& maxhp);

	char* GetName(int id) { return players[id]->GetName(); }

	void GetTLPos(short& left, short& top) { left = m_left; top = m_top; }
};

