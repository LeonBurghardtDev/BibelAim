#include "../../utils/memory.h"
#include "../../offsets/csgo.hpp"
#include "../../globals.h"
#include <thread>
#include <iostream>
#include <array>
#include <string>
#include <stdexcept>


	constexpr const int GetWeaponPaint(const short& itemDefinition) {
		
		for (const auto& weapon : skins_ids::weapons) {
			int weapon_id_to_find = itemDefinition; 
			auto it = std::find_if(skins_ids::weapons.begin(), skins_ids::weapons.end(),
				[weapon_id_to_find](const auto& weapon) {
					return weapon.second.weapon_id == weapon_id_to_find;
				});
			if (it != skins_ids::weapons.end()) {
				return it->second.skin_id;
			}
			else {
				return 0;
			}

		}


		/*
		// https://pastebin.com/3zNVRK4W
		// https://totalcsgo.com/skin-ids
		switch (itemDefinition) {
		case 1: return 711; // deagle
		case 4: return 38; // glock
		case 7: return 302; // ak47
		case 9: return 344; // awp
		case 61: return 653; // usp
		default: return 0;
		}*/
	}
	
	int skinchanger_main(const Memory& memory) {
		
		uintptr_t client = globals::clientAdress;
		uintptr_t engine = globals::engineAdress;
		
		try {

			const auto& localPlayer = memory.Read<std::uintptr_t>(client + offsets::dwLocalPlayer);
			const auto& weapons = memory.Read<std::array<unsigned long, 8>>(localPlayer + offsets::m_hMyWeapons);

			for (const auto& handle : weapons) {
				
				const auto& weapon = memory.Read<std::uintptr_t>((client + offsets::dwEntityList + (handle & 0xFFF) * 0x10) - 0x10);

				if (!weapon) {
					continue;
				}


				const short itemDefinition = memory.Read<short>(weapon + offsets::m_iItemDefinitionIndex);

				/* Not working static knife changer

				if (itemDefinition == 42 || itemDefinition == 59) {

					
					memory.Write<short>(weapon + offsets::m_iItemDefinitionIndex, 507);
					memory.Write<short>(weapon + 0x258, 507);
					memory.Write<short>(weapon + 0x3224, 507);
					memory.Write<short>(weapon + offsets::m_iItemIDHigh, -1);
					memory.Write<short>(weapon + offsets::m_OriginalOwnerXuidLow, 0);
					memory.Write<short>(weapon + offsets::m_iAccountID, memory.Read<short>(weapon + offsets::m_OriginalOwnerXuidHigh));
					memory.Write<short>(weapon + offsets::m_nFallbackSeed, 125);
					memory.Write<short>(weapon + offsets::m_flFallbackWear, 0.0f);
					memory.Write<short>(weapon + offsets::m_nFallbackStatTrak, 1337);
					memory.Write<short>(weapon + offsets::m_nFallbackPaintKit, 180);
					memory.Write<int>(weapon + offsets::m_iEntityQuality, 3);
					
				}

				*/



				
				if (const auto paint = GetWeaponPaint(itemDefinition)) {

					const bool shouldUpdate = memory.Read < ::int32_t>(weapon + offsets::m_nFallbackPaintKit) != paint;

					memory.Write<std::int32_t>(weapon + offsets::m_iItemIDHigh, -1);

					memory.Write<std::int32_t>(weapon + offsets::m_nFallbackPaintKit, paint);
					memory.Write<float>(weapon + offsets::m_flFallbackWear, 0.1f);

					if (shouldUpdate) {
						memory.Write<std::int32_t>(memory.Read<std::uintptr_t>(engine + offsets::dwClientState) + 0x174, -1);
					}
				}
			}

		}
		catch (const std::exception& e) {
			std::cout << e.what() << std::endl;
		}
		
	return 0;
}
