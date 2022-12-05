#include "Sector.h"
#include "Player.h"
#include "function.h"

void CSector::UpdateSector(short id, short x, short y, short oldx, short oldy)
{
	short sctrX = x / SECTOR_SIZE;
	short sctrY = y / SECTOR_SIZE;

	if (m_sectorObjList[sctrX][sctrY].count(id) == 0) {
		m_sectorObjList[sctrX][sctrY].insert(id);						// 현재 섹터 셋에 없으면 추가하고

		short oldsctrX = oldx / SECTOR_SIZE;
		short oldsctrY = oldx / SECTOR_SIZE;

		if (m_sectorObjList[oldsctrX][oldsctrY].count(id) != 0) {		// 옛날 위치 섹터 셋에서 제거
			m_sectorObjList[oldsctrX][oldsctrY].erase(id);
		}
	}		// 현재 섹터 셋에 있으면 그냥 넘어감
}

void CSector::InsertSector(short id, short x, short y)
{
	short sctrX = x / SECTOR_SIZE;
	short sctrY = y / SECTOR_SIZE;

	if (m_sectorObjList[sctrX][sctrY].count(id) == 0) {
		m_sectorObjList[sctrX][sctrY].insert(id);
	}
}

void CSector::RemoveSector(short id, short x, short y)
{
	short sctrX = x / SECTOR_SIZE;
	short sctrY = y / SECTOR_SIZE;

	if (m_sectorObjList[sctrX][sctrY].count(id) != 0) {
		m_sectorObjList[sctrX][sctrY].erase(id);
	}
}

void CSector::FindNear(std::unordered_set<short>& nearlist, int id, int xIdx, int yIdx)
{
	for (auto& cl : m_sectorObjList[xIdx][yIdx]) {
		if (SharedData::g_clients[cl]->m_state != ST_INGAME) continue;
		if (SharedData::g_clients[cl]->m_id == id) continue;
		if (can_see(id, cl))
			nearlist.insert(cl);
	}
}

void CSector::CreateNearList(std::unordered_set<short>& nearlist, short id, short x, short y)
{
	// 0 2
	// 1 3
	short xIdx = x / SECTOR_SIZE;			// sector의 인덱스
	short yIdx = y / SECTOR_SIZE;
	if (xIdx < 1) xIdx = 1;
	if (yIdx < 1) yIdx = 1;
	if (xIdx > SECTOR_CNT - 2) xIdx = SECTOR_CNT - 2;
	if (yIdx > SECTOR_CNT - 2) yIdx = SECTOR_CNT - 2;

	if (x < xIdx * SECTOR_SIZE + SECTOR_HALF) {
		if (y < yIdx * SECTOR_SIZE + SECTOR_HALF) {
			FindNear(nearlist, id, xIdx - 1, yIdx - 1);
			FindNear(nearlist, id, xIdx, yIdx - 1);
			FindNear(nearlist, id, xIdx - 1, yIdx);
			FindNear(nearlist, id, xIdx, yIdx);
		}
		else {
			FindNear(nearlist, id, xIdx - 1, yIdx);
			FindNear(nearlist, id, xIdx, yIdx);
			FindNear(nearlist, id, xIdx - 1, yIdx + 1);
			FindNear(nearlist, id, xIdx, yIdx + 1);
		}
	}
	else {
		if (y < yIdx * SECTOR_SIZE + SECTOR_HALF) {
			FindNear(nearlist, id, xIdx, yIdx);
			FindNear(nearlist, id, xIdx + 1, yIdx);
			FindNear(nearlist, id, xIdx, yIdx - 1);
			FindNear(nearlist, id, xIdx + 1, yIdx - 1);
		}
		else {
			FindNear(nearlist, id, xIdx, yIdx + 1);
			FindNear(nearlist, id, xIdx + 1, yIdx + 1);
			FindNear(nearlist, id, xIdx, yIdx);
			FindNear(nearlist, id, xIdx + 1, yIdx);
		}
	}
}