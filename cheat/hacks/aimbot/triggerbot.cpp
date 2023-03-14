#pragma once
#include "../../utils/memory.h"
#include "../../offsets/csgo.hpp"
#include "../../globals.h"
#include <thread>



void triggerbot_main(const Memory& memory) {

	if (!GetAsyncKeyState(VK_SHIFT))
		return;
	
	const auto& LocalPlayer = memory.Read<uintptr_t>(globals::clientAdress + offsets::dwLocalPlayer);
	const auto& localHealth = memory.Read<int32_t>(LocalPlayer + offsets::m_iHealth);

	if (localHealth <= 0)
		return;

	const auto& crosshairId = memory.Read<int32_t>(LocalPlayer + offsets::m_iCrosshairId);

	if (!crosshairId || crosshairId > 64)
		return;
	

	const auto& player = memory.Read<uintptr_t>(globals::clientAdress + offsets::dwEntityList + (crosshairId - 1) * 0x10);

	if (!memory.Read<bool>(player + offsets::m_iHealth))
		return;

	const auto& playerTeam = memory.Read<int32_t>(player + offsets::m_iTeamNum);
	const auto& localTeam = memory.Read<int32_t>(LocalPlayer + offsets::m_iTeamNum);
	
	if (playerTeam == localTeam)
		return;

	memory.Write<uintptr_t>(globals::clientAdress + offsets::dwForceAttack, 6); 
	std::this_thread::sleep_for(std::chrono::milliseconds(20));
	memory.Write<uintptr_t>(globals::clientAdress + offsets::dwForceAttack, 4);
	
}