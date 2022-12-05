#include "Player.h"
#include "Sector.h"
#include "DataBase.h"
#include "function.h"

void CPlayer::Initialize()
{
	m_s_lock.lock();
	m_state = ST_ALLOC;
	m_s_lock.unlock();

	m_name[0] = 0;
	m_prev_remain = 0;

	m_x = rand() % W_WIDTH;
	m_y = rand() % W_HEIGHT;
	m_exp = START_EXP;
	m_level = 1;
	m_hp = MAX_HP;
	m_requiredExp = START_EXP * 2;
	m_is_active = false;
	m_is_attack = false;
}

void CPlayer::Disconnect()
{
	m_s_lock.lock();
	unordered_set<short> vl = m_viewlist;
	m_s_lock.unlock();

	for (auto& id : vl)
	{
		if (is_npc(id)) continue;
		CPlayer* cl = reinterpret_cast<CPlayer*>(SharedData::g_clients[id]);
		if (cl->m_id == m_id) continue;
		if (ST_INGAME != cl->m_state) continue;
		cl->SendRemovePlayerPacket(m_id);
	}

	m_s_lock.lock();
	m_state = ST_FREE;
	m_s_lock.unlock();

	closesocket(m_socket);

	SharedData::g_db.Enqueue(m_id, m_name, DB_DISCONNECT);
}

void CPlayer::Tick()
{
	if (m_state != ST_INGAME) return;

	if (m_recovery_cooltime + 5s < chrono::system_clock::now()) {
		RecoverHp();
		m_recovery_cooltime = chrono::system_clock::now();
	}

	CheckExpAndLevel();

	if (m_hp <= 0) {
		Respawn();
	}
}

void CPlayer::Move(int direction, unsigned char move_time)
{
	short x = m_x;
	short y = m_y;
	short oldX = x;
	short oldY = y;
	switch (direction) {
	case 0: if (y > 0) y--; break;
	case 1: if (y < W_HEIGHT - 1) y++; break;
	case 2: if (x > 0) x--; break;
	case 3: if (x < W_WIDTH - 1) x++; break;
	}

	// 서버에서 한 번 더 체크 -> 갈 수 없는 곳에 갔으면 원위치로 이동
	if (SharedData::g_map[x][y]) {
		m_x = x;
		m_y = y;
		m_move_time = move_time;

		SharedData::g_sector.UpdateSector(m_id, x, y, oldX, oldY);
		CheckViewList();
		//player->SendMovePlayerPacket(c_id);
	}
}

void CPlayer::CheckExpAndLevel()
{
	if (m_exp >= m_requiredExp) {
		m_level += 1;
		m_requiredExp *= 2;
	}
}

void CPlayer::Respawn()
{
	m_exp -= m_exp / 2;
	m_hp = MAX_HP;
	// 시작 위치로 옮기기
}

void CPlayer::RecoverHp()
{
	if (m_hp < MAX_HP) {
		m_hp += MAX_HP / 10;
		if (m_hp > MAX_HP)
			m_hp = MAX_HP;
	}
}

void CPlayer::Attack()
{
	m_vl_lock.lock();
	unordered_set<short> vlist = m_viewlist;
	m_vl_lock.unlock();

	SendAttackPlayerPacket(m_id);
	for (auto& pl : vlist) {
		if (is_pc(pl)) {
			reinterpret_cast<CPlayer*>(SharedData::g_clients[pl])->SendAttackPlayerPacket(m_id);
		}
	}
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

void CPlayer::CheckViewList()
{
	m_vl_lock.lock();
	unordered_set<short> old_vlist = m_viewlist;
	m_vl_lock.unlock();

	unordered_set<short> near_list;

	SharedData::g_sector.CreateNearList(near_list, m_id, m_x, m_y);
	
	SendMovePlayerPacket(m_id);

	for (auto& pl : near_list) {
		if (is_pc(pl)) {
			CPlayer* cpl = reinterpret_cast<CPlayer*>(SharedData::g_clients[pl]);
			cpl->m_vl_lock.lock();
			if (cpl->m_viewlist.count(m_id)) {
				cpl->m_vl_lock.unlock();
				cpl->SendMovePlayerPacket(m_id);
			}
			else {
				cpl->m_vl_lock.unlock();
				cpl->SendAddPlayerPacket(m_id);
			}
		}
		//else WakeUpNPC(pl, m_id);

		if (old_vlist.count(pl) == 0) {
			SendAddPlayerPacket(pl);
		}
	}

	for (auto& pl : old_vlist) {
		if (near_list.count(pl) == 0) {
			SendRemovePlayerPacket(pl);
			if (is_pc(pl)) {
				reinterpret_cast<CPlayer*>(SharedData::g_clients[pl])->SendRemovePlayerPacket(m_id);
			}
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
	p.exp = m_exp;
	p.level = m_level;
	p.hp = m_hp;
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
	strcpy_s(p.name, SharedData::g_clients[c_id]->m_name);
	p.size = sizeof(p);
	p.type = SC_ADD_PLAYER;
	p.x = SharedData::g_clients[c_id]->m_x;
	p.y = SharedData::g_clients[c_id]->m_y;
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
	p.x = reinterpret_cast<CPlayer*>(SharedData::g_clients[c_id])->m_x;
	p.y = reinterpret_cast<CPlayer*>(SharedData::g_clients[c_id])->m_y;
	p.move_time = reinterpret_cast<CPlayer*>(SharedData::g_clients[c_id])->m_move_time;
	SendPacket(&p);
}

void CPlayer::SendRemovePlayerPacket(short c_id)
{
	m_vl_lock.lock();
	if (m_viewlist.count(c_id)) {
		m_viewlist.erase(c_id);	
	}
	else {
		m_vl_lock.unlock();
		return;
	}
	m_vl_lock.unlock();	

	//SharedData::g_sector.RemoveSector(c_id, SharedData::g_clients[c_id]->m_x, SharedData::g_clients[c_id]->m_y);

	SC_REMOVE_PLAYER_PACKET p;
	p.id = c_id;
	p.size = sizeof(SC_REMOVE_PLAYER_PACKET);
	p.type = SC_REMOVE_PLAYER;
	SendPacket(&p);
}

void CPlayer::SendAttackPlayerPacket(short c_id)
{
	SC_ATTACK_PLAYER_PACKET p;
	p.id = c_id;
	p.size = sizeof(SC_ATTACK_PLAYER_PACKET);
	p.type = SC_ATTACK_PLAYER;
	SendPacket(&p);
}