#include "GameFramework.h"
#include "Network.h"
#include "SceneMgr.h"
#include "GameGUI.h"

CGameFramework::CGameFramework()
{

}

CGameFramework::~CGameFramework()
{

}

void CGameFramework::Init()
{
	CNetwork::GetInst()->Init();
	CSceneMgr::GetInst()->Init();
}

void CGameFramework::Update()
{
	CNetwork::GetInst()->Update();
	CSceneMgr::GetInst()->Update();
	CGameGUI::GetInst()->Update();
	CSceneMgr::GetInst()->Render();
	CGameGUI::GetInst()->Render();
}

void CGameFramework::CleanUp()
{
	CGameGUI::GetInst()->CleanUp();
	CSceneMgr::GetInst()->CleanUp();
	CNetwork::GetInst()->CleanUp();
}