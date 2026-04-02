#pragma once
#include "Abstract/ECS/Component/Component.hpp"

struct StatsComponent : Component<StatsComponent> {

	static const float maxHP = 100;

  public:
	StatsComponent() = default;
	float health{100};
	float strength{1};
	float dexterity{1};
	float faith{1};
	float experience{1};
};
