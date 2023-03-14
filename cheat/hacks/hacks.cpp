#include "hacks.h"
#include "../gui.h"
#include "../globals.h"
#include "skinchanger/skinchanger.h"
#include "glow/glow.h"
#include "radar/radar.h"
#include <thread>



void hacks::VisualThread(const Memory& mem) noexcept {
	while (gui::isRunning)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));

		skinchanger_main(mem);
		glow_main(mem);
		radar_main(mem);
			
		
	}
}