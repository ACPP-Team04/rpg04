#pragma once
#include "Abstract/ECS/Component/Component.hpp"
#include "Abstract/ECS/Entity/EntityID.hpp"
#include <iostream>
#include <vector>
struct BattleManagerComponent : public Component<BattleManagerComponent> {
	std::vector<EntityID> participants;
	int currentTurnIndex = 0;
	bool isBattleOver = false;

	void readFromJson(const nlohmann::json &j) override {}
};
