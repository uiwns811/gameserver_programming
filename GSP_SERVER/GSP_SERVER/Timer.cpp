#include "Timer.h"
#include "expover.h"
#include "NPC.h"

void do_timer()
{
	while (true) {
		TIMER_EVENT ev;
		auto current_time = chrono::system_clock::now();
		if (true == SharedData::timer_queue.try_pop(ev)) {
			if (ev.wakeup_time > current_time) {
				SharedData::timer_queue.push(ev);

				this_thread::sleep_for(1ms);
				continue;
			}
			switch (ev.event_id) {
			case EV_DB_UPDATE:
			{
				EXP_OVER* over = new EXP_OVER;
				over->op_type = OP_DB_UPDATE;
				PostQueuedCompletionStatus(SharedData::g_iocp, 1, ev.obj_id, &over->over);
			}
			break;
			case EV_PLAYER_HEAL:
			{
				EXP_OVER* over = new EXP_OVER;
				over->op_type = OP_PLAYER_HEAL;
				PostQueuedCompletionStatus(SharedData::g_iocp, 1, ev.obj_id, &over->over);
			}
			break;
			case EV_RESPAWN:
			{
				EXP_OVER* over = new EXP_OVER;
				over->op_type = OP_RESPAWN;
				PostQueuedCompletionStatus(SharedData::g_iocp, 1, ev.obj_id, &over->over);
			}
			break;
			case EV_NPC_AI:
			{
				EXP_OVER* over = new EXP_OVER;
				over->op_type = OP_NPC_AI;
				PostQueuedCompletionStatus(SharedData::g_iocp, 1, ev.obj_id, &over->over);
			}
			break;
			}
		}
		else this_thread::sleep_for(1ms);
	}
}