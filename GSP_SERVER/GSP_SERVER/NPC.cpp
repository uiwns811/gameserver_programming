#include "NPC.h"
#include "Player.h"
#include "Sector.h"
#include "function.h"

void CNPC::Initialize()
{

}

void CNPC::Attack()
{
	CPlayer* player = reinterpret_cast<CPlayer*>(SharedData::g_clients[m_target_id]);
	int damage = m_level * m_level * 2;
	if (m_move_type == NPC_LOAMING) damage *= 2;
	if (m_attack_type == NPC_AGRO) damage *= 2;
	player->SetDamage(damage);
	char mess[CHAT_SIZE];
	sprintf_s(mess, "%s의 공격으로 데미지 %d!", m_name, damage);
	player->SendChatPacket(SYSTEM_CHAT_ID, mess);

	if (m_move_type == NPC_LOAMING) m_npc_state = NPC_MOVE;
	else m_npc_state = NPC_IDLE;
}

void CNPC::SetDamage(int damage)
{
	m_hp -= damage;
	if (m_hp <= 0) {
		Die();
	}
}

void CNPC::Die()
{
	bool old_state = true;
	if (false == atomic_compare_exchange_strong(&m_is_active, &old_state, false)) return;

	m_s_lock.lock();
	m_state = ST_ALLOC;
	m_s_lock.unlock();

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

	TIMER_EVENT ev{ m_id, chrono::system_clock::now() + 30s, EV_RESPAWN, 0 };
	SharedData::timer_queue.push(ev);
}

bool CNPC::Agro_IsInArea()
{
	// 11x11 영역에 접근하면 타겟 설정
	unordered_set<int> nearlist;
	SharedData::g_sector.CreateNearPlayerList(nearlist, m_id, m_x, m_y);
	for (auto& cl : nearlist) {
		if (abs(SharedData::g_clients[m_id]->m_x - SharedData::g_clients[cl]->m_x) > AGRO_AREA) 
			return false;
		if (abs(SharedData::g_clients[m_id]->m_y - SharedData::g_clients[cl]->m_y) <= AGRO_AREA) {
			m_target_id = cl;
			return true;
		}
		else
			return false;
	}
}

void CNPC::NPCAI()
{
	if (m_is_active == false) return;
	m_s_lock.lock();
	if (m_state != ST_INGAME) {
		m_s_lock.unlock();
		return;
	}
	m_s_lock.unlock();

	switch (m_npc_state) {
	case NPC_IDLE:
	{
		unordered_set<int> vlist;
		SharedData::g_sector.CreateNearPlayerList(vlist, m_id, m_x, m_y);

		for (auto& pl : vlist) {
			if (CanAttack(m_id, pl) == true) {
				m_npc_state = NPC_ATTACK;
				m_target_id = pl;
			}
			break;
		}
	}
	break;
	case NPC_MOVE:
	{
		if (m_target_id == -1) MoveRandom();
		else MoveToTarget();
	}
	break;
	case NPC_ATTACK:
		Attack();
	break;
	}
}

