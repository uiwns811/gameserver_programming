#include "Player.h"
#include "GameFramework.h"
#include "Map.h"

void CPlayer::Render(short left, short top)
{
	// 106, 37이면 6, 17에 표시 -> 6 * TILE_SIZE -> 6은 나머지

	short x = (m_x - left) * TILE_SIZE + SCREEN_WIDTH / 2;
	short y = (m_y - top) * TILE_SIZE + SCREEN_HEIGHT / 2;
	m_sprite.setPosition((float)x, (float)y);
	DRAW(m_sprite);
}