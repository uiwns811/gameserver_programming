#include "GameGUI.h"
#include "GameFramework.h"
#include "ObjectMgr.h"
#include "Network.h"

void CGameGUI::Init(sf::RenderWindow& window)
{
	m_window = &window;
	ImGui::SFML::Init(*m_window);

	//ImGuiIO& IO = ImGui::GetIO();

	//IO.Fonts->Clear();
	//IO.Fonts->AddFontFromFileTTF("Resource/cour.ttf", 12.f, NULL, IO.Fonts->GetGlyphRangesKorean());

	//ImGui::SFML::UpdateFontTexture();
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
	short x, y;
	int exp, level, hp, maxhp;
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

	ImGui::Dummy(ImVec2(0, 0.5));

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
			ImGui::SetItemDefaultFocus();

			ImGui::SetKeyboardFocusHere(-1);
		}
	}

	if (ImGui::InputText("##ChatMsg", sendMessage, CHAT_SIZE)) {

	}

	ImGui::End();
}