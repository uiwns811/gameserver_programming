#include "Map.h"
#include "GameFramework.h"
#include "ObjectMgr.h"

void CMap::Init()
{
	// 맵 로딩하는 함수 추가로 만들기
	tileTex = new sf::Texture;
	tileTex->loadFromFile("Resource/tilemap4.png");

	ifstream fin("Resource/Map.txt");
	if (fin.fail())
	{
		cout << "Map 파일을 열 수 없음" << endl;
		exit(0);
	}

	for (int i = 0; i < 200; i++) {
		for (int j = 0; j < 200; j++) {
			char s_tileInfo[NAME_SIZE];
			fin >> s_tileInfo;
			int tileInfo = stoi(s_tileInfo);

			short top = tileInfo / 23;
			short left = tileInfo - 1 - (top * 23);

			for (int yCnt = 0; yCnt < 10; yCnt++) {
				for (int xCnt = 0; xCnt < 10; xCnt++) {
					if (tileInfo == 48)
						m_tilemap[j + 200 * xCnt][i + 200 * yCnt] = new CTile(*tileTex, left * TILE_HALF, top * TILE_HALF, j + 200 * xCnt, i + 200 * yCnt, true);
					else
						m_tilemap[j + 200 * xCnt][i + 200 * yCnt] = new CTile(*tileTex, left * TILE_HALF, top * TILE_HALF, j + 200 * xCnt, i + 200 * yCnt, false);
				}
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
	CObjectMgr::GetInst()->GetTLPos(left, top);

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