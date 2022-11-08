#include "Map.h"
#include "GameFramework.h"
#include "PlayerMgr.h"
#include "Player.h"

void CMap::Init()
{
	// 맵 로딩하는 함수 추가로 만들기
	tileTex = new sf::Texture;
	tileTex->loadFromFile("Resource/tilemap4.png");

	ifstream in("Resource/MapInfo.txt");
	if (in.fail())
	{
		cout << "Map 파일을 열 수 없음" << endl;
		exit(0);
	}

	for (short i = 0; i < W_HEIGHT; ++i) {
		for (short j = 0; j < W_WIDTH; ++j) {
			//int tileX, tileY;
			//in >> tileX >> tileY;
			switch (j % 20) {
			case 3:
			case 4:
			case 5:
			case 6:
			case 16:
			case 17:
			case 18:
				if (i % 5 == 0)
					m_tilemap[j][i] = new CTile(*tileTex, 5 * TILE_HALF, 1 * TILE_HALF, j, i, false);
				else 
					m_tilemap[j][i] = new CTile(*tileTex, 1 * TILE_HALF, 1 * TILE_HALF, j, i, true);
				break;
			default:
				m_tilemap[j][i] = new CTile(*tileTex, 1 * TILE_HALF, 1 * TILE_HALF, j, i, true);
				break;
			}
		}
	}
}

void CMap::Update()
{

}

void CMap::Render()
{
	if (!m_bRender) return;

	short left, top;
	CPlayerMgr::GetInst()->GetTLPos(left, top);

	for (short i = top; i < top + SCREEN_HEIGHT; ++i) {
		for (short j = left; j < left + SCREEN_WIDTH; ++j) {
			if ((i < 0) || (j < 0)) continue;
			if ((i > W_HEIGHT) || (j > W_WIDTH)) continue;

			short x = (j - left) * TILE_SIZE + SCREEN_WIDTH / 2;
			short y = (i - top) * TILE_SIZE + SCREEN_HEIGHT / 2;
			m_tilemap[j][i]->m_sprite.setPosition(x, y);

			DRAW(m_tilemap[j][i]->m_sprite);
		} 
	}
}

void CMap::CleanUp()
{
	delete tileTex;
}