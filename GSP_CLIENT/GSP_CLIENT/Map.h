#pragma once
#include "stdafx.h"

class CTile {
private:
	short m_x, m_y;
	bool m_flag;

public:
	sf::Sprite m_sprite;

public:
	CTile(sf::Texture& t, short left, short top, short x, short y, bool flag)
	{
		m_sprite.setTexture(t);
		m_sprite.setTextureRect(sf::IntRect(left, top, TILE_HALF, TILE_HALF));
		m_sprite.scale(2.f, 2.f);
		m_x = x * TILE_SIZE + SCREEN_WIDTH / 2;
		m_y = y * TILE_SIZE + SCREEN_WIDTH / 2;
		m_flag = flag;
		m_sprite.setPosition(m_x, m_y);
	}

	short GetX() { return m_x; }
	short GetY() { return m_y; }
	bool GetFlag() { return m_flag; }
};

class CMap : public TSingleton<CMap>
{
private:
	sf::Texture* tileTex;

public:
	void Init();
	void Update();
	void Render();
	void CleanUp();

	bool CanMove(short x, short y) { return m_tilemap[x][y]->GetFlag(); }

	//std::vector<std::vector<CTile*>> m_tilemap;
	CTile* m_tilemap[W_WIDTH][W_HEIGHT];
};

