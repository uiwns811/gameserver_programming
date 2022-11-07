#include "IngameScene.h"
#include "SceneMgr.h"
#include "Map.h"
#include "GameFramework.h"
#include "PlayerMgr.h"

void CIngameScene::Init()
{
	CMap::GetInst()->Init();
	CPlayerMgr::GetInst()->Init();
}

void CIngameScene::Start()
{

}

void CIngameScene::Update()
{
	CMap::GetInst()->Update();
}

void CIngameScene::Render()
{
	CMap::GetInst()->Render();
	CPlayerMgr::GetInst()->Render();
}

void CIngameScene::CleanUp()
{
	CMap::GetInst()->CleanUp();
	CPlayerMgr::GetInst()->CleanUp();
}