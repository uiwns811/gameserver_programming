#include "Player.h"
#include "Sector.h"
#include "DataBase.h"
#include "function.h"
#include "NPC.h"

void CPlayer::Initialize()
{
	m_s_lock.lock();
	m_state = ST_ALLOC;
	m_s_lock.unlock();

	m_name[0] = 0;
	m_prev_remain = 0;

	m_x = 2;
	m_y = 2;
	m_exp = START_EXP;
	m_level = 1;
	m_hp = MAX_HP;
	m_maxhp = MAX_HP;
	m_requiredExp = START_EXP * 2;
	m_is_active = false;
	m_is_attack = false;
}

void CPlayer::Disconnect()
{
	m_s_lock.lock();
	unordered_set<int> vl = m_viewlist;
	m_s_lock.unlock();

	for (auto& id : vl)
	{
		if (is_npc(id)) continue;
		CPlayer* cl = reinterpret_cast<CPlayer*>(SharedData::g_clients[id]);
		if (cl->m_id == m_id) continue;
		if (ST_INGAME != cl->m_state) continue;
		cl->SendRemoveObjectPacket(m_id);
	}

	m_s_lock.lock();
	m_state = ST_FREE;
	m_s_lock.unlock();

	closesocket(m_socket);

	SharedData::g_db.Enqueue(m_id, m_name, DB_DISCONNECT);
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

	m_move_time = move_time;
	
	// 서버에서 한 번 더 체크 -> 갈 수 없는 곳에 갔으면 원위치로 이동
	if (SharedData::g_map[x][y]) {
		m_x = x;
		m_y = y;

		SharedData::g_sector.UpdateSector(m_id, x, y, oldX, oldY);
		CheckViewList();
	}
}

void CPlayer::CheckExpAndLevel()
{
	// 얘는 exp 건드릴때마다만 호출
	if (m_exp >= m_requiredExp) {
		m_level += 1;
		m_requiredExp *= 2;
	}
}

void CPlayer::Respawn()
{
	m_exp -= m_exp / 2;
	m_hp = MAX_HP;
	
	m_x = 2;
	m_y = 2;

	SendStatChangePacket(m_id, m_exp, m_level, m_hp, m_maxhp);
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
	unordered_set<int> vlist = m_viewlist;
	m_vl_lock.unlock();

	for (auto& cl : vlist) {
		if (true == CanAttack(m_id, cl)) {
			if (cl >= MAX_USER) {
				CNPC* npc = reinterpret_cast<CNPC*>(SharedData::g_clients[cl]);
				if (npc->m_attack_type == NPC_PEACE) {
					npc->SetTarget(m_id);
				}

				// 플레이어 경험치 처리
				int exp = npc->m_level * npc->m_level * 2;
				if (npc->m_move_type == NPC_LOAMING) exp *= 2;
				if (npc->m_attack_type == NPC_AGRO) exp *= 2;
				m_exp += exp;
				CheckExpAndLevel();
				SendStatChangePacket(m_id, m_exp, m_level, m_hp, m_maxhp);

				// 몬스터 hp 처리
				int damage = m_level * m_level * 2;
				if (npc->m_move_type == NPC_LOAMING) damage *= 2;
				if (npc->m_attack_type == NPC_AGRO) damage *= 2;
				npc->SetDamage(damage);

				char mess[CHAT_SIZE];
				sprintf_s(mess, "%s가 %s를 공격하여 %d의 경험치 획득!", m_name, npc->m_name, exp);
				SendChatPacket(SYSTEM_CHAT_ID, mess);
			}
		}
	}



	SendAttackPlayerPacket(m_id);
	for (auto& pl : vlist) {
		if (is_pc(pl)) {
			reinterpret_cast<CPlayer*>(SharedData::g_clients[pl])->SendAttackPlayerPacket(m_id);
		}
	}
}

