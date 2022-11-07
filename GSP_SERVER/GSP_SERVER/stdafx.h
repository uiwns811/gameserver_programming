#pragma once
#include <iostream>
#include <WS2tcpip.h>
#include <MSWSock.h>
#include <thread>
#include <array>
#include <vector>
#include <mutex>
#include "..\..\protocol.h"

#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "MSWSock.lib")
using namespace std;
constexpr int MAX_USER = 10000;
constexpr int MAX_NPC = 5000;

namespace SharedData {
	extern array<SESSION, MAX_USER + MAX_NPC> g_clients;
	extern SOCKET g_s_socket;
	extern SOCKET g_c_socket;
	extern EXP_OVER g_a_over;
}

enum S_STATE { ST_FREE, ST_ALLOC, ST_INGAME };

