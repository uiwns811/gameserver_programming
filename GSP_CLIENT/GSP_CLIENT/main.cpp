#include "stdafx.h"
#include "GameFramework.h"
#include "SceneMgr.h"
#include "ObjectMgr.h"
#include "Network.h"
#include "GameGUI.h"

int main()
{
	CGameFramework::GetInst()->Init();

	sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "2019182026");
	CGameFramework::GetInst()->SetWindow(window);

	CGameGUI::GetInst()->Init(window);

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			ImGui::SFML::ProcessEvent(window, event);
			switch (event.type) {
			case sf::Event::Closed:
				CNetwork::GetInst()->SendLogoutPacket();
				window.close();
				break;
			case sf::Event::KeyPressed: 
			{
				int direction = -1;
				switch (event.key.code) {
				case sf::Keyboard::Left:
					direction = 2;
					break;
				case sf::Keyboard::Right:
					direction = 3;
					break;
				case sf::Keyboard::Up:
					direction = 0;
					break;
				case sf::Keyboard::Down:
					direction = 1;
					break;
				case sf::Keyboard::A:
					CNetwork::GetInst()->SendAttackPacket();
					break;
				case sf::Keyboard::Escape:
					CNetwork::GetInst()->SendLogoutPacket();
					window.close();
					break;
				}
				if (-1 != direction) {
					CNetwork::GetInst()->SendMovePacket(direction);
				}
			}
			break;
			}
		}

		CGameGUI::GetInst()->Update();
		window.clear();
		CGameFramework::GetInst()->Update();
		window.display();
	}
	CNetwork::GetInst()->SendLogoutPacket();
	CGameFramework::GetInst()->CleanUp();
	return 0;
}