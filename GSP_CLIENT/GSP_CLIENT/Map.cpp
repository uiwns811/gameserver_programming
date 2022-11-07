#include "Map.h"
#include "GameFramework.h"
#include "PlayerMgr.h"
#include "Player.h"

void CMap::Init()
{
	// 맵 로딩하는 함수 추가로 만들기
	tileTex = new sf::Texture;
	tileTex->loadFromFile("Resource/tilemap4.png");
	
	//std::vector<CTile*> row1;
	//row1.reserve(W_HEIGHT);
	//row1.emplace_back(new CTile(*tileTex, TILE_HALF, TILE_HALF, 1, 0));
	//m_tilemap.emplace_back(row1);

	//std::vector<CTile*> row2;
	//row2.reserve(W_WIDTH);
	//row2.emplace_back(new CTile(*tileTex, TILE_HALF, TILE_HALF * 2, 1, 1));
	//m_tilemap.emplace_back(row2);

	for (short i = 0; i < W_HEIGHT; ++i) {
		for (short j = 0; j < W_WIDTH; ++j) {
			m_tilemap[i][j] = new CTile(*tileTex, j % 18 * TILE_HALF, i % 18 * TILE_HALF, j, i);
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

			//short x = (m_tilemap[j][i]->GetX() - left) * TILE_SIZE;
			//short y = (m_tilemap[j][i]->GetY() - top) * TILE_SIZE;
			//m_tilemap[j][i]->m_sprite.setPosition(x, y);

			DRAW(m_tilemap[i][j]->m_sprite);
		} 
	}


	//for (int i = 0; i < W_HEIGHT; ++i) {
	//	for (int j = 0; j < W_WIDTH; ++j) {
	//		DRAW(m_tilemap[i][j]->m_sprite);
	//	}
	//}
}

void CMap::CleanUp()
{
	delete tileTex;
}