
#include "../../offsets/csgo.hpp"
#include "../../globals.h"

void bhop_main(const Memory& memory) {
	
	const auto localPlayer = memory.Read<uintptr_t>(globals::clientAdress + offsets::dwLocalPlayer);
	
	if (localPlayer) {
		const auto onGround = memory.Read<bool>(localPlayer + offsets::m_fFlags);
		
		if (GetAsyncKeyState(VK_SPACE) || GetAsyncKeyState(VK_MBUTTON) == SHORT(0x8000) ||  GetAsyncKeyState(VK_MBUTTON) == SHORT(0x80) && onGround & (1 << 0)) {
			memory.Write<uintptr_t>(globals::clientAdress + offsets::dwForceJump, 6);
		}
	}
}