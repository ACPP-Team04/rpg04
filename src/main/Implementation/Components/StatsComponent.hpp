#pragma once
#include "Abstract/ECS/Component/Component.hpp"
#include "Abstract/TILE_ENUMS.hpp"
#include <array>
#include <spdlog/spdlog.h>
struct StatsComponent : Component<StatsComponent> {
  public:
	float experience{1};
	int experienceLevel{1};
	int numberOfFightsWon{0};
	int health{100};
	// 4 stats: strength, dexterity, faith, max_health
	std::array<int, static_cast<size_t>(4)> stats{};

	StatsComponent()
	{
		stats.fill(1);
		stats[static_cast<size_t>(STATS::MAX_HEALTH)] = 100;
	}

	void readFromJson(tson::TiledClass &j) override
	{

		int maxHealth = j.get<int>("maxHealth");
		int strength = j.get<int>("strength");
		int dexterity = j.get<int>("dexterity");
		int faith = j.get<int>("faith");
		this->health = maxHealth;

		addScalableStats(STATS::MAX_HEALTH, maxHealth);
		addScalableStats(STATS::STRENGTH, strength);
		addScalableStats(STATS::DEXTERITY, dexterity);
		addScalableStats(STATS::FAITH, faith);
	}

	void addScalableStats(STATS stat, int factor) { stats[static_cast<size_t>(stat)] = factor; }

	int getStat(STATS stat) const { return stats[static_cast<size_t>(stat)]; }
};
