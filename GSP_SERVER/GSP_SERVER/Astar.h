#pragma once
#include "stdafx.h"

// 1. ������ p
// 2. p�� f, g, h �Ҵ�
//		- f = g + h (����� ���)
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

// Cost Function
// F = G + H
//	- F : ��� n�� ���
//	- G : ���������� n������ �ּҺ��
//	- H : ������������ �ٻ���
//

struct NODE
{
	int x;
	int y;

	int f;		// f = g + h
	int g;		// ��������� ���� ��� ��ġ������ �Ÿ�
	int h;		// ���� ��� ��ġ���� ��ǥ�������� �޸���ƽ �Ÿ�

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