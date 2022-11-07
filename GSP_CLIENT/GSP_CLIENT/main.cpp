#include "stdafx.h"
#include "GameFramework.h"
#include "SceneMgr.h"
#include "PlayerMgr.h"
#include "Network.h"

int main()
{
	CGameFramework::GetInst()->Init();

	sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "2019182026");
	CGameFramework::GetInst()->SetWindow(window);

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			switch (event.type) {
			case sf::Event::Closed:
				window.close();
				break;
			case sf::Event::KeyPressed:
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
				case sf::Keyboard::Escape:
					window.close();
					break;
				}
				if (-1 != direction) {
					CS_MOVE_PACKET p;
					p.size = sizeof(CS_MOVE_PACKET);
					p.type = CS_MOVE;
					p.direction = direction;
					CNetwork::GetInst()->SendPacket(&p);
				}
			}
		}
		window.clear();
		CGameFramework::GetInst()->Update();
		window.display();
	}
	CGameFramework::GetInst()->CleanUp();
	return 0;
}