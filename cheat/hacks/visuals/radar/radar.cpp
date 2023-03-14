#include "../../../utils/memory.h"
#include "../../../offsets/csgo.hpp"
#include "../../../globals.h"


int radar_main(const Memory& memory) {
	const auto localPlayer = memory.Read<uintptr_t>(globals::clientAdress + offsets::dwLocalPlayer);

	if (!localPlayer)
		return 0;

	for (auto i = 1; i <= 32; i++) {
		const auto player = memory.Read<std::uintptr_t>(globals::clientAdress + offsets::dwEntityList + i * 0x10);

		if (!player)
			continue;
		
		memory.Write(player + offsets::m_bSpotted, true);
		

	}
	
}