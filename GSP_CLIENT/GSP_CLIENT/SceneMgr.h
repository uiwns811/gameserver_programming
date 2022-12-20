#pragma once
#include "stdafx.h"

class CScene;

class CSceneMgr : public TSingleton<CSceneMgr>
{
public:
	CSceneMgr();
	~CSceneMgr();

public:
	void Init();
	void Update();
	void Render();
	void CleanUp();

	void ChangeScene(SCENE_TYPE scene_type);

	void SetRender(bool _bTrue) { m_bRender = _bTrue; }

private:
	CScene* m_pCurScene;
	SCENE_TYPE m_pType;
	CScene* m_scenes[(UINT)SCENE_TYPE::END];

	bool m_bRender = false;
};

