#pragma once
#include "stdafx.h"

bool is_pc(int object_id);

bool is_npc(int object_id);

bool can_see(int from, int to);

bool isInAgroArea(int from, int to);

int get_new_client_id();

void InitializeMap();

bool CanAttack(int from, int to);