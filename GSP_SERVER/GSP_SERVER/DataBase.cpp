#include "Database.h"
#include "Player.h"

void CDataBase::DB_Initialize()
{
	setlocale(LC_ALL, "korean");
	_wsetlocale(LC_ALL, L"korean");

	retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);

	if (!(retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)) {
		return;
	}

	retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER*)SQL_OV_ODBC3, 0);

	if (!(retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)) {
		return;
	}

	retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);

	if (!(retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)) {
		show_error(hdbc, SQL_HANDLE_DBC, retcode); return;
	}

	SQLSetConnectAttr(hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)5, 0);

	retcode = SQLConnect(hdbc, (SQLWCHAR*)L"gsp_tp_odbc_2019182026", SQL_NTS, (SQLWCHAR*)NULL, 0, NULL, 0);

	if (!(retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)) {
		show_error(hdbc, SQL_HANDLE_DBC, retcode); return;
	}

	retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);

	cout << "DB connect complete" << endl;
}

void CDataBase::DB_Thread()
{
	while (true) {
		DB_EVENT event;
		if (true == db_queue.try_pop(event)) {

			switch (event.event_type) {
			case DB_LOGIN:
				DB_CheckLogin(event);
				break;
			case DB_UPDATE:
				DB_SaveData();
				break;
			case DB_DISCONNECT:
				DB_UpdateUserData(event);
				break;
			}
		}
	}
}

void CDataBase::DB_CleanUp()
{
	DB_SaveData();

	SQLCancel(hstmt);
	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
	SQLDisconnect(hdbc);
	SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
	SQLFreeHandle(SQL_HANDLE_ENV, henv);
}

void CDataBase::DB_CheckLogin(DB_EVENT event)
{
	SQLWCHAR user_name[NAME_SIZE];
	SQLLEN cbName = 0;

	wstring player_name(event.name, &event.name[NAME_SIZE]);

	wstring query = L"EXEC check_login " + player_name;

	retcode = SQLExecDirect(hstmt, (SQLWCHAR*)(query.c_str()), SQL_NTS);
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		retcode = SQLBindCol(hstmt, 1, SQL_C_WCHAR, user_name, NAME_SIZE, &cbName);

		retcode = SQLFetch(hstmt);

		if (retcode == SQL_ERROR || retcode == SQL_SUCCESS_WITH_INFO)
			show_error(hdbc, SQL_HANDLE_DBC, retcode);

		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
			SQLCancel(hstmt);

			DB_LoginOk(event);
		}
		else if (retcode == SQL_NO_DATA_FOUND) {
			SQLCancel(hstmt);

			DB_InsertUserData(event);
		}
	}
	else {
		show_error(hdbc, SQL_HANDLE_DBC, retcode);
	}
}

void CDataBase::DB_LoginOk(DB_EVENT event)
{
	CObject user_info;
	SQLLEN cb_x = 0, cb_y = 0, cb_exp = 0, cb_level = 0, cb_hp = 0;

	wstring player_name(event.name, &event.name[NAME_SIZE]);

	wstring query = L"EXEC get_user_info " + player_name;

	retcode = SQLExecDirect(hstmt, (SQLWCHAR*)(query.c_str()), SQL_NTS);
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		retcode = SQLBindCol(hstmt, 1, SQL_C_LONG, &user_info.m_x, 100, &cb_x);
		retcode = SQLBindCol(hstmt, 2, SQL_C_LONG, &user_info.m_y, 100, &cb_y);
		retcode = SQLBindCol(hstmt, 3, SQL_C_LONG, &user_info.m_exp, 100, &cb_exp);
		retcode = SQLBindCol(hstmt, 4, SQL_C_LONG, &user_info.m_level, 100, &cb_level);
		retcode = SQLBindCol(hstmt, 5, SQL_C_LONG, &user_info.m_hp, 100, &cb_hp);

		retcode = SQLFetch(hstmt);

		if (retcode == SQL_ERROR || retcode == SQL_SUCCESS_WITH_INFO)
			show_error(hdbc, SQL_HANDLE_DBC, retcode);

		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
			EXP_OVER* over = new EXP_OVER;
			over->op_type = OP_DB_LOGIN_WITH_INFO;
			memcpy(over->sendbuf, &user_info, sizeof(user_info));

			PostQueuedCompletionStatus(SharedData::g_iocp, 1, event.obj_id, &over->over);
		}
	}
	else {
		show_error(hdbc, SQL_HANDLE_DBC, retcode);
	}

	SQLCancel(hstmt);
}

