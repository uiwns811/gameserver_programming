#include "Object.h"
#include "GameFramework.h"
#include "Map.h"
#include "Network.h"
#include "GameGUI.h"

void CObject::Render(short left, short top)
{
	short x = (m_x - left) * TILE_SIZE + SCREEN_WIDTH / 2;
	short y = (m_y - top) * TILE_SIZE + SCREEN_HEIGHT / 2;
	m_sprite.setPosition((float)x, (float)y);
	DRAW(m_sprite);

	//m_name_info.setPosition((float)x - TILE_HALF, (float)y - 10);
	//DRAW(m_name_info);

	if (m_is_attack == true)
		Attack(x, y);

	RenderHpBar(x, y);
}

void CObject::RenderHpBar(short x, short y)
{
	m_maxhpbar.setPosition(x - 20, y - 20);
	m_maxhpbar.setOutlineColor(sf::Color::White);
	m_maxhpbar.setFillColor(sf::Color(0, 0, 0, 100));
	m_maxhpbar.setSize(sf::Vector2f(100, 10));
	DRAW(m_maxhpbar);

	m_hpbar.setPosition(x - 20, y - 20);
	m_hpbar.setFillColor(sf::Color(255, 0, 0, 100));
	float cur_hp = (float)m_hp / (float)m_maxhp * 100;
	m_hpbar.setSize(sf::Vector2f(cur_hp, 10));
	DRAW(m_hpbar);
}

void CObject::Move(char direction)
{
	switch (direction) {
	case 0: 
		if (m_y <= 0) return;
		if (!CMap::GetInst()->CanMove(m_x, m_y - 1)) return;
		m_y--;
	break;
	case 1:
		if (m_y >= W_HEIGHT - 1) return;
		if (!CMap::GetInst()->CanMove(m_x, m_y + 1)) return;
		m_y++;
	break;
	case 2:
		if (m_x <= 0) return;
		if (!CMap::GetInst()->CanMove(m_x - 1, m_y)) return;
		m_x--;
	break;
	case 3:
		if (m_x >= W_WIDTH - 1) return;
		if (!CMap::GetInst()->CanMove(m_x + 1, m_y)) return;
		m_x++;
	break;
	}
}

void CObject::Attack(short x, short y)
{
	m_attack.m_attack_sprite.setPosition((float)(x - TILE_SIZE), (float)y);
	DRAW(m_attack.m_attack_sprite);
	m_attack.m_attack_sprite.setPosition((float)(x + TILE_SIZE), (float)y);
	DRAW(m_attack.m_attack_sprite);
	m_attack.m_attack_sprite.setPosition((float)(x), (float)(y - TILE_SIZE));
	DRAW(m_attack.m_attack_sprite);
	m_attack.m_attack_sprite.setPosition((float)(x), (float)(y + TILE_SIZE));
	DRAW(m_attack.m_attack_sprite);

	if (m_attack.m_attack_time + 1s < chrono::system_clock::now()) {
		m_is_attack = false;
	}
}

void CObject::SetNameInfo(const char name[])
{
	char nameinfo[NAME_SIZE];
	sprintf_s(nameinfo, name);
	m_name_info.setString(nameinfo);
	m_name_info.setFont(CGameGUI::GetInst()->GetFont());
	m_name_info.setCharacterSize(20);
	if (m_id < MAX_USER) m_name_info.setFillColor(sf::Color(0, 0, 255));
	else m_name_info.setFillColor(sf::Color(255, 0, 0));
	m_name_info.setStyle(sf::Text::Bold);
}