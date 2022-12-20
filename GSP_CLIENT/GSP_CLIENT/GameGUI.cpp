#include "GameGUI.h"
#include "GameFramework.h"
#include "ObjectMgr.h"
#include "Network.h"

void CGameGUI::Init(sf::RenderWindow& window)
{
	if (false == m_font.loadFromFile("Resource/cour.ttf")) {
		cout << "Font Loading Error!\n";
		exit(-1);
	}
}

void CGameGUI::Update()
{
}

void CGameGUI::Render()
{
	ShowUserInfo();
	ShowChatting();

	if (m_bRequested)
		ShowPartyRequested();

	ShowPartyInfo();
}

void CGameGUI::CleanUp()
{

}

void CGameGUI::ShowUserInfo()
{
	if (CObjectMgr::GetInst()->GetAvatarID() == -1) return;

	char name[NAME_SIZE];
	short x, y;
	int exp, level, hp, maxhp;
	CObjectMgr::GetInst()->GetAvatarInfo(name, x, y, exp, level, hp, maxhp);

	char nameinfo[CHAT_SIZE];
	sprintf_s(nameinfo, "name : %s", name);

	char posinfo[CHAT_SIZE];
	sprintf_s(posinfo, "pos : (%d, %d)", x, y);

	char expinfo[CHAT_SIZE];
	sprintf_s(expinfo, "exp : %d", exp);

	char levelinfo[CHAT_SIZE];
	sprintf_s(levelinfo, "level : %d", level);

	char hpinfo[CHAT_SIZE];
	sprintf_s(hpinfo, "hp : %d / %d", hp, maxhp);

	m_user_info[0].setString(nameinfo);
	m_user_info[1].setString(posinfo);
	m_user_info[2].setString(expinfo);
	m_user_info[3].setString(levelinfo);
	m_user_info[4].setString(hpinfo);

	for (int i = 0; i < 5; i++) {
		m_user_info[i].setFont(m_font);
		m_user_info[i].setCharacterSize(20);
		if (i==0) m_user_info[i].setFillColor(sf::Color(255, 255, 0));
		else m_user_info[i].setFillColor(sf::Color(255, 255, 255));
		m_user_info[i].setStyle(sf::Text::Bold);
		m_user_info[i].setPosition(10, 10 + i * 15);
		DRAW(m_user_info[i]);
	}
}

void CGameGUI::ShowChatting()
{
	/*
	ImGui::SetNextWindowPos(ImVec2(50, 950));
	ImGui::SetNextWindowSize(ImVec2(400, 300));

	ImGui::Begin("CHATTING");
	ImGui::BeginChild("chatting", ImVec2(400, 200));

	for (auto& chat : m_chat_data) {
		string name(chat.name);
		string mess(chat.mess);
		string message = "[" + name + "] : " + mess;
		ImGui::Text(message.c_str());

		if (m_chat_data.size() > 10)
			m_chat_data.erase(m_chat_data.begin());
	}
	ImGui::EndChild();

	static char sendMessage[CHAT_SIZE] = { 0 };

	ImGui::BeginChild("inputchatting", ImVec2(400, 100));

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
		if (sendMessage[0] != NULL) {
			CNetwork::GetInst()->SendChatPacket(sendMessage);
			cout << sendMessage << endl;
			ZeroMemory(&sendMessage, sizeof(sendMessage));
		}

		if (!m_bChat) {
			if (!ImGui::IsAnyItemActive())
				ImGui::SetKeyboardFocusHere(0);

			m_bChat = true;
		}
		else {
			m_bChat = false;
			ImGui::SetItemDefaultFocus();

			ImGui::SetKeyboardFocusHere(-1);
		}

	}

	ImGui::InputText("##ChatMsg", sendMessage, CHAT_SIZE);

	ImGui::EndChild();
	ImGui::End();
	*/

	if (m_chat_data.size() <= 0) return;

	for (int i = 0; i < m_chat_data.size(); i++) {
		string name(m_chat_data[i].name);
		string mess(m_chat_data[i].mess);
		string message = "[" + name + "] : " + mess;

		//m_chat[i].setString(message);
		//m_chat[i].setFont(m_font);
		//m_chat[i].setCharacterSize(20);
		//m_chat[i].setFillColor(sf::Color(255, 255, 255));
		//m_chat[i].setStyle(sf::Text::Bold);
		//m_chat[i].setPosition(10, SCREEN_WIDTH * TILE_SIZE - 200 + i * 20);

		if (m_chat_data[i].chat_time + 3s < chrono::system_clock::now()) {
			m_chat_data.erase(m_chat_data.begin() + i);
			continue;
		}

		sf::Text chat;
		chat.setString(message);
		chat.setFont(m_font);
		chat.setCharacterSize(20);
		chat.setFillColor(sf::Color(255, 255, 255));
		chat.setStyle(sf::Text::Bold);
		chat.setPosition(10, SCREEN_WIDTH * TILE_SIZE - 200 + i * 20);

		m_chat.emplace_back(chat);
	}

	for (auto& c : m_chat)
		DRAW(c);

	m_chat.clear();
}

void CGameGUI::ShowPartyRequested()
{
	if (m_requested_id == -1) return;

	string name(CObjectMgr::GetInst()->GetName(m_requested_id));

	string message = name + " invited me to a party. Accept : y / Reject : n";
	
	m_party_text.setString(message);
	m_party_text.setFont(m_font);
	m_party_text.setCharacterSize(30);
	m_party_text.setFillColor(sf::Color(255, 0, 0));
	m_party_text.setStyle(sf::Text::Bold);
	auto size = m_party_text.getGlobalBounds();
	m_party_text.setPosition(SCREEN_WIDTH * TILE_SIZE / 2 - size.width/2, 30);
	DRAW(m_party_text);
}

void CGameGUI::ShowPartyInfo()
{
	unordered_set<int> party = CObjectMgr::GetInst()->GetParty();

	if (party.empty()) return;
	
	string desc = "[name : (x, y) hp / max_hp]";
	m_party_desc.setString(desc);
	m_party_desc.setFont(m_font);
	m_party_desc.setCharacterSize(20);
	m_party_desc.setFillColor(sf::Color(255, 255, 0));
	m_party_desc.setStyle(sf::Text::Bold);
	auto size = m_party_desc.getGlobalBounds();
	m_party_desc.setPosition(SCREEN_WIDTH * TILE_SIZE - size.width, SCREEN_HEIGHT * TILE_SIZE / 3);
	DRAW(m_party_desc);

	int i = 0;
	for (auto& p : party) {
		//string name(CObjectMgr::GetInst()->GetName(p));
		char name[NAME_SIZE];
		sprintf_s(name, CObjectMgr::GetInst()->GetName(p), NAME_SIZE);
		short x, y;
		int hp, maxhp;
		CObjectMgr::GetInst()->GetPlayerStat(p, x, y, hp, maxhp);

		char partyInfo[CHAT_SIZE];
		sprintf_s(partyInfo, "%s : (%d, %d) %d / %d", name, x, y, hp, maxhp);

		m_party_info[i].setString(partyInfo);
		m_party_info[i].setFont(m_font);
		m_party_info[i].setCharacterSize(20); 
		m_party_info[i].setFillColor(sf::Color(255, 255, 255));
		m_party_info[i].setStyle(sf::Text::Bold);
		auto size = m_party_info[i].getGlobalBounds();
		m_party_info[i].setPosition(SCREEN_WIDTH * TILE_SIZE - size.width, SCREEN_HEIGHT * TILE_SIZE / 3 + i * 20 + 20);
		DRAW(m_party_info[i]);
		i++;
	}
}