#pragma once
#include "stdafx.h"

enum COMP_TYPE {OP_ACCEPT, OP_RECV, OP_SEND, OP_DB_LOGIN_WITH_INFO, OP_DB_LOGIN_NO_INFO };

class EXP_OVER
{
public:
	WSAOVERLAPPED over;
	WSABUF wsabuf;
	char sendbuf[BUF_SIZE];
	COMP_TYPE op_type;

	EXP_OVER()
	{
		wsabuf.len = BUF_SIZE;
		wsabuf.buf = sendbuf;
		op_type = OP_RECV;
		ZeroMemory(&over, sizeof(over));
	}

	EXP_OVER(char* packet)
	{
		wsabuf.len = packet[0];
		wsabuf.buf = sendbuf;
		ZeroMemory(&over, sizeof(over));
		op_type = OP_SEND;
		memcpy(sendbuf, packet, packet[0]);
	}
};