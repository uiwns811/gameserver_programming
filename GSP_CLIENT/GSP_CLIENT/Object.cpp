#include "Object.h"
#include "GameFramework.h"
#include "Map.h"
#include "Network.h"

void CObject::Render(short left, short top)
{
	short x = (m_x - left) * TILE_SIZE + SCREEN_WIDTH / 2;
	short y = (m_y - top) * TILE_SIZE + SCREEN_HEIGHT / 2;
	m_sprite.setPosition((float)x, (float)y);
	DRAW(m_sprite);

	if (m_is_attack == true)
		Attack(x, y);
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