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
#include <queue>
#include <sqlext.h> 
#include <chrono>
#include <list>
#include <fstream>
#include "..\..\protocol_2022.h"

extern "C" {
#include "include/lua.h"
#include "include/lauxlib.h"
#include "include/lualib.h"
}

#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "MSWSock.lib")
#pragma comment (lib, "lua54.lib")

using namespace std;

constexpr int MAX_HP = 1000;
constexpr int START_EXP = 100;
constexpr int AGRO_AREA = 5;
constexpr int LOAMING_AREA = 10;
constexpr int START_X = 1002;
constexpr int START_Y = 1002;

enum COMP_TYPE { OP_ACCEPT, OP_RECV, OP_SEND, OP_DB_LOGIN_WITH_INFO, OP_DB_LOGIN_NO_INFO, OP_DB_UPDATE, OP_PLAYER_HEAL, OP_RESPAWN_NPC, OP_NPC_AI};
enum EVENT_TYPE { EV_DB_UPDATE, EV_PLAYER_HEAL, EV_RESPAWN_NPC, EV_NPC_AI};
enum S_STATE { ST_FREE, ST_ALLOC, ST_INGAME };

enum NPC_ATTACK_TYPE { NPC_PEACE, NPC_AGRO };
enum NPC_MOVE_TYPE { NPC_FIXED, NPC_LOAMING };
enum NPC_STATE {NPC_IDLE, NPC_MOVE, NPC_ATTACK };
enum NPC_TYPE {NPC_PEACE_FIXED, NPC_PEACE_LOAMING, NPC_AGRO_FIXED, NPC_AGRO_LOAMING};

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

struct TIMER_EVENT {
	int obj_id;
	chrono::system_clock::time_point wakeup_time;
	EVENT_TYPE event_id;
	int target_id;
	constexpr bool operator < (const TIMER_EVENT& L) const
	{
		return (wakeup_time > L.wakeup_time);
	}
};