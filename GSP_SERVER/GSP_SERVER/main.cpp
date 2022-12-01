#include "stdafx.h"
#include "Player.h"
#include "Sector.h"
#include "DataBase.h"
#include "Timer.h"

namespace SharedData {
	array<CPlayer, MAX_USER + MAX_NPC> g_clients;
	SOCKET g_listen_socket;
	SOCKET g_c_socket;
	EXP_OVER g_over;
	bool g_map[W_WIDTH][W_HEIGHT];		// 나중에 sector로 바꾸자!
	HANDLE g_iocp;
	CSector g_sector;
	CDataBase g_db;
	concurrency::concurrent_priority_queue<TIMER_EVENT> timer_queue;
}

void Initialize()
{
	memset(SharedData::g_map, true, sizeof(SharedData::g_map));
	SharedData::g_db.DB_Initialize();

	// 서버 지형 -> 진짜 좀 ..
	for (short i = 0; i < W_HEIGHT; ++i) {
		for (short j = 0; j < W_WIDTH; ++j) {
			if (i % 5 == 0) {
				switch (j % 20) {
				case 3:
				case 4:
				case 5:
				case 6:
				case 16:
				case 17:
				case 18:
					SharedData::g_map[j][i] = false;
					break;
				}
			}
		}
	}
}

short GetNewClientID()
{
	for (int i = 0; i < MAX_USER; ++i) {
		SharedData::g_clients[i].m_s_lock.lock();
		if (ST_FREE == SharedData::g_clients[i].m_state) {
			SharedData::g_clients[i].m_s_lock.unlock();
			return i;
		}
		SharedData::g_clients[i].m_s_lock.unlock();
	}
	return -1;
}


void do_tick()
{
	while (true) {
		for (auto& c : SharedData::g_clients) {
			c.Tick();
		}
		this_thread::sleep_for(1s);
	}
}

void ProcessPacket(int c_id, char* packet)
{
	switch (packet[1]) {
	case CS_LOGIN:
	{
		CS_LOGIN_PACKET* p = reinterpret_cast<CS_LOGIN_PACKET*>(packet);

		// 만약 이미 들어와있는 애들 중에 name 겹치는 애가 없으면 로그인 성공한다치고 DB Access
		// g_clients에 중복된 name이 있고, 걔가 지금 활동중이면 아예 로그인 못함
		for (auto& cl : SharedData::g_clients) {
			if (cl.m_id == c_id) continue;
			if (cl.m_state != ST_INGAME) continue;
			if (strcmp(cl.m_name, p->name) == 0) {
				SharedData::g_clients[c_id].SendLoginFailPacket();
				cout << c_id << " : Login Fail!!" << endl;
				return;
			}
		}

		// DB에 insert, 초기값으로 넣어주기
		strncpy_s(SharedData::g_clients[c_id].m_name, p->name, NAME_SIZE);

		SharedData::g_db.Enqueue(c_id, p->name, DB_LOGIN);

		SharedData::g_db.Enqueue(0, DB_UPDATE);

		TIMER_EVENT ev{ c_id, chrono::system_clock::now() + 60s, EV_DB_UPDATE, 0 };
		SharedData::timer_queue.push(ev);



		//SharedData::g_clients[c_id].SendLoginOkPacket();		// 여기서 DB 체크하고 LoginOk일지 LoginFail일지 판단

		//SharedData::g_clients[c_id].m_s_lock.lock();
		//SharedData::g_clients[c_id].m_state = ST_INGAME;
		//SharedData::g_clients[c_id].m_s_lock.unlock();

		//for (auto& pl : SharedData::g_clients) {
		//	if (ST_INGAME != pl.m_state) continue;
		//	if (pl.m_id == c_id) continue;
		//	if (false == SharedData::g_clients[c_id].CanSee(pl.m_id)) continue;
		//	pl.SendAddPlayerPacket(c_id);
		//	SharedData::g_clients[c_id].SendAddPlayerPacket(pl.m_id);
		//}
	}
	break;
	case CS_MOVE:
		CS_MOVE_PACKET* p = reinterpret_cast<CS_MOVE_PACKET*>(packet);
		short x = SharedData::g_clients[c_id].m_x;
		short y = SharedData::g_clients[c_id].m_y;
		short oldX = x;
		short oldY = y;
		switch (p->direction) {
		case 0: if (y > 0) y--; break;
		case 1: if (y < W_HEIGHT - 1) y++; break;
		case 2: if (x > 0) x--; break;
		case 3: if (x < W_WIDTH - 1) x++; break;
		}

		// 서버에서 한 번 더 체크 -> 갈 수 없는 곳에 갔으면 원위치로 이동
		if (SharedData::g_map[x][y]) {
			SharedData::g_clients[c_id].m_x = x;
			SharedData::g_clients[c_id].m_y = y;
			SharedData::g_clients[c_id].m_move_time = p->move_time;

			SharedData::g_sector.UpdateSector(c_id, x, y, oldX, oldY);
			SharedData::g_clients[c_id].CheckViewList();
			SharedData::g_clients[c_id].SendMovePlayerPacket(c_id);
		}
		break;
	}
}

