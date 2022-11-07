#include "stdafx.h"
#include "SESSION.h"

namespace SharedData {
	array<SESSION, MAX_USER + MAX_NPC> g_clients;
	SOCKET g_s_socket;
	SOCKET g_c_socket;
	EXP_OVER g_a_over;
}

int main()
{
	HANDLE h_iocp;

	WSADATA WSAData;

	HANDLE h_iocp;

	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 2), &WSAData);
	SharedData::g_s_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	SOCKADDR_IN server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT_NUM);
	server_addr.sin_addr.S_un.S_addr = INADDR_ANY;
	bind(SharedData::g_s_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));
	listen(SharedData::g_s_socket, SOMAXCONN);
	SOCKADDR_IN cl_addr;
	int addr_size = sizeof(cl_addr);
	int client_id = 0;

	h_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(SharedData::g_s_socket), h_iocp, 9999, 0);
	SharedData::g_c_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	SharedData::g_a_over._comp_type = OP_ACCEPT;
	AcceptEx(SharedData::g_s_socket, SharedData::g_c_socket, SharedData::g_a_over._send_buf, 0, addr_size + 16, addr_size + 16, 0, &SharedData::g_a_over._over);

}