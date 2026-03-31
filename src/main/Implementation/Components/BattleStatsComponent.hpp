#pragma once
#include "Abstract/ECS/Component/Component.hpp"

struct BattleStatsComponent : Component<BattleStatsComponent> {
  public:
	BattleStatsComponent() = default;
	float health{100};
	float AP{2};
	int numberOfUltimateAttacksUsed{0};
};
