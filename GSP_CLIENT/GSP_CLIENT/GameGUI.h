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
	void ShowChatting();
	
	vector<CHAT> m_chat_data;

private:
	sf::RenderWindow* m_window;

	bool m_bChat = false;
	sf::Clock m_deltaClock;
};