void CDataBase::DB_InsertUserData(DB_EVENT event)
{
	string name(event.name);
	short x = SharedData::g_clients[event.obj_id]->m_x;
	short y = SharedData::g_clients[event.obj_id]->m_y;
	int exp = SharedData::g_clients[event.obj_id]->m_exp;
	int level = SharedData::g_clients[event.obj_id]->m_level;
	int hp = SharedData::g_clients[event.obj_id]->m_hp;

	string query_s = "EXEC insert_data " + name + ", " + to_string(x) + ", " + to_string(y) + ", " + to_string(exp) + ", " + to_string(level) + ", " + to_string(hp);
	wstring query(query_s.begin(), query_s.end());

	retcode = SQLExecDirect(hstmt, (SQLWCHAR*)(query.c_str()), SQL_NTS);
	if (retcode == SQL_ERROR || retcode == SQL_SUCCESS_WITH_INFO)
		show_error(hdbc, SQL_HANDLE_DBC, retcode);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		EXP_OVER* over = new EXP_OVER;
		over->op_type = OP_DB_LOGIN_NO_INFO;

		PostQueuedCompletionStatus(SharedData::g_iocp, 1, event.obj_id, &over->over);
	}

	SQLCancel(hstmt);
}

void CDataBase::DB_SaveData()		// player info 전체 업데이트
{
	for (auto& cl : SharedData::g_clients) {
		if (cl->m_id > MAX_USER) continue;

		cl->m_s_lock.lock();
		if (cl->m_state != ST_INGAME) {
			cl->m_s_lock.unlock();
			continue;
		}
		cl->m_s_lock.unlock();

		wstring player_name(cl->m_name, &cl->m_name[NAME_SIZE]);
		string name(cl->m_name);
		string query_s = "EXEC update_data " + name + ", " + to_string(cl->m_x) + ", " + to_string(cl->m_y) + ", " + to_string(cl->m_exp) + ", " + to_string(cl->m_level) + ", " + to_string(cl->m_hp);

		wstring query(query_s.begin(), query_s.end());

		retcode = SQLExecDirect(hstmt, (SQLWCHAR*)(query.c_str()), SQL_NTS);
		if (retcode == SQL_ERROR || retcode == SQL_SUCCESS_WITH_INFO)
			show_error(hdbc, SQL_HANDLE_DBC, retcode);

		SQLCancel(hstmt);
	}
}

void CDataBase::DB_UpdateUserData(DB_EVENT event)		// disconnect했을 때 특정 객체 업데이트
{
	string name(event.name);
	short x = SharedData::g_clients[event.obj_id]->m_x;
	short y = SharedData::g_clients[event.obj_id]->m_y;
	int exp = SharedData::g_clients[event.obj_id]->m_exp;
	int level = SharedData::g_clients[event.obj_id]->m_level;
	int hp = SharedData::g_clients[event.obj_id]->m_hp;

	string query_s = "EXEC update_data " + name + ", " + to_string(x) + ", " + to_string(y) + ", " + to_string(exp) + ", " + to_string(level) + ", " + to_string(hp);
	wstring query(query_s.begin(), query_s.end());

	retcode = SQLExecDirect(hstmt, (SQLWCHAR*)(query.c_str()), SQL_NTS);
	if (retcode == SQL_ERROR || retcode == SQL_SUCCESS_WITH_INFO)
		show_error(hdbc, SQL_HANDLE_DBC, retcode);

	SQLCancel(hstmt);
}

void CDataBase::show_error(SQLHANDLE hHandle, SQLSMALLINT hType, RETCODE RetCode)
{
	SQLSMALLINT iRec = 0;
	SQLINTEGER iError;
	WCHAR wszMessage[1000];
	WCHAR wszState[SQL_SQLSTATE_SIZE + 1];
	if (RetCode == SQL_INVALID_HANDLE) {
		fwprintf(stderr, L"Invalid handle!\n");
		return;
	}
	while (SQLGetDiagRec(hType, hHandle, ++iRec, wszState, &iError, wszMessage,
		(SQLSMALLINT)(sizeof(wszMessage) / sizeof(WCHAR)), (SQLSMALLINT*)NULL) == SQL_SUCCESS) {
		// Hide data truncated..
		if (wcsncmp(wszState, L"01004", 5)) {
			fwprintf(stderr, L"[%5.5s] %s (%d)\n", wszState, wszMessage, iError);
		}
	}

}

void CDataBase::Enqueue(int _obj_id, char* _name, DB_TYPE _type)
{
	DB_EVENT event;
	event.obj_id = _obj_id;
	strncpy_s(event.name, _name, NAME_SIZE);
	event.event_type = _type;
	db_queue.push(event);
}

void CDataBase::Enqueue(int _obj_id, DB_TYPE _type)
{
	DB_EVENT event;
	event.obj_id = _obj_id;
	event.event_type = _type;
	db_queue.push(event);
}