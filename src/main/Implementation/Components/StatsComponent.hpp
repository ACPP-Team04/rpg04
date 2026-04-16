#pragma once
#include "Abstract/ECS/Component/Component.hpp"
#include "Abstract/TILE_ENUMS.hpp"
#include <spdlog/spdlog.h>
struct StatsComponent : Component<StatsComponent> {
  public:
	StatsComponent() = default;
	float experience{1};
	int experienceLevel{1};
	int numberOfFightsWon{0};
	int health{100};
	std::unordered_map<STATS, int> stats;
	void readFromJson(const nlohmann::json &j) override
	{
		int maxHealth = j.value("maxHealt", 100);
		int strength = j.value("strength", 1);
		int dexterity = j.value("dexterity", 1);
		int faith = j.value("faith", 1);
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
			if (stat == STATS::MAX_HEALTH) {
				spdlog::get("combat")->warn("Returned MAX_HEALTH 100, because it was not in the stats");
				return 100;
			}
			spdlog::get("combat")->warn("Returned 0 for {}, because it was not in the stats", (int)stat);
			return 0;
		}
		return stats[stat];
	}
};
