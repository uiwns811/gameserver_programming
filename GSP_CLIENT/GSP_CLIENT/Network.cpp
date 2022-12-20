#include "Network.h"
#include "ObjectMgr.h"
#include "Map.h"
#include "GameFramework.h"
#include "GameGUI.h"
#include "SceneMgr.h"

CNetwork::CNetwork() : m_hostID(-1)
{

}

CNetwork::~CNetwork()
{

}

void CNetwork::Init()
{
	wcout.imbue(locale("korean"));
	sf::Socket::Status status = g_socket.connect("127.0.0.1", PORT_NUM);
	g_socket.setBlocking(false);

	if (status != sf::Socket::Done) {
		wcout << L"서버와 연결할 수 없습니다.\n";
		while (true);
	}

	cout << "ID 입력 : ";
	cin >> m_hostName;

	// 로그인 패킷 전송
	SendLoginPacket(m_hostName);
}

void CNetwork::Update()
{
	char net_buf[BUF_SIZE];
	size_t	received;

	auto recv_result = g_socket.receive(net_buf, BUF_SIZE, received);
	if (recv_result == sf::Socket::Error)
	{
		wcout << L"Recv 에러 (CNetwork::Update)";
		while (true);
	}
	if (recv_result != sf::Socket::NotReady)
		if (received > 0) ProcessData(net_buf, received);
}

void CNetwork::CleanUp()
{
}

void CNetwork::ProcessData(char* net_buf, size_t io_byte)
{
	char* ptr = net_buf;
	static size_t in_packet_size = 0;
	static size_t saved_packet_size = 0;
	static char packet_buffer[BUF_SIZE];

	while (0 != io_byte) {
		if (0 == in_packet_size) in_packet_size = ptr[0];
		if (io_byte + saved_packet_size >= in_packet_size) {
			memcpy(packet_buffer + saved_packet_size, ptr, in_packet_size - saved_packet_size);
			ProcessPacket(packet_buffer);
			ptr += in_packet_size - saved_packet_size;
			io_byte -= in_packet_size - saved_packet_size;
			in_packet_size = 0;
			saved_packet_size = 0;
		}
		else {
			memcpy(packet_buffer + saved_packet_size, ptr, io_byte);
			saved_packet_size += io_byte;
			io_byte = 0;
		}
	}
}

void CNetwork::ProcessPacket(char* packet)
{
	static bool first_time = true;
	switch (packet[1])
	{
	case SC_LOGIN_OK:
	{
		cout << "LOGIN OK!" << endl;
		CSceneMgr::GetInst()->SetRender(true);
	}
	break;
	case SC_LOGIN_FAIL:
	{
		cout << "Login Fail!!" << endl;
		CNetwork::GetInst()->SendLogoutPacket();
		CGameFramework::GetInst()->CloseWindow();
	}
	break;
	case SC_LOGIN_INFO: 
	{
		SC_LOGIN_INFO_PACKET* my_packet = reinterpret_cast<SC_LOGIN_INFO_PACKET*>(packet);
		int id = my_packet->id;
		short x = my_packet->x;
		short y = my_packet->y;
		int exp = my_packet->exp;
		int level = my_packet->level;
		int hp = my_packet->hp;
		int maxhp = my_packet->max_hp;
		CObjectMgr::GetInst()->SetAvatar(id);
		CObjectMgr::GetInst()->AddObject(m_hostName, id, x, y);
		CObjectMgr::GetInst()->SetStat(id, exp, level, hp, maxhp);
		m_hostID = id;
	}
	break;
	case SC_ADD_OBJECT:
	{
		SC_ADD_OBJECT_PACKET* my_packet = reinterpret_cast<SC_ADD_OBJECT_PACKET*>(packet);
		int id = my_packet->id;
		short x = my_packet->x;
		short y = my_packet->y;
		char name[NAME_SIZE];
		strncpy_s(name, my_packet->name, NAME_SIZE);
		CObjectMgr::GetInst()->AddObject(name, id, x, y);
	}
	break;
	case SC_MOVE_OBJECT:
	{
		SC_MOVE_OBJECT_PACKET* my_packet = reinterpret_cast<SC_MOVE_OBJECT_PACKET*>(packet);
		int id = my_packet->id;
		short x = my_packet->x;
		short y = my_packet->y;
		CObjectMgr::GetInst()->Move(id, x, y);
		//cout << id << endl;
		break;
	}

	case SC_REMOVE_OBJECT:
	{
		SC_REMOVE_OBJECT_PACKET* my_packet = reinterpret_cast<SC_REMOVE_OBJECT_PACKET*>(packet);
		int id = my_packet->id;
		if (CObjectMgr::GetInst()->isParty(id) == false)
			CObjectMgr::GetInst()->RemoveObject(id);
	}
	break;
	case SC_ATTACK:
	{
		SC_ATTACK_PACKET* my_packet = reinterpret_cast<SC_ATTACK_PACKET*>(packet);
		int id = my_packet->id;
		CObjectMgr::GetInst()->Attack(id);
	}
	break;
	case SC_CHAT:
	{
		SC_CHAT_PACKET* my_packet = reinterpret_cast<SC_CHAT_PACKET*>(packet);
		int id = my_packet->id;
		CHAT chat;
		if (id == SYSTEM_CHAT_ID) 
			memcpy(chat.name, "SYSTEM", NAME_SIZE);
		else 
			memcpy(chat.name, CObjectMgr::GetInst()->GetName(id), NAME_SIZE);
		memcpy(chat.mess, my_packet->mess, CHAT_SIZE);
		chat.chat_time = chrono::system_clock::now();
		CGameGUI::GetInst()->m_chat_data.emplace_back(chat);
	}
	break;
	case SC_STAT_CHANGE:
	{
		SC_STAT_CHANGE_PACKET* my_packet = reinterpret_cast<SC_STAT_CHANGE_PACKET*>(packet);
		int id = my_packet->id;
		int exp = my_packet->exp;
		int level = my_packet->level;
		int hp = my_packet->hp;
		int maxhp = my_packet->max_hp;
		CObjectMgr::GetInst()->SetStat(id, exp, level, hp, maxhp);
	}
	break;
	case SC_INVITE_PARTY:
	{
		SC_INVITE_PARTY_PACKET* my_packet = reinterpret_cast<SC_INVITE_PARTY_PACKET*>(packet);
		int id = my_packet->id;			// 초대한 애
		CGameGUI::GetInst()->SetRequestedId(id);
		CGameGUI::GetInst()->SetShowPartyRequested(true);
	}
	break;
	case SC_JOIN_PARTY:
	{
		SC_JOIN_PARTY_PACKET* my_packet = reinterpret_cast<SC_JOIN_PARTY_PACKET*>(packet);
		int id = my_packet->id;		// 파티에 새로 들어온 애
		if (id != m_hostID)
			CObjectMgr::GetInst()->JoinParty(id);
		else
			CObjectMgr::GetInst()->JoinParty(CGameGUI::GetInst()->GetRequestedId());
	}
	break;
	case SC_EXIT_PARTY:
	{
		SC_EXIT_PARTY_PACKET* my_packet = reinterpret_cast<SC_EXIT_PARTY_PACKET*>(packet);
		int id = my_packet->id;
		CObjectMgr::GetInst()->ExitParty(id);
	}
	break;
	case SC_PLAYER_DIE:
	break;
	default:
		printf("Unknown PACKET type [%d]\n", packet[1]);
	}
}

