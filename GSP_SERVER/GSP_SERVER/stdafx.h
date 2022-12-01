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

#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "MSWSock.lib")
using namespace std;
constexpr short MAX_USER = 10000;
constexpr short MAX_NPC = 5000;

constexpr short MAX_HP = 200;
constexpr short START_EXP = 100;

enum COMP_TYPE { OP_ACCEPT, OP_RECV, OP_SEND, OP_DB_LOGIN_WITH_INFO, OP_DB_LOGIN_NO_INFO, OP_DB_UPDATE };
enum EVENT_TYPE { EV_RANDOM_MOVE, EV_NPC_RUN, EV_DB_UPDATE };

class CPlayer;
class CSector;
class EXP_OVER;
class CDataBase;
struct TIMER_EVENT;
namespace SharedData {
	extern array<CPlayer, MAX_USER + MAX_NPC> g_clients;
	extern SOCKET g_listen_socket;
	extern SOCKET g_c_socket;
	extern EXP_OVER g_over;
	extern bool g_map[W_WIDTH][W_HEIGHT];
	extern HANDLE g_iocp;
	extern CSector g_sector;
	extern CDataBase g_db;
	extern concurrency::concurrent_priority_queue<TIMER_EVENT> timer_queue;
}
