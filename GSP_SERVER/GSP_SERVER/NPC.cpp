#include "NPC.h"
#include "Player.h"
#include "Sector.h"
#include "function.h"
#include "Astar.h"
#include "Party.h"

void CNPC::Initialize()
{

}

void CNPC::Attack()
{
	if (m_target_id == -1) return;

	if (CanAttack(m_id, m_target_id) == false) {
		m_npc_state = NPC_MOVE;
		return;
	}

	CPlayer* player = reinterpret_cast<CPlayer*>(SharedData::g_clients[m_target_id]);
	int damage = m_level * m_level * 2;
	if (m_move_type == NPC_LOAMING) damage *= 2;
	if (m_attack_type == NPC_AGRO) damage *= 2;
	player->SetDamage(damage);

	char mess[CHAT_SIZE];
	sprintf_s(mess, "Get Damage %d - by %s", damage, m_name);
	player->SendChatPacket(SYSTEM_CHAT_ID, mess);

	// 파티원 경험치 공유
	if (player->GetParty() != nullptr) {
		unordered_set<int> party = player->GetParty()->GetPartyMemeber();
		for (auto& p : party) {
			if (p == m_target_id) continue;
			CPlayer* pp = reinterpret_cast<CPlayer*>(SharedData::g_clients[p]);
			pp->SetDamage(damage);
			pp->SendStatChangePacket(pp->m_id, pp->m_exp, pp->m_level, pp->m_hp, pp->m_maxhp);
			pp->SendChatPacket(SYSTEM_CHAT_ID, mess);
		}
	}

	unordered_set<int> vlist;
	SharedData::g_sector.CreateNearPlayerList(vlist, m_id, m_x, m_y);
	for (auto& pl : vlist) {
		if (is_pc(pl)) {
			reinterpret_cast<CPlayer*>(SharedData::g_clients[pl])->SendAttackPacket(m_id);
		}
	}

	if (CanAttack(m_id, m_target_id) == false)
		m_npc_state = NPC_MOVE;
}

void CNPC::SetDamage(int damage)
{
	m_hp -= damage;

	unordered_set<int> vlist;
	SharedData::g_sector.CreateNearPlayerList(vlist, m_id, m_x, m_y);
	for (auto& cl : vlist) {
		CPlayer* p = reinterpret_cast<CPlayer*>(SharedData::g_clients[cl]);
		p->SendStatChangePacket(m_id, m_exp, m_level, m_hp, m_maxhp);
	}
	if (m_hp <= 0) {
		Die();
	}
}

void CNPC::Die()
{
	bool old_state = true;
	if (false == atomic_compare_exchange_strong(&m_is_active, &old_state, false)) return;


	unordered_set<int> vlist;
	SharedData::g_sector.CreateNearPlayerList(vlist, m_id, m_x, m_y);
	for (auto& cl : vlist) {
		CPlayer* p = reinterpret_cast<CPlayer*>(SharedData::g_clients[cl]);

		p->m_vl_lock.lock();
		if (0 != p->m_viewlist.count(m_id)) {
			p->m_vl_lock.unlock();
			p->SendRemoveObjectPacket(m_id);
		}
		else {
			p->m_vl_lock.unlock();
		}
	}

	SharedData::g_sector.RemoveSector(m_id, m_x, m_y);

	m_hp = m_maxhp;

	TIMER_EVENT ev{ m_id, chrono::system_clock::now() + 30s, EV_RESPAWN_NPC, 0 };
	SharedData::timer_queue.push(ev);
}

bool CNPC::CheckAttackRange()
{
	unordered_set<int> vlist;
	SharedData::g_sector.CreateNearPlayerList(vlist, m_id, m_x, m_y);

	for (auto& pl : vlist) {
		if (CanAttack(m_id, pl) == true) {
			m_target_id = pl;
			return true;
		}
	}
	return false;
}

