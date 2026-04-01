#pragma once
#include "Abstract/ECS/Component/Component.hpp"
#include "StatsComponent.hpp"
#include <map>

enum class WeaponType { MELEE, RANGE };
enum class ScalingFactor { A, B, C };
struct WeaponComponent : Component<WeaponComponent> {
	WeaponComponent() = default;
	WeaponType weaponType;
	ScalingFactor scalingFactor;
};
