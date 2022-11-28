#pragma once
#include "stdafx.h"

enum EV_DB_TYPE { EV_DB_LOGIN, EV_DB_UPDATE, EV_DB_DISCONNECT };

struct DB_EVENT {
	int obj_id;
	char name[NAME_SIZE];
	EV_DB_TYPE event_type;
};

class CDataBase {
private:
	SQLHENV henv;
	SQLHDBC hdbc;               // odbc 핸들
	SQLHSTMT hstmt = 0;         // sql 명령어 statement의 핸들
	SQLRETURN retcode;


public:
	concurrency::concurrent_queue<DB_EVENT> db_queue;

	CDataBase() {};
	~CDataBase() {};

	void DB_Initialize();
	void DB_Thread();
	void DB_CleanUp();

	void DB_CheckLogin(DB_EVENT event);
	void DB_LoginOk(DB_EVENT event);
	void DB_InsertUserData(DB_EVENT event);
	void DB_UpdateClientsData();
	void DB_UpdateUserData(DB_EVENT event);

	void show_error(SQLHANDLE hHandle, SQLSMALLINT hType, RETCODE RetCode);
};