#include "Astar.h"

// 1. ������ p
// 2. p�� f, g, h �Ҵ�
//		- f = g + h
//		- g : ��������� ���� ��� ��ġ������ �Ÿ�
//		- h : ���� ��� ��ġ���� ��ǥ�������� �޸���ƽ�� �Ÿ�
// 3. Open List�� p �ֱ�
// 4. B = OpenList���� ���� ���� f���� ���� ���
//		- b == ��ǥ�� -> ��� �ϼ�
//		- openList�� ������� -> ��ǥ�������� ��ΰ� �������� ����
// 5. C = B�� ����� ���
//		- C�� f, g, h�� �Ҵ�
//		- Open/Close List���� C�� �ִ��� üũ
//			- ������ : �� ���� f������ ��� ������Ʈ
//			- ������ : C�� OpenList�� �ֱ�
// 6. 4������ ���ư���.

// 1. ���� �����Ѵ�
// 2. ������, ��ǥ���� �����ϰ� block�� �ʿ��ϴٸ� �����ش�
// 3. �������� OpenList�� �ִ´�
// 4. OpenList�� ����ų�, ���� ���� f���� ���� ��尡 ��ǥ���� ������ �ݺ��ϳ�
//		- OpenList���� ���� ���� f���� ���� ��� �˻� (���ʿ��� ������)
//		- ���� ��带 ClosedList�� �ְ� OpenList���� �����
//		- ClosedList�� �����¿� 4������ Ž���ϰ� OpenList�� �ִ´� (���̸� ����)
//		- ClosedList�� ������ ����


void CAstar::Initialize()
{
	//m_openList = priority_queue<NODE*, vector<NODE*>, compareNode>();
	m_closedList.clear();
	m_current = nullptr;
	m_path.clear();

	m_start->g = 0;
	m_start->h = GetDistEstimate(m_start, m_goal);			// �޸���ƽ

	m_start->f = m_start->g + m_start->h;
	m_start->parent = nullptr;

	m_openList.push(m_start);
}

bool CAstar::FindPath()
{
	Initialize();

	while (!m_openList.empty())
	{
		// m_current = ���� ���� f�� ���� ��� 
		m_current = m_openList.top();
		m_openList.pop();

		m_closedList.emplace_back(m_current);

		// ��ã�� ����
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

		// ���� 4���� Ž��

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

		//	// �̹� ���� ���� Ž������
		//	list<NODE*>::iterator it = std::find(m_closedList.begin(), m_closedList.end(), now);
		//	if (it != m_closedList.end()) continue;

		//	// �� ������ Ž������
		//	if (SharedData::g_map[now->x][now->y] == false) continue;

		//	//int new_g = m_current->g + GetCost(now);
		//	//if (now->g <= new_g) continue;
		//	
		//	// ���� Ž���ϰ� �ִ� ����� f ���ϱ�
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

			// �̹� ���� ���� Ž������
			list<NODE*>::iterator it = std::find(m_closedList.begin(), m_closedList.end(), now);
			if (it != m_closedList.end()) {
				delete now;
				continue;
			}

			// �� ������ Ž������

			//int new_g = m_current->g + GetCost(now);
			//if (now->g <= new_g) continue;

			// ���� Ž���ϰ� �ִ� ����� f ���ϱ�
			//now->parent = m_current;
			now->g = GetCost(now);
			now->h = GetDistEstimate(now, m_goal);
			now->f = now->g + now->h;

			//��κ� �̻��ѵ�	
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

		//	// �̹� ���� ���� Ž������
		//	list<NODE*>::iterator it = std::find(m_closedList.begin(), m_closedList.end(), &now);
		//	if (it != m_closedList.end()) {
		//		continue;
		//	}

		//	// �� ������ Ž������

		//	//int new_g = m_current->g + GetCost(now);
		//	//if (now->g <= new_g) continue;

		//	// ���� Ž���ϰ� �ִ� ����� f ���ϱ�
		//	//now->parent = m_current;
		//	now.g = GetCost(&now);
		//	now.h = GetDistEstimate(&now, m_goal);
		//	now.f = now.g + now.h;

		//	//��κ� �̻��ѵ�	
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