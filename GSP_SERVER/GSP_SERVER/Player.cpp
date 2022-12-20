#include "Player.h"
#include "Sector.h"
#include "DataBase.h"
#include "function.h"
#include "NPC.h"
#include "Party.h"

void CPlayer::Initialize()
{
	m_s_lock.lock();
	m_state = ST_ALLOC;
	m_s_lock.unlock();

	m_name[0] = 0;
	m_prev_remain = 0;

	m_x = START_X;
	m_y = START_Y;
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
		if (isParty(id)) continue;			// 뒤에 파티원 따로 처리해줄거임
		cl->SendRemoveObjectPacket(m_id);
	}


	if (m_party != nullptr) {
		for (auto& pm : m_party->GetPartyMemeber()) {
			CPlayer* pl = reinterpret_cast<CPlayer*>(SharedData::g_clients[pm]);
			pl->SendExitPartyPacket(m_id);
			pl->SendRemoveObjectPacket(m_id);
		}
	}

	SharedData::g_sector.RemoveSector(m_id, m_x, m_y);

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
	switch (direction) {
	case 0: if (y > 0 && (SharedData::g_map[x][y-1] == true)) y--; break;
	case 1: if (y < W_HEIGHT - 1 && (SharedData::g_map[x][y + 1] == true)) y++; break;
	case 2: if (x > 0 && (SharedData::g_map[x-1][y] == true)) x--; break;
	case 3: if (x < W_WIDTH - 1 && (SharedData::g_map[x+1][y] == true)) x++; break;
	}

	//m_move_time = 0;
	
	SharedData::g_sector.UpdateSector(m_id, x, y, m_x, m_y);
	m_x = x;
	m_y = y;

	CheckViewList();
}

void CPlayer::CheckExpAndLevel()
{
	// 얘는 exp 건드릴때마다만 호출
	if (m_exp >= m_requiredExp) {
		m_level += 1;
		m_requiredExp *= 2;
	}

	if (m_exp <= 0) m_exp = 0;
}

void CPlayer::Respawn()
{
	m_vl_lock.lock();
	unordered_set<int> vlist = m_viewlist;
	m_vl_lock.unlock();

	// 날 보고 있던 애들한테서 나 remove
	for (auto& cl : vlist) {
		SendRemoveObjectPacket(cl);
		if (is_pc(cl)) {
			CPlayer* p = reinterpret_cast<CPlayer*>(SharedData::g_clients[cl]);
			p->m_vl_lock.lock();
			if (p->m_viewlist.count(m_id)) {
				p->m_vl_lock.unlock();
				p->SendRemoveObjectPacket(m_id);
			}
			else {
				p->m_vl_lock.unlock();
			}
		}
		else {
			//SleepNPC(cl, m_id);
		}
	}

	SharedData::g_sector.UpdateSector(m_id, START_X, START_Y, m_x, m_y);
	m_x = START_X;
	m_y = START_Y;
	SendMoveObjectPacket(m_id, 0);

	m_exp -= m_exp / 2;
	m_hp = MAX_HP;
	
	SendStatChangePacket(m_id, m_exp, m_level, m_hp, m_maxhp);

	// 위치 이동 후 주위 플레이어 싹 다 추가

	unordered_set<int> nearlist;
	SharedData::g_sector.CreateNearList(nearlist, m_id, m_x, m_y);

	for (auto& nc : nearlist) {
		if (is_pc(nc)) {
			CPlayer* p = reinterpret_cast<CPlayer*>(SharedData::g_clients[nc]);
			p->SendAddObjectPacket(m_id);
			p->SendStatChangePacket(m_id, m_exp, m_level, m_hp, m_maxhp);
			SendAddObjectPacket(nc);
			SendStatChangePacket(nc, p->m_exp, p->m_level, p->m_hp, p->m_maxhp);

		}
		else {
			CNPC* npc = reinterpret_cast<CNPC*>(SharedData::g_clients[nc]);
			if (npc->m_is_active) {
				SendAddObjectPacket(nc);
				SendStatChangePacket(nc, npc->m_exp, npc->m_level, npc->m_hp, npc->m_maxhp);
			}
			WakeUpNPC(nc, m_id);		
		}
	}
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
		if (CanAttack(m_id, cl)) {
			if (is_npc(cl)) {
				CNPC* npc = reinterpret_cast<CNPC*>(SharedData::g_clients[cl]);
				if (npc->m_is_active == false) continue;
				if (npc->m_attack_type == NPC_PEACE) {
					npc->SetTarget(m_id);
					if (CanAttack(npc->m_id, m_id)) npc->m_npc_state = NPC_ATTACK;
					else npc->m_npc_state = NPC_MOVE;
				}

				int exp = m_level * m_level * 2;
				
				npc->SetDamage(exp);
				
				// 플레이어 경험치 처리
				if (npc->m_move_type == NPC_LOAMING) exp *= 2;
				if (npc->m_attack_type == NPC_AGRO) exp *= 2;
				m_exp += exp;
				CheckExpAndLevel();
				SendStatChangePacket(m_id, m_exp, m_level, m_hp, m_maxhp);

				char mess[CHAT_SIZE];
				sprintf_s(mess, "Get EXP %d - attack %s!", exp, npc->m_name);
				SendChatPacket(SYSTEM_CHAT_ID, mess);

				// 파티원 경험치 공유
				if (m_party != nullptr) {
					unordered_set<int> party = m_party->GetPartyMemeber();
					for (auto& p : party) {
						if (p == m_id) continue;
						CPlayer* pp = reinterpret_cast<CPlayer*>(SharedData::g_clients[p]);
						pp->m_exp += exp;
						pp->CheckExpAndLevel();
						pp->SendStatChangePacket(pp->m_id, pp->m_exp, pp->m_level, pp->m_hp, pp->m_maxhp);
						pp->SendChatPacket(SYSTEM_CHAT_ID, mess);
					}
				}


			}
		}
	}

	SendAttackPacket(m_id);
	for (auto& pl : vlist) {
		if (is_pc(pl)) {
			reinterpret_cast<CPlayer*>(SharedData::g_clients[pl])->SendAttackPacket(m_id);
		}
	}
}

