#pragma once
#include "stdafx.h"

class CPlayer;

class CPlayerMgr : public TSingleton<CPlayerMgr>
{
private:
	std::unordered_map<int, CPlayer*> players;
	short m_avatarID = -1;

	sf::Texture* playerTex;
	short m_left = 0;
	short m_top = 0;

public:
	void Init();
	void Render();
	void CleanUp();

	void Move(short id, short x, short y);		

	void GetAvaterPos(short& x, short& y);

	void CreatePlayer(char* name, short id, short x, short y);
	void RemovePlayer(short id);

	void GetTLPos(short& left, short& top) { left = m_left; top = m_top; }
};

