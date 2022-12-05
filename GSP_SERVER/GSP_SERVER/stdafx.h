#pragma once
#include <iostream>
#include <WS2tcpip.h>
#include <MSWSock.h>
#include <thread>
#include <array>
#include <vector>
#include <mutex>
#include <string>
#include <algorithm>
#include <unordered_set>
#include <concurrent_queue.h>
#include <concurrent_priority_queue.h>
#include <sqlext.h> 
#include <chrono>
#include "..\..\protocol.h"

extern "C" {
#include "include/lua.h"
#include "include/lauxlib.h"
#include "include/lualib.h"
}

#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "MSWSock.lib")
#pragma comment (lib, "lua54.lib")

using namespace std;
constexpr short MAX_USER = 10000;
constexpr short MAX_NPC = 5000;

constexpr short MAX_HP = 200;
constexpr short START_EXP = 100;

enum COMP_TYPE { OP_ACCEPT, OP_RECV, OP_SEND, OP_DB_LOGIN_WITH_INFO, OP_DB_LOGIN_NO_INFO, OP_DB_UPDATE };
enum EVENT_TYPE { EV_RANDOM_MOVE, EV_NPC_RUN, EV_DB_UPDATE };
enum S_STATE { ST_FREE, ST_ALLOC, ST_INGAME };
enum NPC_ATTACK_TYPE { NPC_PEACE, NPC_AGRO };
enum NPC_MOVE_TYPE { NPC_FIXED, NPC_LOAMING };

class CObject;
class CSector;
class EXP_OVER;
class CDataBase;
struct TIMER_EVENT;
namespace SharedData {
	extern array<CObject*, MAX_USER + MAX_NPC> g_clients;
	extern SOCKET g_listen_socket;
	extern SOCKET g_c_socket;
	extern EXP_OVER g_over;
	extern bool g_map[W_WIDTH][W_HEIGHT];
	extern HANDLE g_iocp;
	extern CSector g_sector;
	extern CDataBase g_db;
	extern concurrency::concurrent_priority_queue<TIMER_EVENT> timer_queue;
}
