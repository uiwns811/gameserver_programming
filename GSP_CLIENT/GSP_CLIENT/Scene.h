#pragma once
#include "stdafx.h"

class CScene
{
public:
	virtual void Init();
	virtual void Start();
	virtual void Update();
	virtual void Render();
	virtual void CleanUp();
};