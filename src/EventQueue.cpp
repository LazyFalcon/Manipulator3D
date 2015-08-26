#include "EventQueue.h"
#include <list>

namespace EventQueue {

struct Event
{
	i32 time;
	std::function<void(void)> fun;
};

std::list<Event> eventQueue;

void spawn(i32 time, std::function<void(void)> &fun){
	eventQueue.emplace_back({time, fun});
}
void update(u32 dt){
	for(auto &e : eventQueue){
		e.time -= dt;
		if(e.time < 0)
			e.fun();
	}
	std::remove_if(eventQueue.begin(), eventQueue.end(), [](Event &e){ return e.time<0; });

}

}

