#pragma once
#include "Abstract/ECS/Component/Component.hpp"

struct StatsComponent : Component<StatsComponent> {

  public:
	StatsComponent() = default;
	float health{100};
	float strength{1};
	float dexterity{1};
	float faith{1};
	float experience{1};
	float maxHealth{100};
	int experienceLevel{1};
	int numberOfFightsWon{0};
	void readFromJson(const nlohmann::json &j) override {}
};
