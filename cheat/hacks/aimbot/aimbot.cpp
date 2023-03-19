#include "../../utils/memory.h"
#include "../../globals.h"
#include <numbers>
#include <sstream>
#include <cmath>
#include "../../offsets/csgo.hpp"

struct Vector3
{
	// constructor
	constexpr Vector3(
		const float x = 0.f,
		const float y = 0.f,
		const float z = 0.f) noexcept :
		x(x), y(y), z(z) { }

	// operator overloads
	constexpr const Vector3& operator-(const Vector3& other) const noexcept
	{
		return Vector3{ x - other.x, y - other.y, z - other.z };
	}

	constexpr const Vector3& operator+(const Vector3& other) const noexcept
	{
		return Vector3{ x + other.x, y + other.y, z + other.z };
	}

	constexpr const Vector3& operator/(const float factor) const noexcept
	{
		return Vector3{ x / factor, y / factor, z / factor };
	}

	constexpr const Vector3& operator*(const float factor) const noexcept
	{
		return Vector3{ x * factor, y * factor, z * factor };
	}

	// utils
	constexpr const Vector3& ToAngle() const noexcept
	{
		return Vector3{
			std::atan2(-z, std::hypot(x, y)) * (180.0f / std::numbers::pi_v<float>),
			std::atan2(y, x) * (180.0f / std::numbers::pi_v<float>),
			0.0f };
	}

	constexpr const bool IsZero() const noexcept
	{
		return x == 0.f && y == 0.f && z == 0.f;
	}

	// struct data
	float x, y, z;
};

constexpr Vector3 CalculateAngle(
	const Vector3& localPosition,
	const Vector3& enemyPosition,
	const Vector3& viewAngles) noexcept
{
	return ((enemyPosition - localPosition).ToAngle() - viewAngles);
}

void aimbot_main(const Memory& memory) {
	// aimbot key
	if (!GetAsyncKeyState(VK_SHIFT))
		return;
	// get local player
	const auto localPlayer = memory.Read<std::uintptr_t>(globals::clientAdress + offsets::dwLocalPlayer);
	const auto localTeam = memory.Read<std::int32_t>(localPlayer + offsets::m_iTeamNum);

	// eye position = origin + viewoffsets
	const auto localEyePosition = memory.Read<Vector3>(localPlayer + offsets::m_vecOrigin) + memory.Read<Vector3>(localPlayer + offsets::m_vecViewOffset);

	const auto clientState = memory.Read<std::uintptr_t>(globals::engineAdress + offsets::dwClientState);

	const auto localPlayerId = memory.Read<std::int32_t>(clientState + offsets::dwClientState_GetLocalPlayer);

	const auto viewAngles = memory.Read<Vector3>(clientState + offsets::dwClientState_ViewAngles);
	const auto aimPunch = memory.Read<Vector3>(localPlayer + offsets::m_aimPunchAngle) * 2;

	// aimbot fov
	auto bestFov = 180.f;
	try {
		bestFov = globals::aimbotFov;
	}catch (const std::exception& e){}
	
	auto bestAngle = Vector3{ };

	for (auto i = 1; i <= 32; ++i)
	{
		const auto player = memory.Read<std::uintptr_t>(globals::clientAdress + offsets::dwEntityList + i * 0x10);

		if (memory.Read<std::int32_t>(player + offsets::m_iTeamNum) == localTeam)
			continue;

		if (memory.Read<bool>(player + offsets::m_bDormant))
			continue;

		if (memory.Read<std::int32_t>(player + offsets::m_lifeState))
			continue;

		
		if (!globals::aimbotThroughWalls && !(memory.Read<std::int32_t>(player + offsets::m_bSpottedByMask) & (1 << localPlayerId)))
			continue;

		const auto boneMatrix = memory.Read<std::uintptr_t>(player + offsets::m_dwBoneMatrix);

		int multiplier; // 8 = head; 6 = chest
		
		if (globals::aimForHead)
			multiplier = 8;
		else
			multiplier = 6;
		
		const auto playerPosition = Vector3{
			memory.Read<float>(boneMatrix + 0x30 * multiplier + 0x0C),
			memory.Read<float>(boneMatrix + 0x30 * multiplier + 0x1C),
			memory.Read<float>(boneMatrix + 0x30 * multiplier + 0x2C)
		};

		const auto angle = CalculateAngle(
			localEyePosition,
			playerPosition,
			viewAngles + aimPunch
		);

		const auto fov = std::hypot(angle.x, angle.y);

		if (fov < bestFov)
		{
			bestFov = fov;
			bestAngle = angle;
		}
	}
	// if we have a best angle, do aimbot
	if (!bestAngle.IsZero()) {
		memory.Write<Vector3>(clientState + offsets::dwClientState_ViewAngles, viewAngles + bestAngle / 3.f); // 
	}
			
	
}
	
