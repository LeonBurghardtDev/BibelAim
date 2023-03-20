#pragma once
#include "../utils/memory.h"

namespace threads {

	void VisualThread(const Memory&, HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) noexcept;
	void SkinChangerThread(const Memory&) noexcept;
	void MovementThread(const Memory&) noexcept;
	void AimbotThread(const Memory&) noexcept;
	void MiscThread(const Memory&) noexcept;
	
}