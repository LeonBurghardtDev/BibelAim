#include "../../utils/memory.h"
#include "../../offsets/csgo.hpp"
#include "../../globals.h"


int radar_main(const Memory& memory) {
	const auto localPlayer = memory.Read<uintptr_t>(globals::clientAdress + hazedumper::signatures::dwLocalPlayer);

	if (!localPlayer)
		return 0;

	for (auto i = 1; i <= 32; i++) {
		const auto player = memory.Read<std::uintptr_t>(globals::clientAdress + hazedumper::signatures::dwEntityList + i * 0x10);

		if (!player)
			continue;
		
		if (globals::radar) {
			memory.Write(player + hazedumper::netvars::m_bSpotted, true);
		}

	}
	
}