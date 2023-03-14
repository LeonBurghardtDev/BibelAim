#include "threads.h"
#include "../gui.h"
#include "../globals.h"
#include "skinchanger/skinchanger.h"
#include "visuals/glow/glow.h"
#include "visuals/radar/radar.h"
#include "movement/bhop.h"
#include "aimbot/triggerbot.h"
#include "aimbot/rcs.h"
#include <thread>



void threads::VisualThread(const Memory& mem) noexcept {
	while (gui::isRunning)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		
		if (globals::glow)
			glow_main(mem);
		if (globals::radar)
			radar_main(mem);
	}
}

void threads::SkinChangerThread(const Memory& mem) noexcept {
	while (gui::isRunning) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		if(globals::skinchanger)
			skinchanger_main(mem);
	}
}


void threads::MovementThread(const Memory& mem) noexcept {
	while (gui::isRunning) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		if(globals::bhop)
			bhop_main(mem);
	}
}

void threads::AimbotThread(const Memory& mem) noexcept {
	auto oldPunch = Vector2{ };
	while (gui::isRunning) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		if (globals::triggerbot)
			triggerbot_main(mem);
		if (globals::rcs) {
			oldPunch = rcs_main(mem, oldPunch);
		}
			
	}
}

