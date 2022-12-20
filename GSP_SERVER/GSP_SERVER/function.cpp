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

void InitializeMap()
{
	ifstream fin("../../GSP_CLIENT/GSP_CLIENT/Resource/Map.txt");
	if (fin.fail())
	{
		cout << "Map 파일을 열 수 없음" << endl;
		exit(0);
	}

	for (int i = 0; i < 200; ++i) {
		for (int j = 0; j < 200; ++j) {
			char s_tileInfo[NAME_SIZE];
			fin >> s_tileInfo;
			int tileInfo = stoi(s_tileInfo);

			for (int yCnt = 0; yCnt < 10; yCnt++) {
				for (int xCnt = 0; xCnt < 10; xCnt++) {
					if (tileInfo != 48)
						SharedData::g_map[j + 200 * xCnt][i + 200 * yCnt] = false;
				}
			}
		}
	}

	cout << "Initialize Map Complete" << endl;
}

bool CanAttack(int from, int to)
{
	int x = SharedData::g_clients[from]->m_x;
	int y = SharedData::g_clients[from]->m_y;
	if ((SharedData::g_clients[to]->m_x == (x - 1)) && SharedData::g_clients[to]->m_y == y) return true;
	else if ((SharedData::g_clients[to]->m_x == (x + 1)) && SharedData::g_clients[to]->m_y == y) return true;
	else if (SharedData::g_clients[to]->m_x == x && SharedData::g_clients[to]->m_y == (y - 1)) return true;
	else if (SharedData::g_clients[to]->m_x == x && SharedData::g_clients[to]->m_y == (y + 1)) return true;
	else return false;
}

bool isInAgroArea(int from, int to)
{
	if (abs(SharedData::g_clients[from]->m_x - SharedData::g_clients[to]->m_x) > AGRO_AREA) return false;
	return abs(SharedData::g_clients[from]->m_y - SharedData::g_clients[to]->m_y) <= AGRO_AREA;
}