void CNPC::MoveRandom()
{
	unordered_set<int> old_vl;
	SharedData::g_sector.CreateNearPlayerList(old_vl, m_id, m_x, m_y);

	short x = m_x;
	short y = m_y;
	short oldX = x;
	short oldY = y;
	switch (rand() % 4) {
	case 0: if (x < (W_WIDTH - 1) && x < (m_loaming_x + LOAMING_AREA)) x++; break;
	case 1: if (x > 0 && x > (m_loaming_x - LOAMING_AREA)) x--; break;
	case 2: if (y < (W_HEIGHT - 1) && y < (m_loaming_y + LOAMING_AREA)) y++; break;
	case 3: if (y > 0 && y > (m_loaming_y - LOAMING_AREA)) y--; break;
	}

	while (SharedData::g_map[x][y] == false) {
		switch (rand() % 4) {
		case 0: if (x < (W_WIDTH - 1) && x < (m_loaming_x + LOAMING_AREA)) x++; break;
		case 1: if (x > 0 && x > (m_loaming_x - LOAMING_AREA)) x--; break;
		case 2: if (y < (W_HEIGHT - 1) && y < (m_loaming_y + LOAMING_AREA)) y++; break;
		case 3: if (y > 0 && y > (m_loaming_y - LOAMING_AREA)) y--; break;
		}
	}

	m_x = x;
	m_y = y;

	SharedData::g_sector.UpdateSector(m_id, x, y, oldX, oldY);

	unordered_set<int> new_vl;
	SharedData::g_sector.CreateNearPlayerList(new_vl, m_id, m_x, m_y);

	for (auto& pl : new_vl) {
		if (0 == old_vl.count(pl)) {
			reinterpret_cast<CPlayer*>(SharedData::g_clients[pl])->SendAddObjectPacket(m_id);
		}
		else {
			reinterpret_cast<CPlayer*>(SharedData::g_clients[pl])->SendMoveObjectPacket(m_id);
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

void CNPC::MoveToTarget()
{

}

void InitializeNPC()
{
	for (int i = MAX_USER; i < MAX_USER + MAX_NPC; ++i) {
		CNPC* npc = reinterpret_cast<CNPC*>(SharedData::g_clients[i]);
		npc->m_id = i;
		npc->m_x = rand() % W_WIDTH;
		npc->m_y = rand() % W_HEIGHT;

		while (SharedData::g_map[npc->m_x][npc->m_y] == false) {
			npc->m_x = rand() % W_WIDTH;
			npc->m_y = rand() % W_HEIGHT;
		}

		npc->m_state = ST_INGAME;
		npc->m_attack_type = static_cast<NPC_ATTACK_TYPE>(rand() % 2);
		npc->m_move_type = static_cast<NPC_MOVE_TYPE>(rand() % 2);

		if (npc->m_move_type == NPC_LOAMING) {
			npc->m_loaming_x = npc->m_x;
			npc->m_loaming_y = npc->m_y;
			npc->m_npc_state = NPC_MOVE;
		}
		else {
			npc->m_npc_state = NPC_IDLE;
		}

		// lua 연동하면 이거 빼자
		npc->m_hp = 100;
		npc->m_level = 1;
		
		npc->m_target_id = -1;

		sprintf_s(npc->m_name, "%d%d_NPC%d", npc->m_attack_type, npc->m_move_type, i);

		SharedData::g_sector.InsertSector(i, npc->m_x, npc->m_y);

		auto L = npc->m_L = luaL_newstate();
		luaL_openlibs(L);
		int error = luaL_loadfile(L, "npc.lua") ||
			lua_pcall(L, 0, 0, 0);
		lua_getglobal(L, "set_uid");
		lua_pushnumber(L, i);
		lua_pcall(L, 1, 0, 0);
		lua_pop(L, 1);

		lua_getglobal(L, "Initialize");
		lua_pushnumber(L, static_cast<int>(npc->m_attack_type));
		lua_pushnumber(L, static_cast<int>(npc->m_move_type));
		error = lua_pcall(L, 2, 2, 0);
		if (error) {
			cout << "Error : " << lua_tostring(L, -1);
			lua_pop(L, 1);
		}
		npc->m_level = (int)lua_tonumber(L, -2);
		npc->m_hp = (int)lua_tonumber(L, -1);
		lua_pop(L, 3);

		//lua_register(L, "API_SendMessage", API_SendMessage);
		//lua_register(L, "API_get_x", API_get_x);
		//lua_register(L, "API_get_y", API_get_y);
		//lua_register(L, "API_Run", API_Run);
		//lua_register(L, "API_do_npc_run", API_do_npc_run);
	}
	cout << "NPC Initialize End" << endl;
}

void WakeUpNPC(int npc_id, int player_id)
{
	if (is_pc(npc_id)) {
		cout << "WakeUpNpc ERROR : this is pc" << endl;
	}

	CNPC* npc = reinterpret_cast<CNPC*>(SharedData::g_clients[npc_id]);
	npc->m_s_lock.lock();
	if (npc->m_state != ST_INGAME) {
		npc->m_s_lock.unlock();
		return;
	}
	npc->m_s_lock.unlock();
	if (npc->m_is_active) return;
	if (npc->m_hp <= 0) return;

	// 어그로 깨워주기
	if (npc->Agro_IsInArea() == true) {
		// 타겟을 찾아 갑니당
	}

	// AI 가동
	bool old_state = false;
	if (false == atomic_compare_exchange_strong(&npc->m_is_active, &old_state, true)) return;

	TIMER_EVENT ev{ npc_id, chrono::system_clock::now(), EV_NPC_AI, 0 };
	SharedData::timer_queue.push(ev);	
}