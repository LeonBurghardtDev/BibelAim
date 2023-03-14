#include "../../utils/memory.h"
#include "../../offsets/csgo.hpp"
#include "../../globals.h"

int glow_main(const Memory& memory) {
	const auto localPlayer = memory.Read<uintptr_t>(globals::clientAdress + hazedumper::signatures::dwLocalPlayer);

	if (!localPlayer)
		return 0;
	
	const auto glowManager = memory.Read<uintptr_t>(globals::clientAdress + hazedumper::signatures::dwGlowObjectManager);

	if (!glowManager)
		return 0;
	
	const auto localTeam = memory.Read<int32_t>(localPlayer + hazedumper::netvars::m_iTeamNum);

	for (auto i = 1; i <= 32; i++) {
		const auto player = memory.Read<std::uintptr_t>(globals::clientAdress + hazedumper::signatures::dwEntityList + i * 0x10);
		
		if (!player)
			continue;
		
		const auto team = memory.Read<std::int32_t>(player + hazedumper::netvars::m_iTeamNum);
		
		if (team == localTeam)
			continue;
		
		const auto lifeState = memory.Read<std::int32_t>(player + hazedumper::netvars::m_lifeState);
		
		if (lifeState != 0)
			continue;
			
		if(globals::glow) {
			const auto glowIndex = memory.Read<std::int32_t>(player + hazedumper::netvars::m_iGlowIndex);

			memory.Write<float>(glowManager + (glowIndex * 0x38) + 0x8, globals::glowColor[0]);
			memory.Write<float>(glowManager + (glowIndex * 0x38) + 0xC, globals::glowColor[1]);
			memory.Write<float>(glowManager + (glowIndex * 0x38) + 0x10, globals::glowColor[2]);
			memory.Write<float>(glowManager + (glowIndex * 0x38) + 0x14, globals::glowColor[3]);

			memory.Write<bool>(glowManager + (glowIndex * 0x38) + 0x28, true);
			memory.Write<bool>(glowManager + (glowIndex * 0x38) + 0x29, false);
		}
	}
		
	
	
}