#pragma once
#include "Scene.h"

class CLoginScene : public CScene
{
public:
	virtual void Init();
	virtual void Start();
	virtual void Update();
	virtual void Render();
	virtual void CleanUp();
};

