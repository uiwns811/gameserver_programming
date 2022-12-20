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
}

void CObjectMgr::Render()
{
	for (auto& p : players) {
		if (p.first == m_avatarID) continue;
		p.second->Render(m_left, m_top);
	}

	for (auto& n : npc) {
		n.second->Render(m_left, m_top);
	}

	players[m_avatarID]->Render(m_left, m_top);
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

void CObjectMgr::Move(int id, short x, short y)
{
	if (id < MAX_USER) {
		if (players.count(id) != 0)
			players[id]->Move(x, y);
	}
	else {
		if (npc.count(id) != 0)
			npc[id]->Move(x, y);
	}

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

void CObjectMgr::AddObject(char* name, int id, short x, short y)
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
		players[id] = new CObject(name, id, *playerTex, 0, 0);
		//players[id]->SetName(name);
		players[id]->Move(x, y);
	}
	else {
		char type = name[0];
		char new_name[NAME_SIZE];

		if (type == '0') {
			sprintf_s(new_name, "PF%s", name);
			npc[id] = new CObject(new_name, id, *npcTex1, 0, 0);
		}
		else if (type == '1') {
			sprintf_s(new_name, "PL%s", name);
			npc[id] = new CObject(new_name, id, *npcTex2, 0, 0);
		}
		else if (type == '2') {
			sprintf_s(new_name, "AF%s", name);
			npc[id] = new CObject(new_name, id, *npcTex3, 0, 0);
		}
		else if (type == '3') {
			sprintf_s(new_name, "AL%s", name);
			npc[id] = new CObject(new_name, id, *npcTex4, 0, 0);
		}
		else {
			cout << type << endl;
			sprintf_s(new_name, "AL%s", name);
			npc[id] = new CObject(new_name, id, *npcTex4, 0, 0);
		}
		//npc[id]->SetName(new_name);
		npc[id]->Move(x, y);
	}
}

void CObjectMgr::RemoveObject(int id)
{
	if (id < MAX_USER) {
		players.erase(id);
	}
	else {
		npc.erase(id);
	}
}

void CObjectMgr::Attack(int id)
{
	if (id < MAX_USER) {
		if (players.count(id) == 0) return;
		players[id]->SetAttack(true);
		players[id]->SetAttackTime(chrono::system_clock::now());
	}
	else {
		if (npc.count(id) == 0) return;
		npc[id]->SetAttack(true);
		npc[id]->SetAttackTime(chrono::system_clock::now());
	}
}

void CObjectMgr::SetAvatar(int id)
{
	m_avatarID = id; 
	char temp[NAME_SIZE] = "avatar";
	players[m_avatarID] = new CObject(temp, id, *playerTexAvatar, 0, 0);
}

void CObjectMgr::GetAvatarInfo(char* name, short& x, short& y, int& exp, int& level, int& hp, int& maxhp)
{
	strncpy_s(name, NAME_SIZE, players[m_avatarID]->GetName(), NAME_SIZE);
	x = players[m_avatarID]->GetX();
	y = players[m_avatarID]->GetY();
	exp = players[m_avatarID]->GetExp();
	level = players[m_avatarID]->GetLevel();
	hp = players[m_avatarID]->GetHp();
	maxhp = players[m_avatarID]->GetMaxHp();
}

void CObjectMgr::GetPlayerInfo(int id, char* name, short& x, short& y, int& exp, int& level, int& hp, int& maxhp)
{
	strncpy_s(name, NAME_SIZE, players[id]->GetName(), NAME_SIZE);
	x = players[id]->GetX();
	y = players[id]->GetY();
	exp = players[id]->GetExp();
	level = players[id]->GetLevel();
	hp = players[id]->GetHp();
	maxhp = players[id]->GetMaxHp();
}

void CObjectMgr::GetPlayerStat(int id, short& x, short& y, int& hp, int& maxhp)
{
	x = players[id]->GetX();
	y = players[id]->GetY();
	hp = players[id]->GetHp();
	maxhp = players[id]->GetMaxHp();

}

void CObjectMgr::SetStat(int id, int exp, int level, int hp, int maxhp)
{
	if (id < MAX_USER) {
		if (players.count(id) != 0) 
			players[id]->SetStat(exp, level, hp, maxhp);
	}
	else {
		if (npc.count(id) != 0)
			npc[id]->SetStat(exp, level, hp, maxhp);
	}
}