#pragma once
#include "Abstract/ECS/Component/Component.hpp"

enum class BattleAction { LIGHT_ATTACK, HEAVY_ATTACK, ULTIMATE_ATTACK, HEAL, REST };
struct BattleComponent : Component<BattleComponent> {
  public:
	BattleComponent() = default;
	float health{100};
	float AP{2};
	int numberOfUltimateAttacksUsed{0};
	BattleAction selectedAction;
	Entity *target = nullptr;
	bool isPlayerTeam;
	int turnOrder;
};
