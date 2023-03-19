#include "../../utils/memory.h"
#include "../../globals.h"
#include "../../offsets/csgo.hpp"


void fov_changer_main(const Memory& memory) {

	float targetFov = globals::fovChangerValue;
	
	const auto localPlayer = memory.Read<uintptr_t>(globals::clientAdress + offsets::dwLocalPlayer);

	const auto isLocalPlayerAlive = memory.Read<int>(localPlayer + offsets::m_fFlags);
	
	if (!isLocalPlayerAlive)
		return;
	
	const auto currentFov = memory.Read<float>(localPlayer + offsets::m_iFOV);

	if (currentFov == targetFov)
		memory.Write(localPlayer + offsets::m_iDefaultFOV, 106);
	else
		memory.Write(localPlayer + offsets::m_iDefaultFOV, targetFov);
	
	
	
}