void CNetwork::SendPacket(void* packet)
{
	unsigned char* p = reinterpret_cast<unsigned char*>(packet);
	size_t sent = 0;
	g_socket.send(packet, p[0], sent);
}

void CNetwork::SendLoginPacket(char* name)
{
	CS_LOGIN_PACKET p;
	p.size = sizeof(CS_LOGIN_PACKET);
	p.type = CS_LOGIN;
	strcpy_s(p.name, name);
	SendPacket(&p);
}

void CNetwork::SendMovePacket(int direction)
{
	CS_MOVE_PACKET p;
	p.size = sizeof(CS_MOVE_PACKET);
	p.type = CS_MOVE;
	p.direction = direction;
	SendPacket(&p);
}

void CNetwork::SendAttackPacket()
{
	CS_ATTACK_PACKET p;
	p.size = sizeof(CS_MOVE_PACKET);
	p.type = CS_ATTACK;
	SendPacket(&p);
}

void CNetwork::SendLogoutPacket()
{
	CS_LOGOUT_PACKET p;
	p.size = sizeof(CS_LOGOUT_PACKET);
	p.type = CS_LOGOUT;
	SendPacket(&p);
}

void CNetwork::SendChatPacket(const char* mess)
{
	CS_CHAT_PACKET p;
	p.size = sizeof(CS_CHAT_PACKET);
	p.type = CS_CHAT;
	strcpy_s(p.mess, mess);
	SendPacket(&p);
}

void CNetwork::SendInvitePartyPacket()
{
	CS_INVITE_PARTY_PACKET p;
	p.size = sizeof(CS_INVITE_PARTY_PACKET);
	p.type = CS_INVITE_PARTY;
	SendPacket(&p);
}

void CNetwork::SendJoinPartyPacket()
{
	if (!CObjectMgr::GetInst()->GetParty().empty()) return;

	CS_JOIN_PARTY_PACKET p;
	p.size = sizeof(CS_JOIN_PARTY_PACKET);
	p.type = CS_JOIN_PARTY;
	p.id = CGameGUI::GetInst()->GetRequestedId();
	SendPacket(&p);
}

void CNetwork::SendTeleportPacket()
{
	CS_TELEPORT_PACKET p;
	p.size = sizeof(CS_TELEPORT_PACKET);
	p.type = CS_TELEPORT;
	SendPacket(&p);
}