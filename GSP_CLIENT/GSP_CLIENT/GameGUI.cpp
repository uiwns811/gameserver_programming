#include "GameGUI.h"
#include "GameFramework.h"
#include "ObjectMgr.h"
#include "Network.h"

void CGameGUI::Init(sf::RenderWindow& window)
{
	m_window = &window;
	ImGui::SFML::Init(*m_window);
	ImGuiIO& io = ImGui::GetIO();
}

void CGameGUI::Update()
{
	ImGui::SFML::Update(*m_window, m_deltaClock.restart());

}

void CGameGUI::Render()
{
	ShowUserInfo();
	ShowChatting();

	ImGui::SFML::Render(*m_window);
}

void CGameGUI::CleanUp()
{
	ImGui::SFML::Shutdown();
}

void CGameGUI::ShowUserInfo()
{
	if (CObjectMgr::GetInst()->GetAvatarID() == -1) return;

	ImGui::Begin("User Information");
	
	char name[NAME_SIZE];
	short x, y, exp, level, hp, maxhp;
	CObjectMgr::GetInst()->GetAvatarInfo(name, x, y, exp, level, hp, maxhp);
	ImGui::Text("name : %s", &name);
	ImGui::Text("pos : (%d, %d)", x, y);
	ImGui::Text("exp : %d", exp);
	ImGui::Text("level : %d", level);
	ImGui::Text("HP : %d / %d", hp, maxhp);

	ImGui::End();
}

void CGameGUI::ShowChatting()
{
	ImGui::SetNextWindowPos(ImVec2(50, 900));

	ImGui::Begin("CHATTING");
	ImGui::BeginChild("chatting", ImVec2(10, 30));

	if (m_chat_data.empty() == false)
	{
		char* mess = m_chat_data.front();
		cout << mess << endl;
		ImGui::Text(mess);
		m_chat_data.pop();
	}
	ImGui::EndChild();

	static char sendMessage[CHAT_SIZE] = { 0 };


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
			//ImGui::SetItemDefaultFocus();

			ImGui::SetKeyboardFocusHere(-1);
		}
	}

	if (ImGui::InputText("##ChatMsg", sendMessage, CHAT_SIZE)) {
		cout << sendMessage << endl;
	}
	ImGui::ShowDebugLogWindow();

	ImGui::End();
}