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
	
	void AddChat(char* mess) { m_chat_data.push(mess); }

private:
	sf::RenderWindow* m_window;

	bool m_bChat = false;
	queue<char*> m_chat_data;


	sf::Clock m_deltaClock;
};

