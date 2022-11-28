#pragma once
#include "stdafx.h"

// set...
class CSector
{
private:
	std::unordered_set<short> m_sectorObjList[SECTOR_CNT][SECTOR_CNT];			// �� ���͸��� �����ϴ� �÷��̾��� id ������

public:

	void UpdateSector(short id, short x, short y, short oldX, short oldY);
	void InsertSector(short id, short x, short y);
	void RemoveSector(short id, short x, short y);

	void FindSector(short id, short x, short y);
};