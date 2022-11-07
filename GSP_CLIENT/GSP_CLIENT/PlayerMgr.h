#pragma once
#include "stdafx.h"

class CPlayer;

class CPlayerMgr : public TSingleton<CPlayerMgr>
{
private:
	std::unordered_map<int, CPlayer*> players;
	short m_avatarID = -1;

	sf::Texture* playerTex;
	short m_left = 0;
	short m_top = 0;

public:
	void Init();
	void Render();
	void CleanUp();

	void Move(short id, short x, short y);		
	// 지금 이걸 계속 player.h에 넣어놓고 캡슐화해놨는데, 이걸 playerMgr에서 id로 관리해주는 방식으로 바꾸고 싶음
	// avatar랑 player를 구분해놓지말고 그냥 플레이어에 놓고 isAvatar로 관리해주는게 나을 것 같기도 함 -> Map에서도 바꿔야함
	// 화요일에 서버에서 연동 이동 구현 + 맵 읽어오기
	// 주말까지 충돌처리 해보기

	void GetAvaterPos(short& x, short& y);

	void CreatePlayer(char* name, short id, short x, short y);
	void RemovePlayer(short id);

	void GetTLPos(short& left, short& top) { left = m_left; top = m_top; }
};