void WorkerThread()
{
	while (true) {
		DWORD num_bytes;
		ULONG_PTR key;
		WSAOVERLAPPED* over = nullptr;
		BOOL ret = GetQueuedCompletionStatus(SharedData::g_iocp, &num_bytes, &key, &over, INFINITE);
		EXP_OVER* exp_over = reinterpret_cast<EXP_OVER*>(over);
		short client_id = static_cast<short>(key);
		if (FALSE == ret) {
			if (exp_over->op_type == OP_ACCEPT) {
				cout << "Accept Error" << endl;
			}
			else {
				cout << "Disconnect (GQCS Error) in [" << client_id << "]\n";
				SharedData::g_clients[client_id].Disconnect();
				if (exp_over->op_type == OP_SEND) delete exp_over;
				continue;
			}
		}

		switch (exp_over->op_type) {
		case OP_ACCEPT:
		{
			short new_id = GetNewClientID();
			if (new_id != -1) {
				SharedData::g_clients[new_id].m_id = new_id;
				SharedData::g_clients[new_id].Initialize();
				SharedData::g_clients[new_id].m_socket = SharedData::g_c_socket;
				CreateIoCompletionPort(reinterpret_cast<HANDLE>(SharedData::g_c_socket), SharedData::g_iocp, new_id, 0);
				SharedData::g_clients[new_id].Recv();
				SharedData::g_c_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
			}
			else {
				cout << "Accept Error.\n";
			}
			ZeroMemory(&SharedData::g_over.over, sizeof(SharedData::g_over.over));
			int addr_size = sizeof(SOCKADDR_IN);
			AcceptEx(SharedData::g_listen_socket, SharedData::g_c_socket, SharedData::g_over.sendbuf, 0, addr_size + 16, addr_size + 16, 0, &SharedData::g_over.over);
		}
		break;
		case OP_RECV:
		{
			int remain_data = num_bytes + SharedData::g_clients[client_id].m_prev_remain;
			char* p = exp_over->sendbuf;
			while (remain_data > 0) {
				int packet_size = p[0];
				if (packet_size <= remain_data) {
					ProcessPacket(static_cast<int>(key), p);
					p = p + packet_size;
					remain_data = remain_data - packet_size;
				}
				else break;
			}
			SharedData::g_clients[client_id].m_prev_remain = remain_data;
			if (remain_data > 0) {
				memcpy(exp_over->sendbuf, p, remain_data);
			}
			SharedData::g_clients[client_id].Recv();
		}
		break;
		case OP_SEND:
			delete exp_over;
		break;
		case OP_DB_LOGIN_WITH_INFO:
		{
			CObject* user_info = reinterpret_cast<CObject*>(exp_over->sendbuf);
			SharedData::g_clients[client_id].m_x = user_info->m_x;
			SharedData::g_clients[client_id].m_y = user_info->m_y;
			SharedData::g_clients[client_id].m_exp = user_info->m_exp;
			SharedData::g_clients[client_id].m_level = user_info->m_level;
			SharedData::g_clients[client_id].m_hp = user_info->m_hp;
			SharedData::g_clients[client_id].m_s_lock.lock();
			SharedData::g_clients[client_id].m_state = ST_INGAME;
			SharedData::g_clients[client_id].m_is_active = true;
			SharedData::g_clients[client_id].m_s_lock.unlock();
			SharedData::g_sector.InsertSector(client_id, SharedData::g_clients[client_id].m_x, SharedData::g_clients[client_id].m_y);
			SharedData::g_clients[client_id].SendLoginOkPacket();
			for (auto& pl : SharedData::g_clients) {
				{
					lock_guard<mutex> ll(pl.m_s_lock);
					if (ST_INGAME != pl.m_state) continue;
				}
				if (pl.m_id == client_id) continue;
				if (SharedData::g_clients[client_id].CanSee(pl.m_id) == false) continue;
				pl.SendAddPlayerPacket(client_id);
				SharedData::g_clients[client_id].SendAddPlayerPacket(pl.m_id);
			}
			delete exp_over;
		}
		break;
		case OP_DB_LOGIN_NO_INFO:
		{
			SharedData::g_sector.InsertSector(client_id, SharedData::g_clients[client_id].m_x, SharedData::g_clients[client_id].m_y);
			SharedData::g_clients[client_id].SendLoginOkPacket();
			for (auto& pl : SharedData::g_clients) {
				{
					lock_guard<mutex> ll(pl.m_s_lock);
					if (ST_INGAME != pl.m_state) continue;
				}
				if (pl.m_id == client_id) continue;
				if (SharedData::g_clients[client_id].CanSee(pl.m_id) == false) continue;
				pl.SendAddPlayerPacket(client_id);
				SharedData::g_clients[client_id].SendAddPlayerPacket(pl.m_id);
			}
		}
		break;
		case OP_DB_UPDATE:
			SharedData::g_db.Enqueue(0, DB_UPDATE);

			TIMER_EVENT ev{ key, chrono::system_clock::now() + 30s, EV_DB_UPDATE, 0 };			// 30초마다 저장
			SharedData::timer_queue.push(ev);
			break;
		}
	}
}

