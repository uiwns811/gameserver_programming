#include "Sector.h"
#include "Player.h"

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

void CSector::FindSector(short id, short x, short y)
{
	// 0 3
	// 1 2
	// 로 구분해서 nearlist 만들 때 검색할 섹터 지정하기

	short xIdx = x / SECTOR_SIZE;			// sector의 인덱스
	short yIdx = y / SECTOR_SIZE;

	// 아래 4가지 경우에 따라 상하좌우 4개에 있는 오브젝트들을 nearlist에 넣자

	if (x < xIdx * SECTOR_SIZE + SECTOR_HALF) {
		if (y < yIdx * SECTOR_SIZE + SECTOR_HALF) {
			// m_sectorObjList[xIdx-1][yIdx-1]
			// m_sectorObjList[xIdx][yIdx-1]
			// m_sectorObjList[xIdx-1][yIdx]
			// m_sectorObjList[xIdx][yIdx]
		}
		else {									
			// m_sectorObjList[xIdx-1][yIdx]
			// m_sectorObjList[xIdx][yIdx]
			// m_sectorObjList[xIdx-1][yIdx+1]
			// m_sectorObjList[xIdx][yIdx+1]
		}
	}
	else {
		if (y < yIdx * SECTOR_SIZE + SECTOR_HALF) {
			// m_sectorObjList[xIdx][yIdx]
			// m_sectorObjList[xIdx+1][yIdx]
			// m_sectorObjList[xIdx][yIdx+1]
			// m_sectorObjList[xIdx+1][yIdx+1]
		}
		else {
			// m_sectorObjList[xIdx][yIdx-1]
			// m_sectorObjList[xIdx+1][yIdx-1]
			// m_sectorObjList[xIdx][yIdx]
			// m_sectorObjList[xIdx+1][yIdx]
		}
	}
}

