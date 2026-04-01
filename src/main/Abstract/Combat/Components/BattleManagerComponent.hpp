#pragma once
#include "Abstract/ECS/Entity/EntityID.hpp"
#include <iostream>
#include <vector>
struct BattleManagerComponent {
	std::vector<EntityID> participants;
	int currentTurnIndex = 0;
	bool isBattleOver = false;
};
