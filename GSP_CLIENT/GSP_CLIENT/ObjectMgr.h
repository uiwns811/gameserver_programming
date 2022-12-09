#pragma once
#include "stdafx.h"
#include "Object.h"

class CObject;

class CObjectMgr : public TSingleton<CObjectMgr>
{
private:
	std::unordered_map<short, CObject*> players;
	std::unordered_map<short, CObject*> npc;
	short m_avatarID = -1;

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

	void Move(short id, short x, short y);		
	void Move(char direction);

	void AddObject(char* name, short id, short x, short y);
	void RemoveObject(short id);

	void Attack(short id);

	void SetExp(short id, short exp) { players[id]->SetExp(exp); }
	void SetLevel(short id, short level) { players[id]->SetLevel(level); }
	void SetHP(short id, short hp) { players[id]->SetHp(hp); }
	void SetMaxHp(short id, short maxhp) { players[id]->SetMaxHp(maxhp); }
	void SetStat(short id, short exp, short level, short hp, short maxhp) { players[id]->SetStat(exp, level, hp, maxhp); }

	void SetAvatar(short id);

	short GetAvatarID() { return m_avatarID; }
	void GetAvatarInfo(char* name, short& x, short& y, short& exp, short& level, short& hp, short& maxhp);

	void GetTLPos(short& left, short& top) { left = m_left; top = m_top; }
};

