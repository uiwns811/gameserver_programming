#include "PlayerMgr.h"
#include "Player.h"
#include "Network.h"

void CPlayerMgr::Init()
{
	playerTex = new sf::Texture;
	playerTex->loadFromFile("Resource/player.png");

	for (int i = 0; i < MAX_USER; ++i) {
		players[i] = new CPlayer(*playerTex, 0, 0);
	}
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
	// if (id == m_avatarID) return;
	players[id]->Move(x, y);
}

void CPlayerMgr::Move(char direction)
{
	players[m_avatarID]->Move(direction);
	m_left = players[m_avatarID]->GetX() - SCREEN_WIDTH / 2;
	m_top = players[m_avatarID]->GetY() - SCREEN_HEIGHT / 2;
	if (m_left < 0) m_left = 0;
	if (m_left > W_WIDTH - SCREEN_WIDTH) m_left = W_WIDTH - SCREEN_WIDTH;
	if (m_top < 0) m_top = 0;
	if (m_top > W_HEIGHT - SCREEN_HEIGHT) m_top = W_HEIGHT - SCREEN_HEIGHT;

}

void CPlayerMgr::AddPlayer(char* name, short id, short x, short y)
{
	if (id > MAX_USER) return;
	players[id]->SetActive(true);
	players[id]->SetName(name);
	players[id]->Move(x, y);

}

void CPlayerMgr::RemovePlayer(short id)
{
	if (id > MAX_USER) return;
	if (0 == players.count(id)) return;
	players[id]->SetActive(false);
}