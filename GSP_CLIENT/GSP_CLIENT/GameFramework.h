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
	template<typename T> 
	void Render(T& t) { m_window->draw(t); }
	void CloseWindow() { m_window->close(); }

	//sf::Clock GetDT() { return m_deltaClock; }

private:
	sf::RenderWindow* m_window;
};

