#include "Player.h"

void CPlayer::SendPacket(void* packet)
{
	EXP_OVER* s = new EXP_OVER{ reinterpret_cast<char*>(packet) };
	WSASend(m_socket, &s->wsabuf, 1, 0, 0, &s->over, 0);
}

void CPlayer::Recv()
{
	DWORD recv_flag = 0;
	memset(&m_recv_over.over, 0, sizeof(m_recv_over.over));
	m_recv_over.wsabuf.len = BUF_SIZE - m_prev_remain;
	m_recv_over.wsabuf.buf = m_recv_over.sendbuf + m_prev_remain;
	WSARecv(m_socket, &m_recv_over.wsabuf, 1, 0, &recv_flag, &m_recv_over.over, 0);
}

bool CPlayer::CanSee(short to)
{
	if ((m_x - SharedData::g_clients[to].m_x) > VIEW_RANGE) return false;
	return abs(m_y - SharedData::g_clients[to].m_y) <= VIEW_RANGE;
}

void CPlayer::SendLoginOkPacket()
{
	SC_LOGIN_OK_PACKET p;
	p.id = m_id;
	p.size = sizeof(SC_LOGIN_OK_PACKET);
	p.type = SC_LOGIN_OK;
	p.x = m_x;
	p.y = m_y;
	SendPacket(&p);
}

void CPlayer::SendLoginFailPacket()
{

}

void CPlayer::SendAddPlayerPacket(short c_id)
{
	SC_ADD_PLAYER_PACKET p;
	p.id = c_id;
	strcpy_s(p.name, SharedData::g_clients[c_id].m_name);
	p.size = sizeof(p);
	p.type = SC_ADD_PLAYER;
	p.x = SharedData::g_clients[c_id].m_x;
	p.y = SharedData::g_clients[c_id].m_y;
	SendPacket(&p);
}

void CPlayer::SendMovePlayerPacket(short c_id)
{
	SC_MOVE_PLAYER_PACKET p;
	p.id = c_id;
	p.size = sizeof(SC_MOVE_PLAYER_PACKET);
	p.type = SC_MOVE_PLAYER;
	p.x = SharedData::g_clients[c_id].m_x;
	p.y = SharedData::g_clients[c_id].m_y;
	p.move_time = SharedData::g_clients[c_id].m_move_time;
	SendPacket(&p);
}

void CPlayer::SendRemovePlayerPacket(short c_id)
{
	SC_REMOVE_PLAYER_PACKET p;
	p.id = c_id;
	p.size = sizeof(p);
	p.type = SC_REMOVE_PLAYER;
	SendPacket(&p);
}