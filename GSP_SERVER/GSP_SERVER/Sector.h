#pragma once
#include "stdafx.h"

class CSector
{
private:
	std::unordered_set<int> m_sectorObjList[SECTOR_CNT][SECTOR_CNT];			// 각 섹터마다 존재하는 플레이어의 id 모음집

public:

	void UpdateSector(int id, short x, short y, short oldX, short oldY);
	void InsertSector(int id, short x, short y);
	void RemoveSector(int id, short x, short y);

	unordered_set<int> GetSector(int id, short x, short y);

	void FindNearObject(std::unordered_set<int>& nearlist, int id, int xIdx, int yIdx);
	void FindNearPlayer(std::unordered_set<int>& nearlist, int id, int xIdx, int yIdx);

	void CreateNearList(std::unordered_set<int>& nearlist, int id, short x, short y);
	void CreateNearPlayerList(std::unordered_set<int>& nearlist, int id, short x, short y);
};