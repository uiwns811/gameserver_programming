#pragma once
#include "stdafx.h"

class CGameGUI : public TSingleton<CGameGUI>
{
public:
	void Init(sf::RenderWindow& window);
	void Update();
	void Render();
	void CleanUp();

	void ShowUserInfo();

private:
	sf::RenderWindow* m_window;
};

