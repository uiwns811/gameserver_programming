#pragma once
#include "Scene.h"

class CObject;

class CIngameScene : public CScene
{
public:
	virtual void Init();
	virtual void Start();
	virtual void Update();
	virtual void Render();
	virtual void CleanUp();
};

