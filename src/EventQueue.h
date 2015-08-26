#include <Utils/Includes.h>
namespace EventQueue {

void spawn(u32 time, std::function<void(void)> fun);
void update(u32 dt);

}

