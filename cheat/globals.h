#pragma once
#include <cstdint>
#include <cstddef>
#include "utils/memory.h"
#include <vector>
#include <utility>

namespace globals {
	inline bool csgoRunning = false;
	
	inline std::uintptr_t clientAdress = 0;
	inline std::uintptr_t engineAdress = 0;

	inline bool skinchanger = false;

	inline bool bhop = false;

	inline bool triggerbot = false;
	inline int triggerbot_key = 0;

	inline bool fovChanger = false;
	inline float fovChangerValue = 106.f;

    inline bool aimbot = false;
    inline float aimbotFov = 180.f;
	inline bool aimbotThroughWalls = false;
    inline bool aimForHead = true;

	inline bool rcs = false;
	
	inline bool glow = false;
	inline float glowColor[] = { 1.0f, 0.0f, 0.0f, 1.0f};

	inline bool radar = false;
}

#ifndef SKINS_IDS_H
#define SKINS_IDS_H

namespace skins_ids {
    
    class weapon {
    public:
        int weapon_id;
        int skin_id;

        weapon(int weapon_id, int skin_id) : weapon_id(weapon_id), skin_id(skin_id) {}
    };
    
    inline std::vector<std::pair<std::string, weapon>> weapons{
        {"pistol_glock", weapon(4, 0)},
        {"pistol_p2000", weapon(32, 0)},
        {"pistol_usp_s", weapon(61, 0)},
        {"pistol_dual_berettas", weapon(2, 0)},
        {"pistol_p250", weapon(36, 0)},
        {"pistol_tec9", weapon(30, 0)},
        {"pistol_five_seven", weapon(3, 0)},
        {"pistol_cz75a", weapon(63, 0)},
        {"pistol_deagle", weapon(1, 0)},
        {"pistol_revolver", weapon(64, 0)},
        {"shotgun_nova", weapon(35, 0)},
        {"shotgun_xm1014", weapon(25, 0)},
        {"shotgun_sawed_off", weapon(29, 0)},
        {"shotgun_mag7", weapon(27, 0)},
        {"smg_mac10", weapon(17, 0)},
        {"smg_mp9", weapon(34, 0)},
        {"smg_mp7", weapon(33, 0)},
        {"smg_bizon", weapon(26, 0)},
        {"smg_ump45", weapon(24, 0)},
        {"rifle_galil_ar", weapon(13, 0)},
        {"rifle_ak47", weapon(7, 0)},
        {"rifle_sg553", weapon(39, 0)},
        {"rifle_m4a4", weapon(16, 0)},
        {"rifle_m4a1_s", weapon(60, 0)},
        {"rifle_aug", weapon(8, 0)},
        {"rifle_famas", weapon(10, 0)},
        {"sniper_ssg08", weapon(40, 0)},
        {"sniper_awp", weapon(9, 0)},
        {"sniper_g3sg1", weapon(11, 0)},
        {"sniper_scar20", weapon(38, 0)},
        {"heavy_m249", weapon(14, 0)},
        {"heavy_negev", weapon(28, 0)}
    };


    inline void set_skin_id(int weapon_id, int skin_id) {
        for (auto& weapon : skins_ids::weapons) {
            if (weapon.second.weapon_id == weapon_id) {
                weapon.second.skin_id = skin_id;
                break;
            }
        }
    }
}

#endif // SKINS_IDS_H