void CNPC::CheckTargetRange()
{
	switch (m_type) {
	case NPC_PEACE_FIXED:
	{
		if (m_target_id != -1) {
			if (can_see(m_id, m_target_id) == false) {
				m_target_id = -1;
				m_npc_state = NPC_IDLE;
			}
		}
		else {
			if (CheckAttackRange())
				m_npc_state = NPC_ATTACK;
		}
	}
	break;
	case NPC_PEACE_LOAMING:
	{
		if (m_target_id != -1) {
			if (can_see(m_id, m_target_id) == false) {
				m_target_id = -1;
				m_npc_state = NPC_MOVE;
			}
		}
	}
	break;
	case NPC_AGRO_FIXED:
	{
		if (m_target_id != -1) {
			if (isInAgroArea(m_id, m_target_id) == false) {
				m_target_id = -1;
				m_npc_state = NPC_IDLE;
			}
		}
		else {
			unordered_set<int> nplist;
			SharedData::g_sector.CreateNearPlayerList(nplist, m_id, m_x, m_y);
			for (auto& cl : nplist) {
				if (isInAgroArea(m_id, cl)) {
					m_target_id = cl;
					m_npc_state = NPC_MOVE;
				}
			}

			if (CheckAttackRange())
				m_npc_state = NPC_ATTACK;
		}
	}
	break;
	case NPC_AGRO_LOAMING:
	{
		if (m_target_id != -1) {
			if (isInAgroArea(m_id, m_target_id) == false) {
				m_target_id = -1;
				m_npc_state = NPC_MOVE;
			}
		}
		else {
			unordered_set<int> nplist;
			SharedData::g_sector.CreateNearPlayerList(nplist, m_id, m_x, m_y);
			for (auto& cl : nplist) {
				if (isInAgroArea(m_id, cl)) {
					m_target_id = cl;
					m_npc_state = NPC_MOVE;
					break;
				}
			}

		}
	}
	break;
	}
}

void CNPC::NPCAI()
{
	CheckTargetRange();

	switch (m_npc_state) {
	case NPC_IDLE:
	break;
	case NPC_MOVE:
	{
		if (m_target_id != -1) MoveToTarget();
		else if (m_target_id == -1 && m_move_type == NPC_LOAMING) MoveRandom();
	}
	break;
	case NPC_ATTACK:
		Attack();
	break;
	}
}

void CNPC::MoveRandom()
{
	if (m_target_id != -1 || m_move_type == NPC_FIXED) return;

	unordered_set<int> old_vl;
	SharedData::g_sector.CreateNearPlayerList(old_vl, m_id, m_x, m_y);

	short x = m_x;
	short y = m_y;

	while (true) {
		switch (rand() % 4) {
		case 0: if (x < (W_WIDTH - 1) && x < (m_loaming_x + LOAMING_AREA) && SharedData::g_map[x+1][y]) x++; break;
		case 1: if (x > 0 && x > (m_loaming_x - LOAMING_AREA) && SharedData::g_map[x - 1][y]) x--; break;
		case 2: if (y < (W_HEIGHT - 1) && y < (m_loaming_y + LOAMING_AREA) && SharedData::g_map[x][y + 1]) y++; break;
		case 3: if (y > 0 && y > (m_loaming_y - LOAMING_AREA) && SharedData::g_map[x][y - 1]) y--; break;
		}

		if (m_x != x || m_y != y) break;
	}

	SharedData::g_sector.UpdateSector(m_id, x, y, m_x, m_y);
	m_x = x;
	m_y = y;

	Move(old_vl);
}

void CNPC::MoveToTarget()
{
	unordered_set<int> old_vl;
	SharedData::g_sector.CreateNearPlayerList(old_vl, m_id, m_x, m_y);

	NODE start = NODE(m_x, m_y, nullptr);
	NODE goal = NODE(SharedData::g_clients[m_target_id]->m_x, SharedData::g_clients[m_target_id]->m_y, nullptr);

	m_astar = new CAstar();

	m_astar->SetStartPoint(&start);
	m_astar->SetGoalPoint(&goal);
	if (m_astar->FindPath()) {
		
		short x = m_astar->GetPath().x;
		short y = m_astar->GetPath().y;

		if (x == SharedData::g_clients[m_target_id]->m_x && y == SharedData::g_clients[m_target_id]->m_y) return;

		SharedData::g_sector.UpdateSector(m_id, x, y, m_x, m_y);
		m_x = x;
		m_y = y;
	}
	delete m_astar;

	Move(old_vl);
}

void CNPC::Move(unordered_set<int>& old_vl)
{
	unordered_set<int> new_vl;
	SharedData::g_sector.CreateNearPlayerList(new_vl, m_id, m_x, m_y);

	for (auto& pl : new_vl) {
		CPlayer* p = reinterpret_cast<CPlayer*>(SharedData::g_clients[pl]);
		if (0 == old_vl.count(pl)) {
			p->SendAddObjectPacket(m_id);
			p->SendStatChangePacket(m_id, m_exp, m_level, m_hp, m_maxhp);
		}
		else {
			p->SendMoveObjectPacket(m_id, 0);
		}

		if (CanAttack(m_id, pl) == true) {
			m_npc_state = NPC_ATTACK;
			m_target_id = pl;
		}
	}

	for (auto& pl : old_vl) {
		if (0 == new_vl.count(pl)) {
			CPlayer* p = reinterpret_cast<CPlayer*>(SharedData::g_clients[pl]);
			p->m_vl_lock.lock();
			if (0 != p->m_viewlist.count(m_id)) {
				p->m_vl_lock.unlock();
				p->SendRemoveObjectPacket(m_id);
			}
			else {
				p->m_vl_lock.unlock();
			}
		}
	}
}

