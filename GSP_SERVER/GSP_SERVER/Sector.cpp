#include "Sector.h"
#include "Player.h"
#include "function.h"

unordered_set<int> CSector::GetSector(int id, short x, short y)
{
	int sctrX = x / SECTOR_SIZE;
	int sctrY = y / SECTOR_SIZE;
	return m_sectorObjList[sctrX][sctrY];
}

void CSector::UpdateSector(int id, short x, short y, short oldx, short oldy)
{
	int sctrX = x / SECTOR_SIZE;
	int sctrY = y / SECTOR_SIZE;

	if (m_sectorObjList[sctrX][sctrY].count(id) == 0) {
		m_sectorObjList[sctrX][sctrY].insert(id);						// 현재 섹터 셋에 없으면 추가하고

		int oldsctrX = oldx / SECTOR_SIZE;
		int oldsctrY = oldx / SECTOR_SIZE;

		if (m_sectorObjList[oldsctrX][oldsctrY].count(id) != 0) {		// 옛날 위치 섹터 셋에서 제거
			m_sectorObjList[oldsctrX][oldsctrY].erase(id);
		}
	}		// 현재 섹터 셋에 있으면 그냥 넘어감
}

void CSector::InsertSector(int id, short x, short y)
{
	int sctrX = x / SECTOR_SIZE;
	int sctrY = y / SECTOR_SIZE;

	if (m_sectorObjList[sctrX][sctrY].count(id) == 0) {
		m_sectorObjList[sctrX][sctrY].insert(id);
	}
}

void CSector::RemoveSector(int id, short x, short y)
{
	int sctrX = x / SECTOR_SIZE;
	int sctrY = y / SECTOR_SIZE;

	if (m_sectorObjList[sctrX][sctrY].count(id) != 0) {
		m_sectorObjList[sctrX][sctrY].erase(id);
	}
}

void CSector::FindNearObject(std::unordered_set<int>& nearlist, int id, int xIdx, int yIdx)
{
	for (auto& cl : m_sectorObjList[xIdx][yIdx]) {
		SharedData::g_clients[cl]->m_s_lock.lock();
		if (SharedData::g_clients[cl]->m_state != ST_INGAME) {
			SharedData::g_clients[cl]->m_s_lock.unlock();
			continue;
		}
		SharedData::g_clients[cl]->m_s_lock.unlock();
		if (SharedData::g_clients[cl]->m_id == id) continue;
		if (can_see(id, cl))
			nearlist.insert(cl);
	}
}

void CSector::FindNearPlayer(std::unordered_set<int>& nearlist, int id, int xIdx, int yIdx)
{
	for (auto& cl : m_sectorObjList[xIdx][yIdx]) {
		SharedData::g_clients[cl]->m_s_lock.lock();
		if (SharedData::g_clients[cl]->m_state != ST_INGAME) {
			SharedData::g_clients[cl]->m_s_lock.unlock();
			continue;
		}
		SharedData::g_clients[cl]->m_s_lock.unlock();
		if (SharedData::g_clients[cl]->m_id == id) continue;
		if (true == is_npc(SharedData::g_clients[cl]->m_id)) continue;
		if (can_see(id, cl))
			nearlist.insert(cl);
	}
}

void CSector::CreateNearList(std::unordered_set<int>& nearlist, int id, short x, short y)
{
	// 0 2
	// 1 3
	int xIdx = x / SECTOR_SIZE;
	int yIdx = y / SECTOR_SIZE;
	if (xIdx < 1) xIdx = 1;
	if (yIdx < 1) yIdx = 1;
	if (xIdx > SECTOR_CNT - 2) xIdx = SECTOR_CNT - 2;
	if (yIdx > SECTOR_CNT - 2) yIdx = SECTOR_CNT - 2;

	if (x < xIdx * SECTOR_SIZE + SECTOR_HALF) {
		if (y < yIdx * SECTOR_SIZE + SECTOR_HALF) {
			FindNearObject(nearlist, id, xIdx - 1, yIdx - 1); FindNearObject(nearlist, id, xIdx, yIdx - 1);	FindNearObject(nearlist, id, xIdx - 1, yIdx);	FindNearObject(nearlist, id, xIdx, yIdx);
		}
		else {
			FindNearObject(nearlist, id, xIdx - 1, yIdx); FindNearObject(nearlist, id, xIdx, yIdx);	FindNearObject(nearlist, id, xIdx - 1, yIdx + 1);	FindNearObject(nearlist, id, xIdx, yIdx + 1);
		}
	}
	else {
		if (y < yIdx * SECTOR_SIZE + SECTOR_HALF) {
			FindNearObject(nearlist, id, xIdx, yIdx);	FindNearObject(nearlist, id, xIdx + 1, yIdx);	FindNearObject(nearlist, id, xIdx, yIdx - 1);	FindNearObject(nearlist, id, xIdx + 1, yIdx - 1);
		}
		else {
			FindNearObject(nearlist, id, xIdx, yIdx + 1);	FindNearObject(nearlist, id, xIdx + 1, yIdx + 1);	FindNearObject(nearlist, id, xIdx, yIdx);	FindNearObject(nearlist, id, xIdx + 1, yIdx);
		}
	}
}

void CSector::CreateNearPlayerList(std::unordered_set<int>& nearlist, int id, short x, short y)
{
	// 0 2
	// 1 3
	int xIdx = x / SECTOR_SIZE;
	int yIdx = y / SECTOR_SIZE;
	if (xIdx < 1) xIdx = 1;
	if (yIdx < 1) yIdx = 1;
	if (xIdx > SECTOR_CNT - 2) xIdx = SECTOR_CNT - 2;
	if (yIdx > SECTOR_CNT - 2) yIdx = SECTOR_CNT - 2;

	if (x < xIdx * SECTOR_SIZE + SECTOR_HALF) {
		if (y < yIdx * SECTOR_SIZE + SECTOR_HALF) {
			FindNearPlayer(nearlist, id, xIdx - 1, yIdx - 1); FindNearPlayer(nearlist, id, xIdx, yIdx - 1);	FindNearPlayer(nearlist, id, xIdx - 1, yIdx);	FindNearPlayer(nearlist, id, xIdx, yIdx);
		}
		else {
			FindNearPlayer(nearlist, id, xIdx - 1, yIdx); FindNearPlayer(nearlist, id, xIdx, yIdx);	FindNearPlayer(nearlist, id, xIdx - 1, yIdx + 1);	FindNearPlayer(nearlist, id, xIdx, yIdx + 1);
		}
	}
	else {
		if (y < yIdx * SECTOR_SIZE + SECTOR_HALF) {
			FindNearPlayer(nearlist, id, xIdx, yIdx);	FindNearPlayer(nearlist, id, xIdx + 1, yIdx);	FindNearPlayer(nearlist, id, xIdx, yIdx - 1);	FindNearPlayer(nearlist, id, xIdx + 1, yIdx - 1);
		}
		else {
			FindNearPlayer(nearlist, id, xIdx, yIdx + 1);	FindNearPlayer(nearlist, id, xIdx + 1, yIdx + 1);	FindNearPlayer(nearlist, id, xIdx, yIdx);	FindNearPlayer(nearlist, id, xIdx + 1, yIdx);
		}
	}
}