#include "../../utils/memory.h"
#include "../../offsets/csgo.hpp"
#include "../../globals.h"
#include <thread>
#include <iostream>
#include <array>
#include <stdexcept>


	constexpr const int GetWeaponPaint(const short& itemDefinition) {

		// https://pastebin.com/3zNVRK4W
		// https://totalcsgo.com/skin-ids
		switch (itemDefinition) {
		case 1: return 711; // deagle
		case 4: return 38; // glock
		case 7: return 490; // ak47
		case 9: return 344; // awp
		case 61: return 653; // usp
		default: return 0;
		}
	}

	int skinchanger_main(const Memory& memory) {
		
		uintptr_t client = globals::clientAdress;
		uintptr_t engine = globals::engineAdress;
		
			try {

				const auto& localPlayer = memory.Read<std::uintptr_t>(client + hazedumper::signatures::dwLocalPlayer);
				const auto& weapons = memory.Read<std::array<unsigned long, 8>>(localPlayer + hazedumper::netvars::m_hMyWeapons);

				for (const auto& handle : weapons) {
					const auto& weapon = memory.Read<std::uintptr_t>((client + hazedumper::signatures::dwEntityList + (handle & 0xFFF) * 0x10) - 0x10);

					if (!weapon) {
						continue;
					}


					const auto& itemDefinition = memory.Read<short>(weapon + hazedumper::netvars::m_iItemDefinitionIndex);
					if (const auto paint = GetWeaponPaint(itemDefinition)) {

						const bool shouldUpdate = memory.Read < ::int32_t>(weapon + hazedumper::netvars::m_nFallbackPaintKit) != paint;

						memory.Write<std::int32_t>(weapon + hazedumper::netvars::m_iItemIDHigh, -1);

						memory.Write<std::int32_t>(weapon + hazedumper::netvars::m_nFallbackPaintKit, paint);
						memory.Write<float>(weapon + hazedumper::netvars::m_flFallbackWear, 0.1f);

						if (shouldUpdate) {
							memory.Write<std::int32_t>(memory.Read<std::uintptr_t>(engine + hazedumper::signatures::dwClientState) + 0x174, -1);
						}
					}
				}

			}
			catch (const std::exception& e) {
				std::cout << e.what() << std::endl;
			}
		
		return 0;
	}
