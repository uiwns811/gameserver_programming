#include "NPC.h"

int API_InitializePos(lua_State* L)
{
	lua_pop(L, 1);
	int x, y;

	while (true) {
		x = rand() % W_WIDTH;
		y = rand() % W_HEIGHT;
		if (SharedData::g_map[x][y]) break;
	}

	lua_pushnumber(L, x);
	lua_pushnumber(L, y);
	return 2;
}