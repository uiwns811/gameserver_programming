#include "ObjectMgr.h"
#include "Network.h"

void CObjectMgr::Init()
{
	playerTexAvatar = new sf::Texture;
	playerTex = new sf::Texture;
	npcTex1 = new sf::Texture;
	npcTex2 = new sf::Texture;
	npcTex3 = new sf::Texture;
	npcTex4 = new sf::Texture;

	playerTexAvatar->loadFromFile("Resource/player1.png");
	playerTex->loadFromFile("Resource/player2.png");
	npcTex1->loadFromFile("Resource/Monster1.png");
	npcTex2->loadFromFile("Resource/Monster2.png");
	npcTex3->loadFromFile("Resource/Monster3.png");
	npcTex4->loadFromFile("Resource/Monster4.png");

	//for (int i = 0; i < MAX_USER; ++i) {
	//	players[i] = new CObject(*playerTex, 0, 0);
	//}
}

void CObjectMgr::Render()
{
	for (auto& p : players) {
		p.second->Render(m_left, m_top);
	}

	for (auto& n : npc) {
		n.second->Render(m_left, m_top);
	}
}

void CObjectMgr::CleanUp()
{
	delete playerTexAvatar;
	delete playerTex;
	delete npcTex1;
	delete npcTex2;
	delete npcTex3;
	delete npcTex4;
}

void CObjectMgr::Move(short id, short x, short y)
{
	if (id < MAX_USER)
		players[id]->Move(x, y);
	else
		npc[id]->Move(x, y);

	if (id == m_avatarID)
	{
		m_left = players[m_avatarID]->GetX() - SCREEN_WIDTH / 2;
		m_top = players[m_avatarID]->GetY() - SCREEN_HEIGHT / 2;
		if (m_left < 0) m_left = 0;
		if (m_left > W_WIDTH - SCREEN_WIDTH) m_left = W_WIDTH - SCREEN_WIDTH;
		if (m_top < 0) m_top = 0;
		if (m_top > W_HEIGHT - SCREEN_HEIGHT) m_top = W_HEIGHT - SCREEN_HEIGHT;
	}
}

void CObjectMgr::Move(char direction)
{
	players[m_avatarID]->Move(direction);

	m_left = players[m_avatarID]->GetX() - SCREEN_WIDTH / 2;
	m_top = players[m_avatarID]->GetY() - SCREEN_HEIGHT / 2;
	if (m_left < 0) m_left = 0;
	if (m_left > W_WIDTH - SCREEN_WIDTH) m_left = W_WIDTH - SCREEN_WIDTH;
	if (m_top < 0) m_top = 0;
	if (m_top > W_HEIGHT - SCREEN_HEIGHT) m_top = W_HEIGHT - SCREEN_HEIGHT;
}

void CObjectMgr::AddObject(char* name, short id, short x, short y)
{
	if (id == m_avatarID)
	{
		players[id]->SetName(name);
		players[id]->Move(x, y);

		m_left = players[m_avatarID]->GetX() - SCREEN_WIDTH / 2;
		m_top = players[m_avatarID]->GetY() - SCREEN_HEIGHT / 2;
		if (m_left < 0) m_left = 0;
		if (m_left > W_WIDTH - SCREEN_WIDTH) m_left = W_WIDTH - SCREEN_WIDTH;
		if (m_top < 0) m_top = 0;
		if (m_top > W_HEIGHT - SCREEN_HEIGHT) m_top = W_HEIGHT - SCREEN_HEIGHT;
	}
	else if (id < MAX_USER) {
		players[id] = new CObject(*playerTex, 0, 0);
		players[id]->SetName(name);
		players[id]->Move(x, y);
	}
	else {
		npc[id] = new CObject(*npcTex1, 0, 0);
		npc[id]->SetName(name);
		npc[id]->Move(x, y);
	}
	
}

void CObjectMgr::RemoveObject(short id)
{
	if (id < MAX_USER) {
		players.erase(id);
	}
	else {
		npc.erase(id);
	}
}

void CObjectMgr::Attack(short id)
{
	if (id > MAX_USER) return;
	players[id]->SetAttack(true);
	players[id]->SetAttackTime(chrono::system_clock::now());
}

void CObjectMgr::SetAvatar(short id)
{
	m_avatarID = id; 
	players[m_avatarID] = new CObject(*playerTexAvatar, 0, 0);
}

void CObjectMgr::GetAvatarInfo(char* name, short& x, short& y, short& exp, short& level, short& hp)
{
	strncpy_s(name, NAME_SIZE, players[m_avatarID]->GetName(), NAME_SIZE);
	x = players[m_avatarID]->GetX();
	y = players[m_avatarID]->GetY();
	exp = players[m_avatarID]->GetExp();
	level = players[m_avatarID]->GetLevel();
	hp = players[m_avatarID]->GetHp();
}