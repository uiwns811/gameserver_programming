#pragma once
#include "stdafx.h"

class CGameFramework : public TSingleton<CGameFramework>
{
public:
	CGameFramework();
	~CGameFramework();

public:
	void Init();
	void Update();
	void CleanUp();

	void SetWindow(sf::RenderWindow& window) { m_window = &window; }
	void Render(sf::Drawable& t) { m_window->draw(t); }

private:
	sf::RenderWindow* m_window;
};