void CPlayer::SetDamage(int damage)
{
	m_hp -= damage;
	if (m_hp <= 0) {
		// 플레이어 다이
		char mess[CHAT_SIZE];
		sprintf_s(mess, "%s DIE !!", m_name);
		SendChatPacket(SYSTEM_CHAT_ID, mess);

		SendPlayerDiePacket();

		if (m_party != nullptr) {
			unordered_set<int> party = m_party->GetPartyMemeber();
			for (auto& p : party) {
				CPlayer* pp = reinterpret_cast<CPlayer*>(SharedData::g_clients[p]);
				pp->SendChatPacket(SYSTEM_CHAT_ID, mess);
			}
		}
		
		Respawn();
	}
	else {
		SendStatChangePacket(m_id, m_exp, m_level, m_hp, m_maxhp);

		if (m_party != nullptr) {
			unordered_set<int> party = m_party->GetPartyMemeber();
			for (auto& p : party) {
				CPlayer* pp = reinterpret_cast<CPlayer*>(SharedData::g_clients[p]);
				pp->SendStatChangePacket(m_id, m_exp, m_level, m_hp, m_maxhp);
			}
		}
	}
}

bool CPlayer::isParty(int id)
{
	if (m_party == nullptr) return false;

	if (m_party->GetPartyMemeber().count(id) != 0)
		return true; 
	else 
		return false; 
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
	
	SendMoveObjectPacket(m_id, m_move_time);

	if (m_party != nullptr) {
		for (auto& p : m_party->GetPartyMemeber()) {
			CPlayer* pl = reinterpret_cast<CPlayer*>(SharedData::g_clients[p]);
			pl->SendMoveObjectPacket(m_id, 0);
		}
	}

	for (auto& pl : near_list) {
		if (is_pc(pl)) {
			if (isParty(pl)) continue;
			CPlayer* cpl = reinterpret_cast<CPlayer*>(SharedData::g_clients[pl]);
			cpl->m_vl_lock.lock();
			if (cpl->m_viewlist.count(m_id)) {
				cpl->m_vl_lock.unlock();
				cpl->SendMoveObjectPacket(m_id, 0);
			}
			else {
				cpl->m_vl_lock.unlock();
				cpl->SendAddObjectPacket(m_id);
				cpl->SendStatChangePacket(m_id, m_exp, m_level, m_hp, m_maxhp);
			}
		}
		else WakeUpNPC(pl, m_id);

		if (old_vlist.count(pl) == 0) {
			if (is_pc(pl)) {
				CObject* p = SharedData::g_clients[pl];
				SendAddObjectPacket(pl);
				SendStatChangePacket(pl, p->m_exp, p->m_level, p->m_hp, p->m_maxhp);
			}
			else {
				CNPC* npc = reinterpret_cast<CNPC*>(SharedData::g_clients[pl]);
				if (npc->m_is_active) {
					SendAddObjectPacket(pl);
					SendStatChangePacket(pl, npc->m_exp, npc->m_level, npc->m_hp, npc->m_maxhp);
				}
			}
		}
	}

	for (auto& pl : old_vlist) {
		if (near_list.count(pl) == 0) {
			if (isParty(pl)) continue;

			SendRemoveObjectPacket(pl);
			if (is_pc(pl)) {
				reinterpret_cast<CPlayer*>(SharedData::g_clients[pl])->SendRemoveObjectPacket(m_id);
			}
			else {
				//SleepNPC(pl, m_id);
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
	SendChatPacket(m_id, mess);
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

void CPlayer::SendMoveObjectPacket(int c_id, int move_time)
{
	SC_MOVE_OBJECT_PACKET p;
	p.id = c_id;
	p.size = sizeof(SC_MOVE_OBJECT_PACKET);
	p.type = SC_MOVE_OBJECT;
	p.x = SharedData::g_clients[c_id]->m_x;
	p.y = SharedData::g_clients[c_id]->m_y;
	p.move_time = move_time;
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

	//SharedData::g_sector.RemoveSector(c_id, SharedData::g_clients[c_id]->m_x, SharedData::g_clients[c_id]->m_y);

	SC_REMOVE_OBJECT_PACKET p;
	p.id = c_id;
	p.size = sizeof(SC_REMOVE_OBJECT_PACKET);
	p.type = SC_REMOVE_OBJECT;
	SendPacket(&p);
}

void CPlayer::SendAttackPacket(int c_id)
{
	SC_ATTACK_PACKET p;
	p.id = c_id;
	p.size = sizeof(SC_ATTACK_PACKET);
	p.type = SC_ATTACK;
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

void CPlayer::SendInvitePartyPacket(int c_id)
{
	SC_INVITE_PARTY_PACKET p;
	p.size = sizeof(SC_INVITE_PARTY_PACKET);
	p.type = SC_INVITE_PARTY;
	p.id = c_id;
	SendPacket(&p);
}

void CPlayer::SendJoinPartyPacket(int c_id)
{
	SC_JOIN_PARTY_PACKET p;
	p.size = sizeof(SC_JOIN_PARTY_PACKET);
	p.type = SC_JOIN_PARTY;
	p.id = c_id;
	SendPacket(&p);
}

void CPlayer::SendExitPartyPacket(int c_id)
{
	SC_EXIT_PARTY_PACKET p;
	p.size = sizeof(SC_EXIT_PARTY_PACKET);
	p.type = SC_EXIT_PARTY;
	p.id = c_id;
	SendPacket(&p);
}

void CPlayer::SendPlayerDiePacket()
{
	SC_PLAYER_DIE_PACKET p;
	p.size = sizeof(SC_PLAYER_DIE_PACKET);
	p.type = SC_PLAYER_DIE;
	p.id = m_id;
	SendPacket(&p);

}

void CPlayer::Teleport()
{
	short x = m_x;
	short y = m_y;

	while (true) {
		x = rand() % W_WIDTH;
		y = rand() % W_HEIGHT;
		if (SharedData::g_map[x][y]) break;
	}
	
	SharedData::g_sector.UpdateSector(m_id, x, y, m_x, m_y);
	m_x = x;
	m_y = y;

	CheckViewList();
}