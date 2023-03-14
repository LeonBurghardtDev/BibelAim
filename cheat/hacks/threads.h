#pragma once
#include "../utils/memory.h"

namespace threads {

	void VisualThread(const Memory&) noexcept;
	void SkinChangerThread(const Memory&) noexcept;
	void MovementThread(const Memory&) noexcept;
	void AimbotThread(const Memory&) noexcept;
	
}