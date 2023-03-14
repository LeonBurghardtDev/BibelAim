#pragma once
#include <cstdint>
#include <cstddef>
#include "utils/memory.h"

namespace globals {
	inline std::uintptr_t clientAdress = 0;
	inline std::uintptr_t engineAdress = 0;
	
	inline bool glow = false;
	inline float glowColor[] = { 1.0f, 0.0f, 0.0f, 1.0f};

	inline bool radar = false;
}