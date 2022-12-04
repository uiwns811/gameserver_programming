#include "GameGUI.h"
#include "GameFramework.h"
#include "PlayerMgr.h"

void CGameGUI::Init(sf::RenderWindow& window)
{
	m_window = &window;
	ImGui::SFML::Init(*m_window);
}

void CGameGUI::Update()
{
	ImGui::SFML::Update(*m_window, CGameFramework::GetInst()->GetDT().restart());

	//ImGui::Begin("User Information");
	//ImGui::Button("Look at this pretty button");


}

void CGameGUI::Render()
{
	ShowUserInfo();

	ImGui::SFML::Render(*m_window);
}

void CGameGUI::CleanUp()
{
	ImGui::SFML::Shutdown();
}

void CGameGUI::ShowUserInfo()
{
	if (CPlayerMgr::GetInst()->GetAvatarID() == -1) return;

	ImGui::Begin("User Information");
	
	char name[NAME_SIZE];
	short x, y, exp, level, hp;
	CPlayerMgr::GetInst()->GetAvatarInfo(name, x, y, exp, level, hp);
	ImGui::Text("name : %s", &name);
	ImGui::Text("pos : (%d, %d)", x, y);
	ImGui::Text("exp : %d", exp);
	ImGui::Text("level : %d", level);
	ImGui::Text("HP : %d", hp);

	ImGui::End();
}