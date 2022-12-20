#include "Party.h"
#include "function.h"
#include "Player.h"

// ��Ƽ�� �ִ� 4��

void CParty::InviteParty(int inviter)
{
	CPlayer* player = reinterpret_cast<CPlayer*>(SharedData::g_clients[inviter]);
	player->m_vl_lock.lock();
	unordered_set<int> vlist = player->m_viewlist;
	player->m_vl_lock.unlock();

	// �ʴ��ϴ¾� ���� �þ߿� �ִ� �÷��̾����׸� ��Ƽ ��û ����
	for (auto& vp : vlist) {
		if (is_npc(vp)) continue;
		CPlayer* p = reinterpret_cast<CPlayer*>(SharedData::g_clients[vp]);
		if (p->m_state != ST_INGAME) continue;
		if (player->isParty(vp)) continue;
		p->SendInvitePartyPacket(inviter);
	}

	m_party.insert(inviter);
}

void CParty::JoinParty(int id)
{
	if (m_party.size() >= 4) return;
	
	m_party.insert(id);

	CPlayer* player = reinterpret_cast<CPlayer*>(SharedData::g_clients[id]);
	for (auto& pmi : m_party) {
		// ��Ƽ�� �ִ� ��� �ֵ鿡�� �� ���Դٰ� ������
		if (pmi == id) continue;
		CPlayer* pm = reinterpret_cast<CPlayer*>(SharedData::g_clients[pmi]);
		player->SendJoinPartyPacket(pmi);		// ��� ���¾ֵ����� ��� ��Ƽ�� ���� �����ֱ�
		pm->SendJoinPartyPacket(id);			// ���� �ִ� �ֵ����� ��� ���� �� ���� �˷��ֱ�

		char mess[CHAT_SIZE];
		sprintf_s(mess, "%s joined the party!", player->m_name);
		pm->SendChatPacket(SYSTEM_CHAT_ID, mess);

		player->SendStatChangePacket(pm->m_id, pm->m_exp, pm->m_level, pm->m_hp, pm->m_maxhp);
		pm->SendStatChangePacket(player->m_id, player->m_exp, player->m_level, player->m_hp, player->m_maxhp);
	}
}

void CParty::ExitParty(int id)
{
	if (m_party.count(id) != 0) {
		m_party.erase(id);
	}
}