void CSector::CreateNearList(std::unordered_set<short>& nearlist, short id, short x, short y)
{

	short xIdx = x / SECTOR_SIZE;			// sector의 인덱스
	short yIdx = y / SECTOR_SIZE;
	if (xIdx < 1) xIdx = 1;
	if (yIdx < 1) yIdx = 1;
	if (xIdx > SECTOR_CNT - 1) xIdx = SECTOR_CNT - 1;
	if (yIdx > SECTOR_CNT - 1) yIdx = SECTOR_CNT - 1;

	if (x < xIdx * SECTOR_SIZE + SECTOR_HALF) {
		if (y < yIdx * SECTOR_SIZE + SECTOR_HALF) {
			for (auto& cl : m_sectorObjList[xIdx - 1][yIdx - 1]) {			// 여기를 인접한 sector로 구현
				if (SharedData::g_clients[cl].m_state != ST_INGAME) continue;
				if (SharedData::g_clients[cl].m_id == id) continue;
				if (SharedData::g_clients[id].CanSee(cl))
					nearlist.insert(cl);
			}
			for (auto& cl : m_sectorObjList[xIdx][yIdx - 1]) {			// 여기를 인접한 sector로 구현
				if (SharedData::g_clients[cl].m_state != ST_INGAME) continue;
				if (SharedData::g_clients[cl].m_id == id) continue;
				if (SharedData::g_clients[id].CanSee(cl))
					nearlist.insert(cl);
			}
			for (auto& cl : m_sectorObjList[xIdx - 1][yIdx]) {			// 여기를 인접한 sector로 구현
				if (SharedData::g_clients[cl].m_state != ST_INGAME) continue;
				if (SharedData::g_clients[cl].m_id == id) continue;
				if (SharedData::g_clients[id].CanSee(cl))
					nearlist.insert(cl);
			}
			for (auto& cl : m_sectorObjList[xIdx][yIdx]) {			// 여기를 인접한 sector로 구현
				if (SharedData::g_clients[cl].m_state != ST_INGAME) continue;
				if (SharedData::g_clients[cl].m_id == id) continue;
				if (SharedData::g_clients[id].CanSee(cl))
					nearlist.insert(cl);
			}

			//nearlist.insert(m_sectorObjList[xIdx - 1][yIdx - 1].begin(), m_sectorObjList[xIdx - 1][yIdx - 1].end());
			//nearlist.insert(m_sectorObjList[xIdx][yIdx - 1].begin(), m_sectorObjList[xIdx][yIdx - 1].end());
			//nearlist.insert(m_sectorObjList[xIdx - 1][yIdx].begin(), m_sectorObjList[xIdx - 1][yIdx].end());
			//nearlist.insert(m_sectorObjList[xIdx][yIdx].begin(), m_sectorObjList[xIdx][yIdx].end());
			// m_sectorObjList[xIdx-1][yIdx-1]
			// m_sectorObjList[xIdx][yIdx-1]
			// m_sectorObjList[xIdx-1][yIdx]
			// m_sectorObjList[xIdx][yIdx]
		}
		else {
			for (auto& cl : m_sectorObjList[xIdx - 1][yIdx]) {			// 여기를 인접한 sector로 구현
				if (SharedData::g_clients[cl].m_state != ST_INGAME) continue;
				if (SharedData::g_clients[cl].m_id == id) continue;
				if (SharedData::g_clients[id].CanSee(cl))
					nearlist.insert(cl);
			}
			for (auto& cl : m_sectorObjList[xIdx][yIdx]) {			// 여기를 인접한 sector로 구현
				if (SharedData::g_clients[cl].m_state != ST_INGAME) continue;
				if (SharedData::g_clients[cl].m_id == id) continue;
				if (SharedData::g_clients[id].CanSee(cl))
					nearlist.insert(cl);
			}
			for (auto& cl : m_sectorObjList[xIdx - 1][yIdx + 1]) {			// 여기를 인접한 sector로 구현
				if (SharedData::g_clients[cl].m_state != ST_INGAME) continue;
				if (SharedData::g_clients[cl].m_id == id) continue;
				if (SharedData::g_clients[id].CanSee(cl))
					nearlist.insert(cl);
			}
			for (auto& cl : m_sectorObjList[xIdx][yIdx + 1]) {			// 여기를 인접한 sector로 구현
				if (SharedData::g_clients[cl].m_state != ST_INGAME) continue;
				if (SharedData::g_clients[cl].m_id == id) continue;
				if (SharedData::g_clients[id].CanSee(cl))
					nearlist.insert(cl);
			}
			//nearlist.insert(m_sectorObjList[xIdx - 1][yIdx].begin(), m_sectorObjList[xIdx - 1][yIdx].end());
			//nearlist.insert(m_sectorObjList[xIdx][yIdx].begin(), m_sectorObjList[xIdx][yIdx].end());
			//nearlist.insert(m_sectorObjList[xIdx - 1][yIdx + 1].begin(), m_sectorObjList[xIdx - 1][yIdx + 1].end());
			//nearlist.insert(m_sectorObjList[xIdx][yIdx + 1].begin(), m_sectorObjList[xIdx][yIdx + 1].end());
			// m_sectorObjList[xIdx-1][yIdx]
			// m_sectorObjList[xIdx][yIdx]
			// m_sectorObjList[xIdx-1][yIdx+1]
			// m_sectorObjList[xIdx][yIdx+1]
		}
	}
	else {
		if (y < yIdx * SECTOR_SIZE + SECTOR_HALF) {
			for (auto& cl : m_sectorObjList[xIdx][yIdx]) {			// 여기를 인접한 sector로 구현
				if (SharedData::g_clients[cl].m_state != ST_INGAME) continue;
				if (SharedData::g_clients[cl].m_id == id) continue;
				if (SharedData::g_clients[id].CanSee(cl))
					nearlist.insert(cl);
			}
			for (auto& cl : m_sectorObjList[xIdx + 1][yIdx]) {			// 여기를 인접한 sector로 구현
				if (SharedData::g_clients[cl].m_state != ST_INGAME) continue;
				if (SharedData::g_clients[cl].m_id == id) continue;
				if (SharedData::g_clients[id].CanSee(cl))
					nearlist.insert(cl);
			}
			for (auto& cl : m_sectorObjList[xIdx][yIdx + 1]) {			// 여기를 인접한 sector로 구현
				if (SharedData::g_clients[cl].m_state != ST_INGAME) continue;
				if (SharedData::g_clients[cl].m_id == id) continue;
				if (SharedData::g_clients[id].CanSee(cl))
					nearlist.insert(cl);
			}
			for (auto& cl : m_sectorObjList[xIdx + 1][yIdx + 1]) {			// 여기를 인접한 sector로 구현
				if (SharedData::g_clients[cl].m_state != ST_INGAME) continue;
				if (SharedData::g_clients[cl].m_id == id) continue;
				if (SharedData::g_clients[id].CanSee(cl))
					nearlist.insert(cl);
			}
			//nearlist.insert(m_sectorObjList[xIdx][yIdx].begin(), m_sectorObjList[xIdx][yIdx].end());
			//nearlist.insert(m_sectorObjList[xIdx + 1][yIdx].begin(), m_sectorObjList[xIdx + 1][yIdx].end());
			//nearlist.insert(m_sectorObjList[xIdx][yIdx + 1].begin(), m_sectorObjList[xIdx][yIdx + 1].end());
			//nearlist.insert(m_sectorObjList[xIdx + 1][yIdx + 1].begin(), m_sectorObjList[xIdx + 1][yIdx + 1].end());
			// m_sectorObjList[xIdx][yIdx]
			// m_sectorObjList[xIdx+1][yIdx]
			// m_sectorObjList[xIdx][yIdx+1]
			// m_sectorObjList[xIdx+1][yIdx+1]
		}
		else {
			for (auto& cl : m_sectorObjList[xIdx][yIdx - 1]) {			// 여기를 인접한 sector로 구현
				if (SharedData::g_clients[cl].m_state != ST_INGAME) continue;
				if (SharedData::g_clients[cl].m_id == id) continue;
				if (SharedData::g_clients[id].CanSee(cl))
					nearlist.insert(cl);
			}
			for (auto& cl : m_sectorObjList[xIdx + 1][yIdx - 1]) {			// 여기를 인접한 sector로 구현
				if (SharedData::g_clients[cl].m_state != ST_INGAME) continue;
				if (SharedData::g_clients[cl].m_id == id) continue;
				if (SharedData::g_clients[id].CanSee(cl))
					nearlist.insert(cl);
			}
			for (auto& cl : m_sectorObjList[xIdx][yIdx]) {			// 여기를 인접한 sector로 구현
				if (SharedData::g_clients[cl].m_state != ST_INGAME) continue;
				if (SharedData::g_clients[cl].m_id == id) continue;
				if (SharedData::g_clients[id].CanSee(cl))
					nearlist.insert(cl);
			}
			for (auto& cl : m_sectorObjList[xIdx + 1][yIdx]) {			// 여기를 인접한 sector로 구현
				if (SharedData::g_clients[cl].m_state != ST_INGAME) continue;
				if (SharedData::g_clients[cl].m_id == id) continue;
				if (SharedData::g_clients[id].CanSee(cl))
					nearlist.insert(cl);
			}

			//nearlist.insert(m_sectorObjList[xIdx][yIdx - 1].begin(), m_sectorObjList[xIdx][yIdx - 1].end());
			//nearlist.insert(m_sectorObjList[xIdx + 1][yIdx - 1].begin(), m_sectorObjList[xIdx + 1][yIdx - 1].end());
			//nearlist.insert(m_sectorObjList[xIdx][yIdx].begin(), m_sectorObjList[xIdx][yIdx].end());
			//nearlist.insert(m_sectorObjList[xIdx + 1][yIdx].begin(), m_sectorObjList[xIdx + 1][yIdx].end());
			// m_sectorObjList[xIdx][yIdx-1]
			// m_sectorObjList[xIdx+1][yIdx-1]
			// m_sectorObjList[xIdx][yIdx]
			// m_sectorObjList[xIdx+1][yIdx]
		}
	}
}