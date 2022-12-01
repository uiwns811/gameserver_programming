#pragma once
#include "stdafx.h"

class CSector
{
private:
	std::unordered_set<short> m_sectorObjList[SECTOR_CNT][SECTOR_CNT];			// �� ���͸��� �����ϴ� �÷��̾��� id ������

public:

	void UpdateSector(short id, short x, short y, short oldX, short oldY);
	void InsertSector(short id, short x, short y);
	void RemoveSector(short id, short x, short y);

	void FindNear(std::unordered_set<short>& nearlist, int id, int xIdx, int yIdx);
	void CreateNearList(std::unordered_set<short>& nearlist, short id, short x, short y);
};