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

void InitializeNPC()
{
	cout << "NPC Initialize Start" << endl;
	for (int i = MAX_USER; i < MAX_USER + MAX_NPC; ++i) {
		CNPC* npc = reinterpret_cast<CNPC*>(SharedData::g_clients[i]);
		npc->m_x = rand() % 200;
		npc->m_y = rand() % 200;
		npc->m_id = i;
		sprintf_s(npc->m_name, "NPC%d", i);
		npc->m_state = ST_INGAME;
		npc->m_attack_type = NPC_AGRO;
		npc->m_move_type = NPC_LOAMING;
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