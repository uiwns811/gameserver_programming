#include "Astar.h"

// 1. 시작점 p
// 2. p에 f, g, h 할당
//		- f = g + h
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


void CAstar::Initialize()
{
	//m_openList = priority_queue<NODE*, vector<NODE*>, compareNode>();
	m_closedList.clear();
	m_current = nullptr;
	m_path.clear();

	m_start->g = 0;
	m_start->h = GetDistEstimate(m_start, m_goal);			// 휴리스틱

	m_start->f = m_start->g + m_start->h;
	m_start->parent = nullptr;

	m_openList.push(m_start);
}

bool CAstar::FindPath()
{
	Initialize();

	while (!m_openList.empty())
	{
		// m_current = 가장 작은 f값 가진 노드 
		m_current = m_openList.top();
		m_openList.pop();

		m_closedList.emplace_back(m_current);

		// 길찾기 성공
		if (m_current->x == m_goal->x && m_current->y == m_goal->y) {
			NODE* current = m_current;
			while (current != nullptr) {
				m_path.emplace_back(*current);
				current = current->parent;
			}
			reverse(m_path.begin(), m_path.end());

			while (!m_openList.empty()) {
				auto iter = m_openList.top();
				delete iter;
				m_openList.pop();
			}
			return true;
		}

		// 주위 4방향 탐색

		//for (int i = 0; i < 4; ++i) {
		//	NODE* now;
		//	//NODE now = NODE(m_current);
		//	switch (i) {
		//	case 0:
		//		//now.x = m_current->x - 1;
		//		//now.y = m_current->y;
		//		now = new NODE(m_current->x - 1, m_current->y, m_current);
		//		break;
		//	case 1:
		//		//now.x = m_current->x + 1;
		//		//now.y = m_current->y;
		//		now = new NODE(m_current->x + 1, m_current->y, m_current);
		//		break;
		//	case 2:
		//		//now.x = m_current->x;
		//		//now.y = m_current->y - 1; 
		//		now = new NODE(m_current->x, m_current->y - 1, m_current);
		//		break;
		//	case 3:				
		//		//now.x = m_current->x;
		//		//now.y = m_current->y + 1;
		//		now = new NODE(m_current->x, m_current->y + 1, m_current);
		//		break;
		//	}

		//	// 이미 갔던 노드면 탐색안함
		//	list<NODE*>::iterator it = std::find(m_closedList.begin(), m_closedList.end(), now);
		//	if (it != m_closedList.end()) continue;

		//	// 벽 있으면 탐색안함
		//	if (SharedData::g_map[now->x][now->y] == false) continue;

		//	//int new_g = m_current->g + GetCost(now);
		//	//if (now->g <= new_g) continue;
		//	
		//	// 지금 탐색하고 있는 노드의 f 구하기
		//	//now->parent = m_current;
		//	now->g = GetCost(now);
		//	now->h = GetDistEstimate(now, m_goal);
		//	now->f = now->g + now->h;
		//	
		//	m_openList.push(now);
		//}

		for (int i = 0; i < 4; ++i) {
			//NODE now = new NODE(m_current);
			int x, y;
			switch (i) {
			case 0:
				x = m_current->x - 1;
				y = m_current->y;
				//now = new NODE(m_current->x - 1, m_current->y, m_current);
				break;
			case 1:
				x = m_current->x + 1;
				y = m_current->y;
				//now = new NODE(m_current->x + 1, m_current->y, m_current);
				break;
			case 2:
				x = m_current->x;
				y = m_current->y - 1; 
				//now = new NODE(m_current->x, m_current->y - 1, m_current);
				break;
			case 3:
				x = m_current->x;
				y = m_current->y + 1;
				//now = new NODE(m_current->x, m_current->y + 1, m_current);
				break;
			}

			if (SharedData::g_map[x][y] == false) continue;
			
			NODE* now = new NODE(x, y, m_current);

			// 이미 갔던 노드면 탐색안함
			list<NODE*>::iterator it = std::find(m_closedList.begin(), m_closedList.end(), now);
			if (it != m_closedList.end()) {
				delete now;
				continue;
			}

			// 벽 있으면 탐색안함

			//int new_g = m_current->g + GetCost(now);
			//if (now->g <= new_g) continue;

			// 지금 탐색하고 있는 노드의 f 구하기
			//now->parent = m_current;
			now->g = GetCost(now);
			now->h = GetDistEstimate(now, m_goal);
			now->f = now->g + now->h;

			//요부분 이상한듯	
			m_openList.push(now);
			//delete now;
			//now = nullptr;
		}

		//for (int i = 0; i < 4; ++i) {
		//	int x, y;
		//	switch (i) {
		//	case 0:
		//		x = m_current->x - 1;
		//		y = m_current->y;
		//		//now = new NODE(m_current->x - 1, m_current->y, m_current);
		//		break;
		//	case 1:
		//		x = m_current->x + 1;
		//		y = m_current->y;
		//		//now = new NODE(m_current->x + 1, m_current->y, m_current);
		//		break;
		//	case 2:
		//		x = m_current->x;
		//		y = m_current->y - 1;
		//		//now = new NODE(m_current->x, m_current->y - 1, m_current);
		//		break;
		//	case 3:
		//		x = m_current->x;
		//		y = m_current->y + 1;
		//		//now = new NODE(m_current->x, m_current->y + 1, m_current);
		//		break;
		//	}

		//	if (SharedData::g_map[x][y] == false) continue;

		//	NODE now = NODE(x, y, m_current);

		//	// 이미 갔던 노드면 탐색안함
		//	list<NODE*>::iterator it = std::find(m_closedList.begin(), m_closedList.end(), &now);
		//	if (it != m_closedList.end()) {
		//		continue;
		//	}

		//	// 벽 있으면 탐색안함

		//	//int new_g = m_current->g + GetCost(now);
		//	//if (now->g <= new_g) continue;

		//	// 지금 탐색하고 있는 노드의 f 구하기
		//	//now->parent = m_current;
		//	now.g = GetCost(&now);
		//	now.h = GetDistEstimate(&now, m_goal);
		//	now.f = now.g + now.h;

		//	//요부분 이상한듯	
		//	m_openList.push(&now);
		//	//delete now;
		//	//now = nullptr;
		//}
	}

	return false;
}

int CAstar::GetDistEstimate(NODE* from, NODE* to)
{
	return abs(from->x - to->x) + abs(from->y - to->y);
}

int CAstar::GetCost(NODE* node)
{
	if (node->parent == nullptr)
		return 0;
	else
		return node->parent->g + 1;
}