void CPlayer::SetDamage(int damage)
{
	m_hp -= damage;
	if (m_hp <= 0) {
		// 플레이어 다이
		char mess[CHAT_SIZE];
		sprintf_s(mess, "die..");
		SendChatPacket(SYSTEM_CHAT_ID, mess);
		
		Respawn();
	}
	else {
		SendStatChangePacket(m_id, m_exp, m_level, m_hp, m_maxhp);
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
	unordered_set<int> old_vlist = m_viewlist;
	m_vl_lock.unlock();

	unordered_set<int> near_list;

	SharedData::g_sector.CreateNearList(near_list, m_id, m_x, m_y);
	
	SendMoveObjectPacket(m_id);

	for (auto& pl : near_list) {
		if (is_pc(pl)) {
			CPlayer* cpl = reinterpret_cast<CPlayer*>(SharedData::g_clients[pl]);
			cpl->m_vl_lock.lock();
			if (cpl->m_viewlist.count(m_id)) {
				cpl->m_vl_lock.unlock();
				cpl->SendMoveObjectPacket(m_id);
			}
			else {
				cpl->m_vl_lock.unlock();
				cpl->SendAddObjectPacket(m_id);
			}
		}
		else WakeUpNPC(pl, m_id);

		if (old_vlist.count(pl) == 0) {
			SendAddObjectPacket(pl);
		}
	}

	for (auto& pl : old_vlist) {
		if (near_list.count(pl) == 0) {
			SendRemoveObjectPacket(pl);
			if (is_pc(pl)) {
				reinterpret_cast<CPlayer*>(SharedData::g_clients[pl])->SendRemoveObjectPacket(m_id);
			}
		}
	}
}

void CPlayer::Chatting(char* mess)
{
	m_vl_lock.lock();
	unordered_set<int> vlist = m_viewlist;
	m_vl_lock.unlock();

	for (auto& pl : vlist) {
		if (is_pc(pl)) {
			reinterpret_cast<CPlayer*>(SharedData::g_clients[pl])->SendChatPacket(m_id, mess);
		}
	}
}

void CPlayer::SendLoginOkPacket()
{
	SC_LOGIN_OK_PACKET p;
	p.size = sizeof(SC_LOGIN_OK_PACKET);
	p.type = SC_LOGIN_OK;
	SendPacket(&p);
}

void CPlayer::SendLoginFailPacket()
{
	SC_LOGIN_FAIL_PACKET p;
	p.size = sizeof(SC_LOGIN_FAIL_PACKET);
	p.type = SC_LOGIN_FAIL;
	SendPacket(&p);
}

void CPlayer::SendLoginInfoPacket()
{
	SC_LOGIN_INFO_PACKET p;
	p.size = sizeof(SC_LOGIN_INFO_PACKET);
	p.type = SC_LOGIN_INFO;
	p.id = m_id;
	p.x = m_x;
	p.y = m_y;
	p.exp = m_exp;
	p.level = m_level;
	p.hp = m_hp;
	p.max_hp = m_maxhp;
	SendPacket(&p);
}

void CPlayer::SendAddObjectPacket(int c_id)
{
	SC_ADD_OBJECT_PACKET p;
	p.id = c_id;
	strcpy_s(p.name, SharedData::g_clients[c_id]->m_name);
	p.size = sizeof(p);
	p.type = SC_ADD_OBJECT;
	p.x = SharedData::g_clients[c_id]->m_x;
	p.y = SharedData::g_clients[c_id]->m_y;
	m_vl_lock.lock();
	m_viewlist.insert(c_id);
	m_vl_lock.unlock();
	SendPacket(&p);
}

void CPlayer::SendMoveObjectPacket(int c_id)
{
	SC_MOVE_OBJECT_PACKET p;
	p.id = c_id;
	p.size = sizeof(SC_MOVE_OBJECT_PACKET);
	p.type = SC_MOVE_OBJECT;
	p.x = SharedData::g_clients[c_id]->m_x;
	p.y = SharedData::g_clients[c_id]->m_y;
	p.move_time = SharedData::g_clients[c_id]->m_move_time;
	SendPacket(&p);
}

void CPlayer::SendRemoveObjectPacket(int c_id)
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

	SharedData::g_sector.RemoveSector(c_id, SharedData::g_clients[c_id]->m_x, SharedData::g_clients[c_id]->m_y);

	SC_REMOVE_OBJECT_PACKET p;
	p.id = c_id;
	p.size = sizeof(SC_REMOVE_OBJECT_PACKET);
	p.type = SC_REMOVE_OBJECT;
	SendPacket(&p);
}

void CPlayer::SendAttackPlayerPacket(int c_id)
{
	SC_ATTACK_PLAYER_PACKET p;
	p.id = c_id;
	p.size = sizeof(SC_ATTACK_PLAYER_PACKET);
	p.type = SC_ATTACK_PLAYER;
	SendPacket(&p);
}

void CPlayer::SendChatPacket(int c_id, char* mess)
{
	SC_CHAT_PACKET p;
	p.id = c_id;
	p.size = sizeof(SC_CHAT_PACKET);
	p.type = SC_CHAT;
	strncpy_s(p.mess, mess, CHAT_SIZE);
	SendPacket(&p);
}

void CPlayer::SendStatChangePacket(int c_id, int exp, int level, int hp, int maxhp)
{
	SC_STAT_CHANGE_PACKET p;
	p.size = sizeof(SC_STAT_CHANGE_PACKET);
	p.type = SC_STAT_CHANGE;
	p.id = c_id;
	p.exp = exp;
	p.level = level;
	p.hp = hp;
	p.max_hp = maxhp;
	SendPacket(&p);
}

void CPlayer::Teleport()
{
	short x = m_x;
	short y = m_y;
	m_x = rand() % W_WIDTH;
	m_y = rand() % W_HEIGHT;

	SharedData::g_sector.UpdateSector(m_id, m_x, m_y, x, y);
	CheckViewList();
}