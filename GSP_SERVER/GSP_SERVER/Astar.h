#pragma once
#include "stdafx.h"

// 1. 시작점 p
// 2. p에 f, g, h 할당
//		- f = g + h (노드의 비용)
//		- g : 출발점부터 현재 노드 위치까지의 거리
//		- h : 현재 노드 위치부터 목표점까지의 휴리스틱한 거리
// 3. Open List에 p 넣기
// 4. B = OpenList에서 가장 낮은 f값을 가진 노드
//		- b == 목표점 -> 경로 완성
//		- openList가 비었으면 -> 목표점까지의 경로가 존재하지 않음
// 5. C = B에 연결된 노드
//		- C에 f, g, h값 할당
//		- Open/Close List에서 C가 있는지 체크
//			- 있으면 : 더 낮은 f값으로 경로 업데이트
//			- 없으면 : C를 OpenList에 넣기
// 6. 4번으로 돌아간다.


// 1. 맵을 생성한다
// 2. 시작점, 목표점을 설정하고 block이 필요하다면 지어준다
// 3. 시작점을 OpenList에 넣는다
// 4. OpenList가 비었거나, 가장 낮은 f값을 가진 노드가 목표점일 경우까지 반복하낟
//		- OpenList에서 가장 낮은 f값을 가진 노드 검색 (최초에는 시작점)
//		- 현재 노드를 ClosedList에 넣고 OpenList에서 지운다
//		- ClosedList의 상하좌우 4방향을 탐색하고 OpenList에 넣는다 (벽이면 제외)
//		- ClosedList에 있으면 뺀다

// Cost Function
// F = G + H
//	- F : 노드 n의 비용
//	- G : 시작점에서 n까지의 최소비용
//	- H : 도착점까지의 근사비용
//

struct NODE
{
	int x;
	int y;

	int f;		// f = g + h
	int g;		// 출발점부터 현재 노드 위치까지의 거리
	int h;		// 현재 노드 위치부터 목표점까지의 휴리스틱 거리

	NODE* parent;

	NODE(NODE* _parent) 
	{
		x = 0;
		y = 0;
		f = 0;
		g = 0;
		h = 0;
		parent = _parent;
	}

	NODE(int _x, int _y, NODE* _parent)
	{
		x = _x;
		y = _y;
		parent = _parent;
		f = 0;
		g = 0;
		h = 0;
	}
};

struct compareNode {
	bool operator () (NODE* a, NODE* b) const {
		return a->f > b->f;
	}
};

class CAstar
{
private:
	//priority_queue<NODE*> m_openList;
	priority_queue<NODE*, vector<NODE*>, compareNode> m_openList;
	list<NODE*> m_closedList;
	NODE* m_start;
	NODE* m_goal;
	NODE* m_current;
	
	vector<NODE> m_path;

public:
	CAstar() {};
	~CAstar() {};

	void SetStartPoint(NODE* start) { m_start = start; };
	void SetGoalPoint(NODE* goal) { m_goal = goal; };

	int GetDistEstimate(NODE* from, NODE* to);
	int GetCost(NODE* node);
	
	void Initialize();
	bool FindPath();

	vector<NODE> GetPaths() { return m_path; }
	NODE GetPath() { if (m_path.size() <= 1) return m_path[0]; else return m_path[1]; };
};