#pragma once
#include "stdafx.h"

class CPlayer;

class CPlayerMgr : public TSingleton<CPlayerMgr>
{
private:
	std::unordered_map<int, CPlayer*> players;
	short m_avatarID = -1;

	sf::Texture* playerTex1;
	sf::Texture* playerTex2;
	sf::Texture* playerTex3;
	short m_left = 0;
	short m_top = 0;

public:
	void Init();
	void Render();
	void CleanUp();

	void Move(short id, short x, short y);		
	void Move(char direction);

	void AddPlayer(char* name, short id, short x, short y);
	void RemovePlayer(short id);

	void SetAvatar(short id);

	void GetTLPos(short& left, short& top) { left = m_left; top = m_top; }
};

