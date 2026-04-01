#pragma once
#include "Abstract/ECS/Component/Component.hpp"
#include "Abstract/ECS/Entity/EntityID.hpp"

enum class BattleAction { LIGHT_ATTACK, HEAVY_ATTACK, ULTIMATE_ATTACK, HEAL, REST };
enum class BattleState {
	TURN_START,
	SELECTED_ACTION,
	EXECUTING_ACTION,
	CHECK_DEATH,
	WAITING_FOR_INPUT,
	NEXT_ROUND,
	VICTORY,
	DEFEAT
};
struct BattleComponent : Component<BattleComponent> {
  public:
	BattleComponent() = default;
	float AP{2};
	int numberOfUltimateAttacksUsed{0};
	BattleAction selectedAction;
	EntityID target;
	bool isPlayerTeam;
	bool isActiveTurn = false;
	BattleState battleState = BattleState::TURN_START;
};