int main()
{
	WSADATA WSAData;

	WSAStartup(MAKEWORD(2, 2), &WSAData);
	SharedData::g_listen_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	SOCKADDR_IN server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT_NUM);
	server_addr.sin_addr.S_un.S_addr = INADDR_ANY;

	bind(SharedData::g_listen_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));

	listen(SharedData::g_listen_socket, SOMAXCONN);

	SOCKADDR_IN cl_addr;
	int addr_size = sizeof(cl_addr);
	int client_id = 0;

	SharedData::g_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(SharedData::g_listen_socket), SharedData::g_iocp, 9999, 0);

	// 맵 하자 섹터까지 + g_map에 flag 넣는거부터

	Initialize();
	
	SharedData::g_c_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	SharedData::g_over.op_type = OP_ACCEPT;
	AcceptEx(SharedData::g_listen_socket, SharedData::g_c_socket, SharedData::g_over.sendbuf, 0, addr_size + 16, addr_size + 16, 0, &SharedData::g_over.over);

	vector <thread> worker_threads;
	int num_threads = std::thread::hardware_concurrency();
	for (int i = 0; i < num_threads - 3; ++i)
		worker_threads.emplace_back(WorkerThread);
	thread db_thread([&]() {SharedData::g_db.DB_Thread(); });
	thread timer_thread{ do_timer };
	thread tick_thread{ do_tick };

	tick_thread.join();
	timer_thread.join();
	db_thread.join();
	for (auto& th : worker_threads)
		th.join();

	closesocket(SharedData::g_listen_socket);
	WSACleanup();
}