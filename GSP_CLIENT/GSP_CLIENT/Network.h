#pragma once
#include "stdafx.h"

class CNetwork : public TSingleton<CNetwork>
{
private:
	sf::TcpSocket g_socket;

	char m_hostName[NAME_SIZE];
	short m_hostID;

public:
	CNetwork();
	~CNetwork();

	void err_disp(const char* msg, int err_no)
	{
		WCHAR* h_mess;
		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL, err_no,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPWSTR)&h_mess, 0, NULL);
		cout << msg;
		wcout << L"  ¿¡·¯ => " << h_mess << endl;
		while (true);
		LocalFree(h_mess);
	}

public:
	void Init();
	void Update();
	void CleanUp();

	void ProcessData(char* net_buf, size_t io_byte);
	void ProcessPacket(char* packet);
	void SendPacket(void* packet);

	void SendLoginPacket(char* name);
	void SendLogoutPacket();
	void SendMovePacket(int direction);
	void SendAttackPacket();
	void SendChatPacket(char* mess);
	void SendTeleportPacket();

	void SetHostID(short id) { m_hostID = id; }
};

