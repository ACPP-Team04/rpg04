#pragma once
#include "Abstract/ECS/Component/Component.hpp"
#include "Abstract/TILE_ENUMS.hpp"

struct StatsComponent : Component<StatsComponent> {
  public:
	StatsComponent() = default;
	float experience{1};
	int experienceLevel{1};
	int numberOfFightsWon{0};
	int health{100};
	std::unordered_map<STATS, int> stats;
	void readFromJson(tson::TiledClass &j) override
	{
		int maxHealth = j.get<int>("maxHealt");
		int strength = j.get<int>("strength");
		int dexterity = j.get<int>("dexterity");
		int faith = j.get<int>("faith");
		this->health = maxHealth;

		addScalableStats(STATS::MAX_HEALTH, maxHealth);
		addScalableStats(STATS::STRENGTH, strength);
		addScalableStats(STATS::DEXTERITY, dexterity);
		addScalableStats(STATS::FAITH, faith);
	}

	void addScalableStats(STATS stat, int factor) { stats[stat] = factor; }

	int getStat(STATS stat)
	{
		if (!stats.contains(stat)) {
			return 0;
		}
		return stats[stat];
	}
};
