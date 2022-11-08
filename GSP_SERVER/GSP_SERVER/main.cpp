#include "stdafx.h"
#include "Player.h"

namespace SharedData {
	array<CPlayer, MAX_USER + MAX_NPC> g_clients;
	SOCKET g_listen_socket;
	SOCKET g_c_socket;
	EXP_OVER g_over;
	bool g_map[W_WIDTH][W_HEIGHT];
}

void Disconnect(short c_id)
{
	//SharedData::g_clients[c_id].m_s_lock.lock();
	//unordered_set<short> vl = SharedData::g_clients[c_id].m_viewlist;
	//SharedData::g_clients[c_id].m_s_lock.unlock();

	//for (auto& id : vl)
	//{
	//	auto& cl = SharedData::g_clients[id];
	//	if (cl.m_id == c_id) continue;
	//	if (ST_INGAME != cl.m_state) continue;
	//	cl.SendRemovePlayerPacket(c_id);
	//}
	//
	//closesocket(SharedData::g_clients[c_id].m_socket);
	//
	//SharedData::g_clients[c_id].m_s_lock.lock();
	//SharedData::g_clients[c_id].m_state = ST_FREE;
	//SharedData::g_clients[c_id].m_s_lock.unlock();
}

void Initialize()
{
	memset(SharedData::g_map, true, sizeof(SharedData::g_map));
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

void ProcessPacket(int c_id, char* packet)
{
	switch (packet[1]) {
	case CS_LOGIN:
	{
		CS_LOGIN_PACKET* p = reinterpret_cast<CS_LOGIN_PACKET*>(packet);
		strncpy_s(SharedData::g_clients[c_id].m_name, p->name, NAME_SIZE);
		SharedData::g_clients[c_id].SendLoginOkPacket();		// 여기서 DB 체크하고 LoginOk일지 LoginFail일지 판단

		SharedData::g_clients[c_id].m_s_lock.lock();
		SharedData::g_clients[c_id].m_state = ST_INGAME;
		SharedData::g_clients[c_id].m_s_lock.unlock();

		for (auto& pl : SharedData::g_clients) {
			if (ST_INGAME != pl.m_state) continue;
			if (pl.m_id == c_id) continue;
			if (false == SharedData::g_clients[c_id].CanSee(pl.m_id)) continue;
			pl.SendAddPlayerPacket(c_id);
			SharedData::g_clients[c_id].SendAddPlayerPacket(pl.m_id);
		}
	}
	break;
	case CS_MOVE:
	{
		CS_MOVE_PACKET* p = reinterpret_cast<CS_MOVE_PACKET*>(packet);
		short x = SharedData::g_clients[c_id].m_x;
		short y = SharedData::g_clients[c_id].m_y;
		switch (p->direction) {
		case 0: if (y > 0) y--; break;
		case 1: if (y < W_HEIGHT - 1) y++; break;
		case 2: if (x > 0) x--; break;
		case 3: if (x < W_WIDTH - 1) x++; break;
		}
		SharedData::g_clients[c_id].m_x = x;
		SharedData::g_clients[c_id].m_y = y;
		SharedData::g_clients[c_id].m_move_time = p->move_time;

		// 서버에서도 충돌처리하구.. 
		// SharedData::g_clients[c_id].SendMovePlayerPacket(c_id);

		SharedData::g_clients[c_id].CheckViewList();
	}
		break;
	}
}

void WorkerThread(HANDLE h_iocp)
{
	while (true) {
		DWORD num_bytes;
		ULONG_PTR key;
		WSAOVERLAPPED* over = nullptr;
		BOOL ret = GetQueuedCompletionStatus(h_iocp, &num_bytes, &key, &over, INFINITE);
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
				{
					SharedData::g_clients[new_id].m_s_lock.lock();
					SharedData::g_clients[new_id].m_state = ST_ALLOC;
					SharedData::g_clients[new_id].m_s_lock.unlock();
				}
				SharedData::g_clients[new_id].m_x = rand() % 20;
				SharedData::g_clients[new_id].m_y = rand() % 20;
				SharedData::g_clients[new_id].m_id = new_id;
				SharedData::g_clients[new_id].m_name[0] = 0;
				SharedData::g_clients[new_id].m_prev_remain = 0;
				SharedData::g_clients[new_id].m_socket = SharedData::g_c_socket;
				CreateIoCompletionPort(reinterpret_cast<HANDLE>(SharedData::g_c_socket), h_iocp, new_id, 0);
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
		}
	}
}

int main()
{
	HANDLE h_iocp;
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

	h_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(SharedData::g_listen_socket), h_iocp, 9999, 0);

	// 맵 하자 섹터까지 + g_map에 flag 넣는거부터

	Initialize();
	
	SharedData::g_c_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	SharedData::g_over.op_type = OP_ACCEPT;
	AcceptEx(SharedData::g_listen_socket, SharedData::g_c_socket, SharedData::g_over.sendbuf, 0, addr_size + 16, addr_size + 16, 0, &SharedData::g_over.over);

	vector <thread> worker_threads;
	int num_threads = std::thread::hardware_concurrency();
	for (int i = 0; i < num_threads; ++i)
		worker_threads.emplace_back(WorkerThread, h_iocp);
	for (auto& th : worker_threads)
		th.join();

	closesocket(SharedData::g_listen_socket);
	WSACleanup();
}