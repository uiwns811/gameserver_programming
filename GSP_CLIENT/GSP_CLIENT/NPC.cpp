#include "NPC.h"
#include "GameFramework.h"

void CNPC::Render(short left, short top)
{
	if (!m_bActive) return;

	short x = (m_x - left) * TILE_SIZE + SCREEN_WIDTH / 2;
	short y = (m_y - top) * TILE_SIZE + SCREEN_HEIGHT / 2;
	m_sprite.setPosition((float)x, (float)y);
	DRAW(m_sprite);

	if (m_is_attack == true)
		Attack(x, y);
}


void CNPC::Attack(short x, short y)
{
	//m_attack.m_attack_sprite.setPosition((float)(x), (float)y);
	//DRAW(m_attack.m_attack_sprite);

	//if (m_attack.m_attack_time + 1s < chrono::system_clock::now()) {
	//	m_is_attack = false;
	//}
}