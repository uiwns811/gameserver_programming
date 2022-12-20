#pragma once
#include "stdafx.h"

class CParty
{
private:
	unordered_set<int> m_party;

public:
	unordered_set<int>& GetPartyMemeber() { return m_party; }

	void InviteParty(int inviter);
	void JoinParty(int id);
	void ExitParty(int id);
};

