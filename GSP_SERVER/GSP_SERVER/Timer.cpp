#include "Timer.h"
#include "expover.h"

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
			}
			continue;
		}
		else this_thread::sleep_for(1ms);
	}
}