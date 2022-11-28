#include "Sector.h"

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