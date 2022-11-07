#include "PlayerMgr.h"
#include "Player.h"
#include "Network.h"

void CPlayerMgr::Init()
{

}

void CPlayerMgr::Render()
{
	for (auto& p : players) {
		p.second->Render(m_left, m_top);
	}
}

void CPlayerMgr::CleanUp()
{
	delete playerTex;
}

void CPlayerMgr::Move(short id, short x, short y)
{
	if (id >= MAX_USER) return;

	if (id == m_avatarID) {
		players[id]->Move(x, y);
		m_left = x - SCREEN_WIDTH / 2;
		m_top = y - SCREEN_HEIGHT / 2;
	}
	else {
		players[id]->Move(x, y);
	}

	cout << x << ", " << y << endl;
}

void CPlayerMgr::GetAvaterPos(short& x, short& y) 
{ 
	if (m_avatarID == -1) return;
	if (0 == players.count(m_avatarID)) return;
	players[m_avatarID]->GetPos(x, y);
}

void CPlayerMgr::CreatePlayer(char* name, short id, short x, short y)
{
	playerTex = new sf::Texture;
	playerTex->loadFromFile("Resource/player.png");

	if (players.empty()) {
		players[id] = new CPlayer(name, id, *playerTex, 0, 0, x, y, true);
		m_avatarID = id;
	}
	else if (players.count(id)) {
		players.erase(id);
		players[id] = new CPlayer(name, id, *playerTex, 0, 0, x, y, false);
	}
	else {
		players[id] = new CPlayer(name, id, *playerTex, 0, 0, x, y, false);
	}
}

void CPlayerMgr::RemovePlayer(short id)
{
	if (0 == players.count(id)) return;
	delete players[id];
	players.erase(id);
}