#pragma once

// #define SFML_STATIC 1
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <SFML/System/Clock.hpp>
#include <iostream>
using namespace std;

#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4996)

#ifdef _DEBUG
#pragma comment (lib, "lib/sfml-graphics-s-d.lib")
#pragma comment (lib, "lib/sfml-window-s-d.lib")
#pragma comment (lib, "lib/sfml-system-s-d.lib")
#pragma comment (lib, "lib/sfml-network-s-d.lib")
#else
#pragma comment (lib, "lib/sfml-graphics-s.lib")
#pragma comment (lib, "lib/sfml-window-s.lib")
#pragma comment (lib, "lib/sfml-system-s.lib")
#pragma comment (lib, "lib/sfml-network-s.lib")
#endif																									
#pragma comment (lib, "opengl32.lib")
#pragma comment (lib, "winmm.lib")
#pragma comment (lib, "ws2_32.lib")

#include <imgui/imgui.h>
#include <imgui/imconfig.h>
#include <imgui/imgui-SFML.h>
#include <imgui/imconfig-SFML.h>

#include <WS2tcpip.h>
#include <vector>
#include <unordered_map>
#include <string.h>
#include <fstream>
#include <chrono>
#include <queue>
#include <string.h>

#include "..\..\protocol_2022.h"

constexpr auto SCREEN_WIDTH = 20;
constexpr auto SCREEN_HEIGHT = 20;
constexpr auto TILE_SIZE = 64.0f;
constexpr auto TILE_HALF = TILE_SIZE / 2;
constexpr auto WINDOW_WIDTH = SCREEN_WIDTH * TILE_SIZE;
constexpr auto WINDOW_HEIGHT = SCREEN_WIDTH * TILE_SIZE;

#define DRAW(t) CGameFramework::GetInst()->Render(t);

enum class SCENE_TYPE {
	LOGIN,
	INGAME,
	END,
};

template<typename T>
class TSingleton
{
protected:
	TSingleton()
	{

	}
	~TSingleton()
	{

	}

private:
	static T* m_pInst;

public:
	static T* GetInst()
	{
		if (m_pInst == NULL)
			m_pInst = new T;
		return m_pInst;
	}
	static void DestroyInst()
	{
		if (m_pInst)
		{
			delete m_pInst;
			m_pInst = NULL;
		}
	}
};

template <typename T>
T* TSingleton<T>::m_pInst = nullptr;