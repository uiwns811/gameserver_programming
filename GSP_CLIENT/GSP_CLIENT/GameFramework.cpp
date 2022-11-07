#include "GameFramework.h"
#include "Network.h"
#include "SceneMgr.h"

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
	CSceneMgr::GetInst()->Render();
}

void CGameFramework::CleanUp()
{
	CSceneMgr::GetInst()->CleanUp();
	CNetwork::GetInst()->CleanUp();
}