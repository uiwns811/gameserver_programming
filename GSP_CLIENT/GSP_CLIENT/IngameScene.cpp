#include "IngameScene.h"
#include "SceneMgr.h"
#include "Map.h"
#include "GameFramework.h"
#include "ObjectMgr.h"

void CIngameScene::Init()
{
	CMap::GetInst()->Init();
	CObjectMgr::GetInst()->Init();
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
	CObjectMgr::GetInst()->Render();
}

void CIngameScene::CleanUp()
{
	CMap::GetInst()->CleanUp();
	
	CObjectMgr::GetInst()->CleanUp();
}