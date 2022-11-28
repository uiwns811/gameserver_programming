#include "Player.h"
#include "Sector.h"
#include "DataBase.h"

void CPlayer::Initialize()
{
	m_s_lock.lock();
	m_state = ST_ALLOC;
	m_s_lock.unlock();

	m_name[0] = 0;
	m_prev_remain = 0;

	m_x = rand() % 20;
	m_y = rand() % 20;
	m_exp = 0;
	m_level = 0;
	m_hp = 0;
	SharedData::g_sector.InsertSector(m_id, m_x, m_y);
}

void CPlayer::Disconnect()
{
	m_s_lock.lock();
	unordered_set<short> vl = m_viewlist;
	m_s_lock.unlock();

	for (auto& id : vl)
	{
		auto& cl = SharedData::g_clients[id];
		if (cl.m_id == m_id) continue;
		if (ST_INGAME != cl.m_state) continue;
		cl.SendRemovePlayerPacket(m_id);
	}

	closesocket(m_socket);

	m_s_lock.lock();
	m_state = ST_FREE;
	m_s_lock.unlock();

	DB_EVENT event;
	event.obj_id = m_id;
	strcpy_s(event.name, m_name);
	event.event_type = EV_DB_DISCONNECT;
	SharedData::g_db.db_queue.push(event);
}

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

void CPlayer::CheckViewList()
{
	m_vl_lock.lock();
	unordered_set<short> old_vlist = m_viewlist;
	m_vl_lock.unlock();

	unordered_set<short> near_list;

	for (auto& cl : SharedData::g_clients) {			// 여기를 인접한 sector로 구현
		if (cl.m_state != ST_INGAME) continue;
		if (cl.m_id == m_id) continue;
		if (CanSee(cl.m_id))
			near_list.insert(cl.m_id);
	}
	
	for (auto& pl : near_list) {
		auto& cpl = SharedData::g_clients[pl];
		cpl.m_vl_lock.lock();
		if (cpl.m_viewlist.count(m_id)) {
			cpl.m_vl_lock.unlock();
			cpl.SendMovePlayerPacket(m_id);
		}
		else {
			cpl.m_vl_lock.unlock();
			cpl.SendAddPlayerPacket(m_id);
		}

		if (old_vlist.count(pl) == 0) {
			SendAddPlayerPacket(pl);
		}
	}

	for (auto& pl : old_vlist) {
		if (near_list.count(pl) == 0) {
			SendRemovePlayerPacket(pl);
			SharedData::g_clients[pl].SendRemovePlayerPacket(m_id);
		}
	}
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
	SC_LOGIN_FAIL_PACKET p;
	p.id = m_id;
	p.size = sizeof(SC_LOGIN_FAIL_PACKET);
	p.type = SC_LOGIN_FAIL;
	SendPacket(&p);
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
	m_vl_lock.lock();
	m_viewlist.insert(c_id);
	m_vl_lock.unlock();
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
	m_vl_lock.lock();
	if (m_viewlist.count(c_id)) {
		m_viewlist.erase(c_id);	
		m_vl_lock.unlock();	
	}
	else {
		m_vl_lock.unlock();
		return;
	}

	SC_REMOVE_PLAYER_PACKET p;
	p.id = c_id;
	p.size = sizeof(SC_REMOVE_PLAYER_PACKET);
	p.type = SC_REMOVE_PLAYER;
	SendPacket(&p);
}