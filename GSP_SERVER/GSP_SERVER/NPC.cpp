#include "NPC.h"
#include "Player.h"
#include "Sector.h"

void CNPC::Initialize()
{

}

void CNPC::Tick()
{

}

void CNPC::Move()
{
	switch (m_move_type) {
	case NPC_FIXED:
		break;
	case NPC_LOAMING:
		break;
	}
}

void CNPC::Attack()
{
	switch (m_attack_type) {
	case NPC_PEACE:
		break;
	case NPC_AGRO:
		if (Agro_IsInArea() == true) {
			// 타겟을 찾아 갑니당
		}
		break;
	}
}

bool CNPC::Agro_IsInArea()
{
	// 11x11 영역에 접근하면 쫓아오기
	unordered_set<short> nearlist;
	SharedData::g_sector.CreateNearList(nearlist, m_id, m_x, m_y);
	for (auto& cl : nearlist) {
		if (abs(SharedData::g_clients[m_id]->m_x - SharedData::g_clients[cl]->m_x) > 11) 
			return false;
		if (abs(SharedData::g_clients[m_id]->m_y - SharedData::g_clients[cl]->m_y) <= 11) {
			m_target_id = cl;
			return true;
		}
		else
			return false;
	}
}

void InitializeNPC()
{
	cout << "NPC Initialize Start" << endl;
	for (int i = MAX_USER; i < MAX_USER + MAX_NPC; ++i) {
		CNPC* npc = reinterpret_cast<CNPC*>(SharedData::g_clients[i]);
		npc->m_x = rand() % W_WIDTH;
		npc->m_y = rand() % W_HEIGHT;
		npc->m_state = ST_INGAME;
		npc->m_attack_type = (NPC_ATTACK_TYPE)(rand()%2);
		npc->m_move_type = (NPC_MOVE_TYPE)(rand() % 2);
		npc->m_id = i;

		npc->m_target_id = -1;
		npc->m_peace_is_attacked = false;

		sprintf_s(npc->m_name, "%d%d_NPC%d", npc->m_attack_type, npc->m_move_type, i);
		npc->m_is_run = false;

		SharedData::g_sector.InsertSector(i, npc->m_x, npc->m_y);

		auto L = npc->m_L = luaL_newstate();
		luaL_openlibs(L);
		int error = luaL_loadfile(L, "npc.lua") ||
			lua_pcall(L, 0, 0, 0);
		lua_getglobal(L, "set_uid");
		lua_pushnumber(L, i);
		lua_pcall(L, 1, 0, 0);
		//lua_pop(L, 1);// eliminate set_uid from stack after call

		//lua_register(L, "API_SendMessage", API_SendMessage);
		//lua_register(L, "API_get_x", API_get_x);
		//lua_register(L, "API_get_y", API_get_y);
		//lua_register(L, "API_Run", API_Run);
		//lua_register(L, "API_do_npc_run", API_do_npc_run);
	}
	cout << "NPC Initialize End" << endl;
}