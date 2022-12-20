#include "SceneMgr.h"
#include "Scene.h"
#include "LoginScene.h"
#include "IngameScene.h"

CSceneMgr::CSceneMgr() : m_pCurScene(nullptr), m_pType(SCENE_TYPE::INGAME)
{

}

CSceneMgr::~CSceneMgr()
{
	delete m_pCurScene;
}

void CSceneMgr::Init()
{
	m_scenes[(UINT)SCENE_TYPE::LOGIN] = new CLoginScene;
	m_scenes[(UINT)SCENE_TYPE::INGAME] = new CIngameScene;
	
	m_scenes[(UINT)SCENE_TYPE::LOGIN]->Init();
	m_scenes[(UINT)SCENE_TYPE::INGAME]->Init();
	
	m_pCurScene = m_scenes[(UINT)m_pType];

	m_pCurScene->Start();
}

void CSceneMgr::Update()
{
	m_pCurScene->Update();
}

void CSceneMgr::Render()
{
	if (m_bRender == false) return;
	m_pCurScene->Render();
}

void CSceneMgr::CleanUp()
{
	m_pCurScene->CleanUp();
}

void CSceneMgr::ChangeScene(SCENE_TYPE scene_type)
{
	if (m_pType == scene_type) return;
	
	if (m_pCurScene != nullptr) {
		delete m_pCurScene;
		m_pCurScene = nullptr;
	}

	switch (scene_type) {
	case SCENE_TYPE::LOGIN:
		m_pCurScene = m_scenes[(UINT)SCENE_TYPE::LOGIN];
		m_pType = SCENE_TYPE::LOGIN;
		break;
	case SCENE_TYPE::INGAME:
		m_pCurScene = m_scenes[(UINT)SCENE_TYPE::INGAME];
		m_pType = SCENE_TYPE::INGAME;
		break;
	}
	m_pCurScene->Start();
}