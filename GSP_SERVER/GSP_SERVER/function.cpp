#include "function.h"
#include "Object.h"

bool is_pc(int object_id)
{
	return object_id < MAX_USER;
}

bool is_npc(int object_id)
{
	return !is_pc(object_id);
}

bool can_see(int from, int to)
{
	if (abs(SharedData::g_clients[from]->m_x - SharedData::g_clients[to]->m_x) > VIEW_RANGE) return false;
	return abs(SharedData::g_clients[from]->m_y - SharedData::g_clients[to]->m_y) <= VIEW_RANGE;
}

int get_new_client_id()
{
	for (int i = 0; i < MAX_USER; ++i) {
		lock_guard <mutex> ll{ SharedData::g_clients[i]->m_s_lock };
		if (SharedData::g_clients[i]->m_state == ST_FREE)
			return i;
	}
	return -1;
}