void InitializeNPC()
{
	for (int i = MAX_USER; i < MAX_USER + MAX_NPC; ++i) {
		CNPC* npc = reinterpret_cast<CNPC*>(SharedData::g_clients[i]);
		npc->m_id = i;


		auto L = npc->m_L = luaL_newstate();
		luaL_openlibs(L);
		int error = luaL_loadfile(L, "npc.lua") ||
			lua_pcall(L, 0, 0, 0);
		lua_getglobal(L, "set_uid");
		lua_pushnumber(L, i);
		lua_pcall(L, 1, 0, 0);
		lua_pop(L, 1);

		lua_register(L, "API_InitializePos", API_InitializePos);

		lua_getglobal(L, "Init");
		error = lua_pcall(L, 0, 5, 0);
		if (error) {
			cout << "Error : " << lua_tostring(L, -1);
			lua_pop(L, 1);
		}
		npc->m_type = static_cast<NPC_TYPE>(lua_tonumber(L, -5));
		npc->m_x = (int)lua_tonumber(L, -4);
		npc->m_y = (int)lua_tonumber(L, -3);
		npc->m_level = (int)lua_tonumber(L, -2);
		npc->m_hp = (int)lua_tonumber(L, -1);
		lua_pop(L, 6);

		npc->m_maxhp = npc->m_hp;

		//lua_register(L, "API_get_x", API_get_x);
		//lua_register(L, "API_get_y", API_get_y);
		//lua_register(L, "API_Run", API_Run);
		//lua_register(L, "API_do_npc_run", API_do_npc_run);

		SharedData::g_sector.InsertSector(i, npc->m_x, npc->m_y);

		npc->m_is_active = false;
		npc->m_state = ST_INGAME;

		switch (npc->m_type) {
		case NPC_PEACE_FIXED:
			npc->m_move_type = NPC_FIXED;
			npc->m_attack_type = NPC_PEACE;
			npc->m_npc_state = NPC_IDLE;
			break;
		case NPC_PEACE_LOAMING:
			npc->m_move_type = NPC_LOAMING;
			npc->m_attack_type = NPC_PEACE;
			npc->m_loaming_x = npc->m_x;
			npc->m_loaming_y = npc->m_y;
			npc->m_npc_state = NPC_MOVE;
			break;
		case NPC_AGRO_FIXED:
			npc->m_move_type = NPC_FIXED;
			npc->m_attack_type = NPC_AGRO;
			npc->m_npc_state = NPC_IDLE;
			break;
		case NPC_AGRO_LOAMING:
			npc->m_move_type = NPC_LOAMING;
			npc->m_attack_type = NPC_AGRO;
			npc->m_loaming_x = npc->m_x;
			npc->m_loaming_y = npc->m_y;
			npc->m_npc_state = NPC_MOVE;
			break;
		}

		npc->m_target_id = -1;

		sprintf_s(npc->m_name, "%d_NPC%d", static_cast<int>(npc->m_type), i);
	}
	cout << "NPC Initialize End" << endl;
}

void WakeUpNPC(int npc_id, int player_id)
{
	if (is_pc(npc_id)) return;

	CNPC* npc = reinterpret_cast<CNPC*>(SharedData::g_clients[npc_id]);
	if (npc->m_is_active) return;

	bool old_state = false;
	if (false == atomic_compare_exchange_strong(&npc->m_is_active, &old_state, true)) return;

	TIMER_EVENT ev{ npc_id, chrono::system_clock::now(), EV_NPC_AI, 0 };
	SharedData::timer_queue.push(ev);	
}

void SleepNPC(int npc_id, int player_id)
{
	if (is_pc(npc_id)) return;

	CNPC* npc = reinterpret_cast<CNPC*>(SharedData::g_clients[npc_id]);
	if (false == npc->m_is_active) return;

	// npc 주위에 아무도 없을 때 Sleep 시킴
	unordered_set<int> nplist;
	SharedData::g_sector.CreateNearPlayerList(nplist, npc_id, npc->m_x, npc->m_y);
	if (nplist.empty() == false) return;

	bool old_state = true;
	if (false == atomic_compare_exchange_strong(&npc->m_is_active, &old_state, false)) return;
}

