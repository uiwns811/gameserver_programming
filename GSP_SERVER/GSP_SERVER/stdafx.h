#pragma once
#include <iostream>
#include <WS2tcpip.h>
#include <MSWSock.h>
#include <thread>
#include <array>
#include <vector>
#include <mutex>
#include <unordered_set>
#include "..\..\protocol.h"

#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "MSWSock.lib")
using namespace std;
constexpr auto MAX_USER = 10000;
constexpr auto MAX_NPC = 5000;

class CPlayer;
class EXP_OVER;
namespace SharedData {
	extern array<CPlayer, MAX_USER + MAX_NPC> g_clients;
	extern SOCKET g_listen_socket;
	extern SOCKET g_c_socket;
	extern EXP_OVER g_over;
}
