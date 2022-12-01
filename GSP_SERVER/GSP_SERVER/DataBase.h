#pragma once
#include "stdafx.h"

enum DB_TYPE { DB_LOGIN, DB_UPDATE, DB_DISCONNECT };

struct DB_EVENT {
	int obj_id;
	char name[NAME_SIZE];
	DB_TYPE event_type;
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
	void DB_SaveData();
	void DB_UpdateUserData(DB_EVENT event);

	void Enqueue(int _obj_id, char* _name, DB_TYPE _type);
	void Enqueue(int _obj_id, DB_TYPE _type);

	void show_error(SQLHANDLE hHandle, SQLSMALLINT hType, RETCODE RetCode);
};