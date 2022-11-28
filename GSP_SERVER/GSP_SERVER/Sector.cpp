#include "Sector.h"
#include "Player.h"

void CSector::UpdateSector(short id, short x, short y, short oldx, short oldy)
{
	short sctrX = x / SECTOR_SIZE;
	short sctrY = y / SECTOR_SIZE;

	if (m_sectorObjList[sctrX][sctrY].count(id) == 0) {
		m_sectorObjList[sctrX][sctrY].insert(id);						// ���� ���� �¿� ������ �߰��ϰ�

		short oldsctrX = oldx / SECTOR_SIZE;
		short oldsctrY = oldx / SECTOR_SIZE;

		if (m_sectorObjList[oldsctrX][oldsctrY].count(id) != 0) {		// ���� ��ġ ���� �¿��� ����
			m_sectorObjList[oldsctrX][oldsctrY].erase(id);
		}
	}		// ���� ���� �¿� ������ �׳� �Ѿ
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
	// �� �����ؼ� nearlist ���� �� �˻��� ���� �����ϱ�

	short xIdx = x / SECTOR_SIZE;			// sector�� �ε���
	short yIdx = y / SECTOR_SIZE;

	// �Ʒ� 4���� ��쿡 ���� �����¿� 4���� �ִ� ������Ʈ���� nearlist�� ����

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

	short xIdx = x / SECTOR_SIZE;			// sector�� �ε���
	short yIdx = y / SECTOR_SIZE;
	if (xIdx < 1) xIdx = 1;
	if (yIdx < 1) yIdx = 1;
	if (xIdx > SECTOR_CNT - 1) xIdx = SECTOR_CNT - 1;
	if (yIdx > SECTOR_CNT - 1) yIdx = SECTOR_CNT - 1;

	if (x < xIdx * SECTOR_SIZE + SECTOR_HALF) {
		if (y < yIdx * SECTOR_SIZE + SECTOR_HALF) {
			for (auto& cl : m_sectorObjList[xIdx - 1][yIdx - 1]) {			// ���⸦ ������ sector�� ����
				if (SharedData::g_clients[cl].m_state != ST_INGAME) continue;
				if (SharedData::g_clients[cl].m_id == id) continue;
				if (SharedData::g_clients[id].CanSee(cl))
					nearlist.insert(cl);
			}
			for (auto& cl : m_sectorObjList[xIdx][yIdx - 1]) {			// ���⸦ ������ sector�� ����
				if (SharedData::g_clients[cl].m_state != ST_INGAME) continue;
				if (SharedData::g_clients[cl].m_id == id) continue;
				if (SharedData::g_clients[id].CanSee(cl))
					nearlist.insert(cl);
			}
			for (auto& cl : m_sectorObjList[xIdx - 1][yIdx]) {			// ���⸦ ������ sector�� ����
				if (SharedData::g_clients[cl].m_state != ST_INGAME) continue;
				if (SharedData::g_clients[cl].m_id == id) continue;
				if (SharedData::g_clients[id].CanSee(cl))
					nearlist.insert(cl);
			}
			for (auto& cl : m_sectorObjList[xIdx][yIdx]) {			// ���⸦ ������ sector�� ����
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
			for (auto& cl : m_sectorObjList[xIdx - 1][yIdx]) {			// ���⸦ ������ sector�� ����
				if (SharedData::g_clients[cl].m_state != ST_INGAME) continue;
				if (SharedData::g_clients[cl].m_id == id) continue;
				if (SharedData::g_clients[id].CanSee(cl))
					nearlist.insert(cl);
			}
			for (auto& cl : m_sectorObjList[xIdx][yIdx]) {			// ���⸦ ������ sector�� ����
				if (SharedData::g_clients[cl].m_state != ST_INGAME) continue;
				if (SharedData::g_clients[cl].m_id == id) continue;
				if (SharedData::g_clients[id].CanSee(cl))
					nearlist.insert(cl);
			}
			for (auto& cl : m_sectorObjList[xIdx - 1][yIdx + 1]) {			// ���⸦ ������ sector�� ����
				if (SharedData::g_clients[cl].m_state != ST_INGAME) continue;
				if (SharedData::g_clients[cl].m_id == id) continue;
				if (SharedData::g_clients[id].CanSee(cl))
					nearlist.insert(cl);
			}
			for (auto& cl : m_sectorObjList[xIdx][yIdx + 1]) {			// ���⸦ ������ sector�� ����
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
			for (auto& cl : m_sectorObjList[xIdx][yIdx]) {			// ���⸦ ������ sector�� ����
				if (SharedData::g_clients[cl].m_state != ST_INGAME) continue;
				if (SharedData::g_clients[cl].m_id == id) continue;
				if (SharedData::g_clients[id].CanSee(cl))
					nearlist.insert(cl);
			}
			for (auto& cl : m_sectorObjList[xIdx + 1][yIdx]) {			// ���⸦ ������ sector�� ����
				if (SharedData::g_clients[cl].m_state != ST_INGAME) continue;
				if (SharedData::g_clients[cl].m_id == id) continue;
				if (SharedData::g_clients[id].CanSee(cl))
					nearlist.insert(cl);
			}
			for (auto& cl : m_sectorObjList[xIdx][yIdx + 1]) {			// ���⸦ ������ sector�� ����
				if (SharedData::g_clients[cl].m_state != ST_INGAME) continue;
				if (SharedData::g_clients[cl].m_id == id) continue;
				if (SharedData::g_clients[id].CanSee(cl))
					nearlist.insert(cl);
			}
			for (auto& cl : m_sectorObjList[xIdx + 1][yIdx + 1]) {			// ���⸦ ������ sector�� ����
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
			for (auto& cl : m_sectorObjList[xIdx][yIdx - 1]) {			// ���⸦ ������ sector�� ����
				if (SharedData::g_clients[cl].m_state != ST_INGAME) continue;
				if (SharedData::g_clients[cl].m_id == id) continue;
				if (SharedData::g_clients[id].CanSee(cl))
					nearlist.insert(cl);
			}
			for (auto& cl : m_sectorObjList[xIdx + 1][yIdx - 1]) {			// ���⸦ ������ sector�� ����
				if (SharedData::g_clients[cl].m_state != ST_INGAME) continue;
				if (SharedData::g_clients[cl].m_id == id) continue;
				if (SharedData::g_clients[id].CanSee(cl))
					nearlist.insert(cl);
			}
			for (auto& cl : m_sectorObjList[xIdx][yIdx]) {			// ���⸦ ������ sector�� ����
				if (SharedData::g_clients[cl].m_state != ST_INGAME) continue;
				if (SharedData::g_clients[cl].m_id == id) continue;
				if (SharedData::g_clients[id].CanSee(cl))
					nearlist.insert(cl);
			}
			for (auto& cl : m_sectorObjList[xIdx + 1][yIdx]) {			// ���⸦ ������ sector�� ����
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