#pragma once
#include "../../utils/memory.h"
#include "../../globals.h"
#include "../../offsets/csgo.hpp"

struct Vector2 {
	float x = { }, y = { };
};

Vector2 rcs_main(const Memory& memory, const Vector2 _oldPunch){
	
	Vector2 oldPunch = _oldPunch;
	
	
	const auto& localPlayer = memory.Read<std::uintptr_t>(globals::clientAdress + offsets::dwLocalPlayer);
	const auto& shotsFired = memory.Read<int32_t>(localPlayer + offsets::m_iShotsFired);
	
	if (shotsFired){
		const auto& clientState = memory.Read<std::uintptr_t>(globals::engineAdress + offsets::dwClientState);
		const auto& viewAngles = memory.Read<Vector2>(clientState + offsets::dwClientState_ViewAngles);
		
		const auto& aimPunch = memory.Read<Vector2>(localPlayer + offsets::m_aimPunchAngle);
		
		auto newAngles = Vector2{
			viewAngles.x + oldPunch.x - aimPunch.x * 2.f,
			viewAngles.y + oldPunch.y - aimPunch.y * 2.f
		};
		
		if (newAngles.x > 89.f)
			newAngles.x = 89.f;
		if (newAngles.x < -89.f)
			newAngles.x = -89.f;

		while (newAngles.y > 180.f)
			newAngles.y -= 360.f;
		while (newAngles.y < -180.f)
			newAngles.y += 360.f;
		

		memory.Write<Vector2>(clientState + offsets::dwClientState_ViewAngles, newAngles);
		
		oldPunch.x = aimPunch.x;
		oldPunch.y = aimPunch.y;
	}
	else{
		oldPunch.x = oldPunch.y = 0.f;
	}
	return oldPunch;
	
}