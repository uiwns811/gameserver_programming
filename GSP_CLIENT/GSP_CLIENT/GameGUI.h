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
	
	void SetRequestedId(int id) { m_requested_id = id; }
	int GetRequestedId() { return m_requested_id; }
	void SetShowPartyRequested(bool _bTrue) { m_bRequested = _bTrue; }
	void ShowPartyRequested();
	void ShowPartyInfo();

	vector<CHAT> m_chat_data;

	sf::Font GetFont() { return m_font; }
private:
	sf::RenderWindow* m_window;
	sf::Font m_font;

	// user info
	sf::Text m_user_info[5];

	// chatting
	vector<sf::Text> m_chat;
	bool m_bChat = false;

	// party
	sf::Text m_party_text;
	sf::Text m_party_desc;
	sf::Text m_party_info[4];

	bool m_bRequested = false;
	int m_requested_id = -1;
};

