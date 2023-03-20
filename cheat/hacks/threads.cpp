#include "threads.h"
#include "../gui.h"
#include "../globals.h"
#include "../offsets/csgo.hpp"
#include "skinchanger/skinchanger.h"
#include "visuals/glow/glow.h"
#include "visuals/radar/radar.h"
#include "movement/bhop.h"
#include "misc/fov_changer.h"
#include "aimbot/triggerbot.h"
#include "aimbot/rcs.h"
#include "visuals/esp/esp.h"
#include "aimbot/aimbot.h"
#include <thread>



void threads::VisualThread(const Memory& mem, HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) noexcept {
	while (gui::isRunning)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		
		if (globals::glow)
			glow_main(mem);
		if (globals::radar)
			radar_main(mem);
	//	if(globals::esp) // not working atm
	//		esp_main(mem, hInstance, hPrevInstance, lpCmdLine, nCmdShow);
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
		if (globals::aimbot) {
			aimbot_main(mem);
		}
			
	}
}

void threads::MiscThread(const Memory& mem) noexcept {
	while (gui::isRunning) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		if (globals::fovChanger)
			fov_changer_main(mem);
		else
			mem.Write(mem.Read<uintptr_t>(globals::clientAdress + offsets::dwLocalPlayer) + offsets::m_iDefaultFOV, 106);

	}
}