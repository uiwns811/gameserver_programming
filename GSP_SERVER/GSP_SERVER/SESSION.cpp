#include "SESSION.h"

void SESSION::SendPacket(void* packet)
{
	EXP_OVER* s = new EXP_OVER{ reinterpret_cast<char*>(packet) };
	WSASend(m_socket, &s->_wsabuf, 1, 0, 0, &s->_over, 0);
}

void SESSION::Recv()
{
	DWORD recv_flag = 0;
	memset(&m_recv_over._over, 0, sizeof(m_recv_over._over));
	m_recv_over._wsabuf.len = BUF_SIZE - m_prev_remain;
	m_recv_over._wsabuf.buf = m_recv_over._send_buf + m_prev_remain;
	WSARecv(m_socket, &m_recv_over._wsabuf, 1, 0, &recv_flag, &m_recv_over._over, 0);
}

void SESSION::SendLoginOkPacket()
{
	SC_LOGIN_OK_PACKET p;
	p.id = GetID();
	p.size = sizeof(SC_LOGIN_OK_PACKET);
	p.type = SC_LOGIN_OK;
	p.x = GetX();
	p.y = GetY();
	SendPacket(&p);
}

void SESSION::SendLoginFailPacket()
{

}

void SESSION::SendAddPlayerPacket(short c_id)
{
	SC_ADD_PLAYER_PACKET p;
	p.id = c_id;
	strcpy_s(p.name, SharedData::g_clients[c_id].GetName());
	p.size = sizeof(p);
	p.type = SC_ADD_PLAYER;
	p.x = SharedData::g_clients[c_id].GetX();
	p.y = SharedData::g_clients[c_id].GetY();
	SendPacket(&p);
}

void SESSION::SendMovePlayerPacket(short c_id)
{
	SC_MOVE_PLAYER_PACKET p;
	p.id = c_id;
	p.size = sizeof(SC_MOVE_PLAYER_PACKET);
	p.type = SC_MOVE_PLAYER;
	p.x = SharedData::g_clients[c_id].GetX();
	p.y = SharedData::g_clients[c_id].GetY();
	p.move_time = SharedData::g_clients[c_id].GetMoveTime();
	SendPacket(&p);
}

void SESSION::SendRemovePlayerPacket(short c_id)
{
	SC_REMOVE_PLAYER_PACKET p;
	p.id = c_id;
	p.size = sizeof(p);
	p.type = SC_REMOVE_PLAYER;
	SendPacket